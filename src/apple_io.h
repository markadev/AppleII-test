#ifndef INCLUDE_APPLE_IO_H
#define INCLUDE_APPLE_IO_H

#define IO_RD_KBD ((volatile uchar *)0xc000)
#define IO_WR_KBDSTRB ((volatile uchar *)0xc010)

#define SOFTSW_80STOREOFF asm("sta $c000")
#define SOFTSW_80STOREON asm("sta $c001")
#define SOFTSW_80COLOFF asm("sta $c00c")
#define SOFTSW_80COLON asm("sta $c00d")
#define SOFTSW_ALTCHARSETOFF asm("sta $c00e")
#define SOFTSW_ALTCHARSETON asm("sta $c00f")
#define SOFTSW_TEXTOFF asm("sta $c050")
#define SOFTSW_TEXTON asm("sta $c051")
#define SOFTSW_MIXEDOFF asm("sta $c052")
#define SOFTSW_MIXEDON asm("sta $c053")
#define SOFTSW_PAGE2OFF asm("sta $c054")
#define SOFTSW_PAGE2ON asm("sta $c055")
#define SOFTSW_HIRESOFF asm("sta $c056")
#define SOFTSW_HIRESON asm("sta $c057")
#define SOFTSW_DHIRESON asm("sta $c05e")
#define SOFTSW_DHIRESOFF asm("sta $c05f")

#endif
