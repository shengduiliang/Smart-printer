#include"print_bmp.h"
#include "printerException.h"


/* 
 *�������ã�ԭͼ��ӡ
 *����������
		@row : ��ӡλͼ������
		@col : ��ӡλͼ������
		@buf : λͼ��һ�е�����
  *��������ֵ����
 */
void original_print(const unsigned int row,const unsigned int col,unsigned char *buf, unsigned char deviceNum)
{
	unsigned char original_buf[25] = {0x00};
	unsigned int i = 0;
	
	for(i=0; i<col; i++)
	{
		PRINTER_PUT(*(buf+i), deviceNum);
	}	
}

	
/* 
 *�������ã�˫���Ŵ�
 *����������
		@row : ��ӡλͼ������
		@col : ��ӡλͼ������
		@buf : λͼ��һ�е�����
  *��������ֵ����
 */
void double_print(const unsigned int row,const unsigned int col,unsigned char *buf, unsigned char deviceNum)
{
	unsigned short amplify_buf[25] = {0x00};
	unsigned char map = 0x00;  //����
	unsigned int i = 0x00; //���ֽ���չ���м����������ӡ
	unsigned int j = 0x00; //���ߴ�ӡ
	unsigned char *p = (unsigned char *)amplify_buf;   //��amplify_buf�����ֽ�����
	
	for(i=0;i<col;i++)
	{
		for(map=0x80;map!=0x00;map>>=1)
		{
			amplify_buf[i] <<= 2;
			if((*buf & map) != 0x00)
			{
				amplify_buf[i] |= 0x03;
			}
		}
		buf++;
	}
	for(j = 0; j<2; j++)
	{
		for(i=0; i<col*2; i+=2)
		{
			PRINTER_PUT(*(p+i+1), deviceNum);
			PRINTER_PUT(*(p+i), deviceNum);
			
//			putchar(*(p+i+1));
//			putchar(*(p+i));
			
		}	
		p = (unsigned char *)amplify_buf;
	}

}

/* 
 *�������ã��ı��Ŵ�
 *����������
		@row : ��ӡλͼ������
		@col : ��ӡλͼ������
		@buf : λͼ��һ�е�����
		@deviceNum:
  *��������ֵ����
 */
void fourflod_print(const unsigned char row,const unsigned char col,unsigned char *buf, unsigned char deviceNum)
{
	unsigned int four_amplify_buf[25] = {0x00};
	unsigned char map = 0x00;  //����
	unsigned char i = 0x00; //���ֽ���չ���м����4������ӡ
	unsigned char j = 0x00; //4���ߴ�ӡ
	unsigned char *p = (unsigned char *)four_amplify_buf;   //��amplify_buf�����ֽ�����
	

	for(i=0;i<col;i++)
	{
		for(map=0x80;map!=0x00;map>>=1)
		{
			four_amplify_buf[i] <<= 4;
			if((*buf & map) != 0x00)
			{
				four_amplify_buf[i] |= 0x0f;
			}
		}
		buf++;
	}
	for(j = 0; j<4; j++)
	{
		for(i=0; i<col*4; i+=4)
		{
			PRINTER_PUT(*(p+i+3), deviceNum);
			PRINTER_PUT(*(p+i+2), deviceNum);
			PRINTER_PUT(*(p+i+1), deviceNum);
			PRINTER_PUT(*(p+i), deviceNum);
			
//			putchar(*(p+i+3));
//			putchar(*(p+i+2));
//			putchar(*(p+i+1));
//			putchar(*(p+i));
		}	
		p = (unsigned char *)four_amplify_buf;
	}
}

