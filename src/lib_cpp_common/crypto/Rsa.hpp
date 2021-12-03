#pragma once
#include <string>
#include "openssl/rsa.h"
#include "openssl/pem.h"

namespace libcc
{
	namespace crypto
	{
		using std::string;
		class Rsa
		{
		public:
			enum PaddingMode
			{
				PKCS1 = RSA_PKCS1_PADDING,
				SSLV23 = RSA_SSLV23_PADDING,
				NoPadding = RSA_NO_PADDING,
				PKCS1_OAEP = RSA_PKCS1_OAEP_PADDING,
				X931 = RSA_X931_PADDING,
				/* EVP_PKEY_ only */
				PKCS1_PSS = RSA_PKCS1_PSS_PADDING


			};
		public:
			// 私钥加密
			static string privateEncrypt(const string& clearText, const string& pubKey, PaddingMode paddingMode = PKCS1_OAEP)
			{
				string strRet;
				BIO* keybio = BIO_new_mem_buf((unsigned char*)pubKey.c_str(), -1);

				RSA* rsa = RSA_new();
				rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
				if (!rsa)
				{
					BIO_free_all(keybio);
					return string("");
				}

				int len = RSA_size(rsa);
				char* encryptedText = (char*)malloc(len + 1);
				memset(encryptedText, 0, len + 1);

				// 加密
				int ret = RSA_private_encrypt(clearText.length(), (const unsigned char*)clearText.c_str(), (unsigned char*)encryptedText, rsa, paddingMode);
				if (ret >= 0)
					strRet = string(encryptedText, ret);

				// 释放内存
				free(encryptedText);
				BIO_free_all(keybio);
				RSA_free(rsa);

				return strRet;
			}

			// 公钥解密
			static string publicDecrypt(const string& clearText, const string& pubKey, PaddingMode paddingMode = PKCS1_OAEP)
			{
				string strRet;
				BIO* keybio = BIO_new_mem_buf((unsigned char*)pubKey.c_str(), -1);
				RSA* rsa = RSA_new();

				//rsa = PEM_read_bio_RSAPublicKey(keybio, &rsa, NULL, NULL);//这个失败，二脸萌壁...
				rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
				if (!rsa)
				{

					BIO_free_all(keybio);
					return string("");
				}

				int len = RSA_size(rsa);
				char* encryptedText = (char*)malloc(len + 1);
				memset(encryptedText, 0, len + 1);

				//解密
				int ret = RSA_public_decrypt(clearText.length(), (const unsigned char*)clearText.c_str(), (unsigned char*)encryptedText, rsa, paddingMode);

				if (ret >= 0)
					strRet = string(encryptedText, ret);

				// 释放内存
				free(encryptedText);
				BIO_free_all(keybio);
				RSA_free(rsa);

				return strRet;
			}

			// 公钥加密
			static string publicEncrypt(const string& clearText, const string& pubKey, PaddingMode paddingMode = PKCS1_OAEP)
			{
				string strRet;
				BIO* keybio = BIO_new_mem_buf((unsigned char*)pubKey.c_str(), -1);
				RSA* rsa = RSA_new();


				//rsa = PEM_read_bio_RSAPublicKey(keybio, &rsa, NULL, NULL);//这个失败，二脸萌壁..
				rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
				if (!rsa)
				{
					BIO_free_all(keybio);
					return string("");
				}

				int len = RSA_size(rsa);
				char* encryptedText = (char*)malloc(len + 1);
				memset(encryptedText, 0, len + 1);

				// 加密函数
				int ret = RSA_public_encrypt(clearText.length(), (const unsigned char*)clearText.c_str(), (unsigned char*)encryptedText, rsa, paddingMode);
				if (ret >= 0)
					strRet = string(encryptedText, ret);

				// 释放内存
				free(encryptedText);
				BIO_free_all(keybio);
				RSA_free(rsa);

				return strRet;
			}

			// 私钥解密
			static string privateDecrypt(const string& cipherText, const string& priKey, PaddingMode paddingMode = PKCS1_OAEP)
			{
				string strRet;
				RSA* rsa = RSA_new();
				BIO* keybio;
				keybio = BIO_new_mem_buf((unsigned char*)priKey.c_str(), -1);

				rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);

				int len = RSA_size(rsa);
				char* decryptedText = (char*)malloc(len + 1);
				memset(decryptedText, 0, len + 1);

				// 解密函数
				int ret = RSA_private_decrypt(cipherText.length(), (const unsigned char*)cipherText.c_str(), (unsigned char*)decryptedText, rsa, paddingMode);
				if (ret >= 0)
					strRet = string(decryptedText, ret);

				// 释放内存
				free(decryptedText);
				BIO_free_all(keybio);
				RSA_free(rsa);
				return strRet;
			}

		};
	}
}