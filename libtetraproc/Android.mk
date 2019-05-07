LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CPP_EXTENSION := .cc
LOCAL_MODULE := libtetraproc
LOCAL_SRC_FILES := \
	abconfig.cc \
	abprocess.cc \
	hpfilt.cc \
	oscform.cc \
	pmfilt.cc

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../include
LOCAL_SHARED_LIBRARIES := libfftw3f
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../include
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/../include
LOCAL_EXPORT_LDLIBS := -ltetraproc

include $(BUILD_SHARED_LIBRARY)
