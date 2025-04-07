#ifndef FILE_ACCESS_H
#define FILE_ACCESS_H

#include "ext2.h"

int32_t fetchBlockFromFile(Inode *inode, uint32_t bNum, void *buf);
int32_t writeBlockToFile(Inode *inode, uint32_t bNum, void *buf);

#endif
