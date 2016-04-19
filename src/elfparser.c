/* Tomasz Zakrzewski, tz336079        /
 * ZSO 2015/2016, raise - ELF parser */
#include <stdlib.h>
#include <stdio.h>

#include "elfparser.h"

void parse_elf(char *path, Elf32_Ehdr *hdr, Elf32_Phdr *phdrs) {
	int i;
	FILE *f = fopen(path, "r");
	fread(hdr, sizeof(Elf32_Ehdr), 1, f);
	
	printf("Read the file header!\n");
	
	// Move to the beginning of program segment headers
	fseek(f, hdr->e_phoff, SEEK_SET);
	
	if (hdr->e_phentsize != sizeof(Elf32_Phdr)) {
		printf("Warning: e_phentsize not equal to sizeof(Elf32_Phdr): %d 32\n", hdr->e_phentsize);
	}
	
	// Read the table of phdrs
	for (i = 0; i < hdr->e_phnum; ++i) {
		fread(&phdrs[i], hdr->e_phentsize, 1, f);
		printf("Read the program segment header! %#08X\n", phdrs[i].p_type);
	}
	
	fclose(f);
	return;
}
