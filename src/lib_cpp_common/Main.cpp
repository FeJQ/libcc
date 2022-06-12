
#include <string>
//#include "test/NetworkTest.h"
#include "test/ElementTest.h"
//#include "test/CryptoTest.h"
#include "test/HookTest.h"

#include "inject/RemoteThreadInjector.hpp"

#include <thread>

using namespace libcc;
using namespace libcc::test;


int main(int argc, char** argv)
{
	/*MessageBoxA(NULL,"开始注入",NULL,0);
	RemoteThreadInjector injector("iTunes.exe",LR"(C:\aid\aiddll.dll)");
	int result = injector.install();
	if (result != 0)
	{
		MessageBoxA(NULL, "注入失败", NULL, 0);
	}
	return 0 ;*/
	HookTest hookTest;
	hookTest.entry();

	List list = {0,1,2,3};
	for (const auto& i : list)
	{
		
	}
	//NetworktTest network;
	//network.entry();

	ElementTest element;
	element.entry();

	//CryptoTest crypto;
	//crypto.entry();


	getchar();
	return 0;
}

