
# include "Conf.hpp"

class Parser {
    public:
        typedef const char                  sep_type;
        typedef std::string                 line_type;
        typedef std::string                 param_type;
        typedef std::vector<std::string>    param_list;
        typedef size_t                      indent_type;
        typedef Conf                        conf_type;
        typedef std::vector<Conf>           conf_list;
        typedef std::string                 file_type;

        Parser(file_type conf_file);
        ~Parser();

        conf_list   _confs;
        bool        _err;
        std::string _error_message;
        std::string line;
        int         line_number;

        bool	    parse_file_loop(param_list &params);
        bool        parse_file(std::ifstream &ifs);
        param_type  parse_param(line_type &line, sep_type sep);
        param_type  parse_value(line_type &line, param_type param);
        line_type   remove_comments(line_type line, sep_type sep);
        bool        set_param(line_type &line, param_list params, size_t indent);

        bool        zero_indent(param_type param, std::string value);
        bool	    one_indent(param_list params, std::string value);
        bool        two_indent(param_list params, std::string value);
        bool        three_indent(param_list params, std::string value);

        // bool        is_whitespace (const char c);
        line_type   remove_whitespaces(line_type s);

        bool		set_error_message(std::string error_message);
        bool        check();

};