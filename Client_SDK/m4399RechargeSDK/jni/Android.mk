LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES	:=	cn_m4399_recharge_control_strategy_sign_SignUtils.c	\
					src/aes/aes_cbc.c	\
					src/aes/aes_cfb.c	\
					src/aes/aes_core.c	\
					src/aes/aes_ctr.c	\
					src/aes/aes_ecb.c	\
					src/aes/aes_misc.c	\
					src/aes/aes_ofb.c	\
					src/modes/cbc128.c	\
					src/modes/cfb128.c	\
					src/modes/ctr128.c	\
					src/modes/ofb128.c	\
					src/md5/md5.c		\
					src/base64/base64.c	\
					src/ft_security.c
					

LOCAL_MODULE	:=	jni_ftsecurity
LOCAL_C_INCLUDES += $(JNI_H_INCLUDE)
LOCAL_SHARED_LIBRARIES	:=	libcutils libutils
LOCAL_LDLIBS    := -lm -llog 

include $(BUILD_SHARED_LIBRARY)



