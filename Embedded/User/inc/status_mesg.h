/***************************************************************************************
 *	FileName					:	status_mesg.h
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
*        Multi-Include-Prevent Section
**************************************************************/
#ifndef STATUS_MESG_H
#define STATUS_MESG_H

/**************************************************************
*        Debug switch Section
**************************************************************/


/**************************************************************
*        Include File Section
**************************************************************/
#include "cc.h"
#include "ucos_ii.h"
#include <stdlib.h>
#include "print_queue.h"
#include "app_cfg.h"
#include "wifi_conf.h"

/**************************************************************
*        Macro Define Section
**************************************************************/

/*
	PRINTER_MESG_IP WiFi�µ�ʱ����ӡ��Ԫ״̬�����Ĺ㲥IP
*/
#define PRINTER_MESG_IP 0


/*
	��Ϣ���г���
*/
#define MESG_QUEUE_SIZE	20

/*
	MesgQueueNode��ʹ�ã����flag���������ڱ�ʶ����һ���͵���Ϣ
*/
#define BATCH_MESG_QUE_FLAG					1
#define ORDER_QUEUE_MESG_QUE_FLAG		2
#define ORDER_PRINT_MESG_QUE_FLAG		3
#define PRINTER_MESG_QUE_FLAG				4

/*
	������Զ�����ݺ�
*/
#define REMOTE_SOURCE 0 //��������յĳ�ʼ����
#define LOCAL_SOURCE	1	//�ӱ��س�ʼ���յĶ���
#define REMOTE_RE_SOURCE 2	//��������յ��쳣��������
#define LOCAL_RE_SOURCE	3		//�ӱ��ؽ��յ��쳣��������
#define WIFI_SOURCE 4	//��WiFi���ܶ���
#define WIFI_RE_SOURCE 5 //��WiFi�����쳣��������



/*
	Batch_Status_Send�������ú�
	����״̬
*/
#define BATCH_PRINT_SUCCESS  0
#define BATCH_ENUEUE_SUCCESS 1

/* 
	Order_QUEUE_Status_Send ����
	��ӡ����״̬
	*/
#define ENQUEUE_OK 0   //���Ͷ��������ӡ���гɹ����ĵĺ�
#define ENQUEUE_ERR 1  //���Ͷ��������ӡ����ʧ�ܱ��ĵĺ�
#define ENQUQUE_START 2

/* 
	Order_Print_Status_Send�У��ж϶���״̬�Ĳ���
	������ӡ״̬
*/
#define PRINT_STATUS_OK 						0			//��ӡ�ɹ�
#define PRINT_STATUS_DATA_ERR 			1			//��ӡʧ�ܣ�������������
#define PRINT_STATUS_MACHINE_ERR 		2			//��ӡʧ�ܣ���ӡ���쳣
#define PRINT_STATUS_START 					3			//��ʼ��ӡ



/*
	���౨��״ֵ̬
	*/

//����״̬
#define BATCH_PRINT_SUC				0		//�ɹ���ӡ����
#define	BATCH_ENQUE_BUF_SUC		1		//�ɹ����뻺����
	
//����״̬
#define NOR_ORDER_PRINT_SUC		0		//��ͨ������ӡ�ɹ�
#define NOR_ORDER_PRINTER_ERR	1		//��ͨ������ӡ������ӡ���쳣
#define NOR_ORDER_ENQUE_SUC		2		//��ͨ���������ӡ����
#define NOR_ORDER_PRINT_START	3		//��ͨ������ʼ��ӡ
#define NOR_ORDER_DATA_ERR		4		//��ͨ�����������ݽ�������

#define EX_ORDER_PRINT_SUC		5		//ǰ�쳣������ӡ�ɹ�
#define EX_ORDER_PRINTER_ERR	6		//ǰ�쳣������ӡ������ӡ���쳣
#define EX_ORDER_ENQUE_SUC		7		//ǰ�쳣���������ӡ����
#define EX_ORDER_PRINT_START	8		//ǰ�쳣������ʼ��ӡ
#define EX_ORDER_DATA_ERR			9		//ǰ�쳣���󶩵����ݽ�������

/*
	Printer_Status_Send�������ú�
	��ӡ��״̬
*/
#define PRINTER_CUT_ERR 							0x01		//�е�����
#define PRINTER_CASE_OPEN 						0x02		//���д�
#define PRINTER_PAPER_MAY_RUNOUT 			0x03		//ֽ���þ�
#define PRINTER_PAPER_RUNOUT 					0x04		//ȱֽ
#define PRINTER_PAPER_FEED						0x05		//���ڽ�ֽ
#define PRINTER_HIGH_TEMP							0x06		//��о����
#define PRINTER_BURN_DOWN 						0x07		//��о�ջ�
#define PRINTER_NORMAL								0x09		//����״̬
#define PRINTER_BUF_NORMAL_FULL 			0x0c		//��ͨ��������
#define PRINTER_BUF_URGENT_FULL				0x0d		//������������
#define PRINTER_HEALTH_HEALTHY				0x0e	//��ͨ״̬
#define PRITNER_HEALTH_SUB_HEALTH 		0x0f	//�ǽ���״̬
#define PRINTER_HEALTH_UNHEALTHY			0x10	//������״̬



/*
	getHealthValue ����ֵ
	*/
#define HEALTHY						PRINTER_HEALTH_HEALTHY				//����
#define SUB_HEALTHY				PRITNER_HEALTH_SUB_HEALTH			//�ǽ���

/*
	��ӡ������״̬��ֵ
	*/
#define TOTAL_WORKTIME_THRESHOLD	 						100000000 //�ܹ���ʱ����ֵ��2�� ��λ0.1s	����
#define TOTAL_CUT_NUM_THRESHOLD								1000000		//���е�������ֵ��100���
#define TOTAL_PRINT_LEN_THRESHOLD							10000000	//�ܴ�ӡ������ֵ��100km,��λcm
#define ERR_NUM_THRESHOLD											10000			//�ܴ�����������1���   			����
#define KEEP_WORKING_TIME_THRESHOLD						10000			//�����ʱ�䣺��λ0.1s				����

/**************************************************************
*        Struct Define Section
**************************************************************/

//������Ϣ���ݽṹ��
typedef struct{
	u16_t batch_num;
}Batch_Status;

//������Ϣ���ݽṹ��
typedef struct{
	u16_t batch_num;
	u16_t order_num; 
	u8_t data_source;
}Order_QUEUE_Status;

//������Ϣ���ݽṹ��
typedef struct{
	u16_t batch_num;
	u16_t order_num; 
	u8_t data_source;
}Order_Print_Status;

//������Ϣ���ݽṹ��
typedef struct{
	u32_t printer_num;
}Printer_Status;


typedef union{
	Batch_Status				batch_Status;
	Order_QUEUE_Status	order_QUEUE_Status;
	Order_Print_Status 	order_Print_Status;
	Printer_Status			printer_Status;
}MesgQueueData;

/*
	��Ϣ������ָ����ָ���ݽڵ�
	flag����ʾ�Ǻ���������Ϣ
mesgQueueData :��Ϣ����
status ��״̬
*/
typedef struct{
	u8_t flag ;
	MesgQueueData mesgQueueData;
	u32_t ip;
	u8_t status;		
}MesgQueueNode;




/**************************************************************
*        Prototype Declare Section
**************************************************************/


extern void* MesgQueArray [MESG_QUEUE_SIZE];
extern MesgQueueNode MesgQueBuf[MESG_QUEUE_SIZE];
extern u32_t MesgQueIndex ; 

/**
 *  @fn		Batch_Status_Send
 *	@brief 	��������״̬����
 *	@param	batch_num 	
						status 			����״̬
											��BATCH_PRINT_SUCCESS �����δ�ӡ�ɹ� 
												BATCH_ENUEUE_SUCCESS�����ν����ӡ���гɹ���
 *	@ret	None
 */
void Batch_Status_Send(u16_t batch_num ,u8_t status);

/**
 *  @fn		Order_QUEUE_Status_Send
 *	@brief 	���Ͷ��������ӡ����״̬����
 *	@param	order 		�����ṹ��ָ��
						status		����״̬
 *	@ret	None
 */
void Order_QUEUE_Status_Send( order_info* order , u8_t status);

/**
 *  @fn		Order_Print_Status_Send
 *	@brief 	���Ͷ�����ӡ״̬����
 *	@param	order 		�����ṹ��ָ��
						status		����״̬
 *	@ret	None
 */
void Order_Print_Status_Send ( order_info* order, u8_t status);

/**
 *  @fn		Printer_Status_Send
 *	@brief 	���Ͷ�����ӡ״̬����
 *	@param	printer_num 	��ӡ����
						status		��ӡ��״̬
 *	@ret	None
 */
void Printer_Status_Send(u32_t printer_num , u8_t status);


/**
 *  @fn		MesgQue_Deal
 *	@brief 	������ӡ������״̬
 *	@param			
 *	@ret	ans
 */
void MesgQue_Deal();

/**
 *  @fn		getHealthValue
 *	@brief 	������ӡ������״̬
 *	@param	status		��ӡ��״̬
 *	@ret	ans	���ؽ���ֵ��
								����
								�ǽ���
								������
 */
u8_t getHealthValue(u32_t printer_num);


/**
 *  @fn		Health_Detect_Fun
 *	@brief 	����״̬�������
 *	@param	
 *	@ret	ans	
 */
void Health_Detect_Fun();
/**************************************************************
*        End-Multi-Include-Prevent Section
**************************************************************/
#endif