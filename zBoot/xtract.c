/*
 *  linux/zBoot/xtract.c
 *
 *  Copyright (C) 1993  Hannu Savolainen
 *
 *	Extracts the system image and writes it to the stdout.
 *	based on tools/build.c by Linus Torvalds
 */

#include <stdio.h>	/* fprintf */
#include <string.h>
#include <stdlib.h>	/* contains exit */
#include <sys/types.h>	/* unistd.h needs this */
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <unistd.h>	/* contains read/write */
#include <fcntl.h>
#include "../include/linux/elf.h"

#define GCC_HEADER 1024

#define STRINGIFY(x) #x

void die(char * str)
{
	fprintf(stderr,"%s\n",str);
	exit(1);
}

void usage(void)
{
	die("Usage: xtract system [ | gzip | piggyback > piggy.s]");
}

int main(int argc, char ** argv)
{
	int i,c,id,sz,total_sz=0;
	char buf[1024];
	char buff[1024];
	char major_root, minor_root;
	struct stat sb;

	struct elfhdr *hdr= (struct elfhdr*)buf;

	if (argc != 2)
		usage();

	if ((id=open(argv[1],O_RDONLY,0))<0)
		die("Unable to open 'system'");

	if (read(id,buf,GCC_HEADER) != GCC_HEADER)
		die("Unable to read header of 'system'");
fprintf(stderr,"phnum:%d\n",hdr->e_phnum);
	struct elf_phdr *phdr=&buf[hdr->e_phoff];
	for(i=0; i<hdr->e_phnum;i++,phdr++)
	{
fprintf(stderr,"type:0x%x,size:0x%x,msize:0x%x,off:0x%x,va:0x%x,pa:0x%x\n",phdr->p_type,phdr->p_filesz,phdr->p_memsz,phdr->p_offset,phdr->p_vaddr,phdr->p_paddr);
		if (!phdr->p_type || !phdr->p_memsz)
			continue;
		sz = phdr->p_filesz;
		if((phdr->p_vaddr+phdr->p_memsz-0x100000)>total_sz)
			total_sz=(phdr->p_vaddr+phdr->p_memsz-0x100000);
   		lseek(id,phdr->p_offset,SEEK_SET);
   		lseek(1,phdr->p_vaddr-0x100000,SEEK_SET);

		while (sz > 0)
		{
			int l, n;

			l = sz;
			if (l > sizeof(buff)) l = sizeof(buff);

			if ((n=read(id, buff, l)) !=l)
			{
				if (n == -1) 
			   		perror(argv[1]);
				else
			   		fprintf(stderr, "Unexpected EOF\n");

				die("Can't read system");
			}

			write(1, buff, l);
			sz -= l;
		}
		sz = phdr->p_filesz;
                memset(buff,0,sizeof(buff));
                while (sz<phdr->p_memsz)
                {
                        int l, n;

                        l = phdr->p_memsz - sz;
                        if (l > sizeof(buff))
                                l = sizeof(buff);
                        if (write(1, buff, l) != l)
                                die("Write failed");
                        sz += l;
                }
	}
	fprintf(stderr, "System size is %d\n", total_sz);

	close(id);
	return(0);
}
