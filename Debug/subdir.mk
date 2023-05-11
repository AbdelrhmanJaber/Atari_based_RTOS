################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HKPD_programm.c \
../HLCD_programm.c \
../MDIO_programm.c \
../atari.c \
../croutine.c \
../heap_1.c \
../list.c \
../port.c \
../queue.c \
../tasks.c \
../timer.c \
../timers.c 

OBJS += \
./HKPD_programm.o \
./HLCD_programm.o \
./MDIO_programm.o \
./atari.o \
./croutine.o \
./heap_1.o \
./list.o \
./port.o \
./queue.o \
./tasks.o \
./timer.o \
./timers.o 

C_DEPS += \
./HKPD_programm.d \
./HLCD_programm.d \
./MDIO_programm.d \
./atari.d \
./croutine.d \
./heap_1.d \
./list.d \
./port.d \
./queue.d \
./tasks.d \
./timer.d \
./timers.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


