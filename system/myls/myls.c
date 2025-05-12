#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>

void isFile(char* name);

void read_dir(char *dir){
    char path[256];
    DIR *dp;
    struct dirent *sdp;
    dp = opendir(dir);
    if(dp == NULL){
        perror("opendir error");
        return;
    }

    while((sdp = readdir(dp)) != NULL){
        if(strcmp(sdp->d_name, ".") == 0 || strcmp(sdp->d_name, "..") == 0)continue;
        sprintf(path, "%s/%s", dir, sdp->d_name);
        isFile(path);
    }

    closedir(dp);
    return;
}

void isFile(char* name){
    struct stat sb;
    int ret;
    ret = stat(name, &sb);
    if(ret == -1){
        perror("stat error");
        return;
    }
    if(S_ISDIR(sb.st_mode)){
        read_dir(name);
    }
    printf("%10s\t%ld\n", name, sb.st_size);
    return;
}

int main(int argc, char *argv[]){
    if(argc == 1) isFile(".");
    else{
        while(--argc > 0){
            isFile(*++argv);
        }
    }
    return 0;
}