#!/usr/bin/env python3
#-*- coding:utf-8 -*-

from socket import *
from time import ctime
from time import sleep
import serial


host = ''
port = 4302
buffsize = 2048      #缓冲区
ADDR = (host,port)

tctime = socket(AF_INET,SOCK_STREAM)
tctime.bind(ADDR)
tctime.listen(3)




while True:
    print('Wait for connection ...')
    tctimeClient,addr = tctime.accept()
    print("Connection from :",addr)

    ser = serial.Serial('/dev/ttyUSB0',9600,timeout=0.5)

    if ser.isOpen==False:
        ser.open()
        print("serial success open")
    while True:
        #tcp receive

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
            
        ser.write(b"0")    #'b':表示要读写二进制数据 控制信号
        

        #send

        #tctimeClient.send(('[%s] %s' % (ctime(),data)).encode())
    tctimeClient.close()
    ser.close()
tctimeClient.close()
ser.close()
