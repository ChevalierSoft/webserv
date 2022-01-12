# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    cgi_just_print.sh                                  :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/01/12 04:12:05 by dait-atm          #+#    #+#              #
#    Updated: 2022/01/12 04:13:54 by dait-atm         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#!/bin/bash

echo "Content-type: text/html"
echo ""

echo '<html>'
echo '<head>'
echo '<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">'
echo '<title>static html sent from cgi_just_print.sh</title>'
echo '</head>'
echo '<body>'
echo 'static html sent from cgi_just_print.sh'
echo '</body>'
echo '</html>'

exit 0