#include <userlib.h>

int syscall_sleep(dword tick) {

    int result;

    asm volatile("movl $%c2, %%ebx \n"
                 "movl %1  , %%esi \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(result)
                 :"m"(tick), "g"(SYSTEM_CALL_PROCESS_SLEEP)
                 );

    return result;
}

int syscall_heavy() {

    int result;

    asm volatile("movl $%c1, %%ebx \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(result)
                 :"g"(SYSTEM_CALL_HEAVEY)
                 );

    return result;
}

int syscall_print(const char* msg) {

    int result;

    asm volatile("movl $%c1, %%ebx \n"
                 "movl %2  , %%esi \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(result)
                 :"g"(SYSTEM_CALL_PRINT), "m"(msg)
                 );

    return result;
}

int syscall_kill() {

    int result;

    asm volatile("movl $%c1, %%ebx \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(result)
                 :"g"(SYSTEM_CALL_KILL)
                 );

    /* don't come here */
    return result;
}
