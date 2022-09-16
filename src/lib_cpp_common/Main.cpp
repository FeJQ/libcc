#include <string>
#include <thread>
#include "test/BaseTest.h"

//#define ELEMENT_TEST
//#define CRYPTO_TEST
#define NETWORK_TEST
//#define HOOK_TEST
//#define INJECT_TEST

#ifdef ELEMENT_TEST
#include "test/ElementTest.h"
using namespace libcc::
#endif

#ifdef CRYPTO_TEST
#include "test/CryptoTest.h"
#endif

#ifdef NETWORK_TEST
#include "test/NetworkTest.h"
#endif

#ifdef HOOK_TEST
#include "test/HookTest.h"
#endif

#ifdef INJECT_TEST
#include "inject/RemoteThreadInjector.hpp"
#endif

using namespace libcc::test;

int main(int argc, char** argv)
{

#ifdef ELEMENT_TEST
	ElementTest element;
	element.entry();
#endif

#ifdef CRYPTO_TEST
	CryptoTest crypto;
	crypto.entry();
#endif

#ifdef NETWORK_TEST
	NetworktTest network;
	network.entry();
#endif
	
#ifdef HOOK_TEST
	HookTest hookTest;
	hookTest.entry();
#endif 

#ifdef INJECT_TEST
	MessageBoxA(NULL, "开始注入", NULL, 0);
	RemoteThreadInjector injector("iTunes.exe", LR"(C:\aid\aiddll.dll)");
	int result = injector.install();
	if (result != 0)
	{
		MessageBoxA(NULL, "注入失败", NULL, 0);
	}
#endif 




	

	

	


	getchar();
	return 0;
}

