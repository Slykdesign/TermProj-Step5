# TermProj-Step5
## File Access
### Goals
#### • Provide functions to provide access to data within a file, given the file’s inode.
### Details
#### In this step, we provide access to a file’s data, enabling read and write access to a file. Note that this includes directories, which are just files with a bit set in the file’s inode to indicate it is a directory and not a data file.
### Block-level file acces
#### At the core of the file access functions is the ability to read or write one entire block in the file. This isn’t as simple as using fetchBlock() or writeBlock(); the blocks are numbered sequentially, but the numbers reflect a position within the file, not their position in the filesystem. In other words, block 0 is the first block of the file’s data, block 1 is the second block of file data, and so on
#### To provide block-level access, we need two functions:
##### • int32_t fetchBlockFromFile(struct Inode *i, uint32_t bNum, void *buf)
#### Read block bNum from the file, placing the data in the given buffer.
##### • int32_t writeBlockToFile(struct Inode *i, uint32_t bNum, void *buf)
###### Write the given buffer into block bNumin the file. This may involve allocating one or more data blocks.
#### We must also understand how data is organized in a UNIX file. This begins with the i_block field in an inode. This is an array of 1 5 block numbers. The first 12 entries of i_block contain indexes for the file’s first twelve data blocks
#### For a 1KB block system, that covers the first 1 2KB, which is about the average file size on a UNIX system. The next entry in i_block — slot 12 — doesn’t have the block number for the next data block. It tells you where to find a single indirect block, which is a data block that’s been sliced into 4-byte chunks, each holding the block number of the next k data blocks, where k is the block size divided by 4. Using a 1KB system as an example, k = 256
#### The single indirect block is itself an array of k block numbers, each holding the location of the next data block. Between the 1 2 direct blocks in the i_block array and the single indirect block, we can access the first 12 + k blocks of file data. In a 1KB system, that provides access to the first 268KB of data. If we need to access more data, slot 13 in the i_block array holds the index of a double indirect block, which contains the indexes of k single indirect blocks, each of which holds k indexes of data blocks, giving access to k^2 additional data blocks. In a 1KB file system, this yields an additional 65536KB of data
#### If more space is needed — most of you have worked with files larger than 6 6MB — there is still one more level of indirection available. The final slot in i_block holds the index of a triple indirect block. That block holds the indexes of k double indirect blocks, which each hold the indexes of k single indirect blocks, which each hold the indexes of k data blocks. This provides access to a final k^3 data blocks; with a 1KB block size, this provides access to an additional 16777216KB of data. If more space is needed, a larger block size is used; there is no further level of indirection.
#### This looks intimidating, but it is actually rather easy to navigate, and quite efficient — any byte can be accessed in at most five disk accesses (inode, triple, double, single, data blocks).
#### Here are some observations about the file structure:
##### The index of every data block is held in an array. That array might be the i_block array or it might be an array held in a single indirect block.
##### The same can be said about the indexes of the single, double and triple indirect blocks.
##### The trees headed by the single, double and triple indirect blocks all have a regular structure, with each node having exactly k children, where k is the block size divided by 4.
##### Every single indirect block accesses k data blocks; every double indirect block accesses k single indirect blocks and k2 data blocks.
#### The approach to take is to first determine which tree, if any, we need to descend to find our data block, and determine which block number within that tree we want. Then, we can descend the tree in a simple, methodical way.

### Algorithm 1: Fetching a block from a file, part 1 of 2
#### k ←block size /4
#### if b < 12 then              ▷ index is in the i_block array
#### blockList ← i_block          ▷ Set up the array to read from

#### goto direct
#### else if b < 12 + k then       ▷ index is in first single indirect block
#### if i_block[12] = 0 then
#### return false
#### end if
#### FetchBlock(i_block[12], buƒ)     ▷ fetch SIB

#### blockList ← buƒ                   ▷ Set up the array to read from
#### b ← b − 12                          ▷ adjust b for nodes skipped over

#### goto direct
#### else if b < 12 + k + k^2 then       ▷ index is under first double indirect block
#### if i_block[13] = 0 then
#### return false
#### end if
#### FetchBlock(i_block[13], buƒ)       ▷ fetch DIB

#### blockList ← buƒ                    ▷ Set up the array to read from
#### b ← b − 12 − k                      ▷ adjust b for nodes skipped over

#### goto single
#### else                                ▷ index is under triple indirect block
#### if i_block[14] = 0 then
#### return false
#### end if
#### FetchBlock(i_block[14], buƒ)      ▷ fetch TIB

#### blockList ← buƒ                ▷ Set up the array to read from
#### b ← b − 12 − k − k^2          ▷ adjust b for nodes skipped over
#### end if
