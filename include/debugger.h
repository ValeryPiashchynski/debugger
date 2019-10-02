//
// Created by Valery Piashchynski on 2/10/19.
//

#ifndef DEBUGGER_DEBUGGER_H
#define DEBUGGER_DEBUGGER_H

#include <string>
#include <vector>

class debugger {
public:
    debugger(std::string prog_name, pid_t pid);
    void handle_command(const std::string &line);
    std::vector<std::string> split(const std::string &s, char delimiter);
    bool is_prefix(const std::string &s, const std::string &of);

    void run();

private:
    std::string m_prog_name;
    pid_t m_pid;

    void continue_execution();
};


#endif //DEBUGGER_DEBUGGER_H
