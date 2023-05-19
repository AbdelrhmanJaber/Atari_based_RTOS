/*
 * headball.h
 *
 *  Created on: May 11, 2023
 *      Author: Abdelrahman Ibrahim
 */

#ifndef HEADBALL_H_
#define HEADBALL_H_


/* start Head Ball resources and macros */

u8 headball_goal_keeping1[8] = {
		  0x1F,
		  0x10,
		  0x10,
		  0x10,
		  0x10,
		  0x10,
		  0x10,
		  0x10
		};

u8 headball_goal_keeping2[8] = {
		  0x10,
		  0x10,
		  0x10,
		  0x10,
		  0x10,
		  0x10,
		  0x10,
		  0x1F
		};

u8 headball_goal_keeping3[8] = {
		  0x1F,
		  0x01,
		  0x01,
		  0x01,
		  0x01,
		  0x01,
		  0x01,
		  0x01
		};

u8 headball_goal_keeping4[8] = {
		  0x01,
		  0x01,
		  0x01,
		  0x01,
		  0x01,
		  0x01,
		  0x01,
		  0x1F
		};


u8 headball_player[8] = {
		  0x04,
		  0x0A,
		  0x0A,
		  0x04,
		  0x0E,
		  0x04,
		  0x04,
		  0x0A
		};

u8 headball_palyer_with_ball[8] =  {
		  0x05,
		  0x0A,
		  0x0A,
		  0x04,
		  0x0E,
		  0x04,
		  0x04,
		  0x0A
		};

u8 headball_ball[8] = {
		  0x04,
		  0x00,
		  0x00,
		  0x00,
		  0x00,
		  0x00,
		  0x00,
		  0x00
		};

u8 x = 2 ;
u8 y = 2;
u8 prev_x = 2 , prev_y = 1 ;

u8 player_points = 0;
u8 automated_player_points = 0;

u8 headball_end_game = 0;

void headball_player_task(void * Copy_Pv);
void headball_automated_player_task(void * Copy_Pv);
void headball_ball_task(void * Copy_Pv);


void headball_init(void);
/*global struct for headball game*/

typedef struct myStruct3{
	u8 x;
	u8 y;
}headball;



#define HEADBALL_UP     '8'
#define HEADBALL_DOWN   '5'

#define BALL_UP           1
#define BALL_DOWN         2

#define PLAYER1           1
#define AUTO_PLAYER       2

headball player ;
headball automated_player;


/* end Head Ball resources and macros */



#endif /* HEADBALL_H_ */
