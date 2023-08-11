/**
 * @file fatfs.c
 * @author kbpoyo (kbpoyo@qq.com)
 * @brief fat16文件系统
 * @version 0.1
 * @date 2023-08-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "fs/fatfs/fatfs.h"
#include "fs/file.h"
#include "fs/fs.h"
#include "dev/dev.h"
#include "tools/log.h"
#include "core/memory.h"
#include "tools/klib.h"
#include <sys/fcntl.h>

/**
 * @brief 从当前目录项中获取文件名到dest中
 * 
 * @param diritem 
 * @param dest
 */
void diritem_get_name(diritem_t *diritem, char *dest) {
    //file.c 存储在目录项中的形式为 "FILE    C  "
    //8字节的文件名，三字节的拓展名
    char *c = dest;
    char *ext = (char*)0;

    //8字节文件名 + '.' + 3字节拓展名，一共最大12字节空间
    kernel_memset(dest, 0, 12);
    for (int i = 0; i < 11; ++i) {
        if (diritem->DIR_Name[i] != ' ') {
            *(c++) = diritem->DIR_Name[i];
        }

        //读完8字节的文件名，用ext记录".拓展名""
        if (i == 7) {
            ext = c;
            *(c++) = '.';
        }
    }

    //文件无拓展名，将之前写入的'.'设置为'\0'
    if (ext && (ext[1] == '\0')) {
        ext[0] = '\0';
    }

}

/**
 * @brief 以缓存的方式读取扇区
 * 
 * @param fat 
 * @param sector 
 * @return int 
 */
static int cache_read_sector(fat_t *fat, int sector) {
    //要读扇区已被缓存，直接返回即可
    if (sector == fat->curr_sector) {
        return 0;
    }

    //读取信的扇区，并记录扇区号
    int cnt = dev_read(fat->fs->dev_id, sector, fat->fat_buffer, 1);
    if (cnt == 1) {
        fat->curr_sector = sector;
        return 0;
    }

    return -1;
}

/**
 * @brief 从根目录区读取索引为dir_index的目录项
 * 
 * @param fat 
 * @param dir_index 
 * @return diritem_t* 
 */
static diritem_t * read_dir_entry(fat_t *fat, int dir_index) {
    if (dir_index < 0 || dir_index >= fat->root_ent_cnt) {
        return (diritem_t*)0;
    }

    //计算该目录项所在根目录区的扇区的扇区号
    int offset = dir_index * sizeof(diritem_t);
    int sector = fat->root_start_sector + offset / fat->bytes_per_sector;
    int err = cache_read_sector(fat, sector);
    if (err < 0) {
        return (diritem_t*)0;
    }

    //计算出该目录项的起始地址并返回
    return (diritem_t*)(fat->fat_buffer + offset % fat->bytes_per_sector);
}

/**
 * @brief 从根目录项中获取该项的文件类型
 * 
 * @param diritem 
 * @return file_type_t 
 */
file_type_t diritem_get_type(diritem_t *diritem) {
    file_type_t type = FILE_UNKNOWN;

    if (diritem->DIR_Attr & 
    (DIRITEM_ATTR_VOLUME_ID 
    | DIRITEM_ATTR_SYSTEM 
    | DIRITEM_ATTR_HIDDEN)) {   //该项是卷标，隐藏或系统文件时直接跳过
        return FILE_UNKNOWN;
    }

    //该项为LONG_NAME项时也跳过
    if ((diritem->DIR_Attr & DIRITEM_ATTR_LONG_NAME) == DIRITEM_ATTR_LONG_NAME) {
        return FILE_UNKNOWN;

    }


    return diritem->DIR_Attr & DIRITEM_ATTR_DIRECTORY ? FILE_DIR : FILE_NORMAL;
}



/**
 * @brief 挂载fat文件系统
 * 
 * @param fs 
 * @param major 
 * @param minor 
 * @return int 
 */
int fatfs_mount(struct _fs_t *fs, int major, int minor) {

    //打开对应设备 即对应磁盘的对应分区
    int dev_id = dev_open(major, minor, (void *)0);
    if (dev_id < 0) {
        log_printf("open disk failed, major: %x, minor: %x\n", major, minor);
        goto mount_failed;
    }

    //分配一页来作为dbr区域的缓冲区
    dbr_t *dbr = (dbr_t *)memory_alloc_page();
    if (!dbr) {
        log_printf("mount failed: can't alloc buf\n");
        goto mount_failed;
    }


    //读取该分区的0号扇区，获取dbr的信息
    int cnt = dev_read(dev_id, 0, (char *)dbr, 1);
    if (cnt < 1) {
        log_printf("read dbr failed\n");
        goto mount_failed;
    }

    //解析fat表的信息到文件系统中
    fat_t *fat = &fs->fat_data;
    fat->bytes_per_sector = dbr->BPB_BytsPerSec;
    //因为保留区dbr从0扇区开始，且fat表紧邻dbr区
    //所以fat表的起始扇区，也就是保留区dbr的扇区总数
    fat->tbl_start_sector = dbr->BPB_RsvdSecCnt;
    fat->tbl_sectors = dbr->BPB_FATSz16;
    fat->tbl_cnt = dbr->BPB_NumFATs;
    fat->sec_per_cluster = dbr->BPB_SecPerClus;
    fat->root_ent_cnt = dbr->BPB_RootEntCnt;
    fat->root_start_sector = fat->tbl_start_sector + fat->tbl_sectors * fat->tbl_cnt;
    fat->data_start_sector = fat->root_start_sector + fat->root_ent_cnt * 32 / dbr->BPB_BytsPerSec;
    fat->cluster_bytes_size = fat->sec_per_cluster * dbr->BPB_BytsPerSec;
    fat->fat_buffer = (uint8_t*)dbr;
    fat->fs = fs;
    fat->curr_sector = -1;

    if (fat->tbl_cnt != 2) {    //fat表数量一般为2， 不为2则出错
        log_printf("%s: fat table error: major: %x, minor: %x\n", major, minor);
        goto mount_failed;
    }

    if (kernel_strncmp((char*)(dbr->BS_FilSysType), "FAT16", 5) != 0) { //不是FAT16文件系统，出错
        log_printf("not a fat filesystem: major: %x, minor: %x\n", major, minor);
        goto mount_failed;

    }

    fs->type = FS_FAT16;
    fs->data = &fs->fat_data;
    fs->dev_id = dev_id;

    return 0;

mount_failed:
    if (dbr) {
        memory_free_page((uint32_t)dbr);
    }

    dev_close(dev_id);

    return -1;

} 


/**
 * @brief 卸载fat文件系统
 * 
 * @param fs 
 */
void fatfs_unmount(struct _fs_t *fs) {
    fat_t * fat = (fat_t *)fs->data;
    dev_close(fs->dev_id);

    memory_free_page((uint32_t)fat->fat_buffer);
}
int fatfs_open(struct _fs_t *fs, const char *path, file_t *file) {


    return 0;
}
int fatfs_read(char *buf, int size, file_t *file) {
    return 0;

}
int fatfs_write(char *buf, int size, file_t *file) {
    return 0;

}
void fatfs_close(file_t *file) {

}
int fatfs_seek(file_t *file, uint32_t offset, int dir) {
    return 0;

}
int fatfs_stat(file_t *file, struct stat *st) {
    return 0;

}

/**
 * @brief 打开目录
 * 
 * @param fs 
 * @param name 
 * @param dir 
 * @return int 
 */
int fatfs_opendir(struct _fs_t *fs, const char *name, DIR *dir) {
    dir->index = 0;
    return 0;
}

/**
 * @brief 遍历目录
 * 
 * @param fs 
 * @param dir 
 * @param dirent 
 * @return int 
 */
int fatfs_readdir(struct _fs_t *fs, DIR *dir, struct dirent *dirent) {
    //获取当前fat文件系统的fat表信息
    fat_t *fat = (fat_t*)fs->data;

    while (dir->index < fat->root_ent_cnt) {
        diritem_t *item = read_dir_entry(fat, dir->index);
        if (item == (diritem_t *)0) {
            return -1;
        }

        //已经遍历到末尾项
        if (item->DIR_Name[0] == DIRITEM_NAME_END) {
            break;
        }

        //该目录项有效,获取目录项信息到dirent中
        if (item->DIR_Name[0] != DIRITEM_NAEM_FREE) {
            file_type_t type = diritem_get_type(item);
            if ((type == FILE_NORMAL) || (type == FILE_DIR)) {
                dirent->size = item->DIR_FileSize;
                dirent->type = type;
                diritem_get_name(item, dirent->name);

                //记录目录项在该目录中的索引
                dirent->index = dir->index++;
                return 0;
            }
        }

        //该目录项无效，继续获取下一个目录项
        dir->index++;
    }


    return -1;
}

/**
 * @brief 关闭目录
 * 
 * @param fs 
 * @param dir 
 * @return int 
 */
int fatfs_closedir(struct _fs_t *fs, DIR *dir) {
 return 0;
}


//将fat文件系统的操作函数抽象给顶层文件系统使用
//类似于多态处理
fs_op_t fatfs_op = {
    .mount = fatfs_mount,
    .unmount = fatfs_unmount,
    .open = fatfs_open,
    .read = fatfs_read,
    .write = fatfs_write,
    .close = fatfs_close,
    .seek = fatfs_seek,
    .stat = fatfs_stat,
    .opendir = fatfs_opendir,
    .readdir = fatfs_readdir,
    .closedir = fatfs_closedir,
};