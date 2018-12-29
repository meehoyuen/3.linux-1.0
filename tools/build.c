/*
 *  linux/tools/build.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

/*
 * This file builds a disk-image from three different files:
 *
 * - bootsect: max 510 bytes of 8086 machine code, loads the rest
 * - setup: max 4 sectors of 8086 machine code, sets up system parm
 * - system: 80386 code for actual system
 *
 * It does some checking that all files are of the correct type, and
 * just writes the result to stdout, removing headers and padding to
 * the right amount. It also writes some system data to stderr.
 */

/*
 * Changes by tytso to allow root device specification
 */

#include <stdio.h>	/* fprintf */
#include <string.h>
#include <stdlib.h>	/* contains exit */
#include <sys/types.h>	/* unistd.h needs this */
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <unistd.h>	/* contains read/write */
#include <fcntl.h>
#include "../include/linux/config.h"
#include "../include/linux/elf.h"

#define MINIX_HEADER 32
#define GCC_HEADER 1024

#define SYS_SIZE DEF_SYSSIZE

#define DEFAULT_MAJOR_ROOT 3
#define DEFAULT_MINOR_ROOT 1

/* max nr of sectors of setup: don't change unless you also change
 * bootsect etc */
#define SETUP_SECTS 4

#define STRINGIFY(x) #x

typedef union {
	long l;
	short s[2];
	char b[4];
} conv;

long intel_long(long l)
{
	conv t;

	t.b[0] = l & 0xff; l >>= 8;
	t.b[1] = l & 0xff; l >>= 8;
	t.b[2] = l & 0xff; l >>= 8;
	t.b[3] = l & 0xff; l >>= 8;
	return t.l;
}

short intel_short(short l)
{
	conv t;

	t.b[0] = l & 0xff; l >>= 8;
	t.b[1] = l & 0xff; l >>= 8;
	return t.s[0];
}

void die(char * str)
{
	fprintf(stderr,"%s\n",str);
	exit(1);
}

void usage(void)
{
	die("Usage: build bootsect setup system [rootdev] [> image]");
}

int main(int argc, char ** argv)
{
	int i,c,id,sz,total_sz=0;
	unsigned long sys_size;
	char buf[1024];
	char buff[1024];
	struct elfhdr *hdr = (struct elfhdr*)buf;
	char major_root, minor_root;
	struct stat sb;

	if ((argc < 4) || (argc > 5))
		usage();
	if (argc > 4) {
		if (!strcmp(argv[4], "CURRENT")) {
			if (stat("/", &sb)) {
				perror("/");
				die("Couldn't stat /");
			}
			major_root = major(sb.st_dev);
			minor_root = minor(sb.st_dev);
		} else if (strcmp(argv[4], "FLOPPY")) {
			if (stat(argv[4], &sb)) {
				perror(argv[4]);
				die("Couldn't stat root device.");
			}
			major_root = major(sb.st_rdev);
			minor_root = minor(sb.st_rdev);
		} else {
			major_root = 0;
			minor_root = 0;
		}
	} else {
		major_root = DEFAULT_MAJOR_ROOT;
		minor_root = DEFAULT_MINOR_ROOT;
	}
	fprintf(stderr, "Root device is (%d, %d)\n", major_root, minor_root);
	for (i=0;i<sizeof buf; i++) buf[i]=0;
	if ((id=open(argv[1],O_RDONLY,0))<0)
		die("Unable to open 'boot'");
	if (read(id,buf,MINIX_HEADER) != MINIX_HEADER)
		die("Unable to read header of 'boot'");
	i=read(id,buf,sizeof buf);
	fprintf(stderr,"Boot sector %d bytes.\n",i);
	if (i != 512)
		die("Boot block must be exactly 512 bytes");
	if ((*(unsigned short *)(buf+510)) != (unsigned short)intel_short(0xAA55))
		die("Boot block hasn't got boot flag (0xAA55)");
	buf[508] = (char) minor_root;
	buf[509] = (char) major_root;	
	i=write(1,buf,512);
	if (i!=512)
		die("Write call failed");
	close (id);
	
	if ((id=open(argv[2],O_RDONLY,0))<0)
		die("Unable to open 'setup'");
	if (read(id,buf,MINIX_HEADER) != MINIX_HEADER)
		die("Unable to read header of 'setup'");
	for (i=0 ; (c=read(id,buf,sizeof buf))>0 ; i+=c )
		if (write(1,buf,c)!=c)
			die("Write call failed");
	if (c != 0)
		die("read-error on 'setup'");
	close (id);
	if (i > SETUP_SECTS*512)
		die("Setup exceeds " STRINGIFY(SETUP_SECTS)
			" sectors - rewrite build/boot/setup");
	fprintf(stderr,"Setup is %d bytes.\n",i);
	for (c=0 ; c<sizeof(buf) ; c++)
		buf[c] = '\0';
	while (i<SETUP_SECTS*512) {
		c = SETUP_SECTS*512-i;
		if (c > sizeof(buf))
			c = sizeof(buf);
		if (write(1,buf,c) != c)
			die("Write call failed");
		i += c;
	}
	
	if ((id=open(argv[3],O_RDONLY,0))<0)
		die("Unable to open 'system'");

	if (read(id,buf,GCC_HEADER) != GCC_HEADER)
		die("Unable to read header of 'system'");
	fprintf(stderr,"phnum:%hd\n",hdr->e_phnum);
	struct elf_phdr *phdr=&buf[hdr->e_phoff];
        for(i=0; i<hdr->e_phnum;i++,phdr++)
        {
		fprintf(stderr,"type:0x%x,size:0x%x,msize:0x%x,off:0x%x,va:0x%x,pa:0x%x\n",
			phdr->p_type,phdr->p_filesz,phdr->p_memsz,
			phdr->p_offset,phdr->p_vaddr,phdr->p_paddr);
                if (!phdr->p_type || !phdr->p_memsz)
                        continue;
                sz = phdr->p_filesz;
                if((phdr->p_vaddr+phdr->p_memsz-0x1000)>total_sz)
                        total_sz=(phdr->p_vaddr+phdr->p_memsz-0x1000);
                lseek(id,phdr->p_offset,SEEK_SET);
                lseek(1,phdr->p_vaddr+(SETUP_SECTS-7)*512,SEEK_SET);

		while (sz > 0)
		{
			int l, n;

			l = sz;
			if (l > sizeof(buff))
				l = sizeof(buff);
			if ((n=read(id, buff, l)) != l) {
				if (n == -1) 
					perror(argv[1]);
				else
					fprintf(stderr, "Unexpected EOF\n");
				die("Can't read 'system'");
			}
			if (write(1, buff, l) != l)
				die("Write failed");
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
	close(id);

	sys_size = (total_sz + 15) / 16;
	fprintf(stderr,"System is %d kB\n", total_sz/1024);

	if (lseek(1,500,0) == 500) {
		buf[0] = (sys_size & 0xff);
		buf[1] = ((sys_size >> 8) & 0xff);
		if (write(1, buf, 2) != 2)
			die("Write failed");
	}

	if (sys_size > SYS_SIZE)
		die("System is too big");

	return(0);
}
