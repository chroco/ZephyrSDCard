#ifndef _ZSDCARD_H_
#define _ZSDCARD_H_

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/storage/disk_access.h>
#include <zephyr/logging/log.h>
#include <zephyr/fs/fs.h>
#include <zephyr/drivers/sdhc.h>

#if defined(CONFIG_FAT_FILESYSTEM_ELM)

#include <ff.h>

#if defined(CONFIG_DISK_DRIVER_MMC)
#define DISK_DRIVE_NAME "SD2"
#else
#define DISK_DRIVE_NAME "SD"
#endif //CONFIG_DISK_DRIVER_MMC

#define DISK_MOUNT_PT "/" DISK_DRIVE_NAME ":"

#elif defined(CONFIG_FILE_SYSTEM_EXT2)

#include <zephyr/fs/ext2.h>

#define DISK_DRIVE_NAME "SD"
#define DISK_MOUNT_PT "/ext"

#endif // CONFIG_FAT_FILESYSTEM_ELM

#if defined(CONFIG_FAT_FILESYSTEM_ELM)
#define FS_RET_OK FR_OK
#else
#define FS_RET_OK 0
#endif

#define TEST_FILE "TEST.DAT"
#define MAX_FILENAME 64
#define MAX_PATH MAX_FILENAME + 64
#define BASE_NAME "yarrrr"
#define DIR_NAME BASE_NAME
#define FILE_NAME BASE_NAME".dat"
#define REQUIRED_LEN MAX(sizeof(FILE_NAME), sizeof(DIR_NAME))

#define FILE_BUFFER_LENGTH 128

class SDCard
{
	public:
		SDCard();
		~SDCard();

		int	testWrite(void);
		int mount(void);
		int unmount(void);
		int write(char *, const char *);
		int read(char *, const char *);
		int search(char *, const char *);
//ssize_t	fs_read (struct fs_file_t *zfp, void *ptr, size_t size)
		int write(fs_file_t *, char *, const char *);
		int lsdir(const char *);
		int mkdir(const char *);
	private:
		void probe(void);
		
		static FATFS fat_fs;
		static fs_mount_t mp;
		static const char *disk_mount_pt;
};

#endif
