#!/bin/sh
tar zxvf ncftp-1.9.4.tgz
cd ncftp-1.9.4
rm Makefile
cvs import src/usr.bin/ncftp mgleason ncftp_1_9_4
