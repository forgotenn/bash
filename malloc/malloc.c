#include <stddef.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <stdint.h>

const int PAGESIZE = 4096;
void* malloc(size_t size)
{
    void* mem = mmap(NULL, (size + sizeof(size_t) / PAGESIZE + 1) * PAGESIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    printf("malloc: %ld\n", size);
    printf("%lx\n", (intptr_t)mem);
    size_t* mem_size_t = (size_t*) mem;
    mem_size_t[0] = size;
    mem_size_t += 1;
    mem = (void*) mem_size_t;
    return mem;
}

void free(void* ptr)
{
}

void* calloc(size_t nmemb, size_t size)
{
    return memset(malloc(nmemb * size), 0, nmemb * size);
}

size_t min(size_t a, size_t b)
{
    return (a < b) ? a : b;
}

void* realloc(void* ptr, size_t size)
{
    printf("realloc: %lx %ld\n", (intptr_t)ptr, size);
    void* mem = malloc(size);
    printf("malloc done");
    if (ptr != NULL)
    {
        size_t* mem_size_t = (size_t*) mem;
        size_t* ptr_size_t = (size_t*) ptr;
        printf("size_mem=%ld,size_ptr=%ld\n", *(mem_size_t - 1), *(ptr_size_t - 1));
        memmove(mem, ptr, min(*(mem_size_t - 1), *(ptr_size_t - 1)));
    }
    return mem;
}
