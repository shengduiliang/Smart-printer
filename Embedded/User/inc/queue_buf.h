#ifndef QUEUE_BUF_H
#define QUEUE_BUF_H

#include "cc.h"
#include "os_cpu.h"
#include "ucos_ii.h"
#include "data_form.h"

#define ElemType u8_t 

/*ѭ������ṹ���壺
@base : ָ���λ������׵�ַ
@read : ��������ָ��
@write : ������дָ�� 
*/
typedef struct 
{
	ElemType *base;
	u32_t read;                    
	u32_t write;
	u32_t MAX;       //�洢���������ռ�
	unsigned char buf_empty;   //���������б�־��1Ϊ�գ�0Ϊ�ǿ�
	struct os_event *mutex;
}SqQueue;


/*************Read_Order_Length����******************/
#define Que_NOT_EMPTY				 	0
#define Que_EMPTY 					 	1

#define BUF_OK               0
/*******************ѭ��������������******************/
#define BUF_FULL            -2
#define BUF_EMPTY 			-3
#define BUF_ARG_ERR         -4
#define BUF_DATA_ERR        -5

#define MAXQSIZE (1024 * 10)   	//���廷�λ�������СΪ10K
#define MAXUSIZE (1024 * 10)  	//����Ӽ���������СΪ10K
#define BUF_END 4              	//���嶩��β������
#define BUF_HEAD 24            	//���嶩��ͷ������



/*
	�ж��Ƿ������ֵ�ĺ�
	*/
	
#define  THRESHOLD_SIZE	(1024 * 7)	//������ֵ��7k��������4k���Ժ�ָ�

extern SqQueue queue_buf;    
extern SqQueue urgent_buf; 

extern void Init_Queue(void);
s8_t Write_Buf(SqQueue *buf,ElemType *e,u32_t len);
s8_t Read_Order_Length_Queue(SqQueue buf,u16_t *order_len);
s8_t Read_Order_Queue(SqQueue *buf,ElemType *e);
u8_t Is_Empty_Queue(SqQueue buf);
u8_t *Get_Batch_Head(SqQueue buf);
u8_t *Get_Order_Head(SqQueue buf);
s8_t Check_Buf_Request_Signal(SqQueue buf);

#endif
