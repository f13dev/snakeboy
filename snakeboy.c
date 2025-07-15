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
    Version: 0.1.6

*/

#include <gb/gb.h>              // GBDK core library
#include <gb/hardware.h>        // Hardware definitions for GameBoy
#include <stdio.h>              // Standard I/O for printing (e.g., score)
#include <rand.h>               // For random number generation (food placement)
#include <gbdk/console.h>       // For console functions like gotoxy, printf
#include <gbdk/font.h>          // Removed this include, font_init and printf usually work without it
#include <string.h>             // For string manipulation (e.g., strcpy, strncpy)

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
#define TILE_BORDER         4
#define TILE_LOCK           5 // Tile for locked scores
#define TILE_SCORE          6 // Tile for score icon


// Directions
#define DIR_UP    0
#define DIR_DOWN  1
#define DIR_LEFT  2
#define DIR_RIGHT 3

// Game Speed (delay between updates)
// This is no longer used as we are using frame counting to control speed
//#define GAME_SPEED_MS 150       // Milliseconds delay, lower is faster

// --- High Score System Constants ---
#define SRAM_BANK 1             // SRAM bank for high scores
#define MAX_HIGHSCORES 10       // Maximum number of high scores to store
#define NAME_LENGTH 3           // Length of the name (3 letters)

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
    UINT16 checksum;            // Checksum for data integrity
} HighScoresData;

// Default high score data, this will be loaded if the SRAM is empty or corrupted
// The checksum is currently hard coded to 8008.
HighScoresData save_data = {
    .scores = {
        {"AAA", 10},
        {"BBB", 10},
        {"CCC", 10},
        {"DDD", 10},
        {"EEE", 10},
        {"FFF", 10},
        {"GGG", 10},
        {"HHH", 10},
        {"III", 10},
        {"JJJ", 10},
    },
    .checksum = 8008
};

//UBYTE game_frames_easy = 13;
UBYTE game_frames_easy = 9; // Frame counter for game speed control
UBYTE score_increment_easy = 5; // Score increment for easy mode
//UBYTE reduction_frame_easy = 120; // Frame reduction for easy mode
UBYTE reduction_frame_easy = 90; // Frame reduction for easy mode

//UBYTE game_frames_normal = 9;
UBYTE game_frames_normal = 6; // Frame counter for game speed control
UBYTE score_increment_normal = 10; // Score increment for normal mode
//UBYTE reduction_frame_normal = 60; // Frame reduction for normal mode
UBYTE reduction_frame_normal = 45; // Frame reduction for normal mode

//UBYTE game_frames_hard = 6;
UBYTE game_frames_hard = 4; // Frame counter for game speed control
UBYTE score_increment_hard = 15; // Score increment for hard mode
//UBYTE reduction_frame_hard = 40; // Frame reduction for hard mode
UBYTE reduction_frame_hard = 30; // Frame reduction for hard mode

UBYTE game_frames = 6;     // Frame counter for game speed control
UBYTE score_increment = 10; // Score increment when eating food
UBYTE reduction_frame = 0; // Frame counter for score reduction

// --- Game State Variables ---
UBYTE snake_x[MAX_SNAKE_LENGTH]; // X coordinates of snake segments
UBYTE snake_y[MAX_SNAKE_LENGTH]; // Y coordinates of snake segments
UBYTE snake_length;              // Current length of the snake
UBYTE snake_direction;           // Current direction of the snake
UBYTE food_x, food_y;            // Food position
int score;                    // Player's score
int score_lock = 0;
UBYTE  game_over_flag;           // Flag to indicate game over


// --- Tile Data (simple 8x8 pixel tiles) ---
// Tile 0: Empty (all white)
const unsigned char empty_tile[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

// Tile 1: Snake Head (green snake head)
const unsigned char snake_head_tile[] = {
    0x18,0x00,0x3c,0x18,0x7e,0x3c,0xff,0x7e,0xff,0x7e,0x7e,0x3c,0x3c,0x18,0x18,0x00
};

// Tile 2: Snake Body (black square with a dark gray border)
const unsigned char snake_body_tile[] = {
    0xff,0x00,0xff,0x7e,0xff,0x7e,0xff,0x7e,0xff,0x7e,0xff,0x7e,0xff,0x7e,0xff,0x00
};

// Tile 3: Food (faded dot)
const unsigned char food_tile[] = {
    0x00,0x18,0x00,0x10,0x66,0x6e,0xff,0xff,0xff,0xff,0xff,0xff,0x7e,0x7e,0x3c,0x3c
};

// Tile 4: Border (solid black)
const unsigned char border_tile[] = {
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
};

// Tile 5: Lock icon (for locked scores)
const unsigned char lock_tile[] = {
    0x00,0x00,0x18,0x18,0x24,0x24,0x24,0x24,0x7e,0x7e,0x7e,0x7e,0x7e,0x7e,0x00,0x00
};

// Tile 6: Score icon
const unsigned char score_tile[] = {
    0x00,0x00,0xff,0xff,0x7e,0x7e,0x7e,0x7e,0x3c,0x3c,0x18,0x18,0x7e,0x7e,0x00,0x00
};

const unsigned char score_str[] = "SCORE: %d"; // Format string for score display

// --- Function Prototypes ---
int check_if_high_score(int new_score);
void clear_screen();
void draw_food();
void draw_game_area();
void draw_menu_snake();
void draw_snake();
void game_over_screen();
void generate_food();
void handle_input();
void init_game();
void load_save_data();
void log_high_score();
void menu();
void run_game();
void sort_high_scores();
void store_save_data();
void update_game();
void update_high_scores(int new_score, const char *new_name);
void update_score_display();
void set_game_mode();
void set_game_speed(UBYTE game_mode);
void draw_game_modes(UBYTE game_mode);

// --- Main Game Loop ---
void main() {
    font_init(); // Initialize font system

    // Load custom tiles into VRAM
    set_bkg_data(TILE_EMPTY, 1, empty_tile);
    set_bkg_data(TILE_SNAKE_HEAD, 1, snake_head_tile);
    set_bkg_data(TILE_SNAKE_BODY, 1, snake_body_tile);
    set_bkg_data(TILE_FOOD, 1, food_tile);
    set_bkg_data(TILE_BORDER, 1, border_tile);
    set_bkg_data(TILE_LOCK, 1, lock_tile); // Load lock tile
    set_bkg_data(TILE_SCORE, 1, score_tile); // Load score tile

    // Set the background palette (all tiles use this)
    BGP_REG = 0xE4; // 11100100 -> Color 3,2,1,0 (Black, Dark Gray, Light Gray, White)

    SHOW_BKG; // Show the background layer
    DISPLAY_ON; // Turn on the display

    menu(); // Start the game by showing the menu
}

void menu() {
    // Clear the screen
    clear_screen();

    // Main menu screen
    gotoxy(0, 3);
    printf("      SNAKEBOY\n");
    printf("    Press  START\n");
    printf("      to play.\n");
    printf("\n");

    load_save_data(); // Load high scores from SRAM, defult if not available

    // Print the high scores, two columns of 5 scores each
    for (int i = 0; i < MAX_HIGHSCORES / 2; i++) {
        // Print high score names and scores
        gotoxy(10, i + 11);
        printf(" %s %04d", save_data.scores[i + MAX_HIGHSCORES / 2].name, save_data.scores[i + MAX_HIGHSCORES / 2].score);
        gotoxy(0, i + 11);
        printf(" %s %04d", save_data.scores[i].name, save_data.scores[i].score);
    }

    draw_menu_snake(); // Draw the snake graphics on the menu

    // Wait for user input to start the game
    while (1) {
        // store the joypad state
        UBYTE joy = joypad();
        if (joy && joy == J_START) {
            //run_game(); // Start the game when START is pressed
            waitpadup();
            set_game_mode();
            break; // Exit the menu loop
        } else if (joy && joy == J_SELECT) {
            // Random test code for the SELECT button
            clear_screen();
            gotoxy(6, 4);
            printf("SELECT pressed");
            //delay(1000); // Show message for a while
        }

        wait_vbl_done(); // Wait for VBlank to prevent screen tearing
    }
}

void draw_game_modes(UBYTE game_mode) {
    gotoxy(0, 7);
    if (game_mode == 0) {
        printf("      [ Easy ]      ");
    } else {
        printf("        Easy        ");
    }

    gotoxy(0, 8);
    if (game_mode == 1) {
        printf("     [ Normal ]     ");
    } else {
        printf("       Normal       ");
    }

    gotoxy(0, 9);
    if (game_mode == 2) {
        printf("     [  Hard ]      ");
    } else {
        printf("        Hard        ");
    }
}

void set_game_speed(UBYTE game_mode) {
    switch (game_mode) {
        case 0: // Easy
            game_frames = game_frames_easy;
            score_increment = score_increment_easy;
            reduction_frame = reduction_frame_easy;
            break;
        case 1: // Normal
            game_frames = game_frames_normal;
            score_increment = score_increment_normal;
            reduction_frame = reduction_frame_normal;
            break;
        case 2: // Hard
            game_frames = game_frames_hard;
            score_increment = score_increment_hard;
            reduction_frame = reduction_frame_hard;
            break;
        default:
            game_frames = game_frames_easy; // Default to easy if invalid mode
            score_increment = score_increment_easy;
            reduction_frame = reduction_frame_easy;
    }
}

void set_game_mode() {
    // Menu screen showing Easy, Normal, Hard
    // Show 3 options for game speed, up and down to select and A to start
    clear_screen();
    gotoxy(0, 5);
    printf("  Select Game Mode  ");
    gotoxy(0, 12);
    printf("    Press  START    ");

    UBYTE game_mode = 1; // 0: Easy, 1: Normal, 2: Hard
    
    while(1) {
        UBYTE joy = joypad();
        if (joy & J_UP) {
            if (game_mode > 0) {
                game_mode--; // Move up in the menu
                waitpadup();
            }
        } else if (joy & J_DOWN) {
            if (game_mode < 2) {
                game_mode++; // Move down in the menu
                waitpadup();
            }
        } else if (joy & J_START) {
            set_game_speed(game_mode); // Set the game mode based on selection
            run_game(); // Start the game
        }

        draw_game_modes(game_mode); // Redraw the game modes
    }
}

void draw_menu_snake() {
    // Add a food sprite
    set_bkg_data(TILE_FOOD, 1, food_tile);
    UBYTE tile_food_arr[] = {TILE_FOOD}; // Temporary array for TILE_FOOD
    set_bkg_tiles(18, 1, 1, 1, tile_food_arr);

    // Add a snake head sprite
    set_bkg_data(TILE_SNAKE_HEAD, 1, snake_head_tile);
    UBYTE tile_snake_head_arr[] = {TILE_SNAKE_HEAD}; // Temporary array for TILE_SNAKE_HEAD
    set_bkg_tiles(18, 4, 1, 1, tile_snake_head_arr);

    // Add snake body segments
    set_bkg_data(TILE_SNAKE_BODY, 1, snake_body_tile);
    UBYTE tile_snake_body_arr[] = {TILE_SNAKE_BODY}; // Temporary array for TILE_SNAKE_BODY
    
    for (int i = 5; i < 8; i++) {
        set_bkg_tiles(18, i, 1, 1, tile_snake_body_arr);
    }
    for (int i = 17; i > 1; i--) {
        set_bkg_tiles(i, 7, 1, 1, tile_snake_body_arr);
    }
    set_bkg_tiles(2, 8, 1, 1, tile_snake_body_arr);
    set_bkg_tiles(2, 9, 1, 1, tile_snake_body_arr);
    set_bkg_tiles(1, 9, 1, 1, tile_snake_body_arr);
    set_bkg_tiles(0, 9, 1, 1, tile_snake_body_arr);
}

void run_game() {
    // Load custom tiles into VRAM
    set_bkg_data(TILE_BORDER, 1, border_tile);

    // Seed the random number generator
    initrand(DIV_REG);

    int frame = 0;
    int reduction_frame_count = 0;

    // Game loop
    while (1) {
        init_game(); // Initialize game state for a new round

        // Main game play loop
        while (!game_over_flag) {

            frame++;
            reduction_frame++;

            // Do this outside of the frame check to ensure the input is handled
            // instantly, reducing input lag
            handle_input();

            if (reduction_frame == reduction_frame_count) {
                reduction_frame_count = 0;
                if (score > score_lock) {
                    score--;
                }
            }

            if (frame == game_frames) {
                frame = 0;
            
                update_game();
                draw_snake();
                draw_food();
                update_score_display();
            }

            wait_vbl_done(); // Wait for VBlank to prevent screen tearing
            //delay(GAME_SPEED_MS); // Control game speed
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
    }
}

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
        score_lock = score;

        score += score_increment; // Increase score

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

void game_over_screen() {

    clear_screen(); // Clear the screen before showing game over

    int is_high_score = check_if_high_score(score); // Check if it's a new high score

    // Check if it's a new high score
    if (is_high_score == 1) {
        gotoxy(0, 2);
        printf("   NEW HIGH SCORE   ");
        log_high_score();
        return;
    }        

    char score_str[10];

    gotoxy(6, 6); // Move cursor to (1,1) tile position
    printf("GAME OVER");
    gotoxy(6, 8);
    sprintf(score_str, "SCORE: %u", score);
    printf("%s", score_str);
    gotoxy(3, 10);
    printf("Press A button");
    gotoxy(4, 11);
    if (is_high_score == 1) {
        printf("to  continue");
    } else {
        printf(" to restart.");
    }

    // Check for button A press to restart
    while (joypad() != J_A) {
        wait_vbl_done(); // Wait for VBlank to prevent screen tearing
    }

    waitpadup(); // Wait for button release

    if (is_high_score == 1) {
        log_high_score();
    } else {
        menu(); // Return to menu if not a high score
    }    
}

void log_high_score() {
    clear_screen(); // Clear the screen before logging high score

    // Print the score
    gotoxy(0, 3);
    printf("   NEW HIGH SCORE   ");

    gotoxy(0, 9);
    printf("  Score: %u", score); // Print the score

    gotoxy(0, 13);
    printf("   START  to save   ");
    char name[NAME_LENGTH + 1]; // Buffer for name input
    int i;
    for (i = 0; i < NAME_LENGTH; i++) {
        name[i] = ' '; // Initialize with spaces
    }
    name[NAME_LENGTH] = '\0'; // Null-terminate the string

    strncpy(name, DEFAULT_NAME, NAME_LENGTH); // Copy default name to the buffer

    // Wait for user input for the name
    // Press up or down to change the character, 
    // press left or right to move the cursor, and A to confirm
    int cursor_pos = 0; // Cursor position in the name
    while (1) {
        // store the joypad state
        UBYTE joy = joypad();
        if (joy & J_UP)
            name[cursor_pos] = (name[cursor_pos] == 'Z') ? 'A' : name[cursor_pos] + 1; // Cycle through letters A-Z
        else if (joy & J_DOWN)
            name[cursor_pos] = (name[cursor_pos] == 'A') ? 'Z' : name[cursor_pos] - 1; // Cycle through letters Z-A
        else if (joy & J_LEFT)
            cursor_pos = (cursor_pos > 0) ? cursor_pos - 1 : NAME_LENGTH - 1; // Move cursor left
        else if (joy & J_RIGHT)
            cursor_pos = (cursor_pos < NAME_LENGTH - 1) ? cursor_pos + 1 : 0; // Move cursor right
        else if (joy & J_START) {
            update_high_scores(score, name); // Update high scores with the new score and name
            menu(); // Return to the main menu after saving
            break; // Exit the input loop
        }

        // Update the display with the current name
        gotoxy(2, 7); // Move cursor to the line below the prompt
        printf("Name: ");
        for (i = 0; i < NAME_LENGTH; i++) {
            if (i == cursor_pos) {
                printf("[%c]", name[i]); // Highlight the current character
            } else {
                printf(" %c ", name[i]); // Print other characters normally
            }
        }
        wait_vbl_done(); // Wait for VBlank to prevent screen tearing

        delay(100); // Small delay to prevent too fast input
    }
}

void update_score_display() {
    UBYTE empty_tile_arr[] = {TILE_EMPTY}; // Temporary array for TILE_EMPTY

    set_bkg_data(TILE_SCORE, 1, score_tile); // Load score tile
    UBYTE tile_score_arr[] = {TILE_SCORE}; // Temporary array for TILE_SCORE
    set_bkg_tiles(1, 0, 1, 1, tile_score_arr); // Draw the score icon
    gotoxy(3, 0); // Move cursor to (1,1) tile position
    //printf("SCORE: "); // Print the score in the top left corner
    printf("%u", score); // Print the score
    if (score < 10) { set_bkg_tiles(4, 0, 1, 1, empty_tile_arr); }
    if (score < 100) { set_bkg_tiles(5, 0, 1, 1, empty_tile_arr); }
    if (score < 1000) { set_bkg_tiles(6, 0, 1, 1, empty_tile_arr); }

    // Add a lock icon if the score is locked
    set_bkg_data(TILE_LOCK, 1, lock_tile); // Load lock tile
    UBYTE tile_lock_arr[] = {TILE_LOCK}; // Temporary array for TILE_SNAKE_HEAD
    set_bkg_tiles(11, 0, 1, 1, tile_lock_arr); // Draw the lock icon

    gotoxy(13, 0); // Move cursor to (14,1) tile position
    //if (score_lock < 1000) { printf("0"); }
    //if (score_lock < 100) { printf("0"); }
    //if (score_lock < 10) { printf("0"); }
    printf("%d", score_lock); // Print the score lock value
    if (score_lock < 10) { set_bkg_tiles(14, 0, 1, 1, empty_tile_arr); }
    if (score_lock < 100) { set_bkg_tiles(15, 0, 1, 1, empty_tile_arr); }
    if (score_lock < 1000) { set_bkg_tiles(16, 0, 1, 1, empty_tile_arr); }
    //gotoxy(12, 0); // Move cursor to (13,1) tile position
    //printf("SNAKEBOY"); // Print the game title in the top right corner
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
        printf("    High scores"); // Print the checksum
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

void update_high_scores(int new_score, const char *new_name) {
    // Set the last high score to the new one (the old one drops off)
    strncpy(save_data.scores[MAX_HIGHSCORES - 1].name, new_name, NAME_LENGTH);
    save_data.scores[MAX_HIGHSCORES - 1].name[NAME_LENGTH] = '\0'; // Ensure null termination
    save_data.scores[MAX_HIGHSCORES - 1].score = new_score;

    // Sort the high scores
    sort_high_scores();

    // Save the high scores to SRAM
    store_save_data();

    // Re-load the high scores to ensure they are up-to-date
    load_save_data();
}

void sort_high_scores() {
    // Sort the high scores in descending order
    for (int i = 0; i < MAX_HIGHSCORES - 1; i++) {
        for (int j = i + 1; j < MAX_HIGHSCORES; j++) {
            if (save_data.scores[i].score < save_data.scores[j].score) {
                // Swap scores
                HighScore temp = save_data.scores[i];
                save_data.scores[i] = save_data.scores[j];
                save_data.scores[j] = temp;
            }
        }
    }
}

int check_if_high_score(int new_score) {
    // High scores are sorted in descending order already
    if (new_score > save_data.scores[MAX_HIGHSCORES - 1].score) {
        return 1; // New score is a high score
    } else {
        return 0; // New score is not a high score
    }
}
