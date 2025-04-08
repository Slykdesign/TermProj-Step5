#ifndef FILE_ACCESS_H
#define FILE_ACCESS_H

#include "ext2.h"
#include "inode.h"

int32_t fetchBlockFromFile(Ext2File *f, Inode *i, uint32_t bNum, void *buf);
int32_t writeBlockToFile(Ext2File *f, Inode *i, uint32_t bNum, void *buf);
uint32_t Allocate(Ext2File *f);
void updateMetadata(Ext2File *f, uint32_t blockNum);

#endif