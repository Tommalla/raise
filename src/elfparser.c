/* Tomasz Zakrzewski, tz336079        /
 * ZSO 2015/2016, raise - ELF parser */
#include <stdlib.h>
#include <stdio.h>

#include "elfparser.h"

void parse_elf(char *path, Elf32_Ehdr *hdr, Elf32_Shdr **shdr) {
	FILE *f = fopen(path, "r");
	hdr = (Elf32_Ehdr *)malloc(sizeof(Elf32_Ehdr)); 
	fread(hdr, sizeof(Elf32_Ehdr), 1, f);
	printf("Read the file header! %d\n", hdr->e_ehsize);
	return;
}
