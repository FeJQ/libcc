#pragma once
#include "test/BaseTest.h"
#include "network/HttpClient.hpp"
#include <string>
#include <curl/curl.h>
#include <map>

namespace libcc
{
	namespace test
	{
		using network::HtmlEnctype;
		using network::HttpClient;
		using network::Response;
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
				result &= ifanyiTranslate();
				result &= youdaoTranlate();
				result &= googleTranslate();
				return result;
			}

			bool youdaoTranlate()
			{
				string url = "http://fanyi.youdao.com/translate?smartresult=dict&smartresult=rule";
				map<string, string> formDataMap = {
				 { "i", "fdfdd"},
				 { "type" , "AUTO"},
				 { "smartresult" , "dict"},
				 { "client" , "fanyideskweb"},
				 { "ue" , "UTF-8"},
				 { "typoResult" , "true"},
				 { "doctype" , "json"},
				 { "version" , "2.1"},
				 { "keyfrom:" , "fanyi.web"},
				};
				HttpClient httpClient;
				Response response = httpClient.setProxy("127.0.0.1:8888").post(url, formDataMap, HtmlEnctype::application_x_www_urlencoded);
				return response.success();
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

			bool ifanyiTranslate()
			{
				string result;
				char* nation = (char*)"cn";
				char* sl = (char*)"auto";
				char* tl = (char*)"ja";
				string query = R"(年后)";
				string proxy = "127.0.0.1:7890";


				string  url = "http://ifanyi.iciba.com/index.php?c=trans&m=fy&client=6&auth_user=key_ciba&sign=f0d00165077fdb7d";
				HttpClient httpClient;
				string q = query;
				q = httpClient.escape(q);

				httpClient.setProxy(proxy);

				map<string, string> headDataMap = {
						{"User-Agent","Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/72.0.3626.121 Safari/537.36"},
						{"Accept-Encoding","gzip, deflate"},
				};
				/*map<string, string> formDataMap = {
				  { "from", sl},
				  { "to" , tl},
				  { "q" , q},
				};*/
				string raw = "from=" + string(sl) + "&to=" + tl + "&q=" + q;
				Response response = httpClient.setHeaders(headDataMap).post(url, raw);
				if (response.success())
				{
					result = response.data();
					return true;
				}
				return false;
			}
		};
	}
}