//
// ieimg/URLToImage.h

#ifndef IEIMG_URLTOIMAGE_H
#define IEIMG_URLTOIMAGE_H

#ifndef IEIMG_BASIC_H
#include "Basic.h"
#endif

// -------------------------------------------------------------------------
// interface URLToImageCallback

struct URLToImageCallback
{
	virtual void __stdcall Done(
		BSTR bstrURL, HBITMAP hbmResult, BOOL* doFreeBitmap) = 0;
};

// -------------------------------------------------------------------------
// class URLToImage

class URLToImage : public DWebBrowserEvents2Disp<URLToImage>
{
private:
	winx::AxCtrlHandle m_ax;
	CComQIPtr<IWebBrowser2> m_spBrowser;
	CComQIPtr<IViewObject2> m_spViewObject;
	BOOL m_bFired;
	URLToImageCallback* m_callback;
	CComBSTR m_bstrURL;

public:
	URLToImage(URLToImageCallback* callback)
		: m_callback(callback)
	{
	}
	
	BOOL ResizeWindow(int cx, int cy)
	{
		return m_ax.MoveWindow(0, 0, cx, cy, FALSE);
	}
	
	HRESULT Navigate(LPCWSTR szURL)
	{
		CComVariant v;
		m_bstrURL = szURL;
		m_bFired = FALSE;
		return m_spBrowser->Navigate(m_bstrURL, &v, &v, &v, &v);
	}
	
	HRESULT Navigate(LPCSTR szURL)
	{
		CComVariant v;
		m_bstrURL = szURL;
		m_bFired = FALSE;
		return m_spBrowser->Navigate(m_bstrURL, &v, &v, &v, &v);
	}
	
public:
	BOOL Init()
	{
		AtlAxWinInit();
		m_bFired = TRUE;
		
		m_ax.Create(NULL, NULL, _T("{8856F961-340A-11D0-A96B-00C04FD705A2}"));
		
		CComPtr<IAxWinAmbientDispatch> spHost;
		m_ax.QueryHost(&spHost);
		spHost->put_DocHostFlags(
			DOCHOSTUIFLAG_NO3DBORDER | 
			DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE | DOCHOSTUIFLAG_THEME);
		
		m_ax.QueryControl(&m_spBrowser);
		m_spBrowser->put_Silent(VARIANT_TRUE);
		m_spViewObject = m_spBrowser;
		
		DWebBrowserEvents2Disp<URLToImage>::Advise(m_spBrowser);
		
		return TRUE;
	}
	
	BOOL Good() const
	{
		return m_spBrowser != NULL;
	}
	
	void Term()
	{
		DWebBrowserEvents2Disp<URLToImage>::Unadvise(m_spBrowser);
		
		m_spViewObject.Release();
		m_spBrowser.Release();
		m_ax.DestroyWindow();
	}

private:
	HBITMAP SaveToImage()
	{
		winx::ClientDC hdc(m_ax.m_hWnd);
		
		RECTL rcView;
		m_ax.GetClientRect((RECT*)&rcView);
		
		HBITMAP hBitmap = ::CreateCompatibleBitmap(hdc, rcView.right, rcView.bottom);
		HDC hdcCompatible = ::CreateCompatibleDC(hdc);
		HGDIOBJ hBitmapOld = ::SelectObject(hdcCompatible, hBitmap);
		
		m_spViewObject->Draw(
			DVASPECT_CONTENT, -1, NULL, NULL, NULL,
			hdcCompatible, &rcView, NULL, NULL, NULL); 
		
		::SelectObject(hdcCompatible, hBitmapOld); 
		::DeleteDC(hdcCompatible);
		
		return hBitmap;
	}
	
public:
	VOID __stdcall WebBrowser_OnDocumentComplete(
		IN IDispatch* pDisp, IN VARIANT* URL)
	{
		if (m_bFired)
			return;
		
		m_bFired = TRUE;
		
		HBITMAP hbmResult = SaveToImage();
		BOOL doFreeBitmap = TRUE;
		m_callback->Done(m_bstrURL, hbmResult, &doFreeBitmap);
		if (doFreeBitmap)
			::DeleteObject(hbmResult);
	}
};

// -------------------------------------------------------------------------

#endif // IEIMG_URLTOIMAGE_H
