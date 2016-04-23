/* Tomasz Zakrzewski, tz336079        /
 * ZSO 2015/2016, raise - ELF parser */
#include <elf.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/reg.h>
#include <sys/procfs.h>
#include <sys/stat.h>
#include <sys/syscall.h>

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
		int map_fd = syscall(SYS_open, sptr, O_RDONLY);
		// move to next NUL
		for(; *sptr != '\0'; ++sptr);
		sptr++;
		
		// FINALLY map the memory
		syscall(SYS_mmap2, (void *)start, end - start, PROT_WRITE | PROT_READ | PROT_EXEC, 
			MAP_PRIVATE | MAP_FIXED, map_fd, page_of * page_size / 4096);
		
		syscall(SYS_close, map_fd);
	}
}

void process_prstatus(const char* buffer) {
	prstatus_t prstatus;
	memcpy(&prstatus, buffer, sizeof(prstatus_t));
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
	int fd = syscall(SYS_open, path, O_RDONLY);
	syscall(SYS_read, fd, &hdr, sizeof(Elf32_Ehdr));
	
	// Move to the beginning of program segment headers
	syscall(SYS_lseek, fd, hdr.e_phoff, SEEK_SET);
	
	// Read the table of phdrs
	for (i = 0; i < hdr.e_phnum; ++i) {
		syscall(SYS_read,fd, &phdrs[i], hdr.e_phentsize);
	}
	
	for (i = 0; i < hdr.e_phnum; ++i) {
		syscall(SYS_mmap2, (void *)phdrs[i].p_vaddr, phdrs[i].p_memsz, PROT_READ | PROT_WRITE | PROT_EXEC, 
			MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
	}
	
	// Map PT_NOTE files
	for (i = 0; i < hdr.e_phnum; ++i) {
		if (phdrs[i].p_type == PT_NOTE) {
			syscall(SYS_lseek, fd, phdrs[i].p_offset, SEEK_SET);
			size_t bytes_read = 0;
			while (bytes_read < phdrs[i].p_filesz) {
				// Read the note header
				Elf32_Nhdr nhdr;
				syscall(SYS_read, fd, &nhdr, sizeof(Elf32_Nhdr));
				            bytes_read += sizeof(Elf32_Nhdr);
				
				// Skip name (we don't use it anyway)
				int padding = (4 - (nhdr.n_namesz % 4)) % 4;
				syscall(SYS_lseek, fd, nhdr.n_namesz + padding, SEEK_CUR);
				            bytes_read += nhdr.n_namesz + padding;
				
				// Read the description.
				syscall(SYS_read, fd, desc_buf, nhdr.n_descsz);
				padding = (4 - (nhdr.n_descsz % 4)) % 4;
				            bytes_read += nhdr.n_descsz + padding;
				
				// Skip padding
				syscall(SYS_lseek, fd, padding, SEEK_CUR);
				
				switch (nhdr.n_type) {
					case NT_FILE:
						process_note_file(desc_buf);
						break;
					case NT_PRSTATUS:
						process_prstatus(desc_buf);
						break;
					case NT_386_TLS:
						process_tls(desc_buf);
						break;
					default:
						break;
				};
			}
		}
	}
	
	// Scan the table again, this time only load PT_LOAD and set the flags
	for (i = 0; i < hdr.e_phnum; ++i) {
		if (phdrs[i].p_type == PT_LOAD) {
			if (phdrs[i].p_filesz > 0) {
				syscall(SYS_mmap2, (void *)(phdrs[i].p_vaddr), phdrs[i].p_filesz, 
					PROT_READ | PROT_WRITE | PROT_EXEC, 
					MAP_PRIVATE | MAP_FIXED, fd, phdrs[i].p_offset / 4096);
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
			
			syscall(SYS_mprotect, (void *)(phdrs[i].p_vaddr), phdrs[i].p_memsz, RWX);
		}
	}
	
	syscall(SYS_close, fd);
	return;
}
