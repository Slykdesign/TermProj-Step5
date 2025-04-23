#ifndef FILE_ACCESS_H
#define FILE_ACCESS_H

#include "ext2.h"
#include "inode.h"

int32_t fetchBlockFromFile(struct Ext2File *f, Inode *i, uint32_t bNum, void *buf);
int32_t writeBlockToFile(struct Ext2File *f, Inode *i, uint32_t bNum, void *buf);
uint32_t Allocate(struct Ext2File *f);
void updateMetadata(struct Ext2File *f, uint32_t blockNum);

#endif