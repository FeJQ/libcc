#pragma once
#include "test/BaseTest.h"
#include "hook/InlineHook.hpp"




InlineHook inlineHook;

typedef int(__stdcall *t_MessageBoxA)(
	_In_opt_ HWND hWnd,
	_In_opt_ LPCSTR lpText,
	_In_opt_ LPCSTR lpCaption,
	_In_ UINT uType);

int __stdcall proxy_MessageBoxA(
	_In_opt_ HWND hWnd,
	_In_opt_ LPCSTR lpText,
	_In_opt_ LPCSTR lpCaption,
	_In_ UINT uType)
{
	lpText = "张未希是傻子";
	return (reinterpret_cast<t_MessageBoxA>((inlineHook.originalFunction())))(hWnd, lpText, lpCaption, uType);
}


namespace libcc
{
	namespace test
	{

		class HookTest :public BaseTest
		{
		public:


			// 通过 BaseTest 继承
			virtual bool entry() override
			{
				inlineHook.hook(MessageBoxA, proxy_MessageBoxA);
				MessageBoxA(0, "张未希是帅哥", 0, 0);
				return true;
			}
		};
	}
}