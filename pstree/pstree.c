#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 1024
#define PROC_PATH "/proc"
#define TASK_DIR "task"
#define STATUS_FILE "status"

char *get_process_name(char *parentPath, char *pid)
{
  char path[256];
  FILE *file;

  // 拼接状态文件的路径
  snprintf(path, sizeof(path), "%s/%s/%s", parentPath, pid, STATUS_FILE);

  // 打开状态文件
  file = fopen(path, "r");
  if (!file)
  {
    perror("open status file failed");
    return NULL;
  }

  // 缓冲区读取名称字段
  char name[16];
  char line[128];

  while (fgets(line, sizeof(line), file))
  {
    if (strncmp(line, "Name:", 5) == 0)
    {
      sscanf(line, "Name: %s", name);
      break;
    }
  }

  fclose(file);

  return strdup(name);
}

char *get_task_dir_path(char *parentPath, char *pid)
{
  size_t parentPathLen = strlen(parentPath);
  char *path = malloc(parentPathLen + strlen(pid) + strlen(TASK_DIR) + 2);

  if (!path)
  {
    perror("malloc failed");
    exit(1);
  }

  snprintf(path, MAX_BUFFER_SIZE, "%s/%s/%s", parentPath, pid, TASK_DIR);

  return path;
}

void print_process_tree(char *pid, char *parentPath, int level)
{
  // 打印缩进
  for (int i = 0; i < level; i++)
  {
    printf("    ");
  }

  // 打印进程名称
  printf("|-- %s\n", get_process_name(parentPath, pid));

  DIR *dir = opendir(get_task_dir_path(parentPath, pid));
  if (!dir)
    return; // 没有子进程

  struct dirent *ent;
  while ((ent = readdir(dir)) != NULL)
  {
    if (ent->d_type == DT_DIR && atoi(ent->d_name) != 0)
    {
      char *child_pid = ent->d_name;
      size_t parentPathLen = strlen(parentPath);
      char *newParentPath = malloc(parentPathLen + strlen(ent->d_name) + 2);
      snprintf(newParentPath, MAX_BUFFER_SIZE, "%s", get_task_dir_path(parentPath, pid));
      print_process_tree(child_pid, newParentPath, level + 1);
      free(newParentPath);
    }
  }
  closedir(dir);
}

int main()
{
  DIR *dir;
  struct dirent *entry;

  // 打开 /proc 目录
  dir = opendir(PROC_PATH);
  if (dir == NULL)
  {
    perror("Failed to open directory");
    return 1;
  }

  printf("%s\n", get_process_name(PROC_PATH, "1")); // 打印根进程名称

  // 逐个读取 /proc 下的目录
  while ((entry = readdir(dir)) != NULL)
  {
    // 过滤掉非数字目录
    if (entry->d_type == DT_DIR && atoi(entry->d_name) != 0)
    {
      // parentPath
      char *parentPath = malloc(strlen(PROC_PATH) + 2);
      snprintf(parentPath, MAX_BUFFER_SIZE, "%s", PROC_PATH);
      print_process_tree(entry->d_name, parentPath, 1);
      free(parentPath);
    }
  }

  // 关闭目录
  closedir(dir);

  return 0;
}
