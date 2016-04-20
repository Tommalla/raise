/* Tomasz Zakrzewski, tz336079        /
 * ZSO 2015/2016, raise - ELF parser */
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/stat.h>

#include "elfparser.h"

char desc_buf[100000];
void *mapped_mem[100000];

void process_elf(char *path, Elf32_Ehdr *hdr, Elf32_Phdr *phdrs) {
	int i;
	FILE *f = fopen(path, "r");
	int fd = fileno(f);
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
		// FIXME: RWE?
		mapped_mem[i] = mmap((void *)phdrs[i].p_vaddr, phdrs[i].p_memsz, PROT_EXEC | PROT_READ | PROT_WRITE, 
				     MAP_PRIVATE | MAP_ANONYMOUS, fd, phdrs[i].p_offset);
		if (mapped_mem[i] < 0) {
			printf("\tError! Couldn't map the anonymous memory area!");
		}
	}
	
	// Map PT_NOTE files
	for (i = 0; i < hdr->e_phnum; ++i) {
		if (phdrs[i].p_type == PT_NOTE) {
			printf("Reading PT_NOTEs at id %d, size: %d\n", i, phdrs[i].p_filesz);
			fseek(f, phdrs[i].p_offset, SEEK_SET);
			size_t read = 0;
			while (read < phdrs[i].p_filesz) {
				Elf32_Nhdr nhdr;
				fread(&nhdr, sizeof(Elf32_Nhdr), 1, f);
				int padding = (4 - (nhdr.n_namesz % 4)) % 4;
				read += sizeof(Elf32_Nhdr) + padding;
				printf("\t(%d) Read successful: type = %#08X, descsz = %d\n", read, nhdr.n_type, nhdr.n_descsz);
				fseek(f, nhdr.n_namesz + padding, SEEK_CUR);
				fread(desc_buf, nhdr.n_descsz, 1, f);
				padding = (4 - (nhdr.n_descsz % 4)) % 4;
				read += nhdr.n_descsz + padding;
				fseek(f, padding, SEEK_CUR);
				// FIXME NT_PRSTATUS
				if (nhdr.n_type != NT_FILE) {
					puts("\tNot an NT_FILE, skipping");
					continue;
				} else {
					long count, page_size;
					void *ptr = desc_buf;
					memcpy(&count, ptr, sizeof(long));
					ptr += sizeof(long);
					memcpy(&page_size, ptr, sizeof(long));
					ptr += sizeof(long);
					long off = 0;
					char *sptr = ptr + 3 * sizeof(long) * count;
					for (off = 0; off < count; ++off) {
						long start, end, page_of;
						long offset = off * 3 * sizeof(long);
						memcpy(&start, ptr + offset, sizeof(long));
						memcpy(&end, ptr + offset + sizeof(long), sizeof(long));
						memcpy(&page_of, ptr + offset + 2 * sizeof(long), sizeof(long));
						int map_fd = open(sptr, O_RDONLY);
						// move to next NUL
						printf("%#08X %#08X %#08X %s\n", start, end, page_of, sptr); 
						for(; *sptr != '\0'; ++sptr);
						sptr++;
						
						// FINALLY map the memory
						// TODO RWE
						mmap((void *)start, end - start, PROT_EXEC | PROT_READ | PROT_WRITE, 
						     MAP_PRIVATE, map_fd, page_of);
						
						//mmap(NULL
						close(map_fd);
					}
					
				}
			}
		}
	}
	
	// Scan the table again, this time only load PT_LOAD
	for (i = 0; i < hdr->e_phnum; ++i) {
		if (phdrs[i].p_type == PT_LOAD) {
			printf("Reading PT_LOAD at id: %d\n", i);
			// TODO
		}
	}
	
	fclose(f);
	return;
}
