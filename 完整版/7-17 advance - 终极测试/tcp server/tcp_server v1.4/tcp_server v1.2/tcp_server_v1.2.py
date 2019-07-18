#!/usr/bin/env python3
#-*- coding:utf-8 -*-

from socket import *
from time import ctime
from time import sleep
import smbus
import time
import serial

bus = smbus.SMBus(1)
address = 0x04


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
    listtwodata=['0','1','2','3']
    if ser.isOpen==False:
        ser.open()
        print("serial success open")
    while True:
        #tcp receive

        data = tctimeClient.recv(buffsize).decode()

        if not data:
            break
        #    ?%#+#1!
         #  @1 #0 #0 #15.33! example
        #print(data)
        if data[0:1:1] =="@":
            signal = data[0:1]
            name = data[1:3]
            movement1 = data[4:5]
            movement2 = data[7:8]
            ten_digit_p = data[10:12]
            small_digit_p = data[13:15]
            ten_digit_r = data[16:18]
            small_digit_r = data[19:21]
            if movement1 in listdata and movement2 in listdata:
                amovement1=movement1.encode('utf-8')
                amovement2=movement2.encode('utf-8')
                aten_digit_p=ten_digit_p.encode('utf-8')
                asmall_digit_p=small_digit_p.encode('utf-8')
                aten_digit_r=ten_digit_r.encode('utf-8')
                asmall_digit_r=small_digit_r.encode('utf-8')
                #print(signal)
                #print(name)  
                print( movement1 + movement2)
                ser.write(b"{")
                ser.write(amovement1)
                ser.write(amovement2)
                ser.write(aten_digit_p)
                ser.write(asmall_digit_p)
                ser.write(aten_digit_r)
                ser.write(asmall_digit_r)
                ser.write(b"}")
        elif data[0:1:1] == "?":
            two_movement=data[5:6] 
            if two_movement in listtwodata :
                atwo_movement=two_movement.encode('utf-8')
                ser.write(b"{")
                ser.write(b"?")
                ser.write(b"+")
                ser.write(atwo_movement)
                ser.write(b"}")
                if two_movement=="2":
                    bus.write_byte(address,0)#0 为机械臂归位 1为水平 2为伸直
                    bus.write_byte(address,90)
                    bus.write_byte(address,90)
                    bus.write_byte(address,50)
                if two_movement=="4":
                    bus.write_byte(address,3)#0 为机械臂归位 1为水平 2为伸直 3开灯
                    bus.write_byte(address,90)
                    bus.write_byte(address,90)
                    bus.write_byte(address,50)
        elif data[0:1:1]=="%" and data[11:12:1]=="!":
            # %90.00#90.00#90.00!
            servo_roll=data.split('#')[1]
            servo_pitch=data.split('#')[2]
            servo_minus=data.split('#')[3]
            aservo_roll=(int)(servo_roll.encode('utf-8'))*2
            aservo_pitch=(int)(servo_pitch.encode('utf-8'))*2
            aservo_minus=(int)(servo_minus.encode('utf-8'))*2
            bus.write_byte(address,1)
            bus.write_byte(address,aservo_roll)
            bus.write_byte(address,aservo_pitch)
            bus.write_byte(address,aservo_minus)
            #ser.write
            #adata=data.encode('utf-8')
            #ser.write(data)             #此处传一大串，按要求进行
        #send

        #tctimeClient.send(('[%s] %s' % (ctime(),data)).encode())
    tctimeClient.close()
    ser.close()
tctimeClient.close()
ser.close()
