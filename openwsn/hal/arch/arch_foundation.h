#ifndef _ARCH_FOUNDATION_H_
#define _ARCH_FOUNDATION_H_

#include "arch_configall.h"


/********************************/
/*      uC/OS-II specital code  */
/*      uC/OS-II���������      */
/********************************/

#define     USER_USING_MODE    0x10                    /*  User mode ,ARM 32BITS CODE �û�ģʽ,ARM����                  */
//
                                                     /*  Chosen one from 0x10,0x30,0x1f,0x3f.ֻ����0x10,0x30,0x1f,0x3f֮һ       */
//#include "Includes.h"


/********************************/
/*      ARM���������           */
/*      ARM specital code       */
/********************************/
//��һ������Ķ�
//This segment should not be modify

#include    "LPC2294.h"


/********************************/
/*     Ӧ�ó�������             */
/*Application Program Configurations*/
/********************************/
//���¸�����Ҫ�Ķ�
//This segment could be modified as needed.
#include    <stdio.h>
#include    <ctype.h>
#include    <stdlib.h>
#include    <setjmp.h>
#include    <rt_misc.h>


/********************************/
/*     �����ӵ�����             */
/*Configuration of the example */
/********************************/

/* System configuration .Fosc��Fcclk��Fcco��Fpclk must be defined
 * ϵͳ����, Fosc��Fcclk��Fcco��Fpclk���붨�� */

/*
#ifndef Fosc
//Crystal frequence,10MHz~25MHz��should be the same as actual status.
//Ӧ����ʵ��һ������Ƶ��,10MHz~25MHz��Ӧ����ʵ��һ��
#define Fosc            11059200
//System frequence,should be (1~32)multiples of Fosc,and should be equal or less  than 60MHz.
//ϵͳƵ�ʣ�����ΪFosc��������(1~32)����<=60MHZ
#define Fcclk           (Fosc * 4)
//CCO frequence,should be 2��4��8��16 multiples of Fcclk, ranged from 156MHz to 320MHz.
//CCOƵ�ʣ�����ΪFcclk��2��4��8��16������ΧΪ156MHz~320MHz
#define Fcco            (Fcclk * 4)
//VPB clock frequence , must be 1��2��4 multiples of (Fcclk / 4).
//VPBʱ��Ƶ�ʣ�ֻ��Ϊ(Fcclk / 4)��1��2��4��
#define Fpclk           (Fcclk / 4) * 1
#endif

//#include    "target.h"              //This line may not be deleted ��һ�䲻��ɾ��
*/


/* Currently, we support several hardware platforms. You can choose the target
 * platform by defining the platform macros in "configall.h". The hardware platform
 * most used is CONFIG_TARGET_OPENNODE_30 now.
 */

/* System wide configuration for lpc213x.
 * Fosc��Fcclk��Fcco��Fpclk must be defined or you may encounter data failure
 * when using UART, especially when the communication data rate is high.
 *
 * Fosc:	Crystal frequence,10MHz~25MHz��should be the same as actual status.
 *          Ӧ����ʵ��һ������Ƶ��,10MHz~25MHz��Ӧ����ʵ��һ��
 * Fcclk:   System frequence,should be (1~32)multiples of Fosc,and should be equal or less  than 60MHz.
 *          ϵͳƵ�ʣ�����ΪFosc��������(1~32)����<=60MHZ
 * Fcco:    CCO frequence,should be 2��4��8��16 multiples of Fcclk, ranged from 156MHz to 320MHz.
 *          CCOƵ�ʣ�����ΪFcclk��2��4��8��16������ΧΪ156MHz~320MHz
 * Fpclk:   VPB clock frequence , must be 1��2��4 multiples of (Fcclk / 4).
 *          VPBʱ��Ƶ�ʣ�ֻ��Ϊ(Fcclk / 4)��1��2��4��
 */
#if (defined(CONFIG_TARGET_OPENNODE_10) || defined(CONFIG_TARGET_OPENNODE_20) || defined(CONFIG_TARGET_WLSMODEM_11))
  #define Fosc            11059200
  #define Fcclk           (Fosc * 4)
  #define Fcco            (Fcclk * 4)
  #define Fpclk           (Fcclk / 4) * 1
#endif

#ifdef CONFIG_TARGET_OPENNODE_30
  #define Fosc            16000000
  #define Fcclk           (Fosc * 4)           // @todo warning; different macro definition why 200903
  #define Fcco            (Fcclk * 4)
  #define Fpclk           (Fcclk / 4) * 1
#endif



#endif

