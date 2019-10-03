//
// Created by Valery Piashchynski on 2/10/19.
//

#include "../include/main.h"
#include "../include/debugger.h"
#include <sys/ptrace.h>
#include <iostream>
#include <zconf.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Program name not specified";
        return -1;
    }

    auto prog = argv[1];
    auto pid = fork();

    if (pid == 0) {
        // we're in the child
        // exec debugee

        //PTRACE_TRACEME in linux systems
        ptrace(PTRACE_TRACEME, 0, nullptr, nullptr);
        execl(prog, prog, nullptr);

    } else if (pid >= 1) {
        //we're in the parent process
        // exec debugger
        debugger dbg{prog, pid};
    }
}


