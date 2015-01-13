LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := Native

ALL_SOURCES := $(wildcard $(LOCAL_PATH)/*.h) 
LOCAL_C_INCLUDES := $(ALL_SOURCES:$(LOCAL_PATH)/%=%)
ALL_SOURCES := $(wildcard $(LOCAL_PATH)/*.cpp) 
LOCAL_SRC_FILES := $(ALL_SOURCES:$(LOCAL_PATH)/%=%)
ALL_SOURCES := $(wildcard $(LOCAL_PATH)/*.c) 
LOCAL_SRC_FILES += $(ALL_SOURCES:$(LOCAL_PATH)/%=%)

LOCAL_CFLAGS += -Werror -D__android__
LOCAL_CPPFLAGS += -fno-rtti
LOCAL_LDLIBS := -llog -lGLESv2 -lm -lEGL

include $(BUILD_SHARED_LIBRARY)

