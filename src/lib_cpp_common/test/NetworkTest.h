#pragma once
#include "test/BaseTest.h"
#include "network/HttpClient.hpp"
#include <string>
#include <curl/curl.h>

namespace libcc
{
	namespace test
	{
		using network::HttpClient;
		using network::Response;
		using std::string;
		using std::wstring;
		using std::cout;
		using std::wcout;
		using std::endl;

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
				result &= googleTranslate();
				return result;
			}

			bool googleTranslate()
			{
				bool result = true;
				system("chcp 65001");
				string url = "http://translate.google.com/translate_a/single?client=gtx&dt=t&dj=1&ie=UTF-8&sl=auto&tl=en&q=今天天气不错,阳光明媚";

				HttpClient httpClient;
				string s = httpClient.escape(url);

				string userAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36";


				Response response = httpClient.setProxy("127.0.0.1:7890")
					.setHeaders("")
					.get(url);
				if (response.failed())
				{
					cout << "error code:" << response.getCurlCode() << endl;
					cout << "log:" << response.getLog() << endl;
					return result;
				}
				cout << response.data() << endl;
				return result;
			}
		};
	}
}