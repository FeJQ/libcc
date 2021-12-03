

#include "test/NetworkTest.h"
#include "test/ElementTest.h"
#include "test/CryptoTest.h"

using namespace libcc;
using namespace libcc::test;

int main(int argc, char** argv)
{

	NetworktTest network;
	network.entry();

	ElementTest element;
	element.entry();

	CryptoTest crypto;
	crypto.entry();


	getchar();
	return 0;
}

