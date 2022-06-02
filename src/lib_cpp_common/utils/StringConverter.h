/************************************************
* transfer in character ansi, utf8, unicode
* @author rene/2011-5-13
*
* @notice if you use it into something mfc application, you will make sure to include it under these mcf's headers.
* or else, the application can't be compiled with a error message as below:
*    "fatal error C1189: #error :  WINDOWS.H already included.  MFC apps must not #include <windows.h>"
* for example: at least
*    #include <afxwin.h>;
*    #include <afxcmn.h>;
* and so on, and then
*    #include "StringConverter.h"
*
* if you use precompiled file, normally
*    #include "stdafx.h"
* and you can't also use it freely including it any where that you wanna
*
* @demo
std::string szAnsi = "abc123你我他";

std::wstring szUnicode = CStringConverter::Ansi2Unicode(szAnsi);
szAnsi = CStringConverter::Unicode2Ansi(szUnicode);

std::string szUft8 = CStringConverter::Unicode2Utf8(szUnicode);
szUnicode = CStringConverter::Utf82Unicode(szUft8);

szAnsi = CStringConverter::Utf82Ansi(szUft8);
szUft8 = CStringConverter::Ansi2Utf8(szAnsi);
************************************************/
#pragma once

//right now, it's suitable for either windows or mac automatically, no matter vc, qt and cocoa. so don't need to open or close this macro any more.
//#define USING_QT

#include <string>
#ifdef USING_QT
#include <qstring.h>
#elif defined WIN32
#include <wtypes.h>
#else
#include <locale.h>
#include <stdlib.h>
#endif

class CStringConverter {
public:
	static std::wstring Ansi2Unicode(std::string szAnsi) {
#ifdef USING_QT
		size_t len = szAnsi.size();
		QString szQs = QString::fromLocal8Bit(szAnsi.c_str(), len);
		wchar_t* szUnicode = new wchar_t[len + 1];
		len = szQs.toWCharArray(szUnicode);
		std::wstring rs;
		rs.append(szUnicode, len);
		delete[] szUnicode;
		return rs;
#elif defined WIN32
		//calc block size to be returned
		int len = MultiByteToWideChar(CP_ACP, NULL, szAnsi.c_str(), szAnsi.size(), NULL, 0);
		//malloc and fill the returned block
		wchar_t* szUnicode = new wchar_t[len + 1];
		MultiByteToWideChar(CP_ACP, NULL, szAnsi.c_str(), szAnsi.size(), szUnicode, len);
		szUnicode[len] = 0;
		//std::wstring rs = szUnicode;
		std::wstring rs;
		rs.append(szUnicode, len);
		delete[] szUnicode;
		
		return rs;
#else
		//the mbcs in mac is also utf8.
		return Utf82Unicode(szAnsi);
#endif
	}
	static std::string Unicode2Ansi(std::wstring szUnicode) {
#ifdef USING_QT
		QString szQs = QString::fromWCharArray(szUnicode.c_str(), szUnicode.size());
		QByteArray szAnsi = szQs.toLocal8Bit();
		std::string rs;
		rs.append(szAnsi.constData(), szAnsi.size());
		return rs;
#elif defined WIN32
		//calc block size to be returned
		int len = WideCharToMultiByte(CP_ACP, NULL, szUnicode.c_str(), szUnicode.size(), NULL, 0, NULL, NULL);
		//malloc and fill the returned block
		char* szAnsi = new char[len + 1];
		WideCharToMultiByte(CP_ACP, NULL, szUnicode.c_str(), szUnicode.size(), szAnsi, len, NULL, NULL);
		szAnsi[len] = 0;
		//std::string rs = szAnsi;
		std::string rs;
		rs.append(szAnsi, len);
		delete[] szAnsi;
		
		return rs;
#else
		//the mbcs in mac is also utf8.
		return Unicode2Utf8(szUnicode);
#endif
	}
	
	static std::wstring Utf82Unicode(std::string szUtf8) {
#ifdef USING_QT
		size_t len = szUtf8.size();
		QString szQs = QString::fromUtf8(szUtf8.c_str(), len);
		wchar_t* szUnicode = new wchar_t[len + 1];
		len = szQs.toWCharArray(szUnicode);
		std::wstring rs;
		rs.append(szUnicode, len);
		delete[] szUnicode;
		return rs;
#elif defined WIN32
		//calc block size to be returned
		int len = MultiByteToWideChar(CP_UTF8, NULL, szUtf8.c_str(), szUtf8.size(), NULL, 0);
		//malloc and fill the returned block
		wchar_t* szUnicode = new wchar_t[len + 1];
		MultiByteToWideChar(CP_UTF8, NULL, szUtf8.c_str(), szUtf8.size(), szUnicode, len);
		szUnicode[len] = 0;
		//std::wstring rs = szUnicode;
		std::wstring rs;
		rs.append(szUnicode, len);
		delete[] szUnicode;
		
		return rs;
#else
		//setlocale(LC_ALL, "UTF-8");
		setlocale(LC_CTYPE, "UTF-8");
		std::wstring rs;
		//mbstowcs or wcstombs converting the string to the NULL but not with a length. so must invoke it cyclically.
		while(true) {
			//calc block size to be returned
			size_t len = mbstowcs(0, szUtf8.c_str(), 0);
			//malloc and fill the returned block
			wchar_t* szUnicode = new wchar_t[len + 1];
			mbstowcs(szUnicode, szUtf8.c_str(), len);
			szUnicode[len] = 0;
			//std::wstring rs = szUnicode;
		
			rs.append(szUnicode, len);
			delete[] szUnicode;
		
			size_t pos = szUtf8.find('\0');
			if(pos == std::string::npos) break;
			rs.append(1, 0);
			szUtf8 = szUtf8.substr(pos + 1);
		}
		
		return rs;
#endif
	}
	static std::string Unicode2Utf8(std::wstring szUnicode) {
#ifdef USING_QT
		QString szQs = QString::fromWCharArray(szUnicode.c_str(), szUnicode.size());
		QByteArray szUtf8 = szQs.toUtf8();
		std::string rs;
		rs.append(szUtf8.constData(), szUtf8.size());
		return rs;
#elif defined WIN32
		//calc block size to be returned
		int len = WideCharToMultiByte(CP_UTF8, NULL, szUnicode.c_str(), szUnicode.size(), NULL, 0, NULL, NULL);
		//malloc and fill the returned block
		char* szUtf8 = new char[len + 1];
		WideCharToMultiByte(CP_UTF8, NULL, szUnicode.c_str(), szUnicode.size(), szUtf8, len, NULL, NULL);
		szUtf8[len] = 0;
		//std::string rs = szUtf8;
		std::string rs;
		rs.append(szUtf8, len);
		delete[] szUtf8;
		
		return rs;
#else
		//setlocale(LC_ALL, "UTF-8");
		setlocale(LC_CTYPE, "UTF-8");
		std::string rs;
		//mbstowcs or wcstombs converting the string to the NULL but not with a length. so must invoke it cyclically.
		while(true) {
			//calc block size to be returned
			size_t len = wcstombs(0, szUnicode.c_str(), 0);
			//malloc and fill the returned block
			char* szUtf8 = new char[len + 1];
			wcstombs(szUtf8, szUnicode.c_str(), len);
			szUtf8[len] = 0;
			//std::string rs = szUtf8;
			rs.append(szUtf8, len);
			delete[] szUtf8;
		
			size_t pos = szUnicode.find(L'\0');
			if(pos == std::wstring::npos) break;
			rs.append(1, 0);
			szUnicode = szUnicode.substr(pos + 1);
		}
		
		return rs;
#endif
	}
	
	static std::string Ansi2Utf8(std::string szAnsi) {
		return Unicode2Utf8(Ansi2Unicode(szAnsi));
	}
	static std::string Utf82Ansi(std::string szUtf8) {
		return Unicode2Ansi(Utf82Unicode(szUtf8));
	}
};
