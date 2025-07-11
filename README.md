# SnakeBoy

A Gameboy port of the classic Nokia Snake game.

Written in C using the GBDK (Game Boy Developer Kit)

## TODO
1. Sprite graphics for main menu / logo - DONE, but maybe reconsider
1. Settings screen offering 3 (or 4) play modes
   * Easy (300ms game time)
   * Normal (150ms game time)
   * Adaptive (start at 150ms and slowly speed up as apples are eaten)
   * Brutal! (75ms from the start)
1. Maybe reconsider the apple sprite, it can be awkward to see on an original DMG screen, maybe keep it simple with a rounded black dot.
1. Debounce issue has been fixed, but this does give the oposite problem, if user input is very quick the snake can double back on itself before screen update
   * i.e, if the snake is travelling left and the user does a very quick input of down, then right, the snake can start travelling right before the down action is fully processed, resulting in instant death. This input does have to be VERY QUICK.

Buy some blank carts and make up some boxes (maybe print via AliExpress)