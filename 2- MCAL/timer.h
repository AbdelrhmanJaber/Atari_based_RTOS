#ifndef TIMER_H_
#define TIMER_H_


#define MTMR_u8_TCCR0_REG            *((volatile u8*)0x53)

#define MTMR_u8_TCNT0_REG            *((volatile u8*)0x52)

void timer0_init(void);

u8 generateRandomNumbers(u8 mod);

#endif /* TIMER_H_ */
