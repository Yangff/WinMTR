//*****************************************************************************
// FILE:            WinMTRMain.h
//
//
// DESCRIPTION:
//   
//
// NOTES:
//    
//
//*****************************************************************************

#ifndef WINMTRMAIN_H_
#define WINMTRMAIN_H_

#include "WinMTRDialog.h"
#include <string>


//*****************************************************************************
// CLASS:  WinMTRMain
//
//
//*****************************************************************************

class WinMTRMain : public CWinApp
{
public:
	WinMTRMain();

	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()

private:
	void	ParseCommandLineParams(LPTSTR cmd, WinMTRDialog *wmtrdlg);
	int		GetParamValue(LPTSTR cmd, wchar_t * param, wchar_t sparam, wchar_t *value);
	int		GetHostNameParamValue(LPTSTR cmd, std::wstring& value);

};

#endif // ifndef WINMTRMAIN_H_

