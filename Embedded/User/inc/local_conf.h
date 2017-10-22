#ifndef LOCAL_CONF_H
#define LOCAL_CONF_H

/**************************************************************
*	Include File Section
**************************************************************/
#include "cc.h"
#include "more_infomation.h"
#include "data_form.h"
#include "analyze_data.h"
#include "queue_buf.h"
#include "main.h"
#include "pack_data.h"
#include "app_cfg.h"
#include "netconf.h"
#include "analyze_data.h"
#include <string.h>

typedef enum status{
	True, Fail, Success, Error
}Status;


/**************************************************************
*	Macro Define Section
**************************************************************/

/**************************************************************
*	Function Define Section
**************************************************************/

void 	LocalSendData(char *data, u16_t len);		//�Ӵ��ڷ�������
void 	local_receive(void);						//���մ������ݲ��������ݴ���
void 	ReadData(char *data, u16_t *len);			//��ȡ���ڻ��������ж��ٶ����٣������ƶ�������ָ��
void 	USART3_Hook(u8_t ch);						//���Ӵ���3��ȡ�������ݷ��뻺����
void 	AckToLocal(u32_t orderNum);					//��ӡ������Ӧ�������
void 	DealAckFromLocal(char *data);				//������Ա��ص�Ӧ��
void 	recOrder(char *data, u16_t len);			//���ն���
s8_t 	ReadAssignBytes(char *data, u16_t len);		//��ȡ���ڻ�������ָ������Ϊlen
s8_t 	DeleteBuf(u16_t len);						//ɾ������Ϊlen�Ļ�����
u8_t 	GetTypeOfAck(char *data);					//��ȡӦ������
u32_t 	GetAckNumber(char *ack);					//��ȡӦ�����
Status 	CheckOrder(u8_t *data, u16_t len);			//��ⶩ���Ƿ���ȷ
Status 	recAck(u16_t offset);						//����Ӧ��
Status 	CheckRec(u8_t *data, u16_t len);			//�����ձ����Ƿ���ȷ

/****************************************************************************************
*@Name............: SendStatusToLocal
*@Description.....: �������ݱ�������
*@Parameters......: type		:��������
*					symbol		:��־λ
*					preservation:�����ֶΣ���ӡ��״̬ʱΪ��ӡ��Ԫ��ţ�����״̬ʱ��Ϊ�������
*@Return values...: void
*****************************************************************************************/
void SendStatusToLocal(req_type type, u8_t symbol, u32_t preservation);
#endif
