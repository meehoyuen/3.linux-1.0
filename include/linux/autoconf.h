/*
 * Automatically generated C config: don't edit
 */

/* 
 * General setup
 * 
 */
#define CONFIG_MATH_EMULATION 1
#define CONFIG_BLK_DEV_HD 1
#undef CONFIG_BLK_DEV_XD
#define CONFIG_INET 1
#undef CONFIG_MAX_16M
#define CONFIG_SYSVIPC 1
#undef CONFIG_M486

/* 
 * Program binary formats
 * 
 */
#define CONFIG_BINFMT_ELF 1
#define CONFIG_BINFMT_COFF 1

/* 
 * SCSI support
 * 
 */
#undef CONFIG_SCSI

/* 
 * Network device support
 * 
 */
#define CONFIG_ETHERCARDS 1
#undef CONFIG_SLIP
#undef CONFIG_PLIP
#define CONFIG_NE2000 1
#undef CONFIG_WD80x3
#undef CONFIG_ULTRA
#undef CONFIG_EL1
#undef CONFIG_EL2
#undef CONFIG_EL3
#undef CONFIG_HPLAN
#undef CONFIG_LANCE
#undef CONFIG_AT1700
#undef CONFIG_DEPCA
#undef CONFIG_DE600
#undef CONFIG_ATP

/* 
 */
#undef CONFIG_CDU31A
#undef CONFIG_MCD
#undef CONFIG_SBPCD

/* 
 * Filesystems
 * 
 */
#undef CONFIG_MINIX_FS
#undef CONFIG_EXT_FS
#define CONFIG_EXT2_FS 1
#undef CONFIG_XIA_FS
#undef CONFIG_MSDOS_FS
#define CONFIG_PROC_FS 1
#define CONFIG_NFS_FS 1
#undef CONFIG_ISO9660_FS
#undef CONFIG_HPFS_FS
#define CONFIG_SYSV_FS 1

/* 
 * character devices
 * 
 */
#undef CONFIG_PRINTER
#undef CONFIG_BUSMOUSE
#define CONFIG_PSMOUSE 1
#undef CONFIG_82C710_MOUSE
#define CONFIG_MS_BUSMOUSE 1
#undef CONFIG_ATIXL_BUSMOUSE
#define CONFIG_SELECTION 1
#define CONFIG_TAPE_QIC02 1
#define CONFIG_FTAPE 1
#define NR_FTAPE_BUFFERS (3)

/* 
 * Sound
 * 
 */
#undef CONFIG_SOUND

/* 
 * Kernel hacking
 * 
 */
#undef CONFIG_PROFILE
