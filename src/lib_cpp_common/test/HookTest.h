#pragma once
#include "test/BaseTest.h"
#include "InlineHook.hpp"




InlineHook inlineHook;

typedef int  (__stdcall *t_MessageBoxA)(
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
	lpText = "��δϣ��";
	return ((t_MessageBoxA)((void*)inlineHook.tramplineReturn))(hWnd, lpText, lpCaption, uType);
}


namespace libcc
{
	namespace test
	{

		class HookTest :public BaseTest
		{
		public:
			

			// ͨ�� BaseTest �̳�
			virtual bool entry() override
			{
				if (sizeof(void*) == 4)
				{
					inlineHook.hook(MessageBoxA, 12, proxy_MessageBoxA);
				}
				else
				{
					inlineHook.hook(MessageBoxA, 14, proxy_MessageBoxA);
				}

				MessageBoxA(0,"��δϣ��˧��",0,0);
				return true;
			}
		};
	}
}