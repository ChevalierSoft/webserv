#!/bin/bash

echo "Content-type: text/html"
echo ""

for i in 1 2 3 4 5
do
	echo "$i";
	sleep 2;
done

exit 0