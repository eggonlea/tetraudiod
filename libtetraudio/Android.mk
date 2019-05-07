LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libtetraudio
LOCAL_SRC_FILES := ITetraudio.cpp
LOCAL_C_INCLUDES := $(JNI_H_INCLUDE) $(LOCAL_PATH)/../include
LOCAL_SHARED_LIBRARIES := libutils libbinder
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../include
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/../include
LOCAL_EXPORT_LDLIBS := -ltetraudio

include $(BUILD_SHARED_LIBRARY)
