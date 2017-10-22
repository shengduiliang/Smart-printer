/***************************************************************************************
 *	FileName					:	status_mesg.c
 *	CopyRight					:
 *	ModuleName					:	
 *
 *	CPU							:
 *	RTOS						:
 *
 *	Create Data					:	2016/07/29
 *	Author/Corportation			:	subaideng
 *
 *	Abstract Description		:	
 *
 *--------------------------------Revision History--------------------------------------
 *	No		version		Date		Revised By		Item		Description
 *	1		v1.0		2016/		subaideng					Create this file
 *
 ***************************************************************************************/
 


/**************************************************************
*	Debug switch Section
**************************************************************/
//#define DEBUG_DETECT

/**************************************************************
*	Include File Section
**************************************************************/
#include "status_mesg.h"


/**************************************************************
*	Macro Define Section
**************************************************************/
	
/**************************************************************
*	Struct Define Section
**************************************************************/
//��Ϣ����
void* MesgQueArray [MESG_QUEUE_SIZE];
//��Ϣ���ݻ������
MesgQueueNode MesgQueBuf[MESG_QUEUE_SIZE];
u32_t MesgQueIndex = 0; 

/**************************************************************
*	Prototype Declare Section
**************************************************************/


/**************************************************************
*	Global Variable Declare Section
**************************************************************/
extern OS_EVENT *Mesg_Queue;
/**************************************************************
*	File Static Variable Define Section
**************************************************************/


/**************************************************************
*	Function Define Section
**************************************************************/


/**
 *  @fn		Batch_Status_Send
 *	@brief 	��������״̬����
 *	@param	batch_num 	
						status 			����״̬
											��BATCH_PRINT_SUCCESS �����δ�ӡ�ɹ� 
												BATCH_ENUEUE_SUCCESS�����ν����ӡ���гɹ���
 *	@ret	None
 */
void Batch_Status_Send(u16_t batch_num ,u8_t status)
{
	MesgQueBuf[MesgQueIndex].flag = BATCH_MESG_QUE_FLAG;
	MesgQueBuf[MesgQueIndex].mesgQueueData.batch_Status.batch_num = batch_num;
	MesgQueBuf[MesgQueIndex].status = status;
	OSQPost(Mesg_Queue,&MesgQueBuf[MesgQueIndex]);
	MesgQueIndex = (MesgQueIndex + 1) % MESG_QUEUE_SIZE;
	return;
}


/**
 *  @fn		Order_QUEUE_Status_Send
 *	@brief 	���Ͷ��������ӡ����״̬����
 *	@param	order 		�����ṹ��ָ��
						status		����״̬
 *	@ret	None
 */
void Order_QUEUE_Status_Send ( order_info* order , u8_t status )
{

	MesgQueBuf[MesgQueIndex].flag = ORDER_QUEUE_MESG_QUE_FLAG;
	MesgQueBuf[MesgQueIndex].mesgQueueData.order_QUEUE_Status.batch_num = order->batch_number;
	MesgQueBuf[MesgQueIndex].mesgQueueData.order_QUEUE_Status.order_num = order->batch_within_number;
	MesgQueBuf[MesgQueIndex].mesgQueueData.order_QUEUE_Status.data_source = order->data_source;
	MesgQueBuf[MesgQueIndex].ip = order->sever_send_time;
	MesgQueBuf[MesgQueIndex].status = status;	

	OSQPost(Mesg_Queue,&MesgQueBuf[MesgQueIndex]);
	
	MesgQueIndex = (MesgQueIndex + 1) % MESG_QUEUE_SIZE;
	return;
}


/**
 *  @fn		Order_Print_Status_Send
 *	@brief 	���Ͷ�����ӡ״̬����
 *	@param	order 		�����ṹ��ָ��
						status		����״̬
 *	@ret	None
 */
void Order_Print_Status_Send ( order_info* order ,u8_t status)
{
	MesgQueBuf[MesgQueIndex].flag = ORDER_PRINT_MESG_QUE_FLAG;
	MesgQueBuf[MesgQueIndex].mesgQueueData.order_Print_Status.batch_num = order->batch_number;
	MesgQueBuf[MesgQueIndex].mesgQueueData.order_Print_Status.order_num = order->batch_within_number;
	MesgQueBuf[MesgQueIndex].mesgQueueData.order_Print_Status.data_source = order->data_source;
	MesgQueBuf[MesgQueIndex].ip = order->sever_send_time;
	MesgQueBuf[MesgQueIndex].status = status;		
	OSQPost(Mesg_Queue,&MesgQueBuf[MesgQueIndex]);
	
	MesgQueIndex = (MesgQueIndex + 1) % MESG_QUEUE_SIZE;
	return;
}


/**
 *  @fn		Printer_Status_Send
 *	@brief 	���Ͷ�����ӡ״̬����
 *	@param	printer_num 	��ӡ����
						status		��ӡ��״̬
 *	@ret	None
 */
void Printer_Status_Send(u32_t printer_num , u8_t status)
{
	MesgQueBuf[MesgQueIndex].flag = PRINTER_MESG_QUE_FLAG;
	MesgQueBuf[MesgQueIndex].mesgQueueData.printer_Status.printer_num = printer_num;
	MesgQueBuf[MesgQueIndex].ip = PRINTER_MESG_IP;
	MesgQueBuf[MesgQueIndex].status = status;		
	OSQPost(Mesg_Queue,&MesgQueBuf[MesgQueIndex]);	
	
	MesgQueIndex = (MesgQueIndex + 1) % MESG_QUEUE_SIZE;	
	return ;
}






/**
 *  @fn		Batch_Status_Mesg_Queue_Send
 *	@brief 	��������״̬����
 *	@param	batch_num 	
						status 			����״̬
											��BATCH_PRINT_SUCCESS �����δ�ӡ�ɹ� 
												BATCH_ENUEUE_SUCCESS�����ν����ӡ���гɹ���
 *	@ret	None
 */
static void Batch_Status_Mesg_Queue_Send(u16_t batch_num ,u8_t status)
{
	switch(status){
		case BATCH_PRINT_SUCCESS:{//���γɹ���ӡ����
			BASE_SEND_STATUS(batch_status, BATCH_PRINT_SUC, (u32_t)batch_num<<16 );		
			break;
		}
		case BATCH_ENUEUE_SUCCESS:{//���γɹ����뻺��������
			BASE_SEND_STATUS(batch_status, BATCH_ENQUE_BUF_SUC, (u32_t)batch_num<<16 );		
			break;
		}
	}
}


/**
 *  @fn		Order_QUEUE_Status_Mesg_Queue_Send
 *	@brief 	���Ͷ��������ӡ����״̬����
 *	@param	batch_num 	���κţ����Ǳ��ض��������κ�Ϊ0��
						order_num		�����ڶ�����
						status 			���������ӡ���е�״̬
											��ENQUEUE_OK �������ӡ���гɹ� 
												ENQUEUE_ERR�� �����ӡ����ʧ�ܣ�
						data_source ����Դ
						ip          Wifi �µ�ʱ��IP
 *	@ret	None
 */
static void Order_QUEUE_Status_Mesg_Queue_Send ( u16_t batch_num , u16_t order_num , u8_t status , u8_t data_source, u32_t ip )
{
		if(status == ENQUEUE_OK)
	{																	//�ɹ������ӡ����
		switch(data_source){
			case REMOTE_SOURCE:{
				BASE_SEND_STATUS(order_status, NOR_ORDER_ENQUE_SUC, (u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}
			case REMOTE_RE_SOURCE:{
				BASE_SEND_STATUS(order_status, EX_ORDER_ENQUE_SUC, (u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}
			case LOCAL_SOURCE:{
				SendStatusToLocal(order_status, NOR_ORDER_ENQUE_SUC,(u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}
			case LOCAL_RE_SOURCE:{
				SendStatusToLocal(order_status, EX_ORDER_ENQUE_SUC,(u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}		
			case WIFI_SOURCE:{
				SendStatusToWifi(order_status, NOR_ORDER_ENQUE_SUC, ip,(u32_t)batch_num<<16 | (u32_t)order_num);
				break;
			}
			case WIFI_RE_SOURCE:{			
				SendStatusToWifi(order_status, EX_ORDER_ENQUE_SUC, ip,(u32_t)batch_num<<16 |(u32_t)order_num);
			  break;
			}
		}		
	}
	else if(status == ENQUEUE_ERR)
		{																	//ʧ�ܽ����ӡ���У����ݽ�������
		switch(data_source){
			case REMOTE_SOURCE:{
				BASE_SEND_STATUS(order_status, NOR_ORDER_DATA_ERR, (u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}
			case REMOTE_RE_SOURCE:{
				BASE_SEND_STATUS(order_status, EX_ORDER_DATA_ERR, (u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}
			case LOCAL_SOURCE:{
				SendStatusToLocal(order_status, NOR_ORDER_DATA_ERR,(u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}
			case LOCAL_RE_SOURCE:{
				SendStatusToLocal(order_status, EX_ORDER_DATA_ERR,(u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}		
			case WIFI_SOURCE:{
				SendStatusToWifi(order_status, NOR_ORDER_DATA_ERR, ip,(u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}
			case WIFI_RE_SOURCE:{			
				SendStatusToWifi(order_status, EX_ORDER_DATA_ERR, ip,(u32_t)batch_num<<16 |(u32_t)order_num);
			  break;
			}
		}
	}
}


/**
 *  @fn		Order_Print_Status_Mesg_Queue_Send
 *	@brief 	���Ͷ�����ӡ״̬����
 *	@param	batch_num 	���κ�
						order_num		�����ڶ�����
						status 			��������ӡ��״̬
												PRINT_STATUS_OK 		�� ��ӡ�ɹ�
											  PRINT_STATUS_ERR 		�� ��ӡʧ��
											  PRINT_STATUS_START 	�� ��ʼ��ӡ
						data_source ����Դ
 *	@ret	None
 */
void Order_Print_Status_Mesg_Queue_Send ( u16_t batch_num , u16_t order_num , u8_t status , u8_t data_source ,u32_t ip)
{
	if(status == PRINT_STATUS_START)							//������ʼ��ӡ
	{
		switch(data_source){
			case REMOTE_SOURCE:{
				BASE_SEND_STATUS(order_status, NOR_ORDER_PRINT_START, (u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}
			case REMOTE_RE_SOURCE:{
				BASE_SEND_STATUS(order_status, EX_ORDER_PRINT_START, (u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}
			case LOCAL_SOURCE:{
				SendStatusToLocal(order_status, NOR_ORDER_PRINT_START,(u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}
			case LOCAL_RE_SOURCE:{
				SendStatusToLocal(order_status, EX_ORDER_PRINT_START,(u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}		
			case WIFI_SOURCE:{
				SendStatusToWifi(order_status, NOR_ORDER_PRINT_START, ip,(u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}
			case WIFI_RE_SOURCE:{
				SendStatusToWifi(order_status, EX_ORDER_PRINT_START, ip,(u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}	
		}
	}
	else if(status == PRINT_STATUS_OK)						//��ӡ�ɹ�
	{
		switch(data_source){
			case REMOTE_SOURCE:{

				BASE_SEND_STATUS(order_status, NOR_ORDER_PRINT_SUC, (u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}
			case REMOTE_RE_SOURCE:{
				BASE_SEND_STATUS(order_status, EX_ORDER_PRINT_SUC, (u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}
			case LOCAL_SOURCE:{
				SendStatusToLocal(order_status, NOR_ORDER_PRINT_SUC,(u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}
			case LOCAL_RE_SOURCE:{
				SendStatusToLocal(order_status, EX_ORDER_PRINT_SUC,(u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}	
			case WIFI_SOURCE:{
				SendStatusToWifi(order_status, NOR_ORDER_PRINT_SUC, ip,(u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}
			case WIFI_RE_SOURCE:{
				SendStatusToWifi(order_status, EX_ORDER_PRINT_SUC, ip,(u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}			
		}
	}
	else if(status == PRINT_STATUS_DATA_ERR)			//������ӡʧ�ܣ�������������
	{
		switch(data_source){
			case REMOTE_SOURCE:{
				BASE_SEND_STATUS(order_status, NOR_ORDER_DATA_ERR, (u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}
			case REMOTE_RE_SOURCE:{
				BASE_SEND_STATUS(order_status, EX_ORDER_DATA_ERR, (u32_t)batch_num<<16 |(u32_t)order_num);
				break; 
			}
			case LOCAL_SOURCE:{
				SendStatusToLocal(order_status, NOR_ORDER_DATA_ERR,(u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}
			case LOCAL_RE_SOURCE:{
				SendStatusToLocal(order_status, EX_ORDER_DATA_ERR,(u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}		
			case WIFI_SOURCE:{
				SendStatusToWifi(order_status, NOR_ORDER_DATA_ERR, ip,(u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}
			case WIFI_RE_SOURCE:{
				SendStatusToWifi(order_status, EX_ORDER_DATA_ERR, ip,(u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}
		}
	}
	else if(status == PRINT_STATUS_MACHINE_ERR)		//������ӡʧ�ܣ���ӡ���쳣
	{
		switch(data_source){
			case REMOTE_SOURCE:{
				BASE_SEND_STATUS(order_status, NOR_ORDER_PRINTER_ERR, (u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}
			case REMOTE_RE_SOURCE:{
				BASE_SEND_STATUS(order_status, EX_ORDER_PRINTER_ERR, (u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}
			case LOCAL_SOURCE:{
				SendStatusToLocal(order_status, NOR_ORDER_PRINTER_ERR,(u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}
			case LOCAL_RE_SOURCE:{
				SendStatusToLocal(order_status, EX_ORDER_PRINTER_ERR,(u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}		
			case WIFI_SOURCE:{
				SendStatusToWifi(order_status, NOR_ORDER_PRINTER_ERR,ip,(u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}
			case WIFI_RE_SOURCE:{
				SendStatusToWifi(order_status, EX_ORDER_PRINTER_ERR,ip,(u32_t)batch_num<<16 |(u32_t)order_num);
				break;
			}	
		}
	}
	return;
}


/**
 *  @fn		Printer_Status_Mesg_Queue_Send
 *	@brief 	���Ͷ�����ӡ״̬����
 *	@param	printer_num 	��ӡ����
						status		��ӡ��״̬
 *	@ret	None
 */
void Printer_Status_Mesg_Queue_Send(u32_t printer_num , u8_t status)
{
	//status�꣬���Ƕ�Ӧ�ķ���״̬
	DEBUG_PRINT("SEND MESG : start to send printer status \n");
	SendStatusToWifi(printer_status,status, PRINTER_MESG_IP,printer_num+1);
	BASE_SEND_STATUS(printer_status,status,printer_num+1);
	SendStatusToLocal(printer_status,status,printer_num+1); 	
	return ;
}


/**
 *  @fn		MesgQue_Deal
 *	@brief 	������ӡ������״̬
 *	@param			
 *	@ret	ans
 */
void MesgQue_Deal()
{
	INT8U err ;
	MesgQueueNode * receiveMesg ;
	
	while(1){
		receiveMesg = (MesgQueueNode *)OSQPend(Mesg_Queue,0,&err);
//		DEBUG_PRINT("\n order num is %d",receiveMesg->mesgQueueData.order_Print_Status.order_num);
		switch(receiveMesg->flag){
			case BATCH_MESG_QUE_FLAG:{
				Batch_Status_Mesg_Queue_Send(receiveMesg->mesgQueueData.batch_Status.batch_num,receiveMesg->status);				
				break;
			}
			case ORDER_QUEUE_MESG_QUE_FLAG:{
				Order_QUEUE_Status_Mesg_Queue_Send(receiveMesg->mesgQueueData.order_QUEUE_Status.batch_num,
																						receiveMesg->mesgQueueData.order_QUEUE_Status.order_num,
																						receiveMesg->status,
																						receiveMesg->mesgQueueData.order_QUEUE_Status.data_source,
																						receiveMesg->ip);
				break;
			}
			case ORDER_PRINT_MESG_QUE_FLAG:{
				Order_Print_Status_Mesg_Queue_Send(receiveMesg->mesgQueueData.order_Print_Status.batch_num,
																						receiveMesg->mesgQueueData.order_Print_Status.order_num,
																						receiveMesg->status,
																						receiveMesg->mesgQueueData.order_Print_Status.data_source,
																						receiveMesg->ip);
				break;
			}
			case PRINTER_MESG_QUE_FLAG:{
				Printer_Status_Mesg_Queue_Send(receiveMesg->mesgQueueData.printer_Status.printer_num,receiveMesg->status);	
					
				break;
			}		
			default:
				DEBUG_PRINT("Mesg Queue Send Judge  ERROR!!!!!\n");
		}
	}
	return;
}


/**
 *  @fn		getHealthValue
 *	@brief 	������ӡ������״̬
 *	@param	status		��ӡ��״̬
 *	@ret	ans	���ؽ���ֵ��
								HEALTHY ��		����
								SUB_HEALTHY��	�ǽ���
 */
u8_t getHealthValue(u32_t printer_num)
{
	u8_t value = PRINTER_HEALTH_HEALTHY;
	
	if(PCMgr.cells[printer_num].totalTime > TOTAL_WORKTIME_THRESHOLD ){		
		value = PRITNER_HEALTH_SUB_HEALTH; 
	}
	else if(PCMgr.cells[printer_num].cutCnt > TOTAL_CUT_NUM_THRESHOLD){
		value = PRITNER_HEALTH_SUB_HEALTH;
	}	
	else if(PCMgr.cells[printer_num].totalLength> TOTAL_PRINT_LEN_THRESHOLD){
		value = PRITNER_HEALTH_SUB_HEALTH;
	}	
	else if(PCMgr.cells[printer_num].workedTime > KEEP_WORKING_TIME_THRESHOLD){
		value = PRITNER_HEALTH_SUB_HEALTH;
	}
	return value;
}



/**
 *  @fn		Health_Detect_Fun
 *	@brief 	����״̬�������
 *	@param	
 *	@ret	ans	
 */
void Health_Detect_Fun()
{
	extern OS_EVENT * Printer_Status_Rec_Sem;
	extern  PrintCellsMgrInfo PCMgr;
	unsigned char value = 0;
	INT8U os_err;
	u32_t printer_num = 0;
	
	while(1){
		int i = 0 ;
		//��ʱ
		OSTimeDlyHMSM(0,0,2,0);
		//��ѯÿ̨��ӡ��Ԫ
		for( i = 0 ; i < MAX_CELL_NUM ; ++i) {
			printer_num = i;
		//״̬���
			//����Ҫ�����ӡ��״̬
			if(PCMgr.cells[printer_num].status != PRINT_CELL_STATUS_BUSY){	
				DEBUG_PRINT("Health_Detect_Fun: Sending Status Cmd to PC: %u\n", 
									PCMgr.cells[printer_num].no);
				SEND_STATUS_CMD_ONE(PCMgr.cells[printer_num].no);
				//�ȴ�״̬���ܳɹ��ź�����

				OSSemPend(Printer_Status_Rec_Sem,50 * 2,&os_err);		
				//�ж��Ƿ�ʱ
				if(os_err == OS_ERR_TIMEOUT){	//��ӡ��Ԫ����
					if(PCMgr.cells[printer_num].status == PRINT_CELL_STATUS_ERR){//��ӡ�������Ͳ�����					
						continue ;//ʲô�����øɣ�~����Ҳ���÷���
					}
					else{//��ӡ����������Ϊ�쳣�����ߣ�
						OSSemAccept(PCMgr.resrcSem);
						PCMgr.cells[printer_num].status = PRINT_CELL_STATUS_ERR;
						value = PRINTER_HEALTH_UNHEALTHY;//Ҫ���ã��Ƿ�Ҫ�Ӹ�����״̬��
						
						DEBUG_PRINT("\nPrint Cell %u Off-line\n", i+1);
					}
				}
				else{	//��ӡ��û������
					if(PCMgr.cells[printer_num].status == PRINT_CELL_STATUS_ERR ){//��ӡ���쳣	
						value = PRINTER_HEALTH_UNHEALTHY;
					}
					else{//��ӡ�����������ǽ���
						//���ݳ���״̬���㽡��ֵ
						value = getHealthValue(printer_num);
					}
				}
			//��������״̬	
				if( PCMgr.cells[printer_num].health_status != value){				
					Printer_Status_Send(printer_num,value);
					PCMgr.cells[printer_num].health_status = value;
				}
			}
		}
	}
	return ;
}
