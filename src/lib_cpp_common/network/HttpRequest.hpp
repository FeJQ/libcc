#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <curl/curl.h>
#include <assert.h>
#include <comdef.h>
#include <functional>

#ifdef _WIN32
#include <Winhttp.h>
#elif defined __APPLE__
#include <unistd.h>
#include <sys/stat.h>
#include <exception>

#endif

// -----Win-----
#ifdef _WIN32

// CCString
#ifndef CCString
#define CCString wstring
#endif

// CCChar
#ifndef CCChar
#define CCChar wchar_t
#endif

// CCAccess
#ifndef CCAccess
#define CCAccess _waccess
#endif

// CCMkdir
#ifndef CCMkdir
#define CCMkdir(x) _wmkdir(x)
#endif


// ----Apple------
#elif defined __APPLE__

#ifndef CCString
#define CCString string
#endif

#ifndef CCChar
#define CCChar char
#endif

#ifndef CCAccess
#define CCAccess access
#endif

#ifndef CCMkdir
#define CCMkdir(x) ::mkdir(x,0777)
#endif

#endif 







namespace libcc
{
	namespace network
	{
		class HttpRequestPool;
		class Response;

		using std::string;
		using std::map;
		using std::vector;
		using std::wstring;
		using std::function;

		using ResponseCallback = std::function <void(Response response)>;
		using AsyncWaitCallback = std::function<bool(void)>;
		using AsyncReadCallback = std::function<bool(void)>;
		enum class HttpStatus
		{
			kStopedByUser = -1,
			kError = 0,
			kContinue = 100,
			kSwitchingProtocols = 101,
			kOK = 200,
			kCreated = 201,
			kAccepted = 202,
			kNonAuthoritativeInformation = 203,
			kNoContent = 204,
			kResetContent = 205,
			kPartialContent = 206,
			kMultipleChoices = 300,
			kMovedPermanently = 301,
			kFound = 302,
			kSeeOther = 303,
			kNotModified = 304,
			kUseProxy = 305,
			kUnused = 306,
			kTemporaryRedirect = 307,
			kBadRequest = 400,
			kUnauthorized = 401,
			kPaymentRequired = 402,
			kForbidden = 403,
			kNotFound = 404,
			kMethodNotAllowed = 405,
			kNotAcceptable = 406,
			kProxyAuthenticationRequired = 407,
			kRequestTimeout = 408,
			kConflict = 409,
			kGone = 410,
			kLengthRequired = 411,
			kPreconditionFailed = 412,
			kRequestEntityTooLarge = 413,
			kRequestUriTooLarge = 414,
			kUnsupportedMediaType = 415,
			kRequestedRangeNotSatisfiable = 416,
			kExpectationFailed = 417,
			kInternalServerError = 500,
			kNotImplemented = 501,
			kBadGateway = 502,
			kServiceUnavailable = 503,
			kGatewayTimeout = 504,
			kHTTPVersionNotSupported = 505
		};

		enum class HtmlEncType
		{
			application_x_www_urlencoded,
			multipart_form_data,
			text_plain
		};
		enum class HttpRequestMethod
		{
			kNone,
			kGet,
			kPost,
			kPut
		};

		class Response
		{
		public:
			friend class HttpRequest;
			Response() {
				initialize();
			};
			Response(CURLcode code, string msg)
			{
				this->clear();
				this->status = HttpStatus::kError;
				this->curlCode = code;
				this->log = msg;
			}
			Response(const Response& response)
			{
				this->clear();
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

			string getContent() { return responseContent; }
			HttpStatus getStatus() { return status; }
			map<string, string> getHeaderMap() { return responseHeaderMap; }
			string getHeader() { return responseHeaderContent; }
			string* getErrorMessageRef() { return &errorBuffer; }
			string getErrorMessage() { return string(errorBuffer); }

			void clear()
			{
				this->responseContent.clear();
				this->responseHeaderContent.clear();
				this->responseHeaderMap.clear();
				this->errorBuffer.clear();
				this->log.clear();
				this->status = HttpStatus::kError;
				this->curlCode = 0;

			}
			string getLog() { return string(log); }
			int getCurlCode() { return curlCode; }

			bool failed() { return this->status != HttpStatus::kOK; }
			bool success() { return this->status == HttpStatus::kOK; }

		private:
			void setContent(string data) { this->responseContent = data; }
			void setStatus(HttpStatus status) { this->status = status; }
			void setHeaderMap(map<string, string> responseHeaderMap) { this->responseHeaderMap = responseHeaderMap; }
			void setHeader(string responseHeader) { this->responseHeaderContent = responseHeader; }
		private:
			string responseContent;
			string responseHeaderContent;
			map<string, string> responseHeaderMap;

			HttpStatus status;
			string log;
			string errorBuffer;
			int curlCode;
		};


		class  HttpRequest
		{
		public:
			HttpRequest(string url)
			{
				initialize();
				setUrl(url);
			};

			HttpRequest(const HttpRequest& httpRequest)
			{
				m_response = httpRequest.m_response;
				m_curl = curl_easy_duphandle(httpRequest.m_curl);
				m_status = httpRequest.m_status;
				m_url = httpRequest.m_url;
				m_postFields = httpRequest.m_postFields;
				m_formData = httpRequest.m_formData;
				setFinishedCallback(httpRequest.m_responseCallback);
			};

		public:
			~HttpRequest()
			{
				if (!m_useMultiApi)
					curl_easy_cleanup(m_curl);
				//curl_global_cleanup();
			}

			Response getResponse()
			{
				return m_response;
			}

			void initialize()
			{
				CURLcode code;
				code = curl_global_init(CURL_GLOBAL_ALL);
				assert(code == CURLE_OK);
				m_curl = curl_easy_init();
				assert(m_curl != NULL);

				setWriteCallback(writeCallback, this);
				setHeaderCallback(headerCallback, this);

				// 跟踪重定向
				code = curl_easy_setopt(m_curl, CURLOPT_FOLLOWLOCATION, true);
				assert(code == CURLE_OK);
				// 设置error buffer
				code = curl_easy_setopt(m_curl, CURLOPT_ERRORBUFFER, m_response.getErrorMessageRef()->data());
				assert(code == CURLE_OK);
				// 指定压缩算法
				//setAcceptEncoding("gzip, deflate");

				setSslVerify(false);
				setCookiePath(L"cookie");
				setUseMultiApi(false);
			}

			HttpRequest& setWriteCallback(size_t(*)(void* data, size_t size, size_t nmemb, void* userData), void* userData)
			{
				CURLcode code;
				code = curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, writeCallback);
				assert(code == CURLE_OK);
				// 设置WriteFunction传入的user data
				code = curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, this);
				assert(code == CURLE_OK);
				return *this;
			}

			HttpRequest& setHeaderCallback(size_t(*)(void* data, size_t size, size_t nmemb, void* userData), void* userData)
			{
				CURLcode code;
				// 指定响应头回调函数
				code = curl_easy_setopt(m_curl, CURLOPT_HEADERFUNCTION, headerCallback);
				assert(code == CURLE_OK);
				// 设置HeaderFunction传入的user data
				code = curl_easy_setopt(m_curl, CURLOPT_HEADERDATA, this);
				assert(code == CURLE_OK);
				return *this;
			}


			Response perform() {};

			HttpRequest& setHeaders(string headers)
			{
				CURLcode code;
				struct curl_slist* headerList = NULL;
				headerList = curl_slist_append(headerList, headers.c_str());
				code = curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, headerList);
				assert(code == CURLE_OK);
				return *this;
			}

			HttpRequest& setHeaders(vector<string> headerVector)
			{
				CURLcode code;
				struct curl_slist* headerList = NULL;

				/* set http headers */
				for (int i = 0; i < headerVector.size(); i++)
				{
					headerList = curl_slist_append(headerList, headerVector[i].c_str());
				}
				code = curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, headerList);
				assert(code == CURLE_OK);
				return *this;
			}

			HttpRequest& setHeaders(map<string, string> headerMap)
			{
				CURLcode code;
				struct curl_slist* headerList = NULL;
				vector<string> headerVector;
				for (auto i = headerMap.begin(); i != headerMap.end(); i++)
				{
					string header = i->first + ":" + i->second;
					headerVector.push_back(header);
				}
				return setHeaders(headerVector);
			}

			HttpRequest& setSslVerify(bool value)
			{
				CURLcode code;
				code = curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYPEER, value);
				assert(code == CURLE_OK);
				code = curl_easy_setopt(m_curl, CURLOPT_SSL_VERIFYHOST, value);
				assert(code == CURLE_OK);
				return *this;
			}

			HttpRequest& setUseSystemProxy(bool value)
			{
				CURLcode code;
				string sysProxy;
				if (value)
				{
					sysProxy = querySystemProxy();
					this->setProxy(sysProxy);
				}

				return *this;
			}

			/**
			  * 发起Get请求
			  * @param url 请求地址
			  * @return Response
			  */
			Response get()
			{
				CURLcode code = CURLE_OK;
				m_status = HttpStatus::kOK;
				m_response.clear();

				code = curl_easy_perform(m_curl);
				if (code != CURLE_OK)
					return Response(code, curl_easy_strerror(code));
				code = curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &m_status);
				assert(code == CURLE_OK);
				setHttpStatus(m_status);
				return m_response;
			}


			/**
			  * 发起Post请求
			  * @return Response
			  */
			Response post()
			{
				m_status = HttpStatus::kOK;
				m_response.clear();

				/* perform request */
				CURLcode code = curl_easy_perform(m_curl);
				if (code != CURLE_OK)
					return Response(code, curl_easy_strerror(code));
				code = curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &m_status);
				assert(code == CURLE_OK);
				setHttpStatus(m_status);
				return m_response;
			}

			/**
			  * 发起Put请求
			  * @return Response
			  */
			Response put()
			{
				m_status = HttpStatus::kOK;
				m_response.clear();
				CURLcode code = curl_easy_setopt(m_curl, CURLOPT_URL, m_url.c_str());
				assert(code == CURLE_OK);
				code = curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "PUT");
				//code = curl_easy_setopt(this->curl, CURLOPT_PUT, 1);
				assert(code == CURLE_OK);
				code = curl_easy_perform(m_curl);
				if (code != CURLE_OK)
					return Response(code, curl_easy_strerror(code));
				code = curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &m_status);
				assert(code == CURLE_OK);
				setHttpStatus(m_status);
				return m_response;
			}

			/**
			  * 开启并设置cookie的路径
			  * @param path cookie的存放和读取路径
			  * @return HttpRequest 返回本类实例
			  */
			HttpRequest& setCookiePath(CCString fileName)
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
							assert(ret != -1);
						}
					}
				}
				code = curl_easy_setopt(m_curl, CURLOPT_COOKIEFILE, fileName.c_str());
				assert(code == CURLE_OK);
				code = curl_easy_setopt(m_curl, CURLOPT_COOKIEJAR, fileName.c_str());
				assert(code == CURLE_OK);
				return *this;
			}

			/**
			  * 设置代理
			  * @param address 代理的地址和端口,如"127.0.0.1:8888"
			  * @return HttpRequest 返回本类实例
			  */
			HttpRequest& setProxy(string address)
			{
				CURLcode code;
				code = curl_easy_setopt(m_curl, CURLOPT_PROXY, address.c_str());
				assert(code == CURLE_OK);
				return *this;
			}

			/**
			  * 设置用户令牌
			  * @param userAgent 令牌,如"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36"
			  * @return HttpRequest 返回本类实例
			  */
			HttpRequest& setUserAgent(string userAgent)
			{
				CURLcode code;
				code = curl_easy_setopt(m_curl, CURLOPT_USERAGENT, userAgent.c_str());
				assert(code == CURLE_OK);
				return *this;
			}

			/**
			  * 设置超时时间
			  * @param connectTimeout 连接超时时间(ms)
			  * @param requestTimeout 请求超时时间(ms)
			  * @return HttpRequest 返回本类实例
			  */
			HttpRequest& setTimeout(unsigned int connectTimeout, unsigned int requestTimeout = -1)
			{
				CURLcode code;
				if (requestTimeout == -1)
				{
					requestTimeout = connectTimeout;
				}
				code = curl_easy_setopt(m_curl, CURLOPT_CONNECTTIMEOUT_MS, connectTimeout);
				assert(code == CURLE_OK);
				code = curl_easy_setopt(m_curl, CURLOPT_TIMEOUT_MS, requestTimeout);
				assert(code == CURLE_OK);
				return *this;
			}


			HttpRequest& setKeepAlive(bool value, long intervalTime = 60, long idleTime = 120)
			{
				CURLcode code;
				code = curl_easy_setopt(m_curl, CURLOPT_TCP_KEEPALIVE, value);
				assert(code == CURLE_OK);

				/* set keep-alive idle time to 120 seconds */
				code = curl_easy_setopt(m_curl, CURLOPT_TCP_KEEPIDLE, idleTime);
				assert(code == CURLE_OK);

				/* interval time between keep-alive probes: 60 seconds */
				code = curl_easy_setopt(m_curl, CURLOPT_TCP_KEEPINTVL, intervalTime);
				assert(code == CURLE_OK);
				return *this;
			}

			HttpRequest& setAcceptEncoding(string value)
			{
				CURLcode code = curl_easy_setopt(m_curl, CURLOPT_ACCEPT_ENCODING, value.c_str());
				assert(code == CURLE_OK);
				return *this;
			}

			HttpRequest& setUrl(string url)
			{
				m_url = url;
				curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
				return *this;
			}

			HttpRequest& setRequestMethod(HttpRequestMethod method)
			{
				CURLcode code = CURLE_OK;
				switch (method)
				{
				case HttpRequestMethod::kNone:
					break;
				case HttpRequestMethod::kGet:
					code = curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "GET");
					assert(code == CURLE_OK);
					break;
				case HttpRequestMethod::kPost:
					code = curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "POST");
					assert(code == CURLE_OK);
					break;
				case HttpRequestMethod::kPut:
					code = curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "PUT");
					assert(code == CURLE_OK);
					break;
				default:
					break;
				}
				return *this;
			}

			HttpRequest& setFormData(map<string, string> formDataMap, HtmlEncType enctype)
			{
				CURLcode code = CURLE_OK;
				struct curl_httppost* form = NULL;
				struct curl_httppost* last = NULL;

				m_formData = "";

				/* build form */

				switch (enctype)
				{
				case libcc::network::HtmlEncType::application_x_www_urlencoded:
				{
					for (auto i = formDataMap.begin(); i != formDataMap.end(); i++)
					{
						m_formData += (i == formDataMap.begin() ? "" : "&") + escape(i->first) + "=" + escape(i->second);
					}
					code = curl_easy_setopt(getHandle(), CURLOPT_POSTFIELDSIZE, m_formData.size());
					assert(code == CURLE_OK);
					code = curl_easy_setopt(getHandle(), CURLOPT_POSTFIELDS, m_formData.data());
					assert(code == CURLE_OK);
					break;
				}
				case libcc::network::HtmlEncType::multipart_form_data:
					for (auto i = formDataMap.begin(); i != formDataMap.end(); i++)
					{
						curl_formadd(&form, &last, CURLFORM_COPYNAME, i->first.c_str(), CURLFORM_COPYCONTENTS, i->second.c_str(), CURLFORM_END);
					}
					code = curl_easy_setopt(getHandle(), CURLOPT_HTTPPOST, form);
					assert(code == CURLE_OK);
					break;
				case libcc::network::HtmlEncType::text_plain:
					for (auto i = formDataMap.begin(); i != formDataMap.end(); i++)
					{
						m_formData += (i == formDataMap.begin() ? "" : "&") + i->first + "=" + i->second;
					}
					code = curl_easy_setopt(getHandle(), CURLOPT_POSTFIELDSIZE, m_formData.size());
					assert(code == CURLE_OK);
					code = curl_easy_setopt(getHandle(), CURLOPT_POSTFIELDS, m_formData.data());
					assert(code == CURLE_OK);
					break;
				default:
					throw;
				}
			}

			HttpRequest& setPostFields(string postFields)
			{
				m_postFields = postFields;
				CURLcode code = curl_easy_setopt(m_curl, CURLOPT_POSTFIELDSIZE, m_postFields.size());
				assert(code == CURLE_OK);
				code = curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, m_postFields.data());
				assert(code == CURLE_OK);
				return *this;
			}


			/**
			  * 对内容进行转码;对curl_easy_escape函数的一层封装
			  *
			  * @param content 要转码的内容
			  */
			static string escape(string content)
			{
				char* e_content = curl_easy_escape(nullptr, content.c_str(), content.length());
				std::string result = string(e_content);
				curl_free(e_content);
				return result;
			}
			static string unescape(string content)
			{
				int len;
				char* e_content = curl_easy_unescape(nullptr, content.c_str(), content.length(), &len);
				std::string result = string(e_content, len);
				curl_free(e_content);
				return result;
			}

			void setUseMultiApi(bool value)
			{
				m_useMultiApi = value;
			}

		protected:

			CURL* getHandle()
			{
				return m_curl;
			}

		private:
			void setFinishedCallback(ResponseCallback callback)
			{
				m_responseCallback = callback;
			}

			void onFinish()
			{
				if (m_responseCallback != nullptr)
				{
					m_responseCallback(m_response);
				}
			}

		private:
			static size_t writeCallback(void* data, size_t size, size_t nmemb, void* userData)
			{
				auto httpRequest = reinterpret_cast<HttpRequest*>(userData);
				long long total = size * nmemb;
				if (total != 0)
				{
					httpRequest->m_response.setContent(httpRequest->m_response.getContent() + string((char*)data, total));
				}
				else
					return -1;
				return size * nmemb;
			}
			static size_t headerCallback(void* data, size_t size, size_t nmemb, void* userData)
			{
				auto httpRequest = reinterpret_cast<HttpRequest*>(userData);
				const size_t total = size * nmemb;
				if (total != 0) {
					char* header = (char*)malloc(total + 1);
					char* value = NULL;
					char* p = NULL;
					memcpy(header, data, total);
					header[total] = '\0';
					if (strlen(header))
					{
						httpRequest->m_response.setHeader(httpRequest->m_response.getHeader() + header);
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
						map<string, string> map = httpRequest->m_response.getHeaderMap();
						map.insert(std::pair<std::string, std::string>(header, value));
						httpRequest->m_response.setHeaderMap(map);
					}
					free(header);
				}
				return total;
			}

			void setHttpStatus(HttpStatus status)
			{
				m_status = status;
				m_response.setStatus(status);
			}
			void setPrivatePointer(void* pointer)
			{
				curl_easy_setopt(m_curl, CURLOPT_PRIVATE, pointer);
			}


			/**
			 * 查询系统代理
			 *
			 * @return libcc::network::string:
			 */
			string querySystemProxy()
			{
				string proxy;
				WINHTTP_CURRENT_USER_IE_PROXY_CONFIG proxyConfig;

				if (WinHttpGetIEProxyConfigForCurrentUser(&proxyConfig))
				{
					if (proxyConfig.lpszProxy != NULL)
					{
						proxy = string(_bstr_t(proxyConfig.lpszProxy));
						GlobalFree(proxyConfig.lpszProxy);
					}
					else if (proxyConfig.lpszAutoConfigUrl != NULL)
					{
						HINTERNET session = WinHttpOpen(NULL, WINHTTP_ACCESS_TYPE_NO_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
						if (session)
						{
							WINHTTP_PROXY_INFO proxyInfo;
							WINHTTP_AUTOPROXY_OPTIONS options;
							options.fAutoLogonIfChallenged = TRUE;
							options.dwFlags = WINHTTP_AUTOPROXY_CONFIG_URL;
							options.lpszAutoConfigUrl = proxyConfig.lpszAutoConfigUrl;
							options.dwAutoDetectFlags = 0;
							options.lpvReserved = NULL;
							options.dwReserved = 0;

							if (WinHttpGetProxyForUrl(session, L"http://www.github.com", &options, &proxyInfo))
							{
								GlobalFree(proxyConfig.lpszProxy);
								GlobalFree(proxyInfo.lpszProxyBypass);
								proxy = string(_bstr_t(proxyInfo.lpszProxy));
							}
							GlobalFree(proxyConfig.lpszAutoConfigUrl);
							WinHttpCloseHandle(session);
						}

					}
				}
				return proxy;
			}


		private:
			friend class HttpRequestPool;

			Response m_response;
			CURL* m_curl;
			HttpStatus m_status;
			string m_url;
			string m_postFields;
			string m_formData;
			ResponseCallback m_responseCallback;
			bool m_useMultiApi;
		};

		class HttpRequestPool
		{
		public:
			HttpRequestPool()
			{
				m_curlm = curl_multi_init();
				assert(this->m_curlm != NULL);
			}
			~HttpRequestPool()
			{
				// 注意事项:
				// curl_multi_remove_handle应在清理任何easy_curl_handle前调用
				// curl_multi_cleanup应在所有curl_handle被释放后调用
				// see https://curl.se/libcurl/c/curl_multi_cleanup.html

				// 在我查阅文档之前,由HttpRequest在析构中调用curl_easy_cleanup,在HttpRequestPool的析构中调用curl_multi_cleanup,这样会产生两种崩溃:
				// 1.curl_easy_cleanup先被调用, 然后curl_multi_remove_handle才被调用
				// 2.curl_multi_cleanup先被调用, 然后curl_easy_cleanup才被调用
				// 为了避免这两种情况的发生, 同时又不影响HttpRequest模块的独立使用, 我在addRequest的时候设置一个标志位(useMultiApi), 用于区分是否使用了curl_multi系列api, 从而决定curl_easy_cleanup由谁来调用, 以保证清理顺序符合要求
				for (int i = 0; i < m_httpRequestVector.size(); i++)
				{
					curl_multi_remove_handle(m_curlm, m_httpRequestVector[i]->getHandle());
				}
				for (int i = 0; i < m_httpRequestVector.size(); i++)
				{
					curl_easy_cleanup(m_httpRequestVector[i]->getHandle());
				}
				curl_multi_cleanup(m_curlm);
			}

			HttpRequest& addRequest(HttpRequest* httpRequest, ResponseCallback callback = nullptr)
			{
				CURLMcode code;
				if (!m_proxy.empty())
					httpRequest->setProxy(m_proxy);
				httpRequest->setFinishedCallback(callback);
				httpRequest->setUseMultiApi(true);
				m_httpRequestVector.push_back(httpRequest);
				HttpRequest* request = m_httpRequestVector[m_httpRequestVector.size() - 1];
				request->setWriteCallback(HttpRequest::writeCallback, request);
				request->setHeaderCallback(HttpRequest::headerCallback, request);

				code = curl_multi_add_handle(m_curlm, request->m_curl);
				assert(code == CURLM_OK);
				request->setPrivatePointer(request);
				return *httpRequest;
			}

			int multiSelect(CURLM * curlm)
			{
				int ret = 0;

				struct timeval timeout_tv;
				fd_set  fd_read;
				fd_set  fd_write;
				fd_set  fd_except;
				int     max_fd = -1;

				// 注意这里一定要清空fdset,curl_multi_fdset不会执行fdset的清空操作  //  
				FD_ZERO(&fd_read);
				FD_ZERO(&fd_write);
				FD_ZERO(&fd_except);

				// 设置select超时时间  //  
				timeout_tv.tv_sec = 1;
				timeout_tv.tv_usec = 0;

				// 获取multi curl需要监听的文件描述符集合 fd_set //  
				curl_multi_fdset(curlm, &fd_read, &fd_write, &fd_except, &max_fd);

				/**
				 * When max_fd returns with -1,
				 * you need to wait a while and then proceed and call curl_multi_perform anyway.
				 * How long to wait? I would suggest 100 milliseconds at least,
				 * but you may want to test it out in your own particular conditions to find a suitable value.
				 */
				if (max_fd = -1)return -1;


				/**
				 * 执行监听，当文件描述符状态发生改变的时候返回
				 * 返回0，程序调用curl_multi_perform通知curl执行相应操作
				 * 返回-1，表示select错误
				 * 注意：即使select超时也需要返回0，具体可以去官网看文档说明
				 */
				int ret_code = ::select(max_fd + 1, &fd_read, &fd_write, &fd_except, &timeout_tv);
				switch (ret_code)
				{
				case -1:
					/* select error */
					ret = -1;
					break;
				case 0:
					/* select timeout */
				default:
					/* one or more of curl's file descriptors say there's data to read or write*/
					ret = 0;
					break;
				}
				return ret;
			}




			int perform()
			{
				int running_handles = 0;
				int numfds = 0;
				CURLMcode mc;
				mc = curl_multi_perform(m_curlm, &running_handles);
				while (running_handles)
				{
					bool waitContinue = true;
					if (m_asyncWaitCallback != nullptr)
						waitContinue = m_asyncWaitCallback();
					if (!waitContinue)
					{
						for (auto i = 0; i < m_httpRequestVector.size(); i++)
						{
							m_httpRequestVector[i]->setHttpStatus(HttpStatus::kStopedByUser);
						}
						return -1;
					}
					mc = curl_multi_perform(m_curlm, &running_handles);
					mc = curl_multi_wait(m_curlm, nullptr, 0, 200, &numfds);
					assert(mc == CURLM_OK);


					CURLMsg* msg = nullptr;
					int left = 0;
					while ((msg = curl_multi_info_read(m_curlm, &left)))
					{
						CURL* curlHandle = msg->easy_handle;
						CURLcode code = msg->data.result;

						HttpRequest* request = nullptr;
						code = curl_easy_getinfo(curlHandle, CURLINFO_PRIVATE, &request);
						assert(code == CURLE_OK);

						bool readContinue = true;
						if (m_asyncReadCallback != nullptr)
							readContinue = m_asyncReadCallback();
						if (!readContinue)
						{
							//request->setHttpStatus(HttpStatus::kStopedByUser);
							for (auto i = 0; i < m_httpRequestVector.size(); i++)
							{
								m_httpRequestVector[i]->setHttpStatus(HttpStatus::kStopedByUser);
							}
							return -1;
						}
						// msg->msg will always equals to CURLMSG_DONE.
						// CURLMSG_NONE and CURLMSG_LAST were not used.
						if (msg->msg != CURLMSG_DONE)
							continue;


						HttpStatus httpStatus;
						code = curl_easy_getinfo(curlHandle, CURLINFO_RESPONSE_CODE, &httpStatus);
						assert(code == CURLE_OK);

						request->setHttpStatus(httpStatus);
						if (request != nullptr)
						{
							request->onFinish();
						}
					}
				}
				return 0;


			}

			void clear()
			{

			}

			/**
			 * 给所有http实例设置代理
			 *
			 * @param string proxy:
			 * @return libcc::network::HttpRequestPool&:
			 */
			HttpRequestPool& setProxy(string proxy)
			{
				m_proxy = proxy;
				for (auto i = 0; i < m_httpRequestVector.size(); i++)
				{
					m_httpRequestVector[i]->setProxy(proxy);
				}
				return *this;
			}

			/**
			 * 设置异步等待回调,回调会在select之前调用
			 * 回调返回true则继续执行, false则中断并退出该次请求
			 */
			void setAsyncWaitCallback(AsyncWaitCallback asyncWaitCallback)
			{
				m_asyncWaitCallback = asyncWaitCallback;
			}

			/**
			 * 设置异步read message回调,回调会在curl_multi_info_read之后调用
			 * 回调返回true则继续执行, false则中断并退出该次请求
			 */
			void setAsyncReadCallback(AsyncReadCallback asyncReadCallback)
			{
				m_asyncReadCallback = asyncReadCallback;
			}


		private:
			CURLM* m_curlm;
			vector<HttpRequest*> m_httpRequestVector;
			string m_proxy;
			AsyncWaitCallback m_asyncWaitCallback = nullptr;
			AsyncReadCallback m_asyncReadCallback = nullptr;
		};


		class HttpGet : public HttpRequest
		{
		public:
			HttpGet(string url) :HttpRequest(url) {

				setRequestMethod(HttpRequestMethod::kGet);
				setUrl(url);
			};
			Response perform() { return this->get(); }
		};

		class HttpPost : public HttpRequest
		{
		public:
			HttpPost(string url, map<string, string> formDataMap, HtmlEncType enctype = HtmlEncType::multipart_form_data) :HttpRequest(url)
			{
				setRequestMethod(HttpRequestMethod::kPost);
				setUrl(url);
				setFormData(formDataMap, enctype);
			};
			HttpPost(string url, string data) :HttpRequest(url)
			{
				setRequestMethod(HttpRequestMethod::kPost);
				setUrl(url);
				setPostFields(data);
			};

			Response perform()
			{
				return this->post();
			}
		private:
		};

		class HttpPut : public HttpRequest
		{
		public:

			HttpPut(string url) :HttpRequest(url)
			{
				setRequestMethod(HttpRequestMethod::kPut);
				setUrl(url);
			};
			HttpPut(string url, string data) :HttpRequest(url)
			{
				setRequestMethod(HttpRequestMethod::kPut);
				setUrl(url);
				setPostFields(data);
			};

			Response perform()
			{
				return this->put();
			}
		private:
		};
	}
}
