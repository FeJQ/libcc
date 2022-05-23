#pragma once

#define x64

#ifdef KERNEL
#include <ntddk.h>
#define allocMemory(size) ExAllocatePoolWithTag(NonPagedPool,(size),'ilhk')
#else
#include <Windows.h>

#define allocMemory(size) malloc(size)
#define memoryCopy(dst,src,size) memcpy((dst),(src),(size))
#endif


#ifdef x86
// [+0] push address
// [+1] ret
static const unsigned char DETOUR_INST[] = { 0xB8,0x00,0x00,0x00,0x00,0xC3 };


// [+0] push eax
// [+1] mov eax,address
// [+6] jmp eax

//static const unsigned char DETOUR_INST[] = { 0x50,0x68,0x00,0x00,0x00,0x00,0xFF,0xE0 };
#elif defined x64

// [+0] mov rax,address
// [+10] jmp rax
//static const unsigned char DETOUR_INST[] = {0x48,0xB8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xE0 };

// [+0] push address.low
// [+5] mov dword ptr ss:[esp+4],address.high
// [+14] ret
static const unsigned char DETOUR_INST[14] = {
			0x68,0x00,0x00,0x00,0x00, //push proxyAddress.low
			0xC7,0x44,0x24,0x04,0x00,0x00,0x00,0x00, // mov dword ptr ss:[rsp+4],proxyAddress.high
			0xC3// ret		
};

// _x86指令最大长度 see https://bbs.pediy.com/thread-190127.htm
const int MAX_INSTUCTION_LEN = 16;
#endif 

__declspec(thread) CONTEXT tls_context = { 0 };

class Trampline
{
public:
	Trampline() {
		memcpy(tramplineStart, DETOUR_INST, sizeof(DETOUR_INST));
		memset(replacedInstruction, 0x90, sizeof(MAX_INSTUCTION_LEN));
		memcpy(tramplineReturn, DETOUR_INST, sizeof(DETOUR_INST));
	}

	// 原始函数中被替换的指令的长度
	int replacedInstSize;

	// 由原始函数跳转到tramplineStart，再跳转到代理函数
	unsigned char tramplineStart[sizeof(DETOUR_INST)];

	// 从代理函数跳转回replacedCode,执行完后,接着执行tramplineReturn,并跳转回原始函数
	unsigned char replacedInstruction[MAX_INSTUCTION_LEN];

	// 用于跳转回原始函数
	unsigned char tramplineReturn[sizeof(DETOUR_INST)];


	/**
	 * 获取原始函数被替换掉的指令的长度
	 * @return 长度
	 */
	int getReplacedInstSize()
	{
		return this->replacedInstSize;
	}

	/**
	 * 设置跳转到代理函数的地址
	 * @param ULONG_PTR address:地址
	 */
	void setTramplineStartAddress(PVOID address)
	{
		if (sizeof(PVOID) == 4)
		{
			/* 32bit */
			*(DWORD*)(this->tramplineStart + 1) = (DWORD)address;
		}
		else
		{
			/* 64bit */
			*(DWORD*)(this->tramplineStart + 1) = (DWORD)address; // address.low
			*(DWORD*)(this->tramplineStart + 9) = (DWORD)((unsigned char*)address + 4); // address.high
		}
	}

	void setReplacedInstruction(PVOID buffer, int size)
	{
		this->replacedInstSize = size;
		memoryCopy(this->replacedInstruction, buffer, size);
	}

	/**
	 * 设置返回到原始函数的地址
	 * @param ULONG_PTR address:地址
	 */
	void setTramplineReturnAddress(PVOID address)
	{
		if (sizeof(PVOID) == 4)
		{
			/* 32bit */
			*(DWORD*)(this->tramplineReturn + 1) = (DWORD)address;
		}
		else
		{
			/* 64bit */
			*(DWORD*)(this->tramplineReturn + 1) = (DWORD)address; // address.low
			*(DWORD*)(this->tramplineReturn + 9) = (DWORD)((unsigned char*)address + 4); // address.high
		}
	}
};


class InlineHook
{
public:
	bool hook(PVOID targetFunction, LONG instructionSize, PVOID proxyFunction, void*trampoline)
	{
		Trampline* trampline = (Trampline*)allocMemory(sizeof(Trampline));

		// 检查原始函数需要被替换的指令的长度

		if (instructionSize > (MAX_INSTUCTION_LEN + sizeof(DETOUR_INST)))
		{
			return false;
		}
		// 保存原始函数被替换掉的指令
		trampline->setReplacedInstruction(targetFunction, instructionSize);

		// 构造跳板中的跳转地址
		trampline->setTramplineStartAddress(proxyFunction);
		trampline->setTramplineReturnAddress((unsigned char*)targetFunction + trampline->getReplacedInstSize());


		// 保存上下文环境到tls
		//GetThreadContext(GetCurrentThread(), &tls_context);
	}

	NTSTATUS unhook(void*trampline)
	{

	}
};