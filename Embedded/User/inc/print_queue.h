#ifndef PRINT_QUEUE_H
#define PRINT_QUEUE_H

#include "cc.h"
#include "queue_buf.h"
#include "data_form.h"
#include "ucos_ii.h"
#include "os_cfg.h"
#include "main.h"
#include "app_cfg.h"
#include "netconf.h"
#include "printerException.h"
#include "print_cells.h"

#define ORDER_QUEUE_OK      	 0
#define ORDER_DATA_ERROR		-1
#define ORDER_PRINTER_ERROR		-2
#define ORDER_EMPTY_BLOCK		-3
#define ORDER_TOO_LARGER    	-100
#define ORDER_FIND_INDEX_ERR  -101


/*
	ReleaseBlock  ��������ֵ  �ͷ��ڴ������
*/
#define RELEASE_ERR			0	//�ͷ�ʧ��
#define RELEASE_OK   		1	//�ͷųɹ�


/*
	Add_Order_To_Print_Queue ��������ֵ
	�жϼ��뵽��ӡ���е�״̬
*/
#define ORDER_ENTRY_ERR     	-102 	//
#define ORDER_QUEUE_EMPTY   	-103 	//


/*
	�������ڴ������
*/
#define BLOCK_1K_NUM     4
#define BLOCK_2K_NUM     3
#define BLOCK_4K_NUM     3
#define BLOCK_10K_NUM    1
#define ALL_BLOCK_NUM    (BLOCK_1K_NUM + BLOCK_2K_NUM + BLOCK_4K_NUM + BLOCK_10K_NUM)

/*
	�ڴ�����  ����
*/
#define BLOCK_1K_INDEX_END	(BLOCK_1K_NUM)
#define BLOCK_2K_INDEX_END	(BLOCK_1K_NUM + BLOCK_2K_NUM)
#define BLOCK_4K_INDEX_END	(BLOCK_1K_NUM + BLOCK_2K_NUM + BLOCK_4K_NUM)
#define BLOCK_10K_INDEX_END	(BLOCK_1K_NUM + BLOCK_2K_NUM + BLOCK_4K_NUM + BLOCK_10K_NUM)

/*
	�ڴ����ʼ  ����
*/
#define BLOCK_1K_INDEX_START	(0)
#define BLOCK_2K_INDEX_START	(BLOCK_1K_INDEX_END)
#define BLOCK_4K_INDEX_START	(BLOCK_2K_INDEX_END)
#define BLOCK_10K_INDEX_START	(BLOCK_4K_INDEX_END)
/*
	�������ڴ���С
*/
#define BLOCK_1K_SIZE 1024*1
#define BLOCK_2K_SIZE 1024*2
#define BLOCK_4K_SIZE 1024*4
#define BLOCK_10K_SIZE 1024*10

#define BLOCK_MAX_SIZE BLOCK_10K_SIZE

/*
	�������ȼ����壺�������ȼ�
*/
#define ORDER_PRIO_LOW        0
#define ORDER_PRIO_HIGH       1
#define ORDER_PRIO_INVALID    2

/*
	������ʱ�䳤��
*/
#define SENT_TIME_NUM       6
#define REC_TIME_NUM        6
#define START_PRINT_TIME    6
#define END_PRINT_TIME      6
#define ORDER_MAX_TIME_NUM  6      //����4���������ĳ���


/*
	�������� ȱ���ڴ��
*/
#define	Lack_Of_1K		-1
#define Lack_Of_2K		-2
#define Lack_Of_4K		-4
#define Lack_Of_10K		-5



/**
 * @brief	�����������Ͷ���
 */
#define DATA_INVALID		0x00
#define DATA_IS_PLAINTEXT 	0x01
#define DATA_IS_IMAGE		0x02
#define DATA_IS_QRCODE		0x03

#define DATA_HEADER_SIZE 	4
#define DATA_FOOTER_SIZE	4

typedef struct buf_information
{
	u32_t common_buf_size;
	u32_t urgent_buf_size;
	u32_t common_buf_remain_capacity;
	u32_t urgent_buf_remain_capacity;
}buf_info;


typedef struct order_print_queue_information
{
	buf_info buf_node;
	order_info order_node[ALL_BLOCK_NUM];
	struct os_event  *mutex;
	u8_t head;   			//��ӡ���ж�������
	u8_t tail;   			//��ӡ���ж�β����
	u8_t empty;  			//��ӡ�����Ƿ�Ϊ�գ�1����ա�0����ǿ�
}order_print_queue_info;



extern order_print_queue_info order_print_table;




void Init_Order_Table(void);

/**
 * @brief	ɾ��һ�ݶ������黹�ڴ���ϵͳ
 * @param	��������
 * @return	ִ�н��
 */
s8_t Delete_Order(s8_t entry_index);



/**
 * @brief	��鶩���������Ƿ��д�
 * @param	��������
 * @return	ORDER_DATA_ERR	����������
			ORDER_DATA_OK	��������ȷ
 */
s8_t CheckOrderData(u8_t entry_index);


/**
 * @brief	��ӡһ�ݶ��������޸���״̬
 * @param	��ӡ��Ԫ����
 * @return	ִ�н��
 */
s8_t Print_Order(u8_t cellno);
	

/**
 * @fn		GetOrderFromQueue
 * @brief	�����ӡ����ͷ��㲢��������������entryp��ָ�ռ�
 * @param	entryp	��Ż�õĽ�������
 * @return	ORDER_QUEUE_OK	��ȡ�ɹ�
			ORDER_QUEUE_EMPTY ����Ϊ�գ���ȡʧ��
 */
s8_t GetOrderFromQueue(u8_t *entryp);


/**
 *  @fn		Order_QUEUE_Status_Send
 *	@brief 	���Ͷ��������ӡ����״̬����
 *	@param	order 		�����ṹ��ָ��
						status		����״̬
 *	@ret	None
 */
void Order_QUEUE_Status_Send( order_info* order , u8_t status);


/**
 * @fn		Print_Queue_Fun
 * @brief	��ӡ������������
 * @param	
 * @return	
 */
void Print_Queue_Fun();


#endif
