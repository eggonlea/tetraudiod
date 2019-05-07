// Copyright (C) 2017 Essential Products <li@essential.com>

#define LOG_TAG	"TetraudioService"

#include <stdint.h>
#include <sys/types.h>
#include <utils/Errors.h>
#include <utils/Log.h>
#include <binder/BinderService.h>

#include "ITetraudio.h"
#include "abprocess.h"

#define SAMPLE_RATE	48000
#define CHANNELS	4
#define SAMPLE_NUMBER	(SAMPLE_RATE / 50)

namespace android {

class TetraudioService :
	public BinderService<TetraudioService>,
	public BnTetraudio {
public:
	TetraudioService ();
	virtual ~TetraudioService();

	static char const *getServiceName() { return TETRAUDIO_SERVICE_NAME; }

	int setSize(uint32_t count);
	sp<IMemoryHeap> getSrc();
	sp<IMemoryHeap> getDst();
	void reset();
	int process();

private:
	uint32_t mSize;
	sp<MemoryHeapBase> mSrc;
	sp<MemoryHeapBase> mDst;
	ABconfig mConfig;
	ABprocess mProcess;
};

TetraudioService::TetraudioService()
{
	// init ABconfig & ABprocess
	mConfig.reset();
	mProcess.init(SAMPLE_RATE, mSize, mSize);
	mProcess.set_lffilt(&mConfig);
	mProcess.set_matrix(&mConfig);
	mProcess.set_convol(&mConfig);
	mProcess.set_hffilt(&mConfig);
	mProcess.set_hpfil(1);
	mProcess.set_invb(0);
	mProcess.set_endf(0);
	mProcess.set_form(Bformat::FM_SN3D);

	// set default size
	setSize(SAMPLE_NUMBER);
}

TetraudioService::~TetraudioService()
{
	setSize(0);
}

void TetraudioService::reset()
{
	mProcess.reset();
}

int TetraudioService::setSize(uint32_t size)
{
	if (size <= mSize) {
		// nothing to do
		ALOGI("Ignored. old size = %d, new size = %d", mSize, size);
		return 0;
	}

	// free old buf
	if (mSize > 0)
		setSize(0);

	if (size > 0) {
		// allocate new buf
		mSrc = new MemoryHeapBase(size * CHANNELS * (32 / 8));
		if (mSrc == NULL) {
			ALOGE("Failed to allocate src");
			return -1;
		}
		ALOGI("Allocated src (%zu)", mSrc->getSize());

		mDst = new MemoryHeapBase(size * CHANNELS * (32 / 8));
		if (mDst == NULL) {
			ALOGE("Failed to allocate dst");
			mSrc = NULL;
			return -1;
		}
		ALOGI("Allocated dst (%zu)", mDst->getSize());

		mSize = size;
		ALOGI("New size = %d (%dB)", mSize, mSize * CHANNELS * (32 / 8));
	} else if (mSize > 0) {
		// free old buf
		mSrc = NULL;
		mDst = NULL;
		mSize = 0;
		ALOGI("New size = 0");
	} else {
		// nothing to do
		ALOGI("Ignored. old size = %d, new size = %d", mSize, size);
	}

	return 0;
}

sp<IMemoryHeap> TetraudioService::getSrc()
{
	ALOGD("%s()", __func__);
	return mSrc;
}

sp<IMemoryHeap> TetraudioService::getDst()
{
	ALOGD("%s()", __func__);
	return mDst;
}

int TetraudioService::process()
{
	ALOGD("%s()", __func__);

	if (mSize == 0 || mSrc == NULL || mDst == NULL) {
		ALOGW("Invalid process request: buf not initialized");
		return -1;
	}

#ifdef DUMMY_TETRAUDIO
	float *src = (float *)(mSrc->getBase());
	float *dst = (float *)(mDst->getBase());
	// TODO: add B-format conversion
	for(uint32_t i = 0; i < mSize * CHANNELS; i++)
		dst[i] = src[i];
#else
	float *fsrc[4];
	fsrc[0] = (float *)(mSrc->getBase());
	fsrc[1] = fsrc[0] + mSize;
	fsrc[2] = fsrc[1] + mSize;
	fsrc[3] = fsrc[2] + mSize;

	float *fdst[4];
	fdst[0] = (float *)(mDst->getBase());
	fdst[1] = fdst[0] + mSize;
	fdst[2] = fdst[1] + mSize;
	fdst[3] = fdst[2] + mSize;
	mProcess.process(mSize, fsrc, fdst);
#endif

	return 0;
}

};

using namespace android;

int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	TetraudioService::publishAndJoinThreadPool();
	return 0;
}
