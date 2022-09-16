#pragma once
#include "test/BaseTest.h"
#include "network/HttpRequest.hpp"
#include <string>
#include <map>

namespace libcc
{
	namespace test
	{
		using namespace network;
		using std::string;
		using std::wstring;
		using std::cout;
		using std::wcout;
		using std::endl;
		using std::map;



		static size_t receive_data(void* contents, size_t size, size_t nmemb, void* stream) {
			string* str = (string*)stream;
			(*str).append((char*)contents, size * nmemb);
			return size * nmemb;
		}

		class NetworktTest : public BaseTest
		{
		public:
			virtual bool entry()
			{


				bool result = true;
				result &= googleTranslate1();
				result &= googleTranslate2();
				return result;
			}


			bool googleTranslate1()
			{
				bool result = true;
				system("chcp 65001");

				string url = R"(http://translate.google.com/translate_a/single?client=gtx&dt=t&dj=1&ie=UTF-8&sl=auto&tl=en&q=)";
				string content1 = R"(今天天气不错,阳光明媚)";
				string content2 = R"(今天)";
				string content3 = R"(天气)";
				string content4 = R"(不错)";

				string url1 = url + HttpRequest::escape(content1);
				string url2 = url + HttpRequest::escape(content2);
				string url3 = url + HttpRequest::escape(content3);
				string url4 = url + HttpRequest::escape(content4);

				string userAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36";

				HttpRequestPool requestPool;
				HttpGet httpGet1(url1);
				HttpGet httpGet2(url2);
				HttpGet httpGet3(url3);
				HttpGet httpGet4(url4);

				requestPool.setProxy("127.0.0.1:8888");
				requestPool.addRequest(&httpGet1, [](Response response) {
					printf("%s\n\n",response.getContent().c_str());
				});
				requestPool.addRequest(&httpGet2, [](Response response) {
					printf("%s\n\n", response.getContent().c_str());
				});
				requestPool.addRequest(&httpGet3, [](Response response) {
					printf("%s\n\n", response.getContent().c_str());
				});
				requestPool.addRequest(&httpGet4, [](Response response) {
					printf("%s\n\n", response.getContent().c_str());
				});
				int res = requestPool.perform();
				return result;
			}

			bool googleTranslate2()
			{
				string url = R"(http://translate.google.com/translate_a/single?client=gtx&dt=t&dj=1&ie=UTF-8&sl=auto&tl=en&q=)";
				string content1 = R"(今天天气不错,阳光明媚)";

				HttpGet httpGet(url);
				Response response = httpGet.perform();
				printf("%s\n\n", response.getContent().c_str());

				return true;
			}

		};
	}
}