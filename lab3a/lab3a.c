// NAME: Jack Zhang,Edward Zhang
// EMAIL: dvorjackz@ucla.edu,edward123@gmail.com

#include <math.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include "ext2_fs.h"

#define OFFSET_SUPERBLOCK 1024
#define BLOCK_POINTER_SIZE 4

struct ext2_super_block struct_superblock;
struct ext2_group_desc struct_group;

int fd_img; // file descriptor of the image file
int fd_csv; // file descriptor of the csv file containing the summaries

int block_size;
int num_groups;
int offset_group;

void summary_superblock() {
  // read superblock information from image file
  pread(fd_img, &struct_superblock, sizeof(struct_superblock), OFFSET_SUPERBLOCK);
  
  block_size = 1024 << struct_superblock.s_log_block_size;
  // print superblock information
  printf("SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n",
	  struct_superblock.s_blocks_count,
	  struct_superblock.s_inodes_count,
	  block_size,
	  struct_superblock.s_inode_size,
	  struct_superblock.s_blocks_per_group,
	  struct_superblock.s_inodes_per_group,
	  struct_superblock.s_first_ino);
}

// reference: https://www.epochconverter.com/programming/c
void GMTFromEpochTime(int epochTime) {
    char* timestamp = malloc(sizeof(char) * 64); //random value
    time_t time = epochTime;
    struct tm* info = gmtime(&time);
    strftime(timestamp, 32, "%m/%d/%y %H:%M:%S", info);
    printf("%s,", timestamp);
    free(timestamp);
}

void printEntries(int entryOffset, int parentId) {
  struct ext2_dir_entry dir_entry;
  int i = entryOffset;
  while (i < entryOffset + block_size) {
    if (pread(fd_img, &dir_entry, sizeof(struct ext2_dir_entry), i) < 0) {
      fprintf(stderr, "Error: pread() failed\n");
    }
    if (dir_entry.inode != 0) {
      fprintf(stdout, "DIRENT,%d,%d,%d,%d,%d,'%s'\n", 
        parentId,
        i - entryOffset,
        dir_entry.inode,
        dir_entry.rec_len,
        dir_entry.name_len,
        dir_entry.name);
    }
    i += dir_entry.rec_len;
  }
}

void expand_directory(int inodeOffset, int parentId) {
  // for the first 12 direct pointers
  int direct_ptrs[12];
  int ptr_offset = inodeOffset + 40;
  if (pread(fd_img, direct_ptrs, BLOCK_POINTER_SIZE * 12, ptr_offset) < 0) {
    fprintf(stderr, "Error: pread() failed.\n");
  }

  for (int i = 0; i < 12; i++) {
    if (direct_ptrs[i] != 0) {
      // print all the directory entries that are in the block pointed to by the non-zero block pointer
      // direct_ptr is the id of the block
      printEntries(block_size * direct_ptrs[i], parentId);
    }
  }

  // for the 13th indirect pointer
  int indirect_ptr;
  ptr_offset = ptr_offset + BLOCK_POINTER_SIZE * 12;
  if (pread(fd_img, &indirect_ptr, BLOCK_POINTER_SIZE, ptr_offset) < 0) {
    fprintf(stderr, "Error: pread() failed.\n");
  }

  if (indirect_ptr != 0) {
    for (int i = 0; i < block_size; i+= BLOCK_POINTER_SIZE) {
      int temp_direct_ptr;
      if (pread(fd_img, &temp_direct_ptr, BLOCK_POINTER_SIZE, indirect_ptr * block_size + i) < 0) {
        fprintf(stderr, "Error: pread() failed.\n");
      }
      if (temp_direct_ptr != 0) {
        printEntries(temp_direct_ptr * block_size, parentId);
      }
    }
  }

  // for the 14th double indirect pointer
  int second_indirect_ptr;
  ptr_offset = ptr_offset + BLOCK_POINTER_SIZE;
  if (pread(fd_img, &second_indirect_ptr, BLOCK_POINTER_SIZE, ptr_offset) < 0) {
    fprintf(stderr, "Error: pread() failed.\n");
  }

  if (second_indirect_ptr != 0) {
    for (int i = 0; i < block_size; i+= BLOCK_POINTER_SIZE) {
      int temp_direct_ptr;
      if (pread(fd_img, &temp_direct_ptr, BLOCK_POINTER_SIZE, second_indirect_ptr * block_size + i) < 0) {
        fprintf(stderr, "Error: pread() failed.\n");
      }

      if (temp_direct_ptr != 0) {
        for (int j = 0; j < block_size; j+= BLOCK_POINTER_SIZE) {
          int temp_direct_ptr;
          if (pread(fd_img, &temp_direct_ptr, BLOCK_POINTER_SIZE, temp_direct_ptr * block_size + j) < 0) {
            fprintf(stderr, "Error: pread() failed.\n");
          }
          if (temp_direct_ptr != 0) {
            printEntries(temp_direct_ptr * block_size, parentId);
          }
        }
      }
    }
  }

  // for the 15th triple indirect pointer
  int third_indirect_ptr;
  ptr_offset = ptr_offset + BLOCK_POINTER_SIZE;
  if (pread(fd_img, &third_indirect_ptr, BLOCK_POINTER_SIZE, ptr_offset) < 0) {
    fprintf(stderr, "Error: pread() failed.\n");
  }

  if (third_indirect_ptr != 0) {
    for (int i = 0; i < block_size; i++) {
      int temp_second_indirect_ptr;
      if (pread(fd_img, &temp_second_indirect_ptr, BLOCK_POINTER_SIZE, third_indirect_ptr * block_size + i) < 0) {
        fprintf(stderr, "Error: pread() failed.\n");
      }

      if (temp_second_indirect_ptr != 0) {
        for (int j = 0; j < block_size; j+= BLOCK_POINTER_SIZE) {
          int temp_direct_ptr;
          if (pread(fd_img, &temp_direct_ptr, BLOCK_POINTER_SIZE, temp_second_indirect_ptr * block_size + j) < 0) {
            fprintf(stderr, "Error: pread() failed.\n");
          }

          if (temp_direct_ptr != 0) {
            for (int k = 0; k < block_size; k+= BLOCK_POINTER_SIZE) {
              int temp_direct_ptr;
              if (pread(fd_img, &temp_direct_ptr, BLOCK_POINTER_SIZE, temp_direct_ptr * block_size + k) < 0) {
                fprintf(stderr, "Error: pread() failed.\n");
              }
              if (temp_direct_ptr != 0) {
                printEntries(temp_direct_ptr * block_size, parentId);
              }
            }
          }
        }
      }
    }
  }
}

void search_for_indirect_pointers(int blockId, int ownerId, int level, int indirectDegree) {
  if (level == 0) {
    return;
  }
  int block_address = blockId * block_size;
  for (int i = 0; i < block_size; i += BLOCK_POINTER_SIZE) {
    int ptr;
    if (pread(fd_img, &ptr, BLOCK_POINTER_SIZE, block_address + i) < 0) {
      fprintf(stderr, "Error: pread() failed.\n");
    }
    if (ptr != 0) {
      int logical_offset;
      switch (indirectDegree) {
        case 1: 
          logical_offset = 12;
        break;
        case 2:
          logical_offset = 12 + block_size/BLOCK_POINTER_SIZE;
        break;
        case 3:
          logical_offset = 12 + block_size/BLOCK_POINTER_SIZE + (block_size/BLOCK_POINTER_SIZE)*(block_size/BLOCK_POINTER_SIZE);
        break;
        default:
          fprintf(stderr, "Only three degrees of indirectness allowed\n");
          return;
      }
      // i/4 because in the for loop we increment by BLOCK_POINTER_SIZE, and we need the index into the pointer table
      logical_offset += level == 1 ? i/4 : 0;
      printf("INDIRECT,%d,%d,%d,%d,%d\n", ownerId,level,logical_offset,blockId,ptr);
      search_for_indirect_pointers(ptr, ownerId, level - 1, indirectDegree);
    }
  }
}

void expand_indirect_pointers(int blockId, int ownerId, int indirectDegree) {
  search_for_indirect_pointers(blockId, ownerId, indirectDegree, indirectDegree);
}

void summary_inode(int itable, int n_inode) {
  struct ext2_inode struct_inode;
  int inode_offset = itable * block_size + n_inode * sizeof(struct_inode);
  pread(fd_img, &struct_inode, sizeof(struct_inode), inode_offset);
  
  if (struct_inode.i_links_count == 0 || struct_inode.i_mode == 0) {
    return;
  }

  // get file type and mode
  char file_type;
  if (struct_inode.i_mode & 0x8000) {
    file_type = 'f';
  }
  else if (struct_inode.i_mode & 0x4000) {
    file_type = 'd';
    expand_directory(inode_offset, n_inode + 1);
  }
  else if (struct_inode.i_mode & 0xA000) {
    file_type = 's';
  }
  else {
    file_type = '?';
  }
  int mode = struct_inode.i_mode & 0xFFF;

  // n_inode + 1 because we need n_inode to start at 0 for indexing purposes, but the actual numbering of inodes starts at 1
  printf("INODE,%d,%c,%o,%d,%d,%d,", 
	 n_inode + 1,
	 file_type,
	 mode,
	 struct_inode.i_uid,
	 struct_inode.i_gid,
	 struct_inode.i_links_count
	 );
  GMTFromEpochTime(struct_inode.i_ctime);
  GMTFromEpochTime(struct_inode.i_mtime);
  GMTFromEpochTime(struct_inode.i_atime);
  printf("%d,%d",
    struct_inode.i_size,
    struct_inode.i_blocks
    );

  // unless it is a symbolic link and has less than 60 characters, we explore the indirect pointers
  if (!(file_type == 's' && struct_inode.i_size < 60)) {
    for (int i = 0; i < 15; i++) {
      printf(",%d", struct_inode.i_block[i]);
    }
    printf("\n");
    if (struct_inode.i_block[12] != 0) {
      expand_indirect_pointers(struct_inode.i_block[12], n_inode + 1, 1);
    }
    if (struct_inode.i_block[13] != 0) {
      expand_indirect_pointers(struct_inode.i_block[13], n_inode + 1, 2);
    }
    if (struct_inode.i_block[14] != 0) {
      expand_indirect_pointers(struct_inode.i_block[14], n_inode + 1, 3);
    }
  }
  else {
    printf("\n");
  }
}

void summary_free_block_inode(int n_group, struct ext2_group_desc group) {
  // print free block and inode information
  pread(fd_img, &group, sizeof(group), offset_group + (n_group * sizeof(group)));
  int bitmap_block_num = group.bg_block_bitmap; // block number of block bitmap
  int bitmap_inode_num = group.bg_inode_bitmap; // block number of inode bitmap
  uint8_t *bm_block = malloc(block_size); // actual block bitmap
  uint8_t *bm_inode = malloc(block_size); // actual inode bitmap
  int itable = group.bg_inode_table; // used for inode information summary
  pread(fd_img, bm_block, block_size, bitmap_block_num * block_size);
  pread(fd_img, bm_inode, block_size, bitmap_inode_num * block_size);
  
  // block bitmap
  for (int j = 0; j < block_size / 8; j++) {
    for (int k = 0; k < 8; k++) {
      // each element of bm_block is 1 byte, so shift each byte and mask it with 0x1
      // to get the kth bit
      if (((bm_block[j] >> k) & 0x1) == 0) {
	printf("BFREE,%d\n", (n_group * struct_superblock.s_blocks_per_group) + (j * 8) + k + 1);
      }
    }
  }
  
  // easy way out to find number of inodes in group, since we know that we are only being tested with one block group
  int num_inodes_in_group = struct_superblock.s_inodes_count;

  for (int j = 0; j < num_inodes_in_group; j++) {
    uint8_t bit = j & 7;
    uint8_t byte = j / 8;
    if (((bm_inode[byte] >> bit) & 0x1) == 0) {
      printf("IFREE,%d\n", (n_group * struct_superblock.s_blocks_per_group) + j + 1);
    }
    else {
      summary_inode(itable, j);
    }
  }
  free(bm_block);
  free(bm_inode);
}

void summary_group() {
  // read group information
  offset_group = OFFSET_SUPERBLOCK + sizeof(struct_superblock);
  num_groups = // convert to double and ceil() the result to account for the remainder blocks
    ceil((double)struct_superblock.s_blocks_count / struct_superblock.s_blocks_per_group);

  for (int i = 0; i < num_groups; i++) {
    pread(fd_img, &struct_group, sizeof(struct_group), offset_group + (i * sizeof(struct_group)));
    int num_blocks = // account for remainder blocks in the last group
      i == num_groups - 1 ? struct_superblock.s_blocks_count % struct_superblock.s_blocks_per_group : struct_superblock.s_blocks_per_group;
    // print group information
    printf("GROUP,%d,%d,%d,%d,%d,%d,%d,%d\n",
	    i,
	    num_blocks,
	    struct_superblock.s_inodes_per_group,
	    struct_group.bg_free_blocks_count,
	    struct_group.bg_free_inodes_count,
	    struct_group.bg_block_bitmap,
	    struct_group.bg_inode_bitmap,
	    struct_group.bg_inode_table);
    // print bitmap information
    summary_free_block_inode(i, struct_group);
  }
}

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "Error: exactly one argument must be present - image file name\n");
    exit(1);
  }

  if ((fd_img = open(argv[1], O_RDONLY)) < 0) {
    fprintf(stderr, "Error: could not mount file\n");
    exit(1);
  }
  // print summary information
  summary_superblock();
  summary_group();
}
