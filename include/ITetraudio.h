// Copyright (C) 2017 Essential Products <li@essential.com>

#ifndef ITETRAUDIO_H
#define ITETRAUDIO_H

#include <utils/RefBase.h>
#include <binder/Parcel.h>
#include <binder/IInterface.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/MemoryHeapBase.h>

#define TETRAUDIO_SERVICE_NAME  "com.essential.tetraudio"

namespace android {

class ITetraudio: public IInterface {
public:
	DECLARE_META_INTERFACE(Tetraudio);

	virtual int setSize(uint32_t count) = 0;
	virtual sp<IMemoryHeap> getSrc() = 0;
	virtual sp<IMemoryHeap> getDst() = 0;
	virtual void reset() = 0;
	virtual int process() = 0;
};

class BnTetraudio: public BnInterface<ITetraudio> {
public:
	virtual status_t onTransact(uint32_t code, const Parcel& data,
					Parcel* reply, uint32_t flags = 0);
};

};

#endif
