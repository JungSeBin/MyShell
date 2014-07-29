// perfmon.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include <iostream>
#include <locale.h>
#include <windows.h>
#include <string.h>
#pragma  warning(disable:4996)

#define MAX_STR_LEN 256
#define CMD_TOKEN_NUM 10

//TCHAR ERROR_CMD[]
//= _T("'%s'��(��) ������ �� �ִ� ���α׷��� �ƴմϴ�.\n");

int CmdProcessing();
TCHAR* StrLower(TCHAR*);


int _tmain(int argc, _TCHAR* argv[])
{

	//�ѱ� �Է��� ������ �ϱ� ����.
	_tsetlocale(LC_ALL, _T("Korean"));

	DWORD isExit;
	while (1)
	{
		isExit = CmdProcessing();
		if (isExit == TRUE)
		{
			_fputts(_T("��ɾ� ó���� �����մϴ�.\n"), stdout);
			break;
		}
	}
	return 0;
}

TCHAR cmdString[MAX_STR_LEN];
TCHAR cmdTokenList[CMD_TOKEN_NUM][MAX_STR_LEN];
TCHAR seps[] = _T(",\t\n");

int CmdProcessing()
{
	_fputts(_T("My shell>> "), stdout);
	_getts(cmdString);

	TCHAR* token = _tcstok(cmdString, seps);

	int tokenNum = 0;
	while (token != NULL)
	{
		_tcscpy(
			cmdTokenList[tokenNum++], StrLower(token)
			);
		token = _tcstok(NULL, seps);
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
	else
	{
		STARTUPINFO si = { 0, };
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		if (!CreateProcess(NULL, cmdString, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi))
		{
			_putts(cmdTokenList[0]);
		}
		else
		{
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
	}
		
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
