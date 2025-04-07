#include "file_access.h"
#include "ext2.h"
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 1024 // Assuming a block size of 1024 bytes

int32_t fetchBlockFromFile(Inode *inode, uint32_t bNum, void *buf) {
    uint32_t k = BLOCK_SIZE / 4;
    uint32_t *blockList;
    uint8_t tmpBuf[BLOCK_SIZE];

    if (bNum < 12) {
        blockList = inode->i_block;
        goto direct;
    } else if (bNum < 12 + k) {
        if (inode->i_block[12] == 0) return -1;
        fetchBlock(inode->i_block[12], tmpBuf);
        blockList = (uint32_t *)tmpBuf;
        bNum -= 12;
        goto direct;
    } else if (bNum < 12 + k + k * k) {
        if (inode->i_block[13] == 0) return -1;
        fetchBlock(inode->i_block[13], tmpBuf);
        blockList = (uint32_t *)tmpBuf;
        bNum -= 12 + k;
        goto single;
    } else {
        if (inode->i_block[14] == 0) return -1;
        fetchBlock(inode->i_block[14], tmpBuf);
        blockList = (uint32_t *)tmpBuf;
        bNum -= 12 + k + k * k;
    }

    // Fetching from double indirect block
    index = bNum / (k * k);
    bNum %= (k * k);
    if (blockList[index] == 0) return -1;
    fetchBlock(blockList[index], tmpBuf);
    blockList = (uint32_t *)tmpBuf;

single:
    // Fetching from single indirect block
    index = bNum / k;
    bNum %= k;
    if (blockList[index] == 0) return -1;
    fetchBlock(blockList[index], tmpBuf);
    blockList = (uint32_t *)tmpBuf;

direct:
    // Fetching from direct block
    if (blockList[bNum] == 0) return -1;
    fetchBlock(blockList[bNum], buf);
    return 0;
}

int32_t writeBlockToFile(Inode *inode, uint32_t bNum, void *buf) {
    uint32_t k = BLOCK_SIZE / 4;
    uint32_t *blockList;
    uint8_t tmpBuf[BLOCK_SIZE];
    uint32_t ibNum;

    if (bNum < 12) {
        blockList = inode->i_block;
        if (blockList[bNum] == 0) {
            blockList[bNum] = Allocate();
            writeInode(inode);
        }
        goto direct;
    } else if (bNum < 12 + k) {
        if (inode->i_block[12] == 0) {
            inode->i_block[12] = Allocate();
            writeInode(inode);
        }
        fetchBlock(inode->i_block[12], tmpBuf);
        ibNum = inode->i_block[12];
        blockList = (uint32_t *)tmpBuf;
        bNum -= 12;
        goto direct;
    } else if (bNum < 12 + k + k * k) {
        if (inode->i_block[13] == 0) {
            inode->i_block[13] = Allocate();
            writeInode(inode);
        }
        fetchBlock(inode->i_block[13], tmpBuf);
        ibNum = inode->i_block[13];
        blockList = (uint32_t *)tmpBuf;
        bNum -= 12 + k;
        goto single;
    } else {
        if (inode->i_block[14] == 0) {
            inode->i_block[14] = Allocate();
            writeInode(inode);
        }
        fetchBlock(inode->i_block[14], tmpBuf);
        ibNum = inode->i_block[14];
        blockList = (uint32_t *)tmpBuf;
        bNum -= 12 + k + k * k;
    }

    // Fetching from double indirect block
    index = bNum / (k * k);
    bNum %= (k * k);
    if (blockList[index] == 0) {
        blockList[index] = Allocate();
        writeBlock(ibNum, blockList);
    }
    ibNum = blockList[index];
    fetchBlock(blockList[index], tmpBuf);
    blockList = (uint32_t *)tmpBuf;

single:
    // Fetching from single indirect block
    index = bNum / k;
    bNum %= k;
    if (blockList[index] == 0) {
        blockList[index] = Allocate();
        writeBlock(ibNum, blockList);
    }
    ibNum = blockList[index];
    fetchBlock(blockList[index], tmpBuf);
    blockList = (uint32_t *)tmpBuf;

direct:
    // Fetching from direct block
    if (blockList[bNum] == 0) {
        blockList[bNum] = Allocate();
        writeBlock(ibNum, blockList);
    }
    writeBlock(blockList[bNum], buf);
    return 0;
}
