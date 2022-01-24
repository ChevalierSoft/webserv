
# include "Conf.hpp"
# include "typing.hpp"

class Parser {
    private:

    public:
        Parser(file_type conf_file);
        ~Parser();

        conf_list   _confs;
        bool        _err;
        std::string _error_message;
        std::string line;
        int         line_number;

        bool        parse_file(std::ifstream &ifs);
        param_type  parse_param(std::string &line, const char sep);
        param_type  parse_value(std::string &line, std::string param);
        line_type   remove_comments(std::string &line, const char sep);
        bool        set_param(line_type &line, param_list params, size_t indent);

        bool        zero_indent(std::string param, std::string value);
        bool	    one_indent(param_list params, std::string value);
        bool        two_indent(param_list params, std::string value);
        bool        three_indent(param_list params, std::string value);

        // bool        is_whitespace (const char c);
        line_type   remove_whitespaces(line_type s);

        bool		set_error_message(std::string error_message);
        bool        check();

};