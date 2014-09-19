// Sort.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#define MAX_STR_NUM 256
#define MAX_STR_LEN 256
#pragma warning(disable:4996)


int _tmain(int argc, _TCHAR* argv[])
{
	TCHAR strArray[MAX_STR_NUM][MAX_STR_LEN];
	int strNum = 0;
	TCHAR temp[MAX_STR_LEN];

	for (strNum = 0; strNum < MAX_STR_NUM; ++strNum)
	{
		if (_fgetts(strArray[strNum], MAX_STR_LEN, stdin) == nullptr)
			break;
	}

	for (int i = 0; i < strNum - 1; ++i)
	{
		TCHAR* strMin = strArray[i];
		for (int j = i + 1; j < strNum; ++j)
		{
			if (_tcscmp(strMin, strArray[j]) >0)
			{
				strMin = strArray[j];
			}
		}
		if (strMin != strArray[i])
		{
			_tcscpy(temp, strArray[i]);
			_tcscpy(strArray[i], strMin);
			_tcscpy(strMin, temp);
		}
	}
	for (int i = 0; i < strNum; ++i)
	{
		_tprintf(_T("%s"), strArray[i]);
	}
	return 0;
}

