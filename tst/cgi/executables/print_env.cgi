#!/bin/bash

echo "HTTP/1.1 200 OK"
echo "Content-type: text/html"
echo ""

echo '<html>'
echo '<head>'
echo '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">'
echo '<title>static html sent from cgi_just_print.sh</title>'
echo '<style> ul{font-family: sans-serif;} table, th, td { border: 1px solid pink; } th {color: pink} </style>'
echo '</head>'
echo '<body>'

echo '<table style="width:100%">'
echo '	<tr>'
echo '		<th>NAME</th>	<th>VALUE</th>'
echo '	</tr>'

while IFS='=' read -r -d '' n v; do
	printf "<tr><td>%s</td> <td>%s</td> </tr>" "$n" "$v"
done < <(env -0)
echo '</table>'

echo '</body>'
echo '</html>'

exit 0