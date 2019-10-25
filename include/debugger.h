#ifndef DEBUGGER_DEBUGGER_H

#include <string>
#include "../external/libelfin/dwarf/dwarf++.hh"
#include "../external/libelfin/elf/elf++.hh"
#include <vector>
#include <unordered_map>
#include <bits/types/siginfo_t.h>
#include "breakpoint.h"

#define DEBUGGER_DEBUGGER_H

enum class symbol_type {
    notype, // (no type, absolute symbol)
    object, // data object
    func,   // function entry point
    section, // symbol is associated with a section
    file, // source file is associated with a object file
};


struct symbol {
    symbol_type type;
    std::string name;
    std::uintptr_t addr;
};

class debugger {
public:
    debugger(std::string prog_name, pid_t pid) : m_prog_name{std::move(prog_name)}, m_pid{pid} {
        auto fd = open(m_prog_name.c_str(), O_RDONLY);

        m_elf = elf::elf{elf::create_mmap_loader(fd)};
        m_dwarf = dwarf::dwarf{dwarf::elf::create_loader(m_elf)};
    };

    siginfo_t get_signal_info();

    void handle_command(const std::string &line);

    void handle_sigtrap(siginfo_t info);

    std::vector<std::string> split(const std::string &s, char delimiter);

    bool is_prefix(const std::string &s, const std::string &of);

    void set_breakpoint_at_address(std::intptr_t addr);

    void set_breakpoint_at_function(const std::string &name);

    void set_breakpoint_at_source_line(const std::string &file, unsigned line);

    std::vector<symbol> lookup_symbol(const std::string &name);

    void step_over_breakpoint();

    void step_over();

    void step_out();

    void step_in();

    void remove_breakpoint(std::intptr_t addr);

    void single_step_instruction();

    void single_step_instruction_with_breakpoint_check();

    void wait_for_signal();

    void dump_registers();

    dwarf::die get_function_from_pc(uint64_t pc);

    dwarf::line_table::iterator get_line_entry_from_pc(uint64_t pc);

    void print_source(const std::string &file_name, unsigned line, unsigned n_lines_context = 2);

    uint64_t read_memory(uint64_t address);

    void write_memory(uint64_t address, uint64_t value);

    uint64_t get_pc();

    void set_pc(uint64_t pc);

    void run();

private:
    std::string m_prog_name;
    pid_t m_pid;
    dwarf::dwarf m_dwarf;
    elf::elf m_elf;

    void continue_execution();

    std::unordered_map<std::intptr_t, breakpoint> m_breakpoints;
};


#endif //DEBUGGER_DEBUGGER_H
