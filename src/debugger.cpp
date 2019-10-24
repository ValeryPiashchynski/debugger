#include "../include/debugger.h"
#include <string>
#include <vector>
#include <iostream>
#include <sys/ptrace.h>
#include <wait.h>
#include <registers.h>
#include <iomanip>
#include <fstream>
#include "linenoise.h"


void debugger::run() {
    int wait_status;
    auto options = 0;

    waitpid(m_pid, &wait_status, options);

    char *line = nullptr;
    while ((line = linenoise("minidbg> ")) != nullptr) {
        handle_command(line);
        linenoiseHistoryAdd(line);
        linenoiseFree(line);
    }
}

void debugger::handle_command(const std::string &line) {
    std::vector<std::string> args = split(line, ' ');
    auto command = args[0];

    if (is_prefix(command, "cont")) {
        continue_execution();
    } else if (is_prefix(command, "break")) {
        std::string addr{args[1], 2}; //assume, that user enter 0xADDRESS
        set_breakpoint_at_address(std::stol(addr, 0, 16));

    } else if (is_prefix(command, "register")) {
        if (is_prefix(args[1], "dump")) {
            dump_registers();
        } else if (is_prefix(args[1], "read")) {
            std::cout << get_register_value(m_pid, get_register_from_name(args[2])) << std::endl;
        } else if (is_prefix(args[1], "write")) {
            std::string val{args[3], 2}; //assume 0xVAL
            set_register_value(m_pid, get_register_from_name(args[2]), std::stol(val, 0, 16));
        }
    } else if (is_prefix(command, "memory")) {
        std::string addr{args[2], 2}; //assume 0xADDRESS

        if (is_prefix(args[1], "read")) {
            std::cout << std::hex << read_memory(std::stol(addr, 0, 16)) << std::endl;
        }
        if (is_prefix(args[1], "write")) {
            std::string val{args[3], 2}; //assume 0xVAL
            write_memory(std::stol(addr, 0, 16), std::stol(val, 0, 16));
        }
    } else {
        std::cerr << "Unknown command\n";
    }
}

std::vector<std::string> debugger::split(const std::string &s, char delimiter) {
    std::vector<std::string> out{};
    std::stringstream ss{s};
    std::string item;

    while (std::getline(ss, item, delimiter)) {
        out.push_back(item);
    }

    return out;
}

bool debugger::is_prefix(const std::string &s, const std::string &of) {
    if (s.size() > of.size())
        return false;

    return std::equal(s.begin(), s.end(), of.begin());
}

void debugger::continue_execution() {
    step_over_breakpoint();
    ptrace(PTRACE_CONT, m_pid, nullptr, nullptr);
    wait_for_signal();
}

void debugger::set_breakpoint_at_address(std::intptr_t addr) {
    std::cout << "Set breakpoint at address 0x" << std::hex << addr << std::endl;
    breakpoint bp{m_pid, addr};
    bp.enable();
    m_breakpoints.insert(std::make_pair(addr, bp));
}

void debugger::dump_registers() {
    for (const auto &rd:g_registers_descriptors) {
        std::cout
                << rd.name
                << "0x"
                << std::setfill('0')
                << std::setw(16)
                << std::hex
                << get_register_value(m_pid, rd.r)
                << std::endl;
    }
}

uint64_t debugger::read_memory(uint64_t address) {
    return ptrace(PTRACE_PEEKDATA, m_pid, address, nullptr);
}

void debugger::write_memory(uint64_t address, uint64_t value) {
    ptrace(PTRACE_POKEDATA, m_pid, address, value);
}

uint64_t debugger::get_pc() {
    return get_register_value(m_pid, reg::rip);
}

void debugger::set_pc(uint64_t pc) {
    set_register_value(m_pid, reg::rip, pc);
}

void debugger::step_over_breakpoint() {
    if (m_breakpoints.count(get_pc())) {
        auto &bp = m_breakpoints[get_pc()];
        if (bp.is_enabled()) {
            bp.disable();
            ptrace(PTRACE_SINGLESTEP, m_pid, nullptr, nullptr);
            wait_for_signal();
            bp.enable();
        }
    }
}

// encapsulate waitpid syscall
void debugger::wait_for_signal() {
    int wait_status;
    auto options = 0;
    waitpid(m_pid, &wait_status, options);

    auto siginfo = get_signal_info();
    switch (siginfo.si_signo) {
        case SIGTRAP:
            handle_sigtrap(siginfo);
            break;
        case SIGSEGV:
            std::cout << "Segfault, noooo. Reason: " << siginfo.si_code << std::endl;
            break;
        default:
            std::cout << "Got signal " << strsignal(siginfo.si_signo) << std::endl;
    }

}

// debugging information entry (DIE)
dwarf::die debugger::get_function_from_pc(uint64_t pc) {
    for (auto &cu: m_dwarf.compilation_units()) {
        if (die_pc_range(cu.root()).contains(pc)) {
            for (const auto &die: cu.root()) {
                if (die.tag == dwarf::DW_TAG::subprogram) {
                    if (die_pc_range(die).contains(pc)) {
                        return die;
                    }
                }
            }
        }
    }
    throw std::out_of_range{"cannot find function"};
}

// simply find the correct compilation unit, then ask the line table to get us
// the relevant entry
dwarf::line_table::iterator debugger::get_line_entry_from_pc(uint64_t pc) {
    for (auto &cu: m_dwarf.compilation_units()) {
        if (die_pc_range(cu.root()).contains(pc)) {
            auto &lt = cu.get_line_table();
            auto it = lt.find_address(pc);
            if (it == lt.end()) {
                throw std::out_of_range{"cannot find line entry"};
            } else {
                return it;
            }
        }
    }
    throw std::out_of_range{"cannot find line entry"};
}

void debugger::print_source(const std::string &file_name, unsigned line, unsigned n_lines_context) {
    std::ifstream file{file_name};

    auto start_line = line <= n_lines_context ? 1 : line - n_lines_context;
    auto end_line = line + n_lines_context + (line < n_lines_context ? n_lines_context - line : 0);

    char c{};

    auto current_line = 1u;

    //skip line until start line
    while (current_line != start_line && file.get(c)) {
        if (c == '\n')
            ++current_line;
    }

    //output cursor if we're at the current line
    std::cout << (current_line == line ? "> " : "  ");

    //write lines up until end_line
    while (current_line <= end_line && file.get(c)) {
        std::cout << c;
        if (c == '\n') {
            ++current_line;
            //output cursor if we're at the current line
            std::cout << (current_line == line ? "> " : "  ");
        }
    }

    //write newline and make sure that the stream is flushed properly
    std::cout << std::endl;
}

// to be able to tell what signal was sent to the process, but also we want to know
// how in was produced;
siginfo_t debugger::get_signal_info() {
    siginfo_t info;
    ptrace(PTRACE_GETSIGINFO, m_pid, nullptr, &info);
    return info;
}

// man sigaction
void debugger::handle_sigtrap(siginfo_t info) {
    switch (info.si_code) {
        //one of these will be set if a breakpoint was hit
        case SI_KERNEL:
        case TRAP_BRKPT: {
            set_pc(get_pc() - 1); //put the pc back where is should be
            std::cout << "Hit breakpoint at address 0x" << std::hex << get_pc() << std::endl;
            auto line_entry = get_line_entry_from_pc(get_pc());
            print_source(line_entry->file->path, line_entry->line);
            return;;
        }
        case TRAP_TRACE:
            return;
        default:
            std::cout << "Unknown SIGTRAP code " << info.si_code << std::endl;
            return;
    }
}








































