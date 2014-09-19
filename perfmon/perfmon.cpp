// perfmon.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include <iostream>
#include <locale.h>
#include <windows.h>
#include <string.h>
#include <TlHelp32.h>
#include <vector>
#pragma  warning(disable:4996)

#define MAX_STR_LEN 256
#define CMD_TOKEN_NUM 10
#define MAX_BUF_SIZE 256
#define MAX_STR_NUM 256
#define MAX_HISTORY 256

//TCHAR ERROR_CMD[]
//= _T("'%s'��(��) ������ �� �ִ� ���α׷��� �ƴմϴ�.\n");

TCHAR* cmdName = NULL;
TCHAR cmdString[MAX_STR_LEN] = { 0, };
TCHAR cmdTokenList[CMD_TOKEN_NUM][MAX_STR_LEN];
TCHAR seps[] = _T(" ,\t\n");
TCHAR cmdHistory[MAX_HISTORY][MAX_STR_LEN];
int historyHead = 0;

int CmdProcessing();
TCHAR* StrLower(TCHAR*);


int _tmain(int argc, _TCHAR* argv[])
{
	DWORD isExit = FALSE;
	//�ѱ� �Է��� ������ �ϱ� ����.
	_tsetlocale(LC_ALL, _T("Korean"));
	cmdName = argv[0];
	if (argc > 1)
	{
		for (int i = 1; i < argc; i++)
		{
			_tcscat(cmdString, argv[i]);
			_tcscat(cmdString, _T(" "));
		}
		isExit = CmdProcessing();
	}

	
	while (isExit == FALSE)
	{
		_fputts(_T("My shell>> "), stdout);
		_getts(cmdString);
		isExit = CmdProcessing();
	}
	_fputts(_T("��ɾ� ó���� �����մϴ�.\n"), stdout);
	return 0;
}


int CmdProcessing()
{
	TCHAR cmdBuffer[MAX_STR_LEN] = { 0, };
	

	if (cmdString[0] == '!')
	{
		TCHAR* cmdStr = cmdString + 1;
		if (cmdStr[0] == '!')
		{
			_tcscpy(cmdString, cmdHistory[(historyHead - 1 + MAX_HISTORY) % MAX_HISTORY]);
		}
		else
		{
			for (int i = (historyHead - 1 + MAX_HISTORY) % MAX_HISTORY
				; i != historyHead && cmdHistory[i][0]
				; i = (i - 1 + MAX_HISTORY) % MAX_HISTORY)
			{
				TCHAR tmp[MAX_STR_LEN] = { 0, };
				_tcscpy(tmp, cmdHistory[i]);
				if (!_tcscmp(cmdStr, _tcstok(tmp, seps)))
				{
					_tcscpy(cmdString, cmdHistory[i]);
					break;
				}
			}
		}
	}

	_tcscpy(cmdBuffer, cmdString);
	_tcscpy(cmdHistory[historyHead], cmdString);
	historyHead = (historyHead + 1) % MAX_HISTORY;

	TCHAR* token = _tcstok(cmdString, seps);

	int tokenNum = 0;
	while (token != NULL)
	{
		_tcscpy(
			cmdTokenList[tokenNum++], StrLower(token)
			);
		token = _tcstok(NULL, seps);
	}
	for (int i = 0; i < _tcslen(cmdBuffer); ++i)
	{
		if (cmdBuffer[i] == '|')
		{
			TCHAR strLeft[MAX_STR_LEN] = { 0, };
			_tcsncpy(strLeft, cmdBuffer, i);
			TCHAR* strRight = cmdBuffer + i + 1;
			while (strRight[0] == ' ') ++strRight;

			HANDLE readPipe;
			HANDLE writePipe;

			SECURITY_ATTRIBUTES pipesa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
			CreatePipe(&readPipe, &writePipe, &pipesa, 0);

			STARTUPINFO si = { 0, };
			PROCESS_INFORMATION piLeft;

			si.cb = sizeof(si);
			si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
			si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
			si.hStdOutput = writePipe;
			si.dwFlags = STARTF_USESTDHANDLES;
			CreateProcess(NULL, strLeft, NULL, NULL, TRUE, 0, NULL, NULL, &si, &piLeft);
			CloseHandle(piLeft.hThread);
			CloseHandle(writePipe);

			PROCESS_INFORMATION piRight;

			si.cb = sizeof(si);
			si.hStdInput = readPipe;
			si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
			si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
			si.dwFlags = STARTF_USESTDHANDLES;
			CreateProcess(NULL, strRight, NULL, NULL, TRUE, 0, NULL, NULL, &si, &piRight);
			CloseHandle(piRight.hThread);
			CloseHandle(readPipe);

			WaitForSingleObject(piLeft.hProcess, INFINITE);
			WaitForSingleObject(piRight.hProcess, INFINITE);

			CloseHandle(piLeft.hProcess);
			CloseHandle(piRight.hProcess);
			return 0;
		}
	}

	if (!_tcscmp(cmdTokenList[0], _T("exit")))
	{
		return TRUE;
	}
	else if (!_tcscmp(cmdTokenList[0], _T("pwd")))
	{
		TCHAR buffer[MAX_STR_LEN] = { 0, };
		GetCurrentDirectory(MAX_STR_LEN, buffer);
		_putts(buffer);
	}
	else if (!_tcscmp(cmdTokenList[0], _T("echo")))
	{
		_putts(cmdBuffer + 5);
	}
	else if (!_tcscmp(cmdTokenList[0], _T("start")))
	{
		STARTUPINFO si = { 0, };
		PROCESS_INFORMATION pi;
		TCHAR startString[MAX_STR_LEN] = { 0, };

		_tcscat(startString, cmdName);
		_tcscat(startString, cmdBuffer + 5);
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		if (CreateProcess(NULL, startString, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
		{
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
		else
		{
			_putts(_T("Error"));
		}
	}
	else if (!_tcscmp(cmdTokenList[0], _T("list")))
	{
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hSnapshot == INVALID_HANDLE_VALUE)
		{
			_putts(_T("CreateToolhelp32Snapshot Error"));
		}
		else
		{
			PROCESSENTRY32 processEntry = { 0, };
			processEntry.dwSize = sizeof(processEntry);
			if (!Process32First(hSnapshot, &processEntry))
			{
				_putts(_T("Process32First"));
			}
			else
			{
				do 
				{
					_tprintf(_T("%4d %s\n"), processEntry.th32ProcessID, processEntry.szExeFile);
				} while (Process32Next(hSnapshot,&processEntry));
			}
			CloseHandle(hSnapshot);
		}
		
	}
	else if (!_tcscmp(cmdTokenList[0], _T("kill")))
	{
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		DWORD pid = 0;
		if (hSnapshot == INVALID_HANDLE_VALUE)
		{
			_putts(_T("CreateToolhelp32Snapshot Error"));
		}
		else
		{
			PROCESSENTRY32 processEntry = { 0, };
			processEntry.dwSize = sizeof(processEntry);
			if (!Process32First(hSnapshot, &processEntry))
			{
				_putts(_T("Process32First"));
			}
			else
			{
				do
				{
					if (_tcscmp(cmdBuffer + 5, processEntry.szExeFile) == 0)
					{
						pid = processEntry.th32ProcessID;
						break;
					}
				} while (Process32Next(hSnapshot, &processEntry));
			}
			CloseHandle(hSnapshot);
			if (pid == 0)
			{
				_putts(_T("Process Not Found"));
			}
			else
			{
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
				if (hProcess == INVALID_HANDLE_VALUE)
				{
					_putts(_T("OpenProcess Error"));
				}
				else
				{
					if (!TerminateProcess(hProcess, 0))
					{
						_putts(_T("TerminateProcess Error"));
					}
					CloseHandle(hProcess);
				}
			}
		}
	}
	else if (!_tcscmp(cmdTokenList[0], _T("dir")))
	{
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind = INVALID_HANDLE_VALUE;

		TCHAR DirSpec[MAX_STR_LEN] = { 0, };

		if (tokenNum > 1)
		{
			_tcscpy(DirSpec, cmdBuffer + 4);
			_tcscat(DirSpec, _T("\\*"));
		}
		else
		{
			_tcscpy(DirSpec, _T("*"));
		}

		hFind = FindFirstFile(DirSpec, &FindFileData);

		if (hFind == INVALID_HANDLE_VALUE)
		{
			_tprintf(_T("�߸��� ��� �Դϴ�.\n"));
		}
		else
		{
			_putts(FindFileData.cFileName);
			while (FindNextFile(hFind, &FindFileData) != 0)
			{
				_putts(FindFileData.cFileName);
			}
			FindClose(hFind);
		}
		printf_s("\n");
	}
	else if (!_tcscmp(cmdTokenList[0], _T("mkdir")))
	{
		TCHAR DirSpec[MAX_STR_LEN] = { 0, };

		if (tokenNum > 1)
		{
			_tcscpy(DirSpec, cmdBuffer + 6);
			if (!CreateDirectory(DirSpec, NULL))
			{
				DWORD Error = GetLastError();
				if (Error == ERROR_ALREADY_EXISTS)
					_putts(_T("�̹� �����ϴ� ���丮 �Դϴ�."));
				else if (Error == ERROR_PATH_NOT_FOUND)
					_putts(_T("�߸��� ��� �Դϴ�."));
			}
			else
			{
				_putts(_T("���丮�� �����Ǿ����ϴ�."));
			}
			
		}
		else
		{
			printf_s("��� ������ �ùٸ��� �ʽ��ϴ�.\n");
		}
	}
	else if (!_tcscmp(cmdTokenList[0], _T("rmdir")))
	{
		TCHAR DirSpec[MAX_STR_LEN] = { 0, };

		if (tokenNum > 1)
		{
			_tcscpy(DirSpec, cmdBuffer + 6);
			if (!RemoveDirectory(DirSpec))
			{
				_putts(_T("�߸��� ��� �Դϴ�."));
			}
			else
			{
				_putts(_T("���丮�� �����Ǿ����ϴ�."));
			}

		}
		else
		{
			printf_s("��� ������ �ùٸ��� �ʽ��ϴ�.\n");
		}
	}
	else if (!_tcscmp(cmdTokenList[0], _T("del")))
	{
		TCHAR DirSpec[MAX_STR_LEN] = { 0, };

		if (tokenNum > 1)
		{
			_tcscpy(DirSpec, cmdBuffer + 4);
			if (!DeleteFile(DirSpec))
			{
				DWORD error = GetLastError();
				if (error == ERROR_FILE_NOT_FOUND)
					_putts(_T("������ �������� �ʽ��ϴ�."));
				else if (error == ERROR_ACCESS_DENIED)
					_putts(_T("������ �źεǾ����ϴ�."));
			}
			else
			{
				_putts(_T("������ �����Ǿ����ϴ�."));
			}

		}
		else
		{
			printf_s("��� ������ �ùٸ��� �ʽ��ϴ�.\n");
		}
	}
	else if (!_tcscmp(cmdTokenList[0], _T("ren")))
	{
		TCHAR DirSpec[MAX_STR_LEN] = { 0, };

		if (tokenNum == 3)
		{
			if (!_trename(cmdTokenList[1], cmdTokenList[2]))
			{
				_putts(_T("������ �̸��� ����Ǿ����ϴ�."));
			}
			else if (errno == EACCES)
			{
				_putts(_T("�� �̸��� �߸��Ǿ����ϴ�."));
			}
			else if (errno == ENOENT)
				_putts(_T("������ ã�� �� �����ϴ�."));
			else if (errno == EINVAL)
				_putts(_T("�� �̸��� �߸��� ���ڰ� ���ԵǾ��ֽ��ϴ�."));
			else if (errno == EEXIST)
				_putts(_T("�̹� �����ϴ� �����̸��Դϴ�."));
			else
			{
				_tprintf(_T("�˼����� �����Դϴ�.%d\n"), errno);
			}
		}
		else
		{
			printf_s("��� ������ �ùٸ��� �ʽ��ϴ�.\n");
		}
	}
	else if (!_tcscmp(cmdTokenList[0], _T("type")))
	{
		STARTUPINFO si = { 0, };
		PROCESS_INFORMATION pi = { 0, };

		si.cb = sizeof(si);
		CreateProcess(NULL, cmdBuffer, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);

		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else if (!_tcscmp(cmdTokenList[0], _T("sort")))
	{
		STARTUPINFO si = { 0, };
		PROCESS_INFORMATION pi = { 0, };

		si.cb = sizeof(si);
		CreateProcess(NULL, cmdBuffer, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);

		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else if (!_tcscmp(cmdTokenList[0], _T("history")))
	{
		_putts(_T("������ ���� ��ɾ��"));
		int num = 1;
		for (int i = cmdHistory[historyHead][0] ? historyHead : 0; i != (historyHead-1+MAX_HISTORY)%MAX_HISTORY; ++i)
		{
			_tprintf_s(_T("%d. %s\n"), num++, cmdHistory[i]);
		}
		if (num == 1)
			_putts(_T("(���� ���� ��ɾ� ����.)"));
	}
	else
	{
		STARTUPINFO si = { 0, };
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		if (!CreateProcess(NULL, cmdBuffer, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi))
		{
			_putts(cmdBuffer);
			historyHead = (historyHead - 1 + MAX_HISTORY) % MAX_HISTORY;
		}
		else
		{
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
	}
	printf_s("\n");
		
	return 0;
}

TCHAR* StrLower(TCHAR* input)
{
	for (int i = 0; input[i]; i++)
	{
		input[i] = tolower(input[i]);
	}
	return input;
}
