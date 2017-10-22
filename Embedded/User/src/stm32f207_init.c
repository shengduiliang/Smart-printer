#include "stm32f2xx.h"
#include "stdio.h"
#include "stm32f2x7_eth.h"
#include "data_form.h"
#include "i2c_ee.h"


#include "app_cfg.h"
#include <string.h> 
#include "wifi_conf.h"


extern void Ethernet_MDIO_Config(void);




/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DP83848_PHY_ADDRESS       0x01 /* Relative to STM322xG-EVAL Board */

//#define MII_MODE          /* MII mode for STM3210C-EVAL Board (MB784) (check jumpers setting) */
#define RMII_MODE       /* RMII mode for STM3210C-EVAL Board (MB784) (check jumpers setting) */



static void Usart1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;//����GPIO_InitTypeDef���͵Ľṹ���ԱGPIO_InitStructure
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//ʹ����Ҫ�õ���GPIO�ܽ�ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	//ʹ��USART1 ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	///��λ����1
	USART_DeInit(USART1);
	
	USART_StructInit(&USART_InitStructure);//����Ĭ��USART����
	USART_ClockStructInit(&USART_ClockInitStruct);//����Ĭ��USART����        
	//���ô���1�Ĺܽ� PA8 USART1_EN PA9 USART1_TX PA10 USART1_RX    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    //����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);        //�ܽ�PA9����ΪUSART1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;        
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
	USART_ClockInit(USART1,&USART_ClockInitStruct);


	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1,&USART_InitStructure); 
	
	/***************�ж�����*******************************/
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	/* Enable the Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);      //�����ж�ʹ��
	USART_ClearITPendingBit(USART1, USART_IT_TC);       //����ж�TCλ
	USART_Cmd(USART1,ENABLE);                            //���ʹ�ܴ�1
	
	//���ڽ���DMA
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
}



static void Usart6_Init(void)
{

	
	
	GPIO_InitTypeDef GPIO_InitStructure;//����GPIO_InitTypeDef���͵Ľṹ���ԱGPIO_InitStructure
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//ʹ����Ҫ�õ���GPIO�ܽ�ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	//ʹ��USART6 ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
	///��λ����6
	USART_DeInit(USART6);
	
	USART_StructInit(&USART_InitStructure);//����Ĭ��USART����
	USART_ClockStructInit(&USART_ClockInitStruct);//����Ĭ��USART����        
	//���ô���6�Ĺܽ� PC6 USART1_TX PC7 USART1_RX    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    //����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6);        //�ܽ�PC6����ΪUSART6
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;  
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6);

	
	USART_ClockInit(USART6,&USART_ClockInitStruct);


	USART_InitStructure.USART_BaudRate = 115200;//57600
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART6,&USART_InitStructure); 
	
	/***************�ж�����*******************************/
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//NVIC_PriorityGroup_4
	
	/* Enable the Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);      //�����ж�ʹ��

	
	USART_ClearITPendingBit(USART6, USART_IT_TC);       //����ж�TCλ
	USART_Cmd(USART6,ENABLE);                            //���ʹ�ܴ�6

	
}

static void Uart4_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;//����GPIO_InitTypeDef���͵Ľṹ���ԱGPIO_InitStructure
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//ʹ����Ҫ�õ���GPIO�ܽ�ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	//ʹ��USART4 ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	///��λ����4
	USART_DeInit(UART4);
	
	USART_StructInit(&USART_InitStructure);//����Ĭ��USART����
	USART_ClockStructInit(&USART_ClockInitStruct);//����Ĭ��USART����        
	//���ô���4 PC10 USART4_TX PC11 USART4_RX    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    //����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4);        //�ܽ�PC6����ΪUSART6
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;        
	GPIO_Init(GPIOC, &GPIO_InitStructure);                                                                                                                 
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4);        //�ܽ�PC7����ΪUSART6
	
	USART_ClockInit(UART4,&USART_ClockInitStruct);


	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART4,&USART_InitStructure); 
	
	
	/***************�ж�����*******************************/
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	/* Enable the Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn ;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	/***************�ж�����*******************************/

	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);        ///////�����ж�ʹ��
	USART_ClearITPendingBit(UART4, USART_IT_TC);//����ж�TCλ
	USART_Cmd(UART4,ENABLE);//���ʹ�ܴ�4
}

static void Usart3_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;//����GPIO_InitTypeDef���͵Ľṹ���ԱGPIO_InitStructure
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//ʹ����Ҫ�õ���GPIO�ܽ�ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	//ʹ��USART3 ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	///��λ����3
	USART_DeInit(USART3);
	
	USART_StructInit(&USART_InitStructure);//����Ĭ��USART����
	USART_ClockStructInit(&USART_ClockInitStruct);//����Ĭ��USART����        
	//���ô���3 PD8 USART3_TX PD9 USART3_RX    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;    //����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_USART3);        //�ܽ�PD8����ΪUSART3
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;        
	GPIO_Init(GPIOD, &GPIO_InitStructure);                                                                                                                 
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_USART3);        //�ܽ�PD9����ΪUSART3
	
	USART_ClockInit(USART3,&USART_ClockInitStruct);


	USART_InitStructure.USART_BaudRate = 9600;//����������Ϊ9600��wifi��Ϊ57600
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3,&USART_InitStructure); 

	/***************�ж�����*******************************/
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	/* Enable the Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	/***************�ж�����*******************************/
	
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);        ///////�����ж�ʹ��
	USART_ClearITPendingBit(USART3, USART_IT_TC);//����ж�TCλ
	USART_Cmd(USART3,ENABLE);//���ʹ�ܴ�3
}


void NVIC_EHT_Configuration(void)
{
	
	NVIC_InitTypeDef   NVIC_InitStructure;
	
	/* Set the Vector Table base location at 0x08000000 */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
	
	/* 2 bit for pre-emption priority, 2 bits for subpriority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); 

	/* Enable the Ethernet global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 	
}



/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void ETH_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
    volatile int i;
  /* Enable GPIOs clocks */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB |
                         RCC_AHB1Periph_GPIOC, ENABLE);
    
  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
  SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII);
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  
	/* Configure PA1, PA2 and PA7 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_7;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_ETH);


  /* Configure PC1, PC4 and PC5 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_ETH);
  
	/* Configure PB11, PB12 and PB13 */                             
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_ETH);
}

/**
  * @brief  Configures the Ethernet Interface
  * @param  None
  * @retval None
  */
static void ETH_MACDMA_Config(void)
{
  ETH_InitTypeDef ETH_InitStructure;

  /* Enable ETHERNET clock  */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | RCC_AHB1Periph_ETH_MAC_Tx |
                         RCC_AHB1Periph_ETH_MAC_Rx, ENABLE);                                             

  /* Reset ETHERNET on AHB Bus */
  ETH_DeInit();

  /* Software reset */
  ETH_SoftwareReset();

  /* Wait for software reset */
  while (ETH_GetSoftwareResetStatus() == SET);
  /* ETHERNET Configuration --------------------------------------------------*/
  /* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameter */
  ETH_StructInit(&ETH_InitStructure);

  /* Fill ETH_InitStructure parametrs */
  /*------------------------   MAC   -----------------------------------*/
  ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
  //ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Disable; 
  //  ETH_InitStructure.ETH_Speed = ETH_Speed_10M;
  //  ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;   

  ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
  ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
  ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
  ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
  ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
  ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
  ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
  ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
#ifdef CHECKSUM_BY_HARDWARE
  ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
#endif

  /*------------------------   DMA   -----------------------------------*/  
  
  /* When we use the Checksum offload feature, we need to enable the Store and Forward mode: 
  the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum, 
  if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
  ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable; 
  ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;         
  ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;     
 
  ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;       
  ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;   
  ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;
  ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;      
  ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;                
  ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;          
  ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;
  ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;

  /* Configure Ethernet */
  ETH_Init(&ETH_InitStructure, DP83848_PHY_ADDRESS);

  /* Enable the Ethernet Rx Interrupt */
  ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R, ENABLE);
}


/********************************************************************************************************
*                                             Mem_To_USART1_DMA_Config
*
* Description: This function is start the DMA to USART1 transmission
*
* Arguments  : size - scale of the data you wanna transfered
*							 SendBuff - the head-address of the data you gonna send to USART[deviceNum]
* Returns    : none
********************************************************************************************************/
void Mem_To_USART1_DMA_Config(uint8_t *SendBuff, uint16_t size)
{
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); 
	
	//��λDMA1 C4
	DMA_DeInit(DMA2_Stream7);
	
	/*����DMAͨ��4*/
	DMA_InitStructure.DMA_Channel = DMA_Channel_4; 
	/*����DMAԴ���������ݼĴ�����ַ*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;	   
	
	DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t)SendBuff;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_BufferSize         = size;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_1QuarterFull ;
    DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
		
			/*����DMA1��4ͨ��*/		   
		DMA_Init(DMA2_Stream7, &DMA_InitStructure); 

	/***************�ж�����*DMA2-S7******************************/
		/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
			/* Enable the Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	/***************�ж�����*******************************/
	
	//����DMA2_Stream7������ɺ�����ж�
	DMA_ITConfig(DMA2_Stream7, DMA_IT_TC,ENABLE);

	/*ʹ��DMA*/
	DMA_Cmd (DMA2_Stream7, ENABLE);					
}

void Mem_To_USART2_DMA_Config(uint8_t *SendBuff, uint16_t size)
{
	
		DMA_InitTypeDef DMA_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
	//����DMAʱ��  
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);  
	
		DMA_DeInit(DMA1_Stream6);
    DMA_InitStructure.DMA_Channel            = DMA_Channel_4;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DR);  
    DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t)SendBuff;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_BufferSize         = size;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_1QuarterFull ;
    DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
 
		DMA_Init(DMA1_Stream6, &DMA_InitStructure); 

	/***************�ж�����*DMA1-S6******************************/
		/* Configure the NVIC Preemption Priority Bits */  
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
			/* Enable the Interrupt */
		NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream6_IRQn;	 
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		/***************�ж�����*******************************/
		//����DMA1_Stream6������ɺ�����ж�
		DMA_ITConfig(DMA1_Stream6, DMA_IT_TC , ENABLE);	 
		
    DMA_Cmd(DMA1_Stream6, DISABLE);  
}


void System_Setup()
{
	RCC_ClocksTypeDef RCC_Clocks;
	
	Usart1_Init();
	Uart4_Init();
	Usart3_Init();
	Usart6_Init();
	I2C_Configuration();
	I2C_EE_Init();
	wifi_init();//��ʼ��esp8266
	
	
	
	
	NVIC_EHT_Configuration();
	 
	/* Configure the GPIO ports for ethernet pins */
  ETH_GPIO_Config();

  /* Configure the Ethernet MAC/DMA */
  ETH_MACDMA_Config();
	
		/* ����systic��Ϊ20ms�ж� */
	RCC_GetClocksFreq(&RCC_Clocks);
	if (SysTick_Config(RCC_Clocks.SYSCLK_Frequency / 50))		//ʱ�ӽ����жϣ�20msһ��
	{ 
		/* error handle*/ 
		while (1);
	}
	
}

/********************************************************************************************************
*                                             DMA_To_USARTx_Send
*
* Description: This function is start the DMA to USART[deviceNum] transmission
*
* Arguments  : size - scale of the data you wanna transfered
*							 SendBuff - the head-address of the data you gonna send to USART[deviceNum]
*							 deviceNum - the Port that you gonna choose to send your data
* Returns    : none
********************************************************************************************************/
void DMA_To_USARTx_Send(uint8_t *SendBuff, uint16_t size, uint8_t deviceNum)
{
	switch(deviceNum)
	{
		case USART1_PORT:
					Mem_To_USART1_DMA_Config(SendBuff, size); break;
		case USART2_PORT:
					Mem_To_USART2_DMA_Config(SendBuff, size); break;
		
		default: break;
	}		
}

/*
    �������´���,֧��printf����,����Ҫѡ��use MicroLIB	  
*/
#ifndef MicroLIB
//#pragma import(__use_no_semihosting)             //û��ʵ��fgetcʱ��Ҫ�����ò���   
/* ��׼����Ҫ��֧�ֺ��� ʹ��printf()���Դ�ӡ����Ҫʵ�ָú��� */               
struct __FILE 
{ 
	int handle; 
    /* Whatever you require here. If the only file you are using is */    
    /* standard output using printf() for debugging, no file handling */    
    /* is required. */
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
/* �ض���fputc���� ���ʹ��MicroLIBֻ��Ҫ�ض���fputc�������� */  
int fputc(int ch, FILE *f)
{
    /* Place your implementation of fputc here */
    /* Loop until the end of transmission */

    /* e.g. write a character to the USART */
    USART_SendData(UART4, (uint8_t) ch);//UART4

    return ch;
}

/*
����ֱ��ʹ��putchar
����Ҫ�ٶ��� int putchar(int ch)����Ϊstdio.h�������¶���
 #define putchar(c) putc(c, stdout)
*/

int ferror(FILE *f) {  
    /* Your implementation of ferror */  
    return EOF;  
} 
#endif 

FILE __stdin;  

int fgetc(FILE *fp)
{
	int ch = 0;

	ch = (int)UART4->DR & 0xFF;
	
	putchar(ch); //����
	
	return ch;
}


