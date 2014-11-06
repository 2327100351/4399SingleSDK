#ifndef FT_SECURITY_H_
#define FT_SECURITY_H_

#include <stdio.h>
#include <string.h>
#include <jni.h>
#include <sys/time.h>
#include<stdlib.h>

#include "aes/aes.h"
#include "md5/md5.h"
#include "include/ft_native_log.h"
#include "base64/base64.h"

#define MD5_SUCCESS 0
#define MD5_FAIL 1
#define AES_ENCRYPT_SUCCESS 0
#define AES_ENCRYPT_FAIL 1
#define AES_DECRYPT_SUCCESS 0
#define AES_DECRYPT_FAIL 1

#define AES_BITS 128
#define MSG_LEN 256

#define FT_AES_KEY_SIZE 32
#define FT_AES_IVEC_SIZE 16

char* Jstring2Cstr(JNIEnv* env, jstring jstr);
jstring Cstr2Jstring(JNIEnv* env, const char* pStr);

void ft_md5(JNIEnv* env, char * jstr, char * out);

int base64_encode(const unsigned char *in, unsigned char *out, int in_len);
int base64_decode(unsigned char *in, unsigned char *out, int out_len, int forced);

int ft_aes_encrypt(const char* in, char* out);
int ft_aes_decrypt(const char* in, char* out);

int ft_aes_cbc_encrypt(const char* in, char* out, size_t len);
int ft_aes_cbc_decrypt(const char* in, char* out, size_t len);

long get_current_time();
void itoa(unsigned long val, char *ret, unsigned radix);

#endif
