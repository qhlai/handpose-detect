#!/usr/bin/env python3
#-*- coding:utf-8 -*-

from socket import *
from time import ctime

host = ''
port = 4302
buffsize = 2048      #缓冲区
ADDR = (host,port)

tctime = socket(AF_INET,SOCK_STREAM)
tctime.bind(ADDR)
tctime.listen(3)


instruction = [0]*16

while True:
    print('Wait for connection ...')
    tctimeClient,addr = tctime.accept()
    print("Connection from :",addr)

    while True:
        #receive

        data = tctimeClient.recv(buffsize).decode()
        print(data)
        if not data:
            break

        #send

        tctimeClient.send(('[%s] %s' % (ctime(),data)).encode())
    tctimeClient.close()
tctimeClient.close()
