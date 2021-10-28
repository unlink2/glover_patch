---
layout: default
title: Afterlife
---

## Afterlife

Afterlife is a glitch that occured by complete accident on mcill's stream causing him to game over, but then getting respawned with "0" lives after.
So what actually happened?

Afterlife is as caused by a branch at 0x801256FC that checks if the fadeout timer (0x801E753D) at a certain value.
If that check is interrupted by pressing A a few frames (note: exact timing is unknown, help by tasing it! - I counted around 5-6 seconds from game over first appearning on screen) before the fade actually happens the game will respawn you with -1 lives (an underflow) and in a game over state.
Interestingly enough, once you beat your current level the game will return you to the main menu.

If a level is beaten in this state however the game will not flag the level as completed, however if a boss is beaten a ball will be awarded to the player while the level remains unbeaten allowing the player to obtain more than 1 ball from a boss.

If the fadeout timer is interrupted at the wrong time, the game might still return glover to the main menu but remain in a gameover state. Upon entering a file and entering an entrance the player will be transported to the map the game over occured on!

What seems to be happening is:

- The game usually resets the last entrance (0x801E7541) upon entering a loading zone, returning to castle or being out of lives
- The game also stores the current world id (0x801E7533)
- When the game is interrupted in the way described above it will load the last entrance id and world id combination instead of the menu
- Entering a file will not reset the entrance id!
- Entering any entrance will reset it
- Returning to castle will reset it
- The map id and world id combination seemingly decides which code gets run upon leaving a map (e.g. part of switch at 0x80123D10 which awards a ball)
- The pause menu will not work in an afterlife state
