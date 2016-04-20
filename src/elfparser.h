/* Tomasz Zakrzewski, tz336079        /
 * ZSO 2015/2016, raise - ELF parser */
#ifndef ELFPARSER_H
#define ELFPARSER_H

unsigned long REAX, REBX, RECX, REDX, RESP, REBP, RESI, REDI, REFLAGS, REIP;

/**
 * @brief Reads and processes the elf file.
 * 
 * This includes mapping the memory.
 */
void process_elf(char *path);

#endif // ELFPARSER_H
