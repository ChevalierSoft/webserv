#!/bin/bash

echo "Content-type: text/html"
echo ""

echo '<html>'
echo '<head>'
echo '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">'
echo '<title>static html sent from cgi_just_print.sh</title>'
echo '</head>'
echo '<body>'
echo '<h3>static html sent from cgi_just_print.sh</h3>'
echo '</body>'
echo '</html>'

exit 0