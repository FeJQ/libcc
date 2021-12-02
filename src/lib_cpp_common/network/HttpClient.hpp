#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include "curl/curl.h"
#include <assert.h>

#ifndef CCString
#ifdef _WIN32
#define CCString wstring
#endif
#elif defined __APPLE__
#define CCString string
#endif 

#ifndef CCChar
#ifdef _WIN32
#define CCChar wchar_t
#endif
#elif defined __APPLE__
#define CCChar char
#endif 

#ifndef CCAccess
#ifdef _WIN32
#define CCAccess _waccess
#endif
#elif defined __APPLE__
#define CCAccess access
#endif 

#ifndef CCMkdir
#ifdef _WIN32
#define CCMkdir _wmkdir
#endif
#elif defined __APPLE__
#define CCMkdir mkdir
#endif 




namespace libcc
{
	namespace network
	{
		using std::string;
		using std::map;
		using std::vector;
		using std::wstring;
		using std::string;
		enum class HttpStatus
		{
			Error = 0,
			Continue = 100,
			SwitchingProtocols = 101,
			OK = 200,
			Created = 201,
			Accepted = 202,
			NonAuthoritativeInformation = 203,
			NoContent = 204,
			ResetContent = 205,
			PartialContent = 206,
			MultipleChoices = 300,
			MovedPermanently = 301,
			Found = 302,
			SeeOther = 303,
			NotModified = 304,
			UseProxy = 305,
			Unused = 306,
			TemporaryRedirect = 307,
			BadRequest = 400,
			Unauthorized = 401,
			PaymentRequired = 402,
			Forbidden = 403,
			NotFound = 404,
			MethodNotAllowed = 405,
			NotAcceptable = 406,
			ProxyAuthenticationRequired = 407,
			RequestTimeout = 408,
			Conflict = 409,
			Gone = 410,
			LengthRequired = 411,
			PreconditionFailed = 412,
			RequestEntityTooLarge = 413,
			RequestUriTooLarge = 414,
			UnsupportedMediaType = 415,
			RequestedRangeNotSatisfiable = 416,
			ExpectationFailed = 417,
			InternalServerError = 500,
			NotImplemented = 501,
			BadGateway = 502,
			ServiceUnavailable = 503,
			GatewayTimeout = 504,
			HTTPVersionNotSupported = 505
		};

		class Response
		{
		public:
			Response() {
				initialize();
			};
			Response(CURLcode code, string msg)
			{
				this->clear();
				this->status = HttpStatus::Error;
				this->curlCode = code;
				this->log = msg;
			}
			Response(const Response& response)
			{
				this->responseContent = response.responseContent;
				this->responseHeaderContent = response.responseHeaderContent;
				this->responseHeaderMap = response.responseHeaderMap;
				this->status = response.status;
				this->errorBuffer = response.errorBuffer;
				this->curlCode = response.curlCode;
				this->log = response.log;
			}

			void initialize()
			{
				this->clear();
				errorBuffer.resize(CURL_ERROR_SIZE);
			}

			string data() { return responseContent; }
			void setData(string data) { this->responseContent = data; }

			HttpStatus code() { return status; }
			void setCode(HttpStatus status) { this->status = status; }

			map<string, string> getResponseHeaderMap() { return responseHeaderMap; }
			void setReponseHeaderMap(map<string, string> responseHeaderMap) { this->responseHeaderMap = responseHeaderMap; }

			string getResponseHeader() { return responseHeaderContent; }
			void setResponseHeader(string responseHeader) { this->responseHeaderContent = responseHeader; }

			string* getErrorMessageRef() { return &errorBuffer; }
			string getErrorMessage() { return string(errorBuffer); }

			void clear()
			{
				this->responseContent.clear();
				this->responseHeaderContent.clear();
				this->responseHeaderMap.clear();
				this->errorBuffer.clear();
				this->log.clear();
				this->status = HttpStatus::Error;
				this->curlCode = 0;

			}
			string getLog() { return string(log); }
			int getCurlCode() { return curlCode; }

			bool failed() { return this->status != HttpStatus::OK; }
			bool success() { return this->status == HttpStatus::OK; }
		private:
			string responseContent;
			string responseHeaderContent;
			map<string, string> responseHeaderMap;

			HttpStatus status;
			string log;
			string errorBuffer;
			int curlCode;
		};

		class  HttpClient
		{
		public:
			HttpClient() {
				CURLcode code;

				code = curl_global_init(CURL_GLOBAL_ALL);
				assert(code == CURLE_OK);
				this->curl = curl_easy_init();
				assert(this->curl != NULL);
				code = curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, writeCallback);
				assert(code == CURLE_OK);
				// 设置WriteFunction传入的user data
				code = curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &this->response);
				assert(code == CURLE_OK);
				code = curl_easy_setopt(this->curl, CURLOPT_HEADERFUNCTION, headerCallback);
				assert(code == CURLE_OK);
				// 设置HeaderFunction传入的user data
				code = curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, &this->response);
				assert(code == CURLE_OK);
				// 跟踪重定向
				code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
				assert(code == CURLE_OK);
				// 设置error buffer
				code = curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, this->response.getErrorMessageRef()->data());
				assert(code == CURLE_OK);
			};

			HttpClient(const HttpClient& httpClient) = delete;


			~HttpClient()
			{
				curl_easy_cleanup(this->curl);
				curl_global_cleanup();
			}


			HttpClient& setHeaders(string headers)
			{
				CURLcode code;
				struct curl_slist* headerList = NULL;
				headerList = curl_slist_append(headerList, headers.c_str());
				code = curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, headerList);
				assert(code == CURLE_OK);
				return *this;
			}

			HttpClient& setHeaders(vector<string> headerVector)
			{
				CURLcode code;
				struct curl_slist* headerList = NULL;
				for (auto i = 0; i < headerVector.size(); i++)
				{
					headerList = curl_slist_append(headerList, headerVector[i].c_str());
				}
				code = curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, headerList);
				assert(code == CURLE_OK);
				return *this;
			}

			HttpClient& setHeaders(map<string, string> headerMap)
			{
				CURLcode code;
				struct curl_slist* headerList = NULL;
				for (auto i = headerMap.begin(); i != headerMap.end(); i++)
				{
					string header = i->first + ":" + i->second;
					headerList = curl_slist_append(headerList, header.c_str());
				}
				code = curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, headerList);
				assert(code == CURLE_OK);
				return *this;
			}

			HttpClient& setSslVerify(bool value)
			{
				CURLcode code;
				code = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, value);
				assert(code == CURLE_OK);
				code = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, value);
				assert(code == CURLE_OK);
				return *this;
			}


			/**
			  * 发送Get请求
			  *
			  * @param url 请求地址
			  * @return Response
			  */
			Response get(string url)
			{
				CURLcode code = CURLE_OK;
				status = HttpStatus::OK;
				this->response.clear();
				code = curl_easy_setopt(this->curl, CURLOPT_URL, url.c_str());
				assert(code == CURLE_OK);
				code = curl_easy_setopt(this->curl, CURLOPT_POST, 0);
				assert(code == CURLE_OK);
				code = curl_easy_perform(this->curl);
				if (code != CURLE_OK)
					return Response(code, curl_easy_strerror(code));
				code = curl_easy_getinfo(this->curl, CURLINFO_RESPONSE_CODE, &status);
				assert(code == CURLE_OK);
				this->response.setCode(status);
				return this->response;
			}


			/**
			  * 发送Post请求
			  *
			  * @param url 请求地址
			  * @param postData post数据
			  * @return Response
			  */
			Response post(string url, string postData)
			{
				status = HttpStatus::OK;
				this->response.clear();
				CURLcode code = curl_easy_setopt(this->curl, CURLOPT_URL, url.c_str());
				assert(code == CURLE_OK);
				code = curl_easy_setopt(this->curl, CURLOPT_POST, 1);
				assert(code == CURLE_OK);
				curl_easy_setopt(this->curl, CURLOPT_POSTFIELDSIZE, postData.length());
				assert(code == CURLE_OK);
				code = curl_easy_setopt(this->curl, CURLOPT_POSTFIELDS, postData.data());
				assert(code == CURLE_OK);
				code = curl_easy_perform(this->curl);
				if (code != CURLE_OK)
					return Response(code, curl_easy_strerror(code));
				code = curl_easy_getinfo(this->curl, CURLINFO_RESPONSE_CODE, &status);
				assert(code == CURLE_OK);
				this->response.setCode(status);
				return this->response;
			}


			/**
			  * 发送Post请求
			  *
			  * @param url 请求地址
			  * @param formDataMap 表单数据，如{"key","value"}
			  * @return Response
			  */
			Response post(string url, map<string, string> formDataMap)
			{
				status = HttpStatus::OK;
				this->response.clear();
				CURLcode code = curl_easy_setopt(this->curl, CURLOPT_URL, url.c_str());
				assert(code == CURLE_OK);
				struct curl_httppost* form = NULL;
				struct curl_httppost* last = NULL;

				/* build form */
				for (auto i = formDataMap.begin(); i != formDataMap.end(); i++)
				{
					curl_formadd(&form, &last, CURLFORM_COPYNAME, i->first.c_str(), CURLFORM_COPYCONTENTS, i->second.c_str(), CURLFORM_END);
				}
				code = curl_easy_setopt(this->curl, CURLOPT_HTTPPOST, form);
				assert(code == CURLE_OK);

				/* perform request */
				code = curl_easy_perform(this->curl);
				if (code != CURLE_OK)
					return Response(code, curl_easy_strerror(code));
				code = curl_easy_getinfo(this->curl, CURLINFO_RESPONSE_CODE, &status);
				assert(code == CURLE_OK);
				this->response.setCode(status);
				return this->response;
			}

			/**
			  * 开启并设置cookie的路径
			  * @param path cookie的存放和读取路径
			  * @return HttpClient 返回本类实例
			  */
			HttpClient& setCookiePath(CCString fileName)
			{
				CURLcode code;
				int size = fileName.size();
				CCChar tmpDirPath[256] = { 0 };
				for (int i = 0; i < size; i++)
				{
					tmpDirPath[i] = fileName[i];
					if (tmpDirPath[i] == '\\' || tmpDirPath[i] == '/')
					{
						if (CCAccess(tmpDirPath, 0) == -1)
						{
							int ret = CCMkdir(tmpDirPath);
							if (ret == -1) throw std::exception("mkdir error");
						}
					}
				}
				code = curl_easy_setopt(curl, CURLOPT_COOKIEFILE, fileName.c_str());
				assert(code == CURLE_OK);
				code = curl_easy_setopt(curl, CURLOPT_COOKIEJAR, fileName.c_str());
				assert(code == CURLE_OK);
				return *this;
			}

			/**
			  * 设置代理
			  *
			  * @param address 代理的地址和端口,如"127.0.0.1:8888"
			  * @return HttpClient 返回本类实例
			  */
			HttpClient& setProxy(string address)
			{
				CURLcode code;
				code = curl_easy_setopt(curl, CURLOPT_PROXY, address.c_str());
				assert(code == CURLE_OK);
				return *this;
			}

			/**
			  * 设置用户令牌
			  *
			  * @param userAgent 令牌,如"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36"
			  * @return HttpClient 返回本类实例
			  */
			HttpClient& setUserAgent(string userAgent)
			{
				CURLcode code;
				code = curl_easy_setopt(curl, CURLOPT_USERAGENT, userAgent.c_str());
				if (code != CURLE_OK)
					throw std::exception(string(curl_easy_strerror(code) + std::to_string(code)).c_str());
				return *this;
			}

			/**
			  * 设置超时时间
			  *
			  * @param connectTimeout 连接超时时间(秒)
			  * @param requestTimeout 请求超时时间(秒)
			  * @return HttpClient 返回本类实例
			  */
			HttpClient& setTimeout(unsigned int connectTimeout, unsigned int requestTimeout = -1)
			{
				CURLcode code;
				if (requestTimeout == -1)
				{
					requestTimeout = connectTimeout;
				}
				code = curl_easy_setopt(this->curl, CURLOPT_CONNECTTIMEOUT, connectTimeout);
				assert(code == CURLE_OK);
				code = curl_easy_setopt(this->curl, CURLOPT_TIMEOUT, requestTimeout);
				assert(code == CURLE_OK);
				return *this;
			}


			HttpClient& setKeepAlive(bool value, long intervalTime = 60, long idleTime = 120)
			{
				CURLcode code;
				code = curl_easy_setopt(this->curl, CURLOPT_TCP_KEEPALIVE, value);
				assert(code == CURLE_OK);

				/* set keep-alive idle time to 120 seconds */
				code = curl_easy_setopt(this->curl, CURLOPT_TCP_KEEPIDLE, idleTime);
				assert(code == CURLE_OK);

				/* interval time between keep-alive probes: 60 seconds */
				code = curl_easy_setopt(this->curl, CURLOPT_TCP_KEEPINTVL, intervalTime);
				assert(code == CURLE_OK);
				return *this;
			}

			HttpClient& setAcceptEncoding(string value)
			{
				CURLcode code = curl_easy_setopt(this->curl, CURLOPT_ACCEPT_ENCODING, value.c_str());
				assert(code == CURLE_OK);
				return *this;
			}


			/**
			  * 对内容进行转码;对curl_easy_escape函数的一层封装
			  *
			  * @param content 要转码的内容
			  */
			string escape(string content)
			{
				char* e_content = curl_easy_escape(curl, content.c_str(), content.length());
				std::string result = string(e_content);
				curl_free(e_content);
				return result;
			}


		private:
			static size_t writeCallback(void* data, size_t size, size_t nmemb, void* userData)
			{
				Response* response = (Response*)userData;
				long long total = size * nmemb;
				if (total != 0)
				{
					response->setData(response->data() + string((char*)data, total));
				}
				else
					return -1;
				return size * nmemb;
			}
			static size_t headerCallback(void* data, size_t size, size_t nmemb, void* userData)
			{
				Response* response = (Response*)userData;
				const size_t total = size * nmemb;
				if (total != 0) {
					char* header = (char*)malloc(total + 1);
					char* value = NULL;
					char* p = NULL;
					memcpy(header, data, total);
					header[total] = '\0';
					if (strlen(header))
					{
						response->setResponseHeader(response->getResponseHeader() + header);
					}
					p = strchr(header, ':');
					if (p) {
						*p = '\0';
						p++;
						while (*p == ' ') {
							p++;
						}
						if (*p != '\0') {
							value = p;
							while (*p != '\0' && *p != '\n' && *p != '\r') {
								p++;
							}
							*p = '\0';
						}
					}
					if (value) {
						map<string, string> map = response->getResponseHeaderMap();
						map.insert(std::pair<std::string, std::string>(header, value));
						response->setReponseHeaderMap(map);
					}
					free(header);
				}
				return total;
			}
		private:
			Response response;
			//string cookies;
			CURL* curl;
			HttpStatus status;
		};
	}
}