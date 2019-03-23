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

        if not data:
            break

         #  @1 #0 #0 #15.33! example

        if data[0:1:1] =="@":
            signal = data[0:1]
            name = data[1:3]
            movement1 = data[4:6]
            movement2 = data[7:9]
            angle = data[10:15]
            print(signal)
            print(name)  
            print( movement1 + movement2+angle)
            
        
        

        #send

        #tctimeClient.send(('[%s] %s' % (ctime(),data)).encode())
    tctimeClient.close()
tctimeClient.close()
