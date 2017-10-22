
#include "queue_buf.h"
#include "print_queue.h"

ElemType Qbuf[MAXQSIZE];        //ѭ���������Ľ���
ElemType Ubuf[MAXUSIZE];        //�Ӽ��������Ľ���
ElemType UsBuf[MAXUSIZE];		//���ڻ������Ľ���
#define KB (1024)
ElemType UsWifiBuf[MAXUSIZE + KB * 1];	//wifi�������Ľ���

SqQueue queue_buf;       //ѭ��������
SqQueue urgent_buf;      //�Ӽ�������
SqQueue usart_buf;		 //���ڻ�����
SqQueue wifi_buf;		 //wifi������

#define assert(expr, str)			 	\
	do {								\
		if(!(expr)) {					\
			printf((str));				\
			while(1);					\
		}								\
	}while(0)
	
/****************************************************************************
* ��    �ƣ�void Init_Queue(void)
* ��    �ܣ���ʼ��ѭ��������������дָ��ָ����㣬��ѭ������Ϊ��
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void Init_Queue(void)
{
	INT8U err;

	assert((queue_buf.mutex = OSMutexCreate(QUEUE_BUF_MUTEX_PRIO, &err)), "CREATE queue_buf.mutex FAILED\n");
	queue_buf.base = Qbuf;
	queue_buf.read = 0;        //ָ����Ҫ��ȡ��λ��
	queue_buf.write = 0;       //ָ����Ҫд���λ��
	queue_buf.MAX = MAXQSIZE;  //������������MAX�ֶ���
	queue_buf.buf_empty = 1;   //������Ϊ��
	
	assert((urgent_buf.mutex = OSMutexCreate(URGENT_BUF_MUTEX_PRIO, &err)), "CREATE urgent_buf.mutex FAILED\n");
	urgent_buf.base = Ubuf; 
	urgent_buf.read = 0;         //ָ����Ҫ��ȡ��λ��
	urgent_buf.write =0;         //ָ����Ҫд���λ��
	urgent_buf.MAX = MAXUSIZE;   //������������MAX�ֶ���
	urgent_buf.buf_empty = 1;    //������Ϊ��
	
	usart_buf.base = UsBuf;
	usart_buf.read = 0;         //ָ����Ҫ��ȡ��λ��
	usart_buf.write =0;         //ָ����Ҫд���λ��
	usart_buf.MAX = MAXUSIZE;   //������������MAX�ֶ���
	usart_buf.buf_empty = 1;    //������Ϊ��
	
	wifi_buf.base = UsWifiBuf;
	wifi_buf.read = 0;         //ָ����Ҫ��ȡ��λ��
	wifi_buf.write =0;         //ָ����Ҫд���λ��
	wifi_buf.MAX = MAXUSIZE + KB * 1;//������������MAX�ֶ���
	wifi_buf.buf_empty = 1;    //������Ϊ��
	
}

/****************************************************************************
* ��    �ƣ�s8_t Write_Buf(SqQueue *buf,ElemType *e,u32_t len)
* ��    �ܣ��򻺳���д������
* ��ڲ�����@e : ָ���д�����ݵ�ָ�� �� @len : д��������� 
* ���ڲ�����д���״̬
* ˵    ����
* ���÷������� 
****************************************************************************/
s8_t Write_Buf(SqQueue *buf,ElemType *e,u32_t len)
{
#if OS_CRITICAL_METHOD == 3
		OS_CPU_SR cpu_sr;
#endif
	INT8U err;
	s8_t buf_err = BUF_OK;
	
	DEBUG_PRINT("-----------------------ready to pend a mutex of buf\n");
	OSMutexPend(buf->mutex,0,&err);			//���뻺����
	DEBUG_PRINT("-----------------------get a mutex of buf\n");
	
	if(len > buf->MAX  || len == 0)            //����������ȷ��
		buf_err = BUF_ARG_ERR;
	 
	//����Ƿ������λ������
	if(buf->write > buf->read){
		if(len > (buf->MAX - (buf->write - buf->read)))          
			buf_err = BUF_FULL;
	}
	else if(buf->write < buf->read){
		if(len > (buf->read - buf->write))          
			buf_err = BUF_FULL;
	}
	else if((buf->write == buf->read)&&(buf->buf_empty == 0)){
		buf_err = BUF_FULL;
	}

	if(buf_err != BUF_OK){
		OSMutexPost(buf->mutex);
		return buf_err;
	}
	
	//���´�ӡ���ж�����Ϣ��
	
	order_print_table.buf_node.common_buf_remain_capacity = order_print_table.buf_node.common_buf_remain_capacity - len;
	
	
//	printf("Revieve Buf Start \n");
	while(len-- > 0){           //�������
		buf->base[buf->write] = *e++;
//		printf("%x ", buf->base [buf->write]);
		buf->write = (buf->write+1)%buf->MAX;
	}
//	printf("Revieve Buf End \n");
	buf->buf_empty = 0;    //���岻Ϊ��

	OSMutexPost(buf->mutex);
	
	return BUF_OK;
}


/****************************************************************************
* ��    �ƣ�s8_t Read_Order_Length_Queue(u16_t *order_len)
* ��    �ܣ����ض�������
* ��ڲ�����@e : ���صĶ�������
* ���ڲ�������ȡ���������Ƿ�ɹ�
* ˵    ����
* ���÷������� 
****************************************************************************/
s8_t Read_Order_Length_Queue(SqQueue buf,u16_t *order_len)
{
	if(!buf.buf_empty)
	{
		*order_len = buf.base[(buf.read + ORDER_SIZE_OFFSET)%buf.MAX] << 8 | buf.base[(buf.read + ORDER_SIZE_OFFSET + 1)%buf.MAX] ;    //���ض������� 
		return Que_NOT_EMPTY;  
	}	
	else
		return Que_EMPTY;                   //����Ϊ��
}


/****************************************************************************
* ��    �ƣ�s8_t Check_Buf_Request_Signal(SqQueue buf)
* ��    �ܣ��жϻ�����������ȷ���Ƿ���Ҫ���������ź�
* ��ڲ�����@e : ���� BUFF_SIZE_Aachieve
								 ���� 
* ���ڲ�������ȡ���������Ƿ�ɹ�
* ˵    ����
* ���÷������� 
****************************************************************************/
s8_t Check_Buf_Request_Signal(SqQueue buf)
{
	extern struct netconn *order_netconn;	//ȫ��TCP����
	extern OS_EVENT *Batch_Rec_Sem;
	INT8U err;
	u16_t empty_buf_size = buf.MAX - (buf.write + buf.MAX -buf.read )%buf.MAX;
	if( empty_buf_size >= THRESHOLD_SIZE)
	{
		if(0 < OSSemAccept(Batch_Rec_Sem))//������ȡ���
		{

			write_connection(order_netconn, order_req, 0, 0); 
				DEBUG_PRINT("<<<<<<<<<<<<<<<<<<<<<<<<<<--------------------------SEND ORDER REQ--------------------------->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
		}
		else
			;	
	}
	
	return 0;
}


/****************************************************************************
* ��    �ƣ�s8_t Read_Order_Queue(ElemType *e)
* ��    �ܣ���ȡһ�ݶ������ݣ���ȥͷβ��Ϣ
* ��ڲ�����@e : ����ָ��÷ݶ������ݵ�ָ��
* ���ڲ�������ȡ�����Ƿ�ɹ�
* ˵    ����
* ���÷������� 
****************************************************************************/
/****************************************************************************
* ��    �ƣ�s8_t Read_Order_Queue(ElemType *e)
* ��    �ܣ���ȡһ�ݶ������ݣ���ȥͷβ��Ϣ
* ��ڲ�����@e : ����ָ��÷ݶ������ݵ�ָ��
* ���ڲ�������ȡ�����Ƿ�ɹ�
* ˵    ����
* ���÷������� 
****************************************************************************/
s8_t Read_Order_Queue(SqQueue *buf,ElemType *e)
{
	u16_t order_len = 0;
	u32_t old_read  = buf->read;	
	u8_t leftBytes;
	int i = 0;
	char * orderDataPtr = (char *) e;
	Read_Order_Length_Queue(*buf,&order_len);   //��ø÷ݶ�������
	//ȥ������ͷ��
	buf->read = (buf->read+BUF_HEAD) % buf->MAX;//�������ͷ
	//���ƻ������ݵ�����data
	DEBUG_PRINT("--------------ORDER  10 K LENGTH : %d\n", order_len );
	for(i = 0 ; i < order_len ; ++i)
	{
		*orderDataPtr = buf->base[(buf->read + i )% buf->MAX];
		orderDataPtr++;					
	}			
	orderDataPtr[order_len] = '\0';
	//��ָ�붨λ����һ�ݶ�����ʼ				
	buf->read = (buf->read + order_len + BUF_END ) % buf->MAX;	
	if(buf->read == buf->write)
	{
		buf->buf_empty = 1;   //������Ϊ��
		DEBUG_PRINT ("Buf is empty\n");
	}

			//���»�����ʣ������
	order_print_table.buf_node.common_buf_remain_capacity = order_print_table.buf_node.common_buf_remain_capacity  + order_len + BUF_HEAD + BUF_END;
	return BUF_OK;
}

//��ȡ����ͷ��
u8_t *Get_Batch_Head(SqQueue buf)
{
	if(buf.base[buf.read] == 0xaa && buf.base[(buf.read + 1) % buf.MAX] == 0x55)
		return buf.base + buf.read;
	else
		return NULL;
}

//��ȡ����ͷ��
u8_t *Get_Order_Head(SqQueue buf)
{
	if(buf.base[buf.read] == 0x3e && buf.base[(buf.read + 1) % buf.MAX] == 0x11)
		return buf.base + buf.read;
	else
		return NULL;
}

u8_t Is_Empty_Queue(SqQueue buf)
{
	return buf.buf_empty;
}


