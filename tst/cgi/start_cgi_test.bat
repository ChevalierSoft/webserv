docker build . -f cgi.dockerfile -t tst_cgi

docker run --rm -it -p 4040:80 -v %cd%\executables:/usr/lib/cgi-bin/ -v %cd%/site/:/var/www/html/ --name c_tst_cgi tst_cgi
