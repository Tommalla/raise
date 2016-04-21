/* Tomasz Zakrzewski, tz336079  /
 * ZSO 2015/2016, raise - main */
#include <stdio.h>

#include <asm/ldt.h>
#include <linux/unistd.h>
#include <sys/syscall.h>

#include "elfparser.h"

void revive() {
	printf("Setting registers:\n"
	       "eax = %#08x\n"
	       "ebx = %#08x\n"
	       "ecx = %#08x\n"
	       "edx = %#08x\n"
	       "esp = %#08x\n"
	       "ebp = %#08x\n"
	       "esi = %#08x\n"
	       "edi = %#08x\n"
	       "eip = %#08x\n", REAX, REBX, RECX, REDX, RESP, REBP, RESI, REDI, REIP);
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
	
	process_elf(argv[1]);
	
	revive();
	return 0;
}
