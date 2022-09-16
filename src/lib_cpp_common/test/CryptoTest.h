#pragma once
#include "test/BaseTest.h"
#include "crypto/Base64.hpp"
#include "crypto/UrlCoding.hpp"

namespace libcc
{
	namespace test
	{
		using libcc::crypto::Base64;
		using libcc::crypto::UrlCoding;
		using std::string;
		class CryptoTest :public BaseTest
		{
		public:
			virtual bool entry()
			{
				bool result = true;
				result &= aesTest();
				result &= base64Test();
				result &= md5Test();
				result &= rsaTest();
				result &= urlCodingTest();
				result &= xxhashTest();
				result &= curveTest();
				return result;
			}
		private:
			bool aesTest()
			{
				return false;
			}
			bool base64Test()
			{
				bool result = true;
				string text = "I really love strawbarry.";
				string encodeText = Base64::encode(text);
				string decodeText = Base64::decode(encodeText);
				result = text == decodeText;
				return result;
			}
			bool md5Test()
			{
				return false;
			}
			bool rsaTest()
			{
				return false;
			}
			bool urlCodingTest()
			{
				bool result = true;
				string text = "https://itunes.apple.com/search?term=tik tok&country=us&entity=software&limit=50";
				string encodeText = UrlCoding::encode(text);
				string decodeText = UrlCoding::decode(encodeText);
				result = text == decodeText;
				return result;
			}
			bool xxhashTest()
			{
				return false;
			}
			bool curveTest()
			{
				return false;
			}
		};
	}
}