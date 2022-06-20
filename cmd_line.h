//==========================================================================================================
// cmd_line.h - Defines a command-line parser
//==========================================================================================================
#pragma once
#include <string>
#include <vector>
#include <map>

enum clp_t {CLP_NONE, CLP_MANDATORY, CLP_OPTIONAL};

class CCmdLine
{
public:

    // Prior to calling parse(), declare which switches are valid
    void    declare_switch(std::string name, clp_t swtype);

    // Call this to parse the command line
    void    parse(int argc, char** argv);

    // Call one of these to determine whether a given switch was used
    bool    has_switch(std::string name);
    bool    has_switch(std::string name, std::string *param);
    bool    has_switch(std::string name, int         *param);
    bool    has_switch(std::string name, double      *param);

    // Returns the count of non-switch command-line arguments
    int     arg_count() {return m_args.size();}

    // Returns the specified non-switch argument
    std::string arg(int index);

    // Returns a vector of every non-switch argument
    std::vector<std::string> args() {return m_args;}

protected:

    // Maps a switch name to a command-line-parameter-type
    std::map<std::string, clp_t> m_valid_switches;

    // Maps a command line switch to it's parameter
    std::map<std::string, std::string> m_switches;

    // This contains all of the non-switch arguments
    std::vector<std::string> m_args;
};
