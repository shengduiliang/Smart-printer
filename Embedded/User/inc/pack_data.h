#ifndef PACK_DATA_H
#define PACK_DATA_H

/**************************************************************
*	Include File Section
**************************************************************/
#include "cc.h"
#include "more_infomation.h"
#include "data_form.h"



/**************************************************************
*	Macro Define Section
**************************************************************/
//32λ����data�����add��ʼ��4���ֽ���
#define SET_DATA_4B(add, data) 		\
	do {							\
		*add = data >> 24;			\
		*(add + 1) = data >> 16;	\
		*(add + 2) = data >> 8;		\
		*(add + 3) = data;			\
	}while(0)

//16λ����data�����add��ʼ��2���ֽ���
#define SET_DATA_2B(add, data) 		\
	do {							\
		*add = data >> 8;			\
		*(add + 1) = data;			\
	}while(0)
	
/**************************************************************
*	Function Decalre Section
**************************************************************/
/****************************************************************************************
*@Name............: Pack_Req_Or_Status_Message
*@Description.....: ��װ���ݱ�
*@Parameters......: message		:���ڴ�ż����������Ϣ
*					type		:��������
*					symbol		:��־λ
*					id			:���������ذ�id(32λ),Ҳ�����������(��16λ)
*					UNIX_time	:�����������ذ巢��ʱ��
*					preservation:�����ֶΣ��ڶ����У���16λΪ������ţ���16λΪ���������
*@Return values...: void

*****************************************************************************************/
void Pack_Req_Or_Status_Message(char *message, req_type type, u8_t symbol, u32_t id, u32_t UNIX_time, u32_t preservation);
#endif
