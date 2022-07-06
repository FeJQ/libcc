#pragma once
#include <string>

using namespace std;

namespace libcc
{
	namespace crypto
	{
		enum class PaddingMode
		{
			NoPadding,
			PKCS5,
			PKCS7,
			ISO_10126,
			ANSI_X9_23,
			ZerosPadding
		};

		class PaddingImpl
		{

		public:
			static string pad(string text, int alignSize, PaddingMode mode = PaddingMode::PKCS7)
			{
				if (text.empty()) return text;
				switch (mode)
				{
				case PaddingMode::NoPadding:return text;
				case PaddingMode::PKCS5:break;
				case PaddingMode::PKCS7: return PKCS7Padding(text, alignSize);
				case PaddingMode::ISO_10126:break;
				case PaddingMode::ANSI_X9_23:break;
				case PaddingMode::ZerosPadding:break;
				default:
					abort();
					break;
				}
				return text;
			}
			static int length(int dataLen, int alignSize, PaddingMode mode = PaddingMode::PKCS7)
			{
				switch (mode)
				{
				case PaddingMode::NoPadding:return dataLen;
				case PaddingMode::PKCS5:break;
				case PaddingMode::PKCS7: return getPKCS7PaddedLength(dataLen, alignSize);
				case PaddingMode::ISO_10126:break;
				case PaddingMode::ANSI_X9_23:break;
				case PaddingMode::ZerosPadding:break;
				default:
					abort();
					break;
				}
				return dataLen;
			}
			static string unpad(string text, PaddingMode mode = PaddingMode::PKCS7)
			{
				if (text.empty()) return text;
				switch (mode)
				{
				case PaddingMode::NoPadding:return text;
				case PaddingMode::PKCS5:break;
				case PaddingMode::PKCS7: return PKCS7UnPadding(text);
				case PaddingMode::ISO_10126:break;
				case PaddingMode::ANSI_X9_23:break;
				case PaddingMode::ZerosPadding:break;
				default:
					abort();
					break;
				}
				return text;
			}
		private:
			static int getPKCS7PaddedLength(int dataLen, int alignSize)
			{
				// 计算填充的字节数（按alignSize字节对齐进行填充）
				int remainder = dataLen % alignSize;
				int paddingSize = (remainder == 0) ? alignSize : (alignSize - remainder);
				return (dataLen + paddingSize);
			}
			static string PKCS7Padding(string text, int alignSize)
			{
				// 计算需要填充字节数（按alignSize字节对齐进行填充）
				int remainder = text.size() % alignSize;
				int paddingSize = (remainder == 0) ? alignSize : (alignSize - remainder);

				// 进行填充
				text.append(paddingSize, paddingSize);
				return text;
			}
			static string PKCS7UnPadding(string text)
			{
				char paddingSize = text[(text.size() - 1)];
				text = text.substr(0, text.size() - paddingSize);
				return text;
			}
		};

	}
}