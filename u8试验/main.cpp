#include <iostream>

using namespace std;

int count1=0;
void init()
{
    static int isinit;
    if(!isinit)//Èç¹ûÃ»¸³³õÖµ£¬Ôò¾Í½øÈëÌõ¼þÃüÁî
    {
        cout<<"hahaha"<<endl;
        isinit=1;//¸³³õÖµÍê±Ï
        count1++;
    }
}
int main()
{
    uint8_t receive[2],i,j,init=65;
    uint8_t arr[5][5];
    int flag;
    for(i=0x00;i<0x05;i++)//赋初值A~Y
    {
        for(j=0x00;j<0x05;j++)
        {
            arr[i][j]=init++;
        }
    }

    //arr[3][4]=67;//C
    //arr[1][2]=68;//D
    //arr[0][0]='A';
    receive[0]=0x34;
    receive[1]=0x33;

    //cout<<receive[0]<<endl;
    i=receive[0]-0x30;
    j=receive[1]-0x30;
    //cout<<i<<endl<<j<<endl;
    cout<<arr[i][j]<<endl;
    flag=arr[i][j];
    cout<<flag-64<<endl;
    //cout<<arr[0][0];
    //return 0;




    int n=0;
    while(n<5)
    {
        init();
        n++;
    }
    cout<<count1<<endl;
    return 0;
}
