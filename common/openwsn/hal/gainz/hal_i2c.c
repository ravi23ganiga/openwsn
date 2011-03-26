/*
 * @modified by Lu Weihui (luweihui163@163.com) , He Yaxi(yaxihe@163.com)  (TongJi University) on 2010.07.10
 * - revision
 * @modified by Lu Weihui (luweihui163@163.com) , He Yaxi(yaxihe@163.com)  (TongJi University) on 2010.09.10
 * - tested ok.
 */
#include "../hal_configall.h"
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "../hal_foundation.h"
#include "../hal_i2c.h"
#include "../hal_cpu.h"
#include "../hal_assert.h"
#include "hpl_atmega128.h"
#include "hpl_atmega128_pin.h"

// I2C ״̬����
// MT ����ʽ���� MR ����ʽ����
#define START    0x08
#define RE_START   0x10
#define MT_SLA_ACK   0x18
#define MT_SLA_NOACK 0x20
#define MT_DATA_ACK   0x28
#define MT_DATA_NOACK 0x30
#define MR_SLA_ACK   0x40
#define MR_SLA_NOACK 0x48
#define MR_DATA_ACK   0x50
#define MR_DATA_NOACK 0x58  

#define _i2c_delayms(ms) cpu_delay(ms)


//����TWI����(��ģʽд�Ͷ�)
#define Start()  (TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN))   //����I2C
#define Stop()   (TWCR=(1<<TWINT)|(1<<TWSTO)|(1<<TWEN))   //ֹͣI2C
#define Wait()    {while(!(TWCR&(1<<TWINT)));}     //�ȴ��жϷ���
#define TestAck()   (TWSR&0xF8)          //�۲췵��״̬
#define SetAck()    (TWCR|=(1<<TWEA))        //����ACKӦ��
#define SetNoAck()  (TWCR&=~(1<<TWEA))        //����Not AckӦ��


//#define SET_READ_CMD(value) ((value) |= 0x01)
//#define SET_WRITE_CMD(value) ((value) &= 0xFE)


/**
 * open TiI2cAdapter for putchar/getchar
 */
TiI2cAdapter * i2c_open( TiI2cAdapter * i2c, uint8 id, uint8 rate )
{  
    i2c->id = id;
    // i2c->rate = rate;

    switch (i2c->id)
    {
    case 0:
        TWBR = rate;     // ���ñ����ʣ�����SCLƵ�ʹ�ʽ�趨������ģʽ�²�С��10//
        TWDR = 0xFF;     // Default content = SDA released//
        TWCR = 0x04;     // TWEN��λ��ʹ��TWI�ӿڣ�Disable Interupt and No Signal requests//
        break;
    case 1:
        // not supported now
        hal_assert( false );
        break;
    default:
        i2c = NULL;
        break;
    }
    return i2c;
}    

void  i2c_close( TiI2cAdapter * i2c )
{
    return;
}


/**
 * prepare to start i2c transmission
 */
int8 i2c_start( TiI2cAdapter * i2c )
{
    // TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    Start();       //I2C����
    Wait();
    if(TestAck()!=START)
    {
        return -1;              //-1��������ʧ��
    }
	return 0;
}

/**
 * stop i2c transmission
 */
void i2c_stop( TiI2cAdapter * i2c )
{
    // TWCR=(1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
  	Stop();                     //I2Cֹͣ
}

uint8 i2c_state( TiI2cAdapter * i2c )
{
    // Q: is it right?
    return TWCR;
}

int8 i2c_putchar_write_addr( TiI2cAdapter * i2c, uint8 addr )
{
    TWDR = addr;               //дI2C��������ַ��д��ʽ
    TWCR=(1<<TWINT)|(1<<TWEN);  //д��ֵʱ��TWINTλҪ��λ
    Wait();
    if(TestAck()!=MT_SLA_ACK)
    {
        return -2;              //-2�����ַдʧ��
    }
	return 0;
}


// wait for interrupt occuring
// Q: is the orginal comment right? --- zhangwei
void i2c_wait( TiI2cAdapter * i2c )
{
    while(!(TWCR&(1<<TWINT)))
    {
    }
}

//�۲췵��״̬
bool i2c_testack( TiI2cAdapter * i2c )
{
    return (TWSR&0xF8);
}

//����ACKӦ��
// formerly known as SetAck()
void i2c_replyack( TiI2cAdapter * i2c )
{
    TWCR|=(1<<TWEA);
}

//����Not AckӦ��
// formerly known as SetNoAck
void i2c_replynack( TiI2cAdapter * i2c )
{
    TWCR&=~(1<<TWEA);
}

/**
 * Put a char into the local i2c hardware for transmission. Before you call this 
 * function, you should call i2c_start() first.
 *
 * @return
 *  >= 0 success
 *  < 0 failed
 */
//����������Ҳ�����Ǵ������ڲ��Ĵ�����ַ��ͨ��
int8 _i2c_putchar( TiI2cAdapter * i2c, uint8 ch )
{
    TWDR = ch;                  //д���ݵ�TWDR
    TWCR=(1<<TWINT)|(1<<TWEN);
    Wait();
    if(TestAck()!=MT_DATA_ACK)
    {
        return -3;              //-3��������дʧ��
    }
	return 0;   
}

int8 i2c_restart( TiI2cAdapter * i2c )
{
    Start();                     
    Wait();
    if (TestAck()!=RE_START) 
    {
        return -4;              //-4������������ʧ��
    }
	return 0;   
}

int8 i2c_putchar_read_addr( TiI2cAdapter * i2c, uint8 addr )
{
    TWDR = addr;               //дI2C��������ַ�Ͷ���ʽ
    TWCR=(1<<TWINT)|(1<<TWEN);  //д��ֵʱ��TWINTλҪ��λ
    Wait();
    if(TestAck()!=MR_SLA_ACK)
    {
        return -5;              //-5�����ַ��ʧ��
    }
	return 0;
}

/**
 * get a char into the local i2c hardware for transmission. before you call this 
 * function, you should call i2c_start() first. 
 * 
 * @return
 *  >= 0 success
 *  < 0 failed
 */
int8 _i2c_getchar( TiI2cAdapter * i2c, uint8 * ch )
{
    TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);    //������I2C����ʽ
    Wait();
    if(TestAck()!=MR_DATA_ACK) 
    {
        return -7;              //-7����ACKӦ�����
    }
    * ch = TWDR;                //��ȡI2C��������
	return 0;
}

int8 i2c_getchar_last_byte( TiI2cAdapter * i2c, uint8 * ch )
{
    TWCR = (1<<TWINT) | (1<<TWEN);  //������I2C����ʽ
    Wait();
    if(TestAck()!=MR_DATA_NOACK) 
    {
        return -6;              //-6����NACKӦ�����
    }
    * ch = TWDR;                //��ȡI2C��������
	return 0;
}


/**
 * @param
 *  dev_id: device id
 */
int8 i2c_connect( TiI2cAdapter * i2c, uint8 dev_id )
{
    i2c_start( i2c );
    _i2c_putchar( i2c, dev_id );
    i2c->devid = dev_id;
    return 0;
}

void i2c_disconnect( TiI2cAdapter * i2c )
{
    i2c_stop( i2c );
}

/**
 * transmit a char to the target device
 */
int8 i2c_putchar( TiI2cAdapter * i2c, uint8 ch )
{
    // current operation will be transmitted to device i2c->device
    return _i2c_putchar( i2c, ch );
}


/**
 * get a char from the target device
 */
int8 i2c_getchar( TiI2cAdapter * i2c, uint8 * ch )
{
    return _i2c_getchar( i2c, ch );
}

/**
 * transmit a data block to to the target device
 */
int8 i2c_write( TiI2cAdapter * i2c, char * buf, uint8 len )
{
    return len;
}

int8 i2c_read( TiI2cAdapter * i2c, char * buf, uint8 size )
{
    return 0;
}












//i2cдһ���ֽ�
int8 i2c_putchar_old( TiI2cAdapter * i2c,unsigned char regaddr, unsigned char ch, unsigned char sla_w )
{
    int temp = 0;
    Start();       //I2C����
    Wait();
    if(TestAck()!=START)
    {
        temp = -1;              //-1��������ʧ��
        return temp;
    }
    
    TWDR = sla_w;               //дI2C��������ַ��д��ʽ
    TWCR=(1<<TWINT)|(1<<TWEN);  //д��ֵʱ��TWINTλҪ��λ
    Wait();
    if(TestAck()!=MT_SLA_ACK)
    {
        temp = -2;              //-2�����ַдʧ��
        return temp;
    }

    TWDR = regaddr;             //д���豸�ڲ��Ĵ�����ַ��TWDR
    TWCR=(1<<TWINT)|(1<<TWEN);
    Wait();
    if(TestAck()!=MT_DATA_ACK)
    {
        temp = -3;              //-3��������дʧ��
        return temp;
    }

    TWDR = ch;                  //д���ݵ�TWDR
    TWCR=(1<<TWINT)|(1<<TWEN);
    Wait();
    if(TestAck()!=MT_DATA_ACK)
    {
        temp = -3;              //-3��������дʧ��
        return temp;
    }   
    Stop();                     //I2Cֹͣ
    temp++;                     //�ɹ�дһ���ֽڣ�temp+1
    return temp;
    _i2c_delayms(100);            //��ʱ     
}



//i2c��һ���ֽ�
int8 i2c_getchar_old( TiI2cAdapter * i2c,unsigned char regaddr, char * pc, unsigned char sla_w, unsigned char sla_r )
{
    int temp = 0;
    Start();
    Wait();
    if (TestAck()!=START) 
    {
        temp = -1;               //-1��������ʧ��
        return temp; 
    }

    TWDR = sla_w;               //дI2C��������ַ��д��ʽ
    TWCR=(1<<TWINT)|(1<<TWEN);  //д��ֵʱ��TWINTλҪ��λ
    Wait();
    if(TestAck()!=MT_SLA_ACK)
    {
        temp = -2;              //-2�����ַдʧ��
        return temp;
    }

    TWDR = regaddr;             //д���豸�ڲ��Ĵ�����ַ��TWDR
    TWCR=(1<<TWINT)|(1<<TWEN);
    Wait();
    if(TestAck()!=MT_DATA_ACK)
    {
        temp = -3;              //-3��������дʧ��
        return temp;
    }

    Start();                     //I2C��������
    Wait();
    if (TestAck()!=RE_START) 
    {
        temp = -4;              //-4������������ʧ��
        return temp;
    }   

    TWDR = sla_r;               //дI2C��������ַ�Ͷ���ʽ
    TWCR=(1<<TWINT)|(1<<TWEN);  //д��ֵʱ��TWINTλҪ��λ
    Wait();
    if(TestAck()!=MR_SLA_ACK)
    {
        temp = -5;              //-5�����ַ��ʧ��
        return temp;
    }

    TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);        //������I2C����ʽ
    Wait();
    if(TestAck()!=MR_DATA_NOACK) 
    {
        temp = -6;              //-6����NACKӦ�����
        return temp;
    }

    * pc = TWDR;                //��ȡI2C��������
    temp++;                     //�ɹ���һ���ֽڣ�temp+1
    Stop();                     //I2Cֹͣ
    /*return temp;*/
	return TWDR;
}

/*
//i2cд����ֽ�
unsigned char i2c_write( TiI2cAdapter * i2c,unsigned char regaddr, char *pc, uint8 len, unsigned char sla_w )
{
    int temp = 0;
    Start();       //I2C����
    Wait();
    if(TestAck()!=START)
    {
        temp = -1;              //-1��������ʧ��
        return temp;
    }
    
    TWDR = sla_w;               //дI2C��������ַ��д��ʽ
    TWCR=(1<<TWINT)|(1<<TWEN);  //д��ֵʱ��TWINTλҪ��λ
    Wait();
    if(TestAck()!=MT_SLA_ACK)
    {
        temp = -2;              //-2�����ַдʧ��
        return temp;
    }

    TWDR = regaddr;             //д���豸�ڲ��Ĵ�����ַ��TWDR
    TWCR=(1<<TWINT)|(1<<TWEN);
    Wait();
    if(TestAck()!=MT_DATA_ACK)
    {
        temp = -3;              //-3��������дʧ��
        return temp;
    }


    //д����
    for (;len>1;len--,pc++)
    {
        TWDR = * pc;                  //���ڴ��е�����д��TWDR��
        TWCR=(1<<TWINT)|(1<<TWEN);
        Wait();
        if(TestAck()!=MT_DATA_ACK)
        {
            temp = -3;                //-3��������дʧ��
            return temp;
        }   
        temp++;                       //�ɹ�дһ�����ݣ�tempֵ+1
    }    

    Stop();                     //I2Cֹͣ
    _i2c_delayms(10000);            //��ʱ   
    return temp;                //temp����ֵ�ܹ۲쵽�ɹ�д�˶�������  
}

//i2c������ֽ�
unsigned char i2c_read(TiI2cAdapter * i2c,unsigned char regaddr, char * pc,uint8 size, unsigned char sla_w, unsigned char sla_r)
{
    int temp = 0;
    Start();                     //I2C����
    Wait();
    if (TestAck()!=START) 
    {
        temp = -1;               //-1��������ʧ��
        return temp; 
    }

    TWDR = sla_w;               //дI2C��������ַ��д��ʽ
    TWCR=(1<<TWINT)|(1<<TWEN);  //д��ֵʱ��TWINTλҪ��λ
    Wait();
    if(TestAck()!=MT_SLA_ACK)
    {
        temp = -2;              //-2�����ַдʧ��
        return temp;
    }

    TWDR = regaddr;             //д���豸�ڲ��Ĵ�����ַ��TWDR
    TWCR=(1<<TWINT)|(1<<TWEN);
    Wait();
    if(TestAck()!=MT_DATA_ACK)
    {
        temp = -3;              //-3��������дʧ��
        return temp;
    }

    Start();                     //I2C��������
    Wait();
    if (TestAck()!=RE_START) 
    {
        temp = -4;              //-4������������ʧ��
        return temp;
    }   

    TWDR = sla_r;               //дI2C��������ַ�Ͷ���ʽ
    TWCR=(1<<TWINT)|(1<<TWEN);  //д��ֵʱ��TWINTλҪ��λ
    Wait();
    if(TestAck()!=MR_SLA_ACK)
    {
        temp = -5;              //-5�����ַ��ʧ��
        return temp;
    }

    //�����ݣ�����ACK
    for (;size>1;size--,pc++)
    {
        TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);    //������I2C����ʽ
        Wait();
        if(TestAck()!=MR_DATA_ACK) 
        {
            temp = -7;              //-7����ACKӦ�����
            return temp;
        }
        * pc = TWDR;                //��ȡI2C�������� 
        temp++;                     //�ɹ���ȡһ�����ݣ�temp+1
    }
    
    //�����һ�����ݣ�����NACK
    TWCR = (1<<TWINT) | (1<<TWEN);  //������I2C����ʽ
    Wait();
    if(TestAck()!=MR_DATA_NOACK) 
    {
        temp = -6;              //-6����NACKӦ�����
        return temp;
    }

    * pc = TWDR;                //��ȡI2C��������
    temp++;
    Stop();                     //I2Cֹͣ
    return temp;                //temp����ֵ�ܹ۲쵽�ɹ���ȡ�˶�������
}
*/



























