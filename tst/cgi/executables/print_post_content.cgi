#!/bin/bash

IFD= read -r input

echo "Content-type: text/html"
echo ""

echo $input

exit 0



# perl script. $FORM can be realy usefull in this case:

# #!/usr/bin/perl

# print "Content-type: text/html\r\n";
# print "\r\n";

# read (STDIN, $buffer, $ENV{'CONTENT_LENGTH'});
# @pairs = split(/&/, $buffer);
# foreach $pair (@pairs) 
# {
# 	($name, $value) = split(/=/, $pair);
# 	$value =~ tr/+/ /;
# 	$value =~ s/%([a-fA-F0-9] [a-fA-F0-9])/pack("C", hex($1))/eg;
# 	$value =~ s/~!/ ~!/g;
	# $FORM{$name} = $value;
# 	print "$name : $value\r\n";
# }
