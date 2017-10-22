#include "print_queue.h"

#include "print_bmp.h"
#include <math.h>

#define assert(expr, str)			 	\
	do {								\
		if(!(expr)) {					\
			printf((str));				\
			while(1);					\
		}								\
	}while(0)
	
/**
 * @name   	Find_Entry
 * @brief	 	���ݶ������ȣ��ҵ����ʵĽڵ㣬���ҷ������������Ҳ�����ڴ������
 * @param	  len 								��������
 *					order_prio_sigal 		�������ȼ�
 * @return	
 */
static s8_t Find_Entry(u16_t len , u8_t order_prio_sigal);


/**
 * @name   	Change_Normal_Order_Seque
 * @brief	 	�����䵽�Ľڵ㣬���뵽�����У����ּӼ�����ͨ��
 * @param	  entry_index ����������
 * @return	
 */
static s8_t Change_Normal_Order_Seque( s8_t entry_index );


/**
 * @name   	Change_Order_Seque
 * @brief	 	��ӡ���нڵ�����ʱ���мӼ�����������δ����ӣ�
						���Ծ��촦�����˳���üӼ�����������ӡ�
 * @param	  entry_index ����������
 * @return	
 */
static s8_t Change_Order_Seque(s8_t entry_index);


/**
 * @name   	Find_Block_Num
 * @brief	 	��Find_Entry���ã����ݶ�Ӧ���С�������ȼ�������������
 * @param	  blocksize 					��Ӧ�ڵ��С
 *					order_prio_sigal 		�������ȼ�
 * @return	
 */
static s8_t Find_Block_Num(u16_t blocksize ,u8_t order_prio_sigal);


/**
 * @name   	Find_Front_And_Next_Index
 * @brief	 	����size�Ľڵ�飬�ҵ���ȴ�С�Ľڵ���ǰ��������
 * @param	  start_index 				��ʼ���
 *					aim_size 						Ŀ���С
 *					front_index					ǰ������
 *					next_index					�������
 *					current_index				��ǰ����
 * @return	
 */
static s8_t Find_Front_And_Next_Index(s8_t start_index , u16_t aim_size,  u8_t *front_index , u8_t *next_index, u8_t *current_index);


/**
 * @fn		OrderEnqueue
 * @brief	�������������
 * @param	buf ������ָ��
 *				entry_index ������
 *				order_len ��������
 *				order_prio_sigal ����������ͨ�ı�־
 * @return	void
 */
static s8_t OrderEnqueue(SqQueue* buf,s8_t entry_index , u16_t order_len,u8_t order_prio_sigal);


/**
 * @brief	����ͨ������ӵ���ӡ������
 * @param	buf ��ͨ����������
			entry_index ��������
 * @return	�������
 */
static s8_t	Add_Order_To_Print_Queue(SqQueue *buf,s8_t entry_index , u8_t order_prio_sigal);

/**
 * @fn		error_order_deal
 * @brief	�������󣬴�������
 * @param	buf	 ������
 * @return	
 */
static s8_t error_order_deal(SqQueue* buf, s8_t order_prio_sigal);


/**
 * @fn		printOrderQueueSeque
 * @brief	���ڵ��������ӡ���е��б���Ϣ��
 * @param	buf	 ������
 * @return	
 */
static void printOrderQueueSeque();
	

//��ӡ���ж�����Ϣ��
order_print_queue_info order_print_table;        
//���α�
batch_info batch_info_table[MAX_BATCH_NUM];				 


/**
 * @name   	Find_Entry
 * @brief	 	���ݶ������ȣ��ҵ����ʵĽڵ㣬���ҷ������������Ҳ�����ڴ������
 * @param	  len 								��������
 *					order_prio_sigal 		�������ȼ�
 * @return	
 */
static s8_t Find_Entry(u16_t len , u8_t order_prio_sigal)
{
	if(len > BLOCK_MAX_SIZE)       //�������
	{
		return ORDER_TOO_LARGER;
	}
	
	if(len <= BLOCK_1K_SIZE)
	{
		DEBUG_PRINT("QUEUE DEBUG : TRYING TO GOT 1K NODE-----------\n");
		return Find_Block_Num(BLOCK_1K_SIZE ,order_prio_sigal);
	}
	else if (len <= BLOCK_2K_SIZE)
	{	
		return Find_Block_Num(BLOCK_2K_SIZE ,order_prio_sigal);
	}
	else if(len <= BLOCK_4K_SIZE)
	{	
		return Find_Block_Num(BLOCK_4K_SIZE ,order_prio_sigal);
	}
	else if (len <= BLOCK_10K_SIZE)
	{	
		return Find_Block_Num(BLOCK_10K_SIZE ,order_prio_sigal);	
	}
	else 
		return ORDER_FIND_INDEX_ERR;
	
}


/**
 * @name   	Find_Block_Num
 * @brief	 	��Find_Entry���ã����ݶ�Ӧ���С�������ȼ�������������
 * @param	  blocksize 					��Ӧ�ڵ��С
 *					order_prio_sigal 		�������ȼ�
 * @return	
 */
static s8_t Find_Block_Num(u16_t blocksize ,u8_t order_prio_sigal)
{	
	u8_t os_err = 0;
	OS_EVENT * Block_Sem = NULL;	
	s8_t Lack_Num = 0;
	
	u8_t entry_index = 0;
	u8_t block_index_start = 0;	
	u8_t block_index_end = 0;
	
	switch(blocksize)
	{	
		case BLOCK_1K_SIZE:{
			block_index_start = BLOCK_1K_INDEX_START;//��ʼ����
			block_index_end = BLOCK_1K_INDEX_END;//��������
			Lack_Num = Lack_Of_1K;	//ȱ���ڴ����ź�
			Block_Sem = Block_1K_Sem; //�ڴ��������ź���
			break;
		}
		case BLOCK_2K_SIZE:{			
			block_index_start = BLOCK_2K_INDEX_START;
			block_index_end = BLOCK_2K_INDEX_END;
			Lack_Num = Lack_Of_2K;
			Block_Sem = Block_2K_Sem;
			break;
		}
		case BLOCK_4K_SIZE:{			
			block_index_start = BLOCK_4K_INDEX_START;			
			block_index_end = BLOCK_4K_INDEX_END;
			Lack_Num = Lack_Of_4K;
			Block_Sem = Block_4K_Sem;
			break;
		}
		case BLOCK_10K_SIZE:{
			block_index_start = BLOCK_10K_INDEX_START;			
			block_index_end = BLOCK_10K_INDEX_END;
			Lack_Num = Lack_Of_10K;
			Block_Sem = Block_10K_Sem;
			break;
		}
		default :
			DEBUG_PRINT("QUEUE DEBUG :  SWITCH ERROR--------------\n");
			break;
	}
	
	if(order_prio_sigal){//��������
		INT16U Accept_Sigal_num = 0;
		Accept_Sigal_num = OSSemAccept(Block_Sem);
		if( Accept_Sigal_num == 0){//֤���Ѿ�û���ڴ��,�����߳̽�������û���ڴ����źš�
			
			DEBUG_PRINT("QUEUE DEBUG :  LACK OF BLOCK ,Lack_Num %d\n ",Lack_Num);
			OSTimeDlyHMSM(0, 0, 0, 500);
			return Lack_Num;
		}
	}
	else{//�ǽ�������
		OSSemPend(Block_Sem,0,&os_err);	//���޿����ڴ�飬������
		DEBUG_PRINT("QUEUE DEBUG :  GOT Block_Sem-------------\n");
	}
	
//	DEBUG_PRINT("QUEUE DEBUG :  acquire mutex of printqueue-----------\n");
	OSMutexPend(order_print_table.mutex,0,&os_err);//�ö�����Դ����
//	DEBUG_PRINT("QUEUE DEBUG :  got mutex of printqueue-----------\n");
	entry_index = block_index_start;
	
	DEBUG_PRINT("QUEUE DEBUG : entry_index: %u, block_index_start = %u,------- block_index_end = %u\n", entry_index, block_index_start, block_index_end);
	
	while(entry_index < block_index_end){//���ҿյ�������
		if(order_print_table.order_node[entry_index].data == NULL){			
			OSMutexPost(order_print_table.mutex);//�ͷŶ�����Դ����	
			DEBUG_PRINT("QUEUE DEBUG : Find EmptyBlock =  %u\n", entry_index);
			return entry_index;
		}
		entry_index++;
	}	
	
	OSMutexPost(order_print_table.mutex);
	DEBUG_PRINT("QUEUE DEBUG :   ORDER_FIND_INDEX_ERR-----------\n");
	return ORDER_FIND_INDEX_ERR;
}


/**
 * @brief	����ͨ������ӵ���ӡ������
 * @param	buf ��ͨ����������
			entry_index ��������
 * @return	�������
 */
static s8_t	Add_Order_To_Print_Queue(SqQueue *buf,s8_t entry_index , u8_t order_prio_sigal)
{
	u8_t os_err = 0;
	u8_t *order_head = NULL;
	INT8U err;
	u8_t time_index = 0;	
	
	order_head = buf->base + buf->read;// ��ȡ����ͷ//  �������ڴ��buf�Ķ���������������ͷ��
	
	OSMutexPend(order_print_table.mutex,0,&os_err);
	
	//��ʼ�����ڴ��
	if(entry_index < BLOCK_1K_INDEX_END){
		DEBUG_PRINT("\nQUEUE DEBUG----GOT 1K BLOCK----\n");
		order_print_table.order_node[entry_index].data = OSMemGet(queue_1K,&os_err);
	}
	else if(entry_index < BLOCK_2K_INDEX_END){
		DEBUG_PRINT("\nQUEUE DEBUG----GOT 2K BLOCK----\n");
		order_print_table.order_node[entry_index].data = OSMemGet(queue_2K,&os_err);
	}
	else if(entry_index < BLOCK_4K_INDEX_END){
		DEBUG_PRINT("\nQUEUE DEBUG----GOT 4K BLOCK----\n");
		order_print_table.order_node[entry_index].data = OSMemGet(queue_4K,&os_err);
	}
	else if(entry_index < BLOCK_10K_INDEX_END){	
		DEBUG_PRINT("\nQUEUE DEBUG----GOT 10K BLOCK----\n");
		order_print_table.order_node[entry_index].data = OSMemGet(queue_10K,&os_err);
		DEBUG_PRINT("ORDER DATA POINTER: %p\n", order_print_table.order_node[entry_index].data);
		
	}
	
	/*������ȡ����ͷ������*/
	order_print_table.order_node[entry_index].size = 								(*(order_head + ORDER_SIZE_OFFSET) << 8) 						+ *(order_head + ORDER_SIZE_OFFSET + 1);											//��������	
	
	order_print_table.order_node[entry_index].mcu_id = 							((u32_t)(*(order_head + ORDER_MCU_ID_OFFSET)) << 24) 				+ 	((u32_t)(*(order_head + ORDER_MCU_ID_OFFSET +1 )) << 16 ) 						
																																	+	((u32_t)(*(order_head + ORDER_MCU_ID_OFFSET +2 )) << 8) 	+		((u32_t)(*(order_head + ORDER_MCU_ID_OFFSET +3 )));													//���ذ�ID
	
	order_print_table.order_node[entry_index].sever_send_time =  		((u32_t)(*(order_head + ORDER_SEVER_SEND_TIME_OFFSET)) << 24) 			+ 	((u32_t)(*(order_head + ORDER_SEVER_SEND_TIME_OFFSET +1 )) << 16 ) 
																																	+	((u32_t)(*(order_head + ORDER_SEVER_SEND_TIME_OFFSET +2 )) << 8) 	+		((u32_t)(*(order_head + ORDER_SEVER_SEND_TIME_OFFSET +3 )));				//ʱ��
	
	order_print_table.order_node[entry_index].serial_number = 			((u32_t)(*(order_head + ORDER_SERIAL_NUMBER_OFFSET)) << 24)				+ 	((u32_t)(*(order_head + ORDER_SERIAL_NUMBER_OFFSET +1 )) << 16 ) 
																																	+	((u32_t)(*(order_head + ORDER_SERIAL_NUMBER_OFFSET +2 )) << 8) 	+		((u32_t)(*(order_head + ORDER_SERIAL_NUMBER_OFFSET +3 )));						//�������
	DEBUG_PRINT("QUEUE DEBUG :id number :%u", order_print_table.order_node[entry_index].serial_number);
	
	order_print_table.order_node[entry_index].batch_number = 				(*(order_head+ORDER_BATCH_NUMBER_OFFSET)<<8)				+ *(order_head + ORDER_BATCH_NUMBER_OFFSET + 1);							//��������
	order_print_table.order_node[entry_index].batch_within_number =	(*(order_head+ORDER_BATCH_WITHIN_NUMBER_OFFSET)<<8)	+ *(order_head + ORDER_BATCH_WITHIN_NUMBER_OFFSET + 1);				//�����ڶ������
	order_print_table.order_node[entry_index].check_sum = 					(*(order_head+ORDER_CHECK_SUM_OFFSET)<<8)						+ *(order_head + ORDER_CHECK_SUM_OFFSET + 1);									//У����	
	order_print_table.order_node[entry_index].preservation = 				(*(order_head+ORDER_PRESERVATION_OFFSET)<<8)				+ *(order_head + ORDER_PRESERVATION_OFFSET + 1);							//����
	order_print_table.order_node[entry_index].data_source = 	order_print_table.order_node[entry_index].preservation;
	order_print_table.order_node[entry_index].priority = order_prio_sigal;
  order_print_table.order_node[entry_index].next_print_node = -1;
	
	
	if(order_print_table.empty == 1){//��ԭ�ȴ�ӡ����Ϊ��
		order_print_table.head = entry_index;
		order_print_table.tail = entry_index;
		order_print_table.empty = 0;
	}
	else{
		//���Ĵ�ӡ������Ϣ��ȷ��������λ��˳��
		DEBUG_PRINT("QUEUE DEBUG :Before Insert\n");
		printOrderQueueSeque();
		Change_Normal_Order_Seque(entry_index);
		DEBUG_PRINT("QUEUE DEBUG :After Insert\n");
		printOrderQueueSeque();
	}
	
	//���������������ݼ����ӡ����
	Read_Order_Queue(buf,order_print_table.order_node[entry_index].data);
	
	//ʹ�����ӡ���У��ͷ���
	OSMutexPost(order_print_table.mutex);
	OSTimeDlyHMSM(0,0,0,100);
	
	return ORDER_QUEUE_OK;
}



static void printOrderQueueSeque()
{
	u8_t index = order_print_table.head;
	DEBUG_PRINT("QUEUE DEBUG : traverse queue{ \n");
	DEBUG_PRINT("				node : %u  - >  next: %u\n",index , order_print_table.order_node[index].next_print_node);		
	while (index != order_print_table.head)
	{				
		index = order_print_table.order_node[index].next_print_node;
		DEBUG_PRINT("				node : %u  - >  next: %u\n",index , order_print_table.order_node[index].next_print_node);
	}
			
		
	return ;
}



/**
 * @name   	Change_Normal_Order_Seque
 * @brief	 	�����䵽�Ľڵ㣬���뵽�����У����ּӼ�����ͨ��
 * @param	  entry_index ����������
 * @return	
 */
static s8_t Change_Normal_Order_Seque( s8_t entry_index )
{
	if(order_print_table.order_node[entry_index].priority == ORDER_PRIO_HIGH){//�Ӽ�����
		u8_t start_index = order_print_table.head;
		u8_t start_index_front = order_print_table.head;
		
		while(order_print_table.order_node[start_index].data != NULL && order_print_table.order_node[start_index].priority == ORDER_PRIO_HIGH){	//������Ӽ����С�
			start_index_front = start_index;
			start_index = order_print_table.order_node[start_index].next_print_node;
		}
		if(start_index == start_index_front){//�����Ӷ�ͷ��ʼ���Ͳ��ǼӼ�����			
			order_print_table.order_node[entry_index].next_print_node = order_print_table.head;
			order_print_table.head = entry_index;							
			return 0;
		}
		else{//�����������мӼ�������
			order_print_table.order_node[entry_index].next_print_node = order_print_table.order_node[start_index_front].next_print_node;
			order_print_table.order_node[start_index_front].next_print_node = entry_index;
			if(order_print_table.order_node[start_index].data == NULL){			//�����ӡ����ֻʣ�¼Ӽ�����
				order_print_table.tail = entry_index;
				order_print_table.order_node[entry_index].next_print_node = -1;
			}
			return 0;			
		}
	}
	else{//��ͨ����
		order_print_table.order_node[order_print_table.tail].next_print_node = entry_index;
		order_print_table.tail = entry_index;
	}
}


/**
 * @name   	Change_Order_Seque
 * @brief	 	��ӡ���нڵ�����ʱ���мӼ�����������δ����ӣ�
						���Ծ��촦�����˳���üӼ�����������ӡ�
 * @param	  entry_index ����������
 * @return	
 */
static s8_t Change_Order_Seque(s8_t entry_index)
{
	u8_t next_index;	//�������ͬ�ڴ��������ĺ�һ��
	u8_t front_index;	//�������ͬ�ڴ���������ǰһ��
	u8_t current_index; //�������ͬ�ڴ�������
	u16_t block_size = 0;
	s8_t same_block_index = 0;
	u16_t printing_size = 0;
	
	if(entry_index < 0){	//�����ж�
		switch (entry_index)
		{
			case Lack_Of_1K :{
				block_size = BLOCK_1K_SIZE;
				break;
			}
			case Lack_Of_2K:{				
				block_size = BLOCK_2K_SIZE;
				break;
			}
			case Lack_Of_4K:{
				block_size = BLOCK_4K_SIZE;
				break;
			}
			case Lack_Of_10K:{
				block_size = BLOCK_10K_SIZE;
				break;
			}
		}
		
		if(order_print_table.order_node[order_print_table.head].size <= BLOCK_1K_SIZE)
		{
			printing_size = BLOCK_1K_SIZE;
		}
		else if(order_print_table.order_node[order_print_table.head].size <= BLOCK_2K_SIZE)
		{
			printing_size = BLOCK_2K_SIZE;
		}
		else if(order_print_table.order_node[order_print_table.head].size <= BLOCK_4K_SIZE)
		{
			printing_size = BLOCK_4K_SIZE;
		}
		else if (order_print_table.order_node[order_print_table.head].size <= BLOCK_10K_SIZE)
		{
			printing_size = BLOCK_10K_SIZE;
		}
		
		if(printing_size != block_size){	//��ͷ���ǲ���ͬ�ڴ�Ķ�������Ҫ�����滻
			u8_t start_index = order_print_table.head;
			u8_t start_index_front = order_print_table.head;
			u8_t in_urgent_order_flag = 0;
			while(order_print_table.order_node[start_index].priority == ORDER_PRIO_HIGH){	//������Ӽ����С�
				if(order_print_table.order_node[start_index].size == block_size){
					in_urgent_order_flag = 1;
					break;
				}
				if(start_index == order_print_table.tail )
					break;
				start_index_front = start_index;				
				start_index = order_print_table.order_node[start_index].next_print_node;
			}
			
			
			if(start_index == start_index_front && order_print_table.order_node[start_index].priority != ORDER_PRIO_HIGH){//�����Ӷ�ͷ��ʼ���Ͳ��ǼӼ�����
				
				
				Find_Front_And_Next_Index(start_index, block_size, &next_index , &front_index, &current_index);//�ҵ������ͷ���Ǹ���ͬ�ڴ�Ķ�������������������
				order_print_table.order_node[front_index].next_print_node = next_index;
				order_print_table.order_node[current_index].next_print_node = order_print_table.head;						
				order_print_table.head = current_index;
				
				DEBUG_PRINT("QUEUE DEBUG : head change \n" );
				return 0;
			}
			else{//�����������мӼ�������
				if(in_urgent_order_flag == 0){//��ͬ�ڴ�����������ڼӼ��������С�
					if(start_index == order_print_table.tail)
					{
						
					DEBUG_PRINT("QUEUE DEBUG : start_index %u\n" , start_index );
						return 0;
					}
					Find_Front_And_Next_Index(start_index, block_size, &next_index , &front_index, &current_index);//�ҵ������ͷ���Ǹ���ͬ�ڴ�Ķ�������������������
					
					if(front_index == current_index){				
						//��ͨ������ӡ�����еĵ�һ��������ͬ��С���ڴ�飬���Բ��ý��в�����
						
					DEBUG_PRINT("QUEUE DEBUG : first do not change \n");
						return 0;
					}
					else{					
						//��ͨ������ӡ�����еĵ�һ�������ڴ����ͬ��С�Ķ�������Ҫ����ͬ��С�Ķ���������ͨ��ӡ���е��ײ���
						order_print_table.order_node[front_index].next_print_node = next_index;
						order_print_table.order_node[current_index].next_print_node = start_index;						
						order_print_table.order_node[start_index_front].next_print_node = current_index;	
					}				
				}				
				else{
					//��ͬ���ڴ浥Ԫ�Ķ������ǼӼ�����������Ҫ���д���
					DEBUG_PRINT("QUEUE DEBUG : same do not change\n" );
					return 0;
				}
			}
		}
		else{		
			return 0; //��ͷ������ͬ�ڴ浥Ԫ�������ȴ����ӡ��ɼ��ɡ�
		}
	}
}


/**
 * @name   	Find_Front_And_Next_Index
 * @brief	 	����size�Ľڵ�飬�ҵ���ȴ�С�Ľڵ���ǰ��������
 * @param	  start_index 				��ʼ���
 *					aim_size 						Ŀ���С
 *					front_index					ǰ������
 *					next_index					�������
 *					current_index				��ǰ����
 * @return	
 */
static s8_t Find_Front_And_Next_Index(s8_t start_index , u16_t aim_size,  u8_t *front_index , u8_t *next_index ,u8_t *current_index)
{
	u8_t front = start_index;
	u8_t next = order_print_table.order_node[start_index].next_print_node; 
	u8_t current = start_index;
	while(next != order_print_table.tail){		 
		if(order_print_table.order_node[current].size == aim_size ){//����
			break;	
		}
		else{
			front = current;
			current = next;
			next = order_print_table.order_node[current].next_print_node;
		}
		OSTimeDlyHMSM(0, 0, 1, 0);
	}
	*front_index = front;
	*next_index = next;
	*current_index = current;
	return 0;
}


/**
 * @brief	ɾ��һ�ݶ������黹�ڴ���ϵͳ
 * @param	��������
 * @return	ִ�н��
 */
s8_t Delete_Order(s8_t entry_index)
{
	u8 err;
	order_info *orderp = &order_print_table.order_node[entry_index];
	u8_t *data = orderp->data;
	
	OSMutexPend(order_print_table.mutex, 0, &err);	
	orderp->data = NULL;
	if(NULL == data) {
		DEBUG_PRINT("Delete_Order: Internal Error: Order %u has null block!\n", 
										orderp->serial_number);
		OSMutexPost(order_print_table.mutex);
		return ORDER_EMPTY_BLOCK;
	}
	DEBUG_PRINT("Delete_Order: Deleting order %u.\n", orderp->serial_number);
	OSMutexPost(order_print_table.mutex);	
	/*�ͷ��ڴ��*/		
	if(entry_index < BLOCK_1K_INDEX_END) {
		DEBUG_PRINT("Delete_Order: PUT 1K BLOCK\n");
		OSMemPut(queue_1K, data);
		OSSemPost(Block_1K_Sem);
	}else if(entry_index < BLOCK_2K_INDEX_END){
		DEBUG_PRINT("Delete_Order: PUT 2K BLOCK\n");
		OSMemPut(queue_2K, data);
		OSSemPost(Block_2K_Sem);
	}else if(entry_index < BLOCK_4K_INDEX_END){
		DEBUG_PRINT("Delete_Order: PUT 4K BLOCK\n");
		OSMemPut(queue_4K, data);			
		OSSemPost(Block_4K_Sem);
	}else if(entry_index < BLOCK_10K_INDEX_END){
		DEBUG_PRINT("Delete_Order: PUT 10K BLOCK\n");
		OSMemPut(queue_10K, data);			
		OSSemPost(Block_10K_Sem);
	}
	return ORDER_QUEUE_OK;
}



static u8_t GetDataType(const u8_t *datap)
{
	if(0x7e == datap[0] && 0xff == datap[1])
		return DATA_IS_PLAINTEXT;
	else if(0xff == datap[0] && 0x7e == datap[1])
		return DATA_IS_IMAGE;
	else if(0x7f == datap[0] && 0xff == datap[1])
		return DATA_IS_QRCODE;
	return DATA_INVALID;
}

static u16_t GetDataLength(const u8_t *datap)
{
	return (datap[2] << 8) + datap[3];
}

static u8_t *MoveToNextData(u8_t *datap, u16_t length)
{
	u8_t *nextp = datap + length + DATA_HEADER_SIZE + DATA_FOOTER_SIZE;
	return nextp;
}

static u16_t GetImageBytes(const u8_t *datap, u16_t length)
{
	return (datap[DATA_HEADER_SIZE+length] << 8) + datap[DATA_HEADER_SIZE+length+1];
}


/**
 * @fn		GetOrderFromQueue
 * @brief	�����ӡ����ͷ��㲢��������������entryp��ָ�ռ�
 * @param	entryp	��Ż�õĽ�������
 * @return	ORDER_QUEUE_OK	��ȡ�ɹ�
			ORDER_QUEUE_EMPTY ����Ϊ�գ���ȡʧ��
 */
s8_t GetOrderFromQueue(u8_t *entryp)
{
	INT8U err;
	
	OSMutexPend(order_print_table.mutex, 0, &err);
	
	if(order_print_table.empty == 1) {
		OSMutexPost(order_print_table.mutex);		
		return ORDER_QUEUE_EMPTY;
	}
	*entryp = order_print_table.head;	//ȡ���е�һ�ݶ���
	if(order_print_table.head == order_print_table.tail) {	//�����н���һ�ݶ�����ȡ�ߺ����Ϊ��
		order_print_table.empty = 1;
		order_print_table.head = order_print_table.tail = 0;
	}else {		//�������ж���һ�ݶ���
		order_print_table.head = order_print_table.order_node[order_print_table.head].next_print_node;
	}
	order_print_table.order_node[*entryp].next_print_node = 0;
	DEBUG_PRINT("GetOrderFromQueue: Got print order %u\n", 
		order_print_table.order_node[*entryp].serial_number);
	if(order_print_table.empty == 1) {
		DEBUG_PRINT("GetOrderFromQueue: No more job appending.\n");
	}else {
		DEBUG_PRINT("GetOrderFromQueue: Next job's num is %u\n", 
			order_print_table.order_node[order_print_table.head].serial_number);		
	}
	OSMutexPost(order_print_table.mutex);
	return ORDER_QUEUE_OK;
}


/**
 * @brief	��鶩���������Ƿ��д�
 * @param	��������
 * @return	ORDER_DATA_ERR	����������
			ORDER_DATA_OK	��������ȷ
 */
s8_t CheckOrderData(u8_t entry_index)
{
	u16_t pbytes, length;
	u8_t type;
	order_info *orderp = &order_print_table.order_node[entry_index];
	u8_t *datap = orderp->data;	
	
	if(NULL == datap) {
		orderp->status = PRINT_STATUS_DATA_ERR;	/* �������ݴ��� */
		Order_Print_Status_Send(orderp,PRINT_STATUS_DATA_ERR);
		DEBUG_PRINT("CheckOrderData: checking empty block!\n");
		return ORDER_DATA_ERR;		
	}
	pbytes = 0;
	while(pbytes < orderp->size) {	// ������������
		length = GetDataLength(datap);
		type = GetDataType(datap);
 
		if(type == DATA_INVALID){	// ���ݴ���			
			orderp->status = PRINT_STATUS_DATA_ERR;	/* �������ݴ��� */
			Order_Print_Status_Send(orderp,PRINT_STATUS_DATA_ERR);

			DEBUG_PRINT("CheckOrderData: Invalid type of data field!\n");
			
			return ORDER_DATA_ERR;
		}
		datap = MoveToNextData(datap, length);	// �ƶ�����һ������
		pbytes += length + DATA_HEADER_SIZE + DATA_FOOTER_SIZE;
	}
	if(pbytes == 0)	// �����ݶ���
		return ORDER_DATA_ERR;
	return ORDER_DATA_OK;
}


/**
 * @brief	��ӡһ�ݶ��������޸���״̬
 * @param	��ӡ��Ԫ����
 * @return	ִ�н��
 */
s8_t Print_Order(u8_t cellno)
{
	u16_t length, pbytes;
	order_info *orderp;
	u8_t *datap;
	u8_t type;
	INT8U err;
	PrintCellInfo *cellp = &PCMgr.cells[cellno-1];
	static int needCutPaper[MAX_CELL_NUM+1] = { 0 };
	int i;
	
	orderp = &order_print_table.order_node[cellp->entryIndex];
	datap = orderp->data;	
	length = GetDataLength(datap);

	if(needCutPaper[cellno]) {	// �г���ӡ���������ڴ�ӡ�����ӶϿ����Դ���ϵ��쳣����ӡ�Ĵ��󶩵�
		OutputErrorTag(cellno);
		cutPaper(cellno);
		needCutPaper[cellno] = 0;
	}
	
	DEBUG_PRINT("-------------Print_Order: ---------------\n");
	DEBUG_PRINT("-ORDER NUM:   %u\n", orderp->serial_number);
	DEBUG_PRINT("-ORDER SIZE : %u\n", orderp->size);
	DEBUG_PRINT("-DATA HEADER: %x%x\n", *orderp->data, *(orderp->data+1));
	DEBUG_PRINT("-DATA LENGTH: %u\n", length);
	DEBUG_PRINT("-DATA TAIL  : %x%x\n", *(datap + 4 + length + 2), *(datap + 4 + length + 3));
	DEBUG_PRINT("-------------------------------------\n");	
	
	orderp->status = PRINT_STATUS_START;
	Order_Print_Status_Send(orderp,	PRINT_STATUS_START);
	
	cellp->beginTick = sys_now();
	
	// ��ҵ���ʱ�����2s����Ϊ��ӡ��Ԫû����������
	if(cellp->beginTick - cellp->endTick > 2000/20)
		cellp->workedTime = 0;

	pbytes = 0;		
	while(pbytes < orderp->size) {	//�����������ݲ���ӡ
		length = GetDataLength(datap);
		type = GetDataType(datap);
 
		if(DATA_IS_PLAINTEXT == type) {		// ��������
			DEBUG_PRINT("Print_Order: Printing Paint Text\n");
			//DMA_To_USARTx_Send(datap + DATA_HEADER_SIZE, length, cellno);
			mySendData(datap + DATA_HEADER_SIZE, length, cellno);
		}
		else if(DATA_IS_IMAGE == type) {	// ͼƬ����		
			DEBUG_PRINT("Print_Order: Printing Picture\n");
//			OSTimeDlyHMSM(0,0,2,0);
			printImages(datap + DATA_HEADER_SIZE, GetImageBytes(datap, length), cellno);
			
		}
		else if(DATA_IS_QRCODE == type) {	// ��ά������		
//			OSTimeDlyHMSM(0,0,0,500);
			printQRCode(datap + DATA_HEADER_SIZE, length, cellno);
			DEBUG_PRINT("Print_OrderPrinting: QR Code\n");
		}else {
			DEBUG_PRINT("BUG DETECT: Print_Order: DATA CHECKING WRONG.\n");
		}
		pbytes += length + DATA_HEADER_SIZE + DATA_FOOTER_SIZE; //�Ѿ�������ֽ���
		PCMgr.cells[cellno-1].totalLength += pbytes;
		
		datap = MoveToNextData(datap, length);
	}
	orderp->status = ORDER_DATA_OK;	// �������ݽ�����ȷ
	
	// ���ͼ���ӡ��״̬��ָ��Ծ����������Ƿ��ӡ�ɹ�
	OSTimeDlyHMSM(0,0,(u8)((orderp->size / 512) + (orderp->size / 2048)), 500);
	DEBUG_PRINT("Print_Order: Waiting for Printer's status.\n");
	SEND_STATUS_CMD_ONE(cellno);
	OSTimeDlyHMSM(0,0,1,0);//����״̬����ʱ
	
	// ���������Pend��������̷��أ�����ʱ������ӡ�����д������쳣
	OSSemPend(cellp->printDoneSem, 20, &err);
	if(err == OS_ERR_TIMEOUT){	// ��ӡ�����г����쳣��ʹ�ô�ӡ������
		DEBUG_PRINT("Print_Order: Printer %u Off-line while Printing.\n", cellno);
		needCutPaper[cellno] = 1;
		Order_Print_Status_Send(orderp, PRINT_CELL_STATUS_ERR);	
		PutPrintCell(cellno, PRINT_CELL_STATUS_ERR);
	}

	return ORDER_DATA_OK;
}


static void eight_image(uint8_t image_data_one , uint8_t *image_data_eight)
{
	int i;
	uint8_t temp1, temp2;
	for(i = 0; i < 4; ++i){
		image_data_eight[i] = 0;
		temp1 = pow(2,7-i*2);     //ȡ��7-i*2λ
		temp2 = pow(2,7-i*2-1);		//ȡ��7-i*2-1λ	
		
		temp1 = temp1 & image_data_one;//�ѳ�7-i*2λ������λ��0
		if(temp1 > 0){
			image_data_eight[i] += 0xf0;//�ж�7-i*2λ�Ƿ�Ϊ1���ǵĻ��ͽ�����λ��1
		}
		
		temp2 = temp2 & image_data_one;//�ѳ�7-i*2-1λ������λ��0
		if(temp2 > 0){
			image_data_eight[i] += 0x0f;//�ж�7-i*2-1λ�Ƿ�Ϊ1���ǵĻ��ͽ�����λ��1
		}
	}
}

///**
// * @brief	��ӡһ�ݶ��������޸���״̬
// * @param	��ӡ��Ԫ����
// * @return	ִ�н��
// */
//extern uint8_t data1[393] ;
//s8_t Print_Order(u8_t cellno)
//{
//	uint8_t *image_data;
//	uint8_t data_2[32];
//	uint8_t data_3[100];
//	int j = 0,k = 0;
//	
//	u16_t length, pbytes;
//	order_info *orderp;
//	u8_t *datap;
//	u8_t type;
//	INT8U err;
//	PrintCellInfo *cellp = &PCMgr.cells[cellno-1];
//	static int needCutPaper[MAX_CELL_NUM+1] = { 0 };
//	int i;
//	
//	orderp = &order_print_table.order_node[cellp->entryIndex];
//	datap = orderp->data;	
//	length = GetDataLength(datap);
//	
//	
//	
//	
//	

//	if(needCutPaper[cellno]) {	// �г���ӡ���������ڴ�ӡ�����ӶϿ����Դ���ϵ��쳣����ӡ�Ĵ��󶩵�
//		OutputErrorTag(cellno);
//		cutPaper(cellno);
//		needCutPaper[cellno] = 0;
//	}
//	
//	DEBUG_PRINT("-------------Print_Order: ---------------\n");
//	DEBUG_PRINT("-ORDER NUM:   %u\n", orderp->serial_number);
//	DEBUG_PRINT("-ORDER SIZE : %u\n", orderp->size);
//	DEBUG_PRINT("-DATA HEADER: %x%x\n", *orderp->data, *(orderp->data+1));
//	DEBUG_PRINT("-DATA LENGTH: %u\n", length);
//	DEBUG_PRINT("-DATA TAIL  : %x%x\n", *(datap + 4 + length + 2), *(datap + 4 + length + 3));
//	DEBUG_PRINT("-------------------------------------\n");	
//	
//	orderp->status = PRINT_STATUS_START;
//	Order_Print_Status_Send(orderp,	PRINT_STATUS_START);
//	
//	cellp->beginTick = sys_now();
//	
//	// ��ҵ���ʱ�����2s����Ϊ��ӡ��Ԫû����������
//	if(cellp->beginTick - cellp->endTick > 2000/20)
//		cellp->workedTime = 0;

//	pbytes = 0;		
////	while(pbytes < orderp->size) {	//�����������ݲ���ӡ
////		length = GetDataLength(datap);
////		type = GetDataType(datap);
//// 
////		if(DATA_IS_PLAINTEXT == type) {		// ��������
////			DEBUG_PRINT("Print_Order: Printing Paint Text\n");
////			//DMA_To_USARTx_Send(datap + DATA_HEADER_SIZE, length, cellno);
////			mySendData(datap + DATA_HEADER_SIZE, length, cellno);
////		}
////		else if(DATA_IS_IMAGE == type) {	// ͼƬ����		
////			DEBUG_PRINT("Print_Order: Printing Picture\n");
//////			OSTimeDlyHMSM(0,0,2,0);
////			printImages(datap + DATA_HEADER_SIZE, GetImageBytes(datap, length), cellno);
////			
////		}
////		else if(DATA_IS_QRCODE == type) {	// ��ά������		
//////			OSTimeDlyHMSM(0,0,0,500);
////			printQRCode(datap + DATA_HEADER_SIZE, length, cellno);
////			DEBUG_PRINT("Print_OrderPrinting: QR Code\n");
////		}else {
////			DEBUG_PRINT("BUG DETECT: Print_Order: DATA CHECKING WRONG.\n");
////		}
////		pbytes += length + DATA_HEADER_SIZE + DATA_FOOTER_SIZE; //�Ѿ�������ֽ���
////		PCMgr.cells[cellno-1].totalLength += pbytes;
////		
////		datap = MoveToNextData(datap, length);
////	}
//	//printImages(data1 + DATA_HEADER_SIZE,16, cellno);

////	compress(data1,&data1[999],10,100,des);
////	uncompress_print(des,&des[1001],cellno);
////	for(k = 0;k < 100; ++k)
////	{		
////			data_2[k] = 0xff;
////	}
////	
////	PRINT_FAST_BITMAP(48,30,0,30*8,0, cellno);
////	for(k = 0;k < 30*8; ++k)
////	{		
////			original_print(8,30,data_2,cellno);
////	}
////	
//	
//		
//		
//	PRINT_FAST_BITMAP(48,28,0,28*8,0, cellno);//��ͨ��ӡ28�У�28*8�У���Ϊ��ÿ�ֽڴ�����ǰ˸��㣬����һ���ֽڴ������һ��������Ҫ��8
//	for(i = 0;i< 56;i++)  //ͼƬ��56��
//	{		
//		for(k = 0;k < 7; ++k)//ͼƬ��7��
//		{		
//			eight_image(data1[i*7+k], data_2 + k*4);//��ÿһ�зŴ��ı�
//		}
//		for(j = 0; j < 4; ++j)//ÿһ��ҲҪ�Ŵ��ı�
//		{
//			original_print(7,28,data_2,cellno);//��ӡͼƬ��һ��28�ֽ�
//		}
////		for(k = 0; k < 32; ++k)
////			printf("%c",data_2[k]);
////		printf("\r\n");
//	}

//	
//	orderp->status = ORDER_DATA_OK;	// �������ݽ�����ȷ
//	
//	// ���ͼ���ӡ��״̬��ָ��Ծ����������Ƿ��ӡ�ɹ�
//	OSTimeDlyHMSM(0,0,(u8)((orderp->size / 512) + (orderp->size / 2048)), 500);
//	DEBUG_PRINT("Print_Order: Waiting for Printer's status.\n");
//	SEND_STATUS_CMD_ONE(cellno);
//	OSTimeDlyHMSM(0,0,1,0);//����״̬����ʱ
//	
//	// ���������Pend��������̷��أ�����ʱ������ӡ�����д������쳣
//	OSSemPend(cellp->printDoneSem, 20, &err);
//	if(err == OS_ERR_TIMEOUT){	// ��ӡ�����г����쳣��ʹ�ô�ӡ������
//		DEBUG_PRINT("Print_Order: Printer %u Off-line while Printing.\n", cellno);
//		needCutPaper[cellno] = 1;
//		Order_Print_Status_Send(orderp, PRINT_CELL_STATUS_ERR);	
//		PutPrintCell(cellno, PRINT_CELL_STATUS_ERR);
//	}

//	return ORDER_DATA_OK;
//}

void Init_Order_Table(void)
{
	u8_t entry_index = 0;
	INT8U  err;
	order_info *orderp;
	
	/*��ʼ����Ϣ���еĻ�������¼*/
	order_print_table.buf_node.common_buf_size = MAXQSIZE;
	order_print_table.buf_node.common_buf_remain_capacity = MAXQSIZE;
	
	order_print_table.buf_node.urgent_buf_size = MAXUSIZE;
	order_print_table.buf_node.urgent_buf_remain_capacity = MAXUSIZE;	
	
	/* ��ʼ�������� */
	order_print_table.mutex = OSMutexCreate(ORDER_PRINT_TABLE_MUTEX_PRIO, &err);
	assert(NULL != order_print_table.mutex, 
		"Init_Order_Table: Create mutex for print table failed.");
	
	/* ��ʼ�����й���ṹ */
	order_print_table.head = 0;   //��ӡ���ж�������
	order_print_table.tail = 0;   //��ӡ���ж�β����
	order_print_table.empty = 1;  //��ӡ�����Ƿ�Ϊ�գ�1����ա�0����ǿ�

	
	/*��ʼ����Ϣ���еĶ�����¼*/
	while(entry_index < ALL_BLOCK_NUM)
	{

		orderp = &order_print_table.order_node[entry_index];

		orderp->data = NULL;
		orderp->size = 0;
		orderp->serial_number = 0;
		orderp->batch_number = 0;
		orderp->batch_within_number = 0;
		orderp->priority = ORDER_PRIO_INVALID;
		orderp->status = 100;
		orderp->mcu_id = 0;
		orderp->next_print_node = 0;
		
		entry_index++;
	}		
}


/**
 * @fn		OrderEnqueue
 * @brief	�������������
 * @param	buf ������ָ��
 *				entry_index ������
 *				order_len ��������
 *				order_prio_sigal ����������ͨ�ı�־
 * @return	void
 */
static s8_t OrderEnqueue(SqQueue* buf,s8_t entry_index , u16_t order_len,u8_t order_prio_sigal)
{		
	extern OS_EVENT *Print_Sem;    
	s8_t print_err = 0;	
	u16_t order_num = 0;
	
	Add_Order_To_Print_Queue(buf,entry_index,order_prio_sigal);												
	//���Ͷ��������ӡ���еı���	
	Order_QUEUE_Status_Send(&(order_print_table.order_node[entry_index]),ENQUEUE_OK	);						
	DEBUG_PRINT("-------ONE ORDER ENQUEUE---------\n");	
	OSSemPost(Print_Sem);//������ӡ�ź�
		
	return 0;
}


/**
 * @fn		error_order_deal
 * @brief	�������󣬴�������
 * @param	buf	 ������
 * @return	
 */
static s8_t error_order_deal(SqQueue* buf, s8_t order_prio_sigal)
{
	u32_t count = 1;
	//�������������
	while(buf->read != buf->write && (buf->base[buf->read] != 0x3e || buf->base[(buf->read+1)%buf->MAX] != 0X11 ))
	{
		buf->read = (buf->read +1) % buf->MAX;
		count++;
	}					
	buf->read = (buf->read +2) % buf->MAX;
  count ++;
	//���»�������
	
	if(order_prio_sigal == 1){//�Ӽ����Ͷ���
		order_print_table.buf_node.urgent_buf_remain_capacity = order_print_table.buf_node.urgent_buf_remain_capacity  + count;
		if(order_print_table.buf_node.urgent_buf_remain_capacity == order_print_table.buf_node.urgent_buf_size)
		{
			buf->buf_empty = 1;
		}
	}
	else{//�ǼӼ����Ͷ���
		order_print_table.buf_node.common_buf_remain_capacity = order_print_table.buf_node.common_buf_remain_capacity  + count;
		if(order_print_table.buf_node.common_buf_remain_capacity == order_print_table.buf_node.common_buf_size)
		{
			buf->buf_empty = 1;
		}
	
	}
	

	
	DEBUG_PRINT("QUEUE_DEBUG : DELETE AN ERROR ORDER! ------\n");
}


/**
 * @fn		Print_Queue_Fun
 * @brief	��ӡ������������
 * @param	
 * @return	
 */
void Print_Queue_Fun()
{
	#if OS_CRITICAL_METHOD == 3u
    OS_CPU_SR  cpu_sr = 0u;
	#endif
	u8_t os_err = 0;
	s8_t print_err = 0;
	u16_t order_len = 0;    //��Ҫ��ӡ�Ķ�������
	s8_t entry_index = 0;
	SqQueue *buf;
	u8_t order_prio_sigal = 0;
	u8_t data_source = 0;
	extern OS_EVENT *Print_Queue_Sem; 
	
	while(1){	
		OSSemPend(Print_Queue_Sem,0,&os_err);
		
		OSMutexPend(urgent_buf.mutex,0,&os_err);		//�������������
		OSMutexPend(queue_buf.mutex,0,&os_err);			//������ͨ������
		
		while(!Is_Empty_Queue(urgent_buf) || !Is_Empty_Queue(queue_buf))
		{
//			//�������7K�жϣ���Ϊÿ���һ�����������п��ܻ��û���������һ�㡣
//			Check_Buf_Request_Signal(queue_buf);
			
			if(!Is_Empty_Queue(urgent_buf)){	//�Ӽ�����
				buf = &urgent_buf;
				order_prio_sigal = 1;
			}
			else{															//��ͨ����
				buf = &queue_buf;
				order_prio_sigal = 0;
			}
			
			Read_Order_Length_Queue(*buf,&order_len);//��ȡ��������			
			DEBUG_PRINT("ORDER DEBUG : Order Length: %u------------\n", order_len);
			
//			�ҵ����������д�Ŷ�����λ��			
//			����ֵ����:		ORDER_TOO_LARGER�� ORDER_FIND_INDEX_ERR
//									��������������ֵ��
//									�����Ǹ�������Ӽ���ȱ���ڴ��				
			entry_index = Find_Entry(order_len,order_prio_sigal);
			DEBUG_PRINT("ORDER DEBUG : Order Index: %d------------\n", entry_index);
			if(entry_index == ORDER_TOO_LARGER){									//������봦������С���������
				DEBUG_PRINT("ORDER_ERR: ORDER_TOO_LARGER\r\n");
				error_order_deal(buf, order_prio_sigal);
			}
			else if( entry_index == ORDER_FIND_INDEX_ERR){				//������С����4K,������û�г�����������ƣ������ڶ�������ƾ�Ϊ4K�����������ϲ��ɴ
				DEBUG_PRINT("ORDER_ERR: ORDER_FIND_INDEX_ERR,  occur error when find the order index \n");	
				error_order_deal(buf , order_prio_sigal);
			}
			else if (entry_index < 0 ){														//������������û�����뵽�ڴ��
				DEBUG_PRINT("ORDER_WARNNING: NON BLOCK FOR UGRGENT ORDER\n");					
				OSSemPost(Print_Queue_Sem);	//��Ϊ�Ӽ�������û��ɾ�������Ի��Ǵ��ڻ�����					
				
				DEBUG_PRINT("QUEUE DEBUG :Before Non block Change\n");
				printOrderQueueSeque();						
				Change_Order_Seque(entry_index);//�ı䶩��˳�����üӼ�����������뵽������)	
				DEBUG_PRINT("QUEUE DEBUG :After  Non block Change\n");
				printOrderQueueSeque();				
				
				OSTimeDlyHMSM(0, 0, 1, 0);
			}
			else{																									//����ȡ������
				DEBUG_PRINT("ORDER_SUCCESS: ENQUEUE SUCCESS \n");					
				OrderEnqueue(buf,entry_index , order_len,order_prio_sigal);
				//�������7K�жϣ���Ϊÿ���һ�����������п��ܻ��û���������һ�㡣
				Check_Buf_Request_Signal(queue_buf);
			}		
		}		
		OSMutexPost(urgent_buf.mutex);		
		OSMutexPost(queue_buf.mutex);		
	}
}


