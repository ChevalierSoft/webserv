#!/bin/bash

IFD= read -r input

echo "Content-type: text/html"
echo ""

# * "<>&*?./" must be filtered for security purpose
# * '&' splits pairs
# * '=' splits the key from it's value
echo $input

exit 0
