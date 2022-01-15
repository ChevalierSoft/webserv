
#include "Conf.hpp"
#include <fstream>
#include <cstdlib>
#include <utility>

Conf::Conf(){}

Conf::Conf(file_type conf_file){
    std::ifstream ifs("../test/conf/webser.conf");

    if (ifs.is_open())
        parse_file(ifs);
}

Conf::~Conf() {}

bool    Conf::parse_file(std::ifstream &ifs) {
    const char  sep = ':';
    std::string line = "init";
    std::string param;

    while (line.compare("") && !ifs.eof() && !ifs.fail())
    {
        std::getline(ifs, line);
        if (!(param = parse_param(line, sep)).compare("error"))
            return (false);
        else
        {
            if (!(set_param(ifs, line, param)))
                return (false);
        }
    }
    return (true);
}

std::string Conf::parse_param(std::string &line, const char sep) {
    
    size_t      found;
    if ((found = line.find(sep)) != std::string::npos)
        return (line.substr(0, found));
    return ("error");
}

std::string Conf::parse_value(std::string &line, std::string param) [
    return (std::string(line.substr(param.size() + 1)));
]

bool    Conf::set_param(std::ifstream &ifs, std::string &line, std::string param) {
    std::string value(parse_value(line, param));

    if (param.compare("server_name"))
        return (set_name(value));
    else if (param.compare("host"))
        return (set_host(value));
    else if (param.compare("port"))
        return (set_port(string_to_port(value)));
    else if (param.compare("error_pages"))
        return (parse_error_pages(ifs, line));
}

bool    Conf::parse_error_pages(std::ifstream &ifs, std::string &line) {
    std::map<code_type, file_type> error_pages;
    while (line.compare("") && line[0] == '\t' && !ifs.eof() && !ifs.fail())
    {
        std::getline(ifs, line);
        add_error(string_to_error(line.substr(1)));
    }
}

bool    Conf::add_error(error_type  error) {
    _error_pages.insert(error);
    return (true);
}

Conf::port_type   Conf::string_to_port(std::string value) {
    return (atoi(value.c_str()));
}


Conf::code_type     Conf::string_to_code(std::string value) {
    return (atoi(value.c_str()));
}

Conf::error_type    Conf::string_to_error(std::string value) {
    const char  sep = ':';
    std::string param(parse_param(value, sep));

    return (make_pair(string_to_code(param, parse_value(value, param))));
}

bool    Conf::set_name(name_type name) {
    _name = name;
    return (true);
}

bool    Conf::set_host(host_type host) {
    _host = host;
    return (true);
}

bool        Conf::set_port(port_type port) {
    _port = port;
    return (true);
}

