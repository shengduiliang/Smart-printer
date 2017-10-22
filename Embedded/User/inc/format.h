#ifndef _FORMAT_H
#define _FORMAT_H


/*==========��ӡ�������뷽ʽ==============*/
#define LEFT_ALIGN(deviceNum) 	PRINTER_PUT(0x1B, deviceNum);PRINTER_PUT(0x61, deviceNum);PRINTER_PUT(0x00, deviceNum);
#define CENTER_ALIGN(deviceNum) PRINTER_PUT(0x1B, deviceNum);PRINTER_PUT(0x61, deviceNum);PRINTER_PUT(0x01, deviceNum);
#define RIGHT_ALIGN(deviceNum)  PRINTER_PUT(0x1B, deviceNum);PRINTER_PUT(0x61, deviceNum);PRINTER_PUT(0x02, deviceNum);
#define CANCEL_ALIGN(deviceNum) PRINTER_PUT(0x1B, deviceNum);PRINTER_PUT(0x61, deviceNum);PRINTER_PUT(0x03, deviceNum);

/*===========��ֽ����======================*/
#define CUT_PAPER(deviceNum)		PRINTER_PUT(0x1B, deviceNum);PRINTER_PUT(0x69, deviceNum);
#define CUT_PART_PAPER					PRINTER_PUT(0x1B, deviceNum);PRINTER_PUT(0x6d, deviceNum);

/*===========��ֽ==========================*/
#define PRINT(deviceNum) 							PRINTER_PUT(0x0a, deviceNum);
#define PRINTN(n, deviceNum) 					PRINTER_PUT(0x1b, deviceNum);PRINTER_PUT(0x64, deviceNum);PRINTER_PUT(n, deviceNum);           //��ӡ����ֽn��
#define PRINTN_BACK_DOT(n, deviceNum) PRINTER_PUT(0x1b, deviceNum);PRINTER_PUT(0x6a, deviceNum);PRINTER_PUT(n, deviceNum);

/*===========�����ַ�����ͱ���=====================*/
#define DOUBLE_WIDTH(deviceNum)  						PRINTER_PUT(0x1c, deviceNum);PRINTER_PUT(0x21, deviceNum);PRINTER_PUT(0x04, deviceNum);
#define DOUBLE_HIGH(deviceNum) 							PRINTER_PUT(0x1c, deviceNum);PRINTER_PUT(0x21, deviceNum);PRINTER_PUT(0x08, deviceNum);
#define DOUBLE_WIDTH_HIGH(deviceNum) 				PRINTER_PUT(0x1c, deviceNum);PRINTER_PUT(0x21, deviceNum);PRINTER_PUT(0x0c, deviceNum);
#define CANCEL_DOUBLE_WIDTH_HIGH(deviceNum) PRINTER_PUT(0x1c, deviceNum);PRINTER_PUT(0x21, deviceNum);PRINTER_PUT(0x01, deviceNum);

/*===========�趨�����ַ����ұ߿�====================*/
#define LEFT_RIGHT_SPACE(n1,n2, deviceNum) 	PRINTER_PUT(0x1c, deviceNum);PRINTER_PUT(0x53, deviceNum);PRINTER_PUT(nl, deviceNum);PRINTER_PUT(nh, deviceNum);

/*===================�趨��߽�===================*/
#define LEFT_EDGE(nl,nh,deviceNum) PRINTER_PUT(0x1d, deviceNum);PRINTER_PUT(0x4c, deviceNum)PRINTER_PUT(nl, deviceNum);PRINTER_PUT(nh, deviceNum);

/*===========�����ַ���ӡ��ʽ========================*/
#define DOUBLE_CHAR_WIDTH(deviceNum) 	  						PRINTER_PUT(0x1b, deviceNum);PRINTER_PUT(0x21, deviceNum);PRINTER_PUT(0x20, deviceNum);
#define DOUBLE_CHAR_HIGH(deviceNum) 	 							PRINTER_PUT(0x1b, deviceNum);PRINTER_PUT(0x21, deviceNum);PRINTER_PUT(0x10, deviceNum);
#define DOUBLE_CHAR_WIDTH_HIGH(deviceNum) 				 	PRINTER_PUT(0x1b, deviceNum);PRINTER_PUT(0x21, deviceNum);PRINTER_PUT(0x30, deviceNum);
#define CANCEL_CHAR_DOUBLE_WIDTH_HIGH(deviceNum) 	 	PRINTER_PUT(0x1b, deviceNum);PRINTER_PUT(0x21, deviceNum);PRINTER_PUT(0x01, deviceNum);

/*==========�����ַ��౶�Ŵ�==========================*/
#define RESIZE(n1,n2,deviceNum)  PRINTER_PUT(0x1d, deviceNum);PRINTER_PUT(0x21, deviceNum);PRINTER_PUT(n1<<4|n2, deviceNum);     //n1Ϊˮƽ������n2Ϊ��ֱ����


/*=========�趨�м����ַ����=========================*/
#define	VERTICAL_SAPCE(n,deviceNum) 	PRINTER_PUT(0x1b, deviceNum);PRINTER_PUT(0x33, deviceNum);PRINTER_PUT(n, deviceNum);
#define	CHAR_SAPCE(n,deviceNum) 			PRINTER_PUT(0x1b, deviceNum);PRINTER_PUT(0x20, deviceNum);PRINTER_PUT(n, deviceNum);

/*=========�趨���/���Դ�ӡλ��========================*/
#define RELATIVE_POSITION(NH,NL,deviceNum) 		PRINTER_PUT(0x1b, deviceNum);PRINTER_PUT(0x5c, deviceNum);PRINTER_PUT(NL, deviceNum);PRINTER_PUT(NH, deviceNum);
#define ABOLUTE_POSITION(NH,NL,deviceNum) 		PRINTER_PUT(0x1b, deviceNum);PRINTER_PUT(0x24, deviceNum);PRINTER_PUT(NL, deviceNum);PRINTER_PUT(NH, deviceNum);

/*================��ʼ��=================================*/
#define INIT(deviceNum) PRINTER_PUT(0x1b, deviceNum);PRINTER_PUT(0x40, deviceNum);

/*===============λͼ��ӡ����==============================*/
#define PRINT_BITMAP(m,nl,nh,deviceNum)  PRINTER_PUT(0x1B, deviceNum);PRINTER_PUT(0x2A, deviceNum);PRINTER_PUT(m, deviceNum);PRINTER_PUT(nl, deviceNum);PRINTER_PUT(nh, deviceNum);  //��ͨ��ӡ��ͼ
#define PRINT_FAST_BITMAP(m,xl,xh,yl,yh,deviceNum)  PRINTER_PUT(0x1D, deviceNum); PRINTER_PUT(0x76, deviceNum);PRINTER_PUT(0x30, deviceNum);PRINTER_PUT(m, deviceNum); PRINTER_PUT(xl, deviceNum); PRINTER_PUT(xh, deviceNum); PRINTER_PUT(yl, deviceNum); PRINTER_PUT(yh, deviceNum);

///*==============��������============================*/
//#define BAR_CODE_WIDE(n) putchar(0x1D);putchar(0x77);putchar(n);

///*==============��ӡ��ά��===========================*/
//#define QRCODE(p,n) printf("\x1d\x01\x03\x0A");printf("\x1d\x01\x04\x34");printf("\x1d\x01\x01");putchar(n%256);putchar(n/256);\
//printf("%s",p);printf("\x1d\x01\x02");putchar(0x0a);

#endif
