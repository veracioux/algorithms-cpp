#!/bin/bash

source common.sh

echo "--interactive, just exit"
acpp --interactive
echo "-i, same"
acpp -i
echo "-int, Unknown option"
acpp -int
echo "minimize, no error"
acpp minimize --dont-care ABCD

