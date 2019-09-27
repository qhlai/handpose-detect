clc;
clear all;
%test.dat
%200hz 数据采样   6个数据，分别是原始gyro数据xyz，51hz滤波后的gyro数据
%%  原始数据
load test

dat = data2';
dat1 = data4';
Ts1 = 0.005;

[m,n]=size(dat);
t1 = (0:n-1)*Ts1;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
A=1;  
B=1;  
H=1; 
% Q=0.01;  % Q越大，说明对模型的信任越低，越小，说明信任A，得到的数据噪音就会小 
% R=0.5; % R越大，说明对观测数据的信任越低，数据噪音小，甚至失真
Q=0.001; 
R=5; 
X=zeros(n,1);    
Z=dat;  
P=zeros(1,n);   
x_now = zeros(n,1);
P_now = zeros(n,1);
delt = zeros(n,1);
x = zeros(n,1);

for i=2:n  
    delt(i) = Z(i) - Z(i-1);
end
[b a]=butter(2,0.5); %30hz  30 / (200 / 2)    %
u = filter(b,a,delt);

%kalman过程
for i=2:n  
    %预测
    x(i) = A * x_now(i-1) + B * u(i-1);
    P(i) = A * P_now(i-1) * A' + Q;
    %更新
    kg = P(i) / (P(i) + R);
    x_now(i) = x(i) + kg * (Z(i) - x(i));
    P_now(i) = (1 - kg) * P(i);
end


figure(1)
plot(t1,dat,t1,x_now,'r');
title('数据');
legend('原始数据','滤波之后数据');
grid on;

 figure(2)
 plot(t1,delt,t1,u);
 title('方差');
 legend('delt','U');
 grid on;

% figure(2)
% plot(t1,P_now);
% title('方差');
% legend('原始数据','滤波之后数据');
% grid on;


