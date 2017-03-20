#include<stdio.h>
#include <unistd.h>
#include<string.h>
#include <sys/stat.h>
#include <ctype.h>
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

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
            char tempstr[strlen(src)];
            for(int i=0;i<strlen(tempstr)+10;i++) tempstr[i]=0;
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

int main(int argc,char *argv[])
{
    FILE *path;
    int ignoreCase=0; // опция -i
    int invertMatch=0; // опция -v
    char str[256],pattern[256], temp[256],str2[256],pattern2[256],temp2[256];
    int option=0;
    int option_count=0;
    while ( (option = getopt(argc,argv,"iv")) != -1){  //считывание опций
        option_count+=1;
        switch (option){
            case 'i': ignoreCase=1; break;
            case 'v': invertMatch=1; break;
        };
    };

    for(int i=0;i<256;i++)
    {
        str[i]=0;
        pattern[i]=0;
        temp[i]=0;
        str2[i]=0;
        pattern2[i]=0;
        temp2[i]=0;
    }

    if(option_count == argc-1) { //считывание из потока stdin(в случае когда в аргументах только опции)
        printf("Enter substring, which will be searched\n");
        scanf("%s", pattern);
        printf("Enter string\n");
        scanf("%s", str);
    }
    else //считывание аргументов
    {
        int flag=0;
        for(int i=1;i<argc;i++)
        {
            if(argv[i][0]!='-' && flag==0)
            {
                strcat(pattern,argv[i]);
                flag=1;
                continue;
            }
            if(flag==1)
            {
                strcat(str,argv[i]);
                break;
            }
        }
    }

    if(ignoreCase == 1) //в случае опции -i приводим строки к нижнему регистру
    {
        if(option_count==argc-1) {
            strcat(str2, str);
            for (int i = 0; str[i]; i++) {
                str[i] = tolower(str[i]);
            }
        }
        strcat(pattern2,pattern);
        for(int i = 0; pattern[i]; i++){
            pattern[i] = tolower(pattern[i]);
        }
    }
    if(option_count == argc-1) { //в случае, когда мы считывали только stdin

        char *k = strstr(str, pattern);
        int position = k - str;
        int start = 0;
        for (int i = start; i < strlen(str); i++) {
            printf("%c", (ignoreCase == 1) ? str2[i] : str[i]);
            if (i == position - 1) {
                printf(" ");
                printf((ignoreCase == 1) ? pattern2 : pattern);
                printf(" ");
                k = strstr(str + i + 2, pattern);
                position = k - str;
                i = i + strlen(pattern);
            }
        }
        return 0;
    }
    char* tmp=isCurrentDirectory(str);

    if(isFile(tmp)==1)
    {
        path=fopen(tmp,"r");
    }
    else
    {
        printf("Wrong type of arguments");
        return 0;
    }
    while(!feof(path)) //считывание из файла
    {
        for(int i=0;i<256;i++) {
            temp[i] = 0;
            temp2[i] = 0;
        }
        fgets(temp,255,path);
        if(ignoreCase==1)
        {
            strcat(temp2,temp);
            for(int i = 0; temp[i]; i++){
                temp[i] = tolower(temp[i]);
            }
        }

        if(invertMatch==0)
        {
            if(ignoreCase==0)
            {
                if (strstr(temp, pattern))
                    printf("%s", temp);
            }
            else
            {
                if (strstr(temp, pattern))
                    printf("%s", temp2);
            }
        }
        else
        {
            if(ignoreCase==0)
            {
                if (strstr(temp, pattern)==NULL)
                    printf("%s", temp);
            }
            else
            {
                if (strstr(temp, pattern)==NULL)
                    printf("%s", temp2);
            }
        }
    }
    fclose(path);
    return 0;
}