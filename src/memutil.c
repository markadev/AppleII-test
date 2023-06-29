#include "memutil.h"
#include <stdio.h>
#include <string.h>


static uchar read_buf[1024];


// Ref: Inside the Apple IIe, page 296
void memcpy_to_aux(void *dst, const void *src, unsigned int len) {
    *((unsigned int *)0x003c) = (unsigned int)src;
    *((unsigned int *)0x003e) = (unsigned int)src + len - 1;
    *((unsigned int *)0x0042) = (unsigned int)dst;

    asm("sec");
    asm("jsr $c311");
}

void memcpy_from_aux(void *dst, const void *src, unsigned int len) {
    *((unsigned int *)0x003c) = (unsigned int)src;
    *((unsigned int *)0x003e) = (unsigned int)src + len - 1;
    *((unsigned int *)0x0042) = (unsigned int)dst;

    asm("clc");
    asm("jsr $c311");
}

void memcpy_aux_to_aux(void *dst, const void *src, unsigned int len) {
    int n;
    while(len > 0) {
        n = len;
        if(n > sizeof(read_buf)) {
            n = sizeof(read_buf);
        }
        memcpy_from_aux(read_buf, src, n);
        memcpy_to_aux(dst, read_buf, n);
        len -= n;
        dst = (void *)((uchar *)dst + n);
        src = (void *)((const uchar *)src + n);
    }
}


// Fill text memory with a value without affecting the screenholes
void memset_text_page(uchar *page, uchar value, uchar dbl) {
    uchar *p;
    for(p = page; p < page + TEXT_PAGE_SIZE; p += TEXT_LINE_STRIDE) {
        memset(p, value, 120);
    }
    if(dbl) {
        memcpy_to_aux(page, page, TEXT_PAGE_SIZE);
    }
}


void memset_dbl(void *p, uchar value, unsigned int len, uchar dbl) {
    memset(p, value, len);
    if(dbl) {
        memcpy_to_aux(p, p, len);
    }
}

int bload_text_page(const char *filename, uchar *page, uchar aux) {
    FILE *fp;
    int n, offs;

    fp = fopen(filename, "rb");
    if(fp == NULL) {
        puts("UNABLE TO OPEN");
        puts(filename);
        return -1;
    }

    n = fread(read_buf, 1, sizeof(read_buf), fp);
    fclose(fp);

    // Copy to text memory, skipping over the screenholes
    for(offs = 0; offs < n; offs += TEXT_LINE_STRIDE) {
        if(aux) {
            memcpy_to_aux(&page[offs], &read_buf[offs], 120);
        } else {
            memcpy(&page[offs], &read_buf[offs], 120);
        }
    }
    return 0;
}


int bload_hires(const char *filename, uchar *page, uchar aux) {
    FILE *fp;
    uchar *dst;
    int i, n;

    fp = fopen(filename, "rb");
    if(fp == NULL) {
        puts("UNABLE TO OPEN");
        puts(filename);
        return -1;
    }

    dst = page;
    for(i = 0; i < HGR_PAGE_SIZE/sizeof(read_buf); i++) {
        n = fread(read_buf, 1, sizeof(read_buf), fp);
        if(aux) {
            memcpy_to_aux(dst, read_buf, n);
        } else {
            memcpy(dst, read_buf, n);
        }
        if(n < sizeof(read_buf))
            break;
        dst += sizeof(read_buf);
    }

    fclose(fp);
    return 0;
}
