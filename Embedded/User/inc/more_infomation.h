#ifndef MORE_INFO_H
#define MORE_INFO_H

/**************************************************************
*	Include File Section
**************************************************************/
#include "cc.h"
#include "stm32f2xx.h"
#include "print_queue.h"

/**************************************************************
*	Function Decalre Section
**************************************************************/
//У����㷨
u16_t Check_Sum(u16_t *data, int len);

//�������ڲ�ͬ����ʱ����Ҫ�������ν���У���
u16_t Check_Sum_With_Diff_Part(u16_t *src1, int len1, u16_t *src2, int len2);

//��ȡ���ذ�id
u32_t Get_Printer_ID(void);

//��ȡʱ���
u32_t Get_Current_Unix_Time(void);

//�ֽ����0
void Fill_Blank(char *add, u8_t len);

//��ȡ����ʱ���
u32_t Get_Batch_Unix_Time(u16_t batch_number);

//��ȡ����ʱ���
u32_t Get_Order_Unix_Time(u32_t order_number);

/*
	�жϻ��������ݸ�ʽ�Ƿ���ȷ����ʼλ���������ȣ�
	������0�����ʾ��ȷ
	������1�����ʾ����
*/
s8_t checkBufData(SqQueue *buf,u32_t writePtr);

#endif

