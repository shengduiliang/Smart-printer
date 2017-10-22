#include "RequestHeap.h"

/* �������󶩵� */
static void swapReq(Request *a, Request *b);

/* ����С���� */
static void AdjustHeap(Heap *heap, u8_t pos);

/* ��ɾ���Ѷ����³��滻���һ��Ԫ�صĶѶ� */
void DownHeap(Heap *heap, u8_t index);

/* �������󶩵� */
void swapReq(Request *a, Request *b)
{
	Request temp;
	temp = *a;
	*a = *b;
	*b = temp;
}

/* ����С���� */
void InsertHeap(Heap *heap, Request *req)
{
	heap->req[++heap->count] = *req;//��ʹ�ýڵ�0
	AdjustHeap(heap, heap->count);	//����
}

/* ɾ���Ѷ� */
void DeleteHeap(Heap *heap)
{
	heap->req[1] = heap->req[heap->count--];
	DownHeap(heap, 1);	//���µ���
}

/* ����С���� */
void AdjustHeap(Heap *heap, u8_t pos)
{
	if(pos > 1){
		u8_t parent = pos / 2;	//������ڵ�
		
		if(heap->req[parent].recTime > heap->req[pos].recTime){
			swapReq(&(heap->req[parent]), &(heap->req[pos])); //����
			AdjustHeap(heap, parent);	//�ݹ����
		}
	}
}

/* ��ɾ���Ѷ����³��滻���һ��Ԫ�صĶѶ� */
void DownHeap(Heap *heap, u8_t index)
{
	u8_t count = heap->count;
	u8_t child = 2 * index;	//���У���Ϊ�����ӽڵ�
	
	if(index <= count){	//ӵ�����ҽڵ�ŵ���
		if(2 * index < count){ //���Ҷ��Ӷ���
			if(heap->req[index].recTime > heap->req[index + 1].recTime){
				++child;
			}
		}
		
		if(heap->req[index].recTime > heap->req[child].recTime){
			swapReq(&(heap->req[index]), &(heap->req[child])); //����
			DownHeap(heap, child);	//�ݹ����
		}
	}
}

/* ���¶ѽṹ */
void UpdateHeap(Heap *heap)
{
	if(heap->count > 0){
		heap->flag = WAIT_FOR_BUF;
	}
}

/* ��ʼ���ѽṹ */
void InitHeap(Heap *heap)
{
	heap->count = 0;
	heap->flag = BUF_CAP_FULFILL;
	heap->capcity = MAX_CAPCITY;
}

#define WAIT(heap) ((heap->req[1].orderLen) > (heap->capcity))	//�ȴ���������������

void DealWifiReq(Heap *heap)
{
	INT8U err;
	extern OS_EVENT *Capacity_Change_Sem;	//wifi�����������ı��ź�
	extern OS_EVENT *Rec_Wifi_Req_Sem;		//��ȡ��wifi���������ź�
	InitHeap(heap);
	while(1){
		DEBUG_PRINT("-------heap pend heapSem mutex!-------\n");
		OSSemPend(Rec_Wifi_Req_Sem, 0, &err);
		
		
		/* �жϻ����������Ƿ���� */
		DEBUG_PRINT("-------heap pend heap mutex!-------\n");
		OSMutexPend(heap->mutex, 0, &err);	//����ѿ�����
		{
			if(heap->count > 0){
				DEBUG_PRINT("-------------before wait, req[1].orderlen is %lu, heap capcity is %lu---------\n", 
							heap->req[1].orderLen, heap->capcity);
				while(WAIT(heap)){	//�ȴ���������������
					DEBUG_PRINT("-------------req[1].orderlen is %lu, heap capcity is %lu---------\n",
							heap->req[1].orderLen, heap->capcity);
					DEBUG_PRINT("-------heap post heap mutex in wait!-------\n");
					OSMutexPost(heap->mutex);			//�ͷŶѿ�����
					DEBUG_PRINT("-------heap pend capcitySem mutex in wait!-------\n");
					OSSemPend(Capacity_Change_Sem, 0, &err);
					DEBUG_PRINT("-------heap pend heap mutex in wait!-------\n");
					OSMutexPend(heap->mutex, 0, &err);	//����ѿ�����
				}
			}
			
			SendStatusToWifi(wifi_order_req, 0, heap->req[1].ipAdd, 0);	//����Ӧ����wifi�ˣ���Ϊ���µ�
			
			
			heap->capcity -= heap->req[1].orderLen;
			DEBUG_PRINT("-------------After rec, heap capcity is %lu---------\n", heap->capcity);
			DeleteHeap(heap);	//ɾ���Ѷ�
		}
		DEBUG_PRINT("-------heap post heap mutex in wait!-------\n");
		OSMutexPost(heap->mutex);	//�ͷŶѿ�����
	}
}