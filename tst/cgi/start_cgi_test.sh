# ? This script starts a container with nginx and some tests CGI

docker build . -f cgi.dockerfile -t tst_cgi

docker run --rm -it -p 4040:80 --name c_tst_cgi tst_cgi
