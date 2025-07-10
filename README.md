# SnakeBoy

A Gameboy port of the classic Nokia Snake game.

Written in C using the GBDK (Game Boy Developer Kit)

## TODO
1. Fix debounce issue, it's getting better but still causes a few issues.
   * This could also be done by pressing A to clear the end game screen, then START to restart the game.
1. High scores... Placeholder text is in place on the menu screen.
   * Will need to work out save states and save an array of 3 letter names and scores
   * At the end of a game, will need to check if the score is higher than the lowest high score and if so:
    * Prompt the user for the 3 letter name to use
    * Work out where in the save state the new higher score fits
    * Shift lower scores down 1 place (the last one will drop off the stack)
    * Insert the new high score into the correct position
1. Sprite graphics for main menu / logo
1. Sounds, subtle sound for change of direction, eat sound when eating an apple, game over "death" sound - Background music?
1. Settings screen offering 3 (or 4) play modes
   * Easy (300ms game time)
   * Normal (150ms game time)
   * Adaptive (start at 150ms and slowly speed up as apples are eaten)
   * Brutal! (75ms from the start)
1. Maybe reconsider the apple sprite, it can be awkward to see on an original DMG screen, maybe keep it simple with a rounded black dot.

Buy some blank carts and make up some boxes (maybe print via AliExpress)

BUG: If score is 100 or over, then the high score screen does not show up!!