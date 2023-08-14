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
 * @brief 判断簇号是否有效
 * 
 * @param cluster 
 * @return int 
 */
static int cluster_is_valid(cluster_t cluster) {
    return (cluster < FAT_CLUSTER_INVALID) && (cluster >= 0x2);
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
 * @brief 根据fat表中记录的簇链信息，获取当前簇号
 *          cblk的下一个簇的簇号
 * 
 * @param fat 
 * @param cblk 
 * @return int 
 */
static int cluster_get_next(fat_t *fat, cluster_t cblk) {
    //簇号无效
    if (!cluster_is_valid(cblk)) {
        return FAT_CLUSTER_INVALID;
    }

    //计算当前簇cblk在对应分区中的扇区号
    //fat表保存了簇链关系，
    int offset = cblk * sizeof(cluster_t);
    int sector = offset / fat->bytes_per_sector;
    //计算该簇在扇区中的偏移量
    int off_in_sector = offset % fat->bytes_per_sector;

    if (sector >= fat->tbl_sectors) {
        log_printf("cluster too big: %d\n", cblk);
        return FAT_CLUSTER_INVALID;
    }

    int err = cache_read_sector(fat, fat->tbl_start_sector + sector);
    if (err < 0) {
        return FAT_CLUSTER_INVALID;
    }

    return *(cluster_t *)(fat->fat_buffer + off_in_sector);
}

/**
 * @brief 将文件的读取位置pos移动move_bytes个字节
 * 
 * @param file 
 * @param fat 
 * @param move_bytes 
 * @param expand 
 * @return int 
 */
static int move_file_pos(file_t *file, 
    fat_t *fat, uint32_t move_bytes, int expand) {
        //判断移动pos后是否当前读取簇号是否需要更改
        uint32_t c_offset = file->pos % fat->cluster_bytes_size;
        if (c_offset + move_bytes >= fat->cluster_bytes_size) {
            //当前簇已读取完毕需更改当前簇号
            //通过fat的簇链获取当前簇的下一个簇
            cluster_t next = cluster_get_next(fat, file->cblk);
            if (next == FAT_CLUSTER_INVALID) {  //簇号无效
                return -1;
            }
            file->cblk = next;
        }

        file->pos += move_bytes;
        return 0;
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
 * @brief 从fat和file_item中读取文件信息到file当中
 * 
 * @param fat 
 * @param file
 * @param file_item 
 * @param p_index 
 */
static void read_from_diritem(fat_t *fat, 
    file_t *file, diritem_t *item, int index) {
        file->type = diritem_get_type(item);
        file->size = item->DIR_FileSize;
        file->pos = 0;
        file->p_index = index;
        file->sblk = (item->DIR_FstClusHI << 16) | item->DIR_FstClusLo;
        file->cblk = file->sblk;
}

/**
 * @brief 从当前目录项中获取文件名到dest中
 * 
 * @param diritem 
 * @param dest
 */
static void diritem_get_name(diritem_t *diritem, char *dest) {
    //file.c 存储在目录项中的形式为 "FILE    C  "
    //8字节的文件名，三字节的拓展名
    char *c = dest;
    char *ext = (char*)0;

    //8字节文件名 + '.' + 3字节拓展名，一共最大12字节空间
    kernel_memset(dest, 0, 12);
    for (int i = 0; i < 11; ++i) {
        if (diritem->DIR_Name[i] != ' ') {
            *c = diritem->DIR_Name[i];
            if (*c >= 'A' && *c <= 'Z') {
                *c -= ('A' - 'a');
            }
            c++;
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
 * @brief 根据根文件区的目录项匹配文件名
 * 
 * @param item 
 * @param dest 
 * @return int 
 */
static int diritem_name_match(diritem_t *item, const char *dest) {
    char buf[12];
    diritem_get_name(item, buf);
    return kernel_strncmp(buf, dest, 11) == 0;
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

/**
 * @brief fat文件系统打开对应文件
 * 
 * @param fs 
 * @param path 
 * @param file 
 * @return int 
 */
int fatfs_open(struct _fs_t *fs, const char *path, file_t *file) {
    //获取fat表信息
    fat_t *fat = (fat_t*)fs->data;

    //遍历读取根目录区的目录项,按路径path匹配对应目录项
    diritem_t *file_item = (diritem_t*)0;
    int p_index = -1;   //记录匹配到的目录项的索引
    for (int i = 0; i < fat->root_ent_cnt; ++i) {
        diritem_t * item = read_dir_entry(fat, i);
        if (item == (diritem_t *)0) {
            return -1;
        }

        if (item->DIR_Name[0] == DIRITEM_NAME_END) {
            break;
        }

        if (item->DIR_Name[0] == DIRITEM_NAEM_FREE) {
            continue;
        }

        //进行路径匹配
        if (diritem_name_match(item, path)) {
            file_item = item;
            p_index = i;
            break;
        }
    }

    //从目录项中读取文件信息到file结构中
    if (file_item) {
        read_from_diritem(fat, file, file_item, p_index);
        return 0;
    }

    return -1;
}


/**
 * @brief fat文件系统读取文件
 * 
 * @param buf 
 * @param size 
 * @param file 
 * @return int 
 */
int fatfs_read(char *buf, int size, file_t *file) {
    fat_t *fat = (fat_t*)file->fs->data;

    //修正读取字节数
    uint32_t nbytes = size;
    if (file->pos + nbytes > file->size) {
        nbytes = file->size - file->pos;
    }

    uint32_t total_read = 0;
  
    //读取nbytes个字节到buf中
    while (nbytes > 0) {
        //记录每次循环读取的字节数
        uint32_t curr_read = nbytes;
        //计算当前读取位置pos在当前读取的簇中的偏移量
        uint32_t cluster_offset = file->pos % fat->cluster_bytes_size;
        //计算文件在该分区中的起始扇区号
        //fat文件系统中，在分区的文件数据区中，簇号从2开始编号
        //[2],[3],[4]
        uint32_t start_sector = fat->data_start_sector + (file->cblk - 2) * fat->sec_per_cluster;

        //当前读取位置刚好在簇的开头，且读取大小为一个簇，直接进行整簇读取即可
        if (cluster_offset == 0 && nbytes == fat->cluster_bytes_size) {
            int err = dev_read(fat->fs->dev_id, start_sector, buf, fat->sec_per_cluster);
            if (err < 0) {
                return total_read;
            }

            curr_read = fat->cluster_bytes_size;
        } else {//当前读取内容需要进行跨簇读取
            if (cluster_offset + curr_read > fat->cluster_bytes_size) {
                curr_read = fat->cluster_bytes_size - cluster_offset;
            }

            //先将当前簇中的内容读取到fat_buffer中
            fat->curr_sector = start_sector;
            int err = dev_read(fat->fs->dev_id, start_sector, fat->fat_buffer, fat->sec_per_cluster);
            if (err < 0) {
                return total_read;
            }
            //再从fat_buffer中读取文件相关部分到buf中
            kernel_memcpy(buf, fat->fat_buffer + cluster_offset, curr_read);
        }
        buf += curr_read;
        nbytes -= curr_read;
        total_read += curr_read;

        //移动文件的读取位置file->pos
        int err = move_file_pos(file, fat, curr_read, 0);
        if (err < 0) {
            return total_read;
        }
    
    }
    

    return total_read;
}


int fatfs_write(char *buf, int size, file_t *file) {
    return 0;

}
void fatfs_close(file_t *file) {

}

/**
 * @brief fat文件系统对文件file的读取位置pos按dir方向偏移offset字节
 * 
 * @param file 
 * @param offset 
 * @param dir 
 * @return int 
 */
int fatfs_seek(file_t *file, uint32_t offset, int dir) {
    if (dir != 0) {
        return -1;
    }

    fat_t *fat = (fat_t *)file->fs->data;
    cluster_t current_cluster = file->cblk;
    uint32_t curr_pos = 0;
    uint32_t offset_to_move = offset;

    //进行偏移处理
    while (offset_to_move) {
        //计算文件pos在当前簇中的偏移量
        uint32_t c_offset = curr_pos % fat->cluster_bytes_size;
        //初始化此次循环预备移动的字节量
        uint32_t curr_move = offset_to_move;

        if (c_offset + curr_move < fat->cluster_bytes_size) {
            //此处循环移动后，pos还在当前簇中
            curr_pos += curr_move;
            break;
        }

        //此次循环移动后pos将移动到下一个簇
        //所以先将本簇中的剩余字节量偏移
        curr_move = fat->cluster_bytes_size - c_offset;
        curr_pos += curr_move;
        offset_to_move -= curr_move;

        //获取下一个簇号
        current_cluster = cluster_get_next(fat, current_cluster);
        if (!cluster_is_valid(current_cluster)) {
            return -1;
        }

    }

    file->cblk = current_cluster;
    file->pos = curr_pos;
    
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