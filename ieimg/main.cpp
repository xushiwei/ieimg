//
// main.cpp

#include "stdafx.h"
#include <ieimg.h>

CComModule _Module;

ULONG_PTR g_token;
CLSID g_clsid[ieimg_format_max];

STDAPI_(void) ieimg_init()
{
	_Module.Init(NULL, GetThisModule(), NULL);

	Gdiplus::GdiplusStartupInput input;
	Gdiplus::GdiplusStartupOutput output;
	Gdiplus::GdiplusStartup(&g_token, &input, &output);

	Gdiplus::GetEncoderClsid(L"image/png", &g_clsid[ieimg_png]);
	Gdiplus::GetEncoderClsid(L"image/jpeg", &g_clsid[ieimg_jpeg]);
}

STDAPI_(void) ieimg_term()
{
	Gdiplus::GdiplusShutdown(g_token);
	_Module.Term();
}

STDAPI_(ieimg_service*) ieimg_new()
{
	IeimgService* service = new IeimgService;
	service->Init();
	return (ieimg_service*)service;
}

STDAPI_(void) ieimg_delete(ieimg_service* p)
{
	IeimgService* service = (IeimgService*)p;
	service->Term();
	delete service;
}

STDAPI_(void) ieimg_resize(ieimg_service* p, int cx, int cy)
{
	IeimgService* service = (IeimgService*)p;
	service->ResizeWindow(cx, cy);
}

STDAPI_(HBITMAP) ieimg_render(ieimg_service* p, LPCWSTR url)
{
	IeimgService* service = (IeimgService*)p;
	return service->ToImage(url);
}

STDAPI_(int) ieimg_save(
	ieimg_service* p, LPCWSTR url, LPCWSTR file,
	ieimg_format format, int quality /* =80 */)
{
	if (format >= ieimg_format_max)
		return -1;

	IeimgService* service = (IeimgService*)p;
	HBITMAP hbm = service->ToImage(url);
	{
		Gdiplus::Bitmap bitmap(hbm, NULL);
		Gdiplus::QualityParameter qualityParam(quality);
		bitmap.Save(file, &g_clsid[format], &qualityParam);
	}
	DeleteObject(hbm);
	return 0;
}
