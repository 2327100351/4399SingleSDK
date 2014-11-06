/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>

#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <malloc.h>

#include "cn_m4399_recharge_control_strategy_sign_SignUtils.h"
#include "include/ft_native_log.h"
#include "include/ft_security.h"

/* Header for class cn_m4399_recharge_control_strategy_sign_ConsolePaySigner */

#ifdef __cplusplus
extern "C" {
#endif

int my_line_feed_flag  = 0;
int max_value_per_base64_codes_line = 76;

/*
 * Class:     cn_m4399_recharge_control_strategy_sign_ConsolePaySigner
 * Method:    nativeEncode
 * Signature: ([Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_cn_m4399_recharge_control_strategy_sign_SignUtils_nativeEncode
  (JNIEnv *env, jclass klass, jobjectArray plain) {
	jsize plain_size = (*env)->GetArrayLength(env, plain);

	unsigned char in[MSG_LEN];
	memset(in, '\0', MSG_LEN);

	unsigned i = 0;
	unsigned in_len = 0;
	for (i=0; i<plain_size; i++) {
		jstring jstr = (jstring)(*env)->GetObjectArrayElement(env, plain, i);
		const char *str_ptr = (*env)->GetStringUTFChars(env, jstr, NULL);

		if (i == 4 && strlen(str_ptr)==0) {
			/**
			 * If the has no uid,
			 * I add a '0' to validate the signature
			 */
			strcat(in, "0");
			in_len += 1;
		} else {
			strcat(in, str_ptr);
			in_len += strlen(str_ptr);
		}

		(*env)->ReleaseStringUTFChars(env, jstr, str_ptr);
	}

	LOGD("plain text: %s", in);

	unsigned char out[MSG_LEN];
	memset(out, '\0', MSG_LEN);

	unsigned out_len;
	if ((in_len%AES_BLOCK_SIZE) == 0) {
		out_len = in_len;
	} else
		out_len = ((in_len/AES_BLOCK_SIZE) + 1) * AES_BLOCK_SIZE;

	unsigned char * in_padding = (unsigned char *)calloc(out_len, sizeof(unsigned char));
	if (in_padding == NULL) {
		return (*env)->NewStringUTF(env, in);
	} else {
		//Set NoPadding or PKCS5Padding
		memset(in_padding, '\0', out_len);
		//memset(in_padding, out_len-in_len, out_len);
		memcpy(in_padding, in, in_len);
	}

	size_t size = 0;
	if (ft_aes_cbc_encrypt(in_padding, out, out_len) == AES_ENCRYPT_SUCCESS) {
		LOGD("encrypt: %s, %s\n", in, out);

		return Cstr2Jstring(env, (char *)out);
	}

	if (in_padding != NULL) {
		free(in_padding);
		in_padding = NULL;
	}

	return (*env)->NewStringUTF(env, in);
}

/*
 * Class:     cn_m4399_recharge_control_strategy_sign_ConsolePaySigner
 * Method:    nativeDecode
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_cn_m4399_recharge_control_strategy_sign_SignUtils_nativeDecode
  (JNIEnv *env, jclass klass, jstring cipher) {
	const char *in = (*env)->GetStringUTFChars(env, cipher, NULL);
	unsigned char out[MSG_LEN];
	memset(out, '\0', MSG_LEN);

	if (ft_aes_cbc_decrypt(in, out, strlen(in)) == AES_DECRYPT_SUCCESS) {
		LOGD("decrypt: %s, %s", in, out);
		(*env)->ReleaseStringUTFChars(env, cipher, in);
		return Cstr2Jstring(env, (char *)out);
	}

	(*env)->ReleaseStringUTFChars(env, cipher, in);
	return cipher;
}

/*
 * Class:     cn_m4399_recharge_control_strategy_sign_ConsolePaySigner
 * Method:    nativeGetMark
 * Signature: ([Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_cn_m4399_recharge_control_strategy_sign_SignUtils_nativeGetMark
  (JNIEnv *env, jclass klass, jobjectArray info) {
	jsize len = (*env)->GetArrayLength(env, info);
	unsigned char in[MSG_LEN];
	memset(in, '\0', MSG_LEN);

	unsigned i = 0;
	for (i=0; i<len; i++) {
		jstring jstr = (jstring)(*env)->GetObjectArrayElement(env, info, i);
		const char *str_ptr = (*env)->GetStringUTFChars(env, jstr, NULL);
		strcat(in, str_ptr);
		(*env)->ReleaseStringUTFChars(env, jstr, str_ptr);
	}

	char buffer[32];
	memset(buffer, '\0', 32);

	itoa(get_current_time(), buffer, 16);
	strcat(in, buffer);

	memset(buffer, '\0', 32);
	ft_md5(env, in, buffer);

	char md5[32];
	memset(md5, '\0', 32);
	for (i=0; i<16; i++) {
		md5[i] = buffer[i+7];
	}

	return Cstr2Jstring(env, (char *)md5);
}

/*
 * Class:     cn_m4399_recharge_control_strategy_sign_ConsolePaySigner
 * Method:    nativeValidate
 * Signature: ([Ljava/lang/String;Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_cn_m4399_recharge_control_strategy_sign_SignUtils_nativeValidate
  (JNIEnv * env, jclass klass, jobjectArray info, jstring sign) {
	jboolean retVal;

	jstring cipher = Java_cn_m4399_recharge_control_strategy_sign_SignUtils_nativeEncode(env, klass, info);
	const char * cipher_ptr = (*env)->GetStringUTFChars(env, cipher, NULL);
	const char * sample_ptr = (*env)->GetStringUTFChars(env, sign, NULL);

	LOGD("%s: %s", cipher_ptr, sample_ptr);
	if (cipher_ptr!=NULL && sample_ptr!=NULL && strcmp(cipher_ptr, sample_ptr) == 0) {
		retVal = JNI_TRUE;
	} else
		retVal = JNI_FALSE;

	(*env)->ReleaseStringUTFChars(env, cipher, cipher_ptr);
	(*env)->ReleaseStringUTFChars(env, sign, sample_ptr);

	return retVal;
}

#ifdef __cplusplus
}
#endif