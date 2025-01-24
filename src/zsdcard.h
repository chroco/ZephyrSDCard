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

#define MAX_PATH 128
#define SOME_FILE_NAME "some.dat"
#define SOME_DIR_NAME "some"
#define SOME_REQUIRED_LEN MAX(sizeof(SOME_FILE_NAME), sizeof(SOME_DIR_NAME))

class SDCard
{
	public:
		SDCard();
		~SDCard();

		int	doSDCardThings(void);
	private:
		static FATFS fat_fs;
		static fs_mount_t mp;
		
		static int lsdir(const char *);
		static const char *disk_mount_pt;// = DISK_MOUNT_PT;
		void probeCard(void);
		
#ifdef CONFIG_FS_SAMPLE_CREATE_SOME_ENTRIES
		bool create_some_entries(const char *);
#endif

};


#endif
