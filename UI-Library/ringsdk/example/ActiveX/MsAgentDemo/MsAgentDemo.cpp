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
//							RingSDK演示程序
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
//原文件名：msagentdemo.cpp
//																						 
//说明：演示对ActiveX控件MsAgent(助手)的调用。
//																						 
*********************************************************************/
#include "ringcontainer.h"
																	 
//#import "C:\WINNT.0\msagent\agentctl.dll"			 
//利用上面的import编译后就会产生tlh和tlb文件，以后只要include tlh就可以，
//否则每次编译都会重新生成tlh和tlb文件。				 
#include "agentctl.tlh"										 
using namespace AgentObjects;								 
																	 
BEGIN_SETUPMAINWINDOW(IS_WINDOW)							 
	//需要支持ActiveX，必须声明BEGIN_DEF_OCXPTR，	 
	//该宏实现了CreateContainer函数(创建ActiveX容器)
	BEGIN_DEF_OCXPTR
		DEFCONTAINER(AgentCon);
		RINGOCXPTR(IAgentCtlEx) gAgent;										
		IAgentCtlCharactersPtr pChars;										
		IAgentCtlCharacterExPtr pMerlin;										
		IAgentCtlRequestPtr pRequest;											
		IConnectionPoint *pCP;													
		IUnknown *pSinkUnk;														
		DWORD dwCookie;															
	END_DEF_OCXPTR																	
																						
	BEGIN_DEFMSG																	
		DEFMSG(WM_CREATE)																
		DEFMSG(WM_CLOSE)																
	END_DEFMAINMSG																	
END_SETUPMAINWINDOW																
																						
BEGIN_REGMAINMSG																	
	REGMSG(WM_CREATE)																	
	REGMSG(WM_CLOSE)																	
END_REGMSG																			
																						
//助手事件处理类
class CSink : public IDispatch												
{																						
public:																				
	// 构造/析构函数																
	CSink():m_uRef(0){}
	virtual ~CSink() {}
	// IUnknown接口实现															
	STDMETHODIMP QueryInterface( REFIID iid, void **ppvObject )
	{
		if ( iid == __uuidof( _AgentEvents ) )
		{
			*ppvObject = (_AgentEvents *)this;
			AddRef();
			return S_OK;
		}
		if ( iid == IID_IUnknown )
		{
			*ppvObject = (IUnknown *)this;
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}
	ULONG STDMETHODCALLTYPE AddRef()
	{
		m_uRef++;
		return m_uRef;
	}
	ULONG STDMETHODCALLTYPE Release()
	{
		ULONG u = m_uRef--;
		if ( 0 == m_uRef )
		{
			delete this;
		}
		return u;
	}
	// IDispatch接口实现
	STDMETHODIMP GetTypeInfoCount( UINT *pctinfo )
	{
		return E_NOTIMPL;
	}
	STDMETHODIMP GetTypeInfo( UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
	{
		return E_NOTIMPL;
	}
	STDMETHODIMP GetIDsOfNames( REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
	{
		return E_NOTIMPL;
	}
	STDMETHODIMP Invoke(DISPID dispIdMember,REFIID riid,LCID lcid,WORD wFlags,
		DISPPARAMS *pDispParams,VARIANT *pVarResult,
		EXCEPINFO *pExcepInfo,UINT *puArgErr)
	{
		HRESULT hr = S_OK;
		
		if ( NULL != pDispParams)
		{
			if(7 == dispIdMember && 2 == pDispParams->cArgs)
			{
				if(VT_I2 == pDispParams->rgvarg[0].vt && VT_BSTR == pDispParams->rgvarg[1].vt)
				{
					OnHide( pDispParams->rgvarg[1].bstrVal, pDispParams->rgvarg[0].iVal );
				}
				else // 类型错误
				{
					hr = DISP_E_TYPEMISMATCH;
				}
			}
			else if(dispIdMember == 6)
			{
				if(pDispParams->cArgs == 5)
				OnDragComplete(pDispParams->rgvarg[4].bstrVal,
					pDispParams->rgvarg[3].iVal,
					pDispParams->rgvarg[2].iVal,
					pDispParams->rgvarg[1].iVal,
					pDispParams->rgvarg[0].iVal);
			}
			else // 参数个数错误
			{
				hr = DISP_E_BADPARAMCOUNT;
			}
		}
		return hr;
	}
	// 要处理的_AgentEvents事件
	STDMETHODIMP OnHide( _bstr_t CharacterID, short Cause )
	{
		PostMessage(GetMainWnd(),WM_CLOSE,0,0);
		return S_OK;
	}
	STDMETHODIMP OnDragComplete(_bstr_t CharacterID,short Button,short Shift,short x,short y)
	{
		window->pMerlin->Speak(CComVariant("轻点，轻点，别乱扔。"));
		return S_OK;
	}
private:
	ULONG m_uRef;
};

int RingMain()
{
	InitRingContainer();

	APP_CREATEWINDOW("MsAgentDemo","RINGSDK_COMAGENTCLASS",WS_OVERLAPPEDWINDOW,(LPSTR)NULL);
	RingApp.Run(FALSE);

	return FreeRingContainer();
}

RINGMAINMSG(WM_CREATE)
{
	//创建包容器
	if(CreateContainer(AgentCon,OLESTR("{D45FD31B-5C6E-11D1-9EC1-00C04FD7081F}"),0))
	{
		HRESULT hr = AgentCon.QueryControl(__uuidof(IAgentCtlEx),(void**)&gAgent);
	
		if(SUCCEEDED(hr))
		{
			pCP = NULL;
			pSinkUnk = NULL;
			dwCookie = 0;

			// 获取角色文件路径
			TCHAR szPath[MAX_PATH];
			GetWindowsDirectory( szPath, MAX_PATH );
			lstrcat(szPath, _T("\\msagent\\chars\\merlin.acs"));
			
			// 进行连接
			gAgent->PutConnected((VARIANT_BOOL)-1);
			
			// 设置连接点的过程开始
			IConnectionPointContainer *pCPC = NULL;
			// 查询连接点容器
			hr = gAgent->QueryInterface( IID_IConnectionPointContainer, (void **)&pCPC );
			if ( FAILED( hr ) )
			{
				Errmsg("查询连接点容器失败！");
				return 0;
			}
			// 查找连接点
			hr = pCPC->FindConnectionPoint(__uuidof(_AgentEvents),&pCP);
			if(FAILED(hr))
			{
				Errmsg("查找连接点失败！");
				return 0;
			}
			// 这时连接点容器已经没用了，释放之
			pCPC->Release();
			pCPC = NULL;
			// 创建通知对象
			CSink* pSink = new CSink;
			hr = pSink->QueryInterface(IID_IUnknown,(void **)&pSinkUnk);
			if(FAILED(hr))
			{
				Errmsg("接口查询失败！");
				return 0;
			}
			// 对连接点进行设置
			hr = pCP->Advise(pSinkUnk,&dwCookie);
			if ( FAILED( hr ) )
			{
				Errmsg("连接点设置失败！");
				return 0;
			}

			// 获得角色列表
			pChars = gAgent->GetCharacters();
			// 装载角色
			pRequest = pChars->Load( _bstr_t("merlin"), CComVariant(szPath) );
			pMerlin = pChars->Character( _bstr_t("merlin") );
			//pMerlin->put_AutoPopupMenu((VARIANT_BOOL)0);
			// 显示角色
			pMerlin->Show();
			// 计算屏幕中央坐标，并移动
			short x = ( GetSystemMetrics( SM_CXFULLSCREEN ) - pMerlin->GetWidth() ) / 2;
			short y = ( GetSystemMetrics( SM_CYFULLSCREEN ) - pMerlin->GetHeight() ) / 2;
			pRequest = pMerlin->MoveTo( x, y );
			pRequest = pMerlin->Speak( CComVariant("右键单击我，选择“隐藏”以结束程序。") );
		}
	}
	return 0;
}

RINGMAINMSG(WM_CLOSE)
{
	pCP->Unadvise(dwCookie);
	pCP->Release();
	// 由于pSinkUnk进行了QueryInterface，所以必须Release之
	pSinkUnk->Release();
	DestroyWindow(m_hWnd);
	return Quit();
}
