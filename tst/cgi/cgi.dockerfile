# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    cgi.dockerfile                                     :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dait-atm <dait-atm@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/01/12 04:20:34 by dait-atm          #+#    #+#              #
#    Updated: 2022/01/12 07:44:03 by dait-atm         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

FROM debian:buster

# ? install apache2
RUN apt-get update && \
	apt-get install -y apache2 vim && \
	rm -rf /var/cache/apk/*

# ? copy test CGI scripts and an index
COPY ./cgi_just_print.sh /usr/lib/cgi-bin/
COPY ./index.html /var/www/html/

# ? enable CGI in apache2.conf
RUN echo '\n\
<Directory "/usr/lib/cgi-bin"> \n\
    AllowOverride None \n\
    Options +ExecCGI \n\
    AddHandler cgi-script .sh .cgi .py .pl \n\
    Require all granted \n\
</Directory>' >> /etc/apache2/apache2.conf

RUN ln -s /etc/apache2/mods-available/cgi.load /etc/apache2/mods-enabled/

CMD /etc/init.d/apache2 start && /bin/bash

EXPOSE 80

# ? CGI is accessible throgh : http://localhost:4040/cgi-bin/cgi_just_print.sh

# ? install php7 cgi https://stackoverflow.com/questions/60751761/install-php7-4-cgi-in-debian-10