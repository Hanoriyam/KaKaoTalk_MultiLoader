#include "pch.h"

std::vector<DWORD> FindProcesses(const WCHAR* pwFileName);
BOOL IsWow64();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpszCmdLine, int nCmdShow)
{
	std::vector<DWORD> pids = FindProcesses(EXE_NAME);
	for (DWORD pid : pids)
	{
		HANDLE hProcess = ::OpenProcess(0x0440, FALSE, pid);
		if (!hProcess)
			continue;

		DWORD dwSessionId = 1;
		ProcessIdToSessionId(pid, &dwSessionId);

		DWORD dwSize = 0;
		NTSTATUS status = STATUS_INFO_LENGTH_MISMATCH;
		std::unique_ptr<BYTE[]> handleInfoBuffer;
		while (status == STATUS_INFO_LENGTH_MISMATCH)
		{
			dwSize += 1024;
			handleInfoBuffer = std::make_unique<BYTE[]>(dwSize);
			status = NtQueryInformationProcess(hProcess, ProcessHandleInformation, handleInfoBuffer.get(), dwSize, &dwSize);
			if (NT_SUCCESS(status))
				break;
		}

		PROCESS_HANDLE_SNAPSHOT_INFORMATION* info = (PROCESS_HANDLE_SNAPSHOT_INFORMATION*)handleInfoBuffer.get();
		for (auto i = 0; i < info->NumberOfHandles; i++)
		{
			HANDLE hTarget;
			if (!DuplicateHandle(hProcess, info->Handles[i].HandleValue, GetCurrentProcess(), &hTarget, 0, FALSE, DUPLICATE_SAME_ACCESS))
				continue;

			BYTE pwName[1024] = { 0, };
			BYTE pwType[1024] = { 0, };
			if (!NT_SUCCESS(NtQueryObject(hTarget, ObjectNameInformation, pwName, sizeof(pwName), nullptr)) ||
				!NT_SUCCESS(NtQueryObject(hTarget, ObjectTypeInformation, pwType, sizeof(pwType), nullptr)))
				continue;
			CloseHandle(hTarget);

			UNICODE_STRING* name = (UNICODE_STRING*)pwName;
			UNICODE_STRING* type = (UNICODE_STRING*)pwType;
			if (name->Buffer == nullptr || type->Buffer == nullptr)
				continue;

			if (_wcsicmp(type->Buffer, L"Semaphore") != 0)
				continue;

			if (wcsstr(name->Buffer, SEMAPHORE_STRING) == NULL)
				continue;

			if (!DuplicateHandle(hProcess, info->Handles[i].HandleValue, GetCurrentProcess(), &hTarget, 0, FALSE, DUPLICATE_CLOSE_SOURCE))
				continue;

			CloseHandle(hTarget);
		}
	}

	STARTUPINFO si = { 0, };
	PROCESS_INFORMATION pi = { 0, };

	if (!CreateProcess(IsWow64() ? X64_PATH : X86_PATH, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		MessageBoxTimeoutW(NULL, L"Load fail.", MESSAGE_TITLE, MB_OK | MB_ICONINFORMATION, 0, 4000);
		return 0;
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	MessageBoxTimeoutW(NULL, L"Load Success. Wait for seconds...", MESSAGE_TITLE, MB_OK | MB_ICONINFORMATION, 0, 4000);
	return 0;
}

std::vector<DWORD> FindProcesses(const WCHAR* pwFileName)
{
	std::vector<DWORD> pids;
	HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		return pids;
	}

	PROCESSENTRY32 pe = { 0, };
	pe.dwSize = sizeof(pe);

	Process32First(hSnapshot, &pe);
	while (Process32Next(hSnapshot, &pe))
	{
		if (_wcsicmp(pe.szExeFile, pwFileName) != 0)
			continue;
		pids.push_back(pe.th32ProcessID);
	}

	if (hSnapshot)
		::CloseHandle(hSnapshot);

	return pids;
}

BOOL IsWow64()
{
	BOOL bIsWow64 = FALSE;
	IsWow64Process(GetCurrentProcess(), &bIsWow64);
	return bIsWow64;
}