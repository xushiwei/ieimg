//
// ieimg/Service.h

#ifndef IEIMG_SERVICE_H
#define IEIMG_SERVICE_H

#ifndef IEIMG_URLTOIMAGE_H
#include "URLToImage.h"
#endif

// -------------------------------------------------------------------------
// class IeimgService

class IeimgService : public URLToImageCallback
{
private:
	DWORD m_threadId;
	HANDLE m_hEvent;

	UINT WM_RESIZEWIN;
	UINT WM_URLTOIMAGE;
	HBITMAP result;

private:
	virtual void __stdcall Done(
		BSTR bstrURL, HBITMAP hbmResult, BOOL* doFreeBitmap)
	{
		result = hbmResult;
		::SetEvent(m_hEvent);
		*doFreeBitmap = FALSE;
	}

private:
	static DWORD CALLBACK Main(void* param)
	{
		::OleInitialize(NULL);
		{
			IeimgService* service = (IeimgService*)param;
			URLToImage ieimg(service);
			ieimg.Init();
			for(;;)
			{
				MSG msg;
				const BOOL bRet = ::GetMessage(&msg, NULL, 0, 0);
				if (bRet == -1)
					continue;
				if (!bRet)
				{
					::OleUninitialize();
					::SetEvent(service->m_hEvent);
					return 0;
				}
				if (msg.message == service->WM_URLTOIMAGE)
				{
					ieimg.Navigate((LPCWSTR)msg.lParam);
				}
				else if (msg.message == service->WM_RESIZEWIN)
				{
					ieimg.ResizeWindow(msg.wParam, msg.lParam);
				}
				else if (msg.message == WM_CLOSE && msg.hwnd == NULL)
				{
					ieimg.Term();
					PostQuitMessage(0);
				}
				else
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
			}
		}
	}

	void RunService()
	{
		const HANDLE hThread = ::CreateThread(NULL, 0, Main, this, 0, &m_threadId);
		::CloseHandle(hThread);
	}

public:
	IeimgService()
		: m_hEvent(NULL)
	{
		WM_URLTOIMAGE = ::RegisterWindowMessageA("ieimg.WM_URLTOIMAGE");
		WM_RESIZEWIN = ::RegisterWindowMessageA("ieimg.WM_RESIZEWIN");
	}

	void Init()
	{
		if (!m_hEvent)
		{
			RunService();
			m_hEvent = ::CreateEvent(NULL, 0, 0, NULL);
		}
	}

	void Term()
	{
		if (m_hEvent)
		{
			::PostThreadMessageA(m_threadId, WM_CLOSE, 0, 0);
			::WaitForSingleObject(m_hEvent, INFINITE);
			m_threadId = 0;

			::DeleteObject(m_hEvent);
			m_hEvent = NULL;
		}
	}

	void ResizeWindow(int cx, int cy)
	{
		int i = 0;
		while (!::PostThreadMessageA(m_threadId, WM_RESIZEWIN, cx, cy) && ++i < 5)
			::Sleep(1000);
	}
	
	HBITMAP ToImage(LPCSTR url)
	{
		USES_CONVERSION;
		return ToImage(A2CW(url));
	}

	HBITMAP ToImage(LPCWSTR url)
	{
		while (!::PostThreadMessageA(m_threadId, WM_URLTOIMAGE, 0, (LPARAM)url))
			::Sleep(1000);
		::WaitForSingleObject(m_hEvent, INFINITE);
		return result;
	}
};

// -------------------------------------------------------------------------

#endif // IEIMG_SERVICE_H
