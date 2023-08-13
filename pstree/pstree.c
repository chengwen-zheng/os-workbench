#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 1024

void print_process_info(const char* pid) {
    char path[MAX_BUFFER_SIZE];
    FILE* file;

    // 构建进程目录的路径
    snprintf(path, sizeof(path), "/proc/%s/status", pid);

    // 打开进程状态文件
    file = fopen(path, "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file for process %s\n", pid);
        return;
    }

    // 读取并打印进程信息
    char buffer[MAX_BUFFER_SIZE];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%s", buffer);
    }

    // 关闭文件
    fclose(file);
}

int main() {
    DIR* dir;
    struct dirent* entry;

    // 打开 /proc 目录
    dir = opendir("/proc");
    if (dir == NULL) {
        perror("Failed to open directory");
        return 1;
    }

    // 逐个读取 /proc 下的目录
    while ((entry = readdir(dir)) != NULL) {
        // 过滤掉非数字目录
        if (entry->d_type == DT_DIR && atoi(entry->d_name) != 0) {
            print_process_info(entry->d_name);
            printf("\n");
        }
    }

    // 关闭目录
    closedir(dir);

    return 0;
}
