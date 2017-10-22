#ifndef ANALYZE_DATA_H
#define ANALYZE_DATA_H

/**************************************************************
*	Include File Section
**************************************************************/
#include "cc.h"
#include "more_infomation.h"
#include "data_form.h"

extern batch_info batch_info_table[];//���α�
/**************************************************************
*	Macro Define Section
**************************************************************/
//add��ʼ��4���ֽڷ�����32λ������
#define ANALYZE_DATA_4B(add, data) 	\
	do {							\
		data = 0;					\
		data = *(add);				\
		data = data << 8;			\
		data |= *((add) + 1);		\
		data = data << 8;			\
		data |= *((add) + 2);		\
		data = data << 8;			\
		data |= *((add) + 3);		\
	}while(0)

//add��ʼ��2���ֽڷ�����16λ������
#define ANALYZE_DATA_2B(add, data) 	\
	do {							\
		data = 0;					\
		data = *(add);				\
		data = data << 8;			\
		data |= *((add) + 1);		\
	}while(0)
	

/**************************************************************
*	Function Define Section
**************************************************************/
	
/**
 * @brief 	��ȡ������Ź�ϣֵ
 */
u8_t get_batch_hash(u16_t batch_number);

/* �������ڲ�ͬ����ʱ����Ҫ�������ν��н��� */
void Analyze_Data_With_Diff_Part(u8_t *src1, int len1, u8_t *src2, int len2, u32_t *data);
	
/**
 * @brief 	��ȡ���γ���
 */
u16_t get_batch_length(u16_t batch_number);
/****************************************************************************************
*@Name............: Analyze_Batch_Info_Table
*@Description.....: ���������ݱ����
*@Parameters......: batch_data		:������ݵ��ֽ���
*					batch_number	:���κ�
*@Return values...: void
*****************************************************************************************/
void Analyze_Batch_Info_Table(char *batch_data, u16_t batch_number);

void find_substr_head(char **data, char *substr, u16_t *len, u16_t sub_len);
#endif
