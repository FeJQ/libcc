#pragma once
#include <string>
#include <xxhash.h>
#include <sstream>


namespace libcc
{
	namespace crypto
	{
		using std::string;
		class Xxhash
		{
		public:
			static string hash32(string input)
			{
				string result;
				/* create a hash state */
				XXH32_state_t* const state = XXH32_createState();
				if (state == NULL) abort();
				size_t size = input.size();

				/* Initialize state with selected seed */
				XXH32_hash_t const seed = 0;   /* or any other value */
				if (XXH32_reset(state, seed) == XXH_ERROR) abort();

				/* Feed the state with input data, any size, any number of times */
				if (XXH32_update(state, input.data(), input.size()) == XXH_ERROR) abort();

				/* Produce the final hash value */
				XXH32_hash_t const hash = XXH32_digest(state);

				char s[9];
				sprintf(s, "%0*x", 8, hash);
				result = s;
				return result;
				//result = string((char*)hash);

				/* State could be re-used; but in this example, it is simply freed  */
				XXH32_freeState(state);
				return result;
			}
			static string hash64(string input)
			{
				string result;
				/* create a hash state */
				XXH64_state_t* const state = XXH64_createState();
				if (state == NULL) abort();
				size_t size = input.size();

				/* Initialize state with selected seed */
				XXH64_hash_t const seed = 0;   /* or any other value */
				if (XXH64_reset(state, seed) == XXH_ERROR) abort();

				/* Feed the state with input data, any size, any number of times */
				if (XXH64_update(state, input.data(), input.size()) == XXH_ERROR) abort();

				/* Produce the final hash value */
				XXH64_hash_t const hash = XXH64_digest(state);
				char s[9];
				sprintf(s, "%0*x", 16, hash);
				result = s;
				return result;

				/* State could be re-used; but in this example, it is simply freed  */
				XXH64_freeState(state);
				return result;
			}

			virtual void update(string input) = 0;
			virtual string digest() = 0;
		};


		class Xxhash32 :public Xxhash
		{
		public:

			Xxhash32(string input = "", XXH32_hash_t seed = 0)
			{
				this->state = XXH32_createState();
				//memset(this->state, 0, sizeof(XXH32_state_t);
				if (this->state == NULL) abort();

				if (XXH32_reset(this->state, seed) == XXH_ERROR) abort();
				this->update(input);
			}

			~Xxhash32()
			{
				XXH32_freeState(this->state);
			}
			void update(string input)
			{
				if (XXH32_update(this->state, input.data(), input.size()) == XXH_ERROR) abort();
			}
			string digest()
			{
				string result;
				XXH32_hash_t const hash = XXH32_digest(this->state);
				char s[9];
				sprintf(s, "%0*x", 8, hash);
				result = s;
				return result;
			}
		private:
			XXH32_state_t* state;
		};

		class Xxhash64 :public Xxhash
		{
		public:
			Xxhash64(string input = "", XXH32_hash_t seed = 0)
			{
				this->state = XXH64_createState();
				if (this->state == NULL) abort();

				if (XXH64_reset(this->state, seed) == XXH_ERROR) abort();
				this->update(input);
			}

			~Xxhash64()
			{
				XXH64_freeState(this->state);
			}
			void update(string input)
			{
				if (XXH64_update(this->state, input.data(), input.size()) == XXH_ERROR) abort();
			}
			string digest()
			{
				string result;
				XXH64_hash_t const hash = XXH64_digest(this->state);
				char s[9];
				sprintf(s, "%0*x", 16, hash);
				result = s;
				return result;
			}
		private:
			XXH64_state_t* state;
		};
	}
}