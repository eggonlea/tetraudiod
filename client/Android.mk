LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := tetraudio_test
LOCAL_SRC_FILES += tetraudio_test.cpp
LOCAL_C_INCLUDES := $(JNI_H_INCLUDE) $(LOCAL_PATH)/../include
LOCAL_SHARED_LIBRARIES := libtetraudio libutils libbinder liblog

include $(BUILD_EXECUTABLE)
