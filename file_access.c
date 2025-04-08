#include "file_access.h"
#include "ext2.h"
#include "inode.h"
#include <stdlib.h>

#define BLOCK_SIZE 1024 // Assuming a block size of 1024 bytes

int32_t fetchBlockFromFile(Ext2File *f, Inode *i, uint32_t bNum, void *buf) {
    uint32_t blockNum;
    if (bNum < 12) {
        // Direct block
        blockNum = i->i_block[bNum];
    } else {
        uint32_t k = f->block_size / 4;
        if (bNum < 12 + k) {
            // Single indirect block
            if (i->i_block[12] == 0) return -1;
            uint32_t singleIndirectBlock[k];
            if (!fetchBlock(f, i->i_block[12], singleIndirectBlock)) return -1;
            blockNum = singleIndirectBlock[bNum - 12];
        } else if (bNum < 12 + k + k * k) {
            // Double indirect block
            if (i->i_block[13] == 0) return -1;
            uint32_t doubleIndirectBlock[k];
            if (!fetchBlock(f, i->i_block[13], doubleIndirectBlock)) return -1;
            uint32_t singleIndirectBlock[k];
            if (!fetchBlock(f, doubleIndirectBlock[(bNum - 12 - k) / k], singleIndirectBlock)) return -1;
            blockNum = singleIndirectBlock[(bNum - 12 - k) % k];
        } else {
            // Triple indirect block
            if (i->i_block[14] == 0) return -1;
            uint32_t tripleIndirectBlock[k];
            if (!fetchBlock(f, i->i_block[14], tripleIndirectBlock)) return -1;
            uint32_t doubleIndirectBlock[k];
            if (!fetchBlock(f, tripleIndirectBlock[(bNum - 12 - k - k * k) / (k * k)], doubleIndirectBlock)) return -1;
            uint32_t singleIndirectBlock[k];
            if (!fetchBlock(f, doubleIndirectBlock[(bNum - 12 - k - k * k) % (k * k) / k], singleIndirectBlock)) return -1;
            blockNum = singleIndirectBlock[(bNum - 12 - k - k * k) % k];
        }
    }
    return fetchBlock(f, blockNum, buf) ? 0 : -1;
}

int32_t writeBlockToFile(Ext2File *f, Inode *i, uint32_t bNum, void *buf) {
    uint32_t blockNum;
    if (bNum < 12) {
        // Direct block
        blockNum = i->i_block[bNum];
    } else {
        uint32_t k = f->block_size / 4;
        if (bNum < 12 + k) {
            // Single indirect block
            if (i->i_block[12] == 0) return -1;
            uint32_t singleIndirectBlock[k];
            if (!fetchBlock(f, i->i_block[12], singleIndirectBlock)) return -1;
            blockNum = singleIndirectBlock[bNum - 12];
        } else if (bNum < 12 + k + k * k) {
            // Double indirect block
            if (i->i_block[13] == 0) return -1;
            uint32_t doubleIndirectBlock[k];
            if (!fetchBlock(f, i->i_block[13], doubleIndirectBlock)) return -1;
            uint32_t singleIndirectBlock[k];
            if (!fetchBlock(f, doubleIndirectBlock[(bNum - 12 - k) / k], singleIndirectBlock)) return -1;
            blockNum = singleIndirectBlock[(bNum - 12 - k) % k];
        } else {
            // Triple indirect block
            if (i->i_block[14] == 0) return -1;
            uint32_t tripleIndirectBlock[k];
            if (!fetchBlock(f, i->i_block[14], tripleIndirectBlock)) return -1;
            uint32_t doubleIndirectBlock[k];
            if (!fetchBlock(f, tripleIndirectBlock[(bNum - 12 - k - k * k) / (k * k)], doubleIndirectBlock)) return -1;
            uint32_t singleIndirectBlock[k];
            if (!fetchBlock(f, doubleIndirectBlock[(bNum - 12 - k - k * k) % (k * k) / k], singleIndirectBlock)) return -1;
            blockNum = singleIndirectBlock[(bNum - 12 - k - k * k) % k];
        }
    }
    return writeBlock(f, blockNum, buf) ? 0 : -1;
}