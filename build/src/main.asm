;--------------------------------------------------------
; File Created by SDCC : free open source ISO C Compiler 
; Version 4.4.1 #14650 (MINGW64)
;--------------------------------------------------------
	.module main
	.optsdcc -msm83
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl _main
	.globl _fill_bkg_rect
	.globl _set_sprite_data
	.globl _set_bkg_data
	.globl _wait_vbl_done
	.globl _joypad
;--------------------------------------------------------
; special function registers
;--------------------------------------------------------
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area _DATA
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area _INITIALIZED
;--------------------------------------------------------
; absolute external ram data
;--------------------------------------------------------
	.area _DABS (ABS)
;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
	.area _HOME
	.area _GSINIT
	.area _GSFINAL
	.area _GSINIT
;--------------------------------------------------------
; Home
;--------------------------------------------------------
	.area _HOME
	.area _HOME
;--------------------------------------------------------
; code
;--------------------------------------------------------
	.area _CODE
;src/main.c:7: void main(void) {
;	---------------------------------
; Function main
; ---------------------------------
_main::
;src/main.c:9: SPRITES_8x8;
	ldh	a, (_LCDC_REG + 0)
	and	a, #0xfb
	ldh	(_LCDC_REG + 0), a
;src/main.c:12: set_bkg_data(0, 2, tileset);
	ld	bc, #_tileset+0
	push	bc
	ld	hl, #0x200
	push	hl
	call	_set_bkg_data
	add	sp, #4
;src/main.c:13: set_sprite_data(0, 2, tileset);
	push	bc
	ld	hl, #0x200
	push	hl
	call	_set_sprite_data
	add	sp, #4
;src/main.c:16: fill_bkg_rect(0, 0, 20, 18, 0);
	xor	a, a
	ld	h, a
	ld	l, #0x12
	push	hl
	ld	a, #0x14
	push	af
	inc	sp
	xor	a, a
	rrca
	push	af
	call	_fill_bkg_rect
	add	sp, #5
;c:\gbdk\include\gb\gb.h:1875: shadow_OAM[nb].tile=tile;
	ld	hl, #(_shadow_OAM + 2)
	ld	(hl), #0x01
;c:\gbdk\include\gb\gb.h:1961: OAM_item_t * itm = &shadow_OAM[nb];
	ld	hl, #_shadow_OAM
;c:\gbdk\include\gb\gb.h:1962: itm->y=y, itm->x=x;
	ld	a, #0x48
	ld	(hl+), a
	ld	(hl), #0x50
;src/main.c:23: SHOW_BKG;
	ldh	a, (_LCDC_REG + 0)
	or	a, #0x01
	ldh	(_LCDC_REG + 0), a
;src/main.c:24: SHOW_SPRITES;
	ldh	a, (_LCDC_REG + 0)
	or	a, #0x02
	ldh	(_LCDC_REG + 0), a
;src/main.c:25: DISPLAY_ON;
	ldh	a, (_LCDC_REG + 0)
	or	a, #0x80
	ldh	(_LCDC_REG + 0), a
;src/main.c:28: UINT8 x = SPRITE_START_X;
;src/main.c:29: UINT8 y = SPRITE_START_Y;
	ld	bc, #0x4850
;src/main.c:32: while(1) {
00110$:
;src/main.c:34: UINT8 keys = joypad();
	call	_joypad
;src/main.c:36: if(keys & J_LEFT) {
	bit	1, a
	jr	Z, 00102$
;src/main.c:37: x--;
	dec	c
00102$:
;src/main.c:39: if(keys & J_RIGHT) {
	bit	0, a
	jr	Z, 00104$
;src/main.c:40: x++;
	inc	c
00104$:
;src/main.c:42: if(keys & J_UP) {
	bit	2, a
	jr	Z, 00106$
;src/main.c:43: y--;
	dec	b
00106$:
;src/main.c:45: if(keys & J_DOWN) {
	bit	3, a
	jr	Z, 00108$
;src/main.c:46: y++;
	inc	b
00108$:
;c:\gbdk\include\gb\gb.h:1961: OAM_item_t * itm = &shadow_OAM[nb];
;c:\gbdk\include\gb\gb.h:1962: itm->y=y, itm->x=x;
	ld	hl, #_shadow_OAM
	ld	(hl), b
	ld	hl, #(_shadow_OAM + 1)
	ld	(hl), c
;src/main.c:53: wait_vbl_done();
	call	_wait_vbl_done
;src/main.c:55: }
	jr	00110$
	.area _CODE
	.area _INITIALIZER
	.area _CABS (ABS)
