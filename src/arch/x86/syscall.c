// MatrixOS - syscall.c
// System calls for interupts
#include "syscall.h"
#include "isr.h"
#include "common.h"

#include "monitor.h"
#include "timer.h"
#include "task.h"

static void syscall_handler ( registers_t *regs );

DEFN_SYSCALL1 ( monitor_write, 0, char* );
DEFN_SYSCALL1 ( monitor_write_hex, 1, u32int );
DEFN_SYSCALL1 ( monitor_write_dec, 2, u32int );
DEFN_SYSCALL1 ( sleep, 3, u32int );
DEFN_SYSCALL0 ( sti, 4 );
DEFN_SYSCALL1 ( mSleep, 5, u32int );
DEFN_SYSCALL0 ( exit , 6);

static void *syscalls[6] = {
	&monitor_write,
	&monitor_write_hex,
	&monitor_write_dec,
	&sleep,
	&sti,
	&mSleep,
};

u32int num_syscalls = 6;

/*
// Initialise syscalls by registereing the handler.
*/
s8int initialise_syscalls() {
	printf ( "Initalizing System Calls" );
	// Register our syscall handler.
	register_interrupt_handler ( 0x80, &syscall_handler );
	SYSCALL_ENABLED = 1;
	return 0;
}

/*
// Our syscall Handler. this is called whenever someone calls a syscall.
*/
void syscall_handler ( registers_t *regs ) {
	// Firstly, check if the requested syscall number is valid.
	// The syscall number is found in EAX.
	if ( regs->eax >= num_syscalls ) {
		return;
	}

	// Get the required syscall location.
	void *location = syscalls[regs->eax];

	// We don't know how many parameters the function wants, so we just
	// push them all onto the stack in the correct order. The function will
	// use all the parameters it wants, and we can pop them all back off afterwards.
	int ret;
	asm volatile ( " \
	  push %1; \
	  push %2; \
	  push %3; \
	  push %4; \
	  push %5; \
	  call *%6; \
	  pop %%ebx; \
	  pop %%ebx; \
	  pop %%ebx; \
	  pop %%ebx; \
	  pop %%ebx; \
	" : "=a" ( ret ) : "r" ( regs->edi ), "r" ( regs->esi ), "r" ( regs->edx ), "r" ( regs->ecx ), "r" ( regs->ebx ), "r" ( location ) );
	regs->eax = ret;
}
