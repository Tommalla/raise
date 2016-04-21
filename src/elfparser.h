/* Tomasz Zakrzewski, tz336079        /
 * ZSO 2015/2016, raise - ELF parser */
#ifndef ELFPARSER_H
#define ELFPARSER_H
#include <asm/ldt.h>

unsigned long REAX, REBX, RECX, REDX, RESP, REBP, RESI, REDI, REFLAGS, REIP, RGS;
struct user_desc USER_DESC;

/**
 * @brief Reads and processes the elf file.
 * 
 * This includes mapping the memory.
 */
void process_elf(char *path);

#endif // ELFPARSER_H
