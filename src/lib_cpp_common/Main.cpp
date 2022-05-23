

#include "test/NetworkTest.h"
#include "test/ElementTest.h"
//#include "test/CryptoTest.h"
#include "InlineHook.hpp"

using namespace libcc;
using namespace libcc::test;

int main(int argc, char** argv)
{
	InlineHook inlineHook;
	inlineHook.hook(0,0,0,0);

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

