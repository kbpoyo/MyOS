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
 * @brief 将普通文件名src转换成系统文件名dest
 *         系统文件名是8个字节文件名 + 3个字节拓展名
 *         拓展名不为空，则8个字节的最后一个字节为'.'
 * 
 * @param dest 
 * @param src 
 */
static void to_sfn(char* dest, const char* src) {
    kernel_memset(dest, ' ', SFN_LEN);

    // 不断生成直到遇到分隔符和写完缓存
    char * curr = dest;
    char * end = dest + SFN_LEN;
    while (*src && (curr < end)) {
        char c = *src++;

        switch (c) {
        case '.':       // 隔附，跳到扩展名区，不写字符
            curr = dest + 8;
            break;
        default:
            if ((c >= 'a') && (c <= 'z')) {
                c = c - 'a' + 'A';
            }
            *curr++ = c;
            break;
        }
    }
}


/**
 * @brief 初始化一个根目录区的目录项
 * 
 * @param item 
 * @param attr 
 * @param path 
 * @return int 
 */
static int diritem_init(diritem_t *item, uint8_t attr, const char *name) {
    to_sfn((char *)item->DIR_Name, name);
    item->DIR_FstClusHI = (uint16_t)(FAT_CLUSTER_INVALID >> 16);
    item->DIR_FstClusLo = (uint16_t)(FAT_CLUSTER_INVALID & 0xffff);
    item->DIR_FileSize = 0;
    item->DIR_Attr = attr;

    //以下数据本系统直接忽略
    //由kbos完成当天在windows磁盘管理器
    //创建的文件得到
    item->DIR_NTRes = 0x18;
    item->DIR_CrtDate = 0x570f;
    item->DIR_CrtTime = 0x86d8;
    item->DIR_WrtDate = 0x570f;
    item->DIR_WrtTime = 0x86e9;
    item->DIR_LastAccDate = 0x570f;
    item->DIR_CrtTimeTeenth = 0x51;

    return 0;
}

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
 * @brief 以缓存的方式读取扇区sector到fat_buffer中
 * 
 * @param fat 
 * @param sector 
 * @return int 
 */
static int fat_read_sector(fat_t *fat, int sector) {
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
 * @brief 以将fat_buffer写回到扇区sector中
 * 
 * @param fat 
 * @param sector 
 * @return int 
 */
static int fat_write_sector(fat_t *fat, int sector) {
    int cnt = dev_write(fat->fs->dev_id, sector, fat->fat_buffer, 1);

    return (cnt == 1) ? 0 : -1;
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

    int err = fat_read_sector(fat, fat->tbl_start_sector + sector);
    if (err < 0) {
        return FAT_CLUSTER_INVALID;
    }

    return *(cluster_t *)(fat->fat_buffer + off_in_sector);
}


/**
 * @brief 设置fat表种簇号start对应的下一个簇号为next
 * 
 * @param fat 
 * @param start 
 * @param next 
 */
static int cluster_set_next(fat_t *fat, cluster_t start, cluster_t next) {
     //簇号无效
    if (!cluster_is_valid(start)) {
        return FAT_CLUSTER_INVALID;
    }

    //计算当前簇cblk在对应分区中的扇区号
    //fat表保存了簇链关系，
    int offset = start * sizeof(cluster_t);
    int sector = offset / fat->bytes_per_sector;
    //计算该簇在扇区中的偏移量
    int off_in_sector = offset % fat->bytes_per_sector;

    if (sector >= fat->tbl_sectors) {
        log_printf("cluster too big: %d\n", start);
        return FAT_CLUSTER_INVALID;
    }

    //将该簇号所在的扇区读到缓冲区fat_buffer中
    int err = fat_read_sector(fat, fat->tbl_start_sector + sector);
    if (err < 0) {
        return FAT_CLUSTER_INVALID;
    }

    //将缓冲区中该表项的值设未next
    *(cluster_t *)(fat->fat_buffer + off_in_sector) = next;
    //再将缓冲区覆盖到磁盘对应区域
    for (int i = 0; i < fat->tbl_cnt; ++i) {
        err = fat_write_sector(fat, fat->tbl_start_sector + sector);
        if (err < 0) {
            log_printf("write cluster failed.\n");
            return -1;
        }

        //偏移一个fat表的大小，将相邻的第二个fat表的对应位置也清空
        sector += fat->tbl_sectors;
    }

    return 0;

}


/**
 * @brief 清除fat表中的簇链关系
 * 
 * @param fat 
 * @param start 
 */
void cluster_free_chain(fat_t *fat, cluster_t start) {
    //链式清空
    while (cluster_is_valid(start)) {
        cluster_t next = cluster_get_next(fat, start);
        cluster_set_next(fat, start, CLUSTER_FAT_FREE);
        start = next;
    }
}


/**
 * @brief 在fat表中分配空闲簇，并建立簇链关系
 * 
 * @param fat 
 * @param cnt 
 * @return cluster_t 
 */
static cluster_t cluster_alloc_free(fat_t *fat, int cnt) {
    cluster_t start = FAT_CLUSTER_INVALID;
    cluster_t pre = FAT_CLUSTER_INVALID;
    cluster_t curr;
    
    //计算一个fat表中有多少个簇号
    int c_total = fat->tbl_sectors * fat->bytes_per_sector / sizeof(cluster_t);
    //0,1簇号未使用
    for (curr = 2; cnt && (curr < c_total); ++curr) {
        cluster_t free = cluster_get_next(fat, curr);
        //当前簇curr没有链接关系，可供分配
        if (free == CLUSTER_FAT_FREE) {
            if (!cluster_is_valid(start)) {
                //链头还未分配，先分配链头
                start = curr;
            }

            if (cluster_is_valid(pre)) {
                //前驱簇号有效且刚分配，则设置curr为pre的后继
                int err = cluster_set_next(fat, pre, curr);
                if (err < 0) {
                    cluster_free_chain(fat, start);
                    return FAT_CLUSTER_INVALID;
                }
            }
            
            //分配成功一个簇，更新pre和cnt
            pre = curr;
            cnt--;
        }
    }

    //设置最后一个簇的后继为无效簇标志
    if (cnt == 0) {
        int err = cluster_set_next(fat, pre, FAT_CLUSTER_INVALID);
        if (err < 0) {
            cluster_free_chain(fat, start);
            return FAT_CLUSTER_INVALID;
        }
    }

    return start;
}



/**
 * @brief 拓展文件空间大小
 * 
 * @param file 
 * @param inc_size 
 * @return int 
 */
static int expand_file(file_t *file, int inc_bytes) {
    fat_t *fat = (fat_t*)file->fs->data;
    
    //计算文件需要拓展多少个簇
    int  cluster_cnt;
    if (file->size % fat->cluster_bytes_size == 0) {
        //文件原始大小刚好装满已分配的最后一簇
        //以簇为单位，计算需要拓展的簇的数量
        cluster_cnt = up2(inc_bytes, fat->cluster_bytes_size) / fat->cluster_bytes_size;
    } else {
        //文件原始分配的最后一个簇空间还未装满
        //计算最后一个簇的空间余量
        int cluster_free = fat->cluster_bytes_size - (file->size % fat->cluster_bytes_size);
        if (cluster_free >= inc_bytes) {
            //余量足够支持需要拓展的字节量，则不需要分配新簇
            return 0;
        }

        //余量不足以支持拓展字节量，需要分配新簇
        cluster_cnt = up2(inc_bytes - cluster_free, fat->cluster_bytes_size) / fat->cluster_bytes_size;
    }

    //分配空闲簇，即在fat表中建立簇链关系
    cluster_t start = cluster_alloc_free(fat, cluster_cnt);
    if (!cluster_is_valid(start)) {
        log_printf("no cluster for file write.\n");
        return -1;
    }

    //文件还没有原始数据，则直接用分配的簇链初始化文件
    if (!cluster_is_valid(file->sblk)) {
        file->sblk = file->cblk = start;
    } else {
        //文件已有原始数据，将新分配的簇链开头设为文件当前簇cblk的下一个簇
        int err = cluster_set_next(fat, file->cblk, start);
        if (err < 0) {
            return -1;
        }
    }

    return 0;

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
            if (next == FAT_CLUSTER_INVALID && expand) {  
                //当前簇cblk为簇链的最后一个簇，需要分配一个新簇再移动pos
                int err = expand_file(file, fat->cluster_bytes_size);
                if (err < 0) {
                    return -1;
                }
                //获取新分配的链尾簇
                next = cluster_get_next(fat, file->cblk);
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
 * @brief 向根目录区写入索引为dir_index的目录项
 * 
 * @param fat 
 * @param dir_index 
 * @return diritem_t* 
 */
static int write_dir_entry(fat_t *fat, diritem_t *item, int dir_index) {
    if (dir_index < 0 || dir_index >= fat->root_ent_cnt) {
        return -1;
    }

    //计算该目录项所在根目录区的扇区的扇区号
    int offset = dir_index * sizeof(diritem_t);
    int sector = fat->root_start_sector + offset / fat->bytes_per_sector;
    int err = fat_read_sector(fat, sector);
    if (err < 0) {
        return -1;
    }

    //将该目录项拷贝到扇区缓存的指定对应位置
    kernel_memcpy(fat->fat_buffer + offset % fat->bytes_per_sector, item, sizeof(diritem_t));

    //将扇区重新覆盖到磁盘上
    fat_write_sector(fat, sector);
    
    return 0;
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
    int err = fat_read_sector(fat, sector);
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

        //记录所遍历到的目录项的索引
        p_index = i;

        if (item->DIR_Name[0] == DIRITEM_NAME_END) {
            continue;
        }

        if (item->DIR_Name[0] == DIRITEM_NAEM_FREE) {
            continue;
        }

        //进行路径匹配
        if (diritem_name_match(item, path)) {
            file_item = item;
            break;
        }
    }

    
    if (file_item) {//从目录项中读取文件信息到file结构中
        read_from_diritem(fat, file, file_item, p_index);

        if (file->mode & O_TRUNC) { //以截断模式打开文件，需清空文件
            cluster_free_chain(fat, file->sblk);
            file->cblk = file->sblk = FAT_CLUSTER_INVALID;
            file->size = 0;
        }
        return 0;
    } else if ((file->mode & O_CREAT) && p_index >= 0){//创建文件模式下未找到对应的目录项，创建新一个文件
        //初始化一个目录项信息
        diritem_t item;
        diritem_init(&item, DIRITEM_ATTR_ARCHIVE, path);

        //将目录项信息写入到根目录区
        int err = write_dir_entry(fat, &item, p_index);
        if (err < 0) {
            log_printf("create file failed\n");
            return -1;
        }

        //将目录项信息读到file结构中
        read_from_diritem(fat, file, &item, p_index);
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
        //[0] = 0xfff8, [1] = 0xffff 固定值
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


/**
 * @brief fat文件系统写入文件
 * 
 * @param buf 
 * @param size 
 * @param file 
 * @return int 
 */
int fatfs_write(char *buf, int size, file_t *file) {

    fat_t *fat = (fat_t*)file->fs->data;

    //文件空间大小不足以写入，需要拓展空间
    if (file->pos + size > file->size) {
        //计算文件当前空间大小与待写入的大小的差值
        int inc_size = file->pos + size - file->size;
        //拓展文件大小
        int err = expand_file(file, inc_size);
        if (err < 0) {
            return 0;
        }
    }

    uint32_t nbytes = size;
    uint32_t total_write = 0;
    while (nbytes > 0) {
        //记录每次循环读取的字节数
        uint32_t curr_write = nbytes;
        //计算当前读取位置pos在当前写入的簇中的偏移量
        uint32_t cluster_offset = file->pos % fat->cluster_bytes_size;
        //计算文件在该分区中的起始扇区号
        //fat文件系统中，在分区的文件数据区中，簇号从2开始编号
        //[2],[3],[4]
        uint32_t start_sector = fat->data_start_sector + (file->cblk - 2) * fat->sec_per_cluster;

        //当前写入位置刚好在簇的开头，且大小为一个簇，直接进行整簇写入即可
        if (cluster_offset == 0 && nbytes == fat->cluster_bytes_size) {
            int err = dev_write(fat->fs->dev_id, start_sector, buf, fat->sec_per_cluster);
            if (err < 0) {
                return total_write;
            }

            curr_write = fat->cluster_bytes_size;
        } else {//当前写入内容需要进行跨簇写入
            if (cluster_offset + curr_write > fat->cluster_bytes_size) {
                curr_write = fat->cluster_bytes_size - cluster_offset;
            }

            //先将当前簇中的内容读取到fat_buffer中
            fat->curr_sector = start_sector;
            int err = dev_read(fat->fs->dev_id, start_sector, fat->fat_buffer, fat->sec_per_cluster);
            if (err < 0) {
                return total_write;
            }
            //再将需要写入的内容写入fat_buffer中对应位置
            kernel_memcpy(fat->fat_buffer + cluster_offset, buf, curr_write);

            //再将fat_buffer覆盖回磁盘
            err = dev_write(fat->fs->dev_id, start_sector, fat->fat_buffer, fat->sec_per_cluster);
            if (err < 0) {
                return total_write;
            }
        }
        buf += curr_write;
        nbytes -= curr_write;
        total_write += curr_write;
        file->size += curr_write;

        //移动文件的读取位置file->pos
        int err = move_file_pos(file, fat, curr_write, 1);
        if (err < 0) {
            return total_write;
        }
    }

    return total_write;

}

/**
 * @brief fat文件系统关闭文件
 * 
 * @param file 
 */
void fatfs_close(file_t *file) {
    if (file->mode == O_RDONLY) {
        //文件只进行读操作，不需要回写到磁盘上
        return;
    }

    fat_t *fat = (fat_t*)file->fs->data;

    //读取文件所属的根目录区的目录项
    diritem_t *item = read_dir_entry(fat, file->p_index);
    if (item == (diritem_t *)0) {
        return;
    }

    //更新目录项信息,并回写到磁盘上
    item->DIR_FileSize = file->size;
    item->DIR_FstClusHI = (uint16_t)(file->sblk << 16);
    item->DIR_FstClusLo = (uint16_t)(file->sblk & 0xffff);
    write_dir_entry(fat, item, file->p_index);
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


        //该目录项有效,获取目录项信息到dirent中
        if (item->DIR_Name[0] != DIRITEM_NAEM_FREE && item->DIR_Name[0] != DIRITEM_NAME_END) {
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

/**
 * @brief fat文件系统删除文件
 * 
 * @param fs 
 * @param path 
 * @return int 
 */
int fatfs_unlink(struct _fs_t *fs, const char *path) {
     //获取fat表信息
    fat_t *fat = (fat_t*)fs->data;

    for (int i = 0; i < fat->root_ent_cnt; ++i) {
        diritem_t * item = read_dir_entry(fat, i);
        if (item == (diritem_t *)0) {
            return -1;
        }

        if (item->DIR_Name[0] == DIRITEM_NAME_END) {
            continue;
        }

        if (item->DIR_Name[0] == DIRITEM_NAEM_FREE) {
            continue;
        }

        //进行路径匹配
        if (diritem_name_match(item, path)) {
            //找到文件，进行删除操作
            //获取文件的起始簇号，并清除fat表中的簇链关系
            int cluster = (item->DIR_FstClusHI << 16) | item->DIR_FstClusLo;
            cluster_free_chain(fat, cluster);

            //将磁盘上该目录项的位置清空
            diritem_t file_item;
            kernel_memset(&file_item, 0, sizeof(diritem_t));
            return write_dir_entry(fat, &file_item, i);
        }
    }

    return -1;
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
    .unlink = fatfs_unlink,
};