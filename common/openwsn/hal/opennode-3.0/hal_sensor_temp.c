
#include "hal_foundation.h"
#include "hal_sensor_temp.h"


static uint8 isexist;
unsigned char i;

uint8 ROMS[8]=//{0x28,0x12,0x70,0x8f,0x00,0x00,0x00,0x05},        //1��
                      //{0x28,0x0d,0x70,0x8f,0x00,0x00,0x00,0x7a}};       //2��
                      //{0x28,0x50,0x64,0x8f,0x00,0x00,0x00,0x7d},       //3��
                      //{0x28,0xeb,0x8b,0x8f,0x00,0x00,0x00,0x37}};      //4��
                      //{0x28,0x0f,0xea,0x8e,0x00,0x00,0x00,0x91},     //5��
                      //{0x28,0x97,0x23,0x8f,0x00,0x00,0x00,0xdf}};    //6��
                      // {0x28,0xd7,0xfb,0x8f,0x00,0x00,0x00,0xca},        //7��
                       {0x28,0x41,0xf1,0x8f,0x00,0x00,0x00,0x74};       //8��

//�ӳ��ӳ���      10MHZ

void delay(int time)
{
  int i;
  for(i=0;i<time;i++);
}


uint8 ResetDS(void)
{
  IO0DIR |= DQ_PIN;
  IO0CLR = DQ_PIN;
  delay(48);                  //����DQ��480us
  IO0SET = DQ_PIN;                // �ͷ�DQ
  delay(8);                   //�ȴ�оƬӦ���ź�
  isexist = !!(IO0PIN & DQ_PIN);
  delay(40);                  //��ʱ���������ʱ��
  return (isexist);           //����Ӧ���źţ���оƬӦ��isexist��0;��оƬӦ��isexist=1
}


//��һ�ֽ��ӳ���

unsigned char read_byte(void)
{
  unsigned char i,t,value=0;
  for(i=0;i<8;i++) {
    IO0DIR |= DQ_PIN;
    IO0CLR  = DQ_PIN;    //������DQ���Ϳ�ʼ��ʱ��
    t++;
    IO0SET  = DQ_PIN;    //�ͷ�����
    IO0DIR &= ~DQ_PIN;
    t++;
    t++;
    if(IO0PIN & DQ_PIN)  value |= 0x01 << i;
    delay(6);
  }
    return value;
 }
 
 

//Write a byte

void write_byte(unsigned char val)
{
  IO0DIR |= DQ_PIN;

  for(i=8;i>0;i--)
  {
    IO0CLR  = DQ_PIN;
    IO0SET  = val & 0x01;  //ÿ��д1λ,ͨ��val���Ƶõ�
    delay(6);              //��ʱ60us
    IO0SET  = DQ_PIN;          //�ͷ�����
    val >>= 1;             //�����ͬʱ�����һ��2us��ʱ���϶
  }
}

void ReadTemps(unsigned char *val)      //�ô˺�������ȡ�¶�
{
   unsigned char i;
   ResetDS();
   write_byte(0x55);
   for(i=0;i<8;i++){
        write_byte(ROMS[i]);
      }
   write_byte(0xbe);         // Read Scratch Pad
   val[0]=read_byte();
   val[1]=read_byte();
}



