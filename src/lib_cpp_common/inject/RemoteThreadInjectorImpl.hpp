#pragma once
#include <Windows.h>
#include "system/ProcessImpl.hpp"
class RemoteThreadInjectorImpl
{
public:
	RemoteThreadInjectorImpl(const char* processName, const wchar_t* dllFullName) {
		strcpy(m_processName, processName);
		lstrcpyW(m_dllFullName, dllFullName);
	}

	int install()
	{
		ProcessImpl::enableDebugPrivilege(GetCurrentProcess());
		// 获取目标进程id
		DWORD pid = ProcessImpl::getProcessIdByName(m_processName);

		// 获取目标进程句柄
		m_hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

		// 在目标进程申请一个足够存放dllFullName的空间
		m_injectedProcessDllFullName = VirtualAllocEx(m_hProcess, NULL, MAX_PATH, MEM_COMMIT, PAGE_READWRITE);

		if (!m_injectedProcessDllFullName)
		{
			return -1;		
		}
		if (!WriteProcessMemory(m_hProcess, m_injectedProcessDllFullName, m_dllFullName, (wcslen(m_dllFullName) + 1) * sizeof(wchar_t), NULL))
		{
			return -1;
		}
		HANDLE hRemoteThread = CreateRemoteThread(m_hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryW, m_injectedProcessDllFullName, 0, NULL);
		if (hRemoteThread == NULL) return -1;
		return 0;
	}

	int uninstall()
	{
		
		// 卸载dll
		if (m_hProcess != NULL)
		{
			CloseHandle(m_hProcess);
			m_hProcess = NULL;
		}
	}
private:
	char m_processName[MAX_PATH];
	wchar_t m_dllFullName[MAX_PATH];
	HANDLE m_hProcess = NULL;
	void* m_injectedProcessDllFullName = NULL;
};