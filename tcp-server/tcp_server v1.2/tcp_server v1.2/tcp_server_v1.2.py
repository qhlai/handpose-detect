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
        #print(data)
        if data[0:1:1] =="@":
            signal = data[0:1]
            name = data[1:3]
            movement1 = data[4:5]
            movement2 = data[7:8]
            angle = data[10:15]
            print(signal)
            print(name)  
            print( movement1 + movement2+angle)
            if movement1 == "1":
                if movement2=="0":  #高速直行无转弯
                    ser.write(b"0")    #'b':表示要读写二进制数据 控制信号
                elif movement2=="5": #高速右转
                    ser.write(b"1")
                elif movement2=="6":#高速左转
                    ser.write(b"2")
                elif movement2=="7":#高速弱右转
                    ser.write(b"3")
                elif movement2=="8":#高速弱左转
                    ser.write(b"4")
            elif movement1=="2":
                if movement2=="0":  #高速倒车无转弯
                    ser.write(b"5")    #'b':表示要读写二进制数据 控制信号
                elif movement2=="5": #倒车强右转
                    ser.write(b"6")
                elif movement2=="6":#倒车强左转
                    ser.write(b"7")
                elif movement2=="7":#倒车弱右转
                    ser.write(b"8")
                elif movement2=="8":#倒车弱左转
                    ser.write(b"9")

                
         



        
        

        #send

        #tctimeClient.send(('[%s] %s' % (ctime(),data)).encode())
    tctimeClient.close()
    ser.close()
tctimeClient.close()
ser.close()
