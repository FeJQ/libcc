
#include <string>
//#include "test/NetworkTest.h"
#include "test/ElementTest.h"
//#include "test/CryptoTest.h"
#include "test/HookTest.h"

using namespace libcc;
using namespace libcc::test;

int main(int argc, char** argv)
{
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

