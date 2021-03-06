/*
 * ext2_err.h:
 * This file is automatically generated; please do not edit it.
 */
#ifdef __STDC__
#define NOARGS void
#else
#define NOARGS
#define const
#endif

#define EXT2_ET_BASE                             (2133571328L)
#define EXT2_ET_BAD_MAGIC                        (2133571329L)
#define EXT2_ET_SB_LSEEK                         (2133571330L)
#define EXT2_ET_SB_READ                          (2133571331L)
#define EXT2_ET_SB_WRITE                         (2133571332L)
#define EXT2_ET_RO_FILSYS                        (2133571333L)
#define EXT2_ET_GDESC_READ                       (2133571334L)
#define EXT2_ET_GDESC_WRITE                      (2133571335L)
#define EXT2_ET_GDESC_BAD_BLOCK_MAP              (2133571336L)
#define EXT2_ET_GDESC_BAD_INODE_MAP              (2133571337L)
#define EXT2_ET_GDESC_BAD_INODE_TABLE            (2133571338L)
#define EXT2_ET_INODE_BITMAP_WRITE               (2133571339L)
#define EXT2_ET_INODE_BITMAP_READ                (2133571340L)
#define EXT2_ET_BLOCK_BITMAP_WRITE               (2133571341L)
#define EXT2_ET_BLOCK_BITMAP_READ                (2133571342L)
#define EXT2_ET_INODE_TABLE_WRITE                (2133571343L)
#define EXT2_ET_INODE_TABLE_READ                 (2133571344L)
#define EXT2_ET_NEXT_INODE_READ                  (2133571345L)
#define EXT2_ET_UNEXPECTED_BLOCK_SIZE            (2133571346L)
#define EXT2_ET_DIR_CORRUPTED                    (2133571347L)
#define EXT2_ET_SHORT_READ                       (2133571348L)
#define EXT2_ET_SHORT_WRITE                      (2133571349L)
#define EXT2_ET_DIR_NO_SPACE                     (2133571350L)
#define EXT2_ET_NO_INODE_BITMAP                  (2133571351L)
#define EXT2_ET_NO_BLOCK_BITMAP                  (2133571352L)
#define EXT2_ET_BAD_INODE_NUM                    (2133571353L)
#define EXT2_ET_BAD_BLOCK_NUM                    (2133571354L)
#define EXT2_ET_EXPAND_DIR_ERR                   (2133571355L)
#define EXT2_ET_TOOSMALL                         (2133571356L)
extern void initialize_ext2_error_table (NOARGS);
#define ERROR_TABLE_BASE_ext2 (2133571328L)

/* for compatibility with older versions... */
#define init_ext2_err_tbl initialize_ext2_error_table
#define ext2_err_base ERROR_TABLE_BASE_ext2
