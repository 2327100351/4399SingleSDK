#include "../include/ft_security.h"

const unsigned char FT_AES_KEY[FT_AES_KEY_SIZE] = { 'a' };
unsigned char * CBC_KEY = "p09|asdf_asf112t";
unsigned char * CBC_IVEC = "asdf123|afa_sf+1";

char* Jstring2Cstr(JNIEnv* env, jstring jstr) {
	char* rtn = NULL;

	jclass clsstring = (*env)->FindClass(env, "java/lang/String");
	jstring strencode = (*env)->NewStringUTF(env, "utf-8");
	jmethodID mid = (*env)->GetMethodID(env, clsstring, "getBytes",
			"(Ljava/lang/String;)[B");
	jbyteArray barr = (jbyteArray)(*env)->CallObjectMethod(env, jstr, mid,
			strencode);
	jsize alen = (*env)->GetArrayLength(env, barr);
	jbyte* ba = (*env)->GetByteArrayElements(env, barr, JNI_FALSE);
	if (alen > 0) {
		rtn = (char*) malloc(alen + 1);
		memcpy(rtn, ba, alen);
		rtn[alen] = 0;
	}
	(*env)->ReleaseByteArrayElements(env, barr, ba, 0);

	return rtn;
}

jstring Cstr2Jstring(JNIEnv* env, const char* pStr) {
	int strLen = strlen(pStr);
	jclass jstrObj = (*env)->FindClass(env, "java/lang/String");
	jmethodID methodId = (*env)->GetMethodID(env, jstrObj, "<init>",
			"([BLjava/lang/String;)V");
	jbyteArray byteArray = (*env)->NewByteArray(env, strLen);
	jstring encode = (*env)->NewStringUTF(env, "utf-8");

	(*env)->SetByteArrayRegion(env, byteArray, 0, strLen, (jbyte*) pStr);

	return (jstring)(*env)->NewObject(env, jstrObj, methodId, byteArray, encode);
}

int base64_encode(const unsigned char *in, unsigned char *out, int in_len) {
	int out_len = 0;
	//static unsigned char base64_encode[] =
	//		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/\\0";
	static const char base64_encode[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G',
			'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
			'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
			'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
			'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6',
			'7', '8', '9', '+', '/', '\0'};

	while (in_len > 0) {
		*out++ = base64_encode[(in[0] >> 2) & 0x3f];
		if (in_len > 2) {
			*out++ = base64_encode[((in[0] & 3) << 4) | (in[1] >> 4)];
			*out++ = base64_encode[((in[1] & 0xF) << 2) | (in[2] >> 6)];
			*out++ = base64_encode[in[2] & 0x3F];
		} else {
			switch (in_len) {
			case 1:
				*out++ = base64_encode[(in[0] & 3) << 4];
				*out++ = '=';
				*out++ = '=';
				break;
			case 2:
				*out++ = base64_encode[((in[0] & 3) << 4) | (in[1] >> 4)];
				*out++ = base64_encode[((in[1] & 0x0F) << 2) | (in[2] >> 6)];
				*out++ = '=';
				break;
			}
		}

		in += 3;
		in_len -= 3;
		out_len += 4;
	}

	*out = '\0';

	return out_len;
}

char _get_base64_value(unsigned char ch) {
	if ((ch >= 'A') && (ch <= 'Z')) // A ~ Z
		return ch - 'A';
	if ((ch >= 'a') && (ch <= 'z')) // a ~ z
		return ch - 'a' + 26;
	if ((ch >= '0') && (ch <= '9')) // 0 ~ 9
		return ch - '0' + 52;
	switch (ch) {
	case '+':
		return 62;
	case '/':
		return 63;
	case '=': //Base64 填充字符
		return 32;
	default:
		return 32;
	}
}

int base64_decode(unsigned char *in, unsigned char *out, int out_len,
		int forced) {
	if (out_len % 4 && !forced) { //如果不是 4 的倍数,则 Base64 编码有问题
		in[0] = '\0';
		return -1;
	}
	unsigned char base64_encode[4];
	int in_len = 0;

	while (out_len > 2) { //当待解码个数不足3个时,将忽略它(强制解码时有效)
		memset(base64_encode, '\0', 4);
		base64_encode[0] = _get_base64_value(out[0]);
		base64_encode[1] = _get_base64_value(out[1]);
		base64_encode[2] = _get_base64_value(out[2]);
		base64_encode[3] = _get_base64_value(out[3]);

		*in++ = (base64_encode[0] << 2) | (base64_encode[1] >> 4);
		*in++ = (base64_encode[1] << 4) | (base64_encode[2] >> 2);
		*in++ = (base64_encode[2] << 6) | (base64_encode[3]);

		out += 4;
		out_len -= 4;
		in_len += 3;
	}

	return in_len;
}

void ft_md5(JNIEnv* env, char * jstr, char * out) {
	if (jstr == NULL)
		return;

	MD5_CTX context = { 0 };
	MD5Init(&context);
	MD5Update(&context, jstr, strlen(jstr));
	unsigned char dest[16] = { 0 };
	MD5Final(dest, &context);

	int i;
	for (i = 0; i < 16; i++) {
		sprintf(out, "%s%02x", out, dest[i]);
	}
}

int ft_aes_encrypt(const char* in, char* out) //, int olen)
{
	if (in == NULL || out == NULL)
		return AES_ENCRYPT_FAIL;

	AES_KEY aes;
	if (AES_set_encrypt_key(FT_AES_KEY, 128, &aes) < 0) {
		return AES_ENCRYPT_FAIL;
	}

	int len = strlen(in), en_len = 0;
//输入输出字符串够长，并且是AES_BLOCK_SIZE的整数倍，需要严格限制
	while (en_len < len) {
		AES_encrypt((unsigned char*) in, (unsigned char*) out, &aes);
		in += AES_BLOCK_SIZE;
		out += AES_BLOCK_SIZE;
		en_len += AES_BLOCK_SIZE;
	}

	return AES_ENCRYPT_SUCCESS;
}

int ft_aes_decrypt(const char* in, char* out) {
	if (in == NULL || out == NULL)
		return AES_ENCRYPT_FAIL;

	AES_KEY aes;
	if (AES_set_encrypt_key(FT_AES_KEY, 128, &aes) < 0) {
		return AES_ENCRYPT_FAIL;
	}

	int len = strlen(in), en_len = 0;
	while (en_len < len) {
		AES_decrypt((unsigned char*) in, (unsigned char*) out, &aes);
		in += AES_BLOCK_SIZE;
		out += AES_BLOCK_SIZE;
		en_len += AES_BLOCK_SIZE;
	}

	return AES_ENCRYPT_SUCCESS;
}

int _aes_cbc_encrypt(const char* in, char* out, size_t len) {
	if (in == NULL || out == NULL)
		return AES_ENCRYPT_FAIL;

	AES_KEY aes_key;
	unsigned char * key = (char *) calloc(AES_BLOCK_SIZE, sizeof(char));
	if (key == NULL)
		return AES_ENCRYPT_FAIL;
	else {
		memset(key, '\0', AES_BLOCK_SIZE);
		memcpy(key, CBC_KEY, AES_BLOCK_SIZE);
	}

	unsigned char * cbc_ivec = (char *) calloc(AES_BLOCK_SIZE, sizeof(char));
	if (cbc_ivec == NULL)
		return AES_ENCRYPT_FAIL;
	else {
		memset(cbc_ivec, '\0', AES_BLOCK_SIZE);
		memcpy(cbc_ivec, CBC_IVEC, AES_BLOCK_SIZE);
	}

	if (AES_set_encrypt_key(key, 128, &aes_key) < 0) {
		return AES_ENCRYPT_FAIL;
	}

	AES_cbc_encrypt((unsigned char *) in, (unsigned char *) out, len, &aes_key,
			cbc_ivec, AES_ENCRYPT);

	if (key != NULL)
		free(key);
	if (cbc_ivec != NULL)
		free(cbc_ivec);

	return AES_ENCRYPT_SUCCESS;
}

int _aes_cbc_decrypt(const char* in, char* out, size_t len) {
	if (in == NULL || out == NULL)
		return AES_DECRYPT_FAIL;

	AES_KEY aes_key;
	unsigned char * key = (char *) calloc(AES_BLOCK_SIZE, sizeof(char));
	if (key == NULL)
		return AES_DECRYPT_FAIL;
	else {
		memset(key, '\0', AES_BLOCK_SIZE);
		memcpy(key, CBC_KEY, AES_BLOCK_SIZE);
	}

	unsigned char * cbc_ivec = (char *) calloc(AES_BLOCK_SIZE, sizeof(char));
	if (cbc_ivec == NULL)
		return AES_DECRYPT_FAIL;
	else {
		memset(cbc_ivec, '\0', AES_BLOCK_SIZE);
		memcpy(cbc_ivec, CBC_IVEC, AES_BLOCK_SIZE);
	}

	if (AES_set_decrypt_key(key, 128, &aes_key) < 0) {
		return AES_DECRYPT_FAIL;
	}

	AES_cbc_encrypt((unsigned char *) in, (unsigned char *) out, len, &aes_key,
			cbc_ivec, AES_DECRYPT);

	if (key != NULL)
		free(key);
	if (cbc_ivec != NULL)
		free(cbc_ivec);

	return AES_DECRYPT_SUCCESS;
}

static const char base64_table[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
		'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
		'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
		'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
		'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/',
		'\0' };

int ft_aes_cbc_encrypt(const char* in, char* out, size_t len) {
	if (_aes_cbc_encrypt(in, out, len) == AES_ENCRYPT_SUCCESS) {

		//unsigned char *php_base64_encode(const unsigned char *str, int length, int *ret_length);
		/*int ret_length = 0;
		unsigned char * base64_out = php_base64_encode((const unsigned char *)out, len, &ret_length);
		memcpy(out, base);*/

		unsigned char buffer[MSG_LEN];
		memset(buffer, '\0', MSG_LEN);

		int base64_buffer_len = base64_encode((char *)out, (char *)buffer, len);
		if (base64_buffer_len < 0)
			return AES_ENCRYPT_FAIL;

		strncpy(out, buffer, strlen(buffer)+1);
	}
	return AES_ENCRYPT_SUCCESS;
}

int ft_aes_cbc_decrypt(const char* in, char* out, size_t len) {
	unsigned char buffer[MSG_LEN];
	memset(buffer, '\0', MSG_LEN);

	//unsigned char *php_base64_decode(const unsigned char *str, int length, int *ret_length);
	//int ret_length;
	//char * buffer_ptr = php_base64_decode((const unsigned char *)in, len, &ret_length);
	//memcpy(buffer, buffer_ptr, ret_length);
	base64_decode(buffer, (char *) in, len, 0);

	if (_aes_cbc_decrypt(buffer, out, strlen(buffer)) == AES_DECRYPT_SUCCESS) {
		return AES_DECRYPT_SUCCESS;
	}

	return AES_DECRYPT_SUCCESS;
}


long get_current_time() {
   struct timeval tv;
   gettimeofday(&tv,NULL);
   return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

void itoa(unsigned long val, char *ret, unsigned radix) {
	char *p;
	char *firstdig;
	char temp;
	unsigned digval;
	p = ret;
	if (val < 0) {
		*p++ = '-';
		val = (unsigned long) (-(long) val);
	}
	firstdig = p;
	do {
		digval = (unsigned) (val % radix);
		val /= radix;

		if (digval > 9)
			*p++ = (char) (digval - 10 + 'a');
		else
			*p++ = (char) (digval + '0');
	} while (val > 0);

	*p-- = '\0';
	do {
		temp = *p;
		*p = *firstdig;
		*firstdig = temp;
		--p;
		++firstdig;
	} while (firstdig < p);
}
