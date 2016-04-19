/* Tomasz Zakrzewski, tz336079  /
 * ZSO 2015/2016, raise - main */
#include <elf.h>
#include <stdlib.h>
#include <stdio.h>

#include "elfparser.h"

#define MAX_PHDRS 10000

int main(int argc, char *argv[]) {
	if (argc != 2) {
		puts("Error: Wrong argument number. Correct invocation:\n\traise <core_dump_file>");
		return 1;
	}
	
	Elf32_Ehdr elf_header;
	Elf32_Phdr elf_pheaders[MAX_PHDRS];
	parse_elf(argv[1], &elf_header, elf_pheaders);
	return 0;
}
