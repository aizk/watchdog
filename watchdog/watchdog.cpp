// wadog.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <windows.h>    
#include <psapi.h>    

#include<iostream>    
using namespace std;

#pragma  once    
#pragma  message("Psapi.h --> Linking with Psapi.lib")    
#pragma  comment(lib,"Psapi.lib")    


bool cmpName_processID(TCHAR * pStrProcessName, DWORD processID)
{
	TCHAR szProcessName[MAX_PATH] = TEXT("");

	// Get a handle to the process.    
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID);

	// Get the process name.    
	if (NULL != hProcess)
	{
		HMODULE hMod;
		DWORD cbNeeded;

		if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
			&cbNeeded))
		{
			GetModuleBaseName(hProcess, hMod, szProcessName,
				sizeof(szProcessName) / sizeof(TCHAR));
		}
	}

	CloseHandle(hProcess);

	if (0 == _tcscmp(pStrProcessName, szProcessName))
	{
		return true;
	}

	return false;
}

void create_process(TCHAR * pStrProcessName)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if (!CreateProcess(pStrProcessName,   //module name  
		NULL,        // Command line  
		NULL,           // Process handle not inheritable  
		NULL,           // Thread handle not inheritable  
		FALSE,          // Set handle inheritance to FALSE  
		0,              // No creation flags  
		NULL,           // Use parent's environment block  
		NULL,           // Use parent's starting directory   
		&si,            // Pointer to STARTUPINFO structure  
		&pi)           // Pointer to PROCESS_INFORMATION structure  
		)
	{
		printf("CreateProcess failed (%d).\n", GetLastError());
		return;
	}

	// Close process and thread handles.   
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return;
}

void check_create_process(TCHAR * pStrProcessName)
{
	bool bExist = false;

	DWORD aProcesses[1024], nByteNeed, nProcesses;
	unsigned int i;

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &nByteNeed))
	{
		printf("获取所有进程失败\n");
		return;
	}

	// Calculate how many process identifiers were returned.    
	nProcesses = nByteNeed / sizeof(DWORD);

	for (i = 0; i < nProcesses; i++)
	{
		if (aProcesses[i] != 0)
		{
			if (cmpName_processID(pStrProcessName, aProcesses[i]))
			{
				if (bExist)
				{
					HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |   // Required by Alpha  
						PROCESS_TERMINATE,
						FALSE, aProcesses[i]);

					if (NULL != hProcess)
					{
						TerminateProcess(hProcess, 0);
						printf("杀死进程\n");
					}
				}
				else
				{
					bExist = true;
				}
			}
		}
	}

	if (bExist)return;

	create_process(pStrProcessName);
}



int _tmain(int argc, _TCHAR* argv[])
{
	char ch;
	//cin>>ch;    

	while (1)
	{
		Sleep(10 * 1000);

		check_create_process(_T("notepad.exe"));
	}
	cout << "start...";

	cin >> ch;

	return 0;
}

