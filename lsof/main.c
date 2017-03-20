#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>
#include <pwd.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>

void print_header()
{
    printf("%-10s %4s %11s %5s %8s %19s %10s %8s %10s\n",
           "COMMAND",
           "PID",
           "USER",
           "FD",
           "TYPE",
           "DEVICE",
           "SIZE/OFF",
           "NODE",
           "NAME");
}
void showInfo(char* tmp, char* str, struct dirent* ent) //выводит информацию о процессе, основываясь на его директории в /proc/ (tmp), имени(str), номере(ent->d_name)
{
    DIR *d;
    struct dirent *dir;
    char* fd_dir = strcat(tmp,"/fd/");
    d = opendir(fd_dir);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (!strcmp(dir->d_name, ".")) {
                continue;
            }
            if (!strcmp(dir->d_name, "..")) {
                continue;
            }
            char tempdir[strlen(fd_dir) + strlen(dir->d_name) + 2];
            for (int i = 0; i < strlen(tempdir); i++) memset(tempdir, 0, sizeof(tempdir));
            strcat(tempdir, fd_dir);
            strcat(tempdir, dir->d_name);

            static ssize_t file_path_size;
            static char file_path[255];
            if ((file_path_size = readlink(tempdir, file_path, sizeof(file_path) - 1)) < 0) {
                if (errno == ENOENT) {
                    printf("Error");
                }
            } else {
                file_path[file_path_size] = '\0';
            }

            if (file_path[0] != '/' || strstr(file_path,"dev")) {
                continue;
            }
            printf("%8.8s\t", str); //выводит имя процесса
            printf("%4.6s\t", ent->d_name); //выводит номер процесса


            struct passwd *pw;
            uid_t uid;

            uid = geteuid();
            pw = getpwuid(uid);
            if (pw) {
                printf("%7.7s\t", pw->pw_name); //выводит пользователя, который запустил процесс
            }

            printf("%5.5s\t", dir->d_name); //выводит номер файлового дескриптора

            struct stat session;
            stat(file_path, &session);

            if (S_ISDIR(session.st_mode)) {  //выводит тип файла, который связан с файловым дескриптором
                printf("%5.5s\t", "DIR");
            }
            if (S_ISCHR(session.st_mode)) {
                printf("%5.5s\t", "CHR");
            }
            if (S_ISBLK(session.st_mode)) {
                printf("%5.5s\t", "BLK");
            }
            if (S_ISREG(session.st_mode)) {
                printf("%5.5s\t", "REG");
            }
            if (S_ISFIFO(session.st_mode)) {
                printf("%5.5s\t", "FIFO");
            }
            if (S_ISLNK(session.st_mode)) {
                printf("%5.5s\t", "LNK");
            }
            if (S_ISSOCK(session.st_mode)) {
                printf("%5.5s\t", "SOCK");
            }

            printf("%13ld %0.1s %0ld", (long) major(session.st_dev), ",", (long) minor(session.st_dev)); //выводит DEVICE
            printf("%12lld", (long long) session.st_size); // выводит SIZE
            printf("%10ld", (long) session.st_ino); //выводит NODE

            printf("%5c",' ');
            for(int i=0;i<strlen(file_path); i++) printf("%c",file_path[i]); //выводит директорию файла, который связан с данным дескриптором

            printf("\n");
        }

    }
}
int main() {
    DIR *proc = opendir("/proc");
    struct dirent *ent;

    if (proc == NULL) {
        perror("opendir(/proc) is not working");
        return 1;
    }
    print_header();
    while ((ent = readdir(proc)) != NULL)
    {
        char *tmp = ent->d_name;
        if (!isdigit(*tmp)) continue;
        char a;

        char t[] = "/proc/";
        tmp = strcat(t, ent->d_name);
        char t2[] = "/stat";
        tmp = strcat(tmp, t2);

        FILE *fp1;
        fp1 = fopen(tmp, "r");
        char str[256] = "";
        for (int i = 0; i < 256; i++) str[i] = 0;
        int count = 0;
        a = (char)fgetc(fp1);
        int flag = 0;

        t[6] = 0;
        char *temp2 = strcat(t, ent->d_name);

        while (a != EOF) { //считываем из stat имя процесса и заносим его в str

            a = (char)fgetc(fp1);

            if (a == '(') {
                flag = 1;
                continue;
            }

            if (a == ')') {
                flag = 0;
                showInfo(tmp,str,ent);
            }
            if (flag == 1) {
                str[count] = a;
                count++;
            }
        }
    }
    closedir(proc);
    return 0;
}