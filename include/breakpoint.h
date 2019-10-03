//
// Created by Valery Piashchynski on 3/10/19.
//

#ifndef DEBUGGER_BREAKPOINT_H
#define DEBUGGER_BREAKPOINT_H


#include <fcntl.h>
#include <cstdint>

class breakpoint {
public:
    breakpoint(pid_t pid, std::intptr_t addr);

    void enable();

    void disable();

    auto is_enabled() const -> bool;
    auto get_address() const -> std::intptr_t;

private:
    pid_t m_pid;
    std::intptr_t m_addr;
    bool m_enabled;
    uint8_t m_saved_data; // data which used to be at the breakpoint address
};


#endif //DEBUGGER_BREAKPOINT_H
