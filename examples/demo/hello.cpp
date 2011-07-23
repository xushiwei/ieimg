//
// hello.cpp

#include "stdafx.h"
#include <ieimg/Service.h>
#include <shellapi.h>

// -------------------------------------------------------------------------

CComModule _Module;

int main()
{
	//_CrtSetBreakAlloc(49);
	EnableMemoryLeakCheck();
	char url[256];
	CComModuleInit module;
	GdiplusAppInit gdiplus;
	IeimgService service;
	service.Init();
	service.ResizeWindow(1024, 768);
	for (;;)
	{
		gets(url);
		if (strcmp(url, "q") == 0)
			break;

		HBITMAP hbm = service.ToImage(url);

		winx::SaveFileDialog dlg(
			_T("Image files(*.png;*.jpg;*.jpeg)\0*.png;*.jpg;*.jpeg\0"), _T("*.png"));
		
		if (IDOK == dlg.DoModal())
		{
			Gdiplus::Bitmap bitmap(hbm, NULL);
			WINX_ASSERT(bitmap.GetLastStatus() == Gdiplus::Ok);
			
			USES_CONVERSION;
			
			LPCSTR lpszExt = PathFindExtensionA(dlg.lpstrFile);
			LPCWSTR lpszEncode = stricmp(lpszExt, ".png") == 0 ? L"image/png" : L"image/jpeg";
			Gdiplus::QualityParameter quality = 80;
			Gdiplus::SaveImage(bitmap, T2CW(dlg.lpstrFile), lpszEncode, &quality);
			
			ShellExecuteA(NULL, "open", dlg.lpstrFile, NULL, NULL, SW_SHOW);
		}
		
		::DeleteObject(hbm);
	}
	service.Term();
	return 0;
}

// -------------------------------------------------------------------------
