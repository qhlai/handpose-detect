#!/usr/bin/env python3
#-*- coding:utf-8 -*-

from socket import *
from time import ctime
from time import sleep
import serial
#树莓派3 + HC-SR04超声波测距模块 https://www.jianshu.com/p/293415ae3e9c
import RPi.GPIO as GPIO
import time
#
#https://www.cnblogs.com/L-Test/p/8811784.html
#https://www.cnblogs.com/alibai/p/4027324.html
distance = []
for i in range(4):
    distance.append([])
    #for j in range(4):
        #distance[i].append(0) 
GPIO_TRIGGER = []
for i in range(4):
    GPIO_TRIGGER.append([])

GPIO_ECHO =[]
for i in range(4):
    GPIO_ECHO.append([])

#设置 GPIO 模式为 BCM
GPIO.setmode(GPIO.BCM)
#定义 GPIO 引脚 https://blog.csdn.net/oalevel/article/details/79211215
GPIO_TRIGGER[0] = 17
GPIO_ECHO[0] = 18
GPIO_TRIGGER[1] = 27
GPIO_ECHO[1] = 22

#设置 GPIO 的工作方式 (IN / OUT)
GPIO.setup(GPIO_TRIGGER[0], GPIO.OUT)
GPIO.setup(GPIO_ECHO[0], GPIO.IN)
GPIO.setup(GPIO_TRIGGER[1], GPIO.OUT)
GPIO.setup(GPIO_ECHO[1], GPIO.IN)

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
    dist = GetDistance()
    print("Measured Distance = {:.2f} cm".format(dist))
    time.sleep(1)
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
                if two_movement=="1":
                    bus.write_byte(address,1)
                    bus.write_byte(address,90)
                    bus.write_byte(address,90)
                    bus.write_byte(address,50)
        elif data[0:1:1]=="%":
            servo_roll=data[1:4]
            servo_pitch=data[8:11]
            servo_minus=data[15:18]
            aservo_roll=(int)(servo_roll.encode('utf-8'))
            aservo_pitch=(int)(servo_pitch.encode('utf-8'))
            aservo_minus=(int)(servo_minus.encode('utf-8'))
            bus.write_byte(address,2)
            bus.write_byte(address,aservo_roll)
            bus.write_byte(address,aservo_pitch)
            bus.write_byte(address,aservo_minus)
        #send

        #tctimeClient.send(('[%s] %s' % (ctime(),data)).encode())
    tctimeClient.close()
    ser.close()
tctimeClient.close()
ser.close()

def GetDistance(i):
    tempdistance = []
    for i in range(8):
        distance.append([])
    for i in range(8):
        # 发送高电平信号到 Trig 引脚
        GPIO.output(GPIO_TRIGGER[i], True)
 
        # 持续 10 us 
        time.sleep(0.00001)
        GPIO.output(GPIO_TRIGGER[i], False)
 
        start_time = time.time()
        stop_time = time.time()
 
        # 记录发送超声波的时刻1
        while GPIO.input(GPIO_ECHO[i]) == 0:
            start_time = time.time()
 
        # 记录接收到返回超声波的时刻2
        while GPIO.input(GPIO_ECHO[i]) == 1:
            stop_time = time.time()
 
        # 计算超声波的往返时间 = 时刻2 - 时刻1
        time_elapsed = stop_time - start_time
        # 声波的速度为 343m/s， 转化为 34300cm/s。
        distance = (time_elapsed * 34300) / 2
        if distance>400:
            distance = 400
        if distance<4:
            distance = 4
        tempdistance[i] =distance
    for i in range(8):
        sum = sum+tempdistance

    distance[i] = sum/8
        
    return distance[i]
def GetAllDiatance(m):
    for i in range(m):
        GetDistance(i)
def DisplayAllDistance(num):
     for i in range(num):
        GetDistance(i);
        print("distance["+i+"] = "+distance[i]+"cm ");
def Avoid_Obstacle():
    if(diatance[0] < 20):
        