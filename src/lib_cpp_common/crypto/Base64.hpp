#pragma once
#include <string>
using namespace std;
namespace libcc
{
	namespace crypto
	{
		class Base64
		{
		public:
			static string encode(string inputText)
			{
				size_t size = inputText.size();
				if (size <= 0) return NULL;
				int outlen = (int)(size / 3) * 4;
				//char *outbuf = (char*)malloc(outlen + 5); // 4 spare bytes + 1 for '\0'
				string encodeText;
				encodeText.resize(outlen + 5);
				size_t n = 0;
				size_t m = 0;
				unsigned char input[3];
				unsigned int output[4];
				while (n < size) {
					input[0] = inputText[n];
					input[1] = (n + 1 < size) ? inputText[n + 1] : 0;
					input[2] = (n + 2 < size) ? inputText[n + 2] : 0;
					output[0] = input[0] >> 2;
					output[1] = ((input[0] & 3) << 4) + (input[1] >> 4);
					output[2] = ((input[1] & 15) << 2) + (input[2] >> 6);
					output[3] = input[2] & 63;
					encodeText[m++] = getBase64Str()[(int)output[0]];
					encodeText[m++] = getBase64Str()[(int)output[1]];
					encodeText[m++] = (n + 1 < size) ? getBase64Str()[(int)output[2]] : getBase64Pad();
					encodeText[m++] = (n + 2 < size) ? getBase64Str()[(int)output[3]] : getBase64Pad();
					n += 3;
				}
				encodeText[m] = 0; // 0-termination!
				encodeText.resize(m);
				return encodeText;
			}
			static string decode(string inputText)
			{
				size_t size = inputText.size();
				if (size <= 0) return NULL;

				string decodeText;
				decodeText.resize((size / 4) * 3 + 3);
				unsigned char* line;
				int p = 0;
				line = (unsigned char*)strtok((char*)inputText.data(), "\r\n\t ");
				while (line) {
					p += decodeBlock((unsigned char*)decodeText.data() + p, (const char*)line, strlen((char*)line));
					// get next line of base64 encoded block
					line = (unsigned char*)strtok(NULL, "\r\n\t ");
				}
				decodeText[p] = 0;
				decodeText.resize(p);
				return decodeText;
			}
		private:
			static int decodeBlock(unsigned char* target, const char* data, size_t data_size)
			{
				int w1, w2, w3, w4, i;
				size_t n;
				if (!data || (data_size <= 0)) {
					return 0;
				}
				n = 0;
				i = 0;
				while (n < data_size - 3) {
					w1 = getBase64Table()[(int)data[n]];
					w2 = getBase64Table()[(int)data[n + 1]];
					w3 = getBase64Table()[(int)data[n + 2]];
					w4 = getBase64Table()[(int)data[n + 3]];

					if (w2 >= 0)
						target[i++] = (char)((w1 * 4 + (w2 >> 4)) & 255);
					if (w3 >= 0)
						target[i++] = (char)((w2 * 16 + (w3 >> 2)) & 255);
					if (w4 >= 0)
						target[i++] = (char)((w3 * 64 + w4) & 255);
					n += 4;
				}
				return i;
			}
			static const char* getBase64Str()
			{
				//局部作用域内静态变量
				static const char* base64Str = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
				return base64Str;
			}
			static const signed char* getBase64Table()
			{
				static const signed char base64Table[256] = {
					-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
					-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
					-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
					52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
					-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
					15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
					-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
					41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
					-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
					-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
					-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
					-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
					-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
					-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
					-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
					-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
				};
				return base64Table;
			}

			static const char getBase64Pad()
			{
				static const char base64Pad = '=';
				return base64Pad;
			}



		private:

			
		};
	}
}