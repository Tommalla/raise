/* Tomasz Zakrzewski, tz336079        /
 * ZSO 2015/2016, raise - ELF parser */
#ifndef ELFPARSER_H
#define ELFPARSER_H

#include <elf.h>

/**
 * @brief Reads and processes the elf file.
 * 
 * This includes mapping the memory.
 */
void process_elf(char *path, Elf32_Ehdr *hdr, Elf32_Phdr *phdrs);

#endif // ELFPARSER_H
