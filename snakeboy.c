#include <gb/gb.h>   // GBDK core library
#include <stdio.h>   // Standard I/O for printing (e.g., score)
#include <rand.h>    // For random number generation (food placement)
#include <gbdk/console.h> // For console functions like gotoxy, printf
#include <gbdk/font.h> // Removed this include, font_init and printf usually work without it

// --- Game Constants ---
#define SCREEN_WIDTH_TILES  20  // GameBoy screen width in tiles (160 pixels / 8 pixels/tile)
#define SCREEN_HEIGHT_TILES 18  // GameBoy screen height in tiles (144 pixels / 8 pixels/tile) - 2 rows for status/border


/*

      SNAKEBOY      

    Press  START    
      to play!

 AAA 9999  AAA 9999 
 BBB 9999  BBB 9999
 CCC 9999  CCC 9999
 DDD 9999  DDD 9999
 EEE 9999  EEE 9999

*/

#define MAX_SNAKE_LENGTH    100 // Maximum possible snake length
#define INITIAL_SNAKE_LENGTH 3  // Starting length of the snake

// Tile IDs for our simple graphics
#define TILE_EMPTY          0
#define TILE_SNAKE_HEAD     1
#define TILE_SNAKE_BODY     2
#define TILE_FOOD           3
#define TILE_BORDER         4 // A simple border tile

// Directions
#define DIR_UP    0
#define DIR_DOWN  1
#define DIR_LEFT  2
#define DIR_RIGHT 3

// Game Speed (delay between updates)
#define GAME_SPEED_MS 150 // Milliseconds delay, lower is faster

// --- Game State Variables ---
UBYTE snake_x[MAX_SNAKE_LENGTH]; // X coordinates of snake segments
UBYTE snake_y[MAX_SNAKE_LENGTH]; // Y coordinates of snake segments
UBYTE snake_length;              // Current length of the snake
UBYTE snake_direction;           // Current direction of the snake

UBYTE food_x, food_y;            // Food position

UINT16 score;                    // Player's score
UBYTE  game_over_flag;           // Flag to indicate game over

// --- Tile Data (simple 8x8 pixel tiles) ---
// Each byte represents 2 pixels (00 = color 0, 01 = color 1, 10 = color 2, 11 = color 3)
// GameBoy palette is typically 0=white, 1=light gray, 2=dark gray, 3=black

// Tile 0: Empty (all white)
const unsigned char empty_tile[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

// Tile 1: Snake Head (green snake head)
const unsigned char snake_head_tile[] = {
    0x18, 0x00, 0x3c, 0x18, 0x7e, 0x3c, 0xff, 0x7e,
    0xff, 0x7e, 0x7e, 0x3c, 0x3c, 0x18, 0x18, 0x00
};

// Tile 2: Snake Body (black square with a dark gray border)
const unsigned char snake_body_tile[] = {
    0xff, 0x00, 0xff, 0x7e, 0xff, 0x7e, 0xff, 0x7e,
    0xff, 0x7e, 0xff, 0x7e, 0xff, 0x7e, 0xff, 0x00
};

// Tile 3: Food (faded dot)
const unsigned char food_tile[] = {
    0x00, 0x00, 0x7e, 0x00, 0x42, 0x3c, 0x5a, 0x3c,
    0x5a, 0x3c, 0x42, 0x3c, 0x7e, 0x00, 0x00, 0x00
};

// Tile 4: Border (solid black)
const unsigned char border_tile[] = {
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
};

const unsigned char score_str[] = "SCORE: %d";

// --- Function Prototypes ---
void init_game();
void clear_screen();
void menu();
void run_game();
void draw_game_area();
void draw_snake();
void draw_food();
void generate_food();
void handle_input();
void update_game();
void game_over_screen();
void update_score_display();

// --- Main Game Loop ---
void main() {
    font_init(); // Initialize font system

    // Load custom tiles into VRAM
    set_bkg_data(TILE_EMPTY, 1, empty_tile);
    set_bkg_data(TILE_SNAKE_HEAD, 1, snake_head_tile);
    set_bkg_data(TILE_SNAKE_BODY, 1, snake_body_tile);
    set_bkg_data(TILE_FOOD, 1, food_tile);
    set_bkg_data(TILE_BORDER, 1, border_tile);

    // Set the background palette (all tiles use this)
    BGP_REG = 0xE4; // 11100100 -> Color 3,2,1,0 (Black, Dark Gray, Light Gray, White)

    // Show the background layer
    SHOW_BKG;
    DISPLAY_ON;

    menu();
}

void menu() {    
    // Seed the random number generator
    //initrand(DIV_REG);

    // Clear the screen
    clear_screen();

    // Main menu screen
    gotoxy(6, 4);
    printf("SNAKEBOY");
    gotoxy(4, 6);
    printf("Press  START");
    gotoxy(6, 7);
    printf("to play.");

    // Hight scores
    gotoxy(1, 10);
    printf("AAA 9999");
    gotoxy(1, 11);
    printf("BBB 9999");
    gotoxy(1, 12);
    printf("CCC 9999");
    gotoxy(1, 13);
    printf("DDD 9999");
    gotoxy(1, 14);
    printf("EEE 9999");

    gotoxy(11, 10);
    printf("FFF 9999");
    gotoxy(11, 11);
    printf("GGG 9999");
    gotoxy(11, 12);
    printf("HHH 9999");
    gotoxy(11, 13);
    printf("III 9999");
    gotoxy(11, 14);
    printf("JJJ 9999");

    // Wait for START button to be pressed
    while (joypad() == 0) {
        wait_vbl_done(); // Wait for VBlank to prevent screen tearing
    }

    run_game();
}

void run_game() {
    // Load custom tiles into VRAM
    set_bkg_data(TILE_EMPTY, 1, empty_tile);
    set_bkg_data(TILE_SNAKE_HEAD, 1, snake_head_tile);
    set_bkg_data(TILE_SNAKE_BODY, 1, snake_body_tile);
    set_bkg_data(TILE_FOOD, 1, food_tile);
    set_bkg_data(TILE_BORDER, 1, border_tile);

    // Set the background palette (all tiles use this)
    //BGP_REG = 0xE4; // 11100100 -> Color 3,2,1,0 (Black, Dark Gray, Light Gray, White)

    // Show the background layer
    //SHOW_BKG;
    //DISPLAY_ON;

    // Seed the random number generator
    initrand(DIV_REG);

    // Game loop
    while (1) {
        init_game(); // Initialize game state for a new round

        // Main game play loop
        while (!game_over_flag) {
            handle_input();
            update_game();
            draw_snake();
            draw_food();
            update_score_display();

            // Wait for VBlank to prevent screen tearing and control speed
            wait_vbl_done();
            delay(GAME_SPEED_MS);
        }

        game_over_screen(); // Display game over message
        // Wait for user input to restart or quit
        //while (joypad() == 0) {
        //    wait_vbl_done();
        //}
        // Small delay to prevent immediate re-start if button held
        //delay(200);
    }
}

// --- Game Initialization ---
void init_game(void) {
    clear_screen();
    
    UBYTE i;
    
    draw_game_area(); // Draw the borders

    snake_length = INITIAL_SNAKE_LENGTH;
    // Initial snake position (center of the screen)
    snake_x[0] = SCREEN_WIDTH_TILES / 2;
    snake_y[0] = SCREEN_HEIGHT_TILES / 2;
    // Body segments follow
    for (i = 1; i < snake_length; i++) {
        snake_x[i] = snake_x[0];
        snake_y[i] = snake_y[0] + i; // Start moving up
    }
    snake_direction = DIR_UP; // Initial direction

    score = 0;
    game_over_flag = 0;

    generate_food(); // Place the first food item
}

// --- Draw Game Area (Borders) ---
void draw_game_area() {
    UBYTE x, y;
    UBYTE tile_border_arr[] = {TILE_BORDER}; // Temporary array for TILE_BORDER

    // Top and Bottom borders
    for (x = 0; x < SCREEN_WIDTH_TILES; x++) {
        set_bkg_tiles(x, 0, 1, 1, tile_border_arr); // Top row
        set_bkg_tiles(x, SCREEN_HEIGHT_TILES - 1, 1, 1, tile_border_arr); // Bottom row
    }

    // Left and Right borders
    for (y = 1; y < SCREEN_HEIGHT_TILES - 1; y++) { // Exclude corners handled by top/bottom
        set_bkg_tiles(0, y, 1, 1, tile_border_arr); // Left column
        set_bkg_tiles(SCREEN_WIDTH_TILES - 1, y, 1, 1, tile_border_arr); // Right column
    }
}

// --- Drawing Functions ---
void draw_snake() {
    UBYTE i;
    UBYTE tile_empty_arr[] = {TILE_EMPTY};     // Temporary array for TILE_EMPTY
    UBYTE tile_snake_head_arr[] = {TILE_SNAKE_HEAD}; // Temporary array for TILE_SNAKE_HEAD
    UBYTE tile_snake_body_arr[] = {TILE_SNAKE_BODY}; // Temporary array for TILE_SNAKE_BODY

    // Clear the old tail (if it moved)
    if (snake_length < MAX_SNAKE_LENGTH) { // Only clear if not maxed out
        set_bkg_tiles(snake_x[snake_length], snake_y[snake_length], 1, 1, tile_empty_arr);
    }

    // Draw head
    set_bkg_tiles(snake_x[0], snake_y[0], 1, 1, tile_snake_head_arr);

    // Draw body segments
    for (i = 1; i < snake_length; i++) {
        set_bkg_tiles(snake_x[i], snake_y[i], 1, 1, tile_snake_body_arr);
    }
}

void draw_food() {
    UBYTE tile_food_arr[] = {TILE_FOOD}; // Temporary array for TILE_FOOD
    set_bkg_tiles(food_x, food_y, 1, 1, tile_food_arr);
}

// --- Food Generation ---
void generate_food() {
    UBYTE new_food_x, new_food_y;
    UBYTE collision;
    UBYTE i;

    do {
        collision = 0;
        // Generate random coordinates within the playable area (excluding borders)
        new_food_x = (rand() % (SCREEN_WIDTH_TILES - 2)) + 1;
        new_food_y = (rand() % (SCREEN_HEIGHT_TILES - 2)) + 1;

        // Check if new food position collides with snake
        for (i = 0; i < snake_length; i++) {
            if (new_food_x == snake_x[i] && new_food_y == snake_y[i]) {
                collision = 1;
                break;
            }
        }
    } while (collision); // Keep trying until a non-colliding spot is found

    food_x = new_food_x;
    food_y = new_food_y;
}

// --- Input Handling ---
void handle_input() {
    UBYTE joy = joypad();

    // Prevent immediate reverse direction
    if ((joy & J_UP) && snake_direction != DIR_DOWN) {
        snake_direction = DIR_UP;
    } else if ((joy & J_DOWN) && snake_direction != DIR_UP) {
        snake_direction = DIR_DOWN;
    } else if ((joy & J_LEFT) && snake_direction != DIR_RIGHT) {
        snake_direction = DIR_LEFT;
    } else if ((joy & J_RIGHT) && snake_direction != DIR_LEFT) {
        snake_direction = DIR_RIGHT;
    }
}

// --- Game Logic Update ---
void update_game() {
    UBYTE i;
    UBYTE head_x = snake_x[0];
    UBYTE head_y = snake_y[0];

    // Move body segments (from tail to head)
    for (i = snake_length; i > 0; i--) {
        snake_x[i] = snake_x[i - 1];
        snake_y[i] = snake_y[i - 1];
    }

    // Move head based on direction
    switch (snake_direction) {
        case DIR_UP:    snake_y[0]--; break;
        case DIR_DOWN:  snake_y[0]++; break;
        case DIR_LEFT:  snake_x[0]--; break;
        case DIR_RIGHT: snake_x[0]++; break;
    }

    // --- Collision Detection ---

    // 1. Wall Collision
    if (snake_x[0] == 0 || snake_x[0] == SCREEN_WIDTH_TILES - 1 ||
        snake_y[0] == 0 || snake_y[0] == SCREEN_HEIGHT_TILES - 1) {
        game_over_flag = 1;
        return;
    }

    // 2. Self-Collision
    for (i = 1; i < snake_length; i++) {
        if (snake_x[0] == snake_x[i] && snake_y[0] == snake_y[i]) {
            game_over_flag = 1;
            return;
        }
    }

    // 3. Food Collision
    if (snake_x[0] == food_x && snake_y[0] == food_y) {
        score += 10; // Increase score
        if (snake_length < MAX_SNAKE_LENGTH) {
            snake_length++; // Increase snake length
        }
        generate_food(); // Generate new food
    }
}

void clear_screen() {
    UBYTE i, j;
    UBYTE tile_empty_arr[] = {TILE_EMPTY}; // Temporary array for TILE_EMPTY

    // Clear screen
    for (i = 0; i < SCREEN_WIDTH_TILES; i++) {
        for (j = 0; j < SCREEN_HEIGHT_TILES; j++) {
            set_bkg_tiles(i, j, 1, 1, tile_empty_arr);
        }
    }
}

// --- Game Over Screen ---
void game_over_screen() {

    clear_screen(); // Clear the screen before showing game over

    char score_str[10];

    /*
    
    =====GAME  OVER=====

    =====Score: XXX=====

    ==Press any button==
    =====to restart.====

    */
    gotoxy(6, 6); // Move cursor to (1,1) tile position
    // Display "GAME OVER"
    printf("GAME OVER");
    // Display final score
    gotoxy(6, 8);
    sprintf(score_str, "SCORE: %u", score);
    printf("%s", score_str);
    gotoxy(2, 10);
    printf("Press any button");
    gotoxy(5, 11);
    printf("to restart.");

    // Ensure text is visible

    // Check for any key input
    while (joypad() == 0) {
        wait_vbl_done(); // Wait for VBlank to prevent screen tearing
    }

    delay(200); // Small delay to prevent immediate re-start if button held

    // Check if it's a new high score
    // if it is, show name input screen
    // and save the score

    menu(); // Return to menu after game over
    
}

// --- Update Score Display ---
void update_score_display() {
    return;
    UBYTE tile_empty_arr_8[] = {TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY}; // Temporary array for 8 TILE_EMPTYs
    gotoxy(1, 1); // Move cursor to (1,1) tile position
    // Clear previous score display
    set_bkg_tiles(1, 1, 8, 1, tile_empty_arr_8); // Clear the previous score display
    
    printf(score);

    return;
    char score_str[10];

    gotoxy(1, 1);
    printf(score); // Print score at (2,2) tile position

    //char score_str[10];
    //UBYTE tile_empty_arr_8[] = {TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY, TILE_EMPTY}; // Temporary array for 8 TILE_EMPTYs

    // Clear top left corner where score will be displayed
    //set_bkg_tiles(1, 1, 8, 1, tile_empty_arr_8);
    gotoxy(1, 1); // Move cursor to (1,1) tile position
    //sprintf(score_str, "SCORE:%u", score);
    // Print score at (1,1) tile position (after border)
    // Note: printf uses character tiles, not our custom background tiles
    // So it overwrites the background tiles where it prints.
    // gotoxy(1, 1); // Removed gotoxy call
    //printf(score_str);

    //printf("SCORE: "); // Print score directly to the console
}
