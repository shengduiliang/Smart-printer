#include "netconf.h"

#define ORDER_NUM_MAX 10

struct netif DM9161_netif;
struct netconn *order_netconn;	//ȫ��TCP����

extern OS_EVENT *Print_Sem;
extern batch_info batch_info_table[];	//���α�
extern OS_EVENT *Batch_Rec_Sem;			//���һ�����ζ�ȡ�Ķ�ֵ�ź���
//#define TCPSEVER
#define TCPCLIENT
#define REMOTE



/****************************************************************************************
*@Name............: put_in_buf
*@Description.....: �����ݴ洢�����ػ�����
*@Parameters......: data		:�洢���ݵ�ָ��
*					len			:���ݳ���
*					urg			:�Ӽ���־
*@Return values...: void
*****************************************************************************************/
void put_in_buf(u8_t *data, u16_t len, u16_t urg)
{
	SqQueue *buf;
	u8_t err;
	s8_t buf_err;
	extern OS_EVENT *Print_Queue_Sem;
	
	if(urg)
		buf = &urgent_buf;
	else
		buf = &queue_buf;
	
	while(1){
		if(BUF_OK != (buf_err = Write_Buf(buf, data, len))){  //���ƶ������ݵ�������
			DEBUG_PRINT("buf %d write error, err is %d, len is %d.\n", urg, buf_err, len);
			OSSemPost(Print_Queue_Sem);//�ͷ�
			OSTimeDlyHMSM(0, 0, 1, 0);
		}else{
			return;
		}
	}
}

static u16_t batch_head_current_len = 0;//��ǰ��ȡ�������εĳ���
static u16_t batch_total_len = 0;//�����ܹ�����
static u16_t batch_number = 0;	//���κ�
static u16_t last_batch_number = 0;
static char batch[21] = {0};
static u16_t len = 0;	//�����绺������ȡ�������ݵĳ���
static u8_t flag = 0;	//1��ʾ�Ѿ���ȡ������ͷ������û��
static u16_t count = 0;
static u16_t leave_len = 0;

/**
 * @brief	�����µ�����ͷ�ĳ�ʼ��
 */
static void begin_new_batch(void)
{
	batch[0] = '\0';
	batch_head_current_len = 0;//�µ�����
	batch_total_len = 0;
	flag = 0;
	count = 0;
	last_batch_number = batch_number;
}

/**
 * @brief	���ձ��Ĳ�����
 */
void receive_connection(struct netconn *conn)
{
	struct netbuf *order_netbuf = NULL;
	extern OS_EVENT *Print_Queue_Sem;
	u8_t hash;//���ι�ϣֵ
	err_t err;

	extern OS_EVENT *Recon_To_Server_Sem;
	
	while((err = netconn_recv(conn, &order_netbuf)) == ERR_OK)
	{		
		char *data;
start:
		while(batch_head_current_len < MAX_BATCH_HEAD_LENGTH)
		{
			netbuf_data(order_netbuf, (void **)&data, &len);//�����绺������ȡ
			/*������һ����������ȡ��һ�����κ�ʣ��һЩ���ݣ�����Ҫ���¶�ȡ����������������ϱ���ȡ�����ƫ������Ŀ������dataָ��ָ��δ��������*/
			data += leave_len;
			len -= leave_len;
			DEBUG_PRINT("get data ,len is %d\n", len);
			
			while(len > 0 && batch_head_current_len < MAX_BATCH_HEAD_LENGTH)
			{
				DEBUG_PRINT("len haa is %d\n", len);
				if(batch[0] != '\xaa')//һֱ��Ѱ��������ͷ������һֱѰ��
				{
					find_substr_head(&data, "\xaa\x55", &len, 2);
					DEBUG_PRINT("len xaa is %d\n", len);
					if(len == 0)
					{
						DEBUG_PRINT("There is no data\n");
					}
				}
				
				if(len > 0)//len����0�����⵽����ͷ����dataָ�뻹ָ��Ļ���������ʣ������
				{
					batch[batch_head_current_len++] = *data++;
					count++;
					DEBUG_PRINT("batch[0] is %x, *data - 1 is %x, *data is %x, *data + 1 is %x, len is %d, batch_head_current_len is %d\n", batch[0], *(data - 1), *data, *(data + 1), len, batch_head_current_len);
					
					if(batch_head_current_len == MAX_BATCH_HEAD_LENGTH)
					{
						if(*(batch + BATCH_TAIL_OFFSET) == '\x55' && *(batch + BATCH_TAIL_OFFSET + 1) == '\xaa')//��ʾ������β
						{
							flag = 1;
							ANALYZE_DATA_2B((batch + BATCH_NUMBER_OFFSET), batch_number);//��ȡ���κ�
							
							DEBUG_PRINT("batch read success ,batch_number is %x %x\n", *(batch + BATCH_NUMBER_OFFSET), *(batch + BATCH_NUMBER_OFFSET + 1));
							DEBUG_PRINT("batch read success ,batch_length is %x %x\n", *(batch + BATCH_TOTAL_LENGTH_OFFSET), *(batch + BATCH_TOTAL_LENGTH_OFFSET + 1));
//							printf("batch read success ,batch_number is %d, ascii is %x %x \n", batch_number, *(batch + BATCH_NUMBER_OFFSET), *(batch + BATCH_NUMBER_OFFSET + 1));
							
							Analyze_Batch_Info_Table(batch, batch_number);//���ν��
							
							hash = get_batch_hash(batch_number);
//							printf("batch read success ,batch_length is %d ascii is %x %x\n", batch_info_table[hash].batch_length, *(batch + BATCH_TOTAL_LENGTH_OFFSET),  *(batch + BATCH_TOTAL_LENGTH_OFFSET + 1));
							len -= count;
//							DEBUG_PRINT("read from netconf1\n");

							batch_total_len = MAX_BATCH_HEAD_LENGTH + len;
							if (batch_total_len >= batch_info_table[hash].batch_length)
							{
								u16_t sub_len;
								sub_len =  len - (batch_total_len - batch_info_table[hash].batch_length);//�����绺������ȡ������һ�����ε����ݣ���ֻ����ǰ�������
								
								if(batch_number != last_batch_number){
									SqQueue * tempBuf = 0;
									u32_t oldWritePtr = 0;
									
									if(batch_info_table[hash].preservation){
										tempBuf = &urgent_buf;										
									}
									else{
										tempBuf = &queue_buf;
									}	
									if(sub_len)
										put_in_buf(data, sub_len, batch_info_table[hash].preservation);//���涩��������������
									
									oldWritePtr = ((tempBuf->write + tempBuf->MAX - (batch_info_table[hash].batch_length - 20) )%tempBuf->MAX );
	
									if(checkBufData(tempBuf,oldWritePtr) == 1){	//�������ݴ���											
										INT8U err = 0;
										OSMutexPend(tempBuf->mutex,0,&err);			//������ͨ������
										tempBuf->write = oldWritePtr;
										OSMutexPost(tempBuf->mutex);			//������ͨ������
									}										
									else{//����������ȷ																			
										OSSemPost(Print_Queue_Sem);
										OSSemPost(Batch_Rec_Sem); 
										NON_BASE_SEND_STATUS(batch_status, BATCH_ENTER_BUF, batch_number);//��������״̬�����뻺����
									}
								}
								
								begin_new_batch();
								//DEBUG_PRINT("batch read enough1!\n");
//								printf("batch read success ,batch_number is %d\n", batch_number);
								//DEBUG_PRINT("PRINT_QUEUE_SEM\n");
								leave_len = len - sub_len;					
								if(leave_len > 0)
									goto start;
								else
									break;
								
							}
							else
							{
								DEBUG_PRINT("Continue put in buf 1\n");
								if(batch_number != last_batch_number)
									put_in_buf(data, len, batch_info_table[hash].preservation);//���涩��������������
								DEBUG_PRINT("Continue put in buf 2\n");
							}
							DEBUG_PRINT("batch read success ,len is %d,count is %d, btl is %d\n", len, count, batch_total_len);
						}
						else
						{
							DEBUG_PRINT("Detect error\n");
							begin_new_batch();
						}
					}
					if(count == len)//���ݶ�ȡ�꣬��������
					{
						count = 0;
						len = 0;
						break;
					}
				}
			}
			if(!(netbuf_next(order_netbuf) > 0))
			{
				goto delete;
			}
		}
		
		DEBUG_PRINT("batch_head_current_len is %d\n", batch_head_current_len);
		do
		{
			if(flag == 0)
				goto start;
			
			DEBUG_PRINT("batch head read finished, bhcl is %d, btl is %d, batch_number is %d, batlen is %d\n", batch_head_current_len, batch_total_len, batch_number, get_batch_length(batch_number));
			netbuf_data(order_netbuf, (void **)&data, &len);
			DEBUG_PRINT("we are in new orderbuf, btl is %d, len is %d\n", batch_total_len, len);

			//DEBUG_PRINT("%s, %d\n", data, batch_total_len);
			
			batch_total_len += len;
			
			if (batch_total_len >= batch_info_table[hash].batch_length)
			{
				u16_t sub_len;
				sub_len =  len - (batch_total_len - batch_info_table[hash].batch_length);//�����绺������ȡ������һ�����ε����ݣ���ֻ����ǰ�������
				
				if(batch_number != last_batch_number){					
					SqQueue * tempBuf = 0;
					u32_t oldWritePtr = 0;
					if(batch_info_table[hash].preservation){
						tempBuf = &urgent_buf;										
					}
					else{
						tempBuf = &queue_buf;
					}		
					if(sub_len)
						put_in_buf(data, sub_len, batch_info_table[hash].preservation);//���涩��������������
					oldWritePtr = ((tempBuf->write + tempBuf->MAX - (batch_info_table[hash].batch_length - 20) )%tempBuf->MAX );
			
					if(checkBufData(tempBuf , oldWritePtr) == 1){	//�������ݴ���											
						INT8U err = 0;
						OSMutexPend(tempBuf->mutex,0,&err);			//������ͨ������
						tempBuf->write = oldWritePtr;
						OSMutexPost(tempBuf->mutex);			//������ͨ������
					}										
					else{//����������ȷ																			
						OSSemPost(Print_Queue_Sem);
						OSSemPost(Batch_Rec_Sem);						
						NON_BASE_SEND_STATUS(batch_status, BATCH_ENTER_BUF, batch_number);//��������״̬�����뻺����
					}		

				}
				
				begin_new_batch();
//				printf("batch read success ,batch_number is %d\n", batch_number);
				//DEBUG_PRINT("PRINT_QUEUE_SEM\n");
				leave_len = len - sub_len;															
				if(leave_len > 0)//����ʣ��������������ؼ���Ƿ���һ���µ�����
					goto start;
				
			}
			else
			{
				if(batch_number != last_batch_number)
					put_in_buf(data, len, batch_info_table[hash].preservation);//������ͨ����������������
			}
		}while(netbuf_next(order_netbuf) > 0);
delete:
		netbuf_delete(order_netbuf);
		DEBUG_PRINT("go to delete\n");
	}
	OSTimeDlyHMSM(0, 0, 0, 50);
}

/****************************************************************************************
*@Name............: write_connection
*@Description.....: �������ݱ�
*@Parameters......: conn		:����
*					type		:��������
*					symbol		:��־λ
*					preservation:�����ֶΣ������У���16λΪ������ţ�������ʱ�ҵ�16λΪ���������
*@Return values...: void
*****************************************************************************************/
void write_connection(struct netconn *conn, req_type type, u8_t symbol, u32_t preservation)
{
	char sent_data[SEND_DATA_SIZE] = {0};	//״̬���ĺ������Ķ��ǹ̶�20�ֽ�
	err_t err;
	int i = 0;
#ifdef REMOTE
	//�ȴ���������Ǳ���
	if(type == order_req){
		Pack_Req_Or_Status_Message(sent_data, ORDER_REQ, symbol, Get_Printer_ID(), 
									Get_Current_Unix_Time(), preservation);
	}
	else if(type == batch_status){//��ʱ��preservation��16λΪ���κ�
		Pack_Req_Or_Status_Message(sent_data, BATCH_STATUS, symbol, Get_Printer_ID(), 
									Get_Batch_Unix_Time((u16_t)preservation), preservation << 16);
	}
	else if(type == printer_status){//��ʱ��preservation�����ذ��ӡ��Ԫ��Ż�Ϊ0
		Pack_Req_Or_Status_Message(sent_data, PRINTER_STATUS, symbol, Get_Printer_ID(), 
									Get_Current_Unix_Time(), preservation);
	}
	else if(type == order_status){//��ʱ��preservation�ĸ�16λΪ���κţ���16λΪ���������
		Pack_Req_Or_Status_Message(sent_data, ORDER_STATUS, symbol, Get_Printer_ID(), 
									Get_Batch_Unix_Time((u16_t)(preservation >> 16)), preservation);
	}
	else if(type == first_req){//�����һ�ν������ӣ��������ذ�id
		Pack_Req_Or_Status_Message(sent_data, FIRST_REQ, symbol, Get_Printer_ID(), 
									Get_Current_Unix_Time(), preservation);	
	}

#endif
	
	while(0 != (err = netconn_write(conn, sent_data, SEND_DATA_SIZE, NETCONN_COPY))){
		if(ERR_IS_FATAL(err))//�������󣬱�ʾû������
			break;
		
		//������д�����ʱ����Ҫ�ȴ�һ��ʱ������д������ݰ��������޷�������������
		OSTimeDlyHMSM(0,0,++i,0);
		DEBUG_PRINT("\n\n\nNETCONN WRITE ERR_T IS %d\n\n\n", err);
		
		if(type != order_req){//����������������ͣ�������������޷��´ﶩ��
			if(i > 3) break;
		}else if(i > 10) break;//���ȴ���κ����������
	}
		
}

/**
 * @brief	������Զ�̷�����
 */
void con_to_server(void)
{
	struct ip_addr server_ip;
	extern struct netconn *order_netconn;	//ȫ��TCP����
	
	if((order_netconn = netconn_new(NETCONN_TCP)) != NULL){
		DEBUG_PRINT("Connection build.\n");
	}else{
		DEBUG_PRINT("Fail to build connection.\n");
	}
	
	netconn_set_recvtimeout(order_netconn,10000);//���ý�����ʱʱ�� 
	
#ifdef REMOTE//���÷�����ip��ַ
	IP4_ADDR(&server_ip,192,168,4,1);//�Ʒ�����192,168,1,116
//	IP4_ADDR(&server_ip,10,21,56,143);
//	IP4_ADDR(&server_ip,192,168,1,110);  //�÷ʳ���Ե�IP
	netconn_connect(order_netconn,&server_ip,8899);//8086
#else	
	IP4_ADDR(&server_ip,192,168,4,1);//�Ʒ�����192,168,1,116
	netconn_connect(order_netconn,&server_ip,8899);//8086
#endif	
	
	write_connection(order_netconn, first_req, REQ_LINK_OK, 0);//����������
	OSTimeDlyHMSM(0,0,1,0);
	write_connection(order_netconn, order_req, ORDER_REQUEST, 0);//���󶩵�
	DEBUG_PRINT("Order req.\n");
	
	//����β���
#ifdef APP_DEBUG
	//write_connection(order_netconn, first_req, REQ_LINK_OK, 0);//����������
	//write_connection(order_netconn, first_req, REQ_LINK_OK, 0);//����������
#endif
}


/**
  * @brief  Initializes the lwIP stack
  * @param  None
  * @retval None
  */
void LwIP_Init(void)
{
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;

	tcpip_init(NULL,NULL);

#if LWIP_DHCP
  ipaddr.addr = 0;
  netmask.addr = 0;
  gw.addr = 0;

	//printf("DHCP can be choosed !!!\n");
#else
  IP4_ADDR(&ipaddr, 192,168,3,1);//1.134
  IP4_ADDR(&netmask, 255, 255, 255, 0);
  IP4_ADDR(&gw, 192, 168, 1, 1);	
#endif


  netif_add(&DM9161_netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

  netif_set_default(&DM9161_netif);

#if LWIP_DHCP
  dhcp_start(&netif);
#endif
  /*  When the netif is fully configured this function must be called.*/
  netif_set_up(&DM9161_netif);
}
