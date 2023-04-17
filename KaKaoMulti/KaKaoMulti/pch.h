#pragma once
#include <Windows.h>
#include <tchar.h>
#include <TlHelp32.h>

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <deque>
#include <list>
#include <thread>

#pragma comment(lib, "ntdll")
#pragma commnet(lib, "user32")

#define EXE_NAME L"KaKaoTalk.exe"
#define SEMAPHORE_STRING L"97C4DDD9"

#define X64_PATH L"C:\\Program Files (x86)\\Kakao\\KakaoTalk\\KaKaoTalk.exe"
#define X86_PATH L"C:\\Program Files\\Kakao\\KakaoTalk\\KaKaoTalk.exe"

#define MESSAGE_TITLE L"KaKaoMulti"

#define NT_SUCCESS(status) (status >= 0)
#define STATUS_INFO_LENGTH_MISMATCH 0xC0000004L

typedef struct _PROCESS_HANDLE_TABLE_ENTRY_INFO
{
	HANDLE HandleValue;
	ULONG_PTR HandleCount;
	ULONG_PTR PointerCount;
	ULONG GrantedAccess;
	ULONG ObjectTypeIndex;
	ULONG HandleAttributes;
	ULONG Reserved;
} PROCESS_HANDLE_TABLE_ENTRY_INFO;

typedef struct _PROCESS_HANDLE_SNAPSHOT_INFORMATION
{
	ULONG_PTR NumberOfHandles;
	ULONG_PTR Reserved;
	PROCESS_HANDLE_TABLE_ENTRY_INFO Handles[1];
} PROCESS_HANDLE_SNAPSHOT_INFORMATION;

enum PROCESSINFOCLASS
{
	ProcessHandleInformation = 51
};

typedef enum _OBJECT_INFORMATION_CLASS
{
	ObjectBasicInformation = 0,
	ObjectNameInformation = 1,
	ObjectTypeInformation = 2,
	ObjectAllTypesInformation = 3,
	ObjectHandleInformation = 4
} OBJECT_INFORMATION_CLASS;

typedef struct _UNICODE_STRING
{
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING;

typedef struct _OBJECT_NAME_INFORMATION
{
	UNICODE_STRING Name;
} OBJECT_NAME_INFORMATION;

extern "C" NTSTATUS __stdcall NtQueryInformationProcess(HANDLE ProcessHandle, PROCESSINFOCLASS ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength);
extern "C" NTSTATUS __stdcall NtQueryObject(HANDLE Handle, OBJECT_INFORMATION_CLASS ObjectInformationClass, PVOID ObjectInformation, ULONG ObjectInformationLength, PULONG ReturnLength);
extern "C" int __stdcall MessageBoxTimeoutW(HWND hwnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType, WORD wLanguageId, DWORD dwMilliseconds);