// kernel.c -- MatrixOS kernel

#include "monitor.h"
#include "descriptor_tables.h"
#include "timer.h"
#include "paging.h"
#include "multiboot.h"
#include "fs.h"
#include "initrd.h"
#include "task.h"
#include "syscall.h"

#include "input/keyboard.h"

#define VER_MAJOR		1
#define VER_MINOR		0
#define VER_FIX			4

extern u32int placement_address;
u32int initial_esp;
struct multiboot *mboot_ptr;

void init();

void sti() {
	asm volatile("sti");
}

void print_version();

int kernel_main(struct multiboot *mboot_point, u32int initial_stack)
{
	monitor_clear();
	initial_esp = initial_stack;
	mboot_ptr = mboot_point;
	monitor_write("#------------------------------------------------------------------------------#");
	monitor_write("|");
	monitor_set_fore_colour(10);
	monitor_set_cursor_pos(1, 3);
	monitor_write("Welcome to MatrixOS!!!");
	monitor_set_fore_colour(15);
	monitor_set_cursor_pos(1, 70);
	print_version();
	monitor_set_cursor_pos(1, 79);
	monitor_write("|");
	monitor_write("#------------------------------------------------------------------------------#");
	monitor_write("> ");

	init();

	sleep(2);

	print("Hello, user world!\n");

	for(;;) {}
}

void print_version() {
	if(VER_MINOR == 0 && VER_FIX == 0){
		monitor_set_back_colour(1);
	}
	if(VER_MINOR >= 1 && VER_FIX == 0) {
		monitor_set_back_colour(12);
	}
	if(VER_FIX >= 1) {
		monitor_set_back_colour(4);
	}
	
	monitor_write("v");
	monitor_set_fore_colour(10);//green
	monitor_write_dec(VER_MAJOR);
	monitor_write(".");
	monitor_set_fore_colour(14);//yellow
	monitor_write_dec(VER_MINOR);
	monitor_set_fore_colour(10);//green
	monitor_write(".");
	monitor_set_fore_colour(6);//red
	monitor_write_dec(VER_FIX);
	monitor_set_fore_colour(15);
	monitor_set_back_colour(0);
}

void init() {

	// Initialise all the ISRs and segmentation
	init_descriptor_tables();
	
	asm volatile("sti");
	initialise_syscalls();
	
	// Initialise the PIT to 100Hz
	asm volatile("sti");
	init_timer(50);

	// Find the location of our initial ramdisk.
	ASSERT(mboot_ptr->mods_count > 0);
	u32int initrd_location = *((u32int*)mboot_ptr->mods_addr);
	u32int initrd_end = *(u32int*)(mboot_ptr->mods_addr+4);
	// Don't trample our module with placement accesses, please!
	placement_address = initrd_end;

	// Start paging.
	initialise_paging();

	init_keyboard();

	// Initialise the initial ramdisk, and set it as the filesystem root.
	fs_root = initialise_initrd(initrd_location);
	
	// Start multitasking.
	//initialise_tasking();
	
	//switch_to_user_mode();

	//switch_to_user_mode();
	return;
}
