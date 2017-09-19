/****************************************************************************
 *   $Id:: extint.c 5670 2010-11-19 01:33:16Z usb00423                      $
 *   Project: NXP LPC17xx EINT example
 *
 *   Description:
 *     This file contains EINT code example which include EINT 
 *     initialization, EINT interrupt handler, and APIs for EINT.
 *
 ****************************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
****************************************************************************/
#include "LPC17xx.h"
#include "type.h"
#include "extint.h"
//#include "timer.h"
//#include "uart.h"
//#include <time.h>
volatile uint32_t eint0_counter;
extern uint32_t timer0_m0_counter, timer1_m0_counter;
extern uint32_t timer0_m1_counter, timer1_m1_counter;
int resultBuf[5];

/*****************************************************************************
** Function name:		EINT0_Handler
**
** Descriptions:		external INT handler
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void EINT0_IRQHandler (void)
{
LPC_SC->EXTINT = EINT0;		/* clear interrupt */

//eint0_counter++;
//if ( eint0_counter & 0x01 )	/* alternate the LED display */
// {
//LPC_GPIO2->FIOSET = 0x0000000F;	/* turn off P2.0~3 */
//LPC_GPIO2->FIOCLR = 0x000000F0;	/* turn on P2.4~7 */
//printf("%d\n",timer0_m0_counter++);
//disable_timer(0);
//}
// else
//{
//LPC_GPIO2->FIOSET = 0x000000F0;	/* turn on P2.0~3 */
//LPC_GPIO2->FIOCLR = 0x0000000F;	/* turn off P2.4~7 */
//printf("%d\n",timer0_m0_counter++);
//disable_timer(0);
//}


printf("0interrupt\n");
LPC_GPIOINT->IO2IntClr = 0xFFFFFFFF;
LPC_GPIOINT->IO0IntClr = 0xFFFFFFFF;

}

void EINT1_IRQHandler (void)
{
LPC_SC->EXTINT = EINT1;		/* clear interrupt */

//eint0_counter++;
//if ( eint0_counter & 0x01 )	/* alternate the LED display */
// {
//LPC_GPIO2->FIOSET = 0x0000000F;	/* turn off P2.0~3 */
//LPC_GPIO2->FIOCLR = 0x000000F0;	/* turn on P2.4~7 */
//printf("%d\n",timer0_m0_counter++);
//disable_timer(0);
//}
// else
//{
//LPC_GPIO2->FIOSET = 0x000000F0;	/* turn on P2.0~3 */
//LPC_GPIO2->FIOCLR = 0x0000000F;	/* turn off P2.4~7 */
//printf("%d\n",timer0_m0_counter++);
//disable_timer(0);
//}

printf("1interrupt\n");
LPC_GPIOINT->IO2IntClr = 0xFFFFFFFF;
LPC_GPIOINT->IO0IntClr = 0xFFFFFFFF;

}


void EINT2_IRQHandler (void)
{
LPC_SC->EXTINT = EINT2;		/* clear interrupt */

//eint0_counter++;
//if ( eint0_counter & 0x01 )	/* alternate the LED display */
// {
//LPC_GPIO2->FIOSET = 0x0000000F;	/* turn off P2.0~3 */
//LPC_GPIO2->FIOCLR = 0x000000F0;	/* turn on P2.4~7 */
//printf("%d\n",timer0_m0_counter++);
//disable_timer(0);
//}
// else
//{
//LPC_GPIO2->FIOSET = 0x000000F0;	/* turn on P2.0~3 */
//LPC_GPIO2->FIOCLR = 0x0000000F;	/* turn off P2.4~7 */
//printf("%d\n",timer0_m0_counter++);
//disable_timer(0);
//}

printf("2interrupt\n");
LPC_GPIOINT->IO2IntClr = 0xFFFFFFFF;
LPC_GPIOINT->IO0IntClr = 0xFFFFFFFF;

}


//void EINT3_IRQHandler (void)
//{
//LPC_SC->EXTINT = EINT3;		/* clear interrupt */
//
////eint0_counter++;
////if ( eint0_counter & 0x01 )	/* alternate the LED display */
//// {
////LPC_GPIO2->FIOSET = 0x0000000F;	/* turn off P2.0~3 */
////LPC_GPIO2->FIOCLR = 0x000000F0;	/* turn on P2.4~7 */
////printf("%d\n",timer0_m0_counter++);
////disable_timer(0);
////}
//// else
////{
////LPC_GPIO2->FIOSET = 0x000000F0;	/* turn on P2.0~3 */
////LPC_GPIO2->FIOCLR = 0x0000000F;	/* turn off P2.4~7 */
////printf("%d\n",timer0_m0_counter++);
////disable_timer(0);
////}
//printf("3interrupt\n");
//LPC_GPIOINT->IO2IntClr = 0xFFFFFFFF;
//LPC_GPIOINT->IO0IntClr = 0xFFFFFFFF;
//
//}


///void EINT0_IRQHandler (void)
//{
//	int i,j;
//	LPC_SC->EXTINT = EINT0;		/* clear interrupt */
//	eint0_counter++;
//	 if ( eint0_counter & 0x01 )	/* alternate the LED display */
//	{
//		 while (i<5)
//		 {
//			 LPC_GPIO2->FIOSET = 0x0000000F;	/* turn off P2.0~3 */
//			 delayMs(1,500);
//			 LPC_GPIO2->FIOCLR = 0x000000F0;	/* turn off P2.4~7 */
//			 delayMs(1,500);
//			 i++;
//		 }
	 //LPC_GPIOINT->IO2IntClr = 0x000000FF;
//	}

//	 	 else
//	 	 {
//	 		 while (j<5)
//	 		 {
//	 			 LPC_GPIO2->FIOSET = 0x000000F0;	/* turn on P2.0~3 */
//	 			 delayMs(1,500);
//	 			 LPC_GPIO2->FIOCLR = 0x0000000F;	/* turn off P2.4~7 */
//	 			 delayMs(1,500);

//	 			 j++;
//	 		 }
	 		// LPC_GPIOINT->IO2IntClr = 0x200;
//	 	 }
		/* clear interrupt */
//}


/*****************************************************************************
** Function name:		EINTInit
**
** Descriptions:		Initialize external interrupt pin and
**						install interrupt handler
**
** parameters:			None
** Returned value:		true or false, return false if the interrupt
**						handler can't be installed to the VIC table.
** 
*****************************************************************************/
uint32_t EINTInit( void )
{
  LPC_PINCON->PINSEL4 = 0x05500000;	/* set P2.10 as EINT0 and
									P2.0~7 GPIO output */

  //LPC_PINCON->PINMODE4 = 0x00000000;
  //LPC_GPIO2->FIODIR &= ~(1<<13);
  //LPC_GPIO2->FIODIR = 0x000000FF;	/* port 2, bit 0~7 only */
  //LPC_GPIO2->FIOCLR = 0x000000FF;	/* turn off LEDs */

  //LPC_GPIOINT->IO2IntEnF = 0x200;	/* Port2.10 is falling edge. */
  //LPC_SC->EXTMODE = EINT0_EDGE;		/* INT0 edge trigger */
  //LPC_SC->EXTPOLAR = 0;				/* INT0 is falling edge by default */

  //LPC_PINCON->PINSEL4 = 0x00400000;	/* set P2.10 as EINT0 and
  									//P2.0~7 GPIO output */
    //LPC_GPIO2->FIODIR = 0x000000FF;	/* port 2, bit 0~7 only */
    //LPC_GPIO2->FIOCLR = 0x000000FF;	/* turn off LEDs */

    //LPC_GPIOINT->IO2IntEnF = 0x200;	/* Port2.10 is falling edge. */
    LPC_GPIOINT->IO2IntEnF = ((0x01 <<10) | (0x01 <<11) | (0x01 <<12) );
    //LPC_GPIOINT->IO2IntEnF = (0x01 <<10) | (0x01 <<11);
    LPC_SC->EXTMODE = EINT0_EDGE | EINT1_EDGE | EINT2_EDGE ;		/* INT0 edge trigger */
    //LPC_SC->EXTMODE = EINT0_EDGE | EINT1_EDGE ;
    LPC_SC->EXTPOLAR = 0;				/* INT0 is falling edge by default */
    NVIC_EnableIRQ(EINT0_IRQn);
    NVIC_EnableIRQ(EINT1_IRQn);
    NVIC_EnableIRQ(EINT2_IRQn);
    //NVIC_EnableIRQ(EINT3_IRQn);

  return( TRUE );
}
//uint32_t EINTInit( void )
//{
//  LPC_PINCON->PINSEL4 = 0x00100000;	/* set P2.10 as EINT0 and
//									P2.0~7 GPIO output */
//  LPC_GPIO2->FIODIR = 0x000000FF;	/* port 2, bit 0~7 only */
//  LPC_GPIO2->FIOCLR = 0x000000FF;	/* turn off LEDs */
//
//  LPC_GPIOINT->IO2IntEnF = 0x200;	/* Port2.10 is falling edge. */
//  LPC_SC->EXTMODE = EINT0_EDGE;		/* INT0 edge trigger */
//  LPC_SC->EXTPOLAR = 0;				/* INT0 is falling edge by default */
//
//  LPC_PINCON->PINSEL4 = 0x00400000;	/* set P2.10 as EINT0 and
//  									P2.0~7 GPIO output */
//    //LPC_GPIO2->FIODIR = 0x000000FF;	/* port 2, bit 0~7 only */
//    //LPC_GPIO2->FIOCLR = 0x000000FF;	/* turn off LEDs */
//
//    //LPC_GPIOINT->IO2IntEnF = 0x200;	/* Port2.10 is falling edge. */
//    LPC_GPIOINT->IO2IntEnF = (0x01 <<11);
//    LPC_SC->EXTMODE = EINT1_EDGE;		/* INT0 edge trigger */
//    LPC_SC->EXTPOLAR = 0;				/* INT0 is falling edge by default */
//
//    NVIC_EnableIRQ(EINT1_IRQn);
//  NVIC_EnableIRQ(EINT0_IRQn);
//  return( TRUE );
//}

//uint32_t EINTInit( void )
//{
//  LPC_PINCON->PINSEL4 = 0x00400000;	/* set P2.10 as EINT0 and
//									P2.0~7 GPIO output */
//  //LPC_GPIO2->FIODIR = 0x000000FF;	/* port 2, bit 0~7 only */
//  //LPC_GPIO2->FIOCLR = 0x000000FF;	/* turn off LEDs */
//
//  //LPC_GPIOINT->IO2IntEnF = 0x200;	/* Port2.10 is falling edge. */
//  LPC_GPIOINT->IO2IntEnF = (0x01 <<11);
//  LPC_SC->EXTMODE = EINT1_EDGE;		/* INT0 edge trigger */
//  LPC_SC->EXTPOLAR = 0;				/* INT0 is falling edge by default */
//
//  NVIC_EnableIRQ(EINT1_IRQn);
//  return( TRUE );
//}

/******************************************************************************
**                            End Of File
******************************************************************************/

