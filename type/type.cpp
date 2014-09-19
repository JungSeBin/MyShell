// type.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <windows.h>

#define MAX_BUF_SIZE 256
#pragma warning(disable:4996)

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 2)
		return 0;

	char cbuffer[MAX_BUF_SIZE] = { 0, };
	HANDLE hFile = CreateFile(argv[1], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	DWORD readByte = 0;
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf_s("CreateFile Error");
		return -1;
	}

	while (true)
	{
		if (!ReadFile(hFile, cbuffer, sizeof(cbuffer)-1, &readByte, NULL))
		{
			CloseHandle(hFile);
			printf_s("ReadFile Error");
			return -2;
		}
		if (readByte == 0)
			break;
		else
		{
			cbuffer[readByte] = '\0';
			puts(cbuffer);
		}
	}
	printf_s("\n");
	CloseHandle(hFile);
	return 0;
}

