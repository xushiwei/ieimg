//
// ieimg.h

#ifndef IEIMG_H
#define IEIMG_H

#if !defined(IEIMG_Linked_ieimg)
#define IEIMG_Linked_ieimg
#pragma comment(lib, "ieimg")
#endif

// -------------------------------------------------------------------------

typedef struct tag_ieimg_service {} ieimg_service;

typedef enum tag_ieimg_format {
	ieimg_png = 0,
	ieimg_jpeg = 1,
	ieimg_format_max,
} ieimg_format;

// -------------------------------------------------------------------------

STDAPI_(void) ieimg_init();
STDAPI_(void) ieimg_term();

STDAPI_(ieimg_service*) ieimg_new();
STDAPI_(void) ieimg_delete(ieimg_service* p);
STDAPI_(void) ieimg_resize(ieimg_service* p, int cx, int cy);

STDAPI_(HBITMAP) ieimg_render(ieimg_service* p, LPCWSTR url);

STDAPI_(int) ieimg_save(
	ieimg_service* p, LPCWSTR url, LPCWSTR file,
	ieimg_format format, int quality /* =80 */);

// -------------------------------------------------------------------------

#endif // IEIMG_H
