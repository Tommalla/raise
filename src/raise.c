/* Tomasz Zakrzewski, tz336079  /
 * ZSO 2015/2016, raise - main */
#include <stdio.h>

#include <asm/ldt.h>
#include <linux/unistd.h>
#include <sys/syscall.h>

#include "elfparser.h"

void revive() {
	syscall(SYS_set_thread_area, &USER_DESC);
	asm volatile ("pushl REFLAGS\n"
		      "popfl\n"
		      "movl REAX, %%eax\n"
		      "movl REBX, %%ebx\n"
		      "movl RECX, %%ecx\n"
		      "movl REDX, %%edx\n"
		      "movl RESP, %%esp\n"
		      "movl REBP, %%ebp\n"
		      "movl RESI, %%esi\n"
		      "movl REDI, %%edi\n"
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
