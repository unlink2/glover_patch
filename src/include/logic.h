#ifndef __LOGIC_H__
#define __LOGIC_H__

#include "memory.h"


void logic();

void enable_timer();
void level_select();

void frame_advance();

void complete_file(WORD_T *);

void store_glover_pos();
void restore_glover_pos();


// finds all actors
// in the heap starting at glover
// all actors loop on each other and are stored in a linked list
// and memcpys them using the following format:
// actor backup format:
// 	4 bytes for original start address
//	4 bytes of actor size
// 	ACTOR_SIZE of data
//	List ends with a word of $00
//	at the end of the list the following helper values
//	are stored:
//		RNG - WORD
//		Camera Values - CAMERA_ACTOR_SIZE
// uses A1 as the pointer to the next free backup heap location
// each actor is ACTOR_SIZE bytes
// the backup heap is locates at the start of exp pack memory
void clone_actors();


// reverses actor backup
// uses A0 as actor heap pointer
void restore_actors();

void toggle_collision();
void toggle_fog();

#endif 
