#include"std_types.h"
#include"Bit_Math.h"
#include"timer.h"


void timer0_init(void){
		CLR_BIT(MTMR_u8_TCCR0_REG , 6);
		CLR_BIT(MTMR_u8_TCCR0_REG , 3);
		CLR_BIT(MTMR_u8_TCCR0_REG , 2);
		SET_BIT(MTMR_u8_TCCR0_REG , 1);
		CLR_BIT(MTMR_u8_TCCR0_REG , 0);
}
u8 generateRandomNumbers(u8 mod){
   u8 random;
   random = (MTMR_u8_TCNT0_REG % mod);
	return random;
}


