/*
 * atari.c
 * Author : Abdelrahman Ibrahim
 *
 * */


#include<stdlib.h>
#include"Bit_Math.h"
#include"std_types.h"
#define F_CPU  8000000UL
#include<avr/delay.h>

/*MCAL*/
#include"MDIO_interface.h"
#include"timer.h"

/*HAL*/
#include"HLCD_interface.h"
#include"HKPD_interface.h"

/*RTOS*/
#include"FreeRTOS.h"
#include"task.h"
#include"semphr.h"

/*library of games*/

/*library of snake game*/
#include"snake.h"

/*library of pacman game*/
#include"pacman.h"

/*library of headball game*/
#include"headball.h"

#define SNAKE          '7'
#define HEADBALL       '9'
#define PACMAN         '8'



/*global array for the lcd*/
u8 lcd[2][16];

u8 Get_Pressed(void);


int main(void){
	MDIO_u8INIT();
	HLCD_voidINIT();
	timer0_init();
	u8 option;

	while(1){
	  HLCD_u8SetCursor(HLCD_u8_LINE_1  , HLCD_u8_Position_0);
	  HLCD_u8SendString("1-Snake 2-Pacman",0);
	  HLCD_u8SetCursor(HLCD_u8_LINE_2 , HLCD_u8_Position_0);
	  HLCD_u8SendString("3-Head Ball " , 0);

	  /* get the option from the player */

	  option = Get_Pressed();
	  HLCD_u8ClearDisplay();

	  /*check which game the player wants and start it*/

	  if(option == SNAKE){

	  /*allocate the tcb of the task 1 for context switching issues*/
	  xTaskCreate(set_snake_direction_input_task , STD_TYPES_NULL , 200  , STD_TYPES_NULL ,0, STD_TYPES_NULL);

	  /*allocate the tcb of the task 2 for context switching issues*/
	  xTaskCreate(move_snake_output_task , STD_TYPES_NULL , 600  , STD_TYPES_NULL ,0, STD_TYPES_NULL);

	  /*start the game*/
	  vTaskStartScheduler();

	  }else if(option == PACMAN){

		  /*create queue for communication between tasks*/
		  pacman_buffer = xQueueCreate(4 , sizeof(u8));

		  /*create semaphore to save lcd resource from mutual exclusion*/
		  vSemaphoreCreateBinary(pacman_sema);

		  /*allocate the tcb of the task 1 for context switching issues*/
		  xTaskCreate(pacman_input_task , STD_TYPES_NULL , 200  , STD_TYPES_NULL ,2, STD_TYPES_NULL);

		  /*allocate the tcb of the task 2 for context switching issues*/
		  xTaskCreate(pacman_eating_output_task , STD_TYPES_NULL , 350  , STD_TYPES_NULL ,3, STD_TYPES_NULL);

		  /*allocate the tcb of the task 3 for context switching issues*/
		  xTaskCreate(pacman_monster_task , STD_TYPES_NULL , 250  , STD_TYPES_NULL ,1, STD_TYPES_NULL);

		  /*start the game*/
		  vTaskStartScheduler();
	  }else if(option == HEADBALL){

		  /*allocate the tcb of the task 1 for context switching issues*/
		  xTaskCreate(headball_player_task , STD_TYPES_NULL , 300  , STD_TYPES_NULL , 3 , STD_TYPES_NULL);

		  /*allocate the tcb of the task 2 for context switching issues*/
		  xTaskCreate(headball_automated_player_task , STD_TYPES_NULL , 300  , STD_TYPES_NULL , 3 , STD_TYPES_NULL);

		  /*allocate the tcb of the task 3 for context switching issues*/
		  xTaskCreate(headball_ball_task , STD_TYPES_NULL , 400  , STD_TYPES_NULL , 4 , STD_TYPES_NULL);

		  /*start the game*/
		  vTaskStartScheduler();
	  }
	}
	return 0;
}


/***
 *
 *
 * start snake game tasks
 *
 *
 *  ***/

void set_snake_direction_input_task(void * Copy_Pv){
	u8 direction;
	while(1){
		/*get the move from the player*/
		direction = Get_Pressed();
		/*set the global enum of direction*/
			if(direction == RIGHT){
				if(global_direction != snake_direction_left) global_direction = snake_direction_right;
			}else if(direction == LEFT){
				if(global_direction != snake_direction_right) global_direction = snake_direction_left;
			}else if(direction == UP){
				if(global_direction != snake_direction_down) global_direction = snake_direction_up;
			}else if(direction == DOWN){
				if(global_direction != snake_direction_up) global_direction = snake_direction_down;
			}
			/*check if the player lost or not*/
			if(endSnakeFlag) break;
		vTaskDelay(50);
  }
}

void move_snake_output_task(void * Copy_Pv){
	/* at the beginning we allocate the snake at the start point */
	for(u8 i = 0 ; i < global_snake_length ; i++){
	   global_snake[i].x = 1;
	   global_snake[i].y = global_snake_length-1-i;
	   global_snake[i].direction = global_direction;
	}

	while(1){
	/*update the direction for all snake points
	 * to make each point follow the next of it*/
	for(int i = global_snake_length-1 ; i > 0 ; i--){
		global_snake[i].direction = global_snake[i-1].direction;
	}

	global_snake[0].direction = global_direction;
	/*update the tail of the snake for the growth*/
	snake_tail = global_snake[global_snake_length - 1];

	/*update each point coordinate */
	for(u8 i = 0 ; i < global_snake_length ; i++){
		switch(global_snake[i].direction){
			case snake_direction_right:{
				global_snake[i].y++;
				global_snake[i].y %= 16;
				break;
			}
			case snake_direction_left:{
				if(global_snake[i].y == 0){
					global_snake[i].y = MAX_SNAKE_LENGHT;
				}else{
					global_snake[i].y--;
				}
				break;
			}
			case snake_direction_up:{
				if(global_snake[i].x == 1) global_snake[i].x = 2;
				else if(global_snake[i].x == 2) global_snake[i].x = 1;
				break;
			}
			case snake_direction_down:{
				if(global_snake[i].x == 1) global_snake[i].x = 2;
				else if(global_snake[i].x == 2) global_snake[i].x = 1;
				break;
			}
		}
		/*update lcd global array to not make food overwrite the snake */
		lcd[global_snake[i].x -1][global_snake[i].y] = 1;
	}

	/*check there is an apple on the lcd or not*/
	if(snake_food.x == EMPTY || snake_food.y == EMPTY){
		/*create new apple for the snake*/
		snake_food.y = generateRandomNumbers(16);
		snake_food.x = generateRandomNumbers(2) + 1 ;
		/*make sure to not overwrite the snake*/
		while(lcd[snake_food.x][snake_food.y] != 0){
			snake_food.y = generateRandomNumbers(16);
			snake_food.x = generateRandomNumbers(2) + 1 ;
		}
		HLCD_u8SetCursor(snake_food.x , snake_food.y );
		HLCD_u8SendChar('X');
	}

	/*make all element of lcd array = 0 for new position */

	for(u8 i = 0 ; i < 2 ;  i++){
		for(u8 j = 0 ; j < 16 ; j++){
			lcd[i][j] = 0;
		}
	}

	static u8 foodFlag = 0;
	/*display the snake*/
	for(u8 i = 0 ; i < global_snake_length ; i++){
		HLCD_u8GenerateSpecialCharacter(snake , 0);
		HLCD_u8DisplaySpecialCharacter(0 , global_snake[i].x , global_snake[i].y);
		/*check if the snake ate the apple or not*/
		if(global_snake[global_snake_length - 1].x == snake_food.x  && global_snake[global_snake_length -1].y == snake_food.y){
			foodFlag = 1;
		}
	}
		HLCD_u8SetCursor(snake_tail.x , snake_tail.y);
		HLCD_u8SendChar(' ');

		/*update snake after eating the apple*/

		if(foodFlag){
			foodFlag = 0;
			global_snake_length++;
			snakeScore++;
			global_snake[global_snake_length - 1] = snake_tail;
			snake_food.x = EMPTY;
			snake_food.y = EMPTY;
		}

		/* check game over*/
		u8 end_game = 0;
	for(u8 i = 0 ; i < global_snake_length ; i++){
		for(u8 j = 0 ; j < global_snake_length ; j++){
			if(i != j){
			  if(global_snake[i].x == global_snake[j].x && global_snake[i].y == global_snake[j].y){
				  end_game = 1;
			  }
			}
		}
	}

	/*check losing*/

	if(end_game){
		/*game over*/
		endSnakeFlag = 1;
		HLCD_u8ClearDisplay();
		HLCD_u8SetCursor(HLCD_u8_LINE_1 , HLCD_u8_Position_4);
		HLCD_u8SendString("Game over",0);
		HLCD_u8SetCursor(HLCD_u8_LINE_2 , HLCD_u8_Position_4);
		HLCD_u8SendString("score : ",0);
		HLCD_u8SendNumber(snakeScore);
		_delay_ms(2000);
		global_snake_length = 3;
		break;
	}
	vTaskDelay(300);

  }
}

/***
 *
 *
 * end snake game tasks
 *
 *
 *  ***/


/***
 *
 *
 *  start hangman game tasks
 *
 *
 *  ***/

void pacman_input_task(void * Copy_Pv){
	/* get input from the player and send it using queue buffer*/
	while(1){
		u8 move = Get_Pressed();

		/*send the move using queue message to pacman_eating_output_task */
		xQueueSend(pacman_buffer , &move , portMAX_DELAY);
		if(pacman_end_game){
			pacman_end_game = 0;
			break;
		}
		vTaskDelay(100);
	}
}

void pacman_eating_output_task(void * Copy_Pv){
 /*init the start position of the game*/

	pacman_init();

	while(1){
		u8 move;

		/*get move from pacman_input_task*/
		xQueueReceive(pacman_buffer , &move , portMAX_DELAY);

		/*start position*/
		HLCD_u8SetCursor(global_pacman.x , global_pacman.y);
		HLCD_u8SendChar(' ');

		/*update pacman direction according to recieved move*/

		switch(move){
			case RIGHT :{
			  if(global_pacman.y < 15 ) global_pacman.y++;
			  break;
			}
			case LEFT :{
				if(global_pacman.y != 0) global_pacman.y--;
				break;
			}
			case UP :{
				if(global_pacman.x == HLCD_u8_LINE_2) global_pacman.x = HLCD_u8_LINE_1;
				else global_pacman.x = HLCD_u8_LINE_1;
				break;
			}
			case DOWN:{
				if(global_pacman.x == HLCD_u8_LINE_1) global_pacman.x = HLCD_u8_LINE_2;
				else global_pacman.x = HLCD_u8_LINE_2;
				break;
			}
		}

		/*display the pacman position*/
		HLCD_u8GenerateSpecialCharacter(pacman_player , 0);
		HLCD_u8DisplaySpecialCharacter(0 , global_pacman.x , global_pacman.y);

		/*update the lcd array if pacman eat the food*/
		xSemaphoreTake(pacman_sema , portMAX_DELAY);
		if(lcd[global_pacman.x -1][global_pacman.y] == 1) lcd[global_pacman.x -1][global_pacman.y] = 0;
		xSemaphoreGive(pacman_sema);

		/* check if pacman ate all food*/
		u8 not_yet = 0;
		for(u8 i = 0 ; i < 2 ; i++){
			for(u8 j = 0 ; j < 16 ; j++){
				if(lcd[i][j] == 1) not_yet = 1;
			}
		}

		/*check winning*/
		if(!not_yet){
			pacman_end_game = 1;
			HLCD_u8ClearDisplay();
			HLCD_u8SetCursor(1,5);
			HLCD_u8SendString("you won",0);
			_delay_ms(2000);
		}

		/*check losing*/
		if(global_pacman.x == global_pacman_moster.x && global_pacman.y == global_pacman_moster.y){
			pacman_end_game = 1;
			HLCD_u8ClearDisplay();
			HLCD_u8SetCursor(1,4);
			HLCD_u8SendString("you lost",0);
			_delay_ms(2000);
			break;
		}
		vTaskDelay(100);
	}
}


void pacman_monster_task(void* Copy_Pv){

   /*start position of pacman monster*/
	global_pacman_moster.x = HLCD_u8_LINE_1;
	global_pacman_moster.y = HLCD_u8_Position_15;
	global_pacman_moster.direction = pacman_direction_left;

	/*display pacman monster at the beginning*/
	HLCD_u8GenerateSpecialCharacter(pacman_monster , 8);
	HLCD_u8DisplaySpecialCharacter(1 , global_pacman_moster.x , global_pacman_moster.y);

	u8 dir_x;
	u8 dir_y;
	while(1){

		static u8 food_x , food_y , monster_overwrite_food = 0;

		/*get random move for pacman monster*/
		dir_x = generateRandomNumbers(2) + 1 ;
		dir_y = generateRandomNumbers(3) ;

		/*clear the previous position */
		HLCD_u8SetCursor(global_pacman_moster.x , global_pacman_moster.y);
		HLCD_u8SendChar(' ');

		/*update pacman monster position according to random move
		 * in such way to prevent stuck in certain position
		 * */

		global_pacman_moster.x = dir_x;
		if(dir_y == 1){
			if(global_pacman_moster.y != 0 ){
				static u8 flag = 0;
				if(!flag){
					flag = 1;
					global_pacman_moster.y--;
				}
				else{
					flag = 0;
					global_pacman_moster.y -= 2;
				}
			}
		}else if(dir_y == 2){
			if(global_pacman_moster.y < 15 ){
				static u8 flag = 0;
				if(!flag){
					flag = 1;
					global_pacman_moster.y++;
				}
				else{
					flag = 0;
					global_pacman_moster.y += 2;
				}
			}
		}
		/*display the new position of pacman monster*/
		HLCD_u8GenerateSpecialCharacter(pacman_monster , 8);
		HLCD_u8DisplaySpecialCharacter(1 , global_pacman_moster.x , global_pacman_moster.y);

		/*check if pacman monster is at the same position of food and handle it*/
		if(monster_overwrite_food){
			HLCD_u8SetCursor(food_x , food_y);
			HLCD_u8SendChar('x');
			food_x = 0 ;
			food_y = 0 ;
			monster_overwrite_food = 0;
		}

		if(lcd[global_pacman_moster.x -1][global_pacman_moster.y] == 1) {
			food_x =  global_pacman_moster.x;
			food_y = global_pacman_moster.y ;
			monster_overwrite_food = 1;
		}

		/*check if game over to break*/
		if(pacman_end_game){
			pacman_end_game = 0;
			break;
		}
		vTaskDelay(300);
	}
}


void pacman_init(void){
	/*display the pacman at the start position*/

	global_pacman.x = HLCD_u8_LINE_1;
	global_pacman.y = 0;
	global_pacman.direction = pacman_direction_right;
	HLCD_u8GenerateSpecialCharacter(pacman_player , 0);
	HLCD_u8DisplaySpecialCharacter(0 , global_pacman.x , global_pacman.y);

	/*print the food of the pacman*/
	for(u8 i = 1 ; i <= 2 ; i++){
		for(u8 j = 0 ; j < 16 ; j++){
				if((i == 1 && j == 0) || (i == 1 && j == 15)) continue;
				else{
					HLCD_u8SetCursor(i , j);
					HLCD_u8SendChar('x');
					lcd[i-1][j] = 1;
				}
			}
		}
}


/*** end hangman game tasks and functions ***/



/*** start Head Ball game tasks ***/

void headball_player_task(void * Copy_Pv){

	while(1){
		u8 move;
		/*get the move from player up or down*/
		move = Get_Pressed();

		/*check the move and update the new position*/
		if(move == HEADBALL_UP){
			 HLCD_u8SetCursor(HLCD_u8_LINE_2 , HLCD_u8_Position_1);
			 HLCD_u8SendChar(' ');
			 HLCD_u8DisplaySpecialCharacter(4 , HLCD_u8_LINE_1 , HLCD_u8_Position_1);
			 player.x = HLCD_u8_LINE_1;
			 player.y = HLCD_u8_Position_1;
		}else if(move == HEADBALL_DOWN){
			HLCD_u8SetCursor(HLCD_u8_LINE_1 , HLCD_u8_Position_1);
			HLCD_u8SendChar(' ');
			HLCD_u8DisplaySpecialCharacter(4 , HLCD_u8_LINE_2 , HLCD_u8_Position_1);
			 player.x = HLCD_u8_LINE_2;
			 player.y = HLCD_u8_Position_1;
		}
		/*check if game over to break*/
		if(headball_end_game) break;
		vTaskDelay(100);
	}
}


void headball_automated_player_task(void * Copy_Pv){
	while(1){
		u8 move;
		static u8 flag1 = 0 , flag2 = 0;

		/*get random move
		 *
		 * it's implemented in such a way to prevent stuck in a certain position */
		move = generateRandomNumbers(4);
		if(move != -1){

			/* flag used to prevent stuck if timer generate same number by chance alot */

			if(flag1 >= 3){
				flag1 = 0;
				move = 0;
			}
			if(flag2 >= 3){
				flag2 = 0;
				move = 1;
			}
		  if(move == 0 || move == 2 ){

			  /*clear the old position*/
			  HLCD_u8SetCursor(HLCD_u8_LINE_1 , HLCD_u8_Position_14);
			  HLCD_u8SendChar(' ');

			  /*update and display the new position*/
			  HLCD_u8DisplaySpecialCharacter(4 , HLCD_u8_LINE_2 , HLCD_u8_Position_14 );
			  automated_player.x = HLCD_u8_LINE_2;
			  automated_player.y = HLCD_u8_Position_14;
			  flag2++;

		  }else if(move == 1 || move == 3){
			  /*clear the old position*/
			  HLCD_u8SetCursor(HLCD_u8_LINE_2 , HLCD_u8_Position_14);
			  HLCD_u8SendChar(' ');

			  /*update and display the new position*/
			  HLCD_u8DisplaySpecialCharacter(4 , HLCD_u8_LINE_1 , HLCD_u8_Position_14 );
			  automated_player.x = HLCD_u8_LINE_1;
			  automated_player.y = HLCD_u8_Position_14;
			  flag1++;
		  }
		}

		/*check if game over to break*/
		if(headball_end_game) break;
		vTaskDelay(250);
	}
}




void headball_ball_task(void * Copy_Pv){

	/*init the headball game and the start position */
	 headball_init();

	while(1){

		/*flag to check if the player touch the ball or not*/
    static u8 flag = 0;

	if(automated_player.x == x && automated_player.y == y){
		/*if automated player touch the ball
		 * update the new position of ball*/
		flag = 1;
		y = 13;
		prev_y = 14;
	}else if(player.x == x && player.y == y){
		/*if automated player didn't touch the ball
		 * player 1 will get the ball again*/
		y = 2;
		prev_y = 1;
		flag = 0;
	}
	if(flag == 0){
		/*update the position of the ball every time when the task
		 * be in the running state*/
		y++;
        prev_y++;

        /*check the ball enter the goal or the other player touched it*/
        if(y == 15){
        	if(automated_player.x == x && automated_player.y == y){

        		/*update the position of ball*/
        		flag = 1;
        		y = 13;
        		prev_y = 14;
        		if(automated_player.x == HLCD_u8_LINE_1) x = HLCD_u8_LINE_2;
        		else x = HLCD_u8_LINE_1;
        	}else{

        		/*ball entered the goal player1 points should increase by one*/
        		player_points++;
        		 y = 2;
        		 prev_y = 1;
        		 x = player.x;
        	}
        	prev_x = x;
        }
	 if(prev_y > 1){
		 /*remove the old position of ball*/
	       HLCD_u8SetCursor(prev_x , prev_y);
		   HLCD_u8SendChar(' ');
	   }
	}else{

		/*now ball in the opposite direction from the automated player to player1 */
		/*update the position of the ball*/
		y--;
		prev_y--;

		/*remove the old position of the ball*/
		if(prev_y < 14){
			HLCD_u8SetCursor(prev_x , prev_y);
			HLCD_u8SendChar(' ');
		}

		if(y == 1){

			/*check the ball enter the goal or the other player touched it*/

		if(player.x == x && player.y == y){
			/*player1 touched the ball and get it*/
			y = 2;
			prev_y = 1;
			flag = 0;
			if(player.x == HLCD_u8_LINE_1) x = HLCD_u8_LINE_2;
			else x = HLCD_u8_LINE_1;
		}else{
			/*the ball entered the goal so automated player should increment by one*/
			automated_player_points++;
			 y = 13;
			 prev_y = 14;
			 x = automated_player.x;
		}
		prev_x = x;
}
	}

	/*display the new position of the ball*/
	HLCD_u8GenerateSpecialCharacter(headball_ball , 56);
	HLCD_u8DisplaySpecialCharacter(7 , x , y);

	/*check the winner*/
	if(automated_player_points >=5 || player_points >= 5){
		HLCD_u8ClearDisplay();
		HLCD_u8SetCursor(HLCD_u8_LINE_1 , HLCD_u8_Position_0);
		HLCD_u8SendString("player1 : " , 0);
		HLCD_u8SendNumber(player_points);
		HLCD_u8SetCursor(HLCD_u8_LINE_2 , HLCD_u8_Position_0);
		HLCD_u8SendString("player2 : " , 0);
		HLCD_u8SendNumber(automated_player_points);
		headball_end_game = 1;
		_delay_ms(2000);
		break;
	}
	vTaskDelay(100);
	}
}


void headball_init(void){

	/*the goal of the player */
	HLCD_u8GenerateSpecialCharacter(headball_goal_keeping1 , 0);
	HLCD_u8DisplaySpecialCharacter(0,HLCD_u8_LINE_1 , HLCD_u8_Position_0);
	HLCD_u8GenerateSpecialCharacter(headball_goal_keeping2 , 8);
	HLCD_u8DisplaySpecialCharacter(1 , HLCD_u8_LINE_2 , HLCD_u8_Position_0);

	/*the goal of automated player*/
	HLCD_u8GenerateSpecialCharacter(headball_goal_keeping3 , 16);
	HLCD_u8DisplaySpecialCharacter(2 , HLCD_u8_LINE_1 , HLCD_u8_Position_15);
	HLCD_u8GenerateSpecialCharacter(headball_goal_keeping4 , 24);
	HLCD_u8DisplaySpecialCharacter(3 , HLCD_u8_LINE_2 , HLCD_u8_Position_15);

	/* display player 1 with ball*/
	HLCD_u8GenerateSpecialCharacter(headball_palyer_with_ball , 32);
	HLCD_u8DisplaySpecialCharacter(4 , HLCD_u8_LINE_2 , HLCD_u8_Position_1);
	player.x = HLCD_u8_LINE_2;
	player.y = HLCD_u8_Position_1;

	/* display automated player without ball */
	HLCD_u8GenerateSpecialCharacter(headball_player , 40);
	HLCD_u8DisplaySpecialCharacter(5 , HLCD_u8_LINE_2 , HLCD_u8_Position_14);
	automated_player.x = HLCD_u8_LINE_2;
	automated_player.y = HLCD_u8_Position_14;

	_delay_ms(500);
	/* display player 1 with ball*/
	HLCD_u8GenerateSpecialCharacter(headball_player , 32);
	HLCD_u8DisplaySpecialCharacter(4 , HLCD_u8_LINE_2 , HLCD_u8_Position_1);
}

/*** end Head Ball game tasks ***/


/* function/s used in all games */
u8 Get_Pressed(void){
	u8 returnedValue;
	while(1){
		/*Get user input */
	  HKPD_u8GetKey(&returnedValue);
	  if(returnedValue != HKPD_u8_NO_KEY_PRESSED){
		  break;
	  }
	}
	  return returnedValue;
}

