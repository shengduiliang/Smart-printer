#include "pack_data.h"

/**************************************************************
*	Function Define Section
**************************************************************/
/****************************************************************************************
*@Name............: Pack_Req_Or_Status_Message
*@Description.....: ��װ���ݱ�
*@Parameters......: message		:���ڴ�ż����������Ϣ
*					type		:��������
*					symbol		:��־λ
*					id			:���������ذ�id(32λ),Ҳ�����������(��16λ)
*					UNIX_time	:�����������ذ巢��ʱ�䣻�ڱ���״̬Ӧ��ʱ��Ϊ���ִ�ӡ���򶩵�Ӧ��
*					preservation:�����ֶΣ��ڶ����У���16λΪ������ţ���16λΪ��������ţ��ڱ���״̬Ӧ��ʱΪӦ�����
*@Return values...: void
*****************************************************************************************/
void Pack_Req_Or_Status_Message(char *message, req_type type, u8_t symbol, u32_t id, u32_t UNIX_time, u32_t preservation)
{
	u16_t check_sum;
	
	/*��ʼ��*/
	message[0] = '\xCF';
	message[1] = '\xFC';
	
	/*�������ͺͱ�־*/
	message[2] = type;
	message[3] = symbol;
	SET_DATA_4B(&message[4], id);//����id

	SET_DATA_4B(&message[8], UNIX_time);//����Unixʱ���
	
	SET_DATA_4B(&message[12], preservation);//����
	
	/*��ֹ��*/
	message[18] = '\xFC';
	message[19] = '\xCF';
	
	/*��ȡУ���*/
	check_sum = Check_Sum((u16_t*)message, SEND_DATA_SIZE);
	SET_DATA_2B(&message[16], ((check_sum << 8) + (check_sum >> 8)));
}