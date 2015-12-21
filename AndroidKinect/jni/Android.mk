TOP_PATH := $(call my-dir)

include $(CLEAR_VARS)
include $(TOP_PATH)/libusb-1.0.18/android/jni/libusb.mk
include $(TOP_PATH)/libfreenect/android/jni/libfreenect.mk

include $(CLEAR_VARS)
LOCAL_PATH := $(TOP_PATH)
include /home/jprakash/jai/android/OpenCV-android-sdk/sdk/native/jni/OpenCV.mk

LOCAL_SHARED_LIBRARIES += libusb1.0 libfreenect
LOCAL_LDLIBS     += -llog -ldl

LOCAL_MODULE    := androidkinect
LOCAL_SRC_FILES := androidkinect.cpp

include $(BUILD_SHARED_LIBRARY)
