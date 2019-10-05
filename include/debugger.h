#ifndef DEBUGGER_DEBUGGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include "breakpoint.h"

#define DEBUGGER_DEBUGGER_H

class debugger {
public:
    debugger(std::string prog_name, pid_t pid);

    void handle_command(const std::string &line);

    std::vector<std::string> split(const std::string &s, char delimiter);

    bool is_prefix(const std::string &s, const std::string &of);

    void set_breakpoint_at_address(std::intptr_t addr);

    void run();

private:
    std::string m_prog_name;
    pid_t m_pid;

    void continue_execution();

    std::unordered_map<std::intptr_t, breakpoint> m_breakpoints;
};


#endif //DEBUGGER_DEBUGGER_H
