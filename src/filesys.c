

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//init.h
#define INIT_H
#define BLOCKSIZE 1024
#define SIZE 1024000
#define END 65535
#define FREE 0
#define ROOTBLOCKNUM 2
#define MAXOPENFILE 10 // 同时打开最大文件数
#define MAX_TEXT_SIZE 10000

typedef struct FCB {
    char filename[8];
    char exname[3];
    unsigned char attribute; // 0: dir file, 1: data file
    unsigned short time;
    unsigned short date;
    unsigned short first;
    unsigned short length;
    char free; // 0: 空 fcb
} fcb;

typedef struct FAT {
    unsigned short id;
} fat;

typedef struct USEROPEN {
    char filename[8];
    char exname[3];
    unsigned char attribute;
    unsigned short time;
    unsigned short date;
    unsigned short first;
    unsigned short length;
    char free;

    int dirno; // 父目录文件起始盘块号
    int diroff; // 该文件对应的 fcb 在父目录中的逻辑序号
    char dir[MAXOPENFILE][80]; // 全路径信息
    int count;
    char fcbstate; // 是否修改 1是 0否
    char topenfile; // 0: 空 openfile
} useropen;

typedef struct BLOCK {
    char magic_number[8];
    char information[200];
    unsigned short root;
    unsigned char* startblock;
} block0;

unsigned char* myvhard;
//useropen *ptrcurdir;
useropen openfilelist[MAXOPENFILE];
int currfd;
//char currentdir[80];
unsigned char* startp;
//extern char *FILENAME;
unsigned char buffer[SIZE];

/* sysfile func */

void startsys();

void exitsys();

void my_format();

void my_ls();

int my_mkdir(char* dirname);

void my_rmdir(char* dirname);

int my_create(char* filename);

void my_rm(char* filename);

int my_open(char* filename);

int my_cd(char* dirname);

int new_cd(char* dirname);

int my_close(int fd);

int my_read(int fd);

int my_write(int fd);

int do_read(int fd, int len, char* text);

int do_write(int fd, char* text, int len, char wstyle);

int get_free_openfilelist();

unsigned short int get_free_block();

void show_tree(char* path, int deep);

int copy(char* srcFilePath, char* dstFilePath);

int myrenamefun(char* srcFilePath, char* dstFilePath);

void new_rd(char* dirname);

int createmyflod(char* name);

int new_copycon(char* dstFilePath);

void new_type(char* dstFilePath);

int new_del(char* dstFilePath);

int new_ls(char* dstFilePath);

void help();

//error func

void mkdirs(char* muldir);

int createdir(char* pDir);

void dirtree();

int mds(char* filename);

void mdk(char* dirname);

void my_mkdirst(char* dirname);

int TYPE(char* filename);

int REN(int fd);

int md(char* filename);


void test()
{
    //debug func
    int openfile_num = 0;
    int i;
    printf("debug area ############\n");
    // print open file number
    for (i = 0; i < MAXOPENFILE; i++) {
        if (openfilelist[i].topenfile == 1) {
            openfile_num++;
        }
    }
    printf("  open file number: %d\n", openfile_num);
    printf("  curr file name: %s\n", openfilelist[currfd].filename);
    printf("  curr file length: %d\n", openfilelist[currfd].length);

    printf("debug end  ############\n");
}

//主函数
int main(void)
{
    char cmd[19][10] = {
        "md", "rd", "dir", "cd", "copycon",
        "del", "type", "close", "write", "read",
        "exit", "help", "test", "cls","copy","ren","tree","attrib","clearall"
    };
    char command[30], *sp,*oth,*tp;
    int cmd_idx, i, res;

    printf("李瑞阳的模拟fat文件系统 [版本v1.0]\n");
    printf("作者：软件1801李瑞阳 120181080315\n");
    printf("提示：操作指南请认真阅读README.md  软件编写时间2020.6.18-2020.7.2\n");
    printf("运行程序前请一定查看 使用须知 ，这会避免很多错误！\n");
    startsys();
    printf("\n");
    while (1) {
        if (strcmp(openfilelist[currfd].dir, "") == 0)
            currfd = 0;
        printf("\n%s> ", openfilelist[currfd].dir);
        gets(command);
        cmd_idx = -1;
        if (strcmp(command, "") == 0) {
            printf("\n");
            continue;
        }
        sp = strtok(command, " ");
        for (i = 0; i < 19; i++) {
            if (strcmp(sp, cmd[i]) == 0) {
                cmd_idx = i;
                break;
            }
        }
        switch (cmd_idx) {
        case 0: // md
            sp = strtok(NULL, " ");
       
            if (sp != NULL) {
                res = createmyflod(sp);
                if(res == -1)
                    printf("%sdir has existed\n","<error>: ");
                if(res == -2)
                    printf("%sopenfilelist is full\n","<error>: ");
                if(res == -3)
                    printf("%sblocks are full\n","<error>: ");
            }
            else
                printf("md error\n");
            break;
        case 1: // rd
            sp = strtok(NULL, " ");
            if (sp != NULL)
            {
                new_rd(sp);
            }
            else
                printf("rd error\n");
            break;
        case 2: // dir
            sp = strtok(NULL, " ");
            if (sp != NULL)
                new_ls(sp);
            else
                my_ls();
            break;
        case 3: // cd
            sp = strtok(NULL, " ");
            res = -2;
            if (sp != NULL)
                res = new_cd(sp);
            if (res == -1)
            {
                printf("<error>: this is not a right dir\n");
            }
            if (res == -2)
            {
                printf("<error>: my_cd: no such dir\n");
            }
            break;
        case 4: // copycon
            sp = strtok(NULL, " ");
            if (sp != NULL)
            {
                new_copycon(sp);
            }
            else
                printf("copycon error\n");
            break;
        case 5: // del
            sp = strtok(NULL, " ");
            if (sp != NULL)
            {
                new_del(sp);
            }
            else
                printf("del error\n");
            break;
        case 6: // type
            sp = strtok(NULL, " ");
            if (sp != NULL)
            {
                new_type(sp);
            }
            else
                printf("type error\n");
            break;
        case 7: // close
            if (openfilelist[currfd].attribute == 1)
            {
                my_close(currfd);
            }
            else
                printf("there is not openning file\n");
            break;
        case 8: // write
            if (openfilelist[currfd].attribute == 1)
            {
                my_write(currfd);
            }
            else
                printf("please open file first, then write\n");
            break;
        case 9: // read
            if (openfilelist[currfd].attribute == 1)
            {
                my_read(currfd);
            }
            else
                printf("please open file first, then read\n");
            break;
        case 10: // exit
            exitsys();
            printf("**************** exit file system ****************\n");
            return 0;
            break;
        case 11: // help
            help();
            break;
        case 12: // test
            test();
            break;
        case 13: //cls
            system("cls");
            break;
        case 14: //copy
            sp = strtok(NULL, " ");
            if (sp != NULL) {
                oth = sp;
                sp = strtok(NULL, " ");
                if (sp != NULL) {
                    copy(oth, sp);
                }
                else
                {
                    printf("copy error\n");
                }
            }
            else
                printf("copy error\n");
            break;
        case 15: //ren
            sp = strtok(NULL, " ");
            if (sp != NULL) {

                oth = sp;
                sp = strtok(NULL, " ");
                if (sp != NULL) {
                    myrenamefun(oth, sp);
                }
                else
                {
                    printf("ren error\n");
                }
            }
            else
                printf("ren error\n");
            break;
        case 16: // tree
            sp = strtok(NULL, " ");
            if (sp == NULL)
            {
                printf("root\n");
                show_tree("/", 0);
            }
            else
            {
                printf("%s\n", sp);
                show_tree(sp, 0);
            }
            break;
        case 17: // attrib
            if(openfilelist[currfd].attribute == 1)
                printf("attrib:file");
            if (openfilelist[currfd].attribute == 0)
                printf("attrib:dir");
            break;
        case 18: //clearall
            my_format();
            break;
        default:
            printf("wrong command: %s\n", command);
            break;
        }
    }
    return 0;
}

char* FILENAME = "zfilesys";

//有用函数
void startsys()
{
    /**
     * 如果存在文件系统（存在 FILENAME 这个文件 且 开头为魔数）则
     * 将 root 目录载入打开文件表。
     * 否则，调用 my_format 创建文件系统，再载入。
     */
    myvhard = (unsigned char*)malloc(SIZE);
    FILE* file;
    if ((file = fopen(FILENAME, "r")) != NULL) {
        fread(buffer, SIZE, 1, file);
        fclose(file);
        if (memcmp(buffer, "10101010", 8) == 0) {
            memcpy(myvhard, buffer, SIZE);
            printf("%smyfsys file read successfully\n", "<info>: ");
        }
        else {
            printf("%sinvalid myfsys magic num, create file system\n", "<info>: ");
            my_format();
            memcpy(buffer, myvhard, SIZE);
        }
    }
    else {
        printf("%smyfsys not find, create file system\n", "<info>: ");
        my_format();
        memcpy(buffer, myvhard, SIZE);
    }

    fcb* root;
    root = (fcb*)(myvhard + 5 * BLOCKSIZE);
    strcpy(openfilelist[0].filename, root->filename);
    strcpy(openfilelist[0].exname, root->exname);
    openfilelist[0].attribute = root->attribute;
    openfilelist[0].time = root->time;
    openfilelist[0].date = root->date;
    openfilelist[0].first = root->first;
    openfilelist[0].length = root->length;
    openfilelist[0].free = root->free;

    openfilelist[0].dirno = 5;
    openfilelist[0].diroff = 0;
    strcpy(openfilelist[0].dir, "/root/");
    openfilelist[0].count = 0;
    openfilelist[0].fcbstate = 0;
    openfilelist[0].topenfile = 1;

    startp = ((block0*)myvhard)->startblock;
    currfd = 0;
    return;
}

void exitsys()
{
    /**
     * 依次关闭 打开文件。 写入 FILENAME 文件
     */
    while (currfd) {
        my_close(currfd);
    }
    FILE* fp = fopen(FILENAME, "w");
    fwrite(myvhard, SIZE, 1, fp);
    fclose(fp);
}

void my_format()
{
    /**
     * 初始化前五个磁盘块
     * 设定第六个磁盘块为根目录磁盘块
     * 初始化 root 目录： 创建 . 和 .. 目录
     * 写入 FILENAME 文件 （写入磁盘空间）
     */
    block0* boot = (block0*)myvhard;
    strcpy(boot->magic_number, "10101010");
    strcpy(boot->information, "fat file system");
    boot->root = 5;
    boot->startblock = myvhard + BLOCKSIZE * 5;

    fat* fat1 = (fat*)(myvhard + BLOCKSIZE);
    fat* fat2 = (fat*)(myvhard + BLOCKSIZE * 3);
    int i;
    for (i = 0; i < 6; i++) {
        fat1[i].id = END;
        fat2[i].id = END;
    }
    for (i = 6; i < 1000; i++) {
        fat1[i].id = FREE;
        fat2[i].id = FREE;
    }

    // 5th block is root
    fcb* root = (fcb*)(myvhard + BLOCKSIZE * 5);
    strcpy(root->filename, ".");
    strcpy(root->exname, "di");
    root->attribute = 0; // dir file

    time_t rawTime = time(NULL);
    struct tm* time = localtime(&rawTime);
    // 5 6 5 bits
    root->time = time->tm_hour * 2048 + time->tm_min * 32 + time->tm_sec / 2;
    // 7 4 5 bits; year from 2000
    root->date = (time->tm_year - 100) * 512 + (time->tm_mon + 1) * 32 + (time->tm_mday);
    root->first = 5;
    root->free = 1;
    root->length = 2 * sizeof(fcb);

    fcb* root2 = root + 1;
    memcpy(root2, root, sizeof(fcb));
    strcpy(root2->filename, "..");
    for (i = 2; i < (int)(BLOCKSIZE / sizeof(fcb)); i++) {
        root2++;
        strcpy(root2->filename, "");
        root2->free = 0;
    }

    FILE* fp = fopen(FILENAME, "w");
    fwrite(myvhard, SIZE, 1, fp);
    fclose(fp);
}

void my_ls()
{
    // 判断是否是目录
    if (openfilelist[currfd].attribute == 1) {
        printf("data file\n");
        return;
    }
    char buf[MAX_TEXT_SIZE];
    int i;

    // 读取当前目录文件信息(一个个fcb), 载入内存
    openfilelist[currfd].count = 0;
    do_read(currfd, openfilelist[currfd].length, buf);

    // 遍历当前目录 fcb
    fcb* fcbptr = (fcb*)buf;
    for (i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb)); i++, fcbptr++) {
        if (fcbptr->free == 1) {
            if (fcbptr->attribute == 0) {
                printf("<DIR> %-8s\t%d/%d/%d %d:%d\n",
                    fcbptr->filename,
                    (fcbptr->date >> 9) + 2000,
                    (fcbptr->date >> 5) & 0x000f,
                    (fcbptr->date) & 0x001f,
                    (fcbptr->time >> 11),
                    (fcbptr->time >> 5) & 0x003f);
            }
            else {
                printf("<---> %-8s\t%d/%d/%d %d:%d\t%d\n",
                    fcbptr->filename,
                    (fcbptr->date >> 9) + 2000,
                    (fcbptr->date >> 5) & 0x000f,
                    (fcbptr->date) & 0x001f,
                    (fcbptr->time >> 11),
                    (fcbptr->time >> 5) & 0x003f,
                    fcbptr->length);
            }
        }
    }
}

int my_mkdir(char* dirname)
{
    /**
     * 当前目录：当前打开目录项表示的目录
     * 该目录：以下指创建的目录
     * 父目录：指该目录的父目录
     * 如:
     * 我现在在 root 目录下， 输入命令 mkdir a/b/bb
     * 表示 在 root 目录下的 a 目录下的 b 目录中创建 bb 目录
     * 这时，父目录指 b，该目录指 bb，当前目录指 root
     * 以下都用这个表达，简单情况下，当前目录和父目录是一个目录
     * 来不及了，先讨论简单情况，即 mkdir bb
     */
    int i = 0;
    char text[MAX_TEXT_SIZE];

    char* fname = strtok(dirname, ".");
    char* exname = strtok(NULL, ".");
    if (exname != NULL) {
        printf("you can not use extension\n");
        return 0;
    }
    // 读取父目录信息
    openfilelist[currfd].count = 0;
    int filelen = do_read(currfd, openfilelist[currfd].length, text);
    fcb* fcbptr = (fcb*)text;

    // 查找是否重名
    for (i = 0; i < (int)(filelen / sizeof(fcb)); i++) {
        if (strcmp(dirname, fcbptr[i].filename) == 0 && fcbptr->attribute == 0) {
            //printf("dir has existed\n");
            return -1;
        }
    }

    // 申请一个打开目录表项
    int fd = get_free_openfilelist();
    if (fd == -1) {
        //printf("openfilelist is full\n");
        return -2;
    }
    // 申请一个磁盘块
    unsigned short int block_num = get_free_block();
    if (block_num == END) {
        //printf("blocks are full\n");
        openfilelist[fd].topenfile = 0;
        return -3;
    }
    // 更新 fat 表
    fat* fat1 = (fat*)(myvhard + BLOCKSIZE);
    fat* fat2 = (fat*)(myvhard + BLOCKSIZE * 3);
    fat1[block_num].id = END;
    fat2[block_num].id = END;

    // 在父目录中找一个空的 fcb，分配给该目录  ??未考虑父目录满的情况??
    for (i = 0; i < (int)(filelen / sizeof(fcb)); i++) {
        if (fcbptr[i].free == 0) {
            break;
        }
    }
    openfilelist[currfd].count = i * sizeof(fcb);
    openfilelist[currfd].fcbstate = 1;
    // 初始化该 fcb
    fcb* fcbtmp = (fcb*)malloc(sizeof(fcb));
    fcbtmp->attribute = 0;
    time_t rawtime = time(NULL);
    struct tm* time = localtime(&rawtime);
    fcbtmp->date = (time->tm_year - 100) * 512 + (time->tm_mon + 1) * 32 + (time->tm_mday);
    fcbtmp->time = (time->tm_hour) * 2048 + (time->tm_min) * 32 + (time->tm_sec) / 2;
    strcpy(fcbtmp->filename, dirname);
    strcpy(fcbtmp->exname, "di");
    fcbtmp->first = block_num;
    fcbtmp->length = 2 * sizeof(fcb);
    fcbtmp->free = 1;
    do_write(currfd, (char*)fcbtmp, sizeof(fcb), 2);

    // 设置打开文件表项
    openfilelist[fd].attribute = 0;
    openfilelist[fd].count = 0;
    openfilelist[fd].date = fcbtmp->date;
    openfilelist[fd].time = fcbtmp->time;
    openfilelist[fd].dirno = openfilelist[currfd].first;
    openfilelist[fd].diroff = i;
    strcpy(openfilelist[fd].exname, "di");
    strcpy(openfilelist[fd].filename, dirname);
    openfilelist[fd].fcbstate = 0;
    openfilelist[fd].first = fcbtmp->first;
    openfilelist[fd].free = fcbtmp->free;
    openfilelist[fd].length = fcbtmp->length;
    openfilelist[fd].topenfile = 1;
    strcat(strcat(strcpy(openfilelist[fd].dir, (char*)(openfilelist[currfd].dir)), dirname), "/");

    // 设置 . 和 .. 目录
    fcbtmp->attribute = 0;
    fcbtmp->date = fcbtmp->date;
    fcbtmp->time = fcbtmp->time;
    strcpy(fcbtmp->filename, ".");
    strcpy(fcbtmp->exname, "di");
    fcbtmp->first = block_num;
    fcbtmp->length = 2 * sizeof(fcb);
    do_write(fd, (char*)fcbtmp, sizeof(fcb), 2);

    fcb* fcbtmp2 = (fcb*)malloc(sizeof(fcb));
    memcpy(fcbtmp2, fcbtmp, sizeof(fcb));
    strcpy(fcbtmp2->filename, "..");
    fcbtmp2->first = openfilelist[currfd].first;
    fcbtmp2->length = openfilelist[currfd].length;
    fcbtmp2->date = openfilelist[currfd].date;
    fcbtmp2->time = openfilelist[currfd].time;
    do_write(fd, (char*)fcbtmp2, sizeof(fcb), 2);

    // 关闭该目录的打开文件表项，close 会修改父目录中对应该目录的 fcb 信息
    /**
     * 这里注意，一个目录存在 2 个 fcb 信息，一个为该目录下的 . 目录文件，一个为父目录下的 fcb。
     * 因此，这俩个fcb均需要修改，前一个 fcb 由各个函数自己完成，后一个 fcb 修改由 close 完成。
     * 所以这里，需要打开文件表，再关闭文件表，实际上更新了后一个 fcb 信息。
     */
    my_close(fd);

    free(fcbtmp);
    free(fcbtmp2);

    // 修改父目录 fcb
    fcbptr = (fcb*)text;
    fcbptr->length = openfilelist[currfd].length;
    openfilelist[currfd].count = 0;
    do_write(currfd, (char*)fcbptr, sizeof(fcb), 2);
    openfilelist[currfd].fcbstate = 1;
    return 1;
}

void my_rmdir(char* dirname)
{
    int i, tag = 0;
    char buf[MAX_TEXT_SIZE];

    // 排除 . 和 .. 目录
    if (strcmp(dirname, ".") == 0 || strcmp(dirname, "..") == 0) {
        printf("%scan not remove . and .. special dir\n", "<error>: ");
        return;
    }
    openfilelist[currfd].count = 0;
    do_read(currfd, openfilelist[currfd].length, buf);

    // 查找要删除的目录
    fcb* fcbptr = (fcb*)buf;
    for (i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb)); i++, fcbptr++) {
        if (fcbptr->free == 0)
            continue;
        if (strcmp(fcbptr->filename, dirname) == 0 && fcbptr->attribute == 0) {
            tag = 1;
            break;
        }
    }
    if (tag != 1) {
        printf("%sno such dir\n", "<error>: ");
        return;
    }
    // 无法删除非空目录
    if (fcbptr->length > 2 * sizeof(fcb)) {
        printf("%scan not remove a non empty dir\n", "<error>: ");
        return;
    }

    // 更新 fat 表
    int block_num = fcbptr->first;
    fat* fat1 = (fat*)(myvhard + BLOCKSIZE);
    int nxt_num = 0;
    while (1) {
        nxt_num = fat1[block_num].id;
        fat1[block_num].id = FREE;
        if (nxt_num != END) {
            block_num = nxt_num;
        }
        else {
            break;
        }
    }
    fat1 = (fat*)(myvhard + BLOCKSIZE);
    fat* fat2 = (fat*)(myvhard + BLOCKSIZE * 3);
    memcpy(fat2, fat1, BLOCKSIZE * 2);

    // 更新 fcb
    fcbptr->date = 0;
    fcbptr->time = 0;
    fcbptr->exname[0] = '\0';
    fcbptr->filename[0] = '\0';
    fcbptr->first = 0;
    fcbptr->free = 0;
    fcbptr->length = 0;

    openfilelist[currfd].count = i * sizeof(fcb);
    do_write(currfd, (char*)fcbptr, sizeof(fcb), 2);
    //my_close(currfd);

    // 删除目录需要相应考虑可能删除 fcb，也就是修改父目录 length
    // 这里需要注意：因为删除中间的 fcb，目录有效长度不变，即 length 不变
    // 因此需要考虑特殊情况，即删除最后一个 fcb 时，极有可能之前的 fcb 都是空的，这是需要
    // 循环删除 fcb (以下代码完成)，可能需要回收 block 修改 fat 表等过程(do_write 完成)
    int lognum = i;
    if ((lognum + 1) * sizeof(fcb) == openfilelist[currfd].length) {
        openfilelist[currfd].length -= sizeof(fcb);
        lognum--;
        fcbptr = (fcb*)buf + lognum;
        while (fcbptr->free == 0) {
            fcbptr--;
            openfilelist[currfd].length -= sizeof(fcb);
        }
    }

    // 更新父目录 fcb
    fcbptr = (fcb*)buf;
    fcbptr->length = openfilelist[currfd].length;
    openfilelist[currfd].count = 0;
    do_write(currfd, (char*)fcbptr, sizeof(fcb), 2);

    openfilelist[currfd].fcbstate = 1;
}

int my_create(char* filename)
{
    // 非法判断
    if (strcmp(filename, "") == 0) {
        printf("<error>: please input filename\n");
        return -1;
    }
    if (openfilelist[currfd].attribute == 1) {
        printf("<error>: you are in data file now\n");
        return -1;
    }

    openfilelist[currfd].count = 0;
    char buf[MAX_TEXT_SIZE];
    do_read(currfd, openfilelist[currfd].length, buf);

    int i;
    fcb* fcbptr = (fcb*)buf;
    // 检查重名
    for (i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb)); i++, fcbptr++) {
        if (fcbptr->free == 0) {
            continue;
        }
        if (strcmp(fcbptr->filename, filename) == 0 && fcbptr->attribute == 1) {
            printf("<error>: the same filename error\n");
            return -1;
        }
    }

    // 申请空 fcb;
    fcbptr = (fcb*)buf;
    for (i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb)); i++, fcbptr++) {
        if (fcbptr->free == 0)
            break;
    }
    // 申请磁盘块并更新 fat 表
    int block_num = get_free_block();
    if (block_num == -1) {
        return -1;
    }
    fat* fat1 = (fat*)(myvhard + BLOCKSIZE);
    fat* fat2 = (fat*)(myvhard + BLOCKSIZE * 3);
    fat1[block_num].id = END;
    memcpy(fat2, fat1, BLOCKSIZE * 2);

    // 修改 fcb 信息
    strcpy(fcbptr->filename, filename);
    time_t rawtime = time(NULL);
    struct tm* time = localtime(&rawtime);
    fcbptr->date = (time->tm_year - 100) * 512 + (time->tm_mon + 1) * 32 + (time->tm_mday);
    fcbptr->time = (time->tm_hour) * 2048 + (time->tm_min) * 32 + (time->tm_sec) / 2;
    fcbptr->first = block_num;
    fcbptr->free = 1;
    fcbptr->attribute = 1;
    fcbptr->length = 0;

    openfilelist[currfd].count = i * sizeof(fcb);
    do_write(currfd, (char*)fcbptr, sizeof(fcb), 2);

    // 修改父目录 fcb
    fcbptr = (fcb*)buf;
    fcbptr->length = openfilelist[currfd].length;
    openfilelist[currfd].count = 0;
    do_write(currfd, (char*)fcbptr, sizeof(fcb), 2);

    openfilelist[currfd].fcbstate = 1;
}

void my_rm(char* filename)
{
    char buf[MAX_TEXT_SIZE];
    openfilelist[currfd].count = 0;
    do_read(currfd, openfilelist[currfd].length, buf);

    int i, flag = 0;
    fcb* fcbptr = (fcb*)buf;
    // 查询
    for (i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb)); i++, fcbptr++) {
        if (strcmp(fcbptr->filename, filename) == 0 && fcbptr->attribute == 1) {
            flag = 1;
            break;
        }
    }
    if (flag != 1) {
        printf("<error>: no such file\n");
        return;
    }

    // 更新 fat 表
    int block_num = fcbptr->first;
    fat* fat1 = (fat*)(myvhard + BLOCKSIZE);
    int nxt_num = 0;
    while (1) {
        nxt_num = fat1[block_num].id;
        fat1[block_num].id = FREE;
        if (nxt_num != END)
            block_num = nxt_num;
        else
            break;
    }
    fat1 = (fat*)(myvhard + BLOCKSIZE);
    fat* fat2 = (fat*)(myvhard + BLOCKSIZE * 3);
    memcpy(fat2, fat1, BLOCKSIZE * 2);

    // 清空 fcb
    fcbptr->date = 0;
    fcbptr->time = 0;
    fcbptr->exname[0] = '\0';
    fcbptr->filename[0] = '\0';
    fcbptr->first = 0;
    fcbptr->free = 0;
    fcbptr->length = 0;
    openfilelist[currfd].count = i * sizeof(fcb);
    do_write(currfd, (char*)fcbptr, sizeof(fcb), 2);
    //
    int lognum = i;
    if ((lognum + 1) * sizeof(fcb) == openfilelist[currfd].length) {
        openfilelist[currfd].length -= sizeof(fcb);
        lognum--;
        fcbptr = (fcb*)buf + lognum;
        while (fcbptr->free == 0) {
            fcbptr--;
            openfilelist[currfd].length -= sizeof(fcb);
        }
    }

    // 修改父目录 . 目录文件的 fcb
    fcbptr = (fcb*)buf;
    fcbptr->length = openfilelist[currfd].length;
    openfilelist[currfd].count = 0;
    do_write(currfd, (char*)fcbptr, sizeof(fcb), 2);

    openfilelist[currfd].fcbstate = 1;
}

int my_open(char* filename)
{
    char buf[MAX_TEXT_SIZE];
    openfilelist[currfd].count = 0;
    do_read(currfd, openfilelist[currfd].length, buf);

    int i, flag = 0;
    fcb* fcbptr = (fcb*)buf;
    // 重名检查
    for (i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb)); i++, fcbptr++) {
        if (strcmp(fcbptr->filename, filename) == 0 && fcbptr->attribute == 1) {
            flag = 1;
            break;
        }
    }
    if (flag != 1) {
        printf("<error>: no such file\n");
        return -1;
    }

    // 申请新的打开目录项并初始化该目录项
    int fd = get_free_openfilelist();
    if (fd == -1) {
        printf("<error>: my_open: full openfilelist\n");
        return -1;
    }

    openfilelist[fd].attribute = 1;
    openfilelist[fd].count = 0;
    openfilelist[fd].date = fcbptr->date;
    openfilelist[fd].time = fcbptr->time;
    openfilelist[fd].length = fcbptr->length;
    openfilelist[fd].first = fcbptr->first;
    openfilelist[fd].free = 1;
    strcpy(openfilelist[fd].filename, fcbptr->filename);
    strcat(strcpy(openfilelist[fd].dir, (char*)(openfilelist[currfd].dir)), filename);
    openfilelist[fd].dirno = openfilelist[currfd].first;
    openfilelist[fd].diroff = i;
    openfilelist[fd].topenfile = 1;

    openfilelist[fd].fcbstate = 0;

    currfd = fd;
    return 1;
}

int my_cd(char* dirname)
{
    int i = 0;
    int tag = -1;
    int fd;
    if (strcmp(dirname, "..") == 0 && currfd == 0)
        return;
    if (openfilelist[currfd].attribute == 1) {
        // if not a dir
        //printf("<error>: you are in a data file, you could use 'close' to exit this file\n");
        return -1;
    }
    char* buf = (char*)malloc(10000);
    openfilelist[currfd].count = 0;
    do_read(currfd, openfilelist[currfd].length, buf);

    fcb* fcbptr = (fcb*)buf;
    // 查找目标 fcb
    for (i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb)); i++, fcbptr++) {
        if (strcmp(fcbptr->filename, dirname) == 0 && fcbptr->attribute == 0) {
            tag = 1;
            break;
        }
    }
    if (tag != 1) {
        //printf("<error>: my_cd: no such dir\n");
        return -2;
    }
    else {
        // . 和 .. 检查
        if (strcmp(fcbptr->filename, ".") == 0) {
            return 0;
        }
        else if (strcmp(fcbptr->filename, "..") == 0) {
            if (currfd == 0) {
                // root
                return 0;
            }
            else {
                currfd = my_close(currfd);
                return;
            }
        }
        else {
            // 其他目录
            fd = get_free_openfilelist();
            if (fd == -1) {
                return -1;
            }
            openfilelist[fd].attribute = fcbptr->attribute;
            openfilelist[fd].count = 0;
            openfilelist[fd].date = fcbptr->date;
            openfilelist[fd].time = fcbptr->time;
            strcpy(openfilelist[fd].filename, fcbptr->filename);
            strcpy(openfilelist[fd].exname, fcbptr->exname);
            openfilelist[fd].first = fcbptr->first;
            openfilelist[fd].free = fcbptr->free;

            openfilelist[fd].fcbstate = 0;
            openfilelist[fd].length = fcbptr->length;
            strcat(strcat(strcpy(openfilelist[fd].dir, (char*)(openfilelist[currfd].dir)), dirname), "/");
            openfilelist[fd].topenfile = 1;
            openfilelist[fd].dirno = openfilelist[currfd].first;
            openfilelist[fd].diroff = i;
            currfd = fd;
        }
    }
    return 1;
}

int new_cd(char* dirname)
{
    char* arrPtr[10];
    char* token = strtok(dirname, "/");
    int j = 0,res = 1;
    if (dirname[0] == '/')
    {
        currfd = 0;
    }
    while (token != NULL)
    {
        /*  printf("%s\n", token);*/
        arrPtr[j] = token;
        j++;
        token = strtok(NULL, "/");
    }
    for (int i = 0; i < j; i++)
    {
        res = my_cd(arrPtr[i]);
        if (res != 1)
            return res;
    }
    return res;
}

int my_close(int fd)
{
    if (fd > MAXOPENFILE || fd < 0) {
        printf("<error>: my_close: fd error\n");
        return -1;
    }

    int i;
    char buf[MAX_TEXT_SIZE];
    int father_fd = -1;
    fcb* fcbptr;
    for (i = 0; i < MAXOPENFILE; i++) {
        if (openfilelist[i].first == openfilelist[fd].dirno) {
            father_fd = i;
            break;
        }
    }
    if (father_fd == -1) {
        printf("<error>: my_close: no father dir\n");
        return -1;
    }
    if (openfilelist[fd].fcbstate == 1) {
        do_read(father_fd, openfilelist[father_fd].length, buf);
        // update fcb
        fcbptr = (fcb*)(buf + sizeof(fcb) * openfilelist[fd].diroff);
        strcpy(fcbptr->exname, openfilelist[fd].exname);
        strcpy(fcbptr->filename, openfilelist[fd].filename);
        fcbptr->first = openfilelist[fd].first;
        fcbptr->free = openfilelist[fd].free;
        fcbptr->length = openfilelist[fd].length;
        fcbptr->time = openfilelist[fd].time;
        fcbptr->date = openfilelist[fd].date;
        fcbptr->attribute = openfilelist[fd].attribute;
        openfilelist[father_fd].count = openfilelist[fd].diroff * sizeof(fcb);
        do_write(father_fd, (char*)fcbptr, sizeof(fcb), 2);
    }
    // 释放打开文件表
    memset(&openfilelist[fd], 0, sizeof(useropen));
    currfd = father_fd;
    return father_fd;
}

int my_read(int fd)
{
    if (fd < 0 || fd >= MAXOPENFILE) {
        printf("<error>: no such file\n");
        return -1;
    }

    openfilelist[fd].count = 0;
    char text[MAX_TEXT_SIZE] = "\0";
    do_read(fd, openfilelist[fd].length, text);
    printf("%s\n", text);
    return 1;
}

int my_write(int fd)
{
    if (fd < 0 || fd >= MAXOPENFILE) {
        printf("<error>: my_write: no such file\n");
        return -1;
    }
    int wstyle;
    while (1) {
        // 1: 截断写，清空全部内容，从头开始写
        // 2. 覆盖写，从文件指针处开始写
        // 3. 追加写，字面意思
        printf("please choose your action\n");
        printf("1:Truncation  2:Coverage  3:Addition\n");
        scanf("%d", &wstyle);
        if (wstyle > 3 || wstyle < 1) {
            printf("<error>: input error\n");
        }
        else {
            break;
        }
    }
    char text[MAX_TEXT_SIZE] = "\0";
    char texttmp[MAX_TEXT_SIZE] = "\0";
    printf("<info>: please input data, line feed + $$ to end file\n");
    getchar();
    while (gets(texttmp)) {
        if (strcmp(texttmp, "$$") == 0) {
            break;
        }
        texttmp[strlen(texttmp)] = '\n';
        strcat(text, texttmp);
    }

    text[strlen(text)] = '\0';
    do_write(fd, text, strlen(text) + 1, wstyle);
    openfilelist[fd].fcbstate = 1;
    return 1;
}

int do_read(int fd, int len, char* text)
{
    int len_tmp = len;
    char* textptr = text;
    unsigned char* buf = (unsigned char*)malloc(1024);
    if (buf == NULL) {
        printf("<error>: do_read reg mem error\n");
        return -1;
    }
    int off = openfilelist[fd].count;
    int block_num = openfilelist[fd].first;
    fat* fatptr = (fat*)(myvhard + BLOCKSIZE) + block_num;

    // 定位读取目标磁盘块和块内地址
    while (off >= BLOCKSIZE) {
        off -= BLOCKSIZE;
        block_num = fatptr->id;
        if (block_num == END) {
            printf("<error>: do_read: block not exist\n");
            return -1;
        }
        fatptr = (fat*)(myvhard + BLOCKSIZE) + block_num;
    }

    unsigned char* blockptr = myvhard + BLOCKSIZE * block_num;
    memcpy(buf, blockptr, BLOCKSIZE);

    // 读取内容
    while (len > 0) {
        if (BLOCKSIZE - off > len) {
            memcpy(textptr, buf + off, len);
            textptr += len;
            off += len;
            openfilelist[fd].count += len;
            len = 0;
        }
        else {
            memcpy(textptr, buf + off, BLOCKSIZE - off);
            textptr += BLOCKSIZE - off;
            len -= BLOCKSIZE - off;

            block_num = fatptr->id;
            if (block_num == END) {
                printf("<error>: do_read: len is lager then file\n");
                break;
            }
            fatptr = (fat*)(myvhard + BLOCKSIZE) + block_num;
            blockptr = myvhard + BLOCKSIZE * block_num;
            memcpy(buf, blockptr, BLOCKSIZE);
        }
    }
    free(buf);
    return len_tmp - len;
}

int do_write(int fd, char* text, int len, char wstyle)
{
    int block_num = openfilelist[fd].first;
    int i, tmp_num;
    int lentmp = 0;
    char* textptr = text;
    char buf[BLOCKSIZE];
    fat* fatptr = (fat*)(myvhard + BLOCKSIZE) + block_num;
    unsigned char* blockptr;

    if (wstyle == 1) {
        openfilelist[fd].count = 0;
        openfilelist[fd].length = 0;
    }
    else if (wstyle == 3) {
        // 追加写，如果是一般文件，则需要先删除末尾 \0，即将指针移到末位减一个字节处
        openfilelist[fd].count = openfilelist[fd].length;
        if (openfilelist[fd].attribute == 1) {
            if (openfilelist[fd].length != 0) {
                // 非空文件
                openfilelist[fd].count = openfilelist[fd].length - 1;
            }
        }
    }

    int off = openfilelist[fd].count;

    // 定位磁盘块和块内偏移量
    while (off >= BLOCKSIZE) {
        block_num = fatptr->id;
        if (block_num == END) {
            printf("<error>: do_write: off error\n");
            return -1;
        }
        fatptr = (fat*)(myvhard + BLOCKSIZE) + block_num;
        off -= BLOCKSIZE;
    }

    blockptr = (unsigned char*)(myvhard + BLOCKSIZE * block_num);
    // 写入磁盘
    while (len > lentmp) {
        memcpy(buf, blockptr, BLOCKSIZE);
        for (; off < BLOCKSIZE; off++) {
            *(buf + off) = *textptr;
            textptr++;
            lentmp++;
            if (len == lentmp)
                break;
        }
        memcpy(blockptr, buf, BLOCKSIZE);

        // 写入的内容太多，需要写到下一个磁盘块，如果没有磁盘块，就申请一个
        if (off == BLOCKSIZE && len != lentmp) {
            off = 0;
            block_num = fatptr->id;
            if (block_num == END) {
                block_num = get_free_block();
                if (block_num == END) {
                    printf("<error>: do_write: block full\n");
                    return -1;
                }
                blockptr = (unsigned char*)(myvhard + BLOCKSIZE * block_num);
                fatptr->id = block_num;
                fatptr = (fat*)(myvhard + BLOCKSIZE) + block_num;
                fatptr->id = END;
            }
            else {
                blockptr = (unsigned char*)(myvhard + BLOCKSIZE * block_num);
                fatptr = (fat*)(myvhard + BLOCKSIZE) + block_num;
            }
        }
    }

    openfilelist[fd].count += len;
    if (openfilelist[fd].count > openfilelist[fd].length)
        openfilelist[fd].length = openfilelist[fd].count;

    // 删除多余的磁盘块
    if (wstyle == 1 || (wstyle == 2 && openfilelist[fd].attribute == 0)) {
        off = openfilelist[fd].length;
        fatptr = (fat*)(myvhard + BLOCKSIZE) + openfilelist[fd].first;
        while (off >= BLOCKSIZE) {
            block_num = fatptr->id;
            off -= BLOCKSIZE;
            fatptr = (fat*)(myvhard + BLOCKSIZE) + block_num;
        }
        while (1) {
            if (fatptr->id != END) {
                i = fatptr->id;
                fatptr->id = FREE;
                fatptr = (fat*)(myvhard + BLOCKSIZE) + i;
            }
            else {
                fatptr->id = FREE;
                break;
            }
        }
        fatptr = (fat*)(myvhard + BLOCKSIZE) + block_num;
        fatptr->id = END;
    }

    memcpy((fat*)(myvhard + BLOCKSIZE * 3), (fat*)(myvhard + BLOCKSIZE), BLOCKSIZE * 2);
    return len;
}

int get_free_openfilelist()
{
    int i;
    for (i = 0; i < MAXOPENFILE; i++) {
        if (openfilelist[i].topenfile == 0) {
            openfilelist[i].topenfile = 1;
            return i;
        }
    }
    return -1;
}

unsigned short int get_free_block()
{
    int i;
    fat* fat1 = (fat*)(myvhard + BLOCKSIZE);
    for (i = 0; i < (int)(SIZE / BLOCKSIZE); i++) {
        if (fat1[i].id == FREE) {
            return i;
        }
    }
    return END;
}

void show_tree(char* path, int deep) {
    //树形目录
    int j = 0;
    int oldpath = currfd;
    char* arrPtr[10];
    char* token = strtok(path, "/");
    char buf[MAX_TEXT_SIZE];
    int i;

    if (path[0] == '/')
    {
        currfd = 0;
    }
    while (token != NULL)
    {
        arrPtr[j] = token;
        j++;
        token = strtok(NULL, "/");
    }
    for (int i = 1; i < j; i++)
    {
        my_cd(arrPtr[i]);
    }

    new_cd(path);
    // 判断是否是目录
    if (openfilelist[currfd].attribute == 1) {
        printf("<error>: data file\n");
        return;
    }
    // 读取当前目录文件信息(一个个fcb), 载入内存
    openfilelist[currfd].count = 0;
    do_read(currfd, openfilelist[currfd].length, buf);

    // 遍历当前目录 fcb
    fcb* fcbptr = (fcb*)buf;
    for (i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb)); i++, fcbptr++) {
        if (strcmp(fcbptr->filename, ".") == 0 || strcmp(fcbptr->filename, "..") == 0)
            continue;
        if (fcbptr->free == 1) {
            for (j = 0; j < deep; j++)
                printf("  ");
            printf("|--");
            if (fcbptr->attribute == 0) {
                printf("<DIR> %-8s\n", fcbptr->filename);
                show_tree(fcbptr->filename, deep + 1);
            }
            else {
                printf("<---> %-8s\n", fcbptr->filename);
            }
        }
    }
    if (deep != 0)
        new_cd("..");
    if (deep == 0)
        currfd = oldpath;
}

int copy(char* srcFilePath, char* dstFilePath)
{
    /*
     * 函数名：copy
     * 功  能：文件拷贝
     * 参  数：
     *     srcFilePath --- 源文件路径
     *     dstFilePath --- 目标文件路径
     * 返回值：无
    **/
    int oldpath = currfd;
    char text[MAX_TEXT_SIZE] = "\0";
    char* arrPtr[10];
    char* token = strtok(dstFilePath, "/");
    int j = 0;
    if (my_open(srcFilePath) != 1)
        return -1;
    do_read(currfd, openfilelist[currfd].length, text);
    my_close(currfd);
    if (dstFilePath[0] == '/')
    {
        currfd = 0;
    }
    while (token != NULL)
    {
        arrPtr[j] = token;
        j++;
        token = strtok(NULL, "/");
    }
    for (int i = 0; i < j - 1; i++)
    {
        my_mkdir(arrPtr[i]);
        my_cd(arrPtr[i]);
    }
    my_create(arrPtr[j - 1]);
    my_open(arrPtr[j - 1]);
    do_write(currfd, text, strlen(text) + 1, 1);
    openfilelist[currfd].fcbstate = 1;
    my_close(currfd);
    for (int i = 0; i < j - 1; i++)
    {
        my_cd("..");
        if (currfd == 0)
            break;
    }
    my_cd("..");
    currfd = oldpath;
    return 1;
}

int myrenamefun(char* srcFilePath, char* dstFilePath) {
    char* temp = strtok(NULL, ".");
    char text[MAX_TEXT_SIZE] = "\0";
    temp = "";
    int oldpath = currfd;
    if (my_open(srcFilePath) == -1)
    {
        printf("<error>:There is no srcfile\n");
        return -1;
    }
    do_read(currfd, openfilelist[currfd].length, text);
    my_close(currfd);
    my_rm(srcFilePath);
    if (my_create(dstFilePath) == -1)
    {
        printf("<error>:There is a samename File!\n");
        return -1;
    }
    my_open(dstFilePath);
    do_write(currfd, text, strlen(text) + 1, 1);
    openfilelist[currfd].fcbstate = 1;
    my_close(currfd);
    my_cd("..");
    currfd = oldpath;
    return 1;
}

void new_rd(char* dirname)
{
    char* arrPtr[100];
    char* token = strtok(dirname, "/");
    int j = 0;
    if (dirname[0] == '/')
    {
        currfd = 0;
    }
    while (token != NULL)
    {
        arrPtr[j] = token;
        j++;
        token = strtok(NULL, "/");
    }
    for (int i = 0; i < j - 1; i++)
    {
        my_cd(arrPtr[i]);
    }
    my_rmdir(arrPtr[j - 1]);
    for (int i = 0; i < j - 1; i++)
    {
        my_cd("..");
        if (currfd == 0)
            break;
    }
}

int createmyflod(char* name) {
    int oldpath = currfd;
    char* arrPtr[100];
    char* token = strtok(name, "/");
    int j = 0, i = 0;
    int errorinfo = 0;
    if (name[0] == '/')
        currfd = 0;
    while (token != NULL)
    {
        arrPtr[j] = token;
        j++;
        token = strtok(NULL, "/");
    }
    for (i = 0; i < j - 1; i++)
    {
        errorinfo = 1;
        if (my_cd(arrPtr[i]) != 1)
        {
            errorinfo = my_mkdir(arrPtr[i]);
            if (errorinfo != 1)
            {
                printf("%smake dir<%s> fail!\n", "<error>: ", arrPtr[i]);
                return errorinfo;
            }
            my_cd(arrPtr[i]);
        }
    }
    errorinfo = my_mkdir(arrPtr[i]);
    if (errorinfo != 1)
    {
        printf("%smake dir<%s> fail!\n", "<error>: ", arrPtr[i]);
        return errorinfo;
    }
    my_cd(arrPtr[i]);
    for (int i = 0; i < j; i++)
    {
        my_cd("..");
        if (currfd == 0)
            break;
    }

    currfd = oldpath;
    return 1;
}

int new_copycon(char* dstFilePath)
{
    int oldpath = currfd;
    char text[MAX_TEXT_SIZE] = "\0";
    char* arrPtr[10];
    char* token = strtok(dstFilePath, "/");
    int j = 0;
    if (dstFilePath[0] == '/')
        currfd = 0;
    while (token != NULL)
    {
        /*  printf("%s\n", token);*/
        arrPtr[j] = token;
        j++;
        token = strtok(NULL, "/");
    }
    for (int i = 0; i < j - 1; i++)
    {
        my_mkdir(arrPtr[i]);
        my_cd(arrPtr[i]);
    }
    for (int i = 0; i < j - 1; i++)
    {
        my_cd("..");
        if (currfd == 0)
            break;
    }
    for (int i = 0; i < j - 1; i++)
    {
        my_cd(arrPtr[i]);
    }
    if (my_create(arrPtr[j - 1]) == -1)
    {
        printf("<error>:There is a samename File!\n");
        return -1;
    }
    my_open(arrPtr[j - 1]);
    my_write(currfd);
    my_close(currfd);
    for (int i = 0; i < j - 1; i++)
    {
        my_cd("..");
        if (currfd == 0)
            break;
    }
    currfd = oldpath;
    return 1;
}

void new_type(char* dstFilePath)
{
    int oldpath = currfd;
    char text[MAX_TEXT_SIZE] = "\0";
    char* arrPtr[10];
    char* token = strtok(dstFilePath, "/");
    int j = 0;
    if (dstFilePath[0] == '/')
        currfd = 0;
    while (token != NULL)
    {
        /*  printf("%s\n", token);*/
        arrPtr[j] = token;
        j++;
        token = strtok(NULL, "/");
    }
    for (int i = 0; i < j - 1; i++)
    {
        my_cd(arrPtr[i]);
    }
    my_open(arrPtr[j - 1]);
    my_read(currfd);
    my_close(currfd);
    currfd = oldpath;
}

int new_del(char* dstFilePath)
{
    int oldpath = currfd;
    char text[MAX_TEXT_SIZE] = "\0";
    char* arrPtr[10];
    char* token = strtok(dstFilePath, "/");
    int j = 0;
    if (dstFilePath[0] == '/')
        currfd = 0;
    while (token != NULL)
    {
        arrPtr[j] = token;
        j++;
        token = strtok(NULL, "/");
    }
    for (int i = 0; i < j - 1; i++)
    {
        my_cd(arrPtr[i]);
    }
    my_rm(arrPtr[j - 1]);
    currfd = oldpath;
}

int new_ls(char* dstFilePath)
{
    int oldpath = currfd;
    char text[MAX_TEXT_SIZE] = "\0";
    char* arrPtr[10];
    char* token = strtok(dstFilePath, "/");
    int j = 0;
    if (dstFilePath[0] == '/')
        currfd = 0;
    while (token != NULL)
    {
        /*  printf("%s\n", token);*/
        arrPtr[j] = token;
        j++;
        token = strtok(NULL, "/");
    }
    for (int i = 0; i < j; i++)
    {
        my_cd(arrPtr[i]);
    }
    my_ls();
    currfd = oldpath;
}

void help()
{
    printf("xxxxxxx\n");
}


//废弃函数

void mkdirs(char* muldir)
{
    int i, len;
    char str[512];
    strncpy(str, muldir, 512);
    len = strlen(str);
    for (i = 0; i < len; i++)
    {
        if (str[i] == '/')
        {
            str[i] = '\0';
            if (access(str, 0) != 0)
            {
                mkdir(str, 0777);
            }
            str[i] = '/';
        }
    }
    if (len > 0 && access(str, 0) != 0)
    {
        mkdir(str, 0777);
    }
    return;
}

int createdir(char* pDir)
{
    int i = 0;
    int iRet;
    int iLen;
    char* pszDir;

    if (NULL == pDir)
    {
        return 0;
    }

    pszDir = _strdup(pDir);
    iLen = strlen(pszDir);

    // 创建中间目录
    for (i = 0; i < iLen; i++)
    {
        if (pszDir[i] == '\\' || pszDir[i] == '/')
        {
            pszDir[i] = '\0';

            //如果不存在,创建
            iRet = access(pszDir, 0);
            if (iRet != 0)
            {
                iRet = mkdir(pszDir);
                if (iRet != 0)
                {
                    return -1;
                }
            }
            //将所有\换成/
            pszDir[i] = '/';
        }
    }

    iRet = mkdir(pszDir);
    free(pszDir);
    return iRet;
}

void dirtree()
{
    // 判断是否是目录
    if (openfilelist[currfd].attribute == 1) {
        printf("%sThis is a data file\n", "<warning>: ");
        return;
    }
    char buf[MAX_TEXT_SIZE];
    int i;
    // 读取当前目录文件信息(一个个fcb), 载入内存
    openfilelist[currfd].count = 0;
    do_read(currfd, openfilelist[currfd].length, buf);
    // 遍历当前目录 fcb
    fcb* fcbptr = (fcb*)buf;
    for (i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb)); i++, fcbptr++) {
        if (fcbptr->free == 1) {
            if (fcbptr->attribute == 0) {
                printf("<DIR> %-8s\t%d/%d/%d %d:%d\n",
                    fcbptr->filename,
                    (fcbptr->date >> 9) + 2000,
                    (fcbptr->date >> 5) & 0x000f,
                    (fcbptr->date) & 0x001f,
                    (fcbptr->time >> 11),
                    (fcbptr->time >> 5) & 0x003f);
            }
            else {
                printf("<---> %-8s\t%d/%d/%d %d:%d\t%d\n",
                    fcbptr->filename,
                    (fcbptr->date >> 9) + 2000,
                    (fcbptr->date >> 5) & 0x000f,
                    (fcbptr->date) & 0x001f,
                    (fcbptr->time >> 11),
                    (fcbptr->time >> 5) & 0x003f,
                    fcbptr->length);
            }
        }
    }
}

int mds(char* filename)
{
    //创建一个目录
    // 非法判断
    if (strcmp(filename, "") == 0) {
        printf("%splease input filename\n", "<error>: ");
        return -1;
    }
    if (openfilelist[currfd].attribute == 1) {
        printf("%syou are in data file now\n", "<error>: ");
        return -1;
    }
    openfilelist[currfd].count = 0;
    char buf[MAX_TEXT_SIZE];
    do_read(currfd, openfilelist[currfd].length, buf);
    int i;
    fcb* fcbptr = (fcb*)buf;
    // 检查重名
    for (i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb)); i++, fcbptr++) {
        if (fcbptr->free == 0) {
            continue;
        }
        if (strcmp(fcbptr->filename, filename) == 0 && fcbptr->attribute == 1) {
            printf("<error>: the same filename error\n");
            return -1;
        }
    }
    // 申请空 fcb;
    fcbptr = (fcb*)buf;
    for (i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb)); i++, fcbptr++) {
        if (fcbptr->free == 0)
            break;
    }
    // 申请磁盘块并更新 fat 表
    int block_num = get_free_block();
    if (block_num == -1) {
        return -1;
    }
    fat* fat1 = (fat*)(myvhard + BLOCKSIZE);
    fat* fat2 = (fat*)(myvhard + BLOCKSIZE * 3);
    fat1[block_num].id = END;
    memcpy(fat2, fat1, BLOCKSIZE * 2);
    // 修改 fcb 信息
    strcpy(fcbptr->filename, filename);
    time_t rawtime = time(NULL);
    struct tm* time = localtime(&rawtime);
    fcbptr->date = (time->tm_year - 100) * 512 + (time->tm_mon + 1) * 32 + (time->tm_mday);
    fcbptr->time = (time->tm_hour) * 2048 + (time->tm_min) * 32 + (time->tm_sec) / 2;
    fcbptr->first = block_num;
    fcbptr->free = 1;
    fcbptr->attribute = 1;
    fcbptr->length = 0;
    openfilelist[currfd].count = i * sizeof(fcb);
    do_write(currfd, (char*)fcbptr, sizeof(fcb), 2);
    // 修改父目录 fcb
    fcbptr = (fcb*)buf;
    fcbptr->length = openfilelist[currfd].length;
    openfilelist[currfd].count = 0;
    do_write(currfd, (char*)fcbptr, sizeof(fcb), 2);
    openfilelist[currfd].fcbstate = 1;
}

void mdk(char* dirname)
{
    /**
     * 当前目录：当前打开目录项表示的目录
     * 该目录：以下指创建的目录
     * 父目录：指该目录的父目录
     * 如:
     * 我现在在 root 目录下， 输入命令 mkdir a/b/bb
     * 表示 在 root 目录下的 a 目录下的 b 目录中创建 bb 目录
     * 这时，父目录指 b，该目录指 bb，当前目录指 root
     * 以下都用这个表达，简单情况下，当前目录和父目录是一个目录
     * 先讨论简单情况，即 mkdir bb
     */
    int i = 0;
    char text[MAX_TEXT_SIZE];
    char* fname = strtok(dirname, ".");
    char* exname = strtok(NULL, ".");
    if (exname != NULL) {
        printf("%syou can not use extension\n", "<error>: ");
        return;
    }
    // 读取父目录信息
    openfilelist[currfd].count = 0;
    int filelen = do_read(currfd, openfilelist[currfd].length, text);
    fcb* fcbptr = (fcb*)text;
    // 查找是否重名
    for (i = 0; i < (int)(filelen / sizeof(fcb)); i++) {
        if (strcmp(dirname, fcbptr[i].filename) == 0 && fcbptr->attribute == 0) {
            printf("%sdir has existed\n", "<error>: ");
            return;
        }
    }
    // 申请一个打开目录表项
    int fd = get_free_openfilelist();
    if (fd == -1) {
        printf("<error>: openfilelist is full\n");
        return;
    }
    // 申请一个磁盘块
    unsigned short int block_num = get_free_block();
    if (block_num == END) {
        printf("<error>: blocks are full\n");
        openfilelist[fd].topenfile = 0;
        return;
    }
    // 更新 fat 表
    fat* fat1 = (fat*)(myvhard + BLOCKSIZE);
    fat* fat2 = (fat*)(myvhard + BLOCKSIZE * 3);
    fat1[block_num].id = END;
    fat2[block_num].id = END;
    // 在父目录中找一个空的 fcb，分配给该目录  ??未考虑父目录满的情况??
    for (i = 0; i < (int)(filelen / sizeof(fcb)); i++) {
        if (fcbptr[i].free == 0) {
            break;
        }
    }
    openfilelist[currfd].count = i * sizeof(fcb);
    openfilelist[currfd].fcbstate = 1;
    // 初始化该 fcb
    fcb* fcbtmp = (fcb*)malloc(sizeof(fcb));
    fcbtmp->attribute = 0;
    time_t rawtime = time(NULL);
    struct tm* time = localtime(&rawtime);
    fcbtmp->date = (time->tm_year - 100) * 512 + (time->tm_mon + 1) * 32 + (time->tm_mday);
    fcbtmp->time = (time->tm_hour) * 2048 + (time->tm_min) * 32 + (time->tm_sec) / 2;
    strcpy(fcbtmp->filename, dirname);
    strcpy(fcbtmp->exname, "di");
    fcbtmp->first = block_num;
    fcbtmp->length = 2 * sizeof(fcb);
    fcbtmp->free = 1;
    do_write(currfd, (char*)fcbtmp, sizeof(fcb), 2);
    // 设置打开文件表项
    openfilelist[fd].attribute = 0;
    openfilelist[fd].count = 0;
    openfilelist[fd].date = fcbtmp->date;
    openfilelist[fd].time = fcbtmp->time;
    openfilelist[fd].dirno = openfilelist[currfd].first;
    openfilelist[fd].diroff = i;
    strcpy(openfilelist[fd].exname, "di");
    strcpy(openfilelist[fd].filename, dirname);
    openfilelist[fd].fcbstate = 0;
    openfilelist[fd].first = fcbtmp->first;
    openfilelist[fd].free = fcbtmp->free;
    openfilelist[fd].length = fcbtmp->length;
    openfilelist[fd].topenfile = 1;
    strcat(strcat(strcpy(openfilelist[fd].dir, (char*)(openfilelist[currfd].dir)), dirname), "/");
    // 设置 . 和 .. 目录
    fcbtmp->attribute = 0;
    fcbtmp->date = fcbtmp->date;
    fcbtmp->time = fcbtmp->time;
    strcpy(fcbtmp->filename, ".");
    strcpy(fcbtmp->exname, "di");
    fcbtmp->first = block_num;
    fcbtmp->length = 2 * sizeof(fcb);
    do_write(fd, (char*)fcbtmp, sizeof(fcb), 2);
    fcb* fcbtmp2 = (fcb*)malloc(sizeof(fcb));
    memcpy(fcbtmp2, fcbtmp, sizeof(fcb));
    strcpy(fcbtmp2->filename, "..");
    fcbtmp2->first = openfilelist[currfd].first;
    fcbtmp2->length = openfilelist[currfd].length;
    fcbtmp2->date = openfilelist[currfd].date;
    fcbtmp2->time = openfilelist[currfd].time;
    do_write(fd, (char*)fcbtmp2, sizeof(fcb), 2);
    // 关闭该目录的打开文件表项，close 会修改父目录中对应该目录的 fcb 信息
    /**
     * 这里注意，一个目录存在 2 个 fcb 信息，一个为该目录下的 . 目录文件，一个为父目录下的 fcb。
     * 因此，这俩个fcb均需要修改，前一个 fcb 由各个函数自己完成，后一个 fcb 修改由 close 完成。
     * 所以这里，需要打开文件表，再关闭文件表，实际上更新了后一个 fcb 信息。
     */
    my_close(fd);
    free(fcbtmp);
    free(fcbtmp2);
    // 修改父目录 fcb
    fcbptr = (fcb*)text;
    fcbptr->length = openfilelist[currfd].length;
    openfilelist[currfd].count = 0;
    do_write(currfd, (char*)fcbptr, sizeof(fcb), 2);
    openfilelist[currfd].fcbstate = 1;
}

void my_mkdirst(char* dirname)
{
    /**
     * 当前目录：当前打开目录项表示的目录
     * 该目录：以下指创建的目录
     * 父目录：指该目录的父目录
     * 如:
     * 我现在在 root 目录下， 输入命令 mkdir a/b/bb
     * 表示 在 root 目录下的 a 目录下的 b 目录中创建 bb 目录
     * 这时，父目录指 b，该目录指 bb，当前目录指 root
     * 以下都用这个表达，简单情况下，当前目录和父目录是一个目录
     * 来不及了，先讨论简单情况，即 mkdir bb
     */
    int i = 0;
    char text[MAX_TEXT_SIZE];
    char* fname = strtok(dirname, ".");
    char* exname = strtok(NULL, ".");
    if (exname != NULL) {
        printf("<error>: you can not use extension\n");
        return;
    }
    // 读取父目录信息
    openfilelist[currfd].count = 0;
    int filelen = do_read(currfd, openfilelist[currfd].length, text);
    fcb* fcbptr = (fcb*)text;
    // 查找是否重名
    for (i = 0; i < (int)(filelen / sizeof(fcb)); i++) {
        if (strcmp(dirname, fcbptr[i].filename) == 0 && fcbptr->attribute == 0) {
            printf("<error>: dir has existed\n");
            return;
        }
    }
    // 申请一个打开目录表项
    int fd = get_free_openfilelist();
    if (fd == -1) {
        printf("<error>: openfilelist is full\n");
        return;
    }
    // 申请一个磁盘块
    unsigned short int block_num = get_free_block();
    if (block_num == END) {
        printf("<error>: blocks are full\n");
        openfilelist[fd].topenfile = 0;
        return;
    }
    // 更新 fat 表
    fat* fat1 = (fat*)(myvhard + BLOCKSIZE);
    fat* fat2 = (fat*)(myvhard + BLOCKSIZE * 3);
    fat1[block_num].id = END;
    fat2[block_num].id = END;
    // 在父目录中找一个空的 fcb，分配给该目录  ??未考虑父目录满的情况??
    for (i = 0; i < (int)(filelen / sizeof(fcb)); i++) {
        if (fcbptr[i].free == 0) {
            break;
        }
    }
    openfilelist[currfd].count = i * sizeof(fcb);
    openfilelist[currfd].fcbstate = 1;
    // 初始化该 fcb
    fcb* fcbtmp = (fcb*)malloc(sizeof(fcb));
    fcbtmp->attribute = 0;
    time_t rawtime = time(NULL);
    struct tm* time = localtime(&rawtime);
    fcbtmp->date = (time->tm_year - 100) * 512 + (time->tm_mon + 1) * 32 + (time->tm_mday);
    fcbtmp->time = (time->tm_hour) * 2048 + (time->tm_min) * 32 + (time->tm_sec) / 2;
    strcpy(fcbtmp->filename, dirname);
    strcpy(fcbtmp->exname, "di");
    fcbtmp->first = block_num;
    fcbtmp->length = 2 * sizeof(fcb);
    fcbtmp->free = 1;
    do_write(currfd, (char*)fcbtmp, sizeof(fcb), 2);
    // 设置打开文件表项
    openfilelist[fd].attribute = 0;
    openfilelist[fd].count = 0;
    openfilelist[fd].date = fcbtmp->date;
    openfilelist[fd].time = fcbtmp->time;
    openfilelist[fd].dirno = openfilelist[currfd].first;
    openfilelist[fd].diroff = i;
    strcpy(openfilelist[fd].exname, "di");
    strcpy(openfilelist[fd].filename, dirname);
    openfilelist[fd].fcbstate = 0;
    openfilelist[fd].first = fcbtmp->first;
    openfilelist[fd].free = fcbtmp->free;
    openfilelist[fd].length = fcbtmp->length;
    openfilelist[fd].topenfile = 1;
    strcat(strcat(strcpy(openfilelist[fd].dir, (char*)(openfilelist[currfd].dir)), dirname), "/");
    // 设置 . 和 .. 目录
    fcbtmp->attribute = 0;
    fcbtmp->date = fcbtmp->date;
    fcbtmp->time = fcbtmp->time;
    strcpy(fcbtmp->filename, ".");
    strcpy(fcbtmp->exname, "di");
    fcbtmp->first = block_num;
    fcbtmp->length = 2 * sizeof(fcb);
    do_write(fd, (char*)fcbtmp, sizeof(fcb), 2);
    fcb* fcbtmp2 = (fcb*)malloc(sizeof(fcb));
    memcpy(fcbtmp2, fcbtmp, sizeof(fcb));
    strcpy(fcbtmp2->filename, "..");
    fcbtmp2->first = openfilelist[currfd].first;
    fcbtmp2->length = openfilelist[currfd].length;
    fcbtmp2->date = openfilelist[currfd].date;
    fcbtmp2->time = openfilelist[currfd].time;
    do_write(fd, (char*)fcbtmp2, sizeof(fcb), 2);
    // 关闭该目录的打开文件表项，close 会修改父目录中对应该目录的 fcb 信息
    /**
     * 这里注意，一个目录存在 2 个 fcb 信息，一个为该目录下的 . 目录文件，一个为父目录下的 fcb。
     * 因此，这俩个fcb均需要修改，前一个 fcb 由各个函数自己完成，后一个 fcb 修改由 close 完成。
     * 所以这里，需要打开文件表，再关闭文件表，实际上更新了后一个 fcb 信息。
     */
    my_close(fd);
    free(fcbtmp);
    free(fcbtmp2);
    // 修改父目录 fcb
    fcbptr = (fcb*)text;
    fcbptr->length = openfilelist[currfd].length;
    openfilelist[currfd].count = 0;
    do_write(currfd, (char*)fcbptr, sizeof(fcb), 2);
    openfilelist[currfd].fcbstate = 1;
}

int TYPE(char* filename)
{
    //显示文本文件的内容
    char buf[MAX_TEXT_SIZE];
    openfilelist[currfd].count = 0;
    do_read(currfd, openfilelist[currfd].length, buf);
    int i, flag = 0;
    fcb* fcbptr = (fcb*)buf;
    // 重名检查
    for (i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb)); i++, fcbptr++) {
        if (strcmp(fcbptr->filename, filename) == 0 && fcbptr->attribute == 1) {
            flag = 1;
            break;
        }
    }
    if (flag != 1) {
        printf("<error>: no such file\n");
        return -1;
    }
    // 申请新的打开目录项并初始化该目录项
    int fd = get_free_openfilelist();
    if (fd == -1) {
        printf("<error>: my_open: full openfilelist\n");
        return -1;
    }
    openfilelist[fd].attribute = 1;
    openfilelist[fd].count = 0;
    openfilelist[fd].date = fcbptr->date;
    openfilelist[fd].time = fcbptr->time;
    openfilelist[fd].length = fcbptr->length;
    openfilelist[fd].first = fcbptr->first;
    openfilelist[fd].free = 1;
    strcpy(openfilelist[fd].filename, fcbptr->filename);
    strcat(strcpy(openfilelist[fd].dir, (char*)(openfilelist[currfd].dir)), filename);
    openfilelist[fd].dirno = openfilelist[currfd].first;
    openfilelist[fd].diroff = i;
    openfilelist[fd].topenfile = 1;
    openfilelist[fd].fcbstate = 0;
    currfd = fd;
    return 1;
}

int REN(int fd)
{
    //重新命名文件
    if (fd > MAXOPENFILE || fd < 0) {
        printf("<error>: my_close: fd error\n");
        return -1;
    }
    int i;
    char buf[MAX_TEXT_SIZE];
    int father_fd = -1;
    fcb* fcbptr;
    for (i = 0; i < MAXOPENFILE; i++) {
        if (openfilelist[i].first == openfilelist[fd].dirno) {
            father_fd = i;
            break;
        }
    }
    if (father_fd == -1) {
        printf("<error>: my_close: no father dir\n");
        return -1;
    }
    if (openfilelist[fd].fcbstate == 1) {
        do_read(father_fd, openfilelist[father_fd].length, buf);
        // update fcb
        fcbptr = (fcb*)(buf + sizeof(fcb) * openfilelist[fd].diroff);
        strcpy(fcbptr->exname, openfilelist[fd].exname);
        strcpy(fcbptr->filename, openfilelist[fd].filename);
        fcbptr->first = openfilelist[fd].first;
        fcbptr->free = openfilelist[fd].free;
        fcbptr->length = openfilelist[fd].length;
        fcbptr->time = openfilelist[fd].time;
        fcbptr->date = openfilelist[fd].date;
        fcbptr->attribute = openfilelist[fd].attribute;
        openfilelist[father_fd].count = openfilelist[fd].diroff * sizeof(fcb);
        do_write(father_fd, (char*)fcbptr, sizeof(fcb), 2);
    }
    // 释放打开文件表
    memset(&openfilelist[fd], 0, sizeof(useropen));
    currfd = father_fd;
    return father_fd;
}

int md(char* filename)
{
    //创建一个目录
    // 非法判断
    if (strcmp(filename, "") == 0) {
        printf("<error>: please input filename\n");
        return -1;
    }
    if (openfilelist[currfd].attribute == 1) {
        printf("<error>: you are in data file now\n");
        return -1;
    }
    openfilelist[currfd].count = 0;
    char buf[MAX_TEXT_SIZE];
    do_read(currfd, openfilelist[currfd].length, buf);
    int i;
    fcb* fcbptr = (fcb*)buf;
    // 检查重名
    for (i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb)); i++, fcbptr++) {
        if (fcbptr->free == 0) {
            continue;
        }
        if (strcmp(fcbptr->filename, filename) == 0 && fcbptr->attribute == 1) {
            printf("<error>: the same filename error\n");
            return -1;
        }
    }
    // 申请空 fcb;
    fcbptr = (fcb*)buf;
    for (i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb)); i++, fcbptr++) {
        if (fcbptr->free == 0)
            break;
    }
    // 申请磁盘块并更新 fat 表
    int block_num = get_free_block();
    if (block_num == -1) {
        return -1;
    }
    fat* fat1 = (fat*)(myvhard + BLOCKSIZE);
    fat* fat2 = (fat*)(myvhard + BLOCKSIZE * 3);
    fat1[block_num].id = END;
    memcpy(fat2, fat1, BLOCKSIZE * 2);
    // 修改 fcb 信息
    strcpy(fcbptr->filename, filename);
    time_t rawtime = time(NULL);
    struct tm* time = localtime(&rawtime);
    fcbptr->date = (time->tm_year - 100) * 512 + (time->tm_mon + 1) * 32 + (time->tm_mday);
    fcbptr->time = (time->tm_hour) * 2048 + (time->tm_min) * 32 + (time->tm_sec) / 2;
    fcbptr->first = block_num;
    fcbptr->free = 1;
    fcbptr->attribute = 1;
    fcbptr->length = 0;
    openfilelist[currfd].count = i * sizeof(fcb);
    do_write(currfd, (char*)fcbptr, sizeof(fcb), 2);
    // 修改父目录 fcb
    fcbptr = (fcb*)buf;
    fcbptr->length = openfilelist[currfd].length;
    openfilelist[currfd].count = 0;
    do_write(currfd, (char*)fcbptr, sizeof(fcb), 2);
    openfilelist[currfd].fcbstate = 1;
}


//void TREE()
//{
//    // 判断是否是目录
//    if (openfilelist[currfd].attribute == 1) {
//        printf("data file\n");
//        return;
//    }
//    char buf[MAX_TEXT_SIZE];
//    int i;
//    // 读取当前目录文件信息(一个个fcb), 载入内存
//    openfilelist[currfd].count = 0;
//    do_read(currfd, openfilelist[currfd].length, buf);
//    // 遍历当前目录 fcb
//    fcb* fcbptr = (fcb*)buf;
//    for (i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb)); i++, fcbptr++) {
//        if (fcbptr->free == 1) {
//            if (fcbptr->attribute == 0) {
//                printf("<DIR> %-8s\t%d/%d/%d %d:%d\n",
//                    fcbptr->filename,
//                    (fcbptr->date >> 9) + 2000,
//                    (fcbptr->date >> 5) & 0x000f,
//                    (fcbptr->date) & 0x001f,
//                    (fcbptr->time >> 11),
//                    (fcbptr->time >> 5) & 0x003f);
//            }
//            else {
//                printf("<---> %-8s\t%d/%d/%d %d:%d\t%d\n",
//                    fcbptr->filename,
//                    (fcbptr->date >> 9) + 2000,
//                    (fcbptr->date >> 5) & 0x000f,
//                    (fcbptr->date) & 0x001f,
//                    (fcbptr->time >> 11),
//                    (fcbptr->time >> 5) & 0x003f,
//                    fcbptr->length);
//            }
//        }
//    }
//}