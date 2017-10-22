/**
  ******************************************************************************
  * @file    stm32f2xx_it.c
  * @author  MCD Application Team
  * @version V1.0.2
  * @date    06-June-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx_exti.h"
#include "stm32f2x7_eth.h"
#include "ucos_ii.h"
#include "stdio.h"

#include "app_cfg.h"
#include "wifi_conf.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/* Extern variables ---------------------------------------------------------*/
extern unsigned int system_tick_num;
extern void process_mac(void);


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */	
//__asm void wait()
//{
//	BX LR
//}
void HardFault_Handler(void)
{		
//		__asm
//		{
//			mov r0,r1
//		}
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {

  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
//void PendSV_Handler(void)
//{
//}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  /* Update the LocalTime by adding SYSTEMTICK_PERIOD_MS each SysTick interrupt */
	OS_CPU_SR  cpu_sr;
	
	system_tick_num++;
   OS_ENTER_CRITICAL();  //����ȫ���жϱ�־,�����ж�/* Tell uC/OS-II that we are starting an ISR*/
   OSIntNesting++;
   OS_EXIT_CRITICAL();	  //�ָ�ȫ���жϱ�־

   OSTimeTick();     /* Call uC/OS-II's OSTimeTick(),��os_core.c�ļ��ﶨ��,��Ҫ�ж���ʱ�������Ƿ��ʱ��*/ 
   OSIntExit();  //��os_core.c�ļ��ﶨ��,����и������ȼ������������,��ִ��һ�������л� 
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


extern void LwIP_Pkt_Handle(void);
/**
  * @brief  This function handles ETH Handler.
  * @param  None
  * @retval None
  */
void ETH_IRQHandler(void)
{
	OS_CPU_SR cpu_sr;
	OS_ENTER_CRITICAL();    // ���ж�                               
    OSIntNesting++;	   		//�ж�Ƕ�ײ�����֪ͨucos
    OS_EXIT_CRITICAL();	   	//���ж�
	/* Handles all the received frames */
    /* check if any packet received */
    while(ETH_CheckFrameReceived())
    { 
      /* process received ethernet packet */
      LwIP_Pkt_Handle();
    }
	/* Clear the Eth DMA Rx IT pending bits */
	ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
	ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);
	OSIntExit();//�ж��˳���֪ͨucos�����þ����ӣ�
}



/******************************************************************************
                           DMA2_Stream7_-TC(For USART1-TX)
******************************************************************************/
extern void USART1_DMA_TC_Hook(void);

void DMA2_Stream7_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_Stream7,DMA_IT_TCIF7) != RESET)
	{
		USART1_DMA_TC_Hook();
	}
	
	DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
}


/******************************************************************************
                           DMA1_Channel-7-TC (For USART2-TX)
******************************************************************************/
extern void USART2_DMA_TC_Hook(void);

void DMA1_Stream6_IRQHandler(void)  
{
	if(DMA_GetITStatus(DMA1_Stream6,DMA_IT_TCIF6) != RESET)
	{
		USART2_DMA_TC_Hook();
	}
	DMA_ClearFlag(DMA1_Stream6,DMA_FLAG_TCIF6);
}


/******************************************************************************
                           USART1_RXNE_IT
******************************************************************************/
extern void USART1_Hook(void);

#include"print_cells.h"

static INT8U sus_print_task[MAX_CELL_NUM] = {0xFF,0xFF};

void USART1_IRQHandler(void)
{
	uint8_t ch;
	//The sentences are commented below are used to receive a byte one by one (in RXNE situation)
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //process the received byte
	{
		ch = USART_ReceiveData(USART1);
		if(ch != Xoff && ch != Xon)
		{
			USART1_Hook();
		}
		else if(ch == Xoff)  //�����建������
		{
			sus_print_task[0] = OSPrioHighRdy;
			OSTaskSuspend(OSPrioHighRdy);
		}
		else if(ch == Xon)    //�����建������
		{
			if(sus_print_task[0] != (INT8U)0xFF)   //���֮ǰ�Ƿ��Ѿ������Ӧ�Ĵ�ӡ�߳�
			{
				OSTaskResume(sus_print_task[0]);     //�ָ���ӡ�߳�
				sus_print_task[0] = 0xFF;
			}		
		}
	}
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
}


/******************************************************************************
                           UART4_RXNE_IT
******************************************************************************/
extern void UART4_Hook(void);

void UART4_IRQHandler(void)
{
	uint8_t ch;
	//The sentences are commented below are used to receive a byte one by one (in RXNE situation)
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //process the received byte
	{
		ch = USART_ReceiveData(UART4);
		if(ch != Xoff && ch != Xon)
		{
			UART4_Hook();
		}
		else if(ch == Xoff)
		{
			sus_print_task[1] = OSPrioHighRdy;
			OSTaskSuspend(OSPrioHighRdy);
		}
		else if(ch == Xon)
		{
			if(sus_print_task[1] != (INT8U)0xFF)   //���֮ǰ�Ƿ��Ѿ������Ӧ�Ĵ�ӡ�߳�
			{
				OSTaskResume(sus_print_task[1]);     //�ָ���ӡ�߳�
				sus_print_task[1] = 0xFF;
			}		
		}
	}
	USART_ClearITPendingBit(UART4, USART_IT_RXNE);
}


/******************************************************************************
                           USART3_RXNE_IT
******************************************************************************/
#include "cc.h"



extern void USART3_Hook(u8_t ch);//��������


void USART3_IRQHandler(void)
{
	uint8_t ch;
	//The sentences are commented below are used to receive a byte one by one (in RXNE situation)
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //process the received byte,RXNE == 1, related Resigister is Stand by
	{
		ch = USART_ReceiveData(USART3);
		USART3_Hook(ch);//�����������׶�
	}
	USART_ClearITPendingBit(USART3, USART_IT_RXNE);
}

extern void UART6_Hook(u8_t ch);//wifi����


uint8_t data1[20] = {0};

void USART6_IRQHandler(void)
{
	uint8_t ch;
	static int i = 0;
	static int flag = 0;
	static long length = 0;
	//The sentences are commented below are used to receive a byte one by one (in RXNE situation)
	if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)  //process the received byte,RXNE == 1, related Resigister is Stand by
	{
		ch = USART_ReceiveData(USART6);//��ȡ�ֽ�
//		printf("%c",ch);
		if(ch == '+')
		{
				flag = 1;
		}
		else if(flag == 1 && ch == 'I')
		{
			flag = 2;
		}
		else if(flag == 2 && ch == 'P')
		{
			flag = 3;
		}
		else if(flag == 3 && ch == 'D')
		{
			flag = 4;
		}
		else if(flag == 4 && ch == ',')
		{
			flag = 5;
		}
		else if(flag == 5 && ch != ',')
		{
			flag = 6;
		}
		else if(flag == 6 && ch == ',')
		{
			flag = 7;
			length = 0;
		}
		else if(flag == 7)
		{
			if(ch == ':')//������ڣ�����˵�������������Ҫ�洢��
			{
				flag = 9;
			}
			else
			{
				length = length*10+(ch - '0');//�����ȡ�ֽ�������
			}
		}
		else if(flag == 9 && length > 0  )
		{


			UART6_Hook(ch);//������wifi�׶�
			length-- ;
			if(length <= 0)
			{
				flag = 0;
	
			}
		}		
	}


	USART_ClearITPendingBit(USART6, USART_IT_RXNE);
}

	
	 	 
	