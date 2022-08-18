#pragma once
#include <openssl/aes.h>
#include "Padding.hpp"
#include <assert.h>
namespace libcc
{
	namespace crypto
	{
		using std::string;
		using std::string_view;
		class Aes
		{
		public:
			enum class EncryptStandard
			{
				AES128 = 128,
				AES192 = 192,
				AES256 = 256
			};
			enum class EncryptMode
			{
				ECB,
				CBC,
				CFB,
				OFB,
				CTR,
			};

			static const int BLOCK_SIZE = AES_BLOCK_SIZE;

		public:
			Aes(string key, EncryptMode encryptMode = EncryptMode::ECB, string iv = "", PaddingMode paddingMode = PaddingMode::PKCS7)
			{
				assert(key.size() == 16 || key.size() == 24 || key.size() == 32);
				this->key = key;
				this->encryptMode = encryptMode;
				this->IV = iv;
				this->paddingMode = paddingMode;
			}

			string encrypt(string inputText)
			{
				this->bufferLength = ((Padding::length(inputText.size(), AES_BLOCK_SIZE) + AES_BLOCK_SIZE - 1) / AES_BLOCK_SIZE) * AES_BLOCK_SIZE;  //¶ÔÆë·Ö×é

				AES_KEY aesKey;
				memset(&aesKey, 0, sizeof(aesKey));
				AES_set_encrypt_key((unsigned char*)this->key.data(), key.size() * 8, &aesKey);
				string paddedText = Padding::pad(inputText, AES_BLOCK_SIZE, this->paddingMode);

				string cipherText;
				cipherText.resize(this->bufferLength);

				string tempIV = this->IV;

				for (int i = 0; i < paddedText.size() / AES_BLOCK_SIZE; i++)
				{
					int groupPos = AES_BLOCK_SIZE * i;
					switch (this->encryptMode)
					{
					case EncryptMode::ECB:
						AES_ecb_encrypt((const unsigned char*)(paddedText.substr(groupPos).data()), (unsigned char*)cipherText.data() + groupPos, &aesKey, AES_ENCRYPT);
						break;
					case EncryptMode::CBC:
						assert(!tempIV.empty());
						AES_cbc_encrypt((const unsigned char*)paddedText.data(), (unsigned char*)cipherText.data(), paddedText.size(), &aesKey, (unsigned char*)tempIV.data(), AES_ENCRYPT);
						return cipherText;
					case EncryptMode::CFB: break;
					case EncryptMode::OFB: break;
					case EncryptMode::CTR: break;
					default:abort(); break;
					}
				}
				return cipherText;
			}
			string decrypt(string_view inputText)
			{
				AES_KEY aesKey;
				memset(&aesKey, 0, sizeof(aesKey));
				AES_set_decrypt_key((unsigned char*)this->key.data(), key.size() * 8, &aesKey);

				string clearText;
				clearText.resize(inputText.size());

				string tempIV = this->IV;
				bool isBreak = false;
				for (int i = 0; i < inputText.size() / AES_BLOCK_SIZE; i++)
				{
					int groupPos = AES_BLOCK_SIZE * i;

					switch (this->encryptMode)
					{
					case EncryptMode::ECB:
						AES_ecb_encrypt((const unsigned char*)(inputText.substr(groupPos).data()), (unsigned char*)clearText.data() + groupPos, &aesKey, AES_DECRYPT);
						break;
					case EncryptMode::CBC:
						AES_cbc_encrypt((const unsigned char*)inputText.data(), (unsigned char*)clearText.data(), inputText.size(), &aesKey, (unsigned char*)tempIV.data(), AES_DECRYPT);
						isBreak = true;
						break;
					case EncryptMode::CFB: break;
					case EncryptMode::OFB:break;
					case EncryptMode::CTR: break;
					default:break;
					}
					if (isBreak) break;
				}
				if (this->paddingMode != PaddingMode::NoPadding)
				{
					return Padding::unpad(clearText);
				}
				return clearText;
			}


		private:
			string key;
			string IV;
			EncryptMode encryptMode;
			PaddingMode paddingMode;
			int bufferLength;
		};
	}
}