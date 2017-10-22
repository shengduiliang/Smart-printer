/***************************************************************************************
*	File Name              :	print_cells.h
* 	CopyRight              :	gaunthan
*	ModuleName             :
*
*	CPU                    :    
*	RTOS                   :    
*
*	Create Data            :
* 	Author/Corportation    :   
*
*	Abstract Description   :
*
*--------------------------------Revision History--------------------------------------
*       No      version     Date        Revised By      Item        Description
*       1       v1.0        2016/       gaunthan	                Create this file
*
***************************************************************************************/

/**************************************************************
*        Multi-Include-Prevent Section
**************************************************************/
#ifndef PRINT_CELLS_H
#define PRINT_CELLS_H

/**************************************************************
*        Debug switch Section
**************************************************************/


/**************************************************************
*        Include File Section
**************************************************************/
#include "cc.h"
#include "ucos_ii.h"
#include <stdlib.h>
#include "print_queue.h"
#include "status_mesg.h"

/**************************************************************
*        Macro Define Section
**************************************************************/
#define MAX_CELL_NUM	2	/* ����ӡ��Ԫ������ӦС�ڵ���4 */

/**
 * ��ӡ��Ԫ��Ŷ���
 */
#define PRINT_CELL_NUM_ONE		1
#define PRINT_CELL_NUM_TWO		2
#define PRINT_CELL_NUM_THREE	3
#define PRINT_CELL_NUM_FOUR		4

/**
 * �����建����״̬����
 */
#define Xoff    0x13           //�����建������
#define Xon     0x11           //�����建������


/**************************************************************
*        Struct Define Section
**************************************************************/
typedef u8_t PrintCellNum;	//��ӡ��Ԫ�������
typedef s8_t OrderEntry;	//������Ϣ�ṹ���

typedef enum {
	PRINT_CELL_STATUS_IDLE,		//����
	PRINT_CELL_STATUS_BUSY,		//æµ
	PRINT_CELL_STATUS_ERR		//������
}PrintCellStatus;	/* ��ӡ��Ԫ״̬���� */

typedef struct {

	PrintCellNum no;					/* ��Ԫ��� */
	OrderEntry entryIndex;				/* ��ָ���� */
	PrintCellStatus status;				/* ָʾ��ӡ��Ԫ��״̬ */
	u8_t health_status;
	
	u32_t totalTime;					// ��ӡ��ʱ������λΪ0.1s	
	u32_t cutCnt;						// ���е�����
	u32_t totalLength;					// �ܴ�ӡ����
	u8_t exceptCnt[EXCEPTION_NUM+1];	// �쳣����Ϸ�������

	
	u32_t workedTime;					// ������ӡʱ������λΪ0.1s
	u32_t beginTick;					// һ�δ�ӡ��ʼʱ�ĵδ�
	u32_t endTick;						// һ�δ�ӡ����ʱ�ĵδ�	
	
	OS_EVENT *printBeginSem;			// ��Ǹô�ӡ��Ԫ�Ĵ�ӡ�߳��Ƿ���Ҫ��ʼ����
	OS_EVENT *printDoneSem;				// ��Ǹô�ӡ��Ԫ�Ĵ�ӡ�߳��Ƿ�����˴�ӡ����
	
}PrintCellInfo;	/* ��ӡ��Ԫ���ݽṹ */

typedef struct {
	OS_EVENT *resrcSem;					//��ǿ��õĴ�ӡ��Ԫ��Դ��	
	PrintCellInfo cells[MAX_CELL_NUM];	//��ʹ�õĴ�ӡ��Ԫ��
}PrintCellsMgrInfo;	/* ��ӡ��Ԫ����ṹ */

/**************************************************************
*        Prototype Declare Section
**************************************************************/
/**
 *  @fn		ReadPrintCellsInfo
 *	@brief	��Ƭ��ROM��ȡ���ָ����д�ӡ��Ԫ����Ϣ
 *	@param	None
 *	@ret	None
 */
void ReadPrintCellsInfo(void);


/**
 *  @fn		WritePrintCellInfo
 *	@brief	����ӡ��Ԫ����Ϣд��ROM
 *	@param	None
 *	@ret	None
 */
void WritePrintCellInfo(PrintCellNum no);


/**
 *  @fn		InitRestoreOrderQueue
 *	@brief	��ʼ���ָ�������
 *	@param	None
 *	@ret	None
 */
void InitRestoreOrderQueue(void);


/**
 *  @fn		RestoreOrder
 *	@brief	��ѯ�Ƿ��д��ָ���ӡ���������������ŷ�����entryp��ָ�ڴ�
 *	@param	entryp ��Ŵ��ָ������ı��
 *	@ret	1 �д��ָ���ӡ����
			0 �޴��ָ���ӡ����
 */
int GetRestoredOrder(u8_t *entryp);


/**
 *  @fn		InitPrintCellsMgr
 *	@brief	��ʼ��PrintCellsManager�Լ���ӡ��Ԫ
 *	@param	None
 *	@ret	None
 */
void InitPrintCellsMgr(void);


/**
 *  @fn		DispensePrintJob
 *	@brief	����ӡ����������ӡ��Ԫ����ִ�д�ӡ
 *	@param	
			entryIndex ���ӡ���������
 *	@ret	None
 */
void DispensePrintJob(u8_t entryIndex);


/**
 *  @fn		PutPrintCell
 *	@brief	�����Ϊno�Ĵ�ӡ��Ԫ����Ϊ���У�������黹������
 *	@param	no ��ӡ��Ԫ���
			status ��ӡ��Ԫ��״̬
 *	@ret	None
 */
void PutPrintCell(PrintCellNum no, PrintCellStatus status);

/**
 *  @fn		OutputErrorTag
 *	@brief	��ӡ������
 *	@param	no ��ӡ��Ԫ���
			status ��ӡ��Ԫ��״̬
 *	@ret	None
 */
void OutputErrorTag(PrintCellNum cellno);


/**
 * UART Receive Interrupt Hook Define 
 */
void USART1_Hook(void);
void UART4_Hook(void);

/**
 * DMA Transmission complete Hook Define 
 */
void USART1_DMA_TC_Hook(void);
void USART2_DMA_TC_Hook(void);


extern PrintCellsMgrInfo PCMgr;

/**************************************************************
*        End-Multi-Include-Prevent Section
**************************************************************/
#endif