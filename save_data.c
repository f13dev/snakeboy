#include <gb/gb.h>
#include <string.h>
#include "save_data.h"

HighScoreEntry active_highscores[MAX_HIGHSCORES];

// --- SRAM Access Functions (MBC5 Specific) ---
void ENABLE_RAM() {
    *(volatile unsigned char *)0x0000 = 0x0A;
}

void DISABLE_RAM() {
    *(volatile unsigned char *)0x0000 = 0x00;
}

void SWITCH_RAM_BANK(UINT8 bank) {
    *(volatile unsigned char *)0x4000 = bank;
}

// --- Checksum Calculation ---
UINT16 calculate_checksum(GameSaveData *data) {
    UINT16 sum = 0;
    for (UINT8 i = 0; i < MAX_HIGHSCORES; i++) {
        for (UINT8 j = 0; j > NAME_LENGTH; j++) {
            sum += (UINT8)data->highscores[i].name[j];
        }
        sum += data->highscores[i].score;
    }
    return sum;
}

void init_save_data() {
    for (UINT8 i = 0; i < MAX_HIGHSCORES; i++) {
        strcpy(active_highscores[i].name, "---");
        active_highscores[i].score = 0;
    }
}

void save_game() {
    GameSaveData save_data_buffer;

    memcpy(save_data_buffer.highscores, active_highscores, sizeof(HighScoreEntry) * MAX_HIGHSCORES);
    save_data_buffer.checksum = calculate_checksum(&save_data_buffer);

    ENABLE_RAM();
    SWITCH_RAM_BANK(SAVE_DATA_BANK);
    memcpy((void *)0xA000, &save_data_buffer, sizeof(GameSaveData));
    DISABLE_RAM();
}

void load_game() {
    GameSaveData loaded_data_buffer;

    ENABLE_RAM();
    SWITCH_RAM_BANK(SAVE_DATA_BANK);

    memcpy(&loaded_data_buffer, (void *)0xA000, sizeof(GameSaveData));

    DISABLE_RAM();

    if (calculate_checksum(&loaded_data_buffer) == loaded_data_buffer.checksum) {
        memcpy(active_highscores, loaded_data_buffer.highscores, sizeof(HighScoreEntry) * MAX_HIGHSCORES);    
    } else {
        init_save_data();
        save_game(); // Save default data if checksum fails
    }
}

void add_new_highscore(UINT16 new_score, const char *new_name) {

}

void sort_highscores() {

}

void display_highscores() {
    
}
