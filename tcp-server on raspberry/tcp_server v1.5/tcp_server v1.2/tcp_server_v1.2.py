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
    listdata=['0','1','2']#定义判定表
    if ser.isOpen==False:
        ser.open()
        print("serial success open")
    while True:
        #tcp receive

        data = tctimeClient.recv(buffsize).decode()

        if not data:
            break
        
         #  @1 #0 #0 #15.33! example
        #print(data)
        if data[0:1:1] =="@":
            #if data[::] == "0":
                signal = data[0:1]
                name = data[1:3]
                movement1 = data[4:5]
                movement2 = data[7:8]
                ten_digit = data[10:12]
                small_digit = data[13:15]
                if movement1 in listdata and movement2 in listdata:
                    amovement1=movement1.encode('utf-8')
                    amovement2=movement2.encode('utf-8')
                    aten_digit=ten_digit.encode('utf-8')
                    asmall_digit=small_digit.encode('utf-8')
                    #print(signal)
                    #print(name)  
                    print( movement1 + movement2)
                    ser.write(b"{")
                    ser.write(movement1)
                    ser.write(movement2)
                    
                    ser.write(ten_digit)
                    ser.write(small_digit)
                    ser.write(b"}")
            #if data[::] == "1":

        #send

        #tctimeClient.send(('[%s] %s' % (ctime(),data)).encode())
    tctimeClient.close()
    ser.close()
tctimeClient.close()
ser.close()
