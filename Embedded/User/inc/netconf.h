#ifndef __NETCONF_H
#define __NETCONF_H
#include <stdio.h>
#include <string.h>
#include "lwip/opt.h"
#include "lwip/init.h"
#include "lwip/stats.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "lwip/sockets.h"
#include "lwip/ip.h"
#include "lwip/raw.h"
#include "lwip/udp.h"
#include "lwip/tcp_impl.h"
#include "lwip/snmp_msg.h"
#include "lwip/autoip.h"
#include "lwip/igmp.h"
#include "lwip/dns.h"
#include "lwip/timers.h"
#include "netif/etharp.h"
#include "lwip/api.h"
#include "tcpip.h"
#include "lwip/memp.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "ethernetif.h"
#include "data_form.h"
#include "queue_buf.h"
#include "main.h"
#include "pack_data.h"
#include "more_infomation.h"
#include "analyze_data.h"
#include "app_cfg.h"
#include "data_form.h"
#include "more_infomation.h"

extern struct netconn *order_netconn;	//ȫ��TCP����

/**
 * @type					:	��data_form.h�ж��壬�ṹ����req_type
 * @symbol					:	symbol�Ǹ����͵ı�־��Ҳ����data_form.h�ж���
 * @serverid_or_preservation: 	4�ֽ�����
 * 								�ڶ���״̬�У���16λΪ������ţ���16λΪ��������ţ�
 * 								������״̬�У����������(��16λ)
 * 								�ڴ�ӡ��״̬�У������ذ��ӡ��Ԫ��ţ����޶����ӡ��Ԫ����Ϊ0
 * 								�ڳ������������ӷ������ذ�ʱ�������Ϊ0
 * 								�ڴﵽ��ֵ�������ݱ�ʱ�������Ϊ0
 */
#define NON_BASE_SEND_STATUS_ON 1//�Ǳ��뷴����״̬
#if NON_BASE_SEND_STATUS_ON
	#define NON_BASE_SEND_STATUS(type, symbol, serverid_or_preservation) \
				write_connection(order_netconn, type, symbol, serverid_or_preservation)
#else
	#define NON_BASE_SEND_STATUS(type, symbol, serverid_or_preservation) 
#endif

#define BASE_SEND_STATUS_ON 1//���뷴����״̬
#if BASE_SEND_STATUS_ON
	#define BASE_SEND_STATUS(type, symbol, serverid_or_preservation) \
				write_connection(order_netconn, type, symbol, serverid_or_preservation)
#else
	#define BASE_SEND_STATUS(type, symbol, serverid_or_preservation) 
#endif


void LwIP_Init(void);
void lwip_demo(void);
void con_to_server(void);
void Recv_Batch(void);
//void Sent_Status(unsigned char order_No,unsigned char order_status);
void receive_connection(struct netconn *conn);
void put_in_buf(u8_t *data, u16_t len, u16_t urg);
/****************************************************************************************
*@Name............: write_connection
*@Description.....: �������ݱ�
*@Parameters......: conn		:����
*					type		:��������
*					symbol		:��־λ
*					preservation:�����ֶΣ������У���16λΪ������ţ�������ʱ�ҵ�16λΪ���������
*@Return values...: void
*****************************************************************************************/
void write_connection(struct netconn *conn, req_type type, u8_t symbol, u32_t preservation);
#endif
