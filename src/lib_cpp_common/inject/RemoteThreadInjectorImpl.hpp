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
		// ��ȡĿ�����id
		DWORD pid = ProcessImpl::getProcessIdByName(m_processName);

		// ��ȡĿ����̾��
		m_hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

		// ��Ŀ���������һ���㹻���dllFullName�Ŀռ�
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
		
		// ж��dll
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