#!/bin/bash

if [[ "$(uname -s)" == "Darwin" ]]; then
    brew install curl python3
else
    sudo apt-get update -qqy
    sudo apt-get install -qqy libcurl-dev python3-dev
fi
