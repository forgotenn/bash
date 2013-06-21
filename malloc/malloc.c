#include <stddef.h>
#include <sys/mman.h>
#include <string.h>

const int PAGESIZE = 4096;
void* malloc(size_t size)
{
    return mmap(NULL, (size / PAGESIZE + 1) * PAGESIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
}

void free(void* ptr)
{
}

void* calloc(size_t nmemb, size_t size)
{
    return memset(malloc(nmemb * size), 0, nmemb * size);
}

void* realloc(void* ptr, size_t size)
{
    void* mem = malloc(size);
    memmove(mem, ptr, size);
    return mem;
}
