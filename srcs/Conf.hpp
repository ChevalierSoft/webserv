
#include <string>

class Conf {
    public:
        typedef std::string     name_type;
        typedef std::string     host_type;
        typedef short           port_type;
        typedef std::string     file_type;
        typedef size_t          code_type;
        typedef class Error {
            friend Conf;
            code_type   _code;
            file_type   _file;
            public:
                Error() {}
                Error(code_type code, file_type file): _code(code), _file(file) {}
                ~Error() {}
        }                       error_type;
        typedef size_t          size_type;
        typedef std::string     method_type;
    private:
        name_type   _name;
        host_type   _host;
        port_type   _port;
        error_type  *_errors;
        size_type   client_body_size;
        method_type *methods;
    public:
        Conf(){}

};