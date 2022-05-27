#pragma once



#include <intrin.h>
#include <assert.h>

#ifdef KERNEL
#include <ntddk.h>
#define allocMemory(size) ExAllocatePoolWithTag(NonPagedPool,(size),'ilhk')
#else
#include <Windows.h>

#define allocMemory(size) malloc(size)
#define memoryCopy(dst,src,size) memcpy((dst),(src),(size))
#endif

// _x86指令最大长度 see https://bbs.pediy.com/thread-190127.htm
const int MAX_INSTUCTION_LEN = 16;

#ifndef _WIN64
// [+0] push address
// [+1] ret
static const unsigned char DETOUR_INST[] = { 0x68,0x00,0x00,0x00,0x00,0xC3 };


// [+0] mov eax,address
// [+5] jmp eax

//static const unsigned char DETOUR_INST[] = {0xB8,0x00,0x00,0x00,0x00,0xFF,0xE0 };
#else

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
#endif 

__declspec(thread) CONTEXT tls_context = { 0 };

//关闭内存写保护
ULONG disableMemProtect(LPVOID lpAddr, ULONG uSize)
{
	ULONG uOld = 0;
#ifdef KERNEL
	__asm
	{
		push eax;
		mov eax, cr0;
		and eax, ~0x10000;
		mov cr0, eax;
		pop eax;
	}
	KIRQL irql;
	irql = KeRaiseIrqlToDpcLevel();
	GainExlusivity();
	uOld = (ULONG)irql;
#else
	VirtualProtect(lpAddr, uSize, PAGE_EXECUTE_READWRITE, &uOld);
#endif 
	return uOld;
}

//打开内存写保护
void enableMemProtect(LPVOID lpAddr, ULONG uSize, ULONG uOldValue)
{
#ifdef KERNEL
	ReleaseExclusivity();
	KIRQL oldIrql = (KIRQL)uOldValue;
	KeLowerIrql(oldIrql);
	__asm
	{
		push eax;
		mov eax, cr0;
		or eax, 0x10000;
		mov cr0, eax;
		pop eax;
	}

#else
	VirtualProtect(lpAddr, uSize, uOldValue, &uOldValue);
#endif 
}


class InlineHook
{
public:
	InlineHook()
	{
		disableMemProtect(this->tramplineStart, sizeof(DETOUR_INST));
		disableMemProtect(this->tramplineReturn, MAX_INSTUCTION_LEN + sizeof(DETOUR_INST) + sizeof(DETOUR_INST));

		memcpy(this->tramplineStart, DETOUR_INST, sizeof(DETOUR_INST));
		memset(this->tramplineReturn, 0x90, MAX_INSTUCTION_LEN + sizeof(DETOUR_INST));
		memcpy(this->tramplineReturn + MAX_INSTUCTION_LEN + sizeof(DETOUR_INST), DETOUR_INST, sizeof(DETOUR_INST));	
	}


	bool hook(PVOID targetFunction, LONG instructionSize, PVOID proxyFunction)
	{
		// 检查原始函数需要被替换的指令的长度
		if (instructionSize > (MAX_INSTUCTION_LEN + sizeof(DETOUR_INST)))
		{
			return false;
		}

		// 保存原始函数被替换掉的指令
		this->setReplacedInstruction(targetFunction, instructionSize);

		// 构造跳板中的跳转地址
		this->setTramplineStartAddress(proxyFunction);
		this->setTramplineReturnAddress((unsigned char*)targetFunction + this->getReplacedInstSize());

		// 替换原始函数的指令
		ULONG oldProtect = disableMemProtect(targetFunction, sizeof(this->tramplineStart));
		memoryCopy(targetFunction, this->tramplineStart, sizeof(this->tramplineStart));
		enableMemProtect(targetFunction, sizeof(this->tramplineStart), oldProtect);
		//this->originalFunction = (unsigned char*)targetFunction + this->replacedInstSize;

		// 保存上下文环境到tls
		//GetThreadContext(GetCurrentThread(), &tls_context);

		return true;
	}

	NTSTATUS unhook(void*trampline)
	{
		return 0;
	}

private:
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
			*(DWORD*)(this->tramplineStart + 9) = (ULONG64)(address)>>32; // address.high
		}
	}

	void setReplacedInstruction(PVOID buffer, int size)
	{
		this->replacedInstSize = size;
		assert(size <= MAX_INSTUCTION_LEN + sizeof(DETOUR_INST));
		memoryCopy(this->tramplineReturn, buffer, size);
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
			*(DWORD*)(this->tramplineReturn + MAX_INSTUCTION_LEN + sizeof(DETOUR_INST) + 1) = (DWORD)address;
		}
		else
		{
			/* 64bit */
			*(DWORD*)(this->tramplineReturn + MAX_INSTUCTION_LEN + sizeof(DETOUR_INST) + 1) = (DWORD)address; // address.low
			*(DWORD*)(this->tramplineReturn + MAX_INSTUCTION_LEN + sizeof(DETOUR_INST) + 9) = (ULONG64)(address) >> 32; // address.high
		}
	}

private:
	// 原始函数中被替换的指令的长度
	int replacedInstSize;

	// 由原始函数跳转到tramplineStart，再跳转到代理函数
	unsigned char tramplineStart[sizeof(DETOUR_INST)];

public:
	// 从代理函数跳转回replacedCode
	// 前MAX_INSTUCTION_LEN+sizeof(DETOUR_INST)字节存放被替换的指令
	// 后sizeof(DETOUR_INST)字节存放跳转到原函数的指令
	unsigned char tramplineReturn[MAX_INSTUCTION_LEN + sizeof(DETOUR_INST) + sizeof(DETOUR_INST)];

};