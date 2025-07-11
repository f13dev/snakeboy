;--------------------------------------------------------
; File Created by SDCC : free open source ISO C Compiler
; Version 4.5.1 #15267 (MINGW64)
;--------------------------------------------------------
	.module snakeboy
	
;--------------------------------------------------------
; Public variables in this module
;--------------------------------------------------------
	.globl _main
	.globl _strncpy
	.globl _memcpy
	.globl _font_init
	.globl _gotoxy
	.globl _rand
	.globl _initrand
	.globl _puts
	.globl _sprintf
	.globl _printf
	.globl _set_bkg_tiles
	.globl _set_bkg_data
	.globl _wait_vbl_done
	.globl _joypad
	.globl _delay
	.globl _save_data
	.globl _game_over_flag
	.globl _score
	.globl _food_y
	.globl _food_x
	.globl _snake_direction
	.globl _snake_length
	.globl _snake_y
	.globl _snake_x
	.globl _game_frames
	.globl _score_str
	.globl _border_tile
	.globl _food_tile
	.globl _snake_body_tile
	.globl _snake_head_tile
	.globl _empty_tile
	.globl _DEFAULT_NAME
	.globl _menu
	.globl _draw_menu_snake
	.globl _run_game
	.globl _init_game
	.globl _draw_game_area
	.globl _draw_snake
	.globl _draw_food
	.globl _generate_food
	.globl _handle_input
	.globl _update_game
	.globl _clear_screen
	.globl _game_over_screen
	.globl _log_high_score
	.globl _update_score_display
	.globl _load_save_data
	.globl _store_save_data
	.globl _update_high_scores
	.globl _sort_high_scores
	.globl _check_if_high_score
;--------------------------------------------------------
; special function registers
;--------------------------------------------------------
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area _DATA
_game_frames::
	.ds 1
_snake_x::
	.ds 150
_snake_y::
	.ds 150
_snake_length::
	.ds 1
_snake_direction::
	.ds 1
_food_x::
	.ds 1
_food_y::
	.ds 1
_score::
	.ds 2
_game_over_flag::
	.ds 1
;--------------------------------------------------------
; ram data
;--------------------------------------------------------
	.area _INITIALIZED
_save_data::
	.ds 62
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
;snakeboy.c:152: void main() {
;	---------------------------------
; Function main
; ---------------------------------
_main::
;snakeboy.c:153: font_init(); // Initialize font system
	call	_font_init
;snakeboy.c:156: set_bkg_data(TILE_EMPTY, 1, empty_tile);
	ld	de, #_empty_tile
	push	de
	xor	a, a
	inc	a
	push	af
	call	_set_bkg_data
	add	sp, #4
;snakeboy.c:157: set_bkg_data(TILE_SNAKE_HEAD, 1, snake_head_tile);
	ld	de, #_snake_head_tile
	push	de
	ld	hl, #0x101
	push	hl
	call	_set_bkg_data
	add	sp, #4
;snakeboy.c:158: set_bkg_data(TILE_SNAKE_BODY, 1, snake_body_tile);
	ld	de, #_snake_body_tile
	push	de
	ld	hl, #0x102
	push	hl
	call	_set_bkg_data
	add	sp, #4
;snakeboy.c:159: set_bkg_data(TILE_FOOD, 1, food_tile);
	ld	de, #_food_tile
	push	de
	ld	hl, #0x103
	push	hl
	call	_set_bkg_data
	add	sp, #4
;snakeboy.c:160: set_bkg_data(TILE_BORDER, 1, border_tile);
	ld	de, #_border_tile
	push	de
	ld	hl, #0x104
	push	hl
	call	_set_bkg_data
	add	sp, #4
;snakeboy.c:163: BGP_REG = 0xE4; // 11100100 -> Color 3,2,1,0 (Black, Dark Gray, Light Gray, White)
	ld	a, #0xe4
	ldh	(_BGP_REG + 0), a
;snakeboy.c:165: SHOW_BKG; // Show the background layer
	ldh	a, (_LCDC_REG + 0)
	or	a, #0x01
	ldh	(_LCDC_REG + 0), a
;snakeboy.c:166: DISPLAY_ON; // Turn on the display
	ldh	a, (_LCDC_REG + 0)
	or	a, #0x80
	ldh	(_LCDC_REG + 0), a
;snakeboy.c:168: menu(); // Start the game by showing the menu
;snakeboy.c:169: }
	jp	_menu
_DEFAULT_NAME:
	.ascii "AAA"
	.db 0x00
_empty_tile:
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x00	; 0
_snake_head_tile:
	.db #0x18	; 24
	.db #0x00	; 0
	.db #0x3c	; 60
	.db #0x18	; 24
	.db #0x7e	; 126
	.db #0x3c	; 60
	.db #0xff	; 255
	.db #0x7e	; 126
	.db #0xff	; 255
	.db #0x7e	; 126
	.db #0x7e	; 126
	.db #0x3c	; 60
	.db #0x3c	; 60
	.db #0x18	; 24
	.db #0x18	; 24
	.db #0x00	; 0
_snake_body_tile:
	.db #0xff	; 255
	.db #0x00	; 0
	.db #0xff	; 255
	.db #0x7e	; 126
	.db #0xff	; 255
	.db #0x7e	; 126
	.db #0xff	; 255
	.db #0x7e	; 126
	.db #0xff	; 255
	.db #0x7e	; 126
	.db #0xff	; 255
	.db #0x7e	; 126
	.db #0xff	; 255
	.db #0x7e	; 126
	.db #0xff	; 255
	.db #0x00	; 0
_food_tile:
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x7e	; 126
	.db #0x00	; 0
	.db #0x42	; 66	'B'
	.db #0x3c	; 60
	.db #0x5a	; 90	'Z'
	.db #0x3c	; 60
	.db #0x5a	; 90	'Z'
	.db #0x3c	; 60
	.db #0x42	; 66	'B'
	.db #0x3c	; 60
	.db #0x7e	; 126
	.db #0x00	; 0
	.db #0x00	; 0
	.db #0x00	; 0
_border_tile:
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
	.db #0xff	; 255
_score_str:
	.ascii "SCORE: %d"
	.db 0x00
;snakeboy.c:171: void menu() {
;	---------------------------------
; Function menu
; ---------------------------------
_menu::
	dec	sp
	dec	sp
;snakeboy.c:173: clear_screen();
	call	_clear_screen
;snakeboy.c:176: gotoxy(0, 3);
	ld	hl, #0x300
	push	hl
	call	_gotoxy
	pop	hl
;snakeboy.c:180: printf("\n");
	ld	de, #___str_14
	call	_puts
;snakeboy.c:182: load_save_data(); // Load high scores from SRAM, defult if not available
	call	_load_save_data
;snakeboy.c:185: for (int i = 0; i < MAX_HIGHSCORES / 2; i++) {
	xor	a, a
	ldhl	sp,	#0
	ld	(hl+), a
	ld	(hl), a
00113$:
	ldhl	sp,	#0
	ld	a, (hl+)
	sub	a, #0x05
	ld	a, (hl)
	sbc	a, #0x00
	jr	NC, 00101$
;snakeboy.c:187: gotoxy(10, i + 11);
	dec	hl
	ld	c, (hl)
	ld	a, c
	add	a, #0x0b
	ld	b, a
	push	bc
	ld	c, #0x0a
	push	bc
	call	_gotoxy
	pop	hl
	pop	bc
;snakeboy.c:188: printf(" %s %04d", save_data.scores[i + MAX_HIGHSCORES / 2].name, save_data.scores[i + MAX_HIGHSCORES / 2].score);
	ld	a, c
	add	a, #0x05
	ld	e, a
	add	a, a
	add	a, e
	add	a, a
	ld	c, a
	add	a,#<(_save_data)
	ld	e, a
	ld	a, #>(_save_data)
	adc	a, #0x00
	ld	d, a
	ld	hl, #0x0004
	add	hl, de
	ld	a,	(hl+)
	ld	h, (hl)
	ld	l, a
	ld	a, c
	add	a, #<(_save_data)
	ld	e, a
	ld	a, #0x00
	adc	a, #>(_save_data)
	ld	d, a
	push	bc
	push	hl
	push	de
	ld	de, #___str_10
	push	de
	call	_printf
;snakeboy.c:189: gotoxy(0, i + 11);
	add	sp, #7
	xor	a, a
	push	af
	inc	sp
	call	_gotoxy
	pop	hl
;snakeboy.c:190: printf(" %s %04d", save_data.scores[i].name, save_data.scores[i].score);
	pop	bc
	push	bc
	ld	l, c
	ld	h, b
	add	hl, hl
	add	hl, bc
	add	hl, hl
	ld	c, l
	ld	b, h
	ld	hl,#_save_data + 1
	add	hl,bc
	inc	hl
	inc	hl
	inc	hl
	ld	a,	(hl+)
	ld	h, (hl)
	ld	l, a
	ld	a, c
	add	a, #<(_save_data)
	ld	c, a
	ld	a, b
	adc	a, #>(_save_data)
	ld	b, a
	push	hl
	push	bc
	ld	de, #___str_10
	push	de
	call	_printf
	add	sp, #6
;snakeboy.c:185: for (int i = 0; i < MAX_HIGHSCORES / 2; i++) {
	ldhl	sp,	#0
	inc	(hl)
	jr	00113$
00101$:
;snakeboy.c:193: draw_menu_snake(); // Draw the snake graphics on the menu
	call	_draw_menu_snake
;snakeboy.c:196: while (1) {
00110$:
;snakeboy.c:198: UBYTE joy = joypad();
	call	_joypad
;snakeboy.c:199: if (joy && joy == J_START) {
	or	a, a
	jr	Z, 00106$
	cp	a, #0x80
	jr	NZ, 00106$
;snakeboy.c:200: run_game(); // Start the game when START is pressed
	inc	sp
	inc	sp
	jp	_run_game
;snakeboy.c:201: break; // Exit the menu loop
	jr	00115$
00106$:
;snakeboy.c:202: } else if (joy && joy == J_SELECT) {
	or	a, a
	jr	Z, 00107$
	sub	a, #0x40
	jr	NZ, 00107$
;snakeboy.c:204: clear_screen();
	call	_clear_screen
;snakeboy.c:205: gotoxy(6, 4);
	ld	hl, #0x406
	push	hl
	call	_gotoxy
	pop	hl
;snakeboy.c:206: printf("SELECT pressed");
	ld	de, #___str_11
	push	de
	call	_printf
	pop	hl
00107$:
;snakeboy.c:210: wait_vbl_done(); // Wait for VBlank to prevent screen tearing
	call	_wait_vbl_done
	jr	00110$
00115$:
;snakeboy.c:212: }
	inc	sp
	inc	sp
	ret
___str_10:
	.ascii " %s %04d"
	.db 0x00
___str_11:
	.ascii "SELECT pressed"
	.db 0x00
___str_14:
	.ascii "      SNAKEBOY"
	.db 0x0a
	.ascii "    Press  START"
	.db 0x0a
	.ascii "      to play."
	.db 0x0a
	.db 0x00
;snakeboy.c:214: void draw_menu_snake() {
;	---------------------------------
; Function draw_menu_snake
; ---------------------------------
_draw_menu_snake::
	add	sp, #-3
;snakeboy.c:216: set_bkg_data(TILE_FOOD, 1, food_tile);
	ld	de, #_food_tile
	push	de
	ld	hl, #0x103
	push	hl
	call	_set_bkg_data
	add	sp, #4
;snakeboy.c:217: UBYTE tile_food_arr[] = {TILE_FOOD}; // Temporary array for TILE_FOOD
	ldhl	sp,	#0
	ld	c, l
	ld	b, h
	ld	(hl), #0x03
;snakeboy.c:218: set_bkg_tiles(18, 1, 1, 1, tile_food_arr);
	push	bc
	ld	hl, #0x101
	push	hl
	ld	hl, #0x112
	push	hl
	call	_set_bkg_tiles
	add	sp, #6
;snakeboy.c:221: set_bkg_data(TILE_SNAKE_HEAD, 1, snake_head_tile);
	ld	de, #_snake_head_tile
	push	de
	ld	hl, #0x101
	push	hl
	call	_set_bkg_data
	add	sp, #4
;snakeboy.c:222: UBYTE tile_snake_head_arr[] = {TILE_SNAKE_HEAD}; // Temporary array for TILE_SNAKE_HEAD
	ldhl	sp,	#1
	ld	c, l
	ld	b, h
	ld	(hl), #0x01
;snakeboy.c:223: set_bkg_tiles(18, 4, 1, 1, tile_snake_head_arr);
	push	bc
	ld	hl, #0x101
	push	hl
	ld	hl, #0x412
	push	hl
	call	_set_bkg_tiles
	add	sp, #6
;snakeboy.c:226: set_bkg_data(TILE_SNAKE_BODY, 1, snake_body_tile);
	ld	de, #_snake_body_tile
	push	de
	ld	hl, #0x102
	push	hl
	call	_set_bkg_data
	add	sp, #4
;snakeboy.c:227: UBYTE tile_snake_body_arr[] = {TILE_SNAKE_BODY}; // Temporary array for TILE_SNAKE_BODY
	ldhl	sp,	#2
	ld	(hl), #0x02
;snakeboy.c:229: for (int i = 5; i < 8; i++) {
	ld	e, #0x05
00104$:
;snakeboy.c:230: set_bkg_tiles(18, i, 1, 1, tile_snake_body_arr);
	ld	hl, #2
	add	hl, sp
	ld	c, l
	ld	b, h
;snakeboy.c:229: for (int i = 5; i < 8; i++) {
;snakeboy.c:230: set_bkg_tiles(18, i, 1, 1, tile_snake_body_arr);
	ld	a,e
	cp	a,#0x08
	jr	NC, 00101$
	push	de
	push	bc
	ld	h, #0x01
	push	hl
	inc	sp
	ld	h, #0x01
	push	hl
	inc	sp
	ld	h, a
	ld	l, #0x12
	push	hl
	call	_set_bkg_tiles
	add	sp, #6
	pop	de
;snakeboy.c:229: for (int i = 5; i < 8; i++) {
	inc	e
	jr	00104$
00101$:
;snakeboy.c:232: for (int i = 17; i > 1; i--) {
	ld	e, #0x11
00107$:
	ld	a, #0x01
	sub	a, e
	jr	NC, 00102$
;snakeboy.c:233: set_bkg_tiles(i, 7, 1, 1, tile_snake_body_arr);
	ld	a, e
	push	de
	push	bc
	ld	h, #0x01
	push	hl
	inc	sp
	ld	h, #0x01
	push	hl
	inc	sp
	ld	h, #0x07
	push	hl
	inc	sp
	push	af
	inc	sp
	call	_set_bkg_tiles
	add	sp, #6
	pop	de
;snakeboy.c:232: for (int i = 17; i > 1; i--) {
	dec	e
	jr	00107$
00102$:
;snakeboy.c:235: set_bkg_tiles(2, 8, 1, 1, tile_snake_body_arr);
	push	bc
	ld	hl, #0x101
	push	hl
	ld	hl, #0x802
	push	hl
	call	_set_bkg_tiles
	add	sp, #6
;snakeboy.c:236: set_bkg_tiles(2, 9, 1, 1, tile_snake_body_arr);
	push	bc
	ld	hl, #0x101
	push	hl
	ld	hl, #0x902
	push	hl
	call	_set_bkg_tiles
	add	sp, #6
;snakeboy.c:237: set_bkg_tiles(1, 9, 1, 1, tile_snake_body_arr);
	push	bc
	ld	hl, #0x101
	push	hl
	ld	hl, #0x901
	push	hl
	call	_set_bkg_tiles
	add	sp, #6
;snakeboy.c:238: set_bkg_tiles(0, 9, 1, 1, tile_snake_body_arr);
	push	bc
	ld	hl, #0x101
	push	hl
	ld	hl, #0x900
	push	hl
	call	_set_bkg_tiles
;snakeboy.c:239: }
	add	sp, #9
	ret
;snakeboy.c:241: void run_game() {
;	---------------------------------
; Function run_game
; ---------------------------------
_run_game::
	dec	sp
	dec	sp
;snakeboy.c:247: set_bkg_data(TILE_BORDER, 1, border_tile);
	ld	de, #_border_tile
	push	de
	ld	hl, #0x104
	push	hl
	call	_set_bkg_data
	add	sp, #4
;snakeboy.c:250: initrand(DIV_REG);
	ldh	a, (_DIV_REG + 0)
	ld	b, #0x00
	ld	c, a
	push	bc
	call	_initrand
	pop	hl
;snakeboy.c:252: int frame = 0;
	xor	a, a
	ldhl	sp,	#0
	ld	(hl+), a
	ld	(hl), a
;snakeboy.c:255: while (1) {
00107$:
;snakeboy.c:256: init_game(); // Initialize game state for a new round
	call	_init_game
;snakeboy.c:259: while (!game_over_flag) {
00103$:
	ld	a, (#_game_over_flag)
	or	a, a
	jr	NZ, 00105$
;snakeboy.c:261: frame++;
	ldhl	sp,	#0
	inc	(hl)
	jr	NZ, 00139$
	inc	hl
	inc	(hl)
00139$:
;snakeboy.c:265: handle_input();
	call	_handle_input
;snakeboy.c:267: if (frame == 10) {
	ldhl	sp,	#0
	ld	a, (hl+)
	sub	a, #0x0a
;snakeboy.c:268: frame = 0;
	or	a,(hl)
	jr	NZ, 00102$
	ldhl	sp,	#0
	ld	(hl+), a
	ld	(hl), a
;snakeboy.c:270: update_game();
	call	_update_game
;snakeboy.c:271: draw_snake();
	call	_draw_snake
;snakeboy.c:272: draw_food();
	call	_draw_food
;snakeboy.c:273: update_score_display();
	call	_update_score_display
00102$:
;snakeboy.c:276: wait_vbl_done(); // Wait for VBlank to prevent screen tearing
	call	_wait_vbl_done
	jr	00103$
00105$:
;snakeboy.c:281: NR52_REG = 0x80; // Enable sound
	ld	a, #0x80
	ldh	(_NR52_REG + 0), a
;snakeboy.c:282: NR51_REG = 0x11; // Enable sound channel 1
	ld	a, #0x11
	ldh	(_NR51_REG + 0), a
;snakeboy.c:283: NR50_REG = 0x77; // Set volume
	ld	a, #0x77
	ldh	(_NR50_REG + 0), a
;snakeboy.c:285: NR10_REG = 0x1E; // Set sound frequency sweep
	ld	a, #0x1e
	ldh	(_NR10_REG + 0), a
;snakeboy.c:286: NR11_REG = 0x10; // Set sound envelope
	ld	a, #0x10
	ldh	(_NR11_REG + 0), a
;snakeboy.c:287: NR12_REG = 0xF3; // Set sound length and volume
	ld	a, #0xf3
	ldh	(_NR12_REG + 0), a
;snakeboy.c:288: NR13_REG = 0x00; // Set sound frequency low byte
	xor	a, a
	ldh	(_NR13_REG + 0), a
;snakeboy.c:289: NR14_REG = 0x87; // Set sound frequency high byte and start sound
	ld	a, #0x87
	ldh	(_NR14_REG + 0), a
;snakeboy.c:291: game_over_screen(); // Display game over message
	call	_game_over_screen
	jr	00107$
;snakeboy.c:293: }
	inc	sp
	inc	sp
	ret
;snakeboy.c:295: void init_game(void) {
;	---------------------------------
; Function init_game
; ---------------------------------
_init_game::
	dec	sp
;snakeboy.c:296: clear_screen();
	call	_clear_screen
;snakeboy.c:300: draw_game_area(); // Draw the borders
	call	_draw_game_area
;snakeboy.c:302: snake_length = INITIAL_SNAKE_LENGTH;
	ld	hl, #_snake_length
	ld	(hl), #0x03
;snakeboy.c:304: snake_x[0] = SCREEN_WIDTH_TILES / 2;
	ld	hl, #_snake_x
	ld	(hl), #0x0a
;snakeboy.c:305: snake_y[0] = SCREEN_HEIGHT_TILES / 2;
	ld	hl, #_snake_y
	ld	(hl), #0x09
;snakeboy.c:307: for (i = 1; i < snake_length; i++) {
	ldhl	sp,	#0
	ld	(hl), #0x01
00103$:
	ldhl	sp,	#0
	ld	a, (hl)
	ld	hl, #_snake_length
	sub	a, (hl)
	jr	NC, 00101$
;snakeboy.c:308: snake_x[i] = snake_x[0];
	ld	de, #_snake_x
	ldhl	sp,	#0
	ld	l, (hl)
	ld	h, #0x00
	add	hl, de
	ld	c, l
	ld	b, h
	ld	a, (#_snake_x + 0)
	ld	(bc), a
;snakeboy.c:309: snake_y[i] = snake_y[0] + i; // Start moving up
	ld	de, #_snake_y
	ldhl	sp,	#0
	ld	l, (hl)
	ld	h, #0x00
	add	hl, de
	ld	c, l
	ld	b, h
	ld	a, (#_snake_y + 0)
	ldhl	sp,	#0
	add	a, (hl)
	ld	(bc), a
;snakeboy.c:307: for (i = 1; i < snake_length; i++) {
	inc	(hl)
	jr	00103$
00101$:
;snakeboy.c:311: snake_direction = DIR_UP; // Initial direction
;snakeboy.c:313: score = 0;
	xor	a, a
	ld	(#_snake_direction), a
	ld	hl, #_score
	ld	(hl+), a
	ld	(hl), a
;snakeboy.c:314: game_over_flag = 0;
	xor	a, a
	ld	(#_game_over_flag),a
;snakeboy.c:316: generate_food(); // Place the first food item
	inc	sp
	jp	_generate_food
;snakeboy.c:317: }
	inc	sp
	ret
;snakeboy.c:319: void draw_game_area() {
;	---------------------------------
; Function draw_game_area
; ---------------------------------
_draw_game_area::
	dec	sp
;snakeboy.c:321: UBYTE tile_border_arr[] = {TILE_BORDER}; // Temporary array for TILE_BORDER
	ldhl	sp,	#0
	ld	(hl), #0x04
;snakeboy.c:324: for (x = 0; x < SCREEN_WIDTH_TILES; x++) {
	ld	b, #0x00
00103$:
;snakeboy.c:325: set_bkg_tiles(x, 1, 1, 1, tile_border_arr); // Top row
	ld	hl, #0
	add	hl, sp
	push	hl
	push	hl
	ld	hl, #0x101
	push	hl
	ld	a, #0x01
	push	af
	inc	sp
	push	bc
	inc	sp
	call	_set_bkg_tiles
	add	sp, #6
	pop	hl
;snakeboy.c:326: set_bkg_tiles(x, SCREEN_HEIGHT_TILES - 1, 1, 1, tile_border_arr); // Bottom row
	push	hl
	push	hl
	ld	hl, #0x101
	push	hl
	ld	a, #0x11
	push	af
	inc	sp
	push	bc
	inc	sp
	call	_set_bkg_tiles
	add	sp, #6
	pop	hl
;snakeboy.c:324: for (x = 0; x < SCREEN_WIDTH_TILES; x++) {
	inc	b
	ld	a, b
	sub	a, #0x14
	jr	C, 00103$
;snakeboy.c:330: for (y = 1; y < SCREEN_HEIGHT_TILES - 1; y++) { // Exclude corners handled by top/bottom
	ld	b, #0x01
00105$:
;snakeboy.c:331: set_bkg_tiles(0, y, 1, 1, tile_border_arr); // Left column
	push	hl
	push	hl
	ld	hl, #0x101
	push	hl
	push	bc
	inc	sp
	xor	a, a
	push	af
	inc	sp
	call	_set_bkg_tiles
	add	sp, #6
	pop	hl
;snakeboy.c:332: set_bkg_tiles(SCREEN_WIDTH_TILES - 1, y, 1, 1, tile_border_arr); // Right column
	push	hl
	push	hl
	ld	hl, #0x101
	push	hl
	push	bc
	inc	sp
	ld	a, #0x13
	push	af
	inc	sp
	call	_set_bkg_tiles
	add	sp, #6
	pop	hl
;snakeboy.c:330: for (y = 1; y < SCREEN_HEIGHT_TILES - 1; y++) { // Exclude corners handled by top/bottom
	inc	b
	ld	a, b
	sub	a, #0x11
	jr	C, 00105$
;snakeboy.c:334: }
	inc	sp
	ret
;snakeboy.c:336: void draw_snake() {
;	---------------------------------
; Function draw_snake
; ---------------------------------
_draw_snake::
	add	sp, #-3
;snakeboy.c:338: UBYTE tile_empty_arr[] = {TILE_EMPTY};     // Temporary array for TILE_EMPTY
	ldhl	sp,	#0
;snakeboy.c:339: UBYTE tile_snake_head_arr[] = {TILE_SNAKE_HEAD}; // Temporary array for TILE_SNAKE_HEAD
	xor	a, a
	ld	(hl+), a
;snakeboy.c:340: UBYTE tile_snake_body_arr[] = {TILE_SNAKE_BODY}; // Temporary array for TILE_SNAKE_BODY
	ld	a, #0x01
	ld	(hl+), a
	ld	(hl), #0x02
;snakeboy.c:343: if (snake_length < MAX_SNAKE_LENGTH) { // Only clear if not maxed out
	ld	hl, #_snake_length
	ld	a, (hl)
	sub	a, #0x96
	jr	NC, 00102$
;snakeboy.c:344: set_bkg_tiles(snake_x[snake_length], snake_y[snake_length], 1, 1, tile_empty_arr);
	ld	a, #<(_snake_y)
	add	a, (hl)
	ld	c, a
	ld	a, #>(_snake_y)
	adc	a, #0x00
	ld	b, a
	ld	a, (bc)
	ld	d, a
	ld	a, #<(_snake_x)
	add	a, (hl)
	ld	c, a
	ld	a, #>(_snake_x)
	adc	a, #0x00
	ld	b, a
	ld	a, (bc)
	ld	b, a
	ld	hl, #0
	add	hl, sp
	push	hl
	ld	hl, #0x101
	push	hl
	ld	e, b
	push	de
	call	_set_bkg_tiles
	add	sp, #6
00102$:
;snakeboy.c:348: set_bkg_tiles(snake_x[0], snake_y[0], 1, 1, tile_snake_head_arr);
	ld	hl, #_snake_y
	ld	b, (hl)
	ld	a, (#_snake_x + 0)
	ld	hl, #1
	add	hl, sp
	push	hl
	ld	h, #0x01
	push	hl
	inc	sp
	ld	h, #0x01
	ld	l, b
	push	hl
	push	af
	inc	sp
	call	_set_bkg_tiles
	add	sp, #6
;snakeboy.c:351: for (i = 1; i < snake_length; i++) {
	ld	c, #0x01
00105$:
	ld	a, c
	ld	hl, #_snake_length
	sub	a, (hl)
	jr	NC, 00107$
;snakeboy.c:352: set_bkg_tiles(snake_x[i], snake_y[i], 1, 1, tile_snake_body_arr);
	ld	hl, #_snake_y
	ld	b, #0x00
	add	hl, bc
	ld	d, (hl)
	ld	hl, #_snake_x
	ld	b, #0x00
	add	hl, bc
	ld	a, (hl)
	ld	hl, #2
	add	hl, sp
	push	hl
	ld	h, #0x01
	push	hl
	inc	sp
	ld	h, #0x01
	ld	l, d
	push	hl
	push	af
	inc	sp
	call	_set_bkg_tiles
	add	sp, #6
;snakeboy.c:351: for (i = 1; i < snake_length; i++) {
	inc	c
	jr	00105$
00107$:
;snakeboy.c:354: }
	add	sp, #3
	ret
;snakeboy.c:356: void draw_food() {
;	---------------------------------
; Function draw_food
; ---------------------------------
_draw_food::
	dec	sp
;snakeboy.c:357: UBYTE tile_food_arr[] = {TILE_FOOD}; // Temporary array for TILE_FOOD
	ldhl	sp,	#0
	ld	c, l
	ld	b, h
	ld	(hl), #0x03
;snakeboy.c:358: set_bkg_tiles(food_x, food_y, 1, 1, tile_food_arr);
	push	bc
	ld	hl, #0x101
	push	hl
	ld	a, (_food_y)
	ld	h, a
	ld	a, (_food_x)
	ld	l, a
	push	hl
	call	_set_bkg_tiles
;snakeboy.c:359: }
	add	sp, #7
	ret
;snakeboy.c:361: void generate_food() {
;	---------------------------------
; Function generate_food
; ---------------------------------
_generate_food::
	dec	sp
;snakeboy.c:366: do {
00105$:
;snakeboy.c:367: collision = 0;
	ldhl	sp,	#0
	ld	(hl), #0x00
;snakeboy.c:369: new_food_x = (rand() % (SCREEN_WIDTH_TILES - 2)) + 1;
	call	_rand
	ld	a, e
	ld	e, #0x12
	call	__moduchar
	inc	c
;snakeboy.c:370: new_food_y = (rand() % (SCREEN_HEIGHT_TILES - 3)) + 2; // Start from row 2 to avoid the top border
	call	_rand
	ld	a, e
	push	bc
	ld	e, #0x0f
	call	__moduchar
	ld	a, c
	pop	bc
	add	a, #0x02
	ld	b, a
;snakeboy.c:373: for (i = 0; i < snake_length; i++) {
	ld	e, #0x00
00109$:
	ld	a, e
	ld	hl, #_snake_length
	sub	a, (hl)
	jr	NC, 00106$
;snakeboy.c:374: if (new_food_x == snake_x[i] && new_food_y == snake_y[i]) {
	ld	hl, #_snake_x
	ld	d, #0x00
	add	hl, de
	ld	a, (hl)
	sub	a, c
	jr	NZ, 00110$
	ld	hl, #_snake_y
	ld	d, #0x00
	add	hl, de
	ld	a, (hl)
	sub	a, b
	jr	NZ, 00110$
;snakeboy.c:375: collision = 1;
	ldhl	sp,	#0
	ld	(hl), #0x01
;snakeboy.c:376: break;
	jr	00106$
00110$:
;snakeboy.c:373: for (i = 0; i < snake_length; i++) {
	inc	e
	jr	00109$
00106$:
;snakeboy.c:379: } while (collision); // Keep trying until a non-colliding spot is found
	ldhl	sp,	#0
	ld	a, (hl)
	or	a, a
	jr	NZ, 00105$
;snakeboy.c:381: food_x = new_food_x;
	ld	hl, #_food_x
	ld	(hl), c
;snakeboy.c:382: food_y = new_food_y;
	ld	hl, #_food_y
	ld	(hl), b
;snakeboy.c:383: }
	inc	sp
	ret
;snakeboy.c:385: void handle_input() {
;	---------------------------------
; Function handle_input
; ---------------------------------
_handle_input::
;snakeboy.c:386: UBYTE joy = joypad();
	call	_joypad
	ld	c, a
;snakeboy.c:389: if ((joy & J_UP) && snake_direction != DIR_DOWN) {
	bit	2, c
	jr	Z, 00113$
	ld	a, (#_snake_direction)
	dec	a
	jr	Z, 00113$
;snakeboy.c:390: snake_direction = DIR_UP;
	xor	a, a
	ld	(#_snake_direction),a
	ret
00113$:
;snakeboy.c:391: } else if ((joy & J_DOWN) && snake_direction != DIR_UP) {
	bit	3, c
	jr	Z, 00109$
	ld	hl, #_snake_direction
	ld	a, (hl)
	or	a, a
	jr	Z, 00109$
;snakeboy.c:392: snake_direction = DIR_DOWN;
	ld	(hl), #0x01
	ret
00109$:
;snakeboy.c:393: } else if ((joy & J_LEFT) && snake_direction != DIR_RIGHT) {
	bit	1, c
	jr	Z, 00105$
	ld	a, (#_snake_direction)
	sub	a, #0x03
	jr	Z, 00105$
;snakeboy.c:394: snake_direction = DIR_LEFT;
	ld	hl, #_snake_direction
	ld	(hl), #0x02
	ret
00105$:
;snakeboy.c:395: } else if ((joy & J_RIGHT) && snake_direction != DIR_LEFT) {
	bit	0, c
	ret	Z
	ld	a, (#_snake_direction)
	sub	a, #0x02
	ret	Z
;snakeboy.c:396: snake_direction = DIR_RIGHT;
	ld	hl, #_snake_direction
	ld	(hl), #0x03
;snakeboy.c:398: }
	ret
;snakeboy.c:400: void update_game() {
;	---------------------------------
; Function update_game
; ---------------------------------
_update_game::
	add	sp, #-3
;snakeboy.c:406: for (i = snake_length; i > 0; i--) {
	ld	a, (#_snake_length)
	ldhl	sp,	#2
	ld	(hl), a
00122$:
	ldhl	sp,	#2
	ld	a, (hl)
	or	a, a
	jr	Z, 00101$
;snakeboy.c:407: snake_x[i] = snake_x[i - 1];
	ld	de, #_snake_x
	ld	l, (hl)
	ld	h, #0x00
	add	hl, de
	ld	e, l
	ld	d, h
	ldhl	sp,	#2
	ld	c, (hl)
	xor	a, a
	ld	b, a
	dec	bc
	ld	hl, #_snake_x
	add	hl, bc
	ld	a, (hl)
	ld	(de), a
;snakeboy.c:408: snake_y[i] = snake_y[i - 1];
	ld	de, #_snake_y
	ldhl	sp,	#2
	ld	l, (hl)
	ld	h, #0x00
	add	hl, de
	ld	e, l
	ld	d, h
	ld	hl, #_snake_y
	add	hl, bc
	ld	a, (hl)
	ld	(de), a
;snakeboy.c:406: for (i = snake_length; i > 0; i--) {
	ldhl	sp,	#2
	dec	(hl)
	jr	00122$
00101$:
;snakeboy.c:412: switch (snake_direction) {
	ld	a, #0x03
	ld	hl, #_snake_direction
	sub	a, (hl)
	jr	C, 00106$
	ld	c, (hl)
	ld	b, #0x00
	ld	hl, #00223$
	add	hl, bc
	add	hl, bc
	ld	c, (hl)
	inc	hl
	ld	h, (hl)
	ld	l, c
	jp	(hl)
00223$:
	.dw	00102$
	.dw	00103$
	.dw	00104$
	.dw	00105$
;snakeboy.c:413: case DIR_UP:    snake_y[0]--; break;
00102$:
	ld	a, (#_snake_y + 0)
	dec	a
	ld	(#_snake_y),a
	jr	00106$
;snakeboy.c:414: case DIR_DOWN:  snake_y[0]++; break;
00103$:
	ld	a, (#_snake_y + 0)
	inc	a
	ld	(#_snake_y),a
	jr	00106$
;snakeboy.c:415: case DIR_LEFT:  snake_x[0]--; break;
00104$:
	ld	a, (#_snake_x + 0)
	dec	a
	ld	(#_snake_x),a
	jr	00106$
;snakeboy.c:416: case DIR_RIGHT: snake_x[0]++; break;
00105$:
	ld	a, (#_snake_x + 0)
	inc	a
	ld	(#_snake_x),a
;snakeboy.c:417: }
00106$:
;snakeboy.c:422: if (snake_x[0] == 0 || snake_x[0] == SCREEN_WIDTH_TILES - 1 ||
	ld	a, (#_snake_x + 0)
	ldhl	sp,#0
	ld	(hl), a
	ld	a, (hl)
	or	a, a
	jr	Z, 00107$
	ld	a, (hl)
	sub	a, #0x13
	jr	Z, 00107$
;snakeboy.c:423: snake_y[0] == 1 || snake_y[0] == SCREEN_HEIGHT_TILES - 1) {
	ld	a, (#_snake_y + 0)
	ldhl	sp,#1
	ld	(hl), a
	ld	a, (hl)
	dec	a
	jr	Z, 00107$
	ldhl	sp,	#1
	ld	a, (hl)
	sub	a, #0x11
	jr	NZ, 00140$
00107$:
;snakeboy.c:424: game_over_flag = 1;
	ld	hl, #_game_over_flag
	ld	(hl), #0x01
;snakeboy.c:425: return;
	jp	00127$
;snakeboy.c:429: for (i = 1; i < snake_length; i++) {
00140$:
	ldhl	sp,	#2
	ld	(hl), #0x01
00125$:
	ldhl	sp,	#2
	ld	a, (hl)
	ld	hl, #_snake_length
	sub	a, (hl)
	jr	NC, 00115$
;snakeboy.c:430: if (snake_x[0] == snake_x[i] && snake_y[0] == snake_y[i]) {
	ld	de, #_snake_x
	ldhl	sp,	#2
	ld	l, (hl)
	ld	h, #0x00
	add	hl, de
	ld	c, l
	ld	b, h
	ld	a, (bc)
	ld	c, a
	ldhl	sp,	#0
	ld	a, (hl)
	sub	a, c
	jr	NZ, 00126$
	ld	de, #_snake_y
	ldhl	sp,	#2
	ld	l, (hl)
	ld	h, #0x00
	add	hl, de
	ld	c, l
	ld	b, h
	ld	a, (bc)
	ld	c, a
	ldhl	sp,	#1
	ld	a, (hl)
	sub	a, c
	jr	NZ, 00126$
;snakeboy.c:431: game_over_flag = 1;
	ld	hl, #_game_over_flag
	ld	(hl), #0x01
;snakeboy.c:432: return;
	jr	00127$
00126$:
;snakeboy.c:429: for (i = 1; i < snake_length; i++) {
	ldhl	sp,	#2
	inc	(hl)
	jr	00125$
00115$:
;snakeboy.c:437: if (snake_x[0] == food_x && snake_y[0] == food_y) {
	ld	a, (#_food_x)
	ldhl	sp,	#0
	sub	a, (hl)
	jr	NZ, 00127$
	ld	a, (#_food_y)
	ldhl	sp,	#1
	sub	a, (hl)
	jr	NZ, 00127$
;snakeboy.c:438: score += 10; // Increase score
	ld	hl, #_score
	ld	a, (hl)
	add	a, #0x0a
	ld	(hl+), a
	ld	a, (hl)
	adc	a, #0x00
	ld	(hl), a
;snakeboy.c:441: NR52_REG = 0x80; // Enable sound
	ld	a, #0x80
	ldh	(_NR52_REG + 0), a
;snakeboy.c:442: NR51_REG = 0x11; // Enable sound channel 1
	ld	a, #0x11
	ldh	(_NR51_REG + 0), a
;snakeboy.c:443: NR50_REG = 0x77; // Set volume
	ld	a, #0x77
	ldh	(_NR50_REG + 0), a
;snakeboy.c:445: NR10_REG = 0x55; // Set sound frequency sweep
	ld	a, #0x55
	ldh	(_NR10_REG + 0), a
;snakeboy.c:446: NR11_REG = 0x10; // Set sound envelope
	ld	a, #0x10
	ldh	(_NR11_REG + 0), a
;snakeboy.c:447: NR12_REG = 0xF3; // Set sound length and volume
	ld	a, #0xf3
	ldh	(_NR12_REG + 0), a
;snakeboy.c:448: NR13_REG = 0x00; // Set sound frequency low byte
	xor	a, a
	ldh	(_NR13_REG + 0), a
;snakeboy.c:449: NR14_REG = 0x87; // Set sound frequency high byte and start sound
	ld	a, #0x87
	ldh	(_NR14_REG + 0), a
;snakeboy.c:451: if (snake_length < MAX_SNAKE_LENGTH) {
	ld	hl, #_snake_length
	ld	a, (hl)
	sub	a, #0x96
	jr	NC, 00117$
;snakeboy.c:452: snake_length++; // Increase snake length
	inc	(hl)
00117$:
;snakeboy.c:455: generate_food(); // Generate new food
	call	_generate_food
00127$:
;snakeboy.c:457: }
	add	sp, #3
	ret
;snakeboy.c:459: void clear_screen() {
;	---------------------------------
; Function clear_screen
; ---------------------------------
_clear_screen::
	dec	sp
;snakeboy.c:461: UBYTE tile_empty_arr[] = {TILE_EMPTY}; // Temporary array for TILE_EMPTY
	ldhl	sp,	#0
	ld	(hl), #0x00
;snakeboy.c:464: for (i = 0; i < SCREEN_WIDTH_TILES; i++) {
	ld	c, #0x00
;snakeboy.c:465: for (j = 0; j < SCREEN_HEIGHT_TILES; j++) {
00109$:
	ld	b, #0x00
00103$:
;snakeboy.c:466: set_bkg_tiles(i, j, 1, 1, tile_empty_arr);
	ld	hl, #0
	add	hl, sp
	push	hl
	ld	hl, #0x101
	push	hl
	push	bc
	inc	sp
	ld	a, c
	push	af
	inc	sp
	call	_set_bkg_tiles
	add	sp, #6
;snakeboy.c:465: for (j = 0; j < SCREEN_HEIGHT_TILES; j++) {
	inc	b
	ld	a, b
	sub	a, #0x12
	jr	C, 00103$
;snakeboy.c:464: for (i = 0; i < SCREEN_WIDTH_TILES; i++) {
	inc	c
	ld	a, c
	sub	a, #0x14
	jr	C, 00109$
;snakeboy.c:469: }
	inc	sp
	ret
;snakeboy.c:471: void game_over_screen() {
;	---------------------------------
; Function game_over_screen
; ---------------------------------
_game_over_screen::
	add	sp, #-11
;snakeboy.c:473: clear_screen(); // Clear the screen before showing game over
	call	_clear_screen
;snakeboy.c:475: int is_high_score = check_if_high_score(score); // Check if it's a new high score
	ld	a, (_score)
	ld	e, a
	ld	hl, #_score + 1
	ld	d, (hl)
	call	_check_if_high_score
;snakeboy.c:478: if (is_high_score == 1) {
	ld	a, c
	dec	a
	or	a, b
	ld	a, #0x01
	jr	Z, 00150$
	xor	a, a
00150$:
	ldhl	sp,	#10
	ld	(hl), a
	or	a, a
	jr	Z, 00102$
;snakeboy.c:479: gotoxy(0, 2);
	ld	hl, #0x200
	push	hl
	call	_gotoxy
	pop	hl
;snakeboy.c:480: printf("   NEW HIGH SCORE   ");
	ld	de, #___str_15
	push	de
	call	_printf
	pop	hl
;snakeboy.c:481: log_high_score();
	call	_log_high_score
;snakeboy.c:482: return;
	jp	00112$
00102$:
;snakeboy.c:487: gotoxy(6, 6); // Move cursor to (1,1) tile position
	ld	hl, #0x606
	push	hl
	call	_gotoxy
	pop	hl
;snakeboy.c:488: printf("GAME OVER");
	ld	de, #___str_16
	push	de
	call	_printf
	pop	hl
;snakeboy.c:489: gotoxy(6, 8);
	ld	hl, #0x806
	push	hl
	call	_gotoxy
	pop	hl
;snakeboy.c:490: sprintf(score_str, "SCORE: %u", score);
	ld	hl, #0
	add	hl, sp
	ld	c, l
	ld	b, h
	push	bc
	ld	a, (_score)
	ld	e, a
	ld	hl, #_score + 1
	ld	d, (hl)
	push	de
	ld	de, #___str_17
	push	de
	push	bc
	call	_sprintf
	add	sp, #6
;snakeboy.c:491: printf("%s", score_str);
	ld	de, #___str_18
	push	de
	call	_printf
	add	sp, #4
;snakeboy.c:492: gotoxy(3, 10);
	ld	hl, #0xa03
	push	hl
	call	_gotoxy
	pop	hl
;snakeboy.c:493: printf("Press A button");
	ld	de, #___str_19
	push	de
	call	_printf
	pop	hl
;snakeboy.c:494: gotoxy(4, 11);
	ld	hl, #0xb04
	push	hl
	call	_gotoxy
	pop	hl
;snakeboy.c:495: if (is_high_score == 1) {
	ldhl	sp,	#10
	ld	a, (hl)
	or	a, a
	jr	Z, 00104$
;snakeboy.c:496: printf("to  continue");
	ld	de, #___str_20
	push	de
	call	_printf
	pop	hl
	jr	00106$
00104$:
;snakeboy.c:498: printf(" to restart.");
	ld	de, #___str_21
	push	de
	call	_printf
	pop	hl
;snakeboy.c:502: while (joypad() != J_A) {
00106$:
	call	_joypad
	sub	a, #0x10
	jr	Z, 00108$
;snakeboy.c:503: wait_vbl_done(); // Wait for VBlank to prevent screen tearing
	call	_wait_vbl_done
	jr	00106$
00108$:
;snakeboy.c:506: delay(200); // Small delay to prevent immediate re-start if button held
	ld	de, #0x00c8
	call	_delay
;snakeboy.c:508: if (is_high_score == 1) {
	ldhl	sp,	#10
	ld	a, (hl)
	or	a, a
	jr	Z, 00110$
;snakeboy.c:509: log_high_score();
	call	_log_high_score
	jr	00112$
00110$:
;snakeboy.c:511: menu(); // Return to menu if not a high score
	call	_menu
00112$:
;snakeboy.c:513: }
	add	sp, #11
	ret
___str_15:
	.ascii "   NEW HIGH SCORE   "
	.db 0x00
___str_16:
	.ascii "GAME OVER"
	.db 0x00
___str_17:
	.ascii "SCORE: %u"
	.db 0x00
___str_18:
	.ascii "%s"
	.db 0x00
___str_19:
	.ascii "Press A button"
	.db 0x00
___str_20:
	.ascii "to  continue"
	.db 0x00
___str_21:
	.ascii " to restart."
	.db 0x00
;snakeboy.c:515: void log_high_score() {
;	---------------------------------
; Function log_high_score
; ---------------------------------
_log_high_score::
	add	sp, #-9
;snakeboy.c:516: clear_screen(); // Clear the screen before logging high score
	call	_clear_screen
;snakeboy.c:519: gotoxy(0, 3);
	ld	hl, #0x300
	push	hl
	call	_gotoxy
	pop	hl
;snakeboy.c:520: printf("   NEW HIGH SCORE   ");
	ld	de, #___str_22
	push	de
	call	_printf
	pop	hl
;snakeboy.c:522: gotoxy(0, 9);
	ld	hl, #0x900
	push	hl
	call	_gotoxy
	pop	hl
;snakeboy.c:523: printf("  Score: %u", score); // Print the score
	ld	a, (_score)
	ld	e, a
	ld	hl, #_score + 1
	ld	d, (hl)
	push	de
	ld	de, #___str_23
	push	de
	call	_printf
	add	sp, #4
;snakeboy.c:525: gotoxy(0, 13);
	ld	hl, #0xd00
	push	hl
	call	_gotoxy
	pop	hl
;snakeboy.c:526: printf("   START  to save   ");
	ld	de, #___str_24
	push	de
	call	_printf
	pop	hl
;snakeboy.c:529: for (i = 0; i < NAME_LENGTH; i++) {
	ld	c, #0x00
00123$:
;snakeboy.c:530: name[i] = ' '; // Initialize with spaces
	ld	e, c
	ld	d, #0x00
	ld	hl, #0
	add	hl, sp
	add	hl, de
	ld	(hl), #0x20
;snakeboy.c:529: for (i = 0; i < NAME_LENGTH; i++) {
	inc	c
	ld	a, c
	sub	a, #0x03
	jr	C, 00123$
;snakeboy.c:532: name[NAME_LENGTH] = '\0'; // Null-terminate the string
	ldhl	sp,	#3
;snakeboy.c:534: strncpy(name, DEFAULT_NAME, NAME_LENGTH); // Copy default name to the buffer
	ld	de, #0x0003
	ld	(hl), d
	push	de
	ld	bc, #_DEFAULT_NAME
	ld	hl, #2
	add	hl, sp
	ld	e, l
	ld	d, h
	call	_strncpy
;snakeboy.c:539: int cursor_pos = 0; // Cursor position in the name
	xor	a, a
	ldhl	sp,	#4
	ld	(hl+), a
	ld	(hl), a
;snakeboy.c:540: while (1) {
00121$:
;snakeboy.c:542: UBYTE joy = joypad();
	call	_joypad
	ld	c, a
;snakeboy.c:544: name[cursor_pos] = (name[cursor_pos] == 'Z') ? 'A' : name[cursor_pos] + 1; // Cycle through letters A-Z
	push	hl
	ld	hl, #2
	add	hl, sp
	ld	e, l
	ld	d, h
	pop	hl
	ldhl	sp,	#4
	ld	a,	(hl+)
	ld	h, (hl)
	ld	l, a
	add	hl, de
	push	hl
	ld	a, l
	ldhl	sp,	#8
	ld	(hl), a
	pop	hl
	ld	a, h
	ldhl	sp,	#7
	ld	(hl), a
;snakeboy.c:543: if (joy & J_UP)
	bit	2, c
	jr	Z, 00114$
;snakeboy.c:544: name[cursor_pos] = (name[cursor_pos] == 'Z') ? 'A' : name[cursor_pos] + 1; // Cycle through letters A-Z
	ldhl	sp,#6
	ld	a, (hl+)
	ld	e, a
	ld	d, (hl)
	ld	a, (de)
	cp	a, #0x5a
	jr	NZ, 00129$
	ld	c, #0x41
	jr	00130$
00129$:
	inc	a
	ld	c, a
00130$:
	ldhl	sp,	#6
	ld	a, (hl+)
	ld	h, (hl)
	ld	l, a
	ld	(hl), c
	jr	00115$
00114$:
;snakeboy.c:545: else if (joy & J_DOWN)
	bit	3, c
	jr	Z, 00111$
;snakeboy.c:546: name[cursor_pos] = (name[cursor_pos] == 'A') ? 'Z' : name[cursor_pos] - 1; // Cycle through letters Z-A
	ldhl	sp,#6
	ld	a, (hl+)
	ld	e, a
	ld	a, (hl+)
	ld	d, a
	ld	a, (de)
	ld	(hl), a
	sub	a, #0x41
	jr	NZ, 00131$
	ld	c, #0x5a
	jr	00132$
00131$:
	ldhl	sp,	#8
	ld	a, (hl)
	dec	a
	ld	c, a
00132$:
	ldhl	sp,	#6
	ld	a, (hl+)
	ld	h, (hl)
	ld	l, a
	ld	(hl), c
	jr	00115$
00111$:
;snakeboy.c:547: else if (joy & J_LEFT)
	bit	1, c
	jr	Z, 00108$
;snakeboy.c:548: cursor_pos = (cursor_pos > 0) ? cursor_pos - 1 : NAME_LENGTH - 1; // Move cursor left
	ldhl	sp,	#4
	xor	a, a
	sub	a, (hl)
	inc	hl
	ld	a, #0x00
	sbc	a, (hl)
	jr	NC, 00133$
	ldhl	sp,#4
	ld	a, (hl+)
	ld	c, a
	ld	b, (hl)
	dec	bc
	jr	00134$
00133$:
	ld	bc, #0x0002
00134$:
	ldhl	sp,	#4
	ld	a, c
	ld	(hl+), a
	ld	(hl), b
	jr	00115$
00108$:
;snakeboy.c:549: else if (joy & J_RIGHT)
	bit	0, c
	jr	Z, 00105$
;snakeboy.c:550: cursor_pos = (cursor_pos < NAME_LENGTH - 1) ? cursor_pos + 1 : 0; // Move cursor right
	ldhl	sp,	#4
	ld	a, (hl+)
	sub	a, #0x02
	ld	a, (hl)
	sbc	a, #0x00
	jr	NC, 00135$
	dec	hl
	ld	c, (hl)
	ld	b, #0x00
	inc	bc
	jr	00136$
00135$:
	ld	bc, #0x0000
00136$:
	ldhl	sp,	#4
	ld	a, c
	ld	(hl+), a
	ld	(hl), b
	jr	00115$
00105$:
;snakeboy.c:551: else if (joy & J_START) {
	bit	7, c
	jr	Z, 00115$
;snakeboy.c:552: update_high_scores(score, name); // Update high scores with the new score and name
	ld	hl, #0
	add	hl, sp
	ld	c, l
	ld	b, h
	ld	a, (_score)
	ld	e, a
	ld	hl, #_score + 1
	ld	d, (hl)
	call	_update_high_scores
;snakeboy.c:553: menu(); // Return to the main menu after saving
	call	_menu
;snakeboy.c:554: break; // Exit the input loop
	jr	00127$
00115$:
;snakeboy.c:558: gotoxy(2, 7); // Move cursor to the line below the prompt
	ld	hl, #0x702
	push	hl
	call	_gotoxy
	pop	hl
;snakeboy.c:559: printf("Name: ");
	ld	de, #___str_25
	push	de
	call	_printf
	pop	hl
;snakeboy.c:560: for (i = 0; i < NAME_LENGTH; i++) {
	ld	bc, #0x0000
00125$:
;snakeboy.c:562: printf("[%c]", name[i]); // Highlight the current character
	ld	hl, #0
	add	hl, sp
	add	hl, bc
	ld	a, (hl)
	ld	e, a
	rlca
	sbc	a, a
	ld	d, a
;snakeboy.c:561: if (i == cursor_pos) {
	ldhl	sp,	#4
	ld	a, (hl)
	sub	a, c
	jr	NZ, 00117$
	xor	a, a
	sub	a, b
	jr	NZ, 00117$
;snakeboy.c:562: printf("[%c]", name[i]); // Highlight the current character
	push	bc
	push	de
	ld	de, #___str_26
	push	de
	call	_printf
	add	sp, #4
	pop	bc
	jr	00126$
00117$:
;snakeboy.c:564: printf(" %c ", name[i]); // Print other characters normally
	push	bc
	push	de
	ld	de, #___str_27
	push	de
	call	_printf
	add	sp, #4
	pop	bc
00126$:
;snakeboy.c:560: for (i = 0; i < NAME_LENGTH; i++) {
	inc	bc
	ld	a, c
	sub	a, #0x03
	jr	C, 00125$
;snakeboy.c:567: wait_vbl_done(); // Wait for VBlank to prevent screen tearing
	call	_wait_vbl_done
;snakeboy.c:569: delay(50); // Small delay to prevent too fast input
	ld	de, #0x0032
	call	_delay
	jp	00121$
00127$:
;snakeboy.c:571: }
	add	sp, #9
	ret
___str_22:
	.ascii "   NEW HIGH SCORE   "
	.db 0x00
___str_23:
	.ascii "  Score: %u"
	.db 0x00
___str_24:
	.ascii "   START  to save   "
	.db 0x00
___str_25:
	.ascii "Name: "
	.db 0x00
___str_26:
	.ascii "[%c]"
	.db 0x00
___str_27:
	.ascii " %c "
	.db 0x00
;snakeboy.c:573: void update_score_display() {
;	---------------------------------
; Function update_score_display
; ---------------------------------
_update_score_display::
;snakeboy.c:574: gotoxy(0, 0); // Move cursor to (1,1) tile position
	xor	a, a
	rrca
	push	af
	call	_gotoxy
	pop	hl
;snakeboy.c:575: printf("SCORE: %u", score); // Print the score in the top left corner
	ld	a, (_score)
	ld	e, a
	ld	hl, #_score + 1
	ld	d, (hl)
	push	de
	ld	de, #___str_28
	push	de
	call	_printf
	add	sp, #4
;snakeboy.c:577: gotoxy(12, 0); // Move cursor to (13,1) tile position
	ld	hl, #0x0c
	push	hl
	call	_gotoxy
	pop	hl
;snakeboy.c:578: printf("SNAKEBOY"); // Print the game title in the top right corner
	ld	de, #___str_29
	push	de
	call	_printf
	pop	hl
;snakeboy.c:586: }
	ret
___str_28:
	.ascii "SCORE: %u"
	.db 0x00
___str_29:
	.ascii "SNAKEBOY"
	.db 0x00
;snakeboy.c:588: void load_save_data() {
;	---------------------------------
; Function load_save_data
; ---------------------------------
_load_save_data::
	add	sp, #-124
;snakeboy.c:590: HighScoresData current_scores = save_data;
	ldhl	sp,	#0
	ld	e, l
	ld	d, h
	ld	bc, #0x003e
	push	bc
	ld	bc, #_save_data
	call	___memcpy
;snakeboy.c:595: ENABLE_RAM_MBC5;
	ld	hl, #_rRAMG
	ld	(hl), #0x0a
;snakeboy.c:596: SWITCH_RAM(0); // Switch to SRAM bank for high scores
	xor	a, a
	ld	(#_rRAMB),a
;snakeboy.c:597: memcpy(&temp_scores, (void *)0xA000, sizeof(save_data)); // Load from RAM at 0xC000
	ld	de, #0x003e
	push	de
	ld	bc, #0xa000
	ld	hl, #64
	add	hl, sp
	ld	e, l
	ld	d, h
	call	_memcpy
;snakeboy.c:598: SWITCH_ROM(1); // Switch back to ROM bank 1 for game code
	ld	a, #0x01
	ldh	(__current_bank + 0), a
	ld	hl, #_rROMB0
	ld	(hl), #0x01
;snakeboy.c:599: DISABLE_RAM_MBC5; // Disable RAM access for MBC5
	xor	a, a
	ld	(#_rRAMG),a
;snakeboy.c:602: gotoxy(0, 9); // Move cursor to the last line
	ld	hl, #0x900
	push	hl
	call	_gotoxy
	pop	hl
;snakeboy.c:604: if (temp_scores.checksum == 8008) {
	ldhl	sp,	#122
	ld	a, (hl+)
	ld	c, a
	ld	b, (hl)
	ld	a, c
	sub	a, #0x48
	jr	NZ, 00102$
	ld	a, b
	sub	a, #0x1f
	jr	NZ, 00102$
;snakeboy.c:606: memcpy(&save_data, &temp_scores, sizeof(HighScoresData));
	ld	de, #0x003e
	push	de
	ld	hl, #64
	add	hl, sp
	ld	c, l
	ld	b, h
	ld	de, #_save_data
	call	_memcpy
;snakeboy.c:607: printf("    High scores"); // Print the checksum
	ld	de, #___str_30
	push	de
	call	_printf
	pop	hl
	jr	00104$
00102$:
;snakeboy.c:609: store_save_data(); // Save the current scores back to RAM
	call	_store_save_data
;snakeboy.c:610: printf("   Default scores   "); // Print the checksum
	ld	de, #___str_31
	push	de
	call	_printf
	pop	hl
00104$:
;snakeboy.c:612: }
	add	sp, #124
	ret
___str_30:
	.ascii "    High scores"
	.db 0x00
___str_31:
	.ascii "   Default scores   "
	.db 0x00
;snakeboy.c:614: void store_save_data() {
;	---------------------------------
; Function store_save_data
; ---------------------------------
_store_save_data::
;snakeboy.c:615: ENABLE_RAM_MBC5;
	ld	hl, #_rRAMG
	ld	(hl), #0x0a
;snakeboy.c:616: SWITCH_RAM(0); // Switch to SRAM bank for high scores
	xor	a, a
	ld	(#_rRAMB),a
;snakeboy.c:617: memcpy((void *)0xA000, &save_data, sizeof(save_data)); // Save to RAM at 0xA000
	ld	de, #0x003e
	push	de
	ld	bc, #_save_data
	ld	de, #0xa000
	call	_memcpy
;snakeboy.c:618: SWITCH_ROM(1);
	ld	a, #0x01
	ldh	(__current_bank + 0), a
	ld	hl, #_rROMB0
	ld	(hl), #0x01
;snakeboy.c:619: DISABLE_RAM_MBC5; // Disable RAM access for MBC5
	xor	a, a
	ld	(#_rRAMG),a
;snakeboy.c:620: }
	ret
;snakeboy.c:622: void update_high_scores(UINT16 new_score, const char *new_name) {
;	---------------------------------
; Function update_high_scores
; ---------------------------------
_update_high_scores::
;snakeboy.c:624: strncpy(save_data.scores[MAX_HIGHSCORES - 1].name, new_name, NAME_LENGTH);
	push	de
	ld	hl, #0x0003
	push	hl
	ld	de, #(_save_data + 54)
	call	_strncpy
	pop	de
;snakeboy.c:625: save_data.scores[MAX_HIGHSCORES - 1].name[NAME_LENGTH] = '\0'; // Ensure null termination
	ld	hl, #_save_data + 57
	ld	(hl), #0x00
;snakeboy.c:626: save_data.scores[MAX_HIGHSCORES - 1].score = new_score;
	ld	hl, #_save_data + 58
	ld	a, e
	ld	(hl+), a
	ld	(hl), d
;snakeboy.c:629: sort_high_scores();
	call	_sort_high_scores
;snakeboy.c:632: store_save_data();
	call	_store_save_data
;snakeboy.c:635: load_save_data();
;snakeboy.c:636: }
	jp	_load_save_data
;snakeboy.c:638: void sort_high_scores() {
;	---------------------------------
; Function sort_high_scores
; ---------------------------------
_sort_high_scores::
	add	sp, #-22
;snakeboy.c:640: for (int i = 0; i < MAX_HIGHSCORES - 1; i++) {
	ld	bc, #0x0000
00109$:
	ld	a, c
	sub	a, #0x09
	jp	NC, 00111$
;snakeboy.c:641: for (int j = i + 1; j < MAX_HIGHSCORES; j++) {
	ld	l, c
	ld	h, b
	inc	hl
	push	hl
	ld	a, l
	ldhl	sp,	#10
	ld	(hl), a
	pop	hl
	ld	a, h
	ldhl	sp,	#9
	ld	(hl-), a
	ld	a, (hl)
	ldhl	sp,	#20
	ld	(hl+), a
	ld	(hl), #0x00
	ld	l, c
	ld	h, b
	add	hl, hl
	add	hl, bc
	add	hl, hl
	push	hl
	ld	a, l
	ldhl	sp,	#12
	ld	(hl), a
	pop	hl
	ld	a, h
	ldhl	sp,	#11
	ld	(hl-), a
	ld	a, (hl+)
	ld	e, a
	ld	d, (hl)
	ld	hl, #_save_data + 0x0004
	add	hl, de
	push	hl
	ld	a, l
	ldhl	sp,	#14
	ld	(hl), a
	pop	hl
	ld	a, h
	ldhl	sp,	#13
	ld	(hl), a
00106$:
	ldhl	sp,	#20
	ld	a, (hl+)
	sub	a, #0x0a
	ld	a, (hl)
	sbc	a, #0x00
	jp	NC, 00110$
;snakeboy.c:642: if (save_data.scores[i].score < save_data.scores[j].score) {
	ldhl	sp,#12
	ld	a, (hl+)
	ld	e, a
	ld	a, (hl+)
	ld	d, a
	ld	a, (de)
	ld	(hl+), a
	inc	de
	ld	a, (de)
	ld	(hl), a
	ldhl	sp,#20
	ld	a, (hl+)
	ld	c, a
	ld	b, (hl)
	ld	l, c
	ld	h, b
	add	hl, hl
	add	hl, bc
	add	hl, hl
	push	hl
	ld	a, l
	ldhl	sp,	#18
	ld	(hl), a
	pop	hl
	ld	a, h
	ldhl	sp,	#17
	ld	(hl), a
	ld	de, #_save_data
	ld	a, (hl-)
	ld	l, (hl)
	ld	h, a
	add	hl, de
	inc	sp
	inc	sp
	ld	e, l
	ld	d, h
	push	de
	ld	hl, #0x0004
	add	hl, de
	push	hl
	ld	a, l
	ldhl	sp,	#20
	ld	(hl), a
	pop	hl
	ld	a, h
	ldhl	sp,	#19
	ld	(hl-), a
	ld	a, (hl+)
	ld	e, a
	ld	a, (hl-)
	ld	d, a
	ld	a, (de)
	ld	(hl+), a
	inc	de
	ld	a, (de)
	ld	(hl), a
	ldhl	sp,	#14
	ld	e, l
	ld	d, h
	ldhl	sp,	#18
	ld	a, (de)
	inc	de
	sub	a, (hl)
	inc	hl
	ld	a, (de)
	sbc	a, (hl)
	jr	NC, 00107$
;snakeboy.c:644: HighScore temp = save_data.scores[i];
	ld	de, #_save_data
	ldhl	sp,	#10
	ld	a,	(hl+)
	ld	h, (hl)
	ld	l, a
	add	hl, de
	ld	c, l
	ld	a, h
	ldhl	sp,	#18
	ld	(hl), c
	inc	hl
	ld	(hl-), a
	ld	de, #0x0006
	push	de
	ld	a, (hl+)
	ld	c, a
	ld	b, (hl)
	ld	hl, #4
	add	hl, sp
	ld	e, l
	ld	d, h
	call	___memcpy
;snakeboy.c:645: save_data.scores[i] = save_data.scores[j];
	ld	de, #_save_data
	ldhl	sp,	#16
	ld	a,	(hl+)
	ld	h, (hl)
	ld	l, a
	add	hl, de
	ld	e, l
	ld	d, h
	push	de
	ld	bc, #0x0006
	push	bc
	ld	c, e
	ld	b, d
	ldhl	sp,	#22
	ld	a, (hl+)
	ld	e, a
	ld	d, (hl)
	call	___memcpy
	pop	de
;snakeboy.c:646: save_data.scores[j] = temp;
	ld	bc, #0x0006
	push	bc
	ld	hl, #4
	add	hl, sp
	ld	c, l
	ld	b, h
	call	___memcpy
00107$:
;snakeboy.c:641: for (int j = i + 1; j < MAX_HIGHSCORES; j++) {
	ldhl	sp,	#20
	inc	(hl)
	jp	00106$
00110$:
;snakeboy.c:640: for (int i = 0; i < MAX_HIGHSCORES - 1; i++) {
	ldhl	sp,	#8
	ld	c, (hl)
	ld	b, #0x00
	jp	00109$
00111$:
;snakeboy.c:650: }
	add	sp, #22
	ret
;snakeboy.c:652: int check_if_high_score(UINT16 new_score) {
;	---------------------------------
; Function check_if_high_score
; ---------------------------------
_check_if_high_score::
	ld	c, e
	ld	b, d
;snakeboy.c:654: if (new_score > save_data.scores[MAX_HIGHSCORES - 1].score) {
	ld	hl, #(_save_data + 58)
	ld	a,	(hl+)
	ld	h, (hl)
	sub	a, c
	ld	a, h
	sbc	a, b
	jr	NC, 00102$
;snakeboy.c:655: return 1; // New score is a high score
	ld	bc, #0x0001
	ret
00102$:
;snakeboy.c:657: return 0; // New score is not a high score
	ld	bc, #0x0000
;snakeboy.c:659: }
	ret
	.area _CODE
	.area _INITIALIZER
__xinit__save_data:
	.ascii "AAA"
	.db 0x00
	.dw #0x0064
	.ascii "BBB"
	.db 0x00
	.dw #0x005a
	.ascii "CCC"
	.db 0x00
	.dw #0x0050
	.ascii "DDD"
	.db 0x00
	.dw #0x0046
	.ascii "EEE"
	.db 0x00
	.dw #0x003c
	.ascii "FFF"
	.db 0x00
	.dw #0x0032
	.ascii "GGG"
	.db 0x00
	.dw #0x0028
	.ascii "HHH"
	.db 0x00
	.dw #0x001e
	.ascii "III"
	.db 0x00
	.dw #0x0014
	.ascii "JJJ"
	.db 0x00
	.dw #0x000a
	.dw #0x1f48
	.area _CABS (ABS)
