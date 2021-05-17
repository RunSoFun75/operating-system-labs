#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>

extern char* tzname[];

int main(int argc, char *argv[]) {
    time_t now;
    struct tm *sp;
    putenv("TZ=America/Los_Angeles");
    (void) time(&now);
    printf("%s", ctime(&now));
    sp = localtime(&now);
    printf("%d/%d/%d %d:%02d %s\n",
           sp->tm_mon + 1, sp->tm_mday,
           sp->tm_year + 1900 , sp->tm_hour,
           sp->tm_min, tzname[sp->tm_isdst]);
    exit(0);
}
