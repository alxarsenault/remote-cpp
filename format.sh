#!/bin/sh

find ./include -iname *.hpp -o -iname *.cpp | xargs clang-format -i
find ./example -iname *.hpp -o -iname *.cpp | xargs clang-format -i
