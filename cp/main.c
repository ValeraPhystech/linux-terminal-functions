#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

int FileToFile(char* src, char* dest);
int FileToDirectory(char* src, char* dest);
int DirectoryToDirectory(char* src, char* dest);
int isFile (char*    src);
char* isCurrentDirectory(char *src);
char *getcwd(char *buf, size_t size);


int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Wrong amount of arguments");
        return 0;
    }

    if(argv[argc-1][strlen(argv[argc-1])-1] == '/') { //проверяет, существует ли директория (на тот случай, когда последний аргумент - директория)
        if (0 != access(argv[argc - 1], F_OK)) {
            if (ENOENT == errno) {
                struct stat st = {0};

                if (stat(argv[argc - 1], &st) == -1) {
                    mkdir(argv[argc - 1], 0700); //Создает директорию, если она не существует
                }
            }
            if (ENOTDIR == errno) {
                // not a directory
            }
        }
    }
    if (argc == 3) { //случай, когда у нас на входе 2 аргумента
        char *src = argv[1];
        char *dest = argv[2];
        dest=isCurrentDirectory(dest);
        if(dest[strlen(dest)-1]=='/') { //проверить, существует ли директория (на тот случай, когда второй аргумент - директория)
            if (0 != access(dest, F_OK)) {
                if (ENOENT == errno) {
                    struct stat st = {0};

                    if (stat(dest, &st) == -1) {
                        mkdir(dest, 0700); //Создает директорию, если она не существует
                    }
                }
                if (ENOTDIR == errno) {
                    // not a directory
                }
            }
        }


        if(isFile(dest) == 1 || isFile(dest)==-1) {
            FileToFile(src, dest);
            return 0;
        }
    }

    if (isFile(argv[argc - 1]) == 0) { //основной случай, когда на входе несколько аргументов
        char *dest = argv[argc-1];

        if (0 != access(dest, F_OK)) {
            if (ENOENT == errno) {
                struct stat st = {0};

                if (stat(dest, &st) == -1) {
                    mkdir(dest, 0700); //Создает директорию, если она не существует
                }
            }
            if (ENOTDIR == errno) {
                // not a directory
            }
        }


        for (int i = 1; i < argc-1; i++) {

            if(dest[strlen(dest)-1]=='/') { //проверить, существует ли директория (на тот случай, когда аргумент - директория)
                if (0 != access(dest, F_OK)) {
                    if (ENOENT == errno) {
                        struct stat st = {0};

                        if (stat(dest, &st) == -1) {
                            mkdir(dest, 0700); //Создает директорию, если она не существует
                        }
                    }
                    if (ENOTDIR == errno) {
                        // not a directory
                    }
                }
            }

            if (isFile(argv[i]) == 1) {
                FileToDirectory(argv[i], dest);
            }
            else
            {
                DirectoryToDirectory(argv[i],dest);
            }
        }

    }
}

    int FileToFile(char* src, char* dest)
    {
        src=isCurrentDirectory(src);
        dest=isCurrentDirectory(dest);
        FILE *fp1, *fp2;
        char a;
        fp1 = fopen(src, "ab+");
        if (fp1 == NULL) {
            puts("cannot open this file");
            exit(1);
        }

        fp2 = fopen(dest, "ab+");
        if (fp2 == NULL) {
            puts("Not able to open this file");
            fclose(fp1);
            exit(1);
        }
        a = fgetc(fp1);
        while (a != EOF) {
            fputc(a, fp2);
            a = fgetc(fp1);
        }
        fcloseall();
        return 0;

    }

    int FileToDirectory(char* src, char* dest)
    {
        src=isCurrentDirectory(src);
        dest=isCurrentDirectory(dest);

        if(isFile(src) == 1 && src[0] != '/') { // на тот случай, когда у нас относительная ссылка на файл
            char correctDest[strlen(src) + strlen(dest)];
            for (int i = 0; i < strlen(correctDest); i++) memset(correctDest, 0, sizeof(correctDest));
            strcpy(correctDest, dest);
            strcat(correctDest, src);
            FileToFile(src, correctDest);
        }
        else //на тот случай, когда у нас абсолютная ссылка на файл
        {
            int k;
            for(int i=strlen(src);i>0;i--)
            {
                if(src[i]=='/')
                {
                    k=i;
                    break;
                }
            }
            char dst[strlen(src)-k];
            for (int j = 0; j < strlen(dst); j++) memset(dst, 0, sizeof(dst));
            strncpy(dst, &src[k+1],strlen(src)-k-1);
            char correctDest[strlen(dest)+strlen(dst)];
            for (int j = 0; j < strlen(correctDest); j++) memset(correctDest, 0, sizeof(correctDest));
            strcat(correctDest,dest);
            strcat(correctDest,dst);
            FileToFile(src,correctDest);
        }
        return 0;

    }

    int isFile(char* src)
    {
        src=isCurrentDirectory(src);
        int status;
        struct stat st_buf;

        stat (src, &st_buf);

        if (S_ISREG (st_buf.st_mode)) {
            return 1;
        }
        if (S_ISDIR (st_buf.st_mode)) {
            return 0;
        }
        return -1;
    }

    char* isCurrentDirectory(char *src)
    {
        if(src[0]=='.' && src[1]=='/') {
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != NULL)
            {
                char src2[strlen(cwd) + strlen(src)];
                for(int i=0;i<strlen(src2);i++) src2[i]=0;
                char* src2pointer=src2;
                strcat(src2,cwd);
                char tempstr[strlen(src)-1];
                tempstr[0]='/';
                for(int i=1;i<strlen(src)-1;i++) tempstr[i]=src[i+1];
                strcat(src2,tempstr);
                return src2pointer;
            }
                else
            perror("getcwd() error");
        }
        return src;
    }

    int DirectoryToDirectory(char* src, char* dest) {
        src=isCurrentDirectory(src);
        dest=isCurrentDirectory(dest);
        if (0 != access(dest, F_OK)) {
            if (ENOENT == errno) {
                struct stat st = {0};

                if (stat(dest, &st) == -1) {
                    mkdir(dest, 0700); //Создает директорию, если она не существует
                }
            }
            if (ENOTDIR == errno) {
                // not a directory
            }
        }

        DIR *d;
        struct dirent *dir;
        d = opendir(src);
        if (d) {
            while ((dir = readdir(d)) != NULL) {
                if (!strcmp (dir->d_name, "."))
                    continue;
                if (!strcmp (dir->d_name, ".."))
                    continue;
                    char tempdir[strlen(src) + strlen(dir->d_name)];
                    for (int i = 0; i < strlen(tempdir); i++) memset(tempdir, 0, sizeof(tempdir));
                    strcat(tempdir, src);
                    strcat(tempdir, dir->d_name);
                    if (isFile(tempdir) == 1)
                        FileToDirectory(tempdir, dest);
                    else if (isFile(tempdir) == 0) DirectoryToDirectory(tempdir, dest);

            }

            closedir(d);
        }
        return 0;
    }