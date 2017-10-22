#ifndef WIFI_CONF_H
#define WIFI_CONF_H

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
#include "local_conf.h"
#include "RequestHeap.h"

/**************************************************************
*	Macro Define Section
**************************************************************/

/**************************************************************
*	Function Define Section
**************************************************************/
void 	WifiSendData(u8_t *data, u16_t len);		//�Ӵ��ڷ�������
void 	wifi_receive(void);							//���մ������ݲ��������ݴ���
void 	WifiReadData(u8_t *data, u16_t *len);		//��ȡ���ڻ��������ж��ٶ����٣������ƶ�������ָ��
void 	UART6_Hook(u8_t ch);						//���Ӵ���6ȡ�������ݷ��뻺����
void 	AckToWifi(u32_t orderNum, u32_t ipAdd);		//��ӡ������Ӧ�������
void 	WifiRecOrder(u8_t *data);					//���ն���
s8_t 	WifiReadAssignBytes(u8_t *data, u16_t len);	//��ȡ���ڻ�������ָ������Ϊlen
s8_t 	WifiDeleteBuf(u16_t len);					//ɾ������Ϊlen�Ļ�����
void 	putbuf(u8_t *data, u16_t len);				//��wifi����������д��Ӽ�������



#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

#define RX_BUF_MAX_LEN     2048 


	


char * usart6_buf(void);
void wifi_init(void);

/****************************************************************************************
*@Name............: SendStatusToLocal
*@Description.....: �������ݱ�������
*@Parameters......: type		:��������
*					symbol		:��־λ
*					IPadd		:ip��ַ
*					preservation:�����ֶΣ���ӡ��״̬ʱΪ��ӡ��Ԫ��ţ�����״̬ʱ��Ϊ�������
*@Return values...: void
*****************************************************************************************/
void SendStatusToWifi(req_type type, u8_t symbol, u32_t IPadd, u32_t preservation);
#endif
