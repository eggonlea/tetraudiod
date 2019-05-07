// Copyright (C) 2017 Essential Products <li@essential.com>

#include <stdint.h>
#include <sys/types.h>
#include <binder/Parcel.h>
#include <utils/Errors.h>

#include "ITetraudio.h"

namespace android {

enum {
	SET_SIZE = IBinder::FIRST_CALL_TRANSACTION,
	GET_SRC,
	GET_DST,
	RESET,
	PROCESS
};

class BpTetraudio: public BpInterface<ITetraudio> {
public:
	BpTetraudio(const sp<IBinder>& impl) : BpInterface<ITetraudio>(impl) {}

	int setSize(uint32_t count) {
		Parcel data, reply;
		data.writeInterfaceToken(ITetraudio::getInterfaceDescriptor());
		data.writeUint32(count);
		return remote()->transact(SET_SIZE, data, &reply, 0);
	}

	sp<IMemoryHeap> getSrc() {
		Parcel data, reply;
		sp<IMemoryHeap> memHeap = NULL;
		data.writeInterfaceToken(ITetraudio::getInterfaceDescriptor());
		if (remote()->transact(GET_SRC, data, &reply, 0) == NO_ERROR) {
			memHeap = interface_cast<IMemoryHeap> (reply.readStrongBinder());
			return memHeap;
		}
		return NULL;
	}

	sp<IMemoryHeap> getDst() {
		Parcel data, reply;
		sp<IMemoryHeap> memHeap = NULL;
		data.writeInterfaceToken(ITetraudio::getInterfaceDescriptor());
		if (remote()->transact(GET_DST, data, &reply, 0) == NO_ERROR) {
			memHeap = interface_cast<IMemoryHeap> (reply.readStrongBinder());
			return memHeap;
		}
		return NULL;
	}

	void reset() {
		Parcel data, reply;
		data.writeInterfaceToken(ITetraudio::getInterfaceDescriptor());
		remote()->transact(RESET, data, &reply, 0);
	}

	int process() {
		Parcel data, reply;
		data.writeInterfaceToken(ITetraudio::getInterfaceDescriptor());
		return remote()->transact(PROCESS, data, &reply, 0);
	}
};

IMPLEMENT_META_INTERFACE(Tetraudio, TETRAUDIO_SERVICE_NAME);

status_t BnTetraudio::onTransact(uint32_t code, const Parcel& data,
				Parcel* reply, uint32_t flags) {
	switch(code) {
	case SET_SIZE: {
		CHECK_INTERFACE(ITetraudio, data, reply);
		uint32_t size;
		data.readUint32(&size);
		if (setSize(size) < 0)
			return BAD_VALUE;
		return NO_ERROR;
	}
	case GET_SRC: {
		CHECK_INTERFACE(ITetraudio, data, reply);
		sp<IMemoryHeap> src = getSrc();
		if (src == NULL)
			return BAD_VALUE;
		reply->writeStrongBinder(IInterface::asBinder(src));
		return NO_ERROR;
	}
	case GET_DST: {
		CHECK_INTERFACE(ITetraudio, data, reply);
		sp<IMemoryHeap> dst = getDst();
		if (dst == NULL)
			return BAD_VALUE;
		reply->writeStrongBinder(IInterface::asBinder(dst));
		return NO_ERROR;
	}
	case RESET: {
		CHECK_INTERFACE(ITetraudio, data, reply);
		reset();
		return NO_ERROR;
	}
	case PROCESS: {
		CHECK_INTERFACE(ITetraudio, data, reply);
		process();
		return NO_ERROR;
	}
	default:
		return BBinder::onTransact(code, data, reply, flags);
	}
}

};
