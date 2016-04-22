/* Tomasz Zakrzewski, tz336079        /
 * ZSO 2015/2016, raise - ELF parser */
#include <elf.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/reg.h>
#include <sys/procfs.h>
#include <sys/stat.h>

#include "elfparser.h"

#define MAX_PHDRS 10000
#define BUFSIZE 100000

char desc_buf[BUFSIZE];

void process_note_file(const char* buffer) {
	// This helper method assumes that 
	long count, page_size;
	const void *ptr = buffer;
	memcpy(&count, ptr, sizeof(long));
	ptr += sizeof(long);
	memcpy(&page_size, ptr, sizeof(long));
	ptr += sizeof(long);
	long off = 0;
	const char *sptr = ptr + 3 * sizeof(long) * count;
	for (off = 0; off < count; ++off) {
		unsigned long start, end, page_of;
		long offset = off * 3 * sizeof(long);
		memcpy(&start, ptr + offset, sizeof(long));
		memcpy(&end, ptr + offset + sizeof(long), sizeof(long));
		memcpy(&page_of, ptr + offset + 2 * sizeof(long), sizeof(long));
		int map_fd = open(sptr, O_RDONLY);
		// move to next NUL
		printf("%#08x %#08x %#08x %s\n", start, end, page_of * page_size, sptr); 
		for(; *sptr != '\0'; ++sptr);
		sptr++;
		
		// FINALLY map the memory
		mmap((void *)start, end - start, PROT_EXEC | PROT_READ | PROT_WRITE, 
		     MAP_PRIVATE | MAP_FIXED, map_fd, 
		     page_of * page_size);
		
		//mmap(NULL
		close(map_fd);
	}
}

void process_prstatus(const char* buffer) {
	prstatus_t prstatus;
	memcpy(&prstatus, buffer, sizeof(prstatus_t));
	printf("\tRead prstatus.\n");
	REAX = prstatus.pr_reg[EAX];
	REBX = prstatus.pr_reg[EBX];
	RECX = prstatus.pr_reg[ECX];
	REDX = prstatus.pr_reg[EDX];
	RESP = prstatus.pr_reg[UESP];
	REBP = prstatus.pr_reg[EBP];
	RESI = prstatus.pr_reg[ESI];
	REDI = prstatus.pr_reg[EDI];
	REFLAGS = prstatus.pr_reg[EFL];
	REIP = prstatus.pr_reg[EIP];
	RGS = prstatus.pr_reg[GS];
}

void process_tls(const char* buffer) {
	memcpy(&USER_DESC, buffer, sizeof(struct user_desc));
}

void process_elf(const char *path) {
	Elf32_Ehdr hdr;
	Elf32_Phdr phdrs[MAX_PHDRS];
	int i;
	FILE *f = fopen(path, "r");
	fread(&hdr, sizeof(Elf32_Ehdr), 1, f);
	
	printf("Read the file header!\n");
	
	// Move to the beginning of program segment headers
	fseek(f, hdr.e_phoff, SEEK_SET);
	
	if (hdr.e_phentsize != sizeof(Elf32_Phdr)) {
		printf("Warning: e_phentsize not equal to sizeof(Elf32_Phdr): %d 32\n", hdr.e_phentsize);
	}
	
	// Read the table of phdrs
	for (i = 0; i < hdr.e_phnum; ++i) {
		fread(&phdrs[i], hdr.e_phentsize, 1, f);
		printf("Read the program segment header! %#08X\n", phdrs[i].p_type);
		if (mmap((void *)phdrs[i].p_vaddr, phdrs[i].p_memsz, PROT_EXEC | PROT_READ | PROT_WRITE, 
			MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0) < 0) {
			printf("\tError! Couldn't map the anonymous memory area!");
		}
	}
	
	// Map PT_NOTE files
	for (i = 0; i < hdr.e_phnum; ++i) {
		if (phdrs[i].p_type == PT_NOTE) {
			printf("Reading PT_NOTEs at id %d, size: %d\n", i, phdrs[i].p_filesz);
			fseek(f, phdrs[i].p_offset, SEEK_SET);
			size_t read = 0;
			while (read < phdrs[i].p_filesz) {
				// Read the note header
				Elf32_Nhdr nhdr;
				fread(&nhdr, sizeof(Elf32_Nhdr), 1, f);
				read += sizeof(Elf32_Nhdr);
				
				// Skip name (we don't use it anyway)
				int padding = (4 - (nhdr.n_namesz % 4)) % 4;
				fseek(f, nhdr.n_namesz + padding, SEEK_CUR);
				read += nhdr.n_namesz + padding;
				
				// Read the description.
				fread(desc_buf, nhdr.n_descsz, 1, f);
				padding = (4 - (nhdr.n_descsz % 4)) % 4;
				read += nhdr.n_descsz + padding;
				
				// Skip padding
				fseek(f, padding, SEEK_CUR);
				
				printf("\t(%d) Note header read successfully: type = %#08X, descsz = %d\n", read, 
				       nhdr.n_type, nhdr.n_descsz);
				switch (nhdr.n_type) {
					case NT_FILE:
						puts("\tNT_FILE");
						process_note_file(desc_buf);
						break;
					case NT_PRSTATUS:
						puts("\tNT_PRSTATUS");
						process_prstatus(desc_buf);
						break;
					case NT_386_TLS:
						puts("\tNT_386_TLS");
						process_tls(desc_buf);
						break;
					default:
						puts("\tNot implemented note type. Ignoring");
						break;
				};
			}
		}
	}
	
	// Scan the table again, this time only load PT_LOAD
	for (i = 0; i < hdr.e_phnum; ++i) {
		if (phdrs[i].p_type == PT_LOAD) {
			if (phdrs[i].p_filesz > 0) {
				printf("Reading PT_LOAD at addr: %#08x\n", phdrs[i].p_vaddr);
				fseek(f, phdrs[i].p_offset, SEEK_SET);
				fread((void *)(phdrs[i].p_vaddr), phdrs[i].p_filesz, 1, f);
			}
			
			int RWX = 0;
			if (phdrs[i].p_flags & PF_X) {
				RWX |= PROT_EXEC;
			}
			if (phdrs[i].p_flags & PF_R) {
				RWX |= PROT_READ;
			}
			if (phdrs[i].p_flags & PF_W) {
				RWX |= PROT_WRITE;
			}
			
			mprotect((void *)(phdrs[i].p_vaddr), phdrs[i].p_memsz, RWX);
		}
	}
	
	fclose(f);
	return;
}
