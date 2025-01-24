#include "zsdcard.h"

SDCard::SDCard()
{

}

SDCard::~SDCard()
{

}

#if defined(CONFIG_FAT_FILESYSTEM_ELM)

FATFS SDCard::fat_fs;

fs_mount_t SDCard::mp = {
	.type = FS_FATFS,
	.fs_data = &fat_fs,
};

#elif defined(CONFIG_FILE_SYSTEM_EXT2)

fs_mount_t SDCard::mp = {
	.type = FS_EXT2,
	.flags = FS_MOUNT_FLAG_NO_FORMAT,
	.storage_dev = (void *)DISK_DRIVE_NAME,
	.mnt_point = "/ext",
};

#endif // CONFIG_FAT_FILESYSTEM_ELM

LOG_MODULE_REGISTER(main);

int SDCard::mkdir(const char *path)
{
	int ret = fs_mkdir(path);
	
	if (ret != 0)
	{
		LOG_ERR("Failed to create dir %s, [%d]", path, ret);
	}

	return 0;
}

const char *SDCard::disk_mount_pt = DISK_MOUNT_PT;

void SDCard::probe(void)
{
	// raw disk i/o 
	do {
		static const char *disk_pdrv = DISK_DRIVE_NAME;
		uint64_t memory_size_mb;
		uint32_t block_count;
		uint32_t block_size;

		if (disk_access_ioctl(disk_pdrv, DISK_IOCTL_CTRL_INIT, NULL) != 0) 
		{
			LOG_ERR("Storage init ERROR!");
			break;
		}

		if (disk_access_ioctl(disk_pdrv, DISK_IOCTL_GET_SECTOR_COUNT, &block_count)) 
		{
			LOG_ERR("Unable to get sector count");
			break;
		}
		
		LOG_INF("Block count %u", block_count);

		if (disk_access_ioctl(disk_pdrv, DISK_IOCTL_GET_SECTOR_SIZE, &block_size)) 
		{
			LOG_ERR("Unable to get sector size");
			break;
		}
		printk("Sector size %u\n", block_size);

		memory_size_mb = (uint64_t)block_count * block_size;
		printk("Memory Size(MB) %u\n", (uint32_t)(memory_size_mb >> 20));

		if (disk_access_ioctl(disk_pdrv, DISK_IOCTL_CTRL_DEINIT, NULL) != 0) 
		{
			LOG_ERR("Storage deinit ERROR!");
			break;
		}
	} while (0);
}

int SDCard::mount(void)
{
	mp.mnt_point = disk_mount_pt;

	int res = fs_mount(&mp);

	if (res == FS_RET_OK)
	{
		printk("Disk mounted.\n");
		// Try to unmount and remount the disk 
		res = fs_unmount(&mp);
		if (res != FS_RET_OK)
		{
			printk("Error unmounting disk\n");
			return res;
		}
		res = fs_mount(&mp);
		if (res != FS_RET_OK)
		{
			printk("Error remounting disk\n");
			return res;
		}
	}
	else 
	{
		printk("Error mounting disk.\n");
	}

	return 0;
}

int SDCard::unmount(void)
{
	fs_unmount(&mp);

	return 0;
}

int SDCard::testWrite(void)
{
	char file_data_buffer[FILE_BUFFER_LENGTH] = {0};
	
	probe();

	mount();

	sprintf(file_data_buffer, "weeeeeeeeeeeeeeeeeee!\n");
	write(file_data_buffer, "fml.dat");
	
	lsdir(disk_mount_pt);
	
	unmount();

	return 0;
}

int SDCard::write(char *file_data_buffer, const char *file_name)
{
	fs_file_t file;

	return write(&file, file_data_buffer, file_name);
}

int SDCard::write(fs_file_t *file, char *file_data_buffer, const char *file_name)
{
	char path[MAX_PATH] = {0};
	unsigned int base = strlen(disk_mount_pt);

	fs_file_t_init(file);

	if (base >= (sizeof(path) - REQUIRED_LEN))
	{
		LOG_ERR("Not enough concatenation buffer to create file paths");
		return 1;
	}

	LOG_INF("Creating some dir entries in %s", disk_mount_pt);
	strncpy(path, disk_mount_pt, sizeof(path));

	path[base] = '/';
	++base;
	strcat(&path[base], file_name);

	if (fs_open(file, path, FS_O_WRITE | FS_O_CREATE) != 0)
	{
		LOG_ERR("Failed to create file %s", path);
		return 1;
	}

	int ret = fs_write(file, file_data_buffer, FILE_BUFFER_LENGTH);
	
	fs_close(file);

	return ret;
}

int mkdirdir(const char *)
{

	return 0;
}

int SDCard::lsdir(const char *path)
{
	int res;
	struct fs_dir_t dirp;
	static struct fs_dirent entry;
	int count = 0;

	fs_dir_t_init(&dirp);

	// Verify fs_opendir()
	res = fs_opendir(&dirp, path);
	if (res) 
	{
		printk("Error opening dir %s [%d]\n", path, res);
		
		return res;
	}

	printk("\nListing dir %s ...\n", path);
	
	for (;;) 
	{
		// Verify fs_readdir() 
		res = fs_readdir(&dirp, &entry);

		// entry.name[0] == 0 means end-of-dir 
		if (res || entry.name[0] == 0) 
		{
			break;
		}

		if (entry.type == FS_DIR_ENTRY_DIR) 
		{
			printk("[DIR ] %s\n", entry.name);
		} 
		else 
		{
			printk("[FILE] %s (size = %zu)\n", entry.name, entry.size);
		}

		++count;
	}

	// Verify fs_closedir() 
	res = fs_closedir(&dirp);
	if (res == 0) 
	{
		res = count;
		printk("Closing dir %s [%d]\n", path, res);
	}

	return res;
}
