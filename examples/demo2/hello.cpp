//
// hello.cpp

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ieimg.h>
#include <shellapi.h>
#include <winx/CommonDialogs.h>

// -------------------------------------------------------------------------

int main()
{
	char url[256];
	ieimg_init();
	ieimg_service* service = ieimg_new();
	ieimg_resize(service, 1024, 768);
	for (;;)
	{
		gets(url);
		if (strcmp(url, "q") == 0)
			break;

		winx::SaveFileDialog dlg(
			_T("Image files(*.png;*.jpg;*.jpeg)\0*.png;*.jpg;*.jpeg\0"), _T("*.png"));
		
		if (IDOK == dlg.DoModal())
		{
			USES_CONVERSION;
			LPCSTR lpszExt = PathFindExtensionA(dlg.lpstrFile);
			ieimg_format format = stricmp(lpszExt, ".png") == 0 ? ieimg_png : ieimg_jpeg;
			ieimg_save(service, T2CW(url), T2CW(dlg.lpstrFile), format, 80);
			
			ShellExecuteA(NULL, "open", dlg.lpstrFile, NULL, NULL, SW_SHOW);
		}
	}
	ieimg_delete(service);
	ieimg_term();
	return 0;
}

// -------------------------------------------------------------------------
