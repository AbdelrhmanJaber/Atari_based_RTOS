/*
 * pacman.h
 *
 *  Created on: May 11, 2023
 *      Author: Kimo Store
 */

#ifndef PACMAN_H_
#define PACMAN_H_


/* start hangman resources and macros */

/*global enum for pacman game direction*/

typedef enum myEnum2{
	pacman_direction_right,
	pacman_direction_left,
	pacman_direction_up,
	pacman_direction_down
}pacman_direction;


/*global struct for pacman game direction*/

typedef struct myStruct2{
	u8 x;
	u8 y;
	pacman_direction direction;
}pacman;

u8 pacman_player[8] = {
		  0x00,
		  0x00,
		  0x0E,
		  0x1B,
		  0x1C,
		  0x0E,
		  0x00,
		  0x00
		};

u8 pacman_monster[8] = {
		  0x00,
		  0x0E,
		  0x15,
		  0x1F,
		  0x1F,
		  0x1F,
		  0x1F,
		  0x15
		};

pacman global_pacman ;
pacman global_pacman_moster;

xQueueHandle pacman_buffer;
xSemaphoreHandle pacman_sema;

u8 pacman_end_game;
void pacman_init(void);

void pacman_input_task(void * Copy_Pv);
void pacman_eating_output_task(void * Copy_Pv);
void pacman_monster_task(void* Copy_Pv);

/* end hangman resources and macros */


#endif /* PACMAN_H_ */
