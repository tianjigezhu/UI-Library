#include "global.h"

BEGIN_REGMSG(leftdlg)
	REGMSG(WM_INITDIALOG)
END_REGMSG

BEGIN_REGMSG(child)	
	REGMSG(WM_CREATE)
END_REGMSG

RINGMSG(leftdlg,WM_INITDIALOG)
{
	return FALSE;
}

RINGMSG(child,WM_CREATE)
{
	SetIcon(LoadIcon(GetInstance(),MAKEINTRESOURCE(IDI_MAIN)));	
	return DefaultProc(param);
	//Show(SW_MAXIMIZE);
	//return res;
}

