LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := tetraudiod
LOCAL_SRC_FILES += tetraudiod.cpp
LOCAL_C_INCLUDES := $(JNI_H_INCLUDE) $(LOCAL_PATH)/../include
LOCAL_SHARED_LIBRARIES := libtetraudio libutils libbinder liblog libtetraproc libfftw3f

include $(BUILD_EXECUTABLE)
