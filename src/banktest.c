#include <apple2.h>
#include <stdio.h>
#include "memutil.h"
#include "apple_io.h"


#define CH_A 0x81
#define CH_X (CH_A+23)
#define CH_Y (CH_A+24)
#define CH_SPACE 0xa0


char getkey() {
    char k;
    while(*IO_RD_KBD < 0x80) {}
    k = *IO_RD_KBD;
    *IO_WR_KBDSTRB = 1;
    return k & 0x7f;
}


// Write bytes to text memory while various combinations of soft-switches
// are enabled. The resulting pattern indicates which memory bank (aux vs main)
// that the writes were made to.
void test_textmem(uchar page_num) {
    uchar *page = (page_num == 2) ? TEXT_PAGE2 : TEXT_PAGE1;
    uchar *line;
    uchar i;

    memset_text_page(page, CH_SPACE, 1);

    // Row headings and the 'control' character are in main text memory
    line = page;
    for(i=0; i < 8; i++) {
        line[2] = CH_A + i;
        line[4] = CH_X;

        // advance to next line
        line += TEXT_LINE_STRIDE;
        if(line >= (page + TEXT_PAGE_SIZE)) {
            line = line - TEXT_PAGE_SIZE + 40;
        }
    }

    // Test these combinations of soft-switches:
    //   {80storeon,80storeoff} * {page2on,page2off} * {ramwrton,ramwrtoff}
    line = page;
    for(i=0; i < 8; i++) {
        if(i & 0x04)
            SOFTSW_80STOREON;
        else
            SOFTSW_80STOREOFF;

        if(i & 0x02)
            SOFTSW_PAGE2ON;
        else
            SOFTSW_PAGE2OFF;

        if(i & 0x01)
            memstore_with_RAMWRTON(CH_Y, &line[5]);
        else
            line[5] = CH_Y;

        // advance to next line
        line += TEXT_LINE_STRIDE;
        if(line >= (page + TEXT_PAGE_SIZE)) {
            line = line - TEXT_PAGE_SIZE + 40;
        }
    }

    SOFTSW_80STOREOFF;
    SOFTSW_PAGE2OFF;

    // Display the results
    if(page_num == 2)
        SOFTSW_PAGE2ON;
    SOFTSW_80COLON;
}


void test_hgrmem(uchar page_num) {
    uchar *page = (page_num == 2) ? HGR_PAGE2 : HGR_PAGE1;
    uchar *line;
    uchar i;

    memset_dbl(page, 0, HGR_PAGE_SIZE, 1);

    // Set the 'control' lines in main memory
    line = page + 0x200; // line 32
    for(i=0; i < 16; i++) {
        line[2] = 0x7f;

        // Advance to the next 8th line
        line += 0x80;
        if(line >= (page+0x400)) {
            line = line - 0x400 + 40;
        }
    }

    // Test these combinations of soft-switches:
    //   {HIRESON,HIRESOFF} * {80storeon,80storeoff} * {page2on,page2off} * {ramwrton,ramwrtoff}
    line = page + 0x200; // line 32
    for(i=0; i < 16; i++) {
        if(i & 0x08)
            SOFTSW_HIRESON;
        else
            SOFTSW_HIRESOFF;

        if(i & 0x04)
            SOFTSW_80STOREON;
        else
            SOFTSW_80STOREOFF;

        if(i & 0x02)
            SOFTSW_PAGE2ON;
        else
            SOFTSW_PAGE2OFF;

        if(i & 0x01)
            memstore_with_RAMWRTON(0x7f, &line[3]);
        else
            line[3] = 0x7f;

        // Advance to the next 8th line
        line += 0x80;
        if(line >= (page+0x400)) {
            line = line - 0x400 + 40;
        }
    }

    SOFTSW_HIRESOFF;
    SOFTSW_80STOREOFF;
    SOFTSW_PAGE2OFF;

    // Display the results
    if(page_num == 2)
        SOFTSW_PAGE2ON;
    SOFTSW_HIRESON;
    SOFTSW_TEXTOFF;
    // mode 2 (color 140x192)
    SOFTSW_80COLON;
    SOFTSW_DHIRESON;
    SOFTSW_DHIRESOFF;
    SOFTSW_DHIRESON;
    SOFTSW_DHIRESOFF;
    SOFTSW_DHIRESON;
}

int main() {
    while(1) {
        // Reset soft-switches to 40 column text mode
        SOFTSW_TEXTON;
        SOFTSW_PAGE2OFF;
        SOFTSW_80COLOFF;
        SOFTSW_80STOREOFF;
        SOFTSW_HIRESOFF;
        SOFTSW_DHIRESOFF;

        puts("\n\n-------------------------------------");
        puts("BANK ACCESS TEST");
        puts("");
        puts("  (Q) TEXT MEMORY PAGE 1");
        puts("  (W) TEXT MEMORY PAGE 2");
        puts("  (E) HGR MEMORY PAGE 1");
        puts("  (R) HGR MEMORY PAGE 2");
        puts("\nENTER SELECTION:");

        switch(getkey()) {
        case 'Q':
            // text memory PAGE1
            test_textmem(1);
            break;
        case 'W':
            // text memory PAGE2
            test_textmem(2);
            break;
        case 'E':
            // hgr memory PAGE1
            test_hgrmem(1);
            break;
        case 'R':
            // hgr memory PAGE2
            test_hgrmem(2);
            break;
        case CH_ESC:
            return 0;
        default:
            continue;
        }

        // wait for keypress
        getkey();
    }
    return 0;
}
