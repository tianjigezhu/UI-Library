/**********************************************************************
//
//
//        ##########                          ######  #########   # ######    #
//      #############                      ########### ######### #########  ###
//     ######## # ###  ##                 ############# ##    ## #####  # ####
//     ####  ##    ## ###                 ###     ### # #      #####   #####
//          #     ###  #            #     ##       ##  ##      ##     ###
//         ## ###### ##      ##  ####    ####          #       #     ##
//       #########  ###  ## ### #######   ######      ##      ##    ###
//      ######      ##  ######  ##  ##       ####     #      ##     ####
//     #######     ##   ###### ##  ###          ##   ##     ###    ######
//     #########   ##  ###### ## ######         ### ##    ###      #  #####
//    ##    ###### ####### ### #### ##  ## #######  ########      ##    ####
//    ##      #### ###  #  ### ### ##  ##########   ######       ##      ####
//   ##         ##                ##   #########    ####         #         ##
//               #              ###
//                              ##
//                             ###
//                             ##
//
//
//							RingSDK类库 ringmain.lib
//作者：临风
//
//版本：1.0
//
//声明：本类库可以自由使用而不须对作者作出任何回报，但作者希望能得到
//		  你的鼓励和支持。你可以对类库源码作出修改和改进，但希望你能在
//		  修改的同时给作者一份同样的副本。
//		  本类库不得用于任何商业用途，如确实需要，请与作者联系。
//
//e-mail:ringphone@sina.com
//
//原文件名：ringmain.h
//
//说明：对WinMain的封装，程序界面库将基于本类库，包含本文件无须实现
//      WinMain，主函数为RingMain，必须自己实现。
//
**********************************************************************/
#ifndef _INC_RINGCONTAINER
#define _INC_RINGCONTAINER

#include "ringmain.h"

#include <atlbase.h>
// Required ATL module
extern CComModule _Module;

#include <atlcom.h>
#include <atlwin.h>
#include <atlhost.h>

extern "C"
{
	void InitRingContainer();
	int FreeRingContainer();
	//BOOL CreateContainer();
}

//以下的宏需要放在窗口设置宏之中，即BEGIN_SETUPWINDOW...END_SETUPWINDOW之间
#define BEGIN_DEF_OCXPTR \
	BOOL CreateContainer(CAxWindow2& Container,LPCOLESTR ocxName,UINT uStyle=WS_HSCROLL|WS_VSCROLL,LPRECT lprc=NULL)\
	{\
		RECT rect;\
		if(lprc)\
			CopyRect(&rect,lprc);\
		else\
			GetClientRect(&rect);\
		HWND hwndContainer = Container.Create(m_hWnd,&rect,0,WS_CHILD|WS_VISIBLE|uStyle);\
		if(!hwndContainer) return FALSE;\
   	HRESULT hr = Container.CreateControl(ocxName);\
		return SUCCEEDED(hr);\
	}\

#define DEFCONTAINER(container)	CAxWindow2 container
#define RINGHTML	CComPtr<IWebBrowser2>
#define RINGOCXPTR(ptr)  CComPtr<ptr>
#define END_DEF_OCXPTR

template <typename TBase = CWindow>
class CAxWindowT2 : public CAxWindowT<TBase>
{
public:
	CAxWindowT2(HWND hwnd = 0):CAxWindowT<TBase>(hwnd){}

	BOOL IsCreated(){return (m_hWnd && ::IsWindow(m_hWnd));}
	
   HRESULT CreateControl(LPCOLESTR lpszName, IStream* pStream = NULL, IUnknown** ppUnkContainer = NULL)
   {
		return CreateControlEx(lpszName, pStream, ppUnkContainer);
   }
    
   HRESULT CreateControl(DWORD dwResID, IStream* pStream = NULL, IUnknown** ppUnkContainer = NULL)
   {
		return CreateControlEx(dwResID, pStream, ppUnkContainer);
   }
    
   HRESULT CreateControlEx(DWORD dwResID,  IStream* pStream = NULL, 
                            IUnknown** ppUnkContainer = NULL, IUnknown** ppUnkControl = NULL,
                            REFIID iidSink = IID_NULL, IUnknown* punkSink = NULL)
   {
      TCHAR szModule[_MAX_PATH];
      GetModuleFileName(_Module.GetModuleInstance(), szModule, _MAX_PATH);
        
      CComBSTR bstrURL(OLESTR("res://"));
      bstrURL.Append(szModule);
      bstrURL.Append(OLESTR("/"));
      TCHAR szResID[11];
      wsprintf(szResID, _T("%0d"), dwResID);
      bstrURL.Append(szResID);
        
      return CreateControlEx(bstrURL, pStream, ppUnkContainer, ppUnkControl, iidSink, punkSink);
   }
    
   HRESULT CreateControlEx(LPCOLESTR lpszName, IStream* pStream = NULL, 
                            IUnknown** ppUnkContainer = NULL, IUnknown** ppUnkControl = NULL,
                            REFIID iidSink = IID_NULL, IUnknown* punkSink = NULL)
   {
      ATLASSERT(::IsWindow(m_hWnd));
        
      HRESULT hr = E_FAIL;
      CComPtr<IAxWinHostWindow> spAxWindow;
        
      // Reuse existing CAxHostWindow
      hr = QueryHost(&spAxWindow);
      if(SUCCEEDED(hr))
      {
         CComPtr<IUnknown> spunkControl;
         hr = spAxWindow->CreateControlEx(lpszName,m_hWnd,pStream,&spunkControl,iidSink,punkSink);
         if(FAILED(hr))
				return hr;
        
         if( ppUnkControl ) (*ppUnkControl = spunkControl)->AddRef();
         if( ppUnkContainer ) (*ppUnkContainer = spAxWindow)->AddRef();
      }
      // Create a new CAxHostWindow
      else
      {
			return AtlAxCreateControlEx(lpszName, m_hWnd, pStream, ppUnkContainer, ppUnkControl, iidSink, punkSink);
      }
      return S_OK;
	}
    
	HRESULT AttachControl(IUnknown* pControl, IUnknown** ppUnkContainer = 0)
   {
		ATLASSERT(::IsWindow(m_hWnd));
        
      HRESULT hr = E_FAIL;
      CComPtr<IAxWinHostWindow> spAxWindow;
        
      // Reuse existing CAxHostWindow
      hr = QueryHost(&spAxWindow);
      if(SUCCEEDED(hr))
      {
         hr = spAxWindow->AttachControl(pControl, m_hWnd);
         if(FAILED(hr))
				return hr;
        
         if(ppUnkContainer)(*ppUnkContainer = spAxWindow)->AddRef();
      }
      // Create a new CAxHostWindow
      else
      {
    		return AtlAxAttachControl(pControl,m_hWnd,ppUnkContainer);
      }
      return S_OK;
	}
};

typedef CAxWindowT2<CWindow> CAxWindow2;

#endif
