//
// Created by Valery Piashchynski on 6/10/19.
//

#include <iostream>
#include "main.h"
#include <sys/ptrace.h>
#include <sys/user.h>
#include <zconf.h>

//000000000000007c <_GLOBAL__sub_I_main>:
//7c:	55                   	push   %rbp
//7d:	48 89 e5             	mov    %rsp,%rbp
//80:	be ff ff 00 00       	mov    $0xffff,%esi
//85:	bf 01 00 00 00       	mov    $0x1,%edi
//8a:	e8 a4 ff ff ff       	callq  33 <_Z41__static_initialization_and_destruction_0ii>
//8f:	5d                   	pop    %rbp
//90:	c3                   	retq

int main() {
    std::cerr << "Some message" << std::endl;
}