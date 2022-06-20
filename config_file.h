//==========================================================================================================
// config_file.h - Defines a parser for configuration/settings files
//==========================================================================================================
#pragma once
#include <string>
#include <vector>
#include <stdexcept>
#include <map>



//----------------------------------------------------------------------------------------------------------
// CConfigScript() - Provides a convenient interface for parsing script-specs in a config-file
//----------------------------------------------------------------------------------------------------------
class CConfigScript
{
public:

    // After reset "get_next_line()" fetches the first line of the script
    void        rewind() {m_line_index = 0;}

    // Call this to begin processing the next line of the script
    bool        get_next_line(int *p_token_count = nullptr, std::string *p_text = nullptr);

    std::string get_next_token(bool make_lower = false);
    int32_t     get_next_int();
    double      get_next_float();

    // Call this to erase the script
    void        make_empty();

    // Overloading the '=' operator so we can assign a string vector
    void        operator=(const std::vector<std::string> rhs) {m_script = rhs; rewind();}

protected:

    // This is index of the next line to be fetched via "get_next_line()"
    int         m_line_index;

    // This is the index of the next token to be fetched
    int         m_token_index;

    // These are the lines of the script
    std::vector<std::string> m_script, m_tokens;
};
//----------------------------------------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------------------
// CConfigFile - Provides a convenient interface for reading configuration files
//----------------------------------------------------------------------------------------------------------
class CConfigFile
{

public:

    // Call this to read the config file.  Returns 'true' on success, 'false' if file not found
    bool    read(std::string filename, bool msg_on_fail = true);

    // Call this to set the name of section to use for name scoping
    void    set_current_section(std::string section) {m_current_section = section;}

    // Call this to determine whether an exception is thrown when trying to fetch an unknown key
    void    throw_on_fail(bool flag = true) {m_throw_on_fail = flag;}

    // Call this to fetch a variable-type configuration spec.
    // Can throw exception runtime_error
    bool    get(std::string key, std::string fmt, void* p1=nullptr, void* p2=nullptr, void* p3=nullptr
                                                , void* p4=nullptr, void* p5=nullptr, void* p6=nullptr
                                                , void* p7=nullptr, void* p8=nullptr, void* p9=nullptr);

    // Call this to fetch integers.
    // Can throw exception runtime_error
    bool    get(std::string key, int32_t* p1=nullptr, int32_t* p2=nullptr, int32_t* p3=nullptr
                               , int32_t* p4=nullptr, int32_t* p5=nullptr, int32_t* p6=nullptr
                               , int32_t* p7=nullptr, int32_t* p8=nullptr, int32_t* p9=nullptr);

    // Call this to fetch doubles
    // Can throw exception runtime_error
    bool    get(std::string key, double* p1=nullptr, double* p2=nullptr, double* p3=nullptr
                               , double* p4=nullptr, double* p5=nullptr, double* p6=nullptr
                               , double* p7=nullptr, double* p8=nullptr, double* p9=nullptr);

    // Call this to fetch stringss
    // Can throw exception runtime_error
    bool    get(std::string key, std::string* p1=nullptr, std::string* p2=nullptr, std::string* p3=nullptr
                               , std::string* p4=nullptr, std::string* p5=nullptr, std::string* p6=nullptr
                               , std::string* p7=nullptr, std::string* p8=nullptr, std::string* p9=nullptr);

    // Call this to fetch bools
    // Can throw exception runtime_error
    bool    get(std::string key, bool* p1=nullptr, bool* p2=nullptr, bool* p3=nullptr
                               , bool* p4=nullptr, bool* p5=nullptr, bool* p6=nullptr
                               , bool* p7=nullptr, bool* p8=nullptr, bool* p9=nullptr);

    // Call these to fetch a vector of values
    // Can throw exception runtime_error
    bool    get(std::string, std::vector<int32_t    > *p_values);
    bool    get(std::string, std::vector<double     > *p_values);
    bool    get(std::string, std::vector<std::string> *p_values);
    bool    get(std::string, std::vector<bool       > *p_values);
    

    // Call this to fetch a script-spec from the config file    
    bool    get(std::string, CConfigScript* p_script);

    // Tells the caller whether or not the specified spec-name exists
    bool    exists(std::string key) {return lookup(key, nullptr);}

    // Dumps out the m_specs in a human-readable form.  This is strictly for testing
    void    dump_specs();

protected:

    // If this is true, fetching the value of an unknown spec will throw 
    bool    m_throw_on_fail = true;

    // A strvec_t is a vector of strings
    typedef std::vector< std::string > strvec_t;

    // Call this to fetch the values-vector associated with a key
    bool    lookup(std::string key, strvec_t *p_result);

    // The section name to look for specs in
    std::string m_current_section;

    // Our configuration specs are a vector of spec_t objects
    std::map<std::string, strvec_t> m_specs;
};
//----------------------------------------------------------------------------------------------------------




