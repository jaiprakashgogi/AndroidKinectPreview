LOCAL_PATH:= $(call my-dir)
LIBFREENECT_ROOT_REL:= ../..
LIBFREENECT_ROOT_ABS:= $(LOCAL_PATH)/../..

# libfreenect

include $(CLEAR_VARS)

LIBFREENECT_ROOT_REL:= ../..
LIBFREENECT_ROOT_ABS:= $(LOCAL_PATH)/../..

LOCAL_SRC_FILES := \
	$(LIBFREENECT_ROOT_REL)/src/core.c \
	$(LIBFREENECT_ROOT_REL)/src/cameras.c \
	$(LIBFREENECT_ROOT_REL)/src/tilt.c \
	$(LIBFREENECT_ROOT_REL)/src/usb_libusb10.c \
	$(LIBFREENECT_ROOT_REL)/src/audio.c \
	$(LIBFREENECT_ROOT_REL)/src/registration.c \
	$(LIBFREENECT_ROOT_REL)/src/loader.c \
	$(LIBFREENECT_ROOT_REL)/src/flags.c

LOCAL_C_INCLUDES += \
  $(LIBFREENECT_ROOT_ABS)/wrappers/cpp \
  $(LIBFREENECT_ROOT_ABS)/include

LOCAL_EXPORT_C_INCLUDES := \
  $(LIBFREENECT_ROOT_ABS)/wrappers/cpp \
  $(LIBFREENECT_ROOT_ABS)/include

LOCAL_SHARED_LIBRARIES += libusb1.0
LOCAL_LDLIBS := -llog

LOCAL_MODULE := libfreenect

include $(BUILD_SHARED_LIBRARY)
