//==========================================================================================================
// cmd_line.cpp - Implements a command-line parser
//==========================================================================================================
#include "cmd_line.h"
using namespace std;

//==========================================================================================================
// declare_switch() - Gets called prior to parse() in order to declare with command-line switches are valid
//==========================================================================================================
void CCmdLine::declare_switch(string name, clp_t swtype)
{
    // Make sure that switch names begin with a dash
    if (name[0] != '-') name = '-' + name;
    
    // And add this switch to the list of valid command-line switches
    m_valid_switches[name] = swtype;
}
//==========================================================================================================


//==========================================================================================================
// parse() - Parses the command line
//==========================================================================================================
void CCmdLine::parse(int argc, char** argv)
{
    int i = 0;

    // Fetch the name of this executable
    const char* exe = argv[0];

    // Clear any existing command line data we have
    m_switches.clear();
    m_args.clear();

    // Loop through every token on the command line...
    while (argv[++i])
    {
        // Fetch this argument from the command line
        const string& token = argv[i];
        
        // If this token isn't a switch, just append it to the list of non-switch arguments
        if (token[0] != '-')
        {
            m_args.push_back(token);
            continue;
        }

        // Is this switch in our list of valid switches?
        auto it = m_valid_switches.find(token);

        // If it's not a valid command-line switch for this program, complain to the user
        if (it == m_valid_switches.end())
        {
            fprintf(stderr, "%s: %s is not a valid switch!\n", exe, token.c_str());
            exit(1);
        }

        // Is the parameter for this switch optional, mandatory, or none?
        clp_t swtype = it->second;

        // Presume for the moment that the user didn't supply a switch parameter
        string switch_param = "";

        // Find out if the user supplied a switch parameter
        bool has_parameter = (argv[i+1] && argv[i+1][0] != '-');

        // If this switch doesn't have a parameter and it was supposed to, complain to the user
        if (!has_parameter && swtype == CLP_MANDATORY)
        {
            fprintf(stderr, "%s: switch %s required a parameter!\n", exe, token.c_str());
            exit(1);    
        }

        // If there is a parameter, and this switch can accept one, fetch it
        if (has_parameter && (swtype == CLP_MANDATORY || swtype == CLP_OPTIONAL))
        {
            switch_param = argv[++i];
        }

        // Store the switch and it's parameter (if any)
        m_switches[token] = switch_param;
    }
}
//==========================================================================================================


//==========================================================================================================
// has_switch() - Returns 'true' if the specified switch exists on the command line
//==========================================================================================================
bool CCmdLine::has_switch(string name, string *param)
{
    // Make sure that switch names begin with a dash
    if (name[0] != '-') name = '-' + name;

    // Did the user use the specified switch on the command line?
    auto it = m_switches.find(name);

    // If the user didn't specify this switch on the command line, tell the caller
    if (it == m_switches.end()) return false;

    // Otherwise, the user *did* use this switch.  Hand the caller the switch parameter
    *param = it->second;

    // And tell the caller that the specified switch was used on the command line
    return true;
}
//==========================================================================================================


//==========================================================================================================
// has_switch() - Returns 'true' if the specified switch exists on the command line
//==========================================================================================================
bool CCmdLine::has_switch(string name)
{
    string dummy;
    return has_switch(name, &dummy);
}
//==========================================================================================================


//==========================================================================================================
// has_switch() - Returns 'true' if the specified switch exists on the command line
//==========================================================================================================
bool CCmdLine::has_switch(string name, int* param)
{
    string token;

    // If this switch was used, translate its parameter into an integer
    if (has_switch(name, &token))
    {   
        *param = strtol(token.c_str(), nullptr, 0);
        return true;        
    }

    // If we get here, the switch wasn't used on the command line
    return false;
}
//==========================================================================================================


//==========================================================================================================
// has_switch() - Returns 'true' if the specified switch exists on the command line
//==========================================================================================================
bool CCmdLine::has_switch(string name, double* param)
{
    string token;

    // If this switch was used, translate its parameter into an floating-point value
    if (has_switch(name, &token))
    {   
        *param = strtod(token.c_str(), nullptr);
        return true;        
    }

    // If we get here, the switch wasn't used on the command line
    return false;
}
//==========================================================================================================


//==========================================================================================================
// arg() - Fetches the non-switch argument with the specified index
//==========================================================================================================
string CCmdLine::arg(int index)
{
    if (index >=0 && index < m_args.size()) return m_args[index];
    return "";
}
//==========================================================================================================

