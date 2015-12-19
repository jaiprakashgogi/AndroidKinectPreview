LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := androidkinect
LOCAL_SRC_FILES := androidkinect.cpp

include $(BUILD_SHARED_LIBRARY)