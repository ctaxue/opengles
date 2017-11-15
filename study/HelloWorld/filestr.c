#include<stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<string.h>


int main(void)
{
    char *ch = NULL;
    ch = file2str("/etc/passwd");
    if(!ch)
    {
        printf("can't obtain\n");
    }
    else
    {
        printf("%s\n",ch);
        free(ch);
        ch = NULL;
    }
    return 0;
}