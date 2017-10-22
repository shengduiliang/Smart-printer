/********************   (C) COPYRIGHT 2013 www.armjishu.com   ********************
 * �ļ���  ��SZ_STM32F107VC_LIB.h
 * ����    ���ṩSTM32F107VC����IV�ſ�����UCOSII����ϵͳ
 * ʵ��ƽ̨��STM32���ۿ�����
 * ����    ��www.armjishu.com 
**********************************************************************************/

#include "stm32f2xx.h"
#include "cc.h"
#include "main.h"
#include "printerException.h"
#include "print_cells.h"
#include "app_cfg.h"
#include "local_conf.h"
#include "wifi_conf.h"
#include "RequestHeap.h"


#define GET_STATUS_01 putchar(0x10);putchar(0x04);putchar(0x01);
#define GET_STATUS_02 putchar(0x10);putchar(0x04);putchar(0x02);
#define GET_STATUS_03 putchar(0x10);putchar(0x04);putchar(0x03);
#define GET_STATUS_04 putchar(0x10);putchar(0x04);putchar(0x04);

/******************************************USART,DMA related Area******************/
void USART1_Hook(void);
void UART4_Hook(void);
//void DMA_To_USARTx_Send(uint8_t *SendBuff, uint16_t size, uint8_t deviceNum);
//void Mem_To_USART1_DMA_Config(uint8_t *SendBuff, uint16_t size);
//void Mem_To_USART2_DMA_Config(uint8_t *SendBuff, uint16_t size);
/******************************************USART,DMA related Area******************/

/* Private function prototypes -----------------------------------------------*/
static  OS_STK LWIP_TaskStartStk[LWIP_TASK_START_STK_SIZE];
static  OS_STK PRINT_TaskStk[PRINT_TASK_STK_SIZE];
static  OS_STK REQ_BATCH_TaskStk[REQ_BATCH_TASK_STK_SIZE];
static  OS_STK PRINT_QUEUE_TaskStk[PRINT_QUEUE_TASK_STK_SIZE];
static  OS_STK HEALTH_DETECT_TaskStk[HEALTH_DETECT_TASK_STK_SIZE];
static  OS_STK LOCAL_REC_TaskStk[LOCAL_REC_STK_SIZE];
static  OS_STK WIFI_REC_TaskStk[WIFI_REC_STK_SIZE];
static  OS_STK WIFI_REC_Req_TaskStk[WIFI_REC_REQ_STK_SIZE];
static  OS_STK MESG_QUE_TaskStk[MESG_QUE_STK_SIZE];

// ��ӡ���䵥Ԫ�̣߳�����ʵ�ʵĶ������乤��
static  OS_STK TRANSMITTER_TaskStk[MAX_CELL_NUM][TRANSMITTER_STK_SIZE];

/* Private Functions ---------------------------------------------------------*/
static  void Lwip_TaskStart(void *p_arg);
static  void Print_Task(void *p_arg); 
static  void Print_Queue_Task(void *p_arg);
static  void Health_Detect_Task(void * p_arg );
static  void Recv_Batch_Task(void* p_arg);
static  void Local_Rec_Task(void* p_arg);
static  void Wifi_Rec_Task(void* p_arg);
static  void Mesg_Queue_Task(void * p_arg);
static  void Transmit_Task(void * p_arg);
static  void Deal_Wifi_Req_Task(void* p_arg);

/* Public Functions ---------------------------------------------------------*/
extern void System_Setup(void);

/* Pulic variables ---------------------------------------------------------*/
unsigned int system_tick_num = 0;

/* siganl for task syn ---------------------------------------------------------*/
OS_EVENT *Recon_To_Server_Sem;		//�����Ҫ���������������
OS_EVENT *Print_Sem;                //����������ӡ������ź�
OS_EVENT *Print_Queue_Sem;          //��������������ӡ���е�����
OS_EVENT *Block_1K_Sem;             //��������1K���������
OS_EVENT *Block_2K_Sem;				//��������2K���������
OS_EVENT *Block_4K_Sem;             //��������4K���������
OS_EVENT *Block_10K_Sem;            //��������10K���������

OS_EVENT *Batch_Rec_Sem;			//���һ�����ζ�ȡ�Ķ�ֵ�ź���
OS_EVENT *Ack_Rec_Order_Sem;		//���ؽ��նԶ���״̬Ӧ����ź���
OS_EVENT *Ack_Rec_Printer_Sem;		//���ؽ��նԴ�ӡ��״̬Ӧ����ź���
OS_EVENT *Local_Send_Data_Mutex;	//���ط���״̬��
OS_EVENT *Wifi_Send_Data_Mutex;		//wifi����״̬��
OS_EVENT *Printer_Status_Rec_Sem;	//��ӡ��״̬���������ź�
OS_EVENT *Print_Done_Sem;			//������ӡ����ź���
OS_EVENT *Local_Rec_Data_Sem;		//���ػ��������յ�����
OS_EVENT *Wifi_Rec_Data_Sem;		//Wifi���������յ�����
OS_EVENT *Capacity_Change_Sem;		//wifi�����������ı��ź�
OS_EVENT *Rec_Wifi_Req_Sem;			//��ȡ��wifi���������ź�

OS_EVENT *Mesg_Queue;				//����״̬��Ϣ�����ź�

INT8U block_1K[BLOCK_1K_NUM][BLOCK_1K_SIZE];   //1K�ڴ���ӡ����
INT8U block_2K[BLOCK_2K_NUM][BLOCK_2K_SIZE];   //2K�ڴ���ӡ����
INT8U block_4K[BLOCK_4K_NUM][BLOCK_4K_SIZE];   //4K�ڴ���ӡ����
INT8U block_10K[BLOCK_10K_NUM][BLOCK_10K_SIZE];//10K�ڴ���ӡ����

Heap heap;//��������ѿ��ƽṹ

OS_MEM *queue_1K;
OS_MEM *queue_2K;
OS_MEM *queue_4K;
OS_MEM *queue_10K;

unsigned char step = 0;
unsigned char num_order = 0;
unsigned char status_order = 0;

#define assert(expr, str)			 	\
	do {								\
		if(!(expr)) {					\
			printf((str));				\
			while(1);					\
		}								\
	}while(0)
	
void InitSemAndMutex(void)
{
	INT8U	 err;
	/*�����ź���*/
	assert((Local_Send_Data_Mutex = OSMutexCreate(LOCAL_SEND_DATA_MUTEX_PRIO, &err)), "CREATE Local_Send_Data_Sem FAILED\n");
	assert((Wifi_Send_Data_Mutex = OSMutexCreate(WIFI_SEND_DATA_MUTEX_PRIO, &err)), "CREATE Wifi_Send_Data_Sem FAILED\n");
	
	assert((Recon_To_Server_Sem = OSSemCreate(0)), "CREATE Recon_To_Server_Sem FAILED\n");
	assert((Print_Sem = OSSemCreate(0)), "CREATE Print_Sem FAILED\n");
	assert((Print_Queue_Sem	= OSSemCreate(0)), "CREATE Print_Queue_Sem FAILED\n");
	assert((Batch_Rec_Sem = OSSemCreate(0)), "CREATE Batch_Rec_Sem FAILED\n");
	assert((Ack_Rec_Order_Sem = OSSemCreate(0)), "CREATE Ack_Rec_Order_Sem FAILED\n");
	assert((Ack_Rec_Printer_Sem = OSSemCreate(0)), "CREATE Ack_Rec_Printer_Sem FAILED\n");
	assert((Printer_Status_Rec_Sem = OSSemCreate(0)), "CREATE Printer_Status_Rec_Sem FAILED\n");
	assert((Print_Done_Sem  = OSSemCreate(0)), "CREATE Print_Done_Sem FAILED\n");
	assert((Local_Rec_Data_Sem = OSSemCreate(0)), "CREATE Local_Rec_Data_Sem FAILED\n");
	assert((Wifi_Rec_Data_Sem = OSSemCreate(0)), "CREATE Wifi_Rec_Data_Sem FAILED\n");
	assert((Capacity_Change_Sem = OSSemCreate(0)), "CREATE Capacity_Change_Sem FAILED\n");
	assert((Rec_Wifi_Req_Sem = OSSemCreate(0)), "CREATE Rec_Wifi_Req_Sem FAILED\n");
	
	assert((Mesg_Queue = OSQCreate(MesgQueArray, MESG_QUEUE_SIZE)), "CREATE Mesg_Queue FAILED\n");
	
	assert((Block_1K_Sem = OSSemCreate(BLOCK_1K_NUM)), "CREATE 1K SEM FAILED\n");   
	assert((Block_2K_Sem = OSSemCreate(BLOCK_2K_NUM)), "CREATE 2K SEM FAILED\n"); 
	assert((Block_4K_Sem = OSSemCreate(BLOCK_4K_NUM)), "CREATE 4K SEM FAILED\n"); 	
	assert((Block_10K_Sem = OSSemCreate(BLOCK_10K_NUM)), "CREATE 10K SEM FAILED\n");
}
	
int main(void)
{
	u8_t os_err;
	u8_t i;

	System_Setup();    	//ϵͳ��ʼ��
	OSInit();			/* UCOSII ����ϵͳ��ʼ�� */
	InitSemAndMutex();	//��ʼ���ź���
		
	

	/*�����ܣ�LWIP��ʼ��*/
	os_err = OSTaskCreate((void (*) (void *)) Lwip_TaskStart,               		 //ָ����������ָ��
                          (void *) 0,												 //����ʼִ��ʱ�����ݸ�����Ĳ�����ָ��
               		      (OS_STK *) &LWIP_TaskStartStk[LWIP_TASK_START_STK_SIZE - 1],//���������Ķ�ջ��ջ��ָ��   �Ӷ����µݼ�
                          (INT8U) LWIP_TASK_START_PRIO);								 //�������������ȼ�		

	/*�����ܣ�����������ӡ*/
	os_err = OSTaskCreate((void (*) (void *))Print_Task,               		    //ָ����������ָ��
                          (void *) 0,											//����ʼִ��ʱ�����ݸ�����Ĳ�����ָ��
               		      (OS_STK *) &PRINT_TaskStk[PRINT_TASK_STK_SIZE - 1],	//���������Ķ�ջ��ջ��ָ��   �Ӷ����µݼ�
                          (INT8U) PRINT_TASK_PRIO);								//�������������ȼ�															
					
	/*�����ܣ��������ν���*/
	os_err = OSTaskCreate((void (*) (void *))Recv_Batch_Task,               		//ָ����������ָ��
                          (void *) 0,												//����ʼִ��ʱ�����ݸ�����Ĳ�����ָ��
               		      (OS_STK *) &REQ_BATCH_TaskStk[REQ_BATCH_TASK_STK_SIZE - 1],//���������Ķ�ջ��ջ��ָ��   �Ӷ����µݼ�
                          (INT8U) REQ_BATCH_TASK_PRIO);								//�������������ȼ�	
	
	/*�����ܣ�������ӡ����*/
	os_err = OSTaskCreate((void (*) (void *))Print_Queue_Task,               		    //ָ����������ָ��
                          (void *) 0,													//����ʼִ��ʱ�����ݸ�����Ĳ�����ָ��
               		      (OS_STK *) &PRINT_QUEUE_TaskStk[PRINT_QUEUE_TASK_STK_SIZE - 1],//���������Ķ�ջ��ջ��ָ��   �Ӷ����µݼ�
                          (INT8U) PRINT_QUEUE_TASK_PRIO);								//�������������ȼ�	
						  
	/*�����ܣ��������ؽ�������*/
	os_err = OSTaskCreate((void (*) (void *))Local_Rec_Task,               		    //ָ����������ָ��
                          (void *) 0,												//����ʼִ��ʱ�����ݸ�����Ĳ�����ָ��
               		      (OS_STK *) &LOCAL_REC_TaskStk[LOCAL_REC_STK_SIZE - 1],		//���������Ķ�ջ��ջ��ָ��   �Ӷ����µݼ�
                          (INT8U) LOCAL_REC_TASK_PRIO);								//�������������ȼ�	
						  
	/*�����ܣ��������ؽ�������*/
	os_err = OSTaskCreate((void (*) (void *))Wifi_Rec_Task,               		    //ָ����������ָ��
                          (void *) 0,												//����ʼִ��ʱ�����ݸ�����Ĳ�����ָ��
               		      (OS_STK *) &WIFI_REC_TaskStk[WIFI_REC_STK_SIZE - 1],		//���������Ķ�ջ��ջ��ָ��   �Ӷ����µݼ�
                          (INT8U) WIFI_REC_TASK_PRIO);								//�������������ȼ�	
													
	/*�����ܣ�������������߳�*/				
	os_err = OSTaskCreate((void (*) (void *))Health_Detect_Task,					//ָ����������ָ��
						  (void *) 0,												//����ʼִ��ʱ�����ݸ�����Ĳ�����ָ��	
						  (OS_STK *) &HEALTH_DETECT_TaskStk[HEALTH_DETECT_TASK_STK_SIZE - 1],//���������Ķ�ջ��ջ��ָ��   �Ӷ����µݼ�
						  (INT8U) HEALTH_DETECT_TASK_PRIO);//�������������ȼ�	
	
	/*�����ܣ�������Ϣ��������*/
	os_err = OSTaskCreate((void (*) (void *))Mesg_Queue_Task,               		//ָ����������ָ��
                          (void *) 0,												//����ʼִ��ʱ�����ݸ�����Ĳ�����ָ��
               		      (OS_STK *) &MESG_QUE_TaskStk[MESG_QUE_STK_SIZE - 1],		//���������Ķ�ջ��ջ��ָ��   �Ӷ����µݼ�
                          (INT8U) MESG_QUE_TASK_PRIO);								//�������������ȼ�	
	
	/*�����ܣ�������Ϣ��������*/
	os_err = OSTaskCreate((void (*) (void *))Deal_Wifi_Req_Task,               		//ָ����������ָ��
                          (void *) 0,												//����ʼִ��ʱ�����ݸ�����Ĳ�����ָ��
               		      (OS_STK *) &WIFI_REC_Req_TaskStk[WIFI_REC_REQ_STK_SIZE - 1],		//���������Ķ�ջ��ջ��ָ��   �Ӷ����µݼ�
                          (INT8U) WIFI_REC_REQ_TASK_PRIO);								//�������������ȼ�	
	
	// ���ݴ�ӡ��Ԫ��Ŀ������Ӧ��Ŀ�Ĵ����߳�
	for(i = 0; i < MAX_CELL_NUM; i++) 
	{
		/*�����ܣ�������Ϣ��������*/
		os_err = OSTaskCreate((void (*) (void *))Transmit_Task,               					//ָ����������ָ��
							  (void *)(PRINT_CELL_NUM_ONE + i),														//����ʼִ��ʱ�����ݸ�����Ĳ�����ָ��
							  (OS_STK *) &TRANSMITTER_TaskStk[i][TRANSMITTER_STK_SIZE - 1],		//���������Ķ�ջ��ջ��ָ��   �Ӷ����µݼ�
							  (INT8U) TRANSMITTER_TASK_PRIO + i);									//�������������ȼ�	
	}		
						  
	OSTimeSet(0);
	
	InitPrintCellsMgr();	//��ʼ����ӡ��Ԫ������
						  
	InitRestoreOrderQueue();// ��ʼ�������ָ�����
						  
	Init_Queue();          //��ʼ��������
													
	Init_Order_Table();    //��ʼ����ӡ���ж�����Ϣ��
	
	ReadPrintCellsInfo();	// ��ȡƬ��ROM���ָ���ӡ��Ԫ������ֵ
						
//	ClearEEPROM();
						  
	/* �½��ڴ�� */
					
	assert((	queue_1K = OSMemCreate(block_1K,BLOCK_1K_NUM,BLOCK_1K_SIZE,&os_err)), "CREATE 1K MEM FAILED\n"); 
	assert((	queue_2K = OSMemCreate(block_2K,BLOCK_2K_NUM,BLOCK_2K_SIZE,&os_err)), "CREATE 2K MEM FAILED\n"); 
	assert((	queue_4K = OSMemCreate(block_4K,BLOCK_4K_NUM,BLOCK_4K_SIZE,&os_err)), "CREATE 4K MEM FAILED\n"); 
	assert((	queue_10K = OSMemCreate(block_10K,BLOCK_10K_NUM,BLOCK_10K_SIZE,&os_err)), "CREATE 10K MEM FAILED\n"); 
	OSStart();	 	/* ����UCOSII����ϵͳ */

	while(1);  //һ�㲻�ᵽ����������������
}


static void Lwip_TaskStart(void* p_arg)
{
	INT8U err;
	extern struct netconn *order_netconn;	//ȫ��TCP����
	(void) p_arg;                          
#if (OS_TASK_STAT_EN > 0)   //��ʹ��ucos ��ͳ������
	OSStatInit(); 	//----ͳ�������ʼ������                                 
#endif
	
	LwIP_Init();
	while(1)
	{
		con_to_server();
		OSSemPend(Recon_To_Server_Sem, 0, &err);
	}
}


/****************************************************************************
* ��    �ƣ�u32_t sys_now(void)
* ��    �ܣ�ʱ�Ӻ��������ڻ�ȡϵͳʱ�ӵδ�20msʱ��system_tick_num��һ
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
u32_t sys_now(void)
{
	return system_tick_num;
}



/****************************************************************************
* ��    �ƣ�static  void Print_Task(void* p_arg)
* ��    �ܣ���ӡ���ڴ�ӡ����ͷ�Ķ�����������ɺ����Ƴ����к��ͷſռ�
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
static  void Print_Task(void* p_arg)
{
	INT8U err;
	u8_t entry;
	order_info *orderp;
	(void) p_arg;

	while(1)
	{
		DEBUG_PRINT("Print_Task: ORDER  WAITING\n");			
		OSSemPend(Print_Sem, 0, &err);		
		DEBUG_PRINT("Print_Task: ORDER  GET\n");
		
		if(GetRestoredOrder(&entry) == 1 || GetOrderFromQueue(&entry) ==  ORDER_QUEUE_OK) {	// �ɹ���ȡ����
			if(CheckOrderData(entry) == ORDER_DATA_OK) {	// ����������ȷ���·���ӡ����
				DEBUG_PRINT("Print_Task: ORDER DATA CHECK OK\n");
				DispensePrintJob(entry);	
			}else {		// �������ݴ��󣬶�������
				Delete_Order(entry);
			}
		}else {
			DEBUG_PRINT("Print_Task: SEM BUG: Print_Task: None Print Order.\n");
		}
	}
}

/****************************************************************************
* ��    �ƣsstatic  void Recv_Batch_Task(void* p_arg)
* ��    �ܣ������������񣬵�����������һ������ʱ
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
static  void Recv_Batch_Task(void* p_arg)
{
	extern struct netconn *order_netconn;	//ȫ��TCP����

	(void) p_arg;                          

	while(1)
	{	
		if(order_netconn != NULL)
		{
			receive_connection(order_netconn);
#if APP_DEBUG
		//	write_connection(order_netconn, first_req, REQ_LINK_OK, 0);//����������
#endif
		}else{
			OSTimeDlyHMSM(0, 0, 1, 0);
		}

	}
}

/****************************************************************************
* ��    �ƣsstatic  void Wifi_Rec_Task(void* p_arg)
* ��    �ܣ����ؽ��ն�����Ӧ������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
static  void Wifi_Rec_Task(void* p_arg)
{
	(void) p_arg; 
	
	while(1)
	{
		
		wifi_receive();
	}
}


/****************************************************************************
* ��    �ƣsstatic  void Local_Rec_Task(void* p_arg)
* ��    �ܣ����ؽ��ն�����Ӧ������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
static  void Local_Rec_Task(void* p_arg)
{
	(void) p_arg; 
	
	while(1)
	{
		local_receive();
	}
}

/****************************************************************************
* ��    �ƣsstatic  void Deal_Wifi_Req_Task(void* p_arg)
* ��    �ܣ����ؽ��ն�����Ӧ������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
static  void Deal_Wifi_Req_Task(void* p_arg)
{
	(void) p_arg; 
	
	while(1)
	{
		DealWifiReq(&heap);
	}
}

/****************************************************************************
* ��    �ƣsstatic  void Print_Queue_Task(void* p_arg)
* ��    �ܣ������ڴ�飬������ӡ����
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
static  void Print_Queue_Task(void* p_arg)
{
	Print_Queue_Fun();
}

/****************************************************************************
* ��    �ƣsstatic  void Health_Detect_Task(void* p_arg)
* ��    �ܣ���ӡ��״̬�������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
static  void Health_Detect_Task(void * p_arg )
{
	Health_Detect_Fun();
	return ;
}

/****************************************************************************
* ��    �ƣsstatic  void Mesg_Queue_Task(void* p_arg)
* ��    �ܣ������Ϣ�������ݷ���
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
static void Mesg_Queue_Task(void * p_arg)
{
	MesgQue_Deal();
	return ;
}


/****************************************************************************
* ��    �ƣsstatic  void Transmit_Task(void* p_arg)
* ��    �ܣ������ӡ��Ԫ��ʵ�ʴ��乤��
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/

static void Transmit_Task(void * p_arg)
{
	u8_t cellno = (u8_t)p_arg;
	PrintCellInfo *cellp = &PCMgr.cells[cellno-1];
	u8_t err;
	
	
	while(1) 
	{
		
		OSSemPend(cellp->printBeginSem, 0, &err);
		
		Print_Order(cellno);
	}
	
	return ;
}