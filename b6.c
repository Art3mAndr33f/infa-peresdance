#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>


char dtype_letter(unsigned d_type) {
    switch(d_type) {
        case DT_BLK: return 'b';
        case DT_CHR: return 'c';
        case DT_DIR: return 'd';
        case DT_FIFO: return 'p';
        case DT_LNK: return 'l';
        case DT_REG: return '-';
        case DT_SOCK: return 's';
    }
    return '?';
}


char mode_letter(mode_t st_mode) {
    switch (st_mode & S_IFMT) {
        case S_IFBLK: return 'b'; 
        case S_IFCHR: return 'c';
        case S_IFDIR: return 'd'; 
        case S_IFIFO: return 'p'; 
        case S_IFLNK: return 'l'; 
        case S_IFREG: return '-'; 
        case S_IFSOCK: return 's';	
    }
    return '?';
}


int readdir_func(DIR* dir_fd) {
    struct dirent* entry;
    int fd = dirfd(dir_fd);
    //возвращает дескриптор файла, связанный с dir_fd
    if(fd == -1) {
        perror("dirfd"); 
        return 1;
    }  
    char type = '?';
    while((entry = readdir(dir_fd)) != NULL) {
        type = dtype_letter(entry->d_type);
        if(type == '?') {
            struct stat sb;
            if(fstatat(fd, entry->d_name, &sb, AT_SYMLINK_NOFOLLOW) == 0) {
                //возвращает информацию о файле в буфере, на который указывает &sb
                type = mode_letter(sb.st_mode);
            }
        }
        printf("%c %s\n", type, entry->d_name);
    }
    return 0;
}


int main(int argc, char* argv[]) {
    if(argc != 2) {
        fprintf(stderr, "Bad argc: %d\n", argc);
        exit(EXIT_FAILURE);
    }
    DIR* dir_fd = opendir(argv[1]);
    if(!dir_fd) {
        perror("Failure to open dir");
        exit(EXIT_FAILURE);
    }
    if(readdir_func(dir_fd) != 0) {
        closedir(dir_fd);
        perror("Failure to read directory");
        exit(EXIT_FAILURE);
    }
    closedir(dir_fd);
    return 0;
}