#include "local_conf.h"

#define ORDER_LENGTH_LAST_OFFSET	4//����ͷ�Ķ������ȵ�����ֽ�λ��
#define ORDER_HEAD_LAST_OFFSET		2//����ͷ�ı�־������ֽ�λ��
#define ORDER_NUM_HEAD_OFFSET		12//�������ͷƫ��
#define URGENT						1//�Ӽ�Ϊ1
#define READ_MAX_SIZE				(1024 * 10)
#define BUF_IS_NOT_EMPTY			0//��������Ϊ��
#define BUF_IS_EMPTY				1//������Ϊ��
#define ORDER_NUM_LENGTH			4//������ų���
#define ORDER_HEAD_LENGTH			28//�����г���������ֽڳ���

/**************************************************************
*	Global Variable Section
**************************************************************/
extern SqQueue usart_buf;				//���ڻ�����
//static char readFromBuf[READ_MAX_SIZE];	//�ӻ�������ȡ������
extern OS_EVENT *Ack_Rec_Order_Sem;		//���ؽ��նԶ���״̬Ӧ����ź���
extern OS_EVENT *Ack_Rec_Printer_Sem;	//���ؽ��նԴ�ӡ��״̬Ӧ����ź���
static u32_t last_order_number = 0;		//��һ�ν��յĶ�����
static u32_t current_order_number = 0;	//���ν��յĶ�����
extern OS_EVENT *Print_Queue_Sem;
/**************************************************************
*	Function Define Section
**************************************************************/

void USART3_Hook(u8_t ch)//���Ӵ���3��ȡ�������ݷ��뻺����
{
	extern OS_EVENT *Local_Rec_Data_Sem;
	if((usart_buf.write + 1) % usart_buf.MAX == usart_buf.read)
		return;
	
	usart_buf.base[usart_buf.write++] = ch;
	usart_buf.write = usart_buf.write % usart_buf.MAX;
	if(usart_buf.buf_empty)
		OSSemPost(Local_Rec_Data_Sem);
	usart_buf.buf_empty = BUF_IS_NOT_EMPTY;    //���岻Ϊ��
}

//��ȡ����������
#define GetBufLen(buf) \
	((buf.write) - (buf.read))
	
void ReadBytes(char *data, u16_t *len)//��ȡ���ڻ��������ж��ٶ����٣������ƶ�������ָ��
{
	u16_t i;
	u16_t buf_len = 0;

	if(!usart_buf.buf_empty){//��������Ϊ��
	
		buf_len = GetBufLen(usart_buf);//����������
		
		if(buf_len > READ_MAX_SIZE)
			buf_len = READ_MAX_SIZE;//���ֻ��������

		for(i = 0; i < buf_len; ++i)
			*data++ = usart_buf.base[usart_buf.read + i];
	}
	
	*len = buf_len;
}

s8_t ReadAssignBytes(char *data, u16_t len)//��ȡ���ڻ�������ָ������Ϊlen
{
	u16_t i;
	u16_t buf_len;
	s8_t buf_err = BUF_EMPTY;
	
	do{
		if(!usart_buf.buf_empty){//��������Ϊ��
		
			buf_len = GetBufLen(usart_buf);//����������
			
			if(buf_len < len){
				buf_err = BUF_ARG_ERR;
				OSTimeDlyHMSM(0, 0, 0, 20);//�ȴ�������
			}else{
				for(i = 0; i < len; ++i)
					*data++ = usart_buf.base[usart_buf.read + i];
				
				buf_err = BUF_OK;
			}
		}else{
			OSTimeDlyHMSM(0, 0, 0, 20);//�ȴ�������
		}
		
	}while(buf_err != BUF_OK);
	
	return BUF_OK;
}

s8_t DeleteBuf(u16_t len)//ɾ������Ϊlen�Ļ�����
{
	s8_t buf_err;
	u16_t buf_len;
	
	if(usart_buf.buf_empty)
		buf_err = BUF_ARG_ERR;
	
	buf_len = GetBufLen(usart_buf);//����������	
	if(buf_len < len)
		buf_err = BUF_ARG_ERR;
	
	if(buf_err == BUF_ARG_ERR){
		return buf_err;
	}
	
	usart_buf.read = usart_buf.read + len;//���¶�ָ��
	if(usart_buf.read == usart_buf.write){
		usart_buf.read = usart_buf.write = 0;
		usart_buf.buf_empty = BUF_IS_EMPTY;
	}
	
	DEBUG_PRINT("usart_buf.write is %lu, usart_buf.read is %lu\n", usart_buf.write, usart_buf.read);
	
	return BUF_OK;
}

static void ClearBuf(void) //��ջ�����
{
	usart_buf.read = usart_buf.write = 0;
	usart_buf.buf_empty = BUF_IS_EMPTY;
}

Status CheckRec(u8_t *data, u16_t len)//�����ձ����Ƿ���ȷ
{
	if(0 == Check_Sum((u16_t*)data, len))
		return True;
	else
		return Fail;
}

void LocalSendData(char *data, u16_t len)//�Ӵ��ڷ�������
{
	int i;
	INT8U err;
	
	extern OS_EVENT *Local_Send_Data_Mutex;		//���ط���״̬��
	
	OSMutexPend(Local_Send_Data_Mutex, 0, &err);	//���뷢��״̬��
	for(i = 0; i < len; ++i)
		USART_SendData(USART3, *(data + i));
	
	OSMutexPost(Local_Send_Data_Mutex);			//�ͷŷ���״̬��
}

/****************************************************************************************
*@Name............: SendStatusToLocal
*@Description.....: �������ݱ�������
*@Parameters......: type		:��������
*					symbol		:��־λ
*					preservation:�����ֶΣ���ӡ��״̬ʱΪ��ӡ��Ԫ��ţ�����״̬ʱ��Ϊ�������
*@Return values...: void
*****************************************************************************************/
void SendStatusToLocal(req_type type, u8_t symbol, u32_t preservation)
{
	char sent_data[SEND_DATA_SIZE] = {0};	//״̬���ĺ������Ķ��ǹ̶�20�ֽ�
	int delayTimes;
	OS_EVENT *tmpSem;
	u8_t accFlag = 0; //����Ӧ���־
	u8_t timeCounter = 0;
	u8_t reSendTimes = 0;//�ش�����
	
	
	if(type == printer_status){
		Pack_Req_Or_Status_Message(sent_data, PRINTER_STATUS, symbol, Get_Printer_ID(), 0, preservation);//��ʱ��preservation�����ذ��ӡ��Ԫ��Ż�Ϊ0
		tmpSem = Ack_Rec_Printer_Sem;
		delayTimes = 4;//��ʱ����Ϊ4
	}else if(type == order_status){
		Pack_Req_Or_Status_Message(sent_data, ORDER_STATUS, symbol, Get_Printer_ID(), Get_Order_Unix_Time(preservation), preservation);//��ʱ��preservationΪ������
		tmpSem = Ack_Rec_Order_Sem;
		delayTimes = 20;//��ʱ����Ϊ20
	}
	
	/*��ʱ�ش�״̬����*/
	do{
		LocalSendData(sent_data, SEND_DATA_SIZE);//�Ӵ��ڷ��ͱ���
		
		do{
			timeCounter++;
			if(timeCounter == delayTimes){//����delayTimes����20�Σ���ʱʱ��Ϊ20*40=800ms�����������Ҫʵ�ʲ��Բ��ܵó�
				timeCounter = 0;
				reSendTimes++;
				break;
			}
			
			OSTimeDlyHMSM(0, 0, 0, 40);//ÿ����ʱΪ40ms��������Ϊ9600��ÿ�봫��1200B������һ��20�ֽڵ�״̬������Ҫ(1 / 60)s�����ؽ��մ����40ms
			if(1 > OSSemAccept(tmpSem))
				accFlag = 1;
		}while(accFlag == 0);
		
		if(reSendTimes > 3)//�ش�3�κ����ش���
			break;
	}while(accFlag == 0);
}

void AckToLocal(u32_t orderNum)//��ӡ������Ӧ�������
{
	char sent_data[SEND_DATA_SIZE] = {0};	//״̬���ĺ������Ķ��ǹ̶�20�ֽ�
	
	Pack_Req_Or_Status_Message(sent_data, ACK_STATUS, 0, Get_Printer_ID(), ORDER_ACK_LOCAL, orderNum);
	
	LocalSendData(sent_data, SEND_DATA_SIZE);//�Ӵ��ڷ��ͱ���
}

u8_t GetTypeOfAck(char *data)//��ȡӦ������
{
#define ACK_TYPE_OFFSET 11
	return (u8_t)(*(data + ACK_TYPE_OFFSET));
}

u32_t GetAckNumber(char *data)//��ȡӦ�����
{
#define ACK_NUMBER_OFFSET 12
		return ((u32_t)(*(data + ACK_NUMBER_OFFSET)) << 24) + ((u32_t)(*(data + ACK_NUMBER_OFFSET + 1)) << 16)
			  + ((u32_t)(*(data + ACK_NUMBER_OFFSET + 2)) << 8) + (u32_t)(*(data + ACK_NUMBER_OFFSET + 3));
}

void DealAckFromLocal(char *ack)//������Ա��ص�Ӧ��
{
	static u32_t printerAckNum = 0;
	static u32_t orderAckNum = 0;
	u32_t tmpAckNum;//��ʱӦ���
	
	tmpAckNum = GetAckNumber(ack);
	if(GetTypeOfAck(ack) == PRINTER_ACK_LOCAL){
		if(printerAckNum != tmpAckNum){//�ж�Ӧ����Ƿ��ظ������ٵ�
			printerAckNum = tmpAckNum;
			OSSemPost(Ack_Rec_Printer_Sem);
		}//�ٵ�Ӧ��ֱ�Ӷ���
	}else{
		if(orderAckNum != tmpAckNum){//�ж�Ӧ����Ƿ��ظ������ٵ�
			orderAckNum = tmpAckNum;
			OSSemPost(Ack_Rec_Order_Sem);
		}//�ٵ�Ӧ��ֱ�Ӷ���
	}
}

void recOrder(char *data, u16_t len)//���ն���
{
	u16_t order_len;
	u16_t order_total_len;			//�����ܹ�����
	
	u16_t timeCount = 0;
	while(len < 20){
		len = GetBufLen(usart_buf);
		OSTimeDlyHMSM(0, 0, 0, 100);
	}
	//OSTimeDlyHMSM(0, 0, 4, 500);
	ANALYZE_DATA_2B((data + ORDER_HEAD_LAST_OFFSET), order_total_len);//��ȡ��������
	order_len = order_total_len + ORDER_HEAD_LENGTH;
	//ReadAssignBytes(readFromBuf, order_len);//���ݶ������ȶ�ȡһ���������
	//DeleteBuf(order_len);//ɾ������������������Ӧ��ʱû��ɾ���Ļ�������Ҳ�����ﱻɾ��
	
	while(order_len > GetBufLen(usart_buf) && timeCount < 40){//��ʱ���ն���20s������20s��û�н����궩������֤�����������д������������Ӧ��ʧ��
		OSTimeDlyHMSM(0, 0, 0, 500);
		timeCount++;
	}
	
	if(timeCount < 40 && True == CheckRec((u8_t *)data, order_len)){//��ⶩ���Ƿ���ȷ
		ANALYZE_DATA_4B((data + ORDER_NUM_HEAD_OFFSET), current_order_number);//��ȡ������
		
		if(current_order_number != last_order_number){//�ж϶������Ƿ��ظ�
			put_in_buf((u8_t*)data, order_len, URGENT);//д�뻺����
			ClearBuf();
			AckToLocal(current_order_number);//���۶����Ƿ��ظ�����ҪӦ������أ����յ��˶���
			last_order_number = current_order_number;
			OSSemPost(Print_Queue_Sem);
		}
		else{
			ClearBuf();
			AckToLocal(current_order_number);//���۶����Ƿ��ظ�����ҪӦ������أ����յ��˶���
		}
				
	}else{//���ʧ��
		DEBUG_PRINT("batch's checksum error\n");
		ClearBuf();
		SendStatusToLocal(order_status, ORDER_DATA_ERR, 0);//�����޷���֪�����ţ�����䶩����Ϊ0		
	}
}

Status recAck(u16_t offset)//����Ӧ��
{
	u8_t *data;
	u16_t len = REC_DATA_SIZE + offset;
	//ReadAssignBytes(readFromBuf, len);//����Ӧ�𳤶ȶ�ȡһ���������
	
	data = (u8_t*)(usart_buf.base + offset);//ƫ�Ʊ�ʾ��Ч������
	if(True == CheckRec((u8_t*)data, REC_DATA_SIZE)){//���Ӧ���Ƿ���ȷ
		if(ACK_STATUS == (u8_t)(*(data + STATUS_TYPE_OFFSET)))
			DealAckFromLocal((char*)data);//�ж�Ӧ������
		else
			return Fail;
		
		//DeleteBuf(len);//ɾ��������
		ClearBuf();
		return True;
	}else{
		return Fail;
	}
}

void local_receive(void)//���ձ��صĶ�����Ӧ��
{
	char *data;
	char readFromBuf[20];
	u16_t len;//�ӻ�������ȡ�������ݵĳ���
	u8_t os_err = 0;
	extern OS_EVENT *Local_Rec_Data_Sem;
	
	while(1){//���ڽ��ջ�������Ϊ��
		OSSemPend(Local_Rec_Data_Sem, 0, &os_err);
		DEBUG_PRINT("receive data from local\n");
		while((len = GetBufLen(usart_buf)) < 2){
			OSTimeDlyHMSM(0, 0, 0, 50);//�ȴ�50ms�Եȴ�����������
		}while(len < 2);
		

		ReadAssignBytes(readFromBuf, 20);//�Ӵ��ڻ�������ȡ����
		data = readFromBuf;
		len = GetBufLen(usart_buf);//����������
		find_substr_head(&data, "\xCF\xFC", &len, 2);//Ѱ��Ӧ��ͷ
		if(len > 0) {//��⵽��Ӧ��ͷ��
			if(True == recAck(0)){
				continue;//����Ӧ��ɹ�
			}
		}
		
		/*
		**�������Ƿ�Ϊ����ͷ
		*/
		data = (char *)usart_buf.base;
		len = GetBufLen(usart_buf);//����������
		find_substr_head(&data, "\x3e\x11", &len, 2);//Ѱ�Ҷ���ͷ
		if(len > 0) {//��⵽�Ƕ���ͷ��
			recOrder(data, len);//���ն���
		}else{//����Ҳ���������Ӧ��ͷ�����������������������һ��
			ClearBuf();
			break;
		}
	}
}
