#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

const int BUFSIZE = 100;
char* buf;
char* good;
char* change;
int change_len;
int buf_size;
int good_size;

int good_char(char c)
{
    return (c >= 32 && c != 127);
}

int equal(char* s)
{
    int i;
    for (i = 0; i < change_len; i++)
        if (s[i] != change[i])
            return 0;
    return 1;
}

void modify(char* s, int len)
{
    int i;
    for (i = 0; i < len - change_len; i++)
    {
        if (equal(s + i))
        {
            s[i] = '\n';
            memmove(s + i + 1, s + change_len, len - i - change_len);
        }
    }
}

int main(int argc, char** argv)
{
    change = "a";
    change_len = 1;
    buf = malloc(BUFSIZE);
    good = malloc(BUFSIZE);
    buf_size = 0;
    good_size = 0;
    while(1)
    {
        int tmp = read(0, buf + buf_size, BUFSIZE - buf_size);
        if (tmp > 0)
            buf_size += tmp;

        if (buf_size > 0)
        {
            int i = 0;
            while (good_size < BUFSIZE && i < buf_size)
            {
                if (good_char(buf[i]))
                {
                    good[good_size] = buf[i];
                    good_size++;
                }
                i++; 
            }
            memmove(buf + i, buf, buf_size - i);
            buf_size -= i;
        }

        if (good_size >= change_len)
            modify(good, good_size);

        if (good_size > change_len)
        {
            tmp =  write(1, good, good_size - change_len);
            if (tmp > 0)
            {
                memmove(good + tmp, good, good_size - tmp);
                good_size -= tmp;
            }
        }
    }
    return 0;
}
