//
// Created by Valery Piashchynski on 3/10/19.
//

#include <sys/ptrace.h>
#include "../include/breakpoint.h"

breakpoint::breakpoint(pid_t pid, std::intptr_t addr) {

}

void breakpoint::enable() {
    long int data = ptrace(PTRACE_PEEKDATA, m_pid, m_addr, nullptr);
    m_saved_data = static_cast<uint8_t>(data & 0xff); // save bottom byte
    uint64_t int3 = 0xcc;
    uint64_t data_with_int3 = ((data & ~0xff) | int3); // set bottom byte to 0xcc
    ptrace(PTRACE_POKEDATA, m_pid, m_addr, data_with_int3);
    m_enabled = true;
}

void breakpoint::disable() {
    long int data = ptrace(PTRACE_PEEKDATA, m_pid, m_addr, nullptr);
    auto restored_data = ((data & ~0xff) | m_saved_data);
    ptrace(PTRACE_POKEDATA, m_pid, m_addr, restored_data);
    m_enabled = false;
}

auto breakpoint::is_enabled() const -> bool {
    return m_enabled;
}

auto breakpoint::get_address() const -> std::intptr_t {
    return m_addr;
}

