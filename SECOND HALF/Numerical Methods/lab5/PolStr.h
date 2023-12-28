//---------------------------------------------------------------------------
// ������ ���������� �������� ���������� ���������, ������ 2.2 (win32)
// Copyright (C) 2003-2020 by Voverkill
//---------------------------------------------------------------------------
#ifndef __POLSTR_H__
#define __POLSTR_H__
//---------------------------------------------------------------------------
#include <locale.h>
#include <windows.h>
#include <stdio.h>
//---------------------------------------------------------------------------
// ��������� �� ������� �� DLL
struct PolStr
{
	typedef int(__stdcall *GetErrorProc)();
	typedef unsigned(__stdcall *StrToPolStrProc)(const char *, char *, unsigned);
	typedef char *(__stdcall *CreatePolStrProc)(const char *, unsigned);
	typedef double(__stdcall *EvalStrProc)(const char *, double, unsigned);
	typedef double(__stdcall *EvalStrVProc)(const char *, const double *, unsigned, unsigned, unsigned);
	typedef double(__stdcall *EvalPolStrProc)(const char *, double, unsigned);
	typedef double(__stdcall *EvalPolStrVProc)(const char *, const double *, unsigned, unsigned);

	HMODULE dll;
	GetErrorProc GetError;
	StrToPolStrProc StrToPolStr;
	CreatePolStrProc CreatePolStr;
	EvalStrProc EvalStr;
	EvalStrVProc EvalStrV;
	EvalPolStrProc EvalPolStr;
	EvalPolStrVProc EvalPolStrV;

	void ShowError(const char *procname = NULL)
	{
		LPVOID buf;
		FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			LPSTR(&buf),
			0,
			NULL
		);
		if (procname == NULL) printf("PolStr.dll: %s", (char *)buf);
		else printf("PolStr.dll::%s: %s", procname, (char *)buf);
		LocalFree(buf);
	}

	FARPROC LoadProc(const char *procname)
	{
		FARPROC proc = GetProcAddress(dll, procname);
		if (proc == NULL)
		{
			ShowError(procname);
		}
		return proc;
	}

	PolStr() : dll(NULL), GetError(NULL), StrToPolStr(NULL), CreatePolStr(NULL),
		EvalStr(NULL), EvalStrV(NULL), EvalPolStr(NULL), EvalPolStrV(NULL)
	{
		setlocale(LC_ALL, "russian");
		dll = LoadLibraryA("PolStr.dll");
		if (dll != NULL)
		{
			GetError = (GetErrorProc)LoadProc("GetError");
			StrToPolStr = (StrToPolStrProc)LoadProc("StrToPolStr");
			CreatePolStr = (CreatePolStrProc)LoadProc("CreatePolStr");
			EvalStr = (EvalStrProc)LoadProc("EvalStr");
			EvalStrV = (EvalStrVProc)LoadProc("EvalStrV");
			EvalPolStr = (EvalPolStrProc)LoadProc("EvalPolStr");
			EvalPolStrV = (EvalPolStrVProc)LoadProc("EvalPolStrV");
		}
		else
		{
			ShowError();
		}
	}

	~PolStr()
	{
		if (dll != NULL)
		{
			//FreeLibrary(dll);
		}
	}
} _;
//---------------------------------------------------------------------------
#define ERR_OK              0
#define ERR_SYNTAX_ERROR    1
#define ERR_BAD_ARGUMENT    2
#define ERR_STACK_ERROR     3
#define ERR_INVALID_PARAM   4
#define ERR_HUGE_VALUE      5
#define ERR_UNKNOWN_DER     6
#define ERR_OUT_OF_MEMORY   7
#define ERR_DLL_NOT_FOUND   8
#define ERR_PROC_NOT_FOUND  9
//---------------------------------------------------------------------------
// ��������� ������:
// � ERR_OK             - ������ ���;
// � ERR_SYNTAX_ERROR   - �������������� ������;
// � ERR_BAD_ARGUMENT   - �������� ������ � ���������;
// � ERR_STACK_ERROR    - � ����� ��� ����������� ������ (������ ��� ���������
//                        �������������� ������);
// � ERR_INVALID_PARAM  - �������� �������� � ��������� (������� ���������,
//                        ����������� �� ��������������� ��������� � �.�.);
// � ERR_HUGE_VALUE     - �������� ��������� �� ����� ��� ������� �� �����
//                        ���� double;
// � ERR_UNKNOWN_DER    - ����������� �� ����� ���� ���������;
// � ERR_OUT_OF_MEMORY  - �� ������� ������������ ������;
// � ERR_DLL_NOT_FOUND  - ���������� PolStr.dll �� �������;
// � ERR_PROC_NOT_FOUND - � ���������� PolStr.dll �� ������� ���� ��� ���������
//                        ��������� ��������.
// ���� ������ ���� ���������, �������� �������� ����� ������.
int GetError()
{
	if (!_.dll) return ERR_DLL_NOT_FOUND;
	if (!_.GetError || !_.StrToPolStr || !_.CreatePolStr || !_.EvalStr || !_.EvalStrV ||
		!_.EvalPolStr || !_.EvalPolStrV) return ERR_PROC_NOT_FOUND;
	return _.GetError();
}
//---------------------------------------------------------------------------
// �������������� ��������� � �������� �������� ������ (���). ��������� �����
// ��������� �������� ���������, ����� PI � E, ��������� (x ��� ����������,
// x1..xn ��� ���������� n-������� ���������), ������ � ��������� ��������:
// � +   (�������� ��� ������� ����);
// � -   (��������� ��� ������� �����);
// � *   (���������);
// � /   (�������);
// � ^   (���������� � �������);
// � sin (�����);
// � cos (�������);
// � tg  (�������);
// � ctg (���������);
// � exp (���������������� �������, exp x = e^x);
// � ln  (����������� ��������);
// � lg  (�������� �� ��������� 10).
// ������� �������� �� �����. ������ ����� ������� �� ������ ���������
// ������� �� ����������� (��������, ����� ��������� ���������: cos 2,
// exp(x), sin (x + pi)). ����� �������������� � ��� ����� �����������
// ��������� �������:
// � s - sin;
// � c - cos;
// � t - tg;
// � z - ctg;
// � e - exp;
// � l - ln;
// � g - lg;
// � n - ������� �����;
// � p - ������� ����;
// � q - ����� e;
// � w - ����� pi.
// ����� ��������, �������� ��������� � ��������� �������� ��� ���������.
// ������ ���������. ���������:
// 1) char *expr - ��������� �� ������, ���������� �������� ���������;
// 2) char *pstr - ��������� �� ������, � ������� ����� �������� ���������
//    � ���� ��� (���� ����� NULL, ������ �� �����������, ������� ������
//    ��������� �� �����);
// 3) unsigned arg_count - ���������� ���������� � ��������� (���� ��������
//    ���������, ������ ���� ����� ����).
// ������������ �������� - ����� ���. ��������� ����������� �� ��� ���, ����
// �� ���������� ������ ����� ������, ������ ";" ��� �� ��������� ������.
// ���� � �������� �������������� �������� ������, ��� ����� ������������ ��
// ���������. ������� �� ����������� ������!
unsigned StrToPolStr(const char *expr, char *pstr, unsigned arg_count = 0)
{
	return _.StrToPolStr(expr, pstr, arg_count);
}
//---------------------------------------------------------------------------
// �������� ���. ���������:
// 1) char *expr - ��������� �� ������, ���������� �������� ���������;
// 2) unsigned arg_count - ���������� ���������� � ���������.
// ������ ������� ������ ������� �������� ������� StrToPolStr, �����
// ��������� ����� ���, ����� �������� ����������� ��� �������� ��� ������ �
// ����� �������� StrToPolStr ��� ������������ ���. ������ ����������
// �����������, ������� �� �������� ���������� �� ���������� delete [].
char *CreatePolStr(const char *expr, unsigned arg_count)
{
	return _.CreatePolStr(expr, arg_count);
}
//---------------------------------------------------------------------------
// ���������� ��������� (��������� ��������). ���������:
// 1) char *expr - ��������� �� ������, ���������� ���������;
// 2) double arg - �������� ���������;
// 3) unsigned der - ������� ����������� (0 - �� ��������� - ���������
//    ���������, 1 - ������ �����������, 2 - ������ ����������� � �.�.).
// ������ ������� ������ ���������� �������� ������� EvalStr ��� ����������
// ���������: EvalStr(expr, &arg, 0, der, 0).
double EvalStr(const char *expr, double arg, unsigned der = 0)
{
	return _.EvalStr(expr, arg, der);
}
//---------------------------------------------------------------------------
// ���������� ��������� (��������� ��������). ���������:
// 1) char *expr - ��������� �� ������, ���������� ���������;
// 2) double *args - ������ ���������� (������� ������� �������� x1,
//    ������ ������� - x2 � �.�);
// 3) unsigned arg_count - ���������� ���������� � ���������;
// 4) unsigned der - ������� ����������� (�� ��������� 0);
// 5) unsigned arg_idx - �� ������ ��������� ������� ����������� (0 - ��
//    ��������� - �� ������������� ���������� ���������, 1 - �� x1 ���
//    ���������� ���������, 2 - �� x2 � �.�.).
// ������ ������� ������� ��������� ��� pstr ������� ������� CreatePolStr,
// �����, ���� ���� ������, ���������� ����, ����� ���������� ��������
// ������� EvalPolStr(pstr, args, der, arg_idx). ���������: ���� ���� � ��
// �� ��������� ����������� ��������� ���, �� ��������� ������ ��� ���,
// ������������ ��� � ������������ ������ ����� ������������� �����������,
// ��� �� ���� ������. � ���� ������ ����� ���� ��� �������������� �������
// ��� � ������� ������� CreatePolStr, � ����� �������� EvalPolStr ������
// EvalStr.
double EvalStr(const char *expr, const double *args, unsigned arg_count,
	unsigned der = 0, unsigned arg_idx = 0)
{
	return _.EvalStrV(expr, args, arg_count, der, arg_idx);
}
//---------------------------------------------------------------------------
// ���������� ��������� � ���� ��� (��������� ��������). ���������:
// 1) char *pstr - ��������� �� ������, ���������� ���;
// 2) double arg - �������� ���������;
// 3) unsigned der - ������� ����������� (�� ��������� 0).
// ������ ������� ������ ���������� �������� ������� EvalPolStr ���
// ���������� ���������: EvalPolStr(pstr, &arg, der, 0).
double EvalPolStr(const char *pstr, double arg, unsigned der = 0)
{
	return _.EvalPolStr(pstr, arg, der);
}
//---------------------------------------------------------------------------
// ���������� ��������� � ���� ��� (��������� ��������). ��� �������� �������
// ��� ���������� ���������, ��� ��������� ������� ��� ��� ����� �������� ��.
// ���������:
// 1) char *pstr - ��������� �� ������, ���������� ���;
// 2) double *args - ������ ����������;
// 3) unsigned der - ������� ����������� (�� ��������� 0);
// 4) unsigned arg_idx - �� ������ ��������� ������� ����������� (��
//    ��������� 0).
// ������� ���������� �������� ���������, ���� �� ����� ���������� �� ����
// ������, ����� ����. ������ ERR_UNKNOWN_DER ��������� � ��� ������, ����
// ���������� ��������� ����������� ���������. ���� ��������� �������� ������
// �������� +, - � *, �� ����� �������� ����� ��� �����������. ���� ���
// �������� ������ ������� � ��������, �� ����� ��������� ������ ������ �
// ������ �����������.
double EvalPolStr(const char *pstr, const double *args, unsigned der = 0,
	unsigned arg_idx = 0)
{
	return _.EvalPolStrV(pstr, args, der, arg_idx);
}
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
