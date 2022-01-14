# webserv
http 1.1 server that can handle a cgi

## class management

#### Config
- parse the provided (or not) config file
- a Config object will be created by host:ip


#### Server
- create a listening socket on a specific port
- listen for a connection
- accept clients
- get there messages in the client's object
- determine if it's a correct message
- call it's own GET/POST/DELETE (parsing + preparing the response)
- The abnf provided in the rfc2616 is a good base
- respond with the right header + file or error
- execute cgi

#### Client
- store input message
- store resonse message

## gide lines

## doc

- [course on I/O Multiplexing](https://notes.shichao.io/unp/ch6/)
- [how to end a telnet session](https://store.chipkin.com/articles/telnet-how-do-i-end-a-telnet-session-windows-linux-mac)
- [how to use epoll ~~and why~~ (because of the subject, we can not use errno after a read/write)](https://www.suchprogramming.com/epoll-in-3-easy-steps/)
- [ibm's poll course](https://www.ibm.com/docs/en/i/7.1?topic=designs-using-poll-instead-select)
- [in6addr\_any](https://stackoverflow.com/questions/16508685/understanding-inaddr-any-for-socket-programming)
- [à l'aide](http://www.kegel.com/c10k.html#strategies)
- [what is a CGI](https://www.geeksforgeeks.org/common-gateway-interface-cgi/)
- [Web CGI with Bash scripts](http://www.yolinux.com/TUTORIALS/BashShellCgi.html)
- [CGI environement](https://www.commentcamarche.net/contents/142-cgi-les-variables-d-environnement)
- [rfc3875 for CGI/1.1](https://datatracker.ietf.org/doc/html/rfc3875)
