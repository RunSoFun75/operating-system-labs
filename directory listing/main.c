#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <time.h>
#include <libgen.h>
#include <string.h>

void getmodestr(struct stat *stbuf, char *mode) {
    switch (stbuf->st_mode & S_IFMT) {
        case S_IFDIR:
            mode[0] = 'd';
            break;
        case S_IFREG:
            mode[0] = '-';
            break;
        default:
            mode[0] = '?';
    }
    mode[1] = (stbuf->st_mode & S_IRUSR) ? 'r' : '-';
    mode[2] = (stbuf->st_mode & S_IWUSR) ? 'w' : '-';
    mode[3] = (stbuf->st_mode & S_IXUSR) ? 'x' : '-';
    mode[4] = (stbuf->st_mode & S_IRGRP) ? 'r' : '-';
    mode[5] = (stbuf->st_mode & S_IWGRP) ? 'w' : '-';
    mode[6] = (stbuf->st_mode & S_IXGRP) ? 'x' : '-';
    mode[7] = (stbuf->st_mode & S_IROTH) ? 'r' : '-';
    mode[8] = (stbuf->st_mode & S_IWOTH) ? 'w' : '-';
    mode[9] = (stbuf->st_mode & S_IXOTH) ? 'r' : '-';
}

int main(int argc, char *argv[]) {
    struct stat stbuf;
    char mode[] = "-rwxrwxrwx";
    struct passwd *passwd;
    struct group *group;
    for (int i = 1; i < argc; ++i) {
        if (lstat(argv[i], &stbuf) == -1) {
            perror(argv[i]);
            continue;
        }
        getmodestr(&stbuf, mode);
        passwd = getpwuid(stbuf.st_uid);
        group = getgrgid(stbuf.st_gid);
        printf("%s %3lu ", mode, stbuf.st_nlink);
        if (passwd == NULL) {
            printf("%d ", stbuf.st_uid);
        } else {
            printf("%s ", passwd->pw_name);
        }
        if (group == NULL) {
            printf("%d ", stbuf.st_gid);
        } else {
            printf("%s ", group->gr_name);
        }
        printf("%7ld %.24s %s\n", stbuf.st_size,
               ctime(&stbuf.st_mtime), basename(argv[i]));
    }
    return 0;
}
