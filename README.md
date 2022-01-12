# webserv
http 1.1 server that can handle a cgi

## class management

#### class Parsing / bnf :
- to parse the config file
- will use the nginx config as model
- this will give the base info for Server class


#### class Server
- create a listening socket on a specific port
- listen for a connection
- accept messages
- determine if it's a correct message.
- call it's own GET/POST/DELETE (parsing + preparing the response)
- maybe will try to use the wierd abnf they provide in rfc
- respond with the right header + file or error
- execute cgi

## gide lines

- no need for multi-threading for now. maybe it will be useful when multiple server configurations will have to be handled.

## doc

- [in6addr\_any](https://stackoverflow.com/questions/16508685/understanding-inaddr-any-for-socket-programming)
- [à l'aide](http://www.kegel.com/c10k.html#strategies)
- [what is a CGI](https://www.geeksforgeeks.org/common-gateway-interface-cgi/)
- [how to end a telnet session on windows](https://store.chipkin.com/articles/telnet-how-do-i-end-a-telnet-session-windows-linux-mac)
- [how to use epoll ~~and why~~ (because of the subject we can not use errno after a read/write)](https://www.suchprogramming.com/epoll-in-3-easy-steps/)
- [ibm's poll course](https://www.ibm.com/docs/en/i/7.1?topic=designs-using-poll-instead-select)
- [rfc3875 for CGI/1.1](https://datatracker.ietf.org/doc/html/rfc3875)
- [Web CGI with Bash scripts](http://www.yolinux.com/TUTORIALS/BashShellCgi.html)
