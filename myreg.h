#pragma once

#include <VersionHelpers.h>

LPCWSTR const szEmEditorClass = L"EmEditorMainFrame3";

void GetProfileStringReg( HKEY hKey, LPCWSTR lpszEntry, LPWSTR lpszBuf, DWORD cchBufSize, LPCWSTR lpszDefault )
{
	DWORD dwType, dwCount;
	dwCount = cchBufSize * sizeof( WCHAR );
	if( hKey == NULL 
		|| RegQueryValueEx( hKey, lpszEntry, NULL, &dwType, (LPBYTE)lpszBuf, &dwCount) != ERROR_SUCCESS ){
		StringCopy( lpszBuf, cchBufSize, lpszDefault );
	}
}

bool MyIsWindows10OrGreater()
{
	return IsWindowsVersionOrGreater( 10, 0, 0 );
}

UINT MyGetDpiForWindow( HWND hwnd )
{
	UINT dpi = 0;
	if (MyIsWindows10OrGreater()){
		int n = ::SetErrorMode( SEM_NOOPENFILEERRORBOX );
		HINSTANCE hinst = ::LoadLibraryEx( L"User32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32 );
		::SetErrorMode( n );
		if( hinst != NULL ) {
			UINT (WINAPI *lpGetDpiForWindow) (HWND) = NULL;
			lpGetDpiForWindow = (UINT (WINAPI*)(HWND))::GetProcAddress( hinst, "GetDpiForWindow" );
			if( lpGetDpiForWindow ){
				dpi = lpGetDpiForWindow( hwnd );
			}
			::FreeLibrary( hinst );
		}
	}

	HWND hwndOrg = hwnd;
	if( dpi == 0 ) {
		WCHAR szClassName[30];
		for( ;; ) {
			if( hwnd == NULL ) {
				hwnd = GetWindow( hwndOrg, GW_OWNER );
			}
			if( hwnd == NULL || !IsWindow( hwnd ) ) {
				break;
			}
			if( !GetClassName(hwnd, szClassName, 30) ) {
				break;
			}
			if( wcscmp(szClassName, szEmEditorClass) == 0 ) {
				// found
				dpi = (UINT)Editor_Info( hwnd, EI_GET_DPI, 0 );
				return dpi;
			}
			hwndOrg = hwnd;
			hwnd = GetParent( hwnd );
		}
	}

	if( dpi == 0 ) {
		_ASSERT( FALSE );
		HDC hdc = GetDC(0);
		if( hdc ) {
			dpi = GetDeviceCaps( hdc, LOGPIXELSY );
			ReleaseDC( 0, hdc );
		}
	}
	if( dpi == 0 ) {
		_ASSERT( FALSE );
		dpi = 96;
	}
	return dpi;
}

