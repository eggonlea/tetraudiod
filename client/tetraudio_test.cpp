// Copyright (C) 2017 Essential Products <li@essential.com>

#define LOG_TAG "TetraudioTest"

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <grp.h>

#include "ITetraudio.h"

#define SAMPLE_RATE	48000
#define CHANNELS	4
#define SAMPLE_NUMBER	(SAMPLE_RATE / 50)

using namespace android;

int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;

	sp<IServiceManager> sm = defaultServiceManager();
	sp<IBinder> binder;
	sp<ITetraudio> tetraudio;

	while (true) {
		binder = sm->getService(String16(TETRAUDIO_SERVICE_NAME));
		if (binder != 0)
			break;
		printf("Waiting for tetraudiod...");
		sleep(1);
	}

	printf("Found tetraudiod\n");
	tetraudio = interface_cast<ITetraudio>(binder);

	int ret = tetraudio->setSize(SAMPLE_NUMBER);
	if (ret) {
		printf("Failed to set sample size = %d\n", SAMPLE_NUMBER);
		return 1;
	}
	printf("Set size = %u (%uB)\n", SAMPLE_NUMBER, SAMPLE_NUMBER * CHANNELS * (32 / 8));

	sp<IMemoryHeap> src = tetraudio->getSrc();
	if (src == NULL) {
		printf("Failed to get src buf\n");
		return 1;
	} else if (src->getSize() < SAMPLE_NUMBER * CHANNELS) {
		printf("Got wrong src size: %zu\n", src->getSize());
		return 1;
	}
	printf("Get src %p[%zu]\n", src->getBase(), src->getSize());

	sp<IMemoryHeap> dst = tetraudio->getDst();
	if (dst == NULL) {
		printf("Failed to get dst buf\n");
		return 1;
	} else if (dst->getSize() < SAMPLE_NUMBER * CHANNELS) {
		printf("Got wrong dst size: %zu\n", dst->getSize());
		return 1;
	}
	printf("Get dst %p[%zu]\n", dst->getBase(), dst->getSize());

	float *psrc = (float *)(src->getBase());
	for (int i = 0; i < SAMPLE_NUMBER * CHANNELS; i++)
		*(psrc+i) = 1.0 * i / (SAMPLE_NUMBER * CHANNELS);

	if (tetraudio->process() != 0) {
		printf("Failed to process buffer\n");
		return 1;
	}

	float *pdst = (float *)(dst->getBase());
	for (int i = 0; i < SAMPLE_NUMBER * CHANNELS; i++) {
		float f = *(pdst+i);
		if (f < -1.0 || f > 1.0)
			printf("Out of range error: [%d]=%f\n", i, f);
	}

	printf("Finished");

	return 0;
}
