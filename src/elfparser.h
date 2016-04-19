/* Tomasz Zakrzewski, tz336079        /
 * ZSO 2015/2016, raise - ELF parser */
#ifndef ELFPARSER_H
#define ELFPARSER_H

#include <elf.h>

void parse_elf(char *path, Elf32_Ehdr *hdr, Elf32_Shdr **shdr);

#endif // ELFPARSER_H