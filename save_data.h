#ifndef _SAVE_DATA_H
#define _SAVE_DATA_H

#include <gb/gb.h>

// --- High Score Definitions ---
#define MAX_HIGHSCORES 10
#define NAME_LENGTH 3

// Structure for a single high score entry
typedef struct {
    char name[NAME_LENGTH];
    UINT16 score;
} HighScoreEntry;

// --- Save Data Structure ---
#define SAVE_DATA_BANK 0

typedef struct {
    // Game progress variables
    HighScoreEntry highscores[MAX_HIGHSCORES];
    UINT16 checksum;
} GameSaveData;

extern HighScoreEntry active_highscores[MAX_HIGHSCORES];

// Function prototypes
void init_save_data();
void save_game();
void load_game();
UINT16 calculate_checksum(GameSaveData *data);

// High score specific functions (new)
void add_new_highscore(UINT16 new_score, const char *new_name);
void sort_highscores(); // Helper to keep scores sorted
void display_highscores(); // For debugging/testing purposes

#endif