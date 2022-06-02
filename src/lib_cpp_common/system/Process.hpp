#pragma once
#include <Windows.h>
#include <TlHelp32.h>

class  EnumProcessCallback
{
public:
	/**
	 * 枚举到一个新的进程时触发
	 * @param PROCESSENTRY32 * pe32:进程信息结构体
	 * @return 是否继续枚举
	 */
	virtual bool onEnumProcess(PROCESSENTRY32* pe32) { return true; };
};

class EnumerateFindSpecifiedProcess :public EnumProcessCallback
{
public:
	EnumerateFindSpecifiedProcess(const char* processName, OUT PROCESSENTRY32* pe32)
	{
		if (processName != NULL)
		{
			strcpy(m_processName, processName);
			m_pe32 = pe32;
		}
	}
	bool onEnumProcess(PROCESSENTRY32* pe32) override {
		if (strcmp(pe32->szExeFile, m_processName) == 0)
		{
			memcpy(m_pe32, pe32, sizeof(PROCESSENTRY32));
			return false;
		}
	}
private:
	char m_processName[MAX_PATH];
	PROCESSENTRY32* m_pe32;
};


class Process
{
public:
	static int enumerateProcesses(EnumProcessCallback* enumProcessCallback)
	{
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(pe32);

		HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
		{
			return -1;
		}

		//首先获得第一个进程
		BOOL bProcess = Process32First(hProcessSnap, &pe32);
		//循环获得所有进程
		while (bProcess)
		{
			if (!enumProcessCallback->onEnumProcess(&pe32))
			{
				break;
			}
			bProcess = Process32Next(hProcessSnap, &pe32);
		}
		CloseHandle(hProcessSnap);
		return 0;
	}

	static DWORD getProcessIdByName(const char* processName)
	{
		PROCESSENTRY32 pe32;
		EnumerateFindSpecifiedProcess enumerateFindSpecifiedProcess(processName, &pe32);
		enumerateProcesses(&enumerateFindSpecifiedProcess);
		return pe32.th32ProcessID;
	}

	static int enableDebugPrivilege(HANDLE hProcess ,const char* privilegeName= SE_DEBUG_NAME)
	{
		HANDLE token;
		TOKEN_PRIVILEGES tp;
		//打开进程令牌环
		if (!OpenProcessToken(hProcess,TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token))
		{
			return -1;
		}
		//获得进程本地唯一ID
		LUID luid;
		if (!LookupPrivilegeValue(NULL, privilegeName, &luid))
		{
			return -1;
		}
		tp.PrivilegeCount = 1;
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		tp.Privileges[0].Luid = luid;
		//调整进程权限
		if (!AdjustTokenPrivileges(token, 0, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
		{
			return -1;
		}
		return 0;
	}
private:
	
	
};