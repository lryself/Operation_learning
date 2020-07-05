

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
#define MAXOPENFILE 10 // ͬʱ������ļ���
#define MAX_TEXT_SIZE 10000

typedef struct FCB {
    char filename[8];
    char exname[3];
    unsigned char attribute; // 0: dir file, 1: data file
    unsigned short time;
    unsigned short date;
    unsigned short first;
    unsigned short length;
    char free; // 0: �� fcb
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

    int dirno; // ��Ŀ¼�ļ���ʼ�̿��
    int diroff; // ���ļ���Ӧ�� fcb �ڸ�Ŀ¼�е��߼����
    char dir[MAXOPENFILE][80]; // ȫ·����Ϣ
    int count;
    char fcbstate; // �Ƿ��޸� 1�� 0��
    char topenfile; // 0: �� openfile
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

//������
int main(void)
{
    char cmd[19][10] = {
        "md", "rd", "dir", "cd", "copycon",
        "del", "type", "close", "write", "read",
        "exit", "help", "test", "cls","copy","ren","tree","attrib","clearall"
    };
    char command[30], *sp,*oth,*tp;
    int cmd_idx, i, res;

    printf("��������ģ��fat�ļ�ϵͳ [�汾v1.0]\n");
    printf("���ߣ����1801������ 120181080315\n");
    printf("��ʾ������ָ���������Ķ�README.md  �����дʱ��2020.6.18-2020.7.2\n");
    printf("���г���ǰ��һ���鿴 ʹ����֪ ��������ܶ����\n");
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

//���ú���
void startsys()
{
    /**
     * ��������ļ�ϵͳ������ FILENAME ����ļ� �� ��ͷΪħ������
     * �� root Ŀ¼������ļ���
     * ���򣬵��� my_format �����ļ�ϵͳ�������롣
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
     * ���ιر� ���ļ��� д�� FILENAME �ļ�
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
     * ��ʼ��ǰ������̿�
     * �趨���������̿�Ϊ��Ŀ¼���̿�
     * ��ʼ�� root Ŀ¼�� ���� . �� .. Ŀ¼
     * д�� FILENAME �ļ� ��д����̿ռ䣩
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
    // �ж��Ƿ���Ŀ¼
    if (openfilelist[currfd].attribute == 1) {
        printf("data file\n");
        return;
    }
    char buf[MAX_TEXT_SIZE];
    int i;

    // ��ȡ��ǰĿ¼�ļ���Ϣ(һ����fcb), �����ڴ�
    openfilelist[currfd].count = 0;
    do_read(currfd, openfilelist[currfd].length, buf);

    // ������ǰĿ¼ fcb
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
     * ��ǰĿ¼����ǰ��Ŀ¼���ʾ��Ŀ¼
     * ��Ŀ¼������ָ������Ŀ¼
     * ��Ŀ¼��ָ��Ŀ¼�ĸ�Ŀ¼
     * ��:
     * �������� root Ŀ¼�£� �������� mkdir a/b/bb
     * ��ʾ �� root Ŀ¼�µ� a Ŀ¼�µ� b Ŀ¼�д��� bb Ŀ¼
     * ��ʱ����Ŀ¼ָ b����Ŀ¼ָ bb����ǰĿ¼ָ root
     * ���¶��������������£���ǰĿ¼�͸�Ŀ¼��һ��Ŀ¼
     * �������ˣ������ۼ�������� mkdir bb
     */
    int i = 0;
    char text[MAX_TEXT_SIZE];

    char* fname = strtok(dirname, ".");
    char* exname = strtok(NULL, ".");
    if (exname != NULL) {
        printf("you can not use extension\n");
        return 0;
    }
    // ��ȡ��Ŀ¼��Ϣ
    openfilelist[currfd].count = 0;
    int filelen = do_read(currfd, openfilelist[currfd].length, text);
    fcb* fcbptr = (fcb*)text;

    // �����Ƿ�����
    for (i = 0; i < (int)(filelen / sizeof(fcb)); i++) {
        if (strcmp(dirname, fcbptr[i].filename) == 0 && fcbptr->attribute == 0) {
            //printf("dir has existed\n");
            return -1;
        }
    }

    // ����һ����Ŀ¼����
    int fd = get_free_openfilelist();
    if (fd == -1) {
        //printf("openfilelist is full\n");
        return -2;
    }
    // ����һ�����̿�
    unsigned short int block_num = get_free_block();
    if (block_num == END) {
        //printf("blocks are full\n");
        openfilelist[fd].topenfile = 0;
        return -3;
    }
    // ���� fat ��
    fat* fat1 = (fat*)(myvhard + BLOCKSIZE);
    fat* fat2 = (fat*)(myvhard + BLOCKSIZE * 3);
    fat1[block_num].id = END;
    fat2[block_num].id = END;

    // �ڸ�Ŀ¼����һ���յ� fcb���������Ŀ¼  ??δ���Ǹ�Ŀ¼�������??
    for (i = 0; i < (int)(filelen / sizeof(fcb)); i++) {
        if (fcbptr[i].free == 0) {
            break;
        }
    }
    openfilelist[currfd].count = i * sizeof(fcb);
    openfilelist[currfd].fcbstate = 1;
    // ��ʼ���� fcb
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

    // ���ô��ļ�����
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

    // ���� . �� .. Ŀ¼
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

    // �رո�Ŀ¼�Ĵ��ļ����close ���޸ĸ�Ŀ¼�ж�Ӧ��Ŀ¼�� fcb ��Ϣ
    /**
     * ����ע�⣬һ��Ŀ¼���� 2 �� fcb ��Ϣ��һ��Ϊ��Ŀ¼�µ� . Ŀ¼�ļ���һ��Ϊ��Ŀ¼�µ� fcb��
     * ��ˣ�������fcb����Ҫ�޸ģ�ǰһ�� fcb �ɸ��������Լ���ɣ���һ�� fcb �޸��� close ��ɡ�
     * ���������Ҫ���ļ����ٹر��ļ���ʵ���ϸ����˺�һ�� fcb ��Ϣ��
     */
    my_close(fd);

    free(fcbtmp);
    free(fcbtmp2);

    // �޸ĸ�Ŀ¼ fcb
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

    // �ų� . �� .. Ŀ¼
    if (strcmp(dirname, ".") == 0 || strcmp(dirname, "..") == 0) {
        printf("%scan not remove . and .. special dir\n", "<error>: ");
        return;
    }
    openfilelist[currfd].count = 0;
    do_read(currfd, openfilelist[currfd].length, buf);

    // ����Ҫɾ����Ŀ¼
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
    // �޷�ɾ���ǿ�Ŀ¼
    if (fcbptr->length > 2 * sizeof(fcb)) {
        printf("%scan not remove a non empty dir\n", "<error>: ");
        return;
    }

    // ���� fat ��
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

    // ���� fcb
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

    // ɾ��Ŀ¼��Ҫ��Ӧ���ǿ���ɾ�� fcb��Ҳ�����޸ĸ�Ŀ¼ length
    // ������Ҫע�⣺��Ϊɾ���м�� fcb��Ŀ¼��Ч���Ȳ��䣬�� length ����
    // �����Ҫ���������������ɾ�����һ�� fcb ʱ�����п���֮ǰ�� fcb ���ǿյģ�������Ҫ
    // ѭ��ɾ�� fcb (���´������)��������Ҫ���� block �޸� fat ��ȹ���(do_write ���)
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

    // ���¸�Ŀ¼ fcb
    fcbptr = (fcb*)buf;
    fcbptr->length = openfilelist[currfd].length;
    openfilelist[currfd].count = 0;
    do_write(currfd, (char*)fcbptr, sizeof(fcb), 2);

    openfilelist[currfd].fcbstate = 1;
}

int my_create(char* filename)
{
    // �Ƿ��ж�
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
    // �������
    for (i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb)); i++, fcbptr++) {
        if (fcbptr->free == 0) {
            continue;
        }
        if (strcmp(fcbptr->filename, filename) == 0 && fcbptr->attribute == 1) {
            printf("<error>: the same filename error\n");
            return -1;
        }
    }

    // ����� fcb;
    fcbptr = (fcb*)buf;
    for (i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb)); i++, fcbptr++) {
        if (fcbptr->free == 0)
            break;
    }
    // ������̿鲢���� fat ��
    int block_num = get_free_block();
    if (block_num == -1) {
        return -1;
    }
    fat* fat1 = (fat*)(myvhard + BLOCKSIZE);
    fat* fat2 = (fat*)(myvhard + BLOCKSIZE * 3);
    fat1[block_num].id = END;
    memcpy(fat2, fat1, BLOCKSIZE * 2);

    // �޸� fcb ��Ϣ
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

    // �޸ĸ�Ŀ¼ fcb
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
    // ��ѯ
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

    // ���� fat ��
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

    // ��� fcb
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

    // �޸ĸ�Ŀ¼ . Ŀ¼�ļ��� fcb
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
    // �������
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

    // �����µĴ�Ŀ¼���ʼ����Ŀ¼��
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
    // ����Ŀ�� fcb
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
        // . �� .. ���
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
            // ����Ŀ¼
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
    // �ͷŴ��ļ���
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
        // 1: �ض�д�����ȫ�����ݣ���ͷ��ʼд
        // 2. ����д�����ļ�ָ�봦��ʼд
        // 3. ׷��д��������˼
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

    // ��λ��ȡĿ����̿�Ϳ��ڵ�ַ
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

    // ��ȡ����
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
        // ׷��д�������һ���ļ�������Ҫ��ɾ��ĩβ \0������ָ���Ƶ�ĩλ��һ���ֽڴ�
        openfilelist[fd].count = openfilelist[fd].length;
        if (openfilelist[fd].attribute == 1) {
            if (openfilelist[fd].length != 0) {
                // �ǿ��ļ�
                openfilelist[fd].count = openfilelist[fd].length - 1;
            }
        }
    }

    int off = openfilelist[fd].count;

    // ��λ���̿�Ϳ���ƫ����
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
    // д�����
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

        // д�������̫�࣬��Ҫд����һ�����̿飬���û�д��̿飬������һ��
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

    // ɾ������Ĵ��̿�
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
    //����Ŀ¼
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
    // �ж��Ƿ���Ŀ¼
    if (openfilelist[currfd].attribute == 1) {
        printf("<error>: data file\n");
        return;
    }
    // ��ȡ��ǰĿ¼�ļ���Ϣ(һ����fcb), �����ڴ�
    openfilelist[currfd].count = 0;
    do_read(currfd, openfilelist[currfd].length, buf);

    // ������ǰĿ¼ fcb
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
     * ��������copy
     * ��  �ܣ��ļ�����
     * ��  ����
     *     srcFilePath --- Դ�ļ�·��
     *     dstFilePath --- Ŀ���ļ�·��
     * ����ֵ����
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


//��������

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

    // �����м�Ŀ¼
    for (i = 0; i < iLen; i++)
    {
        if (pszDir[i] == '\\' || pszDir[i] == '/')
        {
            pszDir[i] = '\0';

            //���������,����
            iRet = access(pszDir, 0);
            if (iRet != 0)
            {
                iRet = mkdir(pszDir);
                if (iRet != 0)
                {
                    return -1;
                }
            }
            //������\����/
            pszDir[i] = '/';
        }
    }

    iRet = mkdir(pszDir);
    free(pszDir);
    return iRet;
}

void dirtree()
{
    // �ж��Ƿ���Ŀ¼
    if (openfilelist[currfd].attribute == 1) {
        printf("%sThis is a data file\n", "<warning>: ");
        return;
    }
    char buf[MAX_TEXT_SIZE];
    int i;
    // ��ȡ��ǰĿ¼�ļ���Ϣ(һ����fcb), �����ڴ�
    openfilelist[currfd].count = 0;
    do_read(currfd, openfilelist[currfd].length, buf);
    // ������ǰĿ¼ fcb
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
    //����һ��Ŀ¼
    // �Ƿ��ж�
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
    // �������
    for (i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb)); i++, fcbptr++) {
        if (fcbptr->free == 0) {
            continue;
        }
        if (strcmp(fcbptr->filename, filename) == 0 && fcbptr->attribute == 1) {
            printf("<error>: the same filename error\n");
            return -1;
        }
    }
    // ����� fcb;
    fcbptr = (fcb*)buf;
    for (i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb)); i++, fcbptr++) {
        if (fcbptr->free == 0)
            break;
    }
    // ������̿鲢���� fat ��
    int block_num = get_free_block();
    if (block_num == -1) {
        return -1;
    }
    fat* fat1 = (fat*)(myvhard + BLOCKSIZE);
    fat* fat2 = (fat*)(myvhard + BLOCKSIZE * 3);
    fat1[block_num].id = END;
    memcpy(fat2, fat1, BLOCKSIZE * 2);
    // �޸� fcb ��Ϣ
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
    // �޸ĸ�Ŀ¼ fcb
    fcbptr = (fcb*)buf;
    fcbptr->length = openfilelist[currfd].length;
    openfilelist[currfd].count = 0;
    do_write(currfd, (char*)fcbptr, sizeof(fcb), 2);
    openfilelist[currfd].fcbstate = 1;
}

void mdk(char* dirname)
{
    /**
     * ��ǰĿ¼����ǰ��Ŀ¼���ʾ��Ŀ¼
     * ��Ŀ¼������ָ������Ŀ¼
     * ��Ŀ¼��ָ��Ŀ¼�ĸ�Ŀ¼
     * ��:
     * �������� root Ŀ¼�£� �������� mkdir a/b/bb
     * ��ʾ �� root Ŀ¼�µ� a Ŀ¼�µ� b Ŀ¼�д��� bb Ŀ¼
     * ��ʱ����Ŀ¼ָ b����Ŀ¼ָ bb����ǰĿ¼ָ root
     * ���¶��������������£���ǰĿ¼�͸�Ŀ¼��һ��Ŀ¼
     * �����ۼ�������� mkdir bb
     */
    int i = 0;
    char text[MAX_TEXT_SIZE];
    char* fname = strtok(dirname, ".");
    char* exname = strtok(NULL, ".");
    if (exname != NULL) {
        printf("%syou can not use extension\n", "<error>: ");
        return;
    }
    // ��ȡ��Ŀ¼��Ϣ
    openfilelist[currfd].count = 0;
    int filelen = do_read(currfd, openfilelist[currfd].length, text);
    fcb* fcbptr = (fcb*)text;
    // �����Ƿ�����
    for (i = 0; i < (int)(filelen / sizeof(fcb)); i++) {
        if (strcmp(dirname, fcbptr[i].filename) == 0 && fcbptr->attribute == 0) {
            printf("%sdir has existed\n", "<error>: ");
            return;
        }
    }
    // ����һ����Ŀ¼����
    int fd = get_free_openfilelist();
    if (fd == -1) {
        printf("<error>: openfilelist is full\n");
        return;
    }
    // ����һ�����̿�
    unsigned short int block_num = get_free_block();
    if (block_num == END) {
        printf("<error>: blocks are full\n");
        openfilelist[fd].topenfile = 0;
        return;
    }
    // ���� fat ��
    fat* fat1 = (fat*)(myvhard + BLOCKSIZE);
    fat* fat2 = (fat*)(myvhard + BLOCKSIZE * 3);
    fat1[block_num].id = END;
    fat2[block_num].id = END;
    // �ڸ�Ŀ¼����һ���յ� fcb���������Ŀ¼  ??δ���Ǹ�Ŀ¼�������??
    for (i = 0; i < (int)(filelen / sizeof(fcb)); i++) {
        if (fcbptr[i].free == 0) {
            break;
        }
    }
    openfilelist[currfd].count = i * sizeof(fcb);
    openfilelist[currfd].fcbstate = 1;
    // ��ʼ���� fcb
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
    // ���ô��ļ�����
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
    // ���� . �� .. Ŀ¼
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
    // �رո�Ŀ¼�Ĵ��ļ����close ���޸ĸ�Ŀ¼�ж�Ӧ��Ŀ¼�� fcb ��Ϣ
    /**
     * ����ע�⣬һ��Ŀ¼���� 2 �� fcb ��Ϣ��һ��Ϊ��Ŀ¼�µ� . Ŀ¼�ļ���һ��Ϊ��Ŀ¼�µ� fcb��
     * ��ˣ�������fcb����Ҫ�޸ģ�ǰһ�� fcb �ɸ��������Լ���ɣ���һ�� fcb �޸��� close ��ɡ�
     * ���������Ҫ���ļ����ٹر��ļ���ʵ���ϸ����˺�һ�� fcb ��Ϣ��
     */
    my_close(fd);
    free(fcbtmp);
    free(fcbtmp2);
    // �޸ĸ�Ŀ¼ fcb
    fcbptr = (fcb*)text;
    fcbptr->length = openfilelist[currfd].length;
    openfilelist[currfd].count = 0;
    do_write(currfd, (char*)fcbptr, sizeof(fcb), 2);
    openfilelist[currfd].fcbstate = 1;
}

void my_mkdirst(char* dirname)
{
    /**
     * ��ǰĿ¼����ǰ��Ŀ¼���ʾ��Ŀ¼
     * ��Ŀ¼������ָ������Ŀ¼
     * ��Ŀ¼��ָ��Ŀ¼�ĸ�Ŀ¼
     * ��:
     * �������� root Ŀ¼�£� �������� mkdir a/b/bb
     * ��ʾ �� root Ŀ¼�µ� a Ŀ¼�µ� b Ŀ¼�д��� bb Ŀ¼
     * ��ʱ����Ŀ¼ָ b����Ŀ¼ָ bb����ǰĿ¼ָ root
     * ���¶��������������£���ǰĿ¼�͸�Ŀ¼��һ��Ŀ¼
     * �������ˣ������ۼ�������� mkdir bb
     */
    int i = 0;
    char text[MAX_TEXT_SIZE];
    char* fname = strtok(dirname, ".");
    char* exname = strtok(NULL, ".");
    if (exname != NULL) {
        printf("<error>: you can not use extension\n");
        return;
    }
    // ��ȡ��Ŀ¼��Ϣ
    openfilelist[currfd].count = 0;
    int filelen = do_read(currfd, openfilelist[currfd].length, text);
    fcb* fcbptr = (fcb*)text;
    // �����Ƿ�����
    for (i = 0; i < (int)(filelen / sizeof(fcb)); i++) {
        if (strcmp(dirname, fcbptr[i].filename) == 0 && fcbptr->attribute == 0) {
            printf("<error>: dir has existed\n");
            return;
        }
    }
    // ����һ����Ŀ¼����
    int fd = get_free_openfilelist();
    if (fd == -1) {
        printf("<error>: openfilelist is full\n");
        return;
    }
    // ����һ�����̿�
    unsigned short int block_num = get_free_block();
    if (block_num == END) {
        printf("<error>: blocks are full\n");
        openfilelist[fd].topenfile = 0;
        return;
    }
    // ���� fat ��
    fat* fat1 = (fat*)(myvhard + BLOCKSIZE);
    fat* fat2 = (fat*)(myvhard + BLOCKSIZE * 3);
    fat1[block_num].id = END;
    fat2[block_num].id = END;
    // �ڸ�Ŀ¼����һ���յ� fcb���������Ŀ¼  ??δ���Ǹ�Ŀ¼�������??
    for (i = 0; i < (int)(filelen / sizeof(fcb)); i++) {
        if (fcbptr[i].free == 0) {
            break;
        }
    }
    openfilelist[currfd].count = i * sizeof(fcb);
    openfilelist[currfd].fcbstate = 1;
    // ��ʼ���� fcb
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
    // ���ô��ļ�����
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
    // ���� . �� .. Ŀ¼
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
    // �رո�Ŀ¼�Ĵ��ļ����close ���޸ĸ�Ŀ¼�ж�Ӧ��Ŀ¼�� fcb ��Ϣ
    /**
     * ����ע�⣬һ��Ŀ¼���� 2 �� fcb ��Ϣ��һ��Ϊ��Ŀ¼�µ� . Ŀ¼�ļ���һ��Ϊ��Ŀ¼�µ� fcb��
     * ��ˣ�������fcb����Ҫ�޸ģ�ǰһ�� fcb �ɸ��������Լ���ɣ���һ�� fcb �޸��� close ��ɡ�
     * ���������Ҫ���ļ����ٹر��ļ���ʵ���ϸ����˺�һ�� fcb ��Ϣ��
     */
    my_close(fd);
    free(fcbtmp);
    free(fcbtmp2);
    // �޸ĸ�Ŀ¼ fcb
    fcbptr = (fcb*)text;
    fcbptr->length = openfilelist[currfd].length;
    openfilelist[currfd].count = 0;
    do_write(currfd, (char*)fcbptr, sizeof(fcb), 2);
    openfilelist[currfd].fcbstate = 1;
}

int TYPE(char* filename)
{
    //��ʾ�ı��ļ�������
    char buf[MAX_TEXT_SIZE];
    openfilelist[currfd].count = 0;
    do_read(currfd, openfilelist[currfd].length, buf);
    int i, flag = 0;
    fcb* fcbptr = (fcb*)buf;
    // �������
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
    // �����µĴ�Ŀ¼���ʼ����Ŀ¼��
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
    //���������ļ�
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
    // �ͷŴ��ļ���
    memset(&openfilelist[fd], 0, sizeof(useropen));
    currfd = father_fd;
    return father_fd;
}

int md(char* filename)
{
    //����һ��Ŀ¼
    // �Ƿ��ж�
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
    // �������
    for (i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb)); i++, fcbptr++) {
        if (fcbptr->free == 0) {
            continue;
        }
        if (strcmp(fcbptr->filename, filename) == 0 && fcbptr->attribute == 1) {
            printf("<error>: the same filename error\n");
            return -1;
        }
    }
    // ����� fcb;
    fcbptr = (fcb*)buf;
    for (i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb)); i++, fcbptr++) {
        if (fcbptr->free == 0)
            break;
    }
    // ������̿鲢���� fat ��
    int block_num = get_free_block();
    if (block_num == -1) {
        return -1;
    }
    fat* fat1 = (fat*)(myvhard + BLOCKSIZE);
    fat* fat2 = (fat*)(myvhard + BLOCKSIZE * 3);
    fat1[block_num].id = END;
    memcpy(fat2, fat1, BLOCKSIZE * 2);
    // �޸� fcb ��Ϣ
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
    // �޸ĸ�Ŀ¼ fcb
    fcbptr = (fcb*)buf;
    fcbptr->length = openfilelist[currfd].length;
    openfilelist[currfd].count = 0;
    do_write(currfd, (char*)fcbptr, sizeof(fcb), 2);
    openfilelist[currfd].fcbstate = 1;
}


//void TREE()
//{
//    // �ж��Ƿ���Ŀ¼
//    if (openfilelist[currfd].attribute == 1) {
//        printf("data file\n");
//        return;
//    }
//    char buf[MAX_TEXT_SIZE];
//    int i;
//    // ��ȡ��ǰĿ¼�ļ���Ϣ(һ����fcb), �����ڴ�
//    openfilelist[currfd].count = 0;
//    do_read(currfd, openfilelist[currfd].length, buf);
//    // ������ǰĿ¼ fcb
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