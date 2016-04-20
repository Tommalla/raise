/* Tomasz Zakrzewski, tz336079  /
 * ZSO 2015/2016, raise - main */
#include <elf.h>
#include <stdio.h>

#include "elfparser.h"

#define MAX_PHDRS 10000

void revive() {
	printf("%lu\n", REAX);
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
	
	Elf32_Ehdr elf_header;
	Elf32_Phdr elf_pheaders[MAX_PHDRS];
	process_elf(argv[1], &elf_header, elf_pheaders);
	
	//revive();
	return 0;
}
