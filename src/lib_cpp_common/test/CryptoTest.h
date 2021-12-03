#pragma once
#include "test/BaseTest.h"

namespace libcc
{
	namespace test
	{
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
				return false;
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
				return false;
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