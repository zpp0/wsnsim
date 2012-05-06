#!/bin/sh

cat $1 | iconv -c -f utf8 -t cp1251 > tmp
cat tmp > $1
rm tmp
