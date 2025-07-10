/*

  ██████  ███▄    █  ▄▄▄       ██ ▄█▀▓█████  ▄▄▄▄    ▒█████ ▓██   ██▓
▒██    ▒  ██ ▀█   █ ▒████▄     ██▄█▒ ▓█   ▀ ▓█████▄ ▒██▒  ██▒▒██  ██▒
░ ▓██▄   ▓██  ▀█ ██▒▒██  ▀█▄  ▓███▄░ ▒███   ▒██▒ ▄██▒██░  ██▒ ▒██ ██░
  ▒   ██▒▓██▒  ▐▌██▒░██▄▄▄▄██ ▓██ █▄ ▒▓█  ▄ ▒██░█▀  ▒██   ██░ ░ ▐██▓░
▒██████▒▒▒██░   ▓██░ ▓█   ▓██▒▒██▒ █▄░▒████▒░▓█  ▀█▓░ ████▓▒░ ░ ██▒▓░
▒ ▒▓▒ ▒ ░░ ▒░   ▒ ▒  ▒▒   ▓▒█░▒ ▒▒ ▓▒░░ ▒░ ░░▒▓███▀▒░ ▒░▒░▒░   ██▒▒▒ 
░ ░▒  ░ ░░ ░░   ░ ▒░  ▒   ▒▒ ░░ ░▒ ▒░ ░ ░  ░▒░▒   ░   ░ ▒ ▒░ ▓██ ░▒░ 
░  ░  ░     ░   ░ ░   ░   ▒   ░ ░░ ░    ░    ░    ░ ░ ░ ░ ▒  ▒ ▒ ░░  
      ░           ░       ░  ░░  ░      ░  ░ ░          ░ ░  ░ ░     
                                                  ░          ░ ░     

    SNAKEBOY - Snake for the GameBoy
    Author: Jim Valentine <jv@f13.dev>
    License: MIT
    Version: 0.1.4

*/

#include <gb/gb.h>   // GBDK core library
#include <gb/hardware.h> // Hardware definitions for GameBoy
#include <stdio.h>   // Standard I/O for printing (e.g., score)
#include <rand.h>    // For random number generation (food placement)
#include <gbdk/console.h> // For console functions like gotoxy, printf
#include <gbdk/font.h> // Removed this include, font_init and printf usually work without it
#include <string.h> // For string manipulation (e.g., strcpy, strncpy)

// --- Game Constants ---
#define SCREEN_WIDTH_TILES  20  // GameBoy screen width in tiles (160 pixels / 8 pixels/tile)
#define SCREEN_HEIGHT_TILES 18  // GameBoy screen height in tiles (144 pixels / 8 pixels/tile) - 2 rows for status/border
#define MAX_SNAKE_LENGTH    150 // Maximum possible snake length
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

#define SRAM_BANK 1;
// Store an array of 10 high scores, each containing a 3 letter name and a 4 digit number
#define MAX_HIGHSCORES 10
#define NAME_LENGTH 3 // Length of the name (3 letters)
// Default name for high scores
const char DEFAULT_NAME[NAME_LENGTH + 1] = "AAA"; // 3 letters
// Structure to hold high score data
typedef struct {
    char name[NAME_LENGTH + 1]; // 3 letters + null terminator
    UINT16 score;               // Score value
} HighScore;
// Structure to hold all high scores
typedef struct {
    HighScore scores[MAX_HIGHSCORES]; // Array of high scores
    UINT16 checksum; // Checksum for data integrity
} HighScoresData;
// Initialise 10 high scores with AAA, BBB, CCC, etc.. and scores of 10, 20, 30, etc...
// create a blank HighScoresData save_data variable for population from RAM
HighScoresData save_data = {
    .scores = {
        {"AAA", 200},
        {"BBB", 100},
        {"CCC", 100},
        {"DDD", 100},
        {"EEE", 100},
        {"FFF", 100},
        {"GGG", 100},
        {"HHH", 100},
        {"III", 100},
        {"JJJ", 100},
    },
    .checksum = 8008 // Example checksum, will be recalculated later
};

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

static int external_data[10];

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
void load_save_data();
void store_save_data();
void update_high_scores(UINT16 new_score, const char *new_name);

// --- Main Game Loop ---
void main() {
    font_init(); // Initialize font system    

    /*
    ENABLE_RAM_MBC5;
    SWITCH_RAM(0); // Switch to RAM bank 0 for high scores
    external_data[0] = 10;
    SWITCH_ROM(1); // Switch to ROM bank 1 for game code
    DISABLE_RAM_MBC5; // Disable RAM access for MBC5
    */

    // Write the save_data variable to RAM
    /*
    ENABLE_RAM_MBC5;
    SWITCH_RAM(0); // Switch to SRAM bank for high scores
    memcpy((void *)0xA000, &save_data, sizeof(save_data)); // Save to RAM at 0xA000
    SWITCH_ROM(1);
    DISABLE_RAM_MBC5; // Disable RAM access for MBC5
    */


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
    gotoxy(0, 3);
    printf("      SNAKEBOY\n");
    printf("    Press  START\n");
    printf("      to play.\n");
    printf("\n");
    //printf("====================");

    //load_game();
    //save_high_scores();
    //load_high_scores();

    // Load high scores from save data in RAM
    /*
    ENABLE_RAM_MBC5;
    SWITCH_RAM(0); // Switch to SRAM bank for high scores
    memcpy(&save_data, (void *)0xA000, sizeof(save_data)); // Load from RAM at 0xC000
    SWITCH_ROM(1); // Switch back to ROM bank 1 for game code
    DISABLE_RAM_MBC5; // Disable RAM access for MBC5
    */

    load_save_data();

    // foreach high score, print the name and score, ensuring the score has preceeding zeros to make it 4 digits
    for (int i = 0; i < MAX_HIGHSCORES / 2; i++) {
        // Print high score names and scores
        gotoxy(10, i + 11);
        printf(" %s %04d", save_data.scores[i + MAX_HIGHSCORES / 2].name, save_data.scores[i + MAX_HIGHSCORES / 2].score);
        gotoxy(0, i + 11);
        printf(" %s %04d", save_data.scores[i].name, save_data.scores[i].score);


        //printf(" %s %05d ", save_data.scores[i].name, save_data.scores[i].score);
        //printf(" %s %05d\n", save_data.scores[i + MAX_HIGHSCORES / 2].name, save_data.scores[i + MAX_HIGHSCORES / 2].score);
    }

    set_bkg_data(TILE_FOOD, 1, food_tile);
    UBYTE tile_food_arr[] = {TILE_FOOD}; // Temporary array for TILE_FOOD
    set_bkg_tiles(18, 1, 1, 1, tile_food_arr);
    set_bkg_data(TILE_SNAKE_HEAD, 1, snake_head_tile);
    UBYTE tile_snake_head_arr[] = {TILE_SNAKE_HEAD}; // Temporary array for TILE_SNAKE_HEAD
    set_bkg_tiles(18, 4, 1, 1, tile_snake_head_arr);
    set_bkg_data(TILE_SNAKE_BODY, 1, snake_body_tile);
    UBYTE tile_snake_body_arr[] = {TILE_SNAKE_BODY}; // Temporary array for TILE_SNAKE_BODY
    set_bkg_tiles(18, 5, 1, 1, tile_snake_body_arr);
    set_bkg_tiles(18, 6, 1, 1, tile_snake_body_arr);
    set_bkg_tiles(18, 7, 1, 1, tile_snake_body_arr);
    set_bkg_tiles(18, 7, 1, 1, tile_snake_body_arr);
    set_bkg_tiles(17, 7, 1, 1, tile_snake_body_arr);
    set_bkg_tiles(16, 7, 1, 1, tile_snake_body_arr);
    set_bkg_tiles(15, 7, 1, 1, tile_snake_body_arr);
    set_bkg_tiles(14, 7, 1, 1, tile_snake_body_arr);
    set_bkg_tiles(13, 7, 1, 1, tile_snake_body_arr);
    set_bkg_tiles(12, 7, 1, 1, tile_snake_body_arr);
    set_bkg_tiles(11, 7, 1, 1, tile_snake_body_arr);
    set_bkg_tiles(10, 7, 1, 1, tile_snake_body_arr);
    set_bkg_tiles(9, 7, 1, 1, tile_snake_body_arr);
    set_bkg_tiles(8, 7, 1, 1, tile_snake_body_arr);
    set_bkg_tiles(7, 7, 1, 1, tile_snake_body_arr);
    set_bkg_tiles(6, 7, 1, 1, tile_snake_body_arr);
    set_bkg_tiles(5, 7, 1, 1, tile_snake_body_arr);
    set_bkg_tiles(4, 7, 1, 1, tile_snake_body_arr);
    set_bkg_tiles(3, 7, 1, 1, tile_snake_body_arr);
    set_bkg_tiles(2, 7, 1, 1, tile_snake_body_arr);
    set_bkg_tiles(2, 8, 1, 1, tile_snake_body_arr);
    set_bkg_tiles(2, 9, 1, 1, tile_snake_body_arr);
    set_bkg_tiles(1, 9, 1, 1, tile_snake_body_arr);
    set_bkg_tiles(0, 9, 1, 1, tile_snake_body_arr);

    

    // High scores,
    // All scores need to be shown as 4 digit numbers
    // e.g., 0000, 0010, 0020, etc.
    /*
    gotoxy(0, 10);
    printf(" AAA 9999  FFF 4444\n");
    printf(" BBB 8888  GGG 3333\n");
    printf(" CCC 7777  HHH 2222\n");
    printf(" DDD 6666  III 1111\n");
    printf(" EEE 5555  JJJ 0000\n");
    */
    while (1) {
        if (joypad() && joypad() == J_START) {
            run_game(); // Start the game when START is pressed
            break; // Exit the menu loop
        } else if (joypad() && joypad() == J_SELECT) {
            // Handle SELECT button if needed (e.g., show options)
            // For now, we will just clear the screen
            clear_screen();
            gotoxy(6, 4);
            printf("SELECT pressed");
            delay(1000); // Show message for a while
        }

        wait_vbl_done(); // Wait for VBlank to prevent screen tearing
    }
}

/*
void load_high_scores() {
    //ENABLE_RAM;
    // Load high scores from save data
    //memcpy(&save_data, (void *)0xC000, sizeof(save_data)); // Load from RAM
    //DISABLE_RAM;

    ENABLE_RAM;
    SWITCH_RAM(0); // Switch to RAM bank 0
    memcpy(&save_data, (void *)0xC000, sizeof(save_data)); // Load from RAM at 0xC000
    DISABLE_RAM;
}

void save_high_scores() {
    ENABLE_RAM;
    SWITCH_RAM(0); // Switch to RAM bank 0
    // save the save_data to RAM
    memcpy((void *)0xC000, &save_data, sizeof(save_data)); // Save to RAM at 0xC000
    DISABLE_RAM;
    // Alternatively, you can save directly to the save_data structure
    // This is useful if you want to save the high scores at the end of the game
    
    UINT16 save_address = 0x00; // Example address
    // create a variable to store the game state variable
    UINT8 my_game_state_variable = 0; // Example game state variable
    eeprom_write_byte(save_address, my_game_state_variable);

    //ENABLE_RAM
    // Save high scores to save data
    //memcpy((HighScoresData *)0xC000, &save_data, sizeof(save_data)); // Save to RAM at 0xC000
    //DISABLE_RAM;
}
*/

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

        // Play death sound
        NR52_REG = 0x80; // Enable sound
        NR51_REG = 0x11; // Enable sound channel 1
        NR50_REG = 0x77; // Set volume

        NR10_REG = 0x1E; // Set sound frequency sweep
        NR11_REG = 0x10; // Set sound envelope
        NR12_REG = 0xF3; // Set sound length and volume
        NR13_REG = 0x00; // Set sound frequency low byte
        NR14_REG = 0x87; // Set sound frequency high byte and start sound


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
        set_bkg_tiles(x, 1, 1, 1, tile_border_arr); // Top row
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
        new_food_y = (rand() % (SCREEN_HEIGHT_TILES - 3)) + 2; // Start from row 2 to avoid the top border

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
        snake_y[0] == 1 || snake_y[0] == SCREEN_HEIGHT_TILES - 1) {
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

        // Play a sound
        NR52_REG = 0x80; // Enable sound
        NR51_REG = 0x11; // Enable sound channel 1
        NR50_REG = 0x77; // Set volume

        NR10_REG = 0x55; // Set sound frequency sweep
        NR11_REG = 0x10; // Set sound envelope
        NR12_REG = 0xF3; // Set sound length and volume
        NR13_REG = 0x00; // Set sound frequency low byte
        NR14_REG = 0x87; // Set sound frequency high byte and start sound

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
    gotoxy(3, 10);
    printf("Press A button");
    gotoxy(5, 11);
    printf("to restart.");

    // Ensure text is visible

    // Check for any key input
    while (joypad() != J_A) {
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
    gotoxy(0, 0); // Move cursor to (1,1) tile position
    printf("SCORE: %u", score); // Print the score in the top left corner

    gotoxy(12, 0); // Move cursor to (13,1) tile position
    printf("SNAKEBOY"); // Print the game title in the top right corner

    /*
    EASY
    NORMAL
    BRUTAL
    DYNAMIC
    */
}

void load_save_data() {
    // Create a copy of the current high scores
    HighScoresData current_scores = save_data;

    // Create a temporary HighScoresData for loading
    HighScoresData temp_scores;

    ENABLE_RAM_MBC5;
    SWITCH_RAM(0); // Switch to SRAM bank for high scores
    memcpy(&temp_scores, (void *)0xA000, sizeof(save_data)); // Load from RAM at 0xC000
    SWITCH_ROM(1); // Switch back to ROM bank 1 for game code
    DISABLE_RAM_MBC5; // Disable RAM access for MBC5

    // Go to the last line and print the checksum
    gotoxy(0, 9); // Move cursor to the last line
    
    if (temp_scores.checksum == 8008) {
        // Copy temp_scores to save_data
        memcpy(&save_data, &temp_scores, sizeof(HighScoresData));
        printf("    Saved scores    "); // Print the checksum

    } else {
        store_save_data(); // Save the current scores back to RAM
        printf("   Default scores   "); // Print the checksum
    }
    
}

void store_save_data() {
    ENABLE_RAM_MBC5;
    SWITCH_RAM(0); // Switch to SRAM bank for high scores
    memcpy((void *)0xA000, &save_data, sizeof(save_data)); // Save to RAM at 0xA000
    SWITCH_ROM(1);
    DISABLE_RAM_MBC5; // Disable RAM access for MBC5
}

void update_high_scores(UINT16 new_score, const char *new_name) {

}