################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/driver/driver.c \
../src/driver/drv_axi_switch.c \
../src/driver/drv_can.c \
../src/driver/drv_dma.c \
../src/driver/drv_fsync.c \
../src/driver/drv_gpio.c \
../src/driver/drv_iic.c \
../src/driver/drv_mb_intc.c \
../src/driver/drv_rec.c \
../src/driver/drv_sec_pulse.c \
../src/driver/drv_spi.c \
../src/driver/drv_spi_nbyte.c \
../src/driver/drv_timer.c \
../src/driver/drv_uart.c \
../src/driver/drv_uart_highspeed.c \
../src/driver/drv_user_ip.c 

OBJS += \
./src/driver/driver.o \
./src/driver/drv_axi_switch.o \
./src/driver/drv_can.o \
./src/driver/drv_dma.o \
./src/driver/drv_fsync.o \
./src/driver/drv_gpio.o \
./src/driver/drv_iic.o \
./src/driver/drv_mb_intc.o \
./src/driver/drv_rec.o \
./src/driver/drv_sec_pulse.o \
./src/driver/drv_spi.o \
./src/driver/drv_spi_nbyte.o \
./src/driver/drv_timer.o \
./src/driver/drv_uart.o \
./src/driver/drv_uart_highspeed.o \
./src/driver/drv_user_ip.o 

C_DEPS += \
./src/driver/driver.d \
./src/driver/drv_axi_switch.d \
./src/driver/drv_can.d \
./src/driver/drv_dma.d \
./src/driver/drv_fsync.d \
./src/driver/drv_gpio.d \
./src/driver/drv_iic.d \
./src/driver/drv_mb_intc.d \
./src/driver/drv_rec.d \
./src/driver/drv_sec_pulse.d \
./src/driver/drv_spi.d \
./src/driver/drv_spi_nbyte.d \
./src/driver/drv_timer.d \
./src/driver/drv_uart.d \
./src/driver/drv_uart_highspeed.d \
./src/driver/drv_user_ip.d 


# Each subdirectory must supply rules for building sources it contributes
src/driver/%.o: ../src/driver/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MicroBlaze gcc compiler'
	mb-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -I../../hw/CPU_microblaze_0/include -mlittle-endian -mcpu=v11.0 -mxl-soft-mul -Wl,--no-relax -ffunction-sections -fdata-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


