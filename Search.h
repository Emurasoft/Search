// implementatin of this specific plug-in is here:
//
#ifndef WM_DPICHANGED_BEFOREPARENT
#define WM_DPICHANGED_BEFOREPARENT      0x02E2
#define WM_DPICHANGED_AFTERPARENT       0x02E3
#define WM_GETDPISCALEDSIZE             0x02E4
#endif

#define UPDATE_TREE_NONE			0
#define UPDATE_OUTLINE				1
#define UPDATE_TREE_SEL				2
#define UPDATE_TREE_LINE_STRING		3
#define UPDATE_TREE_ALL				4

#define ZERO_INIT_FIRST_MEM(classname, firstmem)  ZeroMemory( &firstmem, sizeof( classname ) - ((char*)&firstmem - (char*)this) );

INT_PTR CALLBACK SearchProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );
INT_PTR CALLBACK PropDlg( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );

void CenterWindow( HWND hDlg )
{
	_ASSERT(::IsWindow(hDlg));
	HWND hwndParent = GetParent(hDlg);

	RECT rcDlg;
	::GetWindowRect(hDlg, &rcDlg);
	RECT rcArea;
	RECT rcCenter;

	if( hwndParent != NULL ){
		DWORD dwStyleCenter = ::GetWindowLong(hwndParent, GWL_STYLE);
		if(!(dwStyleCenter & WS_VISIBLE) || (dwStyleCenter & WS_MINIMIZE))
			hwndParent = NULL;
	}

	HMONITOR hMonitor = NULL;
	if(hwndParent != NULL) {
		hMonitor = ::MonitorFromWindow(hwndParent, MONITOR_DEFAULTTONEAREST);
	}
	else {
		hMonitor = ::MonitorFromWindow(hDlg, MONITOR_DEFAULTTONEAREST);
	}
	_ASSERT(hMonitor != NULL);

	MONITORINFO minfo;
	minfo.cbSize = sizeof(MONITORINFO);
	VERIFY( ::GetMonitorInfo(hMonitor, &minfo) );

	rcArea = minfo.rcWork;

	if(hwndParent == NULL)
		rcCenter = rcArea;
	else
		::GetWindowRect(hwndParent, &rcCenter);

	int DlgWidth = rcDlg.right - rcDlg.left;
	int DlgHeight = rcDlg.bottom - rcDlg.top;

	int xLeft = (rcCenter.left + rcCenter.right) / 2 - DlgWidth / 2;
	int yTop = (rcCenter.top + rcCenter.bottom) / 2 - DlgHeight / 2;

	if(xLeft + DlgWidth > rcArea.right)
		xLeft = rcArea.right - DlgWidth;
	if(xLeft < rcArea.left)
		xLeft = rcArea.left;

	if(yTop + DlgHeight > rcArea.bottom)
		yTop = rcArea.bottom - DlgHeight;
	if(yTop < rcArea.top)
		yTop = rcArea.top;

	::SetWindowPos(hDlg, NULL, xLeft, yTop, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void InsertColumnToListView( HWND hwndList, int nIDString, int nIDString2 )
{
	int dpi = MyGetDpiForWindow( hwndList );

	WCHAR szText[32];
	LV_COLUMN lvC;
	ZeroMemory( &lvC, sizeof(lvC) );
	lvC.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvC.fmt = LVCFMT_LEFT;   // left-align column
	lvC.pszText = szText;
	HINSTANCE hinstRes = EEGetLocaleInstanceHandle();
	LoadString( hinstRes, nIDString, szText, _countof( szText ) );
	int nIndex = 0;
	int cx = 0;
	if( nIDString2 != 0 ) {
		cx = MulDiv( 160, dpi, 96 );
		lvC.cx = cx;
		VERIFY( ListView_InsertColumn( hwndList, nIndex++, &lvC ) != -1 );
		LoadString( hinstRes, nIDString2, szText, _countof( szText ) );
	}
	lvC.cx = MulDiv( 40, dpi, 96 );
	VERIFY( ListView_InsertColumn( hwndList, nIndex, &lvC ) != -1 );
}


class CMyFrame : public CETLFrame<CMyFrame>
{
public:
	// _loc.dll in MUI sub folder?
	enum { _USE_LOC_DLL			= LOC_USE_EMEDLOC_DLL	};

	// string ID
	enum { _IDS_MENU			= IDS_SEARCH_MENU_TEXT	};   // name of command, menu
	enum { _IDS_STATUS			= IDS_SEARCH_MENU_TEXT	};   // description of command, status bar
	enum { _IDS_NAME			= IDS_SEARCH_MENU_TEXT	};   // name of plug-in, plug-in settings dialog box
	enum { _IDS_VER				= IDS_PLUGIN_VERSION	};   // version string of plug-in, plug-in settings dialog box

	// bitmaps
	enum { _IDB_BITMAP			= IDB_BITMAP			};
	enum { _IDB_16C_24			= IDB_16C_24			};
	//enum { _IDB_256C_16_DEFAULT = IDB_TRUE_16_DEFAULT	};
	//enum { _IDB_256C_16_HOT		= IDB_TRUE_16_HOT		};
	//enum { _IDB_256C_16_BW		= IDB_TRUE_16_BW		};
	//enum { _IDB_256C_24_DEFAULT = IDB_TRUE_24_DEFAULT	};
	//enum { _IDB_256C_24_HOT		= IDB_TRUE_24_HOT		};
	//enum { _IDB_256C_24_BW		= IDB_TRUE_24_BW		};
	enum { _IDB_TRUE_16_DEFAULT = IDB_TRUE_16_DEFAULT	};
	enum { _IDB_TRUE_16_HOT		= IDB_TRUE_16_HOT		};
	enum { _IDB_TRUE_16_BW		= IDB_TRUE_16_BW		};
	enum { _IDB_TRUE_24_DEFAULT = IDB_TRUE_24_DEFAULT	};
	enum { _IDB_TRUE_24_HOT		= IDB_TRUE_24_HOT		};
	enum { _IDB_TRUE_24_BW		= IDB_TRUE_24_BW		};

	// masks
	enum { _MASK_TRUE_COLOR		= CLR_NONE				};
	//enum { _MASK_256_COLOR		= CLR_NONE				};

	// whether to allow a file is opened in the same window group during the plug-in execution.
	enum { _ALLOW_OPEN_SAME_GROUP = TRUE				};

	// whether to allow multiple instances.
	enum { _ALLOW_MULTIPLE_INSTANCES = TRUE				};

	// supporting EmEditor newest version * 1000
	enum { _MAX_EE_VERSION		= 14900					};

	// supporting EmEditor oldest version * 1000
	enum { _MIN_EE_VERSION		= 12000					};

	// supports EmEditor Professional
	enum { _SUPPORT_EE_PRO		= TRUE					};

	// supports EmEditor Standard
	enum { _SUPPORT_EE_STD		= FALSE					};

	// user-defined members

	// data that can be set zeros below
	HWND m_hDlg;
	UINT m_nClientID;
	int  m_iPos;
	int  m_iOldPos;
	int  m_nDpi;
	WCHAR m_szFind[MAX_PATH];
	bool m_bProfileLoaded;
	bool m_bOpenStartup;
	bool m_bCase;
	bool m_bRegex;
	bool m_bUninstalling;
	
	BOOL DisableAutoComplete( HWND /* hwnd */ )
	{
		return FALSE;
	}

	BOOL UseDroppedFiles( HWND /* hwnd */ )
	{
		return FALSE;
	}

	LRESULT UserMessage( HWND hwnd, WPARAM wParam, LPARAM lParam )
	{
//		if( wParam & 1 ) {
			if( m_hDlg == NULL ) {
				OnCommand( hwnd );
			}
			if( m_hDlg ) {
				CheckDlgButton( m_hDlg, IDC_CASE, (wParam & FLAG_FIND_CASE) != 0 );
				CheckDlgButton( m_hDlg, IDC_REGEX, (wParam & FLAG_FIND_REG_EXP) != 0 );
				SetDlgItemText( m_hDlg, IDC_FIND, (LPCWSTR)lParam );
				OnSearch( m_hDlg );
			}
//		}
		return 0;
	}

	void OnCommand( HWND /*hwndView*/ )
	{
		if( m_hDlg == NULL ){
			//WCHAR sz[260];
			WCHAR szAppName[80];
			LoadString( EEGetLocaleInstanceHandle(), IDS_SEARCH_MENU_TEXT, szAppName, _countof( szAppName ) );
			//if( Editor_GetVersion( m_hWnd ) < 8000 ){
			//	LoadString( EEGetLocaleInstanceHandle(), IDS_INVALID_VERSION, sz, _countof( sz ) );
			//	MessageBox( m_hWnd, sz, szAppName, MB_OK | MB_ICONSTOP );
			//	return;
			//}
			m_hDlg = CreateDialog( EEGetLocaleInstanceHandle(), MAKEINTRESOURCE( IDD_SEARCH ), m_hWnd, SearchProc );

			if( m_hDlg != NULL ){
				SetTreeColor();
				CUSTOM_BAR_INFO cbi;
				ZeroMemory( &cbi, sizeof( cbi ) );
				cbi.cbSize = sizeof( cbi );
				cbi.hwndClient = m_hDlg;
				cbi.iPos = m_iPos;
				cbi.pszTitle = szAppName;
				m_nClientID = Editor_CustomBarOpen( m_hWnd, &cbi );

				if( !m_nClientID ){
					CustomBarClosed();
				}
				else {
					ShowWindow( m_hDlg, SW_SHOW );

					// The parent window is set to the custom bar window.
					_ASSERTE( cbi.hwndCustomBar && GetParent( m_hDlg ) == cbi.hwndCustomBar );
				}
			}
		}
		else {
			_ASSERTE( m_nClientID );
			Editor_CustomBarClose( m_hWnd, m_nClientID );
			CustomBarClosed();
		}
	}

	void CustomBarClosed()
	{
		if( m_hDlg ){
			if( IsWindow( m_hDlg ) ){
				DestroyWindow( m_hDlg );
			}
			_ASSERT( !IsWindow( m_hDlg ) );
			m_hDlg = NULL;
			m_nClientID = 0;
		}
	}

	BOOL QueryStatus( HWND /*hwndView*/, LPBOOL pbChecked )
	{		
		*pbChecked = (m_hDlg != NULL);
		return TRUE;
	}

	void OnEvents( HWND hwndView, UINT nEvent, LPARAM lParam )
	{
		if( nEvent & EVENT_CREATE_FRAME ){
			LoadProfile();
			if( m_bOpenStartup ){
				OnCommand( hwndView );
			}
		}
		if( nEvent & EVENT_CLOSE_FRAME ){
			if( m_hDlg ){
				OnCommand( hwndView );
			}
		}
		if( nEvent & EVENT_CUSTOM_BAR_CLOSED ){
			// this message arrives even if plug-in does not own this custom bar, so make sure it is mine.
			if( m_hDlg != NULL ){  
				CUSTOM_BAR_CLOSE_INFO* pCBCI = (CUSTOM_BAR_CLOSE_INFO*)lParam;
				if( pCBCI->nID == m_nClientID ){
					CustomBarClosed();
					// if the frame closed while Custom Bar is open, save the status for next startup.
					m_bOpenStartup = (pCBCI->dwFlags & CLOSED_FRAME_WINDOW);
					SaveProfile();
				}
			}
		}
		if( nEvent & (EVENT_FILE_OPENED | EVENT_DOC_SEL_CHANGED) ){
		}
		if( nEvent & EVENT_CHANGE ){
		}
		if( nEvent & EVENT_CARET_MOVED ){
		}
		if( nEvent & EVENT_IDLE ){
		}
		if( nEvent & EVENT_CONFIG_CHANGED ) {
			SetTreeColor();
		}
	}

	BOOL QueryUninstall( HWND /*hDlg*/ )
	{
		return TRUE;
	}

	BOOL SetUninstall( HWND hDlg, LPWSTR pszUninstallCommand, LPWSTR pszUninstallParam )
	{
		WCHAR szProductCode[80] = { 0 };
		HKEY hKey = NULL;
		if( RegOpenKeyEx( HKEY_LOCAL_MACHINE, L"Software\\EmSoft\\EmEditorPlugIns\\Search", 0, KEY_READ, &hKey ) == ERROR_SUCCESS && hKey ){
			GetProfileStringReg( hKey, L"ProductCode", szProductCode, _countof( szProductCode ), L"" );
			if( szProductCode[0] ){
				GetSystemDirectory( pszUninstallCommand, MAX_PATH );
				PathAppend( pszUninstallCommand, L"msiexec.exe" );

				StringPrintf( pszUninstallParam, MAX_PATH, L"/X%s", szProductCode );
				RegCloseKey( hKey );
				return UNINSTALL_RUN_COMMAND;
			}
		}
		WCHAR sz[80];
		WCHAR szAppName[80];
		LoadString( EEGetLocaleInstanceHandle(), IDS_SURE_TO_UNINSTALL, sz, sizeof( sz ) / sizeof( WCHAR ) );
		LoadString( EEGetLocaleInstanceHandle(), IDS_SEARCH_MENU_TEXT, szAppName, sizeof( szAppName ) / sizeof( WCHAR ) );
		if( MessageBox( hDlg, sz, szAppName, MB_YESNO | MB_ICONEXCLAMATION ) == IDYES ){
			EraseProfile();
			m_bUninstalling = true;
			return UNINSTALL_SIMPLE_DELETE;
		}
		return UNINSTALL_FALSE;
	}

	BOOL QueryProperties( HWND /*hDlg*/ )
	{
		return TRUE;
	}

	BOOL SetProperties( HWND hDlg )
	{
		DialogBox( EEGetLocaleInstanceHandle(), MAKEINTRESOURCE( IDD_PLUGIN_PROP ), hDlg, PropDlg );
		return TRUE;
	}

	BOOL PreTranslateMessage( HWND /*hwndView*/, MSG* pMsg )
	{
		HWND hwndFocus = GetFocus();
		if( hwndFocus ){
			if( m_hDlg && IsChild( m_hDlg, hwndFocus ) ){
				if( pMsg->message == WM_KEYDOWN ){
					bool bCtrl = GetKeyState( VK_CONTROL ) < 0;
					bool bShift = GetKeyState( VK_SHIFT ) < 0;
					if( !bCtrl ){
						if( pMsg->wParam == VK_ESCAPE ){
							if( !bShift ){
								Editor_ExecCommand( m_hWnd, EEID_ACTIVE_PANE );
								return TRUE;
							}
						}
						//else if( pMsg->wParam == VK_F6 ){
						//	Editor_ExecCommand( m_hWnd, bShift ? EEID_PREV_PANE : EEID_NEXT_PANE );
						//	return TRUE;
						//}
						else if( pMsg->wParam == VK_RETURN ){
							if( !bShift ){
								HWND hwndList = GetDlgItem( m_hDlg, IDC_LIST );
								if( hwndFocus == hwndList ){
									int iItem = ListView_GetNextItem( hwndList, -1, LVNI_SELECTED );
									OnJump( iItem );
									return TRUE;
								}
							}
						}
					}
				}
				if( IsDialogMessage( m_hDlg, pMsg ) ){
					return TRUE;
				}
			}
		}
		return FALSE;
	}

	CMyFrame()
	{
		ZERO_INIT_FIRST_MEM( CMyFrame, m_hDlg );
	}

	UINT_PTR GetCurrentLine( HWND hwndView )
	{
		POINT_PTR ptCurrentPos;
		Editor_GetCaretPos( hwndView, POS_LOGICAL_W, &ptCurrentPos );
		return ptCurrentPos.y;
	}

	void LoadProfile()
	{
		if( !m_bProfileLoaded ){
			m_bProfileLoaded = true;
			m_bOpenStartup = !!GetProfileInt( L"OpenStartup", FALSE );
			m_iPos = GetProfileInt( L"CustomBarPos", CUSTOM_BAR_LEFT );
		}
	}

	void SaveProfile()
	{
		if( !m_bUninstalling ){
			WriteProfileInt( L"OpenStartup", m_bOpenStartup );
		}
	}

	void OnSearch( HWND hwnd )
	{
		WCHAR szFind[MAX_PATH];
		GetDlgItemText( hwnd, IDC_FIND, szFind, _countof( szFind ) );
		if( !szFind[0] )  return;
		bool bCase = !!IsDlgButtonChecked( hwnd, IDC_CASE );
		m_bCase = bCase;
		bool bRegex = !!IsDlgButtonChecked( hwnd, IDC_REGEX );
		m_bRegex = bRegex;
		if( !bCase && !bRegex ){
			int nLen = (int)wcslen( szFind );
			LCMapString( LOCALE_INVARIANT, LCMAP_LOWERCASE, szFind, nLen, szFind, nLen );
		}
		StringCopy( m_szFind, _countof( m_szFind ), szFind );
		HWND hwndList = GetDlgItem( hwnd, IDC_LIST );
		if( !hwndList )  return;
		ListView_DeleteAllItems( hwndList );

		HEEDOC hActiveDoc = (HEEDOC)Editor_Info( m_hWnd, EI_GET_ACTIVE_DOC, 0 );
		UINT nCount = (UINT)Editor_Info( m_hWnd, EI_GET_DOC_COUNT, FLAG_REAL_DOC_INDEX );
		if( nCount > 0 ){
			for( UINT iDoc = 0; iDoc < nCount; iDoc++ ){
				HEEDOC hDoc = (HEEDOC)Editor_Info( m_hWnd, EI_INDEX_TO_DOC_REAL, iDoc );
				_ASSERT( hDoc );
				if( Editor_Info( m_hWnd, EI_SET_ACTIVE_DOC, (LPARAM)hDoc ) ) {
					UINT_PTR nLines = Editor_GetLines( m_hWnd, POS_LOGICAL_W );
					GET_LINE_INFO gli;
					gli.flags = FLAG_LOGICAL;
					WCHAR* pBuf = NULL;
					for( gli.yLine = 0; gli.yLine < nLines; gli.yLine++ ){
						int bFound = FALSE;
						gli.cch = 0;
						UINT_PTR ch = Editor_GetLineW( m_hWnd, &gli, NULL );
						if( ch <= 1 )  continue;
						if( ch > gli.cch ){
							delete [] pBuf;
							pBuf = new WCHAR[ ch ];
							gli.cch = ch;
						}
						if( pBuf == NULL )  break;
						Editor_GetLineW( m_hWnd, &gli, pBuf );

						if( bRegex ){
							LPCWSTR pszStart, pszEnd, pszNext;
							FIND_REGEX_INFO fri;
							ZeroMemory( &fri, sizeof( fri ) );
							fri.cbSize = sizeof( fri );
							fri.pszRegex = szFind;
							fri.pszText = pBuf;
							fri.ppszStart = &pszStart;
							fri.ppszEnd = &pszEnd;
							fri.ppszNext = &pszNext;
							if( bCase ){
								fri.nFlags = FLAG_FIND_CASE;
							}
							bFound = Editor_FindRegex( m_hWnd, &fri );
							if( bFound == -1 ){
								delete [] pBuf;
								return;
							}
						}
						else {
							if( !bCase ){
								CharLower( pBuf );
							}
							bFound = wcsstr( pBuf, szFind ) != NULL;
						}
						if( bFound == TRUE ){
							WCHAR szTitle[MAX_PATH];
							Editor_DocInfoEx( m_hWnd, hDoc, EI_GET_SAVE_AS_TITLEW, (LPARAM)szTitle );
							WCHAR szLine[32];
#ifdef _WIN64
							StringPrintf( szLine, _countof( szLine ), L"%llu", gli.yLine + 1 );
#else
							StringPrintf( szLine, _countof( szLine ), L"%u", gli.yLine + 1 );
#endif
							LVITEM item;
							ZeroMemory( &item, sizeof( item ) );
							item.mask = LVIF_TEXT;
							item.iItem = INT_MAX;
							item.pszText = szTitle;
							int iItem = ListView_InsertItem( hwndList, &item );
							if( iItem >= 0 ){
								item.iItem = iItem;
								item.iSubItem = 1;
								item.pszText = szLine;
								ListView_SetItem( hwndList, &item );
							}
						}
					}
					delete [] pBuf;
				}
			}

			VERIFY( Editor_Info( m_hWnd, EI_SET_ACTIVE_DOC, (LPARAM)hActiveDoc ) );
		}
	}



	BOOL OnInitDialog( HWND hwnd )
	{
		HWND hwndList = GetDlgItem( hwnd, IDC_LIST );
		if( !hwndList )  return FALSE;
		Editor_Info( m_hWnd, EI_INIT_LISTVIEW, (LPARAM)hwndList );
		DWORD dwFlags = LVS_EX_FULLROWSELECT | LVS_EX_ONECLICKACTIVATE | LVS_EX_LABELTIP | LVS_EX_ONECLICKACTIVATE | LVS_EX_UNDERLINEHOT;
		ListView_SetExtendedListViewStyleEx( hwndList, dwFlags, dwFlags );
		InsertColumnToListView( hwndList, IDS_SEARCH_FILE, IDS_SEARCH_LINE );
		SendDlgItemMessage( hwnd, IDC_FIND, EM_LIMITTEXT, MAX_PATH - 1, 0 );
		int dpi = MyGetDpiForWindow( hwndList );
		m_nDpi = dpi;
		if( IsWindowVisible( m_hWnd ) ){
			return TRUE;
		}
		Editor_Info( m_hWnd, EI_WM_INITDIALOG, (LPARAM)hwnd );
		return FALSE;  // return FALSE to prevent focus
	}

	void OnDlgCommand( HWND hwnd, WORD wID )
	{
		switch( wID ){
		case IDC_SEARCH:
			OnSearch( hwnd );
			break;
		}


	}

	bool IsPathEqual( LPCWSTR szPath1, LPCWSTR szPath2 )
	{
//		OSVERSIONINFO osvi;
//		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
//#pragma warning( push )
//#pragma warning( disable : 4996 ) // warning C4996: 'GetVersionExW': was declared deprecated
//		VERIFY(GetVersionEx(&osvi));
//#pragma warning( pop )
//		bool bIsWindowsXPorLater = (osvi.dwMajorVersion > 5) || ((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion >= 1));
//		DWORD lcid = bIsWindowsXPorLater ? LOCALE_INVARIANT : MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
		return (CompareString(LOCALE_INVARIANT, NORM_IGNORECASE, szPath1, -1, szPath2, -1) == CSTR_EQUAL);
	}

	void Jump( LPCWSTR pszPath, INT_PTR nLine )
	{
		UINT nCount = (UINT)Editor_Info( m_hWnd, EI_GET_DOC_COUNT, FLAG_REAL_DOC_INDEX );
		for( UINT iDoc = 0; iDoc != nCount; iDoc++ ){
			HEEDOC hDoc = (HEEDOC)Editor_Info( m_hWnd, EI_INDEX_TO_DOC_REAL, iDoc );
			WCHAR szTitle[MAX_PATH];
			Editor_DocInfoEx( m_hWnd, hDoc, EI_GET_SAVE_AS_TITLEW, (LPARAM)szTitle );

			if( IsPathEqual( pszPath, szTitle ) ){
				Editor_Info( m_hWnd, EI_SET_ACTIVE_DOC, (LPARAM)hDoc );
				POINT_PTR ptPos;
				ptPos.x = 0;
				ptPos.y = nLine;
				Editor_SetCaretPos( m_hWnd, POS_LOGICAL_W, &ptPos );
				Editor_ExecCommand( m_hWnd, EEID_ACTIVE_PANE );

				UINT nFlags = FLAG_FIND_NEXT;
				if( m_bCase )  nFlags |= FLAG_FIND_CASE;
				if( m_bRegex )  nFlags |= FLAG_FIND_REG_EXP;
				Editor_FindW( m_hWnd, nFlags, m_szFind );
				break;
			}
		}
	}

	void OnJump( int iItem )
	{
		HWND hwndList = GetDlgItem( m_hDlg, IDC_LIST );
		if( !hwndList )  return;
		WCHAR szPath[MAX_PATH];
		ListView_GetItemText( hwndList, iItem, 0, szPath, _countof( szPath ) );
		WCHAR szLine[100];
		ListView_GetItemText( hwndList, iItem, 1, szLine, _countof( szLine ) );
		INT_PTR nLine = (INT_PTR)_wtoi64( szLine );
		if( nLine > 0 ){
			nLine--;
			Jump( szPath, nLine );
		}
	}

	BOOL OnPropInitDialog( HWND hwnd )
	{
		CenterWindow( hwnd );
		WCHAR sz[80];
		for( int i = 0; i < 4; i++ ){
			LoadString( EEGetLocaleInstanceHandle(), IDS_POS_LEFT + i, sz, _countof( sz ) );
			SendDlgItemMessage( hwnd, IDC_COMBO_POS, CB_ADDSTRING, 0, (LPARAM)sz );
		}
		SendDlgItemMessage( hwnd, IDC_COMBO_POS, CB_SETCURSEL, m_iPos, 0 );
		LoadString( EEGetLocaleInstanceHandle(), IDS_SEARCH_MENU_TEXT, sz, _countof( sz ) );
		SetWindowText( hwnd, sz );
		m_iOldPos = m_iPos;
		Editor_Info( m_hWnd, EI_WM_INITDIALOG, (LPARAM)hwnd );
		return TRUE;
	}

	void OnPropDlgCommand( HWND hwnd, WPARAM wParam )
	{
		switch( wParam ){
		case IDOK:
			{
				m_iPos = (int)SendDlgItemMessage( hwnd, IDC_COMBO_POS, CB_GETCURSEL, 0, 0 );
				WriteProfileInt( L"CustomBarPos", m_iPos );
				EndDialog( hwnd, IDOK );

				if( m_iPos != m_iOldPos ){
					if( m_hDlg ){
						OnCommand( NULL );
						OnCommand( NULL );
					}
				}
			}
			break;

		case IDCANCEL:
			EndDialog( hwnd, IDCANCEL );
			break;
		}
		return;
	}

	void SetTreeColor()
	{
		if( m_hDlg != NULL ) {
			HWND hwndList = GetDlgItem( m_hDlg, IDC_LIST );
			_ASSERT( hwndList );
			if( hwndList ) {
				DWORD dwColor = (DWORD)Editor_Info( m_hWnd, EI_GET_BAR_BACK_COLOR, SMART_COLOR_NORMAL );
				ListView_SetBkColor( hwndList, dwColor );
				ListView_SetTextBkColor( hwndList, dwColor );
				dwColor = (DWORD)Editor_Info( m_hWnd, EI_GET_BAR_TEXT_COLOR, SMART_COLOR_NORMAL );
				ListView_SetTextColor( hwndList, dwColor );
				RedrawWindow( hwndList, NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ERASENOW | RDW_UPDATENOW );
			}
		}
	}

	void OnDpiChangedAfter()
	{
		HWND hwndList = GetDlgItem( m_hDlg, IDC_LIST );
		if( !hwndList )  return;

		_ASSERT( m_nDpi != 0 );
		if( m_nDpi != 0 ) {
			int nNewDpi = MyGetDpiForWindow( m_hWnd );
			//if( nNewDpi > m_nDpi ) {
			for( WORD wCol = 0; wCol < 2; wCol++ ) {
				int nWidth = ListView_GetColumnWidth( hwndList, wCol );
				ListView_SetColumnWidth( hwndList, wCol, MulDiv( nWidth, nNewDpi, m_nDpi ) );
			}
			//}
			m_nDpi = nNewDpi;
		}
	}

};


INT_PTR CALLBACK SearchProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	BOOL bResult = FALSE;
	switch( msg ){
	case WM_INITDIALOG:
		{
			CMyFrame* pFrame = static_cast<CMyFrame*>(GetFrame( hwnd ));
			bResult = pFrame->OnInitDialog( hwnd );
		}
		break;
	case WM_COMMAND:
		{
			CMyFrame* pFrame = static_cast<CMyFrame*>(GetFrame( hwnd ));
			pFrame->OnDlgCommand( hwnd, LOWORD( wParam ) );
		}
		break;
	case WM_SIZE:
		{
			RECT rcDlg, rcChild;
			HWND hwndChild;
			if( GetClientRect( hwnd, &rcDlg ) ){
				int cxDlg = rcDlg.right;
				int cyDlg = rcDlg.bottom;
				hwndChild = GetDlgItem( hwnd, IDC_FIND );
				GetWindowRect( hwndChild, &rcChild );
				ScreenToClient( hwnd, (POINT*)&rcChild.left );
				ScreenToClient( hwnd, (POINT*)&rcChild.right );
				int cxEdge = rcChild.left;
				SetWindowPos( hwndChild, NULL, 0, 0, cxDlg - cxEdge * 2, rcChild.bottom - rcChild.top, SWP_NOMOVE | SWP_NOZORDER );

				hwndChild = GetDlgItem( hwnd, IDC_SEARCH );
				GetWindowRect( hwndChild, &rcChild );
				ScreenToClient( hwnd, (POINT*)&rcChild.left );
				ScreenToClient( hwnd, (POINT*)&rcChild.right );
				SetWindowPos( hwndChild, NULL, cxDlg - (rcChild.right - rcChild.left) - cxEdge, rcChild.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER );

				hwndChild = GetDlgItem( hwnd, IDC_LIST );
				GetWindowRect( hwndChild, &rcChild );
				ScreenToClient( hwnd, (POINT*)&rcChild.left );
				ScreenToClient( hwnd, (POINT*)&rcChild.right );
				SetWindowPos( hwndChild, NULL, 0, 0, cxDlg - cxEdge * 2, cyDlg - rcChild.top - cxEdge, SWP_NOMOVE | SWP_NOZORDER );

			}
		}
		break;
	case WM_NOTIFY:
		{
			NMHDR* pnmh = (NMHDR*)lParam;
			switch( pnmh->code ){
			//case NM_CLICK:
			case LVN_ITEMACTIVATE:
				{
					NMITEMACTIVATE* pNMItemActivate = (NMITEMACTIVATE*)pnmh;
					if( pNMItemActivate->iItem >= 0 ){
						CMyFrame* pFrame = static_cast<CMyFrame*>(GetFrame( hwnd ));
						pFrame->OnJump( pNMItemActivate->iItem );
					}
				}
				break;
			}
		}
		break;
	case WM_DPICHANGED_AFTERPARENT:
		{
			CMyFrame* pFrame = static_cast<CMyFrame*>(GetFrame( hwnd ));
			if( pFrame ){
				pFrame->OnDpiChangedAfter();
			}
		}
		break;
	case WM_CTLCOLORDLG:
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORBTN:
	case WM_CTLCOLORLISTBOX:
		{
			CMyFrame* pFrame = static_cast<CMyFrame*>( GetFrame( hwnd ) );
			if( pFrame ) {
				return Editor_Info( pFrame->m_hWnd, EI_WM_CTLCOLOR, wParam );
			}
		}
		break;
	case WM_THEMECHANGED:
		{
			CMyFrame* pFrame = static_cast<CMyFrame*>( GetFrame( hwnd ) );
			if( pFrame ) {
				Editor_Info( pFrame->m_hWnd, EI_WM_THEMECHANGED, (LPARAM)hwnd );
			}
		}
		break;
	}
	return bResult;

}

INT_PTR CALLBACK PropDlg( HWND hwnd, UINT msg, WPARAM wParam, LPARAM /*lParam*/ )
{
	BOOL bResult = FALSE;
	switch( msg ){
	case WM_INITDIALOG:
		{
			CMyFrame* pFrame = static_cast<CMyFrame*>(GetFrameFromDlg( hwnd ));
			_ASSERTE( pFrame );
			bResult = pFrame->OnPropInitDialog( hwnd );
		}
		break;
	case WM_COMMAND:
		{
			CMyFrame* pFrame = static_cast<CMyFrame*>(GetFrameFromDlg( hwnd ));
			_ASSERTE( pFrame );
			pFrame->OnPropDlgCommand( hwnd, wParam );
		}
		break;
	case WM_CTLCOLORDLG:
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORBTN:
	case WM_CTLCOLORLISTBOX:
		{
			CMyFrame* pFrame = static_cast<CMyFrame*>( GetFrame( hwnd ) );
			if( pFrame ) {
				return Editor_Info( pFrame->m_hWnd, EI_WM_CTLCOLOR, wParam );
			}
		}
		break;
	case WM_THEMECHANGED:
		{
			CMyFrame* pFrame = static_cast<CMyFrame*>( GetFrame( hwnd ) );
			if( pFrame ) {
				Editor_Info( pFrame->m_hWnd, EI_WM_THEMECHANGED, (LPARAM)hwnd );
			}
		}
		break;
	}
	return bResult;
}


// the following line is needed after CMyFrame definition
_ETL_IMPLEMENT

