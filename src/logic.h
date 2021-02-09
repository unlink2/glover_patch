#ifndef __LOGIC_H__
#define __LOGIC_H__

#include "memory.h"
#include "utility.h"
#include "savefile.h"
#include "playerinfo.h"
#include "task.h"

#define MSG_TIME 10

#define TIMER_PER_FRAME 0.20

/**
 * Contains functions that manipulate the global state of the game
 * and update components of the romhack that do not require rendering.
 *
 * Also contains a global gpatch object that keeps track of global state
 */

// max slots should have all bits set for &ing
#define MAX_RESTORE_SLOTS 3

typedef struct gpatch_t {
    BOOLEAN infinite_hp;
    BOOLEAN infinite_lives;
    BOOLEAN lock_pos;
    BOOLEAN infinite_jump;
    u32 frame_advance;
    u8 menu_toggle; // TODO this is a hack to prevent a crash
    BOOLEAN cutscene_skip;
    BOOLEAN lockrng;
	BOOLEAN disable_pause;
    BOOLEAN clone_obj_bank; // clone obj bank instead of actor restore. disabled feature. not worth it
    BOOLEAN resume_restore; // was map loaded? resume restore of actors now
    u8 resume_timer; // run up until 60 to delay
    u16 restore_slot;

    // frame timer. replaces use of in-game timer
    // fixed point number
    u32 timer_frames;
    BOOLEAN enable_timer;
    BOOLEAN auto_timer; // auto start timer when fade ends
    BOOLEAN reset_now; // reset timer now!
    BOOLEAN use_igt; // use igt for auto timer
    char timer_str[64];

    char *message;
    u16 msg_timer;
    playerinfo pi;
} gpatch_t;

void init_gpatch(gpatch_t *gpatch);

extern gpatch_t gpatch;

/*
 * Set message
 */
void notify(gpatch_t *, const char*, u16);

void logic();

void update_timer(gpatch_t *);
void toggle_timer(gpatch_t *);

void enable_timer();
void level_select();

void frame_advance();

void complete_file(save_file *);

void store_glover_pos();
void restore_glover_pos();

/**
 * Clones a pointer, adds size and original location to dest
 * size must be divisible by 4
 * returns new dest ptr
 */
WORD_T* clone_additional(WORD_T *, WORD_T *, WORD_T);

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
void clone_actors(WORD_T *, u16 slot);

typedef struct obj_bank_t obj_bank_t;
/**
 * Clone other actors from object bank
 */
WORD_T* clone_other_objects(WORD_T *, obj_bank_t *);

// same as clone_actors but just dumps everything in obj_bank
void clone_obj_bank(WORD_T *, u16);

// reverses actor backup
// uses A0 as actor heap pointer
void restore_actors(WORD_T *, u16 slot);

void toggle_collision();
void toggle_fog();

void toggle_show_objects();

/**
 * forward definition of menu
 */
typedef struct menudef menudef;
void trigger_al(menudef *);

#endif
