#include <stdio.h>
#include <math.h>

#include <ext2fs/ext2fs.h>

int diriter(struct ext2_dir_entry *dirent, int offset, int blocksize, char* buf, void* private) {
	ext2_filsys fs = (ext2_filsys) private;

	printf("name: %d; off: %d; bs: %d;\n", dirent->rec_len, offset, blocksize);

	return 1;
	if (strcmp(dirent->name, ".") != 0 && strcmp(dirent->name, "..") != 0) {
		ext2_file_t file;
		errcode_t error = ext2fs_file_open(fs, dirent->inode, 0, &file);
		if (error) {
			printf("file_open error %ld\n", error);
			return 0;
		}

		char path[256];
		snprintf(path, sizeof(path), "/home/alextusinean/Desktop/ext2analyzer/inode.%d", dirent->inode);
		FILE* outfile = fopen(path, "w");

		char buffer[4096];
		int size = ext2fs_file_get_size(file);
		int got = -1;

		while (size > 0) {
			ext2_off_t _pos;
			error = ext2fs_file_lseek(
				file,
				got == -1 ? 0 : got,
				got == -1 ? EXT2_SEEK_SET : EXT2_SEEK_CUR,
				&_pos
			);

			if (error) {
				printf("file_lseek error %ld\n", error);
				return 0;
			}

			error = ext2fs_file_read(file, buffer, sizeof(buffer), &got);
			if (error) {
				printf("file_read error %ld\n", error);
				return 0;
			}

			fwrite(buffer, sizeof(char), got, outfile);
			size -= got;
		}

		fclose(outfile);
		ext2fs_file_close(file);
	}

	return 1;
}

void main() {
	ext2_filsys fs;
	errcode_t error = ext2fs_open("/home/alextusinean/Desktop/mikroarm.img1", 0, 0, 4096, unix_io_manager, &fs);
	if (error) {
		printf("open error %ld\n", error);
		return;
	}

	// blk64_t blockCount = ext2fs_blocks_count(fs->orig_super);
	// char* blockBitmap = malloc(ceil(blockCount / 8));

	// error = ext2fs_read_block_bitmap(fs);
	// if (error) {
	// 	printf("read_block_bitmap error %ld\n", error);
	// 	return;
	// }

	// error = ext2fs_get_block_bitmap_range(fs->block_map, ext2fs_get_block_bitmap_start(fs->block_map), blockCount, blockBitmap);
	// if (error) {
	// 	printf("get_block_bitmap_range error %ld\n", error);
	// 	return;
	// }

	// free(blockBitmap);



	ext2fs_dir_iterate(fs, 11, DIRENT_FLAG_INCLUDE_EMPTY, NULL, diriter, fs);

	// ext2_inode_scan scan;
	// ext2fs_open_inode_scan(fs, 0, &scan);

	// ext2_ino_t ino;
	// struct ext2_inode inode;

	// error = ext2fs_get_next_inode(scan, &ino, &inode);
	// if (error) {
	// 	printf("first_next_inode error %ld\n", error);
	// 	return;
	// }

	// while (ino) {
	// 	if (ino == 2)
	// 		break;

	// 	error = ext2fs_get_next_inode(scan, &ino, &inode);
	// 	if (error) {
	// 		printf("next_inode error %ld\n", error);
	// 		return;
	// 	}
	// }

	// ext2fs_close_inode_scan(scan);

	ext2fs_close(fs);
}

