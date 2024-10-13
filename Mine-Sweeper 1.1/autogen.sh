#!/bin/bash

set -e 

autoreconf --install

./configure

echo "Configuration complete. You can now run 'make' to build the project"

