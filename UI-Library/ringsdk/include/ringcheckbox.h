/*
#ifndef _INC_RINGCHECKBOX
#define _INC_RINGCHECKBOX

class RingCHECKBOX:public RingControls
{
public:
	RingCHECKBOX(HWND hParent,UINT uId = 0);
   ~RingCHECKBOX();

	LRESULT CALLBACK WINAPI DefCtrlProc(HWND hWnd,UINT uMsg,UINT wParam,LONG lParam);

   void SetStyle(int style){m_style = style;};
   void Init(HWND hWnd);

   BOOL Create(LPCTSTR szCaption,DWORD dwStyle,int x,int y,int width,int height);

   void SetFlatBorder(COLORREF left,COLORREF top,COLORREF right,COLORREF bottom);
	void SetFocusBorder(COLORREF left,COLORREF top,COLORREF right,COLORREF bottom);
   void SetBackground(COLORREF crBkg);

public:
	WNDPROC m_preProc;
   HWND m_hParent;
   int m_style;
   UINT m_styleSelf;

private:
	BOOL bPainted,bLocked,bTimerSetted;
   RECT rcItem;
   HDC hDC;
   HPEN hpen;
   int timerId;
   POINT pt;
   int currStyle,currState;
   COLORREF m_borderCol[8];
};
#endif
*/
