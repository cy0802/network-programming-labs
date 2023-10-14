#!/bin/bash
curl -o demo.bin https://inp.zoolab.org/binflag/challenge?id=111550107;
g++ -o lab2.exe lab2.cpp;
flag=$(./lab2.exe);
echo $flag;
curl https://inp.zoolab.org/binflag/verify?v=$flag;