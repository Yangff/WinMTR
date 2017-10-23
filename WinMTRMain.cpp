//*****************************************************************************
// FILE:            WinMTRMain.cpp
//
//
// HISTORY:
//
//
//    -- versions 0.8
//
// - 01.18.2002 - Store LRU hosts in registry (v0.8)
// - 05.08.2001 - Replace edit box with combo box which hold last entered hostnames.
//				  Fixed a memory leak which caused program to crash after a long 
//				  time running. (v0.7)
// - 11.27.2000 - Added resizing support and flat buttons. (v0.6) 
// - 11.26.2000 - Added copy data to clipboard and posibility to save data to file as text or HTML.(v0.5) 
// - 08.03.2000 - added double-click on hostname for detailed information (v0.4)
// - 08.02.2000 - fix icmp error codes handling. (v0.3)
// - 08.01.2000 - support for full command-line parameter specification (v0.2) 
// - 07.30.2000 - support for command-line host specification 
//					by Silviu Simen (ssimen@ubisoft.ro) (v0.1b)
// - 07.28.2000 - first release (v0.1)
//*****************************************************************************

#include "WinMTRGlobal.h"
#include "WinMTRMain.h"
#include "WinMTRDialog.h"
#include "WinMTRHelp.h"
#include <algorithm>
#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

WinMTRMain WinMTR;

//*****************************************************************************
// BEGIN_MESSAGE_MAP
//
// 
//*****************************************************************************
BEGIN_MESSAGE_MAP(WinMTRMain, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

//*****************************************************************************
// WinMTRMain::WinMTRMain
//
// 
//*****************************************************************************
WinMTRMain::WinMTRMain()
{
}

//*****************************************************************************
// WinMTRMain::InitInstance
//
// 
//*****************************************************************************
BOOL WinMTRMain::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();
	
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#endif


	WinMTRDialog mtrDialog;
	m_pMainWnd = &mtrDialog;

	if (lstrlen(m_lpCmdLine)) {	
		lstrcat(m_lpCmdLine,L" ");
		ParseCommandLineParams(m_lpCmdLine, &mtrDialog);
	}

	int nResponse = mtrDialog.DoModal();


	return FALSE;
}


//*****************************************************************************
// WinMTRMain::ParseCommandLineParams
//
// 
//*****************************************************************************
void WinMTRMain::ParseCommandLineParams(LPTSTR cmd, WinMTRDialog *wmtrdlg)
{
	wchar_t value[1024];
	std::wstring host_name = L"";

	if(GetParamValue(cmd, L"help",'h', value)) {
		WinMTRHelp mtrHelp;
		m_pMainWnd = &mtrHelp;
		mtrHelp.DoModal();
		exit(0);
	}

	if(GetHostNameParamValue(cmd, host_name)) {
		wmtrdlg->SetHostName(host_name.c_str());
	}
	if(GetParamValue(cmd, L"interval",'i', value)) {
		wmtrdlg->SetInterval((float)_wtof(value));
		wmtrdlg->hasIntervalFromCmdLine = true;
	}
	if(GetParamValue(cmd, L"size",'s', value)) {
		wmtrdlg->SetPingSize(_wtoi(value));
		wmtrdlg->hasPingsizeFromCmdLine = true;
	}
	if(GetParamValue(cmd, L"maxLRU",'m', value)) {
		wmtrdlg->SetMaxLRU(_wtoi(value));
		wmtrdlg->hasMaxLRUFromCmdLine = true;
	}
	if(GetParamValue(cmd, L"numeric",'n', value)) {
		wmtrdlg->SetUseDNS(FALSE);
		wmtrdlg->hasUseDNSFromCmdLine = true;
	}
}

//*****************************************************************************
// WinMTRMain::GetParamValue
//
// 
//*****************************************************************************
int WinMTRMain::GetParamValue(LPTSTR cmd, wchar_t * param, wchar_t sparam, wchar_t *value)
{
	wchar_t *p;
	
	wchar_t p_long[1024];
	wchar_t p_short[1024];
	
	wsprintf(p_long, L"--%ws ", param);
	wsprintf(p_short, L"-%c ", sparam);
	
	if( (p= wcsstr(cmd, p_long)) ) ;
	else 
		p= wcsstr(cmd, p_short);

	if(p == NULL)
		return 0;

	if(wcscmp(param, L"numeric")==0) 
		return 1;

	while(*p && *p!=' ')
		p++;
	while(*p==' ') p++;
	
	int i = 0;
	while(*p && *p!=' ')
		value[i++] = *p++;
	value[i]='\0';

	return 1;
}

//*****************************************************************************
// WinMTRMain::GetHostNameParamValue
//
// 
//*****************************************************************************
int WinMTRMain::GetHostNameParamValue(LPTSTR cmd, std::wstring& host_name)
{
// WinMTR -h -i 1 -n google.com
	size_t size = lstrlen(cmd);
	std::wstring name = L"";
	while(cmd[--size] == ' ');

	size++;
	while(size-- && cmd[size] != ' ' && (cmd[size] != '-' || cmd[size - 1] != ' ')) {
		name = cmd[size ] + name;
	}

	if(size == -1) {
		if(name.length() == 0) {
			return 0;
		} else {
			host_name = name;
			return 1;
		}
	}
	if(cmd[size] == '-' && cmd[size - 1] == ' ') {
		// no target specified
		return 0;
	}

	std::wstring possible_argument = L"";

	while(size-- && cmd[size] != ' ') {
		possible_argument = cmd[size] + possible_argument;
	}

	if(possible_argument.length() && (possible_argument[0] != '-' || possible_argument == L"-n" || possible_argument == L"--numeric")) {
		host_name = name;
		return 1;
	}

	return 0;
}
