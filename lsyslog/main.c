#include <stdlib.h>
#include <string.h>
//#include <syslog.h>

int main(int argc, char** argv)
{
    openlog(argv[1], atoi(argv[2]), 0);
    int i;
    for (i = 0; i < 100; i++)
        syslog(0, "%d", i);
    closelog();
    return 0;
}
