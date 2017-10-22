#ifndef DATA_FORM_H
#define DATA_FORM_H

/**************************************************************
*	Include File Section
**************************************************************/
#include "cc.h"

/******************����ͷ����ʽ�����ֶε�ƫ����*****************/
#define ORDER_START_SYMBOL_OFFET          0  //��ʼƫ��ֵ
#define ORDER_SIZE_OFFSET                 2	 //��������ƫ��ֵ
#define ORDER_MCU_ID_OFFSET               4  //���ذ�idƫ��ֵ
#define ORDER_SEVER_SEND_TIME_OFFSET	  8  //����������ʱ��ƫ��ֵ
#define ORDER_SERIAL_NUMBER_OFFSET		  12 //�������ƫ��ֵ
#define ORDER_BATCH_NUMBER_OFFSET		  16 //��������ƫ��ֵ
#define ORDER_BATCH_WITHIN_NUMBER_OFFSET  18 //���������ƫ��ֵ
#define ORDER_CHECK_SUM_OFFSET			  20 //У���ƫ��ֵ
#define ORDER_PRESERVATION_OFFSET		  22 //����ֵƫ��ֵ
#define ORDER_DATA_OFFSET				  24 //������ƫ��ֵ

/******************����ͷ����ʽ�����ֶε�ƫ����*****************/
#define BATCH_START_SYMBOL_OFFSET		  0  //��ʼƫ��ֵ
#define BATCH_ORDER_NUMBER_OFFSET		  2  //��������ƫ��ֵ
#define BATCH_NUMBER_OFFSET				  4  //���α��ƫ��ֵ
#define BATCH_TOTAL_LENGTH_OFFSET		  6  //���γ���
#define BATCH_SEVER_SEND_TIME_OFFSET	  8  //����������ʱ��ƫ��ֵ
#define BATCH_CHECK_SUM_OFFSET			  12 //����У���ƫ��ֵ
#define BATCH_PRESERVATION_OFFSET		  14 //����ֵƫ��ֵ
#define BATCH_TAIL_OFFSET				  18 //����β��ƫ��ֵ

/***********************״̬ͷ����񳤶�***********************/
#define SEND_DATA_SIZE 				  	  20 //״̬ͷ������
#define REC_DATA_SIZE					  20 //״̬ͷ������
//ԭ��Ӧ����ΪSTATUS_HEAD_SIZE�ģ������ǽ����뷴���ĸ��ͬ���������ַ���

/**************************************************************
*	Struct Define Section
**************************************************************/
//�������ݱ�
typedef struct order_information {
	u32_t mcu_id;	 		   //���ذ�id
	u32_t sever_send_time;     //����������ʱ��
	u32_t serial_number;	   //�������
	u16_t size;		  		   //��������
	u16_t batch_number;		   //��������
	u16_t batch_within_number; //���������
	u16_t check_sum;		   //У���
	u16_t preservation;		   //����	
	u16_t data_source;		   //������Դ
	u8_t *data;				   //������
	u8_t  priority;			   //���ȼ����ж��Ƿ�Ϊ�Ӽ�����
	u8_t  status;			   //״̬
	u8_t  next_print_node;	   //��һ��ӡ���	
}order_info;

//�������ݱ�
typedef struct batch_information {
#define MAX_BATCH_NUM	10
#define MAX_BATCH_HEAD_LENGTH 20
	u32_t sever_send_time;    //����������ʱ��
	u16_t order_number;		  //��������
	u16_t batch_length;		  //���γ���
	u16_t batch_number;		  //��������
	u16_t check_sum;		  //У���
	u16_t preservation;		  //�����ֶΣ����һλ�ǽ���ָ��
	u8_t  num_printed_order;  //�Ѵ�ӡ��ɶ�������
	u8_t  num_order_que;
}batch_info;

//״̬������������
typedef enum Req_Type {
	first_req, 		//����������������
	order_req, 		//�ﵽ��ֵ���󶩵�
	batch_status, 	//����״̬
	order_status, 	//����״̬
	printer_status,	//��ӡ��״̬
	wifi_order_req	//wifi��������
}req_type;

#define STATUS_TYPE_OFFSET   2	//״̬����ƫ��ֵ
/**********************״̬����*************************/
#define BATCH_STATUS 	0x00	//����״̬
#define ORDER_STATUS	0x20    //����״̬
#define PRINTER_STATUS  0x40    //��ӡ��״̬
#define FIRST_REQ		0x60	//������������
#define ORDER_REQ		0x80	//�ﵽ��ֵ���󶩵�
#define ACK_STATUS		0xa0	//����״̬Ӧ��
#define WIFI_ORDER_REQ	0xc0	//wifi��������Ӧ��
#define WIFI_REQ_ACK    0xe0	//Ӧ��wifi����

/**********************��־����*************************/
#define BATCH_SUCCESS		0x00	//���δ�ӡ�ɹ�
#define BATCH_ENTER_BUF 	0x01	//���ν��뻺����
#define	BATCH_FAIL			0x02	//���δ�ӡʧ��

#define OEDER_SUCCESS		0x00	//������ӡ�ɹ�
#define ORDER_FAIL			0x01	//������ӡʧ��
#define ORDER_ENTER_QUEUE	0x02	//���������ӡ����
#define ORDER_BEGIN_PRINT	0x03	//������ʼ��ӡ
#define ORDER_DATA_ERR		0x04	//�������ݴ���
#define ORDER_DATA_OK		0x05	//��������У��ɹ�

#define PRINTER_ACK_LOCAL	1		//�������ӡ��֮���Ӧ��
#define ORDER_ACK_LOCAL		0		//�����붩��֮���Ӧ��

#define ORDER_REQUEST 		0x00 	//�ﵽ��ֵ����
#define REQ_LINK_OK	 		0x00	//�������ӳɹ�


/*************************************************************************PrinterExcetpion.c*************************************************************************************************/
//Printer-State
#define KNIFE_ERROR_STATE			0x01	//knife error
#define UNEXPECTED_OPENED_STATE		0x02	//unexpectedly open
#define PAPER_INSUFFICIENT_STATE	0x03	//insufficient paper
#define FILL_IN_PAPER_STATE			0x04	//the printer is being filled in the paper
#define PAPER_WILL_BE_IN_INSUFFICIENT_STATE 0x05	//the printer will be in  insufficient paper state

#define NORMAL_STATE				0x09	//the printer is in normal state
#define EXCEPTION_NUM				5

//#define HIGH_TEMPERATURE_STATE		6
//#define SELF_CHECK_STATE					7

//Whether the order is printed successfully or not
#define ORDER_CREATED_OK					0x73     	//the order is printed successfully
#define ORDER_CREATED_FAIL				0x22			//fail to print the order

//Device-Port
#define USART1_PORT 1
#define USART2_PORT 2
#define USART3_PORT 3

//Iamges-Scale
#define NORMAL_SIZE 							0x00            		//��ͨ�ߴ�
#define DOUBLE_WIDTH 							0x01					  		//����
#define DOUBLE_LENGTH 						0x02								//����
#define FOUR_TIMES_AMPLIFICATION 	0x03								//�ı���С
	

////EEPRROM	����״̬��
//#define EEPROM_CHECK_OK 					0xef								//EEPROM�洢����
//#define EEPROM_CHECK_ERROR				0xee								//EEPROM�洢����

//EEPRROM	�洢�εķ��䣨Ϊ�˷����д������4�ֽڱ�ʾ��
//�豸1�����ݿռ� ��0x00 - 0x1c��

#define DEVICE_ONE_START_ADDR								0x00
#define TOTAL_WORKING_TIME_ADDRESS_DEVICE_ONE 				0x00  		//�ܴ�ӡʱ�䣬4�ֽڵĳ���, ��λ������
#define TOTAL_KNIFE_CUT_TIMES_ADDRESS_DEVICE_ONE 			0x04			//���е�������4�ֽڵĳ��ȣ���λ������
#define TOTAL_PRINTED_LENGTH_DEVICE_ONE 							0x08			//�ܴ�ӡ���ȣ�4�ֽڵĳ��ȣ���λ������

//��¼�豸1���쳣���͵ķ��������ĵ�ַ
#define KNIFE_ERROR_TIMES_ADDRESS_DEVICE_ONE				0x0c			//�е����������4�ֽڵĳ���, ��λ������
#define UNEXPECTED_OPENED_TIMES_ADDRESS_DEVICE_ONE			0x10			//��ͷ���򿪴�����4�ֽڵĳ���, ��λ������
#define PAPER_INSUFFICIENT_TIMES_ADDRESS_DEVICE_ONE			0x14			//ȱֽ������4�ֽڵĳ���, ��λ������

#define FULL_IN_PAPER_TIMES_ADDRESS_DEVICE_ONE				0x18			//��ֽ������4�ֽڵĳ���, ��λ������
//#define HIGH_TEMPERATURE_TIMES_ADDRESS_DEVICE_ONE			0x1c			//�¶ȹ��ߴ�����4�ֽڵĳ���, ��λ������

//�豸2�����ݿռ�	(0x32 - 0x4e)

#define DEVICE_TWO_START_ADDR								0x32
#define TOTAL_WORKING_TIME_ADDRESS_DEVICE_TWO 				0x32  			//�ܴ�ӡʱ�䣬4�ֽڵĳ���, ��λ������
#define TOTAL_KNIFE_CUT_TIMES_ADDRESS_DEVICE_TWO			0x36			//���е�������4�ֽڵĳ��ȣ���λ������
#define TOTAL_PRINTED_LENGTH_DEVICE_TWO 					0x3a			//�ܴ�ӡ���ȣ�4�ֽڵĳ��ȣ���λ������

//��¼�豸2���쳣���͵ķ��������ĵ�ַ
#define KNIFE_ERROR_TIMES_ADDRESS_DEVICE_TWO				0x3e			//�е����������4�ֽڵĳ���, ��λ������
#define UNEXPECTED_OPENED_TIMES_ADDRESS_DEVICE_TWO			0x42			//��ͷ���򿪴�����4�ֽڵĳ���, ��λ������
#define PAPER_INSUFFICIENT_TIMES_ADDRESS_DEVICE_TWO			0x46			//ȱֽ������4�ֽڵĳ���, ��λ������

#define FULL_IN_PAPER_TIMES_ADDRESS_DEVICE_TWO				0x4a			//��ֽ������4�ֽڵĳ���, ��λ������
//#define HIGH_TEMPERATURE_TIMES_ADDRESS_DEVICE_TWO			0x4e			//�¶ȹ��ߴ�����4�ֽڵĳ���, ��λ������


#endif


