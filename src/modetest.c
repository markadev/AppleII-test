#include <apple2.h>
#include <stdio.h>
#include <string.h>
#include "memutil.h"
#include "modetest.h"
#include "apple_io.h"


const uchar page1_label[] = {CH_A+15, CH_A, CH_A+6, CH_A+4, CH_0+1, 0}; // "PAGE1"
const uchar page2_label[] = {CH_A+15, CH_A, CH_A+6, CH_A+4, CH_0+2, 0}; // "PAGE2"
const uchar hexchars[16] = {CH_0, CH_0+1, CH_0+2, CH_0+3, CH_0+4, CH_0+5, CH_0+6, CH_0+7, CH_0+8, CH_0+9, CH_A, CH_A+1, CH_A+2, CH_A+3, CH_A+4, CH_A+5};


void place_char_on_line(uchar *line, uchar col, uchar ch, uchar is80) {
    if(! is80) {
        line[col] = ch;
    } else if(col & 0x01) {
        // odd columns are stored in main memory
        line[col >> 1] = ch;
    } else {
        // even columns are stored in aux memory
        memcpy_to_aux(&line[col >> 1], &ch, 1);
    }
}


void place_chars_on_line(uchar *line, uchar col, const uchar *s, uchar is80) {
    uchar i;
    for(i = 0; s[i]; i++) {
        place_char_on_line(line, col+i, s[i], is80);
    }
}


void draw_text_pattern(uchar *page, uchar is80) {
    uchar ch, i, line_no, lmargin, last_col;
    uchar *line;

    // clear
    memset_text_page(page, CH_SPACE, is80);

    ch = 0;
    lmargin = is80 ? 31 : 11;
    last_col = is80 ? 79 : 39;

    for(line=page, line_no=0; line_no < 24; line_no++) {
        if((line_no == 0) || (line_no == 23)) {
            // top/bottom border
            memset_dbl(line, CH_HASH, 40, is80);
        } else {
            // left/right border
            place_char_on_line(line, 0, CH_HASH, is80);
            place_char_on_line(line, last_col, CH_HASH, is80);
        }

        if(line_no == 2) {
            // table top heading
            for(i = 0; i < 16; i++) {
                place_char_on_line(line, lmargin+2+i, hexchars[i], is80);
            }
        } else if((line_no >= 3) && (line_no < 19)) {
            // table rows
            place_char_on_line(line, lmargin, hexchars[line_no-3], is80);
            for(i = 0; i < 16; i++) {
                place_char_on_line(line, lmargin + 2 + i, ch, is80);
                ch++;
            }
        }

        // advance to next line
        line += TEXT_LINE_STRIDE;
        if(line >= (page + TEXT_PAGE_SIZE)) {
            line = line - TEXT_PAGE_SIZE + 40;
        }
    }
}


void draw_mixed_text_pattern(uchar *page, const char *label, uchar is80) {
    uchar last_col;
    uchar *line;

    last_col = is80 ? 79 : 39;

    // line 1 - top border
    line = page+TEXT_LINE_21;
    memset_dbl(line, CH_HASH, 40, is80);

    // line 2
    line += TEXT_LINE_STRIDE;
    memset_dbl(line, CH_SPACE, 40, is80);
    place_char_on_line(line, 0, CH_HASH, is80);
    place_char_on_line(line, last_col, CH_HASH, is80);
    place_chars_on_line(line, 2, label, is80);

    // line 3
    line += TEXT_LINE_STRIDE;
    memset_dbl(line, CH_SPACE, 40, is80);
    place_char_on_line(line, 0, CH_HASH, is80);
    place_char_on_line(line, last_col, CH_HASH, is80);

    // line 4 - bottom border
    line += TEXT_LINE_STRIDE;
    memset_dbl(line, CH_HASH, 40, is80);
}


char getkey() {
    char k;
    while(*IO_RD_KBD < 0x80) {}
    k = *IO_RD_KBD;
    *IO_WR_KBDSTRB = 1;
    return k & 0x7f;
}


// 40 & 80-column text (PAGE1, PAGE2)
void test_text(uchar is80) {
    // load PAGE1 & clear PAGE2
    draw_text_pattern(TEXT_PAGE1, is80);
    place_chars_on_line(TEXT_PAGE1+TEXT_LINE_23, 2, page1_label, is80);
    memset_text_page(TEXT_PAGE2, CH_SPACE, is80);

    if(is80) SOFTSW_80COLON;

    getkey();

    // copy PAGE1 to PAGE2 & clear PAGE1
    memcpy(TEXT_PAGE2, TEXT_PAGE1, TEXT_PAGE_SIZE);
    if(is80) memcpy_aux_to_aux(TEXT_PAGE2, TEXT_PAGE1, TEXT_PAGE_SIZE);
    memset_text_page(TEXT_PAGE1, CH_SPACE, is80);
    place_chars_on_line(TEXT_PAGE2+TEXT_LINE_23, 2, page2_label, is80);

    SOFTSW_PAGE2ON;
}


// lores & mixed lores/text (PAGE1, PAGE2)
void test_lores(uchar mixed) {
    // load PAGE1 & clear PAGE2
    if(bload_text_page("LGR.BIN", TEXT_PAGE1, 0) != 0) {
        return;
    }
    if(mixed) draw_mixed_text_pattern(TEXT_PAGE1, page1_label, 0);
    memset_text_page(TEXT_PAGE2, CH_SPACE, 0);

    if(mixed) SOFTSW_MIXEDON;
    SOFTSW_TEXTOFF;

    getkey();

    // copy PAGE1 to PAGE2 & clear PAGE1
    memcpy(TEXT_PAGE2, TEXT_PAGE1, TEXT_PAGE_SIZE);
    if(mixed) draw_mixed_text_pattern(TEXT_PAGE2, page2_label, 0);
    memset_text_page(TEXT_PAGE1, CH_SPACE, 0);
    SOFTSW_PAGE2ON;
}


// hires & mixed hires/text (PAGE1, PAGE2)
void test_hires(uchar mixed) {
    // load PAGE1 & clear PAGE2
    if(bload_hires("HGR.BIN", HGR_PAGE1, 0) != 0) {
        return;
    }
    if(mixed) {
        draw_mixed_text_pattern(TEXT_PAGE1, page1_label, 0);
        memset_text_page(TEXT_PAGE2, CH_SPACE, 0);
    }
    memset_dbl(HGR_PAGE2, 0, HGR_PAGE_SIZE, 0);

    SOFTSW_HIRESON;
    if(mixed) SOFTSW_MIXEDON;
    SOFTSW_TEXTOFF;

    getkey();

    // copy PAGE1 to PAGE2 & clear PAGE1
    memcpy(HGR_PAGE2, HGR_PAGE1, HGR_PAGE_SIZE);
    if(mixed) {
        draw_mixed_text_pattern(TEXT_PAGE2, page2_label, 0);
        memset_text_page(TEXT_PAGE1, CH_SPACE, 0);
    }
    memset_dbl(HGR_PAGE1, 0, HGR_PAGE_SIZE, 0);
    SOFTSW_PAGE2ON;
}


// double-lores & mixed dlores/text (PAGE1, PAGE2)
void test_dlores(uchar mixed) {
    // load PAGE1 & clear PAGE2
    if(bload_text_page("DLGR.AUX", TEXT_PAGE1, 1) != 0 || bload_text_page("DLGR.BIN", TEXT_PAGE1, 0) != 0) {
        return;
    }
    if(mixed) draw_mixed_text_pattern(TEXT_PAGE1, page1_label, 1);
    memset_text_page(TEXT_PAGE2, CH_SPACE, 1);

    SOFTSW_80COLON;
    SOFTSW_DHIRESON;
    if(mixed) SOFTSW_MIXEDON;
    SOFTSW_TEXTOFF;

    getkey();

    // copy PAGE1 to PAGE2 & clear PAGE1
    memcpy(TEXT_PAGE2, TEXT_PAGE1, TEXT_PAGE_SIZE);
    memcpy_aux_to_aux(TEXT_PAGE2, TEXT_PAGE1, TEXT_PAGE_SIZE);
    if(mixed) draw_mixed_text_pattern(TEXT_PAGE2, page2_label, 1);
    memset_text_page(TEXT_PAGE1, CH_SPACE, 1);

    SOFTSW_80STOREOFF;
    SOFTSW_PAGE2ON;
}


// double-hires & mixed dhires/text (PAGE1, PAGE2)
void test_dhires(uchar mixed) {
    // load PAGE1 & clear PAGE2
    if(bload_hires("DHGR.AUX", HGR_PAGE1, 1) != 0 || bload_hires("DHGR.BIN", HGR_PAGE1, 0) != 0) {
        return;
    }
    if(mixed) {
        draw_mixed_text_pattern(TEXT_PAGE1, page1_label, 1);
        memset_text_page(TEXT_PAGE2, CH_SPACE, 1);
    }
    memset_dbl(HGR_PAGE2, 0, HGR_PAGE_SIZE, 1);

    SOFTSW_HIRESON;
    if(mixed) SOFTSW_MIXEDON;
    SOFTSW_TEXTOFF;
    // mode 2 (color 140x192)
    SOFTSW_80COLON;
    SOFTSW_DHIRESON;
    SOFTSW_DHIRESOFF;
    SOFTSW_DHIRESON;
    SOFTSW_DHIRESOFF;
    SOFTSW_DHIRESON;

    getkey();

    // copy PAGE1 to PAGE2 & clear PAGE1
    memcpy(HGR_PAGE2, HGR_PAGE1, HGR_PAGE_SIZE);
    memcpy_aux_to_aux(HGR_PAGE2, HGR_PAGE1, HGR_PAGE_SIZE);
    if(mixed) {
        draw_mixed_text_pattern(TEXT_PAGE2, page2_label, 1);
        memset_text_page(TEXT_PAGE1, CH_SPACE, 1);
    }
    memset_dbl(HGR_PAGE1, 0, HGR_PAGE_SIZE, 1);

    SOFTSW_80STOREOFF;
    SOFTSW_PAGE2ON;
}


void test_dhires_altmodes() {
    // TODO: create an image to test these modes better
    if(bload_hires("DHGR.AUX", HGR_PAGE1, 1) != 0 || bload_hires("DHGR.BIN", HGR_PAGE1, 0) != 0) {
        return;
    }

    SOFTSW_HIRESON;
    SOFTSW_TEXTOFF;

    // mode 1 (monochrome 560x192)
    SOFTSW_80COLOFF;
    SOFTSW_DHIRESON;
    SOFTSW_DHIRESOFF;
    SOFTSW_DHIRESON;
    SOFTSW_DHIRESOFF;
    SOFTSW_80COLON;
    SOFTSW_DHIRESON;

    getkey();

    // mode 3 (mixed mode)
    SOFTSW_80COLOFF;
    SOFTSW_DHIRESON;
    SOFTSW_DHIRESOFF;
    SOFTSW_80COLON;
    SOFTSW_DHIRESON;
    SOFTSW_DHIRESOFF;
    SOFTSW_DHIRESON;

    getkey();

    // other? (160x192) from US Patent 4631692
    // This mode is not mentioned in the Apple Extended 80-Column Text/AppleColor Adapter Card manual
    SOFTSW_80COLON;
    SOFTSW_DHIRESON;
    SOFTSW_DHIRESOFF;
    SOFTSW_80COLOFF;
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
        SOFTSW_MIXEDOFF;
        SOFTSW_HIRESOFF;
        SOFTSW_ALTCHARSETOFF;
        SOFTSW_DHIRESOFF;

        puts("\n\n-------------------------------------");
        puts("VIDEO MODE TEST");
        puts("");
        puts("APPLE II+ MODES");
        puts("  (Q) 40-COL TEXT (PAGE1, PAGE2)");
        puts("  (W) LORES (PAGE1, PAGE2)");
        puts("  (E) MIXED LORES/TEXT (PAGE1, PAGE2)");
        puts("  (R) HIRES (PAGE1, PAGE2)");
        puts("  (T) MIXED HIRES/TEXT (PAGE1, PAGE2)");
        puts("APPLE IIE MODES");
        puts("  (A) 80-COL TEXT (PAGE1, PAGE2)");
        puts("  (S) TEXT ALTCHARSET (40-COL, 80-COL)");
        puts("  (D) DOUBLE-LORES (PAGE1, PAGE2)");
        puts("  (F) MIXED DLORES/TEXT (PAGE1, PAGE2)");
        puts("  (G) DOUBLE-HIRES (PAGE1, PAGE2)");
        puts("  (H) MIXED DHIRES/TEXT (PAGE1, PAGE2)");
        puts("  (J) DHIRES ALT MODES (WIP)");
        puts("\nENTER SELECTION:");

        switch(getkey()) {
        case 'Q':
            // 40-column text
            test_text(0);
            break;
        case 'W':
            // lores
            test_lores(0);
            break;
        case 'E':
            // mixed lores/text
            test_lores(1);
            break;
        case 'R':
            // hires
            test_hires(0);
            break;
        case 'T':
            // mixed hires/text
            test_hires(1);
            break;
        case 'A':
            // 80-column text
            test_text(1);
            break;
        case 'S':
            // text alt charset (40-col, 80-col)
            SOFTSW_ALTCHARSETON;
            draw_text_pattern(TEXT_PAGE1, 0);

            getkey();

            SOFTSW_80COLON;
            draw_text_pattern(TEXT_PAGE1, 1);
            break;
        case 'D':
            // double-lores
            test_dlores(0);
            break;
        case 'F':
            // mixed double-lores/text
            test_dlores(1);
            break;
        case 'G':
            // double-hires
            test_dhires(0);
            break;
        case 'H':
            // mixed double-hires/text
            test_dhires(1);
            break;
        case 'J':
            // double-hires alt modes
            test_dhires_altmodes();
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
