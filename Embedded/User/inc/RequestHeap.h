#ifndef REQUEST_HEAP_H
#define REQUEST_HEAP_H

/**************************************************************
*	Include File Section
**************************************************************/
#include "cc.h"
#include "more_infomation.h"
#include "data_form.h"
#include "analyze_data.h"

/* ��������ṹ�� */
typedef struct request{
	u32_t orderLen;	//���󶩵�����
	u32_t recTime;	//��������ʱ��
	u32_t ipAdd;	//�ƶ���ip��ַ
}Request;


/* С���ѽṹ�� */
typedef struct heap{
	u8_t flag;		//���������㶩��������־
	u8_t count;		//������Ŀ
	u32_t capcity;	//��ǰ������ʣ������
	struct os_event *mutex;	//�ѽṹ���ƻ�����
#define REQ_MAX_SIZE 50
	Request req[REQ_MAX_SIZE];	//��������ṹ������
}Heap;



/**************************************************************
*	Macro Define Section
**************************************************************/
#define WAIT_FOR_BUF 		0x01
#define BUF_CAP_FULFILL  	0x00
#define MAX_CAPCITY			(1024 * 10)

/**************************************************************
*	Function Define Section
**************************************************************/
/* ����С���� */
void InsertHeap(Heap *heap, Request *req);

/* ���¶ѽṹ */
void UpdateHeap(Heap *heap);

/* ɾ���Ѷ� */
void DeleteHeap(Heap *heap);

/* ��ʼ���ѽṹ */
void InitHeap(Heap *heap);

/* ����wifi�������� */
void DealWifiReq(Heap *heap);
#endif //REQUEST_HEAP_H~