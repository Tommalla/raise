/* Tomasz Zakrzewski, tz336079  /
 * ZSO 2015/2016, raise - main */
#include <elf.h>
#include <stdio.h>

#include "elfparser.h"


int main(int argc, char *argv[]) {
	if (argc != 2) {
		puts("Error: Wrong argument number. Correct invocation:\n\traise <core_dump_file>");
		return 1;
	}
	
	Elf32_Ehdr *elf_header = NULL;
	Elf32_Shdr **elf_sheader = NULL;
	parse_elf(argv[1], elf_header, elf_sheader);
	return 0;
}
