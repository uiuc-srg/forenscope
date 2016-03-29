#include <stdint.h>
#include <stdio.h>

__inline__ uint64_t rdtsc()
{
    uint32_t lo, hi;
   __asm__ __volatile__ (      // serialize
     "xorl %%eax,%%eax \n        cpuid"
     ::: "%rax", "%rbx", "%rcx", "%rdx");
   /* We cannot use "=A", since this would use %rax on x86_64 */
   __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));

   return (uint64_t)hi << 32 | lo;
}

int main(int argc, char *argv[])
{
    uint64_t tsc = rdtsc();

    printf("tsc: 0x%016LX\n", tsc);

    return 0;
}
