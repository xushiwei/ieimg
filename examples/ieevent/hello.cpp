//
// hello.cpp

#include "stdafx.h"
#include "resource.h"

// -------------------------------------------------------------------------
// CHelloDlg

class CHelloDlg : 
	public winx::AxModalDialog<CHelloDlg, IDD_HELLO>,
	public DWebBrowserEvents2Disp<CHelloDlg>
{
	WINX_CMDS_BEGIN()
		WINX_CMD(ID_NAV, DoNav)
	WINX_CMDS_END();

private:
	winx::AxCtrlHandle m_ax;
	CComPtr<IWebBrowser2> m_spBrowser;
	CComQIPtr<IViewObject2> m_spViewObject;
	BOOL m_bFired;
	
public:
	VOID __stdcall WebBrowser_OnDocumentComplete(
		IN IDispatch* pDisp, IN VARIANT* URL)
	{
		WINX_TRACE("WebBrowser_OnDocumentComplete - Thread: %x\n", GetCurrentThreadId());

		if (m_bFired)
			return;

		m_bFired = TRUE;

		HBITMAP hbm = SaveToImage();

		winx::SaveFileDialog dlg(
			_T("Image files(*.png;*.jpg;*.jpeg)\0*.png;*.jpg;*.jpeg\0"), _T("*.png"));
		
		if (IDOK == dlg.DoModal(m_hWnd))
		{
			Gdiplus::Bitmap bitmap(hbm, NULL);
			WINX_ASSERT(bitmap.GetLastStatus() == Gdiplus::Ok);
			
			USES_CONVERSION;

			LPCSTR lpszExt = PathFindExtensionA(dlg.lpstrFile);
			LPCWSTR lpszEncode = stricmp(lpszExt, ".png") == 0 ? L"image/png" : L"image/jpeg";
			Gdiplus::QualityParameter quality = 80;
			Gdiplus::SaveImage(bitmap, T2CW(dlg.lpstrFile), lpszEncode, &quality);

			ShellExecuteA(m_hWnd, "open", dlg.lpstrFile, NULL, NULL, SW_SHOW);
		}
		
		::DeleteObject(hbm);
	}

	void DoNav(HWND hWnd)
	{
		WINX_TRACE("DoNav - Thread: %x\n", GetCurrentThreadId());

		BSTR bstrURL = NULL;
		GetDlgItemText(IDC_URL, bstrURL);

		long cx = ::GetDlgItemInt(hWnd, IDC_CX, NULL, TRUE);
		long cy = ::GetDlgItemInt(hWnd, IDC_CY, NULL, TRUE);
		if (cx == 0)
			cx = 1024;
		if (cy == 0)
			cy = 768*2;

		m_ax.MoveWindow(0, 0, cx, cy, FALSE);

		CComVariant v;
		m_bFired = FALSE;
		m_spBrowser->Navigate(bstrURL, &v, &v, &v, &v);
		SysFreeString(bstrURL);
	}

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

	BOOL OnInitDialog(HWND hDlg, HWND hWndDefaultFocus)
	{
		m_ax.DlgItem(hDlg, IDC_EXPLORER1);
		m_bFired = TRUE;
		
		CComPtr<IAxWinAmbientDispatch> spHost;
		m_ax.QueryHost(&spHost);
		spHost->put_DocHostFlags(
			DOCHOSTUIFLAG_NO3DBORDER | 
			DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE | DOCHOSTUIFLAG_THEME);
			
		m_ax.QueryControl(&m_spBrowser);
		m_spViewObject = m_spBrowser;
		m_spBrowser->put_Silent(VARIANT_TRUE);

		DWebBrowserEvents2Disp<CHelloDlg>::Advise(m_spBrowser);
		//本例由于对话框的生命周期比m_spBrowser长，故此退出时可以不调用Unadvise()。
		
		return TRUE;
	}
};

// -------------------------------------------------------------------------

CComModule _Module;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	InitCommonControls();
	GdiplusAppInit gdiplus;
	CComModuleInit module;
	CHelloDlg dlg;
	dlg.DoModal();
	return 0;
}

// -------------------------------------------------------------------------
