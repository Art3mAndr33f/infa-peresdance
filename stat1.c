#define _FILE_OFFSET_BITS 64


#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>



const char* device_type(const mode_t mode) {
    switch (mode & S_IFMT) {    //bit mask for file type 
        case S_IFBLK:   return "block device";
        case S_IFCHR:   return "character device";
        case S_IFDIR:   return "directory";
        case S_IFIFO:   return "FIFO/pipe";
        case S_IFLNK:   return "symbolic link";
        case S_IFREG:   return "regular file";
        case S_IFSOCK:  return "socket";
    }
    return "unknown";
}


int main(int argc, char* argv[]) {
    if(argc != 2) {
        fprintf(stderr, "Incorrect argc: %d\n", argc);
        exit(EXIT_FAILURE);
    }

    struct stat sb;

    if(lstat(argv[1], &sb) == -1) {
        perror("lstat");
        exit(EXIT_FAILURE);
    }

    printf("File type:              %s\n", device_type(sb.st_mode));
    printf("INode:                  %ju\n", (uintmax_t)sb.st_ino);
    printf("Access mode:            %04jo (octal)\n", (uintmax_t)(sb.st_mode & ALLPERMS));
    printf("Number of links:        %ju\n", (uintmax_t)sb.st_nlink);
    printf("Owner:                  UID=%ju\tGID=%ju\n", (uintmax_t)sb.st_uid, (uintmax_t)sb.st_gid);
    printf("Preferred block size:   %ju byte\n", (uintmax_t)sb.st_blksize);
    printf("File size:              %ju byte\n", (uintmax_t)sb.st_size);
    printf("Allocated blocks:       %ju\n", (uintmax_t)sb.st_blocks);

    const size_t time_str_size = sizeof("Day Mon dd hh:mm:ss yyyy");

    struct tm curr_time;
    char buf[time_str_size];

    const char fmt[] = "%a %b %d %H:%M:%S %Y";
    
    if(!strftime(buf, sizeof(buf), fmt, gmtime_r(&sb.st_ctime, &curr_time))) {
        exit(EXIT_FAILURE);
    }
    printf("C_TIME (UTC)            %s\n", buf); //время последнего изменения статуса файла
    if(!strftime(buf, sizeof(buf), fmt, gmtime_r(&sb.st_atime, &curr_time))) {
        exit(EXIT_FAILURE);
    }
    printf("A_TIME (UTC)            %s\n", buf); //время последнего доступа к файлу
    if(!strftime(buf, sizeof(buf), fmt, gmtime_r(&sb.st_mtime, &curr_time))) {
        exit(EXIT_FAILURE);
    }
    printf("M_TIME (UTC)            %s\n", buf); //время последней модификации файла

    return 0;
}