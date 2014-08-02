LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := cryptopg
LOCAL_SRC_FILES := jcrypto.c aes_t.c bs_t.c jutil.c rc4_t.c iodev_t.c util.c ffmt_t.c
LOCAL_LDLIBS    := -lm -llog

include $(BUILD_SHARED_LIBRARY)
