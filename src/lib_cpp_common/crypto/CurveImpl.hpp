#pragma once
#include <string>
#include "openssl/ecdh.h"	
#include "openssl/evp.h"
#include <assert.h>



#define CURVE25519_KEY_SIZE 32



namespace libcc
{
	namespace crypto
	{
		using std::string;
		class CurveImpl
		{
		public:

			class Keypair
			{
			public:
				Keypair() = default;
				Keypair(const Keypair& keypair) {
					this->privateKey = keypair.privateKey;
					this->publicKey = keypair.publicKey;
				}
				string getPublicKey()
				{
					return this->publicKey;
				}
				string getPrivateKey()
				{
					return this->privateKey;
				}
				string privateKey;
				string publicKey;
			};


			static Keypair generateKeypair()
			{
				Keypair keypair;
				EVP_PKEY *pkey{ nullptr };
				EVP_PKEY_CTX *pctx;

				keypair.privateKey.resize(32);
				keypair.publicKey.resize(32);
				size_t priv_len{ keypair.privateKey.size() };
				size_t pub_len{ keypair.publicKey.size() };

				pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_X25519, nullptr);
				assert(pctx != nullptr);			
				assert(EVP_PKEY_keygen_init(pctx) == 1);
				assert(EVP_PKEY_keygen(pctx, &pkey) == 1);
				assert(EVP_PKEY_get_raw_private_key(pkey, (unsigned char*)keypair.privateKey.data(), &priv_len) == 1);
				assert(EVP_PKEY_get_raw_public_key(pkey, (unsigned char*)keypair.publicKey.data(), &pub_len) == 1);
				EVP_PKEY_free(pkey);
				return keypair;
			}
		};
	}
}