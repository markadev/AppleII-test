#ifndef INCLUDE_MEMUTIL_H
#define INCLUDE_MEMUTIL_H

typedef unsigned char uchar;


#define TEXT_PAGE1 ((uchar *)0x400)
#define TEXT_PAGE2 ((uchar *)0x800)
#define TEXT_PAGE_SIZE 1024
#define TEXT_LINE_STRIDE 128
#define TEXT_LINE_21 0x250
#define TEXT_LINE_22 0x2d0
#define TEXT_LINE_23 0x350
#define TEXT_LINE_24 0x3d0

#define HGR_PAGE1 ((uchar *)0x2000)
#define HGR_PAGE2 ((uchar *)0x4000)
#define HGR_PAGE_SIZE 8192


extern void __fastcall__ memstore_with_RAMWRTON(uchar value, uchar *addr);

void memcpy_to_aux(void *dst, const void *src, unsigned int len);
void memcpy_from_aux(void *dst, const void *src, unsigned int len);
void memcpy_aux_to_aux(void *dst, const void *src, unsigned int len);

void memset_text_page(uchar *page, uchar value, uchar dbl);
void memset_dbl(void *p, uchar value, unsigned int len, uchar dbl);

int bload_text_page(const char *filename, uchar *page, uchar aux);
int bload_hires(const char *filename, uchar *page, uchar aux);

#endif
