#include "wifi_conf.h"



#define ORDER_LENGTH_LAST_OFFSET	4//����ͷ�Ķ������ȵ�����ֽ�λ��
#define ORDER_HEAD_LAST_OFFSET		2//����ͷ�ı�־������ֽ�λ��
#define ORDER_NUM_HEAD_OFFSET		12//�������ͷƫ��
#define URGENT						1//�Ӽ�Ϊ1
#define READ_MAX_SIZE				(1024 * 10)
#define BUF_IS_NOT_EMPTY			0//��������Ϊ��
#define BUF_IS_EMPTY				1//������Ϊ��
#define ORDER_NUM_LENGTH			4//������ų���
#define ORDER_CREATE_TIME			4//��������ʱ��
#define ORDER_REQ_LENGTH			12//���󶩵�����
#define IPADD_OFFSET				8//ip��ַƫ��
#define ORDER_HEAD_LENGTH			28//�����г���������ֽڳ���
#define ACK_LENGTH					20//Ӧ�𳤶�





/**************************************************************
*	Global Variable Section
**************************************************************/
extern SqQueue wifi_buf;				//���ڻ�����
static u32_t last_order_number = 0;		//��һ�ν��յĶ�����
static u32_t current_order_number = 0;	//���ν��յĶ�����
extern OS_EVENT *Print_Queue_Sem;
/**************************************************************
*	Function Define Section
**************************************************************/



static void delay_time(void)
{
	int i=65535, t;
	while(i != 0)
	{
			for(i = 30000; i > 0 ;i--)
				{
						for(t = 535; t > 0 ;t--);
				}
	}
}


extern uint8_t data1[20] ;
char * usart6_buf(void)
{
	
		printf("%s",data1);

}

void wifi_init(void)

{


	
	u8_t data_1[8] = "AT+RST\r\n";//��ʼ��wifiģ��
	u8_t data_2[13] = "AT+CWMODE=3\r\n";//ѡ��ģʽΪSTA+AP
	u8_t data_3[36] = "AT+CWSAP=\"ESP8\",\"0123456789\",1,3\r\n";//��ʼ���ȵ��˺�����
	u8_t data_4[13] = "AT+CIPMUX=1\r\n";//�򿪶�����
	u8_t data_5[21] = "AT+CIPSERVER=1,8899\r\n";//���ö˿ں�
	u8_t data_6[17] = "AT+CIPSEND=0,10\r\n"   ;//���ʹ�ָ��󣬲��ܷ������ݹ�ȥ
	u8_t data_7[12] = "0123456789\r\n";
	u16_t len_1 = 8 ;
	u16_t len_2 = 13 ;
	u16_t len_3 = 36 ;
	u16_t len_4 = 13 ;
	u16_t len_5 = 21 ;
	u16_t len_6 = 17 ;
	u16_t len_7 = 12 ;
	u16_t len_8 = 20 ;

//		
//	WifiSendData(data_1,len_1);
//	delay_time();

	
	
	
	
	WifiSendData(data_2,len_2);
	delay_time();
	
	

//	WifiSendData(data_3,len_3);
//	delay_time();

	
	WifiSendData(data_4,len_4);
	delay_time();
	


	WifiSendData(data_5,len_5);
	delay_time();


//	while(1)
//	{
//		for(time = 0;time < 20;time++)
//		{
//			printf("%c",data1[time]);
//		}
//	}

}








void UART6_Hook(u8_t ch)//���Ӵ���6��ȡ�������ݷ��뻺����
{
	extern OS_EVENT *Wifi_Rec_Data_Sem;
	
	if((wifi_buf.write + 1) % wifi_buf.MAX == wifi_buf.read)
		return;
	
	wifi_buf.base[wifi_buf.write++] = ch;
	
	wifi_buf.write = wifi_buf.write % wifi_buf.MAX;
	if(wifi_buf.buf_empty == BUF_IS_EMPTY)
		OSSemPost(Wifi_Rec_Data_Sem);
	
	wifi_buf.buf_empty = BUF_IS_NOT_EMPTY;    //���岻Ϊ��
}

//��ȡ����������
#define GetBufLen(buf) \
	(((buf.write) + (buf.MAX)- (buf.read)) % buf.MAX)

void WifiReadBytes(u8_t *data, u16_t *len)//��ȡ���ڻ��������ж��ٶ����٣������ƶ�������ָ��
{
	u16_t i;
	u16_t buf_len = 0;

	if(!wifi_buf.buf_empty){//��������Ϊ��
	
		buf_len = GetBufLen(wifi_buf);//����������
		
		if(buf_len > READ_MAX_SIZE)
			buf_len = READ_MAX_SIZE;//���ֻ��������

		for(i = 0; i < buf_len; ++i)
		{
			*data++ = wifi_buf.base[(wifi_buf.read + i) % wifi_buf.MAX];
			
			}
	}
	*len = buf_len;
}

s8_t WifiReadAssignBytes(u8_t *data, u16_t len)//��ȡ���ڻ�������ָ������Ϊlen
{
	u16_t i;
	u16_t buf_len;
	s8_t buf_err = BUF_EMPTY;
	u16_t timeCount = 0;
	
	do{
		if(!wifi_buf.buf_empty){//��������Ϊ��
		
			buf_len = GetBufLen(wifi_buf);//����������
			
			if(buf_len < len){
				buf_err = BUF_ARG_ERR;
				if(++timeCount >= 5)
					return buf_err;
				DEBUG_PRINT("Wait for data from wifi\n");
				OSTimeDlyHMSM(0, 0, 0, 100);//�ȴ�������
				DEBUG_PRINT("Continue to read data from wifi\n");
			}
			else{
				for(i = 0; i < len; ++i)
					*data++ = wifi_buf.base[(wifi_buf.read + i) % wifi_buf.MAX];
				DEBUG_PRINT("Assign read data from wifi ok\n");
				buf_err = BUF_OK;
			}
		}
		else{
			DEBUG_PRINT("Wait for data from wifi\n");
			OSTimeDlyHMSM(0, 0, 0, 100);//�ȴ�������
			DEBUG_PRINT("Continue to read data from wifi\n");
		}
		
	}while(buf_err != BUF_OK);
	
	return BUF_OK;
}

static void ClearBuf(void)
{
	wifi_buf.read = wifi_buf.write = 0;
	wifi_buf.buf_empty = BUF_IS_EMPTY;
}

s8_t WifiDeleteBuf(u16_t len)//ɾ������Ϊlen�Ļ�����
{
	s8_t buf_err;
	u16_t buf_len;
	
	if(wifi_buf.buf_empty)
		buf_err = BUF_ARG_ERR;
	
	buf_len = GetBufLen(wifi_buf);//����������	
	if(buf_len < len)
		buf_err = BUF_ARG_ERR;
	
	if(buf_err == BUF_ARG_ERR){
		ClearBuf();
		return buf_err;
	}
	
	wifi_buf.read = (wifi_buf.read + len) % wifi_buf.MAX;
	if(wifi_buf.read == wifi_buf.write){
		wifi_buf.buf_empty = BUF_IS_EMPTY;
	}
	DEBUG_PRINT("wifi_buf.write is %lu, wifi_buf.read is %lu\n", wifi_buf.write, wifi_buf.read);
	return BUF_OK;
}

void WifiSendData(u8_t *data, u16_t len)//�Ӵ��ڷ�������
{
	int i;
	INT8U err;
	
	extern OS_EVENT *Wifi_Send_Data_Mutex;		//wifi����״̬��
	
	OSMutexPend(Wifi_Send_Data_Mutex, 0, &err);	//���뷢��״̬��
	for(i = 0; i < len; ++i)
		USART_SendData(USART6, *(data + i));	//�ݶ��Ӵ���6���
	
	OSMutexPost(Wifi_Send_Data_Mutex);			//�ͷŷ���״̬��
}

void SendStatusToWifi(req_type type, u8_t symbol, u32_t IPadd, u32_t preservation)
{
	u8_t sent_data[SEND_DATA_SIZE] = {0};	//״̬���ĺ������Ķ��ǹ̶�20�ֽ�
	u8_t data_6[17] = "AT+CIPSEND=0,20\r\n"   ;
	
	
	if(type == printer_status){
		Pack_Req_Or_Status_Message((char *)sent_data, PRINTER_STATUS, symbol, Get_Printer_ID(), 0, preservation);//��ʱ��preservation�����ذ��ӡ��Ԫ��Ż�Ϊ0
	}else if(type == order_status){
		Pack_Req_Or_Status_Message((char *)sent_data, ORDER_STATUS, symbol, Get_Printer_ID(), IPadd, preservation);//��ʱ��preservationΪ������
	}else if(type == wifi_order_req){
		Pack_Req_Or_Status_Message((char *)sent_data, WIFI_REQ_ACK, symbol, Get_Printer_ID(), IPadd, preservation);//��ʱ��preservationΪ����

	}
	
	WifiSendData(data_6, 17);
	OSTimeDlyHMSM(0, 0, 0, 13);
	WifiSendData(sent_data, SEND_DATA_SIZE);//�Ӵ��ڷ��ͱ���PRINTER_STATUS��ӡ��״̬����ORDER_STATUS����״̬����WIFI_REQ_ACK�����ɽ���Ӧ��
	if(sent_data[2] == 0xe0)
	{
		printf("111111111\n");
	}
}

void AckToWifi(u32_t orderNum, u32_t ipAdd)//��ӡ������Ӧ�������
{
	u8_t sent_data[SEND_DATA_SIZE] = {0};	//״̬���ĺ������Ķ��ǹ̶�20�ֽ�
	
	u8_t data_6[17] = "AT+CIPSEND=0,20\r\n";
	
	Pack_Req_Or_Status_Message((char *)sent_data, ACK_STATUS, 0, Get_Printer_ID(), ipAdd, orderNum);//����״̬Ӧ��101
		
	
	WifiSendData(data_6, 17);
	OSTimeDlyHMSM(0, 0, 0, 13);
	WifiSendData(sent_data, SEND_DATA_SIZE);//�Ӵ��ڷ��ͱ���
	if(sent_data[2] == 0xa0)
	{
		printf("success\n");
	}
}

void AnalyzeData(char *data, u8_t offset, u8_t srcLen, u16_t *dst)
{
	if((wifi_buf.read + offset) <= wifi_buf.MAX){
		if((wifi_buf.read + offset + srcLen) <= wifi_buf.MAX){ //����������δ��������
			if(srcLen == 4)
				ANALYZE_DATA_4B(data + offset, (*dst));
			else if(srcLen == 2)
				ANALYZE_DATA_2B(data + offset, (*dst));
		}else{//���������ݿ���������
			u8_t FirstLen = wifi_buf.MAX - (wifi_buf.read + offset);
			u8_t SecondLen = srcLen - FirstLen;
			Analyze_Data_With_Diff_Part((u8_t*)data, FirstLen, wifi_buf.base, SecondLen, (u32_t*)dst);
		}
	}else if((wifi_buf.read + offset + srcLen) > wifi_buf.MAX){ //��������������һ�˵����
		offset = offset - (wifi_buf.MAX - wifi_buf.read);
		
		if(srcLen == 4)
			ANALYZE_DATA_4B(data + offset, (*dst));
		else if(srcLen == 2)
			ANALYZE_DATA_2B(data + offset, (*dst));
	}
}

Status WifiCheckRec(u8_t *data, u16_t len)//�����ձ����Ƿ���ȷ
{
	u16_t sum;
	u16_t subLen = wifi_buf.MAX - wifi_buf.read;
	if(subLen > len){
		sum = Check_Sum((u16_t*)data, len);
	}else{
		sum = Check_Sum_With_Diff_Part((u16_t*)data, subLen, (u16_t*)wifi_buf.base, len - subLen);
	}
	
	if(0 == sum)
		return True;
	else
		return Fail;
}

/* ��wifi����������д��Ӽ������� */
void putbuf(u8_t *data, u16_t len)
{
	u16_t subLen = wifi_buf.MAX - wifi_buf.read;
	if(subLen > len){
		DEBUG_PRINT("put in buf in one part\n");
		put_in_buf(data, len, URGENT);
	}else{
		/* �������ȼ���ߣ���������䲻�ᱻ�жϣ������Ǵ��ڷ��� */
		DEBUG_PRINT("put in buf in two part\n");
		put_in_buf(data, subLen, URGENT);
		put_in_buf(wifi_buf.base, len - subLen, URGENT);
	} 
}

#define KB (1024)
void ChangeCpa(Heap *heap, u16_t len)
{
	INT8U err;
	extern OS_EVENT *Capacity_Change_Sem;	//wifi�����������ı��ź�
	
	OSMutexPend(heap->mutex, 0, &err);	//����ѿ�����
	{
		heap->capcity += len;
		DEBUG_PRINT("-------------Delete an order, heap capcity is %lu---------\n", heap->capcity);
		OSSemPost(Capacity_Change_Sem);
	}
	OSMutexPost(heap->mutex);	//�ͷŶѿ�����
}

void WifiRecOrder(u8_t *data)//���ն���
{
	u16_t order_len;
	u16_t order_total_len;			//�����ܹ�����
	u32_t ipAdd;
	u16_t timeCount = 0;
	u8_t readFromBuf[ORDER_HEAD_LENGTH];	//����ͷ������
	extern Heap heap;
	
	WifiReadAssignBytes(readFromBuf, ORDER_HEAD_LENGTH);//��ȡ���ڻ�������ָ������ΪORDER_HEAD_LENGTH
	
	ANALYZE_DATA_2B((readFromBuf + ORDER_HEAD_LAST_OFFSET), order_total_len);//��ȡ��������
	order_len = order_total_len + ORDER_HEAD_LENGTH;
//	WifiReadAssignBytes(readFromBuf, order_len);//���ݶ������ȶ�ȡһ���������
//	WifiDeleteBuf(order_len);//ɾ������������������Ӧ��ʱû��ɾ���Ļ�������Ҳ�����ﱻɾ��
	
	while(order_len > GetBufLen(wifi_buf) && timeCount < 20){//��ʱ���ն���10s������10s��û�н����궩������֤�����������д������������Ӧ��ʧ��
		timeCount++;
		DEBUG_PRINT("Wait for data from wifi, timeCount is %u\n", timeCount);
		OSTimeDlyHMSM(0, 0, 0, 500);//�ȴ�������
		DEBUG_PRINT("Continue to read data from wifi\n");
	}
		
	ANALYZE_DATA_4B((readFromBuf + IPADD_OFFSET), ipAdd);//��ȡip��ַ
	if(timeCount < 20 && True == WifiCheckRec((u8_t *)data, order_len)){//��ⶩ���Ƿ���ȷ
		ANALYZE_DATA_4B((readFromBuf + ORDER_NUM_HEAD_OFFSET), current_order_number);//��ȡ������
		
//		if(current_order_number != last_order_number){//�ж϶������Ƿ��ظ�
			putbuf(data, order_len);//д�뻺����
			AckToWifi(current_order_number, ipAdd);//���۶����Ƿ��ظ�����ҪӦ������أ����յ��˶���
			printf("order number %ld \n",current_order_number);
			DEBUG_PRINT("receive a new order\n");
			last_order_number = current_order_number;
			OSSemPost(Print_Queue_Sem);
//		}else{
//			AckToWifi(current_order_number, ipAdd);//���۶����Ƿ��ظ�����ҪӦ������أ����յ��˶���
//		}
	}else{//���ʧ��
		DEBUG_PRINT("order's checksum error\n");
		SendStatusToWifi(order_status, ORDER_DATA_ERR, ipAdd, 0);//�����޷���֪�����ţ�����䶩����Ϊ0
	}
	
	WifiDeleteBuf(order_len);
	ChangeCpa(&heap, order_len);//֪ͨ�����������ı�
}

Status recReq(u8_t *data)//����Ӧ��
{
	if(True == CheckRec(data, ACK_LENGTH)){
		if(WIFI_ORDER_REQ == (u8_t)(*(data + STATUS_TYPE_OFFSET))){
			INT8U err;
			extern Heap heap;
			extern OS_EVENT *Rec_Wifi_Req_Sem;		//��ȡ��wifi���������ź�
			Request req;
			
			/* �������󶩵����� */
			ANALYZE_DATA_4B((data + ORDER_CREATE_TIME), req.recTime);//��ȡ��������ʱ��
			ANALYZE_DATA_4B((data + IPADD_OFFSET), req.ipAdd);//��ȡip��ַ
			ANALYZE_DATA_4B((data + ORDER_REQ_LENGTH), req.orderLen);//��ȡ��������
			
			if(req.orderLen > 10 * KB){
				DEBUG_PRINT("Wifi Req Order is more than 10 KB.\n");
				return Fail;
			}
			
			DEBUG_PRINT("-------wifi pend heap mutex!-------\n");
			OSMutexPend(heap.mutex, 0, &err);	//����ѿ�����
			InsertHeap(&heap, &req);	//�����
			DEBUG_PRINT("-------receive a wifi order request!------\n");
			OSMutexPost(heap.mutex);	//�ͷŶѿ�����
			DEBUG_PRINT("-------wifi post heap mutex!-------\n");
			OSSemPost(Rec_Wifi_Req_Sem);	//֪ͨ�Ѿ��յ���������
			WifiDeleteBuf(ACK_LENGTH);
		}else{
			DEBUG_PRINT("-------fail to receive a wifi order request!-------\n");
			return Fail;
		}
	}
}

void wifi_receive(void)//���ձ��صĶ�����Ӧ��
{
	u8_t *data;
	u16_t len;//�ӻ�������ȡ�������ݵĳ���
	u16_t subLen;
	u8_t os_err = 0;
	extern OS_EVENT *Wifi_Rec_Data_Sem;
	u8_t readFromBuf[ORDER_HEAD_LENGTH];
	
	while(1){//���ڽ��ջ�������Ϊ��
		OSSemPend(Wifi_Rec_Data_Sem, 0, &os_err);
		DEBUG_PRINT("receive data from wifi\n");
		while(!wifi_buf.buf_empty){
			while((len = GetBufLen(wifi_buf)) < 20){
				OSTimeDlyHMSM(0, 0, 0, 50);//�ȴ�50ms�Եȴ�����������
				DEBUG_PRINT("Wait for data for 50ms from wifi\n");
			}
			
			WifiReadAssignBytes(readFromBuf, ACK_LENGTH);//�Ӵ��ڻ�������ȡ����
			data = readFromBuf;//20�ֽڵ�����
			len = ACK_LENGTH;
			find_substr_head((char **)(&data), "\xCF\xFC", &len, 2);//Ѱ�Ҷ�������ͷ
			
			if(len > 0) {//��⵽�Ƕ�������ͷ
				DEBUG_PRINT("detect req from wifi\n");
				if(len < ACK_LENGTH) {
					WifiDeleteBuf(ACK_LENGTH - len);
					WifiReadAssignBytes(readFromBuf, ACK_LENGTH);//�Ӵ��ڻ�������ȡ����
				}
				if(True == recReq(readFromBuf)){
					continue;//���ն�������ɹ�
				}
			}
			
			data = wifi_buf.base + wifi_buf.read;
			len = GetBufLen(wifi_buf);
			DEBUG_PRINT("Before detect orderhead ,len is %u\n", len);
			subLen = len = (len > ORDER_HEAD_LENGTH) ? ORDER_HEAD_LENGTH : len;
			find_substr_head((char **)(&data), "\x3e\x11", &len, 2);//Ѱ�Ҷ���ͷ
			DEBUG_PRINT("After detect orderhead ,sublen is %u,len is %u\n",subLen, len);
			if(len > 0) {//��⵽�Ƕ���ͷ��
				DEBUG_PRINT("Detect order from wifi\n");
				WifiDeleteBuf(subLen - len);
				WifiRecOrder(wifi_buf.base + wifi_buf.read);//���ն���
			}else{//����Ҳ���������Ӧ��ͷ�����������������������һ��
				WifiDeleteBuf(subLen);
			}
		}
	}
}
