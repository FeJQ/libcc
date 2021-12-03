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
			// ˽Կ����
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

				// ����
				int ret = RSA_private_encrypt(clearText.length(), (const unsigned char*)clearText.c_str(), (unsigned char*)encryptedText, rsa, paddingMode);
				if (ret >= 0)
					strRet = string(encryptedText, ret);

				// �ͷ��ڴ�
				free(encryptedText);
				BIO_free_all(keybio);
				RSA_free(rsa);

				return strRet;
			}

			// ��Կ����
			static string publicDecrypt(const string& clearText, const string& pubKey, PaddingMode paddingMode = PKCS1_OAEP)
			{
				string strRet;
				BIO* keybio = BIO_new_mem_buf((unsigned char*)pubKey.c_str(), -1);
				RSA* rsa = RSA_new();

				//rsa = PEM_read_bio_RSAPublicKey(keybio, &rsa, NULL, NULL);//���ʧ�ܣ������ȱ�...
				rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
				if (!rsa)
				{

					BIO_free_all(keybio);
					return string("");
				}

				int len = RSA_size(rsa);
				char* encryptedText = (char*)malloc(len + 1);
				memset(encryptedText, 0, len + 1);

				//����
				int ret = RSA_public_decrypt(clearText.length(), (const unsigned char*)clearText.c_str(), (unsigned char*)encryptedText, rsa, paddingMode);

				if (ret >= 0)
					strRet = string(encryptedText, ret);

				// �ͷ��ڴ�
				free(encryptedText);
				BIO_free_all(keybio);
				RSA_free(rsa);

				return strRet;
			}

			// ��Կ����
			static string publicEncrypt(const string& clearText, const string& pubKey, PaddingMode paddingMode = PKCS1_OAEP)
			{
				string strRet;
				BIO* keybio = BIO_new_mem_buf((unsigned char*)pubKey.c_str(), -1);
				RSA* rsa = RSA_new();


				//rsa = PEM_read_bio_RSAPublicKey(keybio, &rsa, NULL, NULL);//���ʧ�ܣ������ȱ�..
				rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
				if (!rsa)
				{
					BIO_free_all(keybio);
					return string("");
				}

				int len = RSA_size(rsa);
				char* encryptedText = (char*)malloc(len + 1);
				memset(encryptedText, 0, len + 1);

				// ���ܺ���
				int ret = RSA_public_encrypt(clearText.length(), (const unsigned char*)clearText.c_str(), (unsigned char*)encryptedText, rsa, paddingMode);
				if (ret >= 0)
					strRet = string(encryptedText, ret);

				// �ͷ��ڴ�
				free(encryptedText);
				BIO_free_all(keybio);
				RSA_free(rsa);

				return strRet;
			}

			// ˽Կ����
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

				// ���ܺ���
				int ret = RSA_private_decrypt(cipherText.length(), (const unsigned char*)cipherText.c_str(), (unsigned char*)decryptedText, rsa, paddingMode);
				if (ret >= 0)
					strRet = string(decryptedText, ret);

				// �ͷ��ڴ�
				free(decryptedText);
				BIO_free_all(keybio);
				RSA_free(rsa);
				return strRet;
			}

		};
	}
}