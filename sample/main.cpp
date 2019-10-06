//
// Created by Valery Piashchynski on 6/10/19.
//

#include <iostream>
#include "main.h"
#include <sys/ptrace.h>
#include <sys/user.h>
#include <zconf.h>

//0000000000001169 <main>:
//1169:	55                   	push   %rbp
//116a:	48 89 e5             	mov    %rsp,%rbp
//116d:	48 8d 35 91 0e 00 00 	lea    0xe91(%rip),%rsi        # 2005 <_ZStL19piecewise_construct+0x1>
//1174:	48 8d 3d e5 2e 00 00 	lea    0x2ee5(%rip),%rdi        # 4060 <_ZSt4cerr@@GLIBCXX_3.4>
//117b:	e8 c0 fe ff ff       	callq  1040 <_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PKc@plt>
//1180:	48 89 c2             	mov    %rax,%rdx
//1183:	48 8b 05 46 2e 00 00 	mov    0x2e46(%rip),%rax        # 3fd0 <_ZSt4endlIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_@GLIBCXX_3.4>
//118a:	48 89 c6             	mov    %rax,%rsi
//118d:	48 89 d7             	mov    %rdx,%rdi
//1190:	e8 bb fe ff ff       	callq  1050 <_ZNSolsEPFRSoS_E@plt>
//1195:	b8 00 00 00 00       	mov    $0x0,%eax
//119a:	5d                   	pop    %rbp
//119b:	c3                   	retq

int main() {
    auto pid = getpid();
    user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, nullptr, &regs);
    std::cerr << "Some message" << std::endl;
}