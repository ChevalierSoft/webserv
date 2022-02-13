#!/bin/bash
while [ 1 ]
do
	curl 127.0.0.1:12345
	curl 127.0.0.1:12345/site
	# curl 127.0.0.1:12345/home
	curl 127.0.0.1:12345/site/sub1
	curl 127.0.0.1:12345/site/sub2
	curl 127.0.0.1:12345/site/sub3
	curl 127.0.0.1:12345/site/sub4
	curl 127.0.0.1:12345/php_info.php
	curl 127.0.0.1:9999
	curl --resolve default:12345:127.0.0.3 default:12345
	curl --resolve other:12345:127.0.0.3 other:12345
	curl 127.0.0.1:12345/error

	# curl -X POST --data "" http://localhost:12345/site/sub4/print_POST.php
done