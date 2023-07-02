.code

.import popa
.importzp ptr1

; extern void __fastcall__ memstore_with_RAMWRTON(uchar value, uchar *addr)
;   A - low byte of 'addr'
;   X - high byte of 'addr'
;   (sp) - 'value'
.export _memstore_with_RAMWRTON
_memstore_with_RAMWRTON:
    sta ptr1
    stx ptr1+1
    jsr popa
    ldy #0
    cli
    sta $c005 ; RAMWRTON
    sta (ptr1),Y
    sta $c004 ; RAMWRTOFF
    sei
    rts
