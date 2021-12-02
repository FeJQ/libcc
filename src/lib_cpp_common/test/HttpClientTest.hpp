#pragma once
#include "test/BaseTest.hpp"
#include "network/HttpClient.hpp"
#include <string>
#include <curl/curl.h>
#include "../StringConverter.h"

namespace libcc
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

	string bytesToString(const string& str)
	{
#ifdef _WIN32
		int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
		wchar_t* pwBuf = new wchar_t[nwLen + 1];    //一定要加1，不然会出现尾巴 
		memset(pwBuf, 0, nwLen * 2 + 2);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);
		int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
		char* pBuf = new char[nLen + 1];
		memset(pBuf, 0, nLen + 1);
		WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

		std::string strRet = pBuf;

		delete[]pBuf;
		delete[]pwBuf;
		pBuf = NULL;
		pwBuf = NULL;

		return strRet;
#elif defined __APPLE__
		return str;
#endif
	}

	class HttpClientTest : public BaseTest
	{
	public:
		virtual void entry()
		{
			system("chcp 65001");
			string url = "http://translate.google.com/translate_a/single?client=gtx&dt=t&dj=1&ie=UTF-8&sl=auto&tl=en&q=今天天气不错,阳光明媚";



			HttpClient httpClient;
			string s =httpClient.escape(url);


			string userAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36";


			Response response = httpClient.setProxy("127.0.0.1:7890")
				.setHeaders("")
				.get(url);
			if (response.failed())
			{
				cout << "error code:" << response.getCurlCode() << endl;
				cout << "log:" << response.getLog() << endl;
				return;
			}
			cout << response.data() << endl;
			getchar();
		}
	};
}