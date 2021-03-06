#ifndef I8042_H
#define I8042_H

#define I8042_COMMAND_REG	0x64
#define I8042_STATUS_REG	0x64
#define I8042_DATA_REG		0x60

#define I8042_KBD_IRQ	1
#define I8042_AUX_IRQ	12

/*
 *Time out for talking to a non existant mouse
 */
#define I8042_CTL_TIMEOUT	10000

/*
 * Return codes.
 */
#define I8042_RET_CTL_TEST	0x55

/*
 * Expected maximum internal i8042 buffer size. This is used for flushing
 * the i8042 buffers.
 */
#define I8042_BUFFER_SIZE	16

u8int i8042_Init();

void i8042_Caps(u8int Caps, u8int Scroll, u8int Num);

u8int i8042_disable_devices();				//1
u8int i8042_flush_output_buffer();			//2
u8int i8042_set_controller_config_byte();	//3
u8int i8042_controller_self_test();		//4
u8int i8042_two_channels();				//5
u8int i8042_interface_test();				//6
u8int i8042_enable_devices();				//7
u8int i8042_reset_devices();				//8
u8int i8042_lable_devices();				//9

#endif //I8042_H
