#include "partition.h"
#include "ext2.h"
#include "directory.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>

// Function prototypes
void displayBufferPage(uint8_t *buf, uint32_t count, uint32_t skip, uint64_t offset);
void displayBuffer(uint8_t *buf, uint32_t count, uint64_t offset);

int main() {
    struct Ext2File *ext2 = openExt2("./good-fixed-4k.vdi");
    if (!ext2) return 1;

    printf("Root directory contents\n");
    struct Directory *d = openDir(ext2, 2); // Open root directory
    uint32_t iNum;
    char name[256];
    while (getNextDirect(d, &iNum, name)) {
        printf("Inode: %u   name: [%s]\n", iNum, name);
    }
    closeDir(d);

    printf("\nlost+found directory contents\n");
    d = openDir(ext2, 11); // Open lost+found directory
    while (getNextDirect(d, &iNum, name)) {
        printf("Inode: %u   name: [%s]\n", iNum, name);
    }
    closeDir(d);
    return 0;
}

void displayBufferPage(uint8_t *buf, uint32_t count, uint32_t skip, uint64_t offset) {
    count = (count > 256) ? 256 : count;  // Limit to 256 bytes
    printf("Offset: 0x%lx\n", offset);
    printf("  00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 0...4...8...c...\n");
    printf("  +------------------------------------------------+   +----------------+\n");

    for (uint32_t i = 0; i < 16; i++) {  // 16 lines of 16 bytes
        printf("%02x|", (unsigned int)(offset + i * 16));

        // Hexadecimal display
        for (uint32_t j = 0; j < 16; j++) {
            size_t pos = i * 16 + j;
            if (pos >= skip && pos < skip + count) {
                printf("%02x ", buf[pos]);
            } else {
                printf("   ");
            }
        }

        printf("|%02x|", (unsigned int)(offset + i * 16));

        // Character display
        for (uint32_t j = 0; j < 16; j++) {
            size_t pos = i * 16 + j;
            if (pos >= skip && pos < skip + count) {
                printf("%c", isprint(buf[pos]) ? buf[pos] : '.');
            } else {
                printf(" ");
            }
        }
        printf("|\n");
    }
    printf("  +------------------------------------------------+   +----------------+\n\n");
}

void displayBuffer(uint8_t *buf, uint32_t count, uint64_t offset) {
    for (uint32_t i = 0; i < count; i += 256) {
        uint32_t chunk_size = (count - i > 256) ? 256 : count - i;
        displayBufferPage(buf + i, chunk_size, 0, offset + i);
    }
}