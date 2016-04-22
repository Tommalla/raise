/* Tomasz Zakrzewski, tz336079  /
 * ZSO 2015/2016, raise - main */
#include <signal.h>
#include <stdio.h>
#include <ucontext.h>

#include <asm/ldt.h>
#include <linux/unistd.h>
#include <sys/mman.h>
#include <sys/syscall.h>

#include "elfparser.h"

void revive(const char *path) {
	process_elf(path);
	syscall(SYS_set_thread_area, &USER_DESC);
	asm volatile ("pushl REFLAGS\n"
		      "popfl\n"
		      "pushl REBP\n"
		      "popl %%ebp\n"
		      "movl REAX, %%eax\n"
		      "movl RECX, %%ecx\n"
		      "movl REDX, %%edx\n"
		      "movl REBX, %%ebx\n"
		      "movl RESP, %%esp\n"
		      "movl RESI, %%esi\n"
		      "movl REDI, %%edi\n"
		      "mov RGS, %%gs\n"
		      "jmp *REIP"
		      :
		      :
		      : "eax", "ecx", "edx", "ebx", "esp", "esi", "edi", "cc");
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		puts("Error: Wrong argument number. Correct invocation:\n\traise <core_dump_file>");
		return 1;
	}
	
	ucontext_t context;
	getcontext(&context);
	context.uc_link = NULL;
	mmap((void *)0x08000000, SIGSTKSZ, PROT_READ | PROT_WRITE | PROT_EXEC, 
	     MAP_FIXED | MAP_PRIVATE | MAP_GROWSDOWN | MAP_ANON, -1, 0);
	context.uc_stack.ss_sp = (void *)0x08000000;
	context.uc_stack.ss_flags = 0;
	context.uc_stack.ss_size = SIGSTKSZ;
	makecontext(&context, revive, 1, argv[1]);
	setcontext(&context);
}
