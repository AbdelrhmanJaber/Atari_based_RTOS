/*
 * snake.h
 *
 *  Created on: May 11, 2023
 *      Author: Kimo Store
 */

#ifndef SNAKE_H_
#define SNAKE_H_


/* start snake game resources and macros*/


/*Macro for snake length*/
#define MAX_SNAKE_LENGHT   15

/*Macros of direction*/

#define RIGHT    '6'
#define LEFT     '4'
#define UP       '8'
#define DOWN     '5'

#define EMPTY    255

/*global enum for directions of snakes*/
typedef enum myEnum{
	snake_direction_right,
	snake_direction_left,
	snake_direction_up,
	snake_direction_down
}snake_direction;


/*
 * struct for each point of snake
 * each point has coordinates x and y
 * and enum of direction
 * */

typedef struct myStruct{
	u8 x;
	u8 y;
	snake_direction direction;
}snake_point;

/*array of struct for snake
 * each point represented by a struct*/
snake_point global_snake[MAX_SNAKE_LENGHT];

/*global enum to determine the direction*/
snake_direction global_direction = snake_direction_right;

/*struct of the tail of the snake*/
snake_point snake_tail;

/*struct of the eat*/
snake_point snake_food = {.x = EMPTY , .y= EMPTY};

u8 global_snake_length = 3;

u8 snake[8] = {
		  0x1F,
		  0x1F,
		  0x11,
		  0x11,
		  0x11,
		  0x11,
		  0x1F,
		  0x1F
		};

void set_snake_direction_input_task(void * Copy_Pv);
void move_snake_output_task(void * Copy_Pv);

u8 endSnakeFlag = 0;
u8 snakeScore = 0;

/* end snake game resources and macros*/


#endif /* SNAKE_H_ */
