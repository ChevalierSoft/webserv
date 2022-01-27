# ? This script starts a container with nginx and some tests CGI

docker build . -f cgi.dockerfile -t tst_cgi

docker run --rm -it -p 4040:80 -v $PWD/executables/:/usr/lib/cgi-bin/ -v $PWD/site/:/var/www/html/ --name c_tst_cgi tst_cgi
