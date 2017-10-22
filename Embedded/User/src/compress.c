#include "compress.h"

//#define ORIGINAL_PRINT       //ԭͼ��ӡ
#define DOUBLE_PRINT       //˫����ӡ
//#define FOURFLOD_PRINT
//#define SEC_COMPRESS       //����ѹ��

static unsigned char uncompress_map[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

/*
 *�������ã����г̱���ķ�ʽ��λͼ����ѹ��
 *������
				@src ��δѹ��ǰ���ݴ���ڴ��׵�ַ
				@src_end ��δѹ��ǰ���ݴ���ڴ�ĩ��ַ 
				@row : ��ӡ���ݵ�����
				@col : ��ӡ���ݵ�����
				@des : ����ѹ�����ŵ��ڴ��׵�ַ
	*����ֵ:��ӡ����ѹ������ֽ���
 */
unsigned int compress(const unsigned char *src,const unsigned char *src_end,unsigned char row,unsigned char col,unsigned char *des)
{
	volatile unsigned char count = 0x00,map = 0x80;
	unsigned char *des_start = des;
	const unsigned char *src_start = src;
	
	/*ѹ����Ϻ������ͷ�����ֽڴ��������������������ֽ�Ϊѹ����ͼ������*/
	*des++ = row;
	*des++ = col;
#ifdef 	SEC_COMPRESS 
	while(1)
	{
		/*����������1*/
		while((*src) & map)
		{
			if(count++ == 200)    //des����һ���ֽ�
			{
				*des++ = 0x80;
				count = 0;
			}
			else if((map >>= 1) == 0)  //src������һ���ֽ�
			{
				if(++src == src_end+1)    //����src�Ѿ�ת�����,����ѭ��
					break;
				
				map = 0x80;
			}
		}	
		if((count != 0)&&(count <= 127))
		{
			*des++ = 128 + count;
			count = 0;
		}
		else if((count >127)&&(count < 200))
		{
			*des++ = 0xff;
			*des++ = 128 + (count - 127);
			count = 0;
		}
	
		if(src == src_end+1)  //����src�Ѿ�ת�����,����ѭ��
		{
			break;
		}
		/*����������0*/
		while(!((*src) & map))
		{
			if(count++ == 127)    //des����һ���ֽ�
			{
				*des++ = 127;
				count = 0;
			}
			else if((map >>= 1) == 0)  //src������һ���ֽ�
			{
				if(++src == src_end+1)    //����src�Ѿ�ת�����,����ѭ��
					break;
				
				map = 0x80;
			}
		}
		
		if(count != 0)
		{
			*des++ = count;
			count = 0;
		}
		
		if(src == src_end+1)  //����src�Ѿ�ת�����,����ѭ��
		{
			break;
		}
	}
#else
		while(1)
	{
		/*����������1*/
		while((*src) & map)
		{
			if(count++ == 127)    //des����һ���ֽ�
			{
				*des++ = 0xff;
				count = 0;
			}
			else if((map >>= 1) == 0)  //src������һ���ֽ�
			{
				if(++src == src_end+1)    //����src�Ѿ�ת�����,����ѭ��
					break;
				
				map = 0x80;
			}
		}
		
		if(count != 0)
		{
			*des++ = 128 + count;
			count = 0;
		}
		
		
		if(src == src_end+1)  //����src�Ѿ�ת�����,����ѭ��
		{
			break;
		}
		/*����������0*/
		while(!((*src) & map))
		{
			if(count++ == 127)    //des����һ���ֽ�
			{
				*des++ = 127;
				count = 0;
			}
			else if((map >>= 1) == 0)  //src������һ���ֽ�
			{
				if(++src == src_end+1)    //����src�Ѿ�ת�����,����ѭ��
					break;
				
				map = 0x80;
			}
		}
		
		if(count != 0)
		{
			*des++ = count;
			count = 0;
		}
		
		if(src == src_end+1)  //����src�Ѿ�ת�����,����ѭ��
		{
			break;
		}
	}
	
#endif
//	printf("ѹ��ǰ�ֽ�����%d,   ѹ�����ֽ�����%d,   ѹ����Ϊ��%d%%\n",src_end-src_start+1,(des-des_start),100-(int)((float)(des-des_start)/(float)(src_end-src_start+1)*100.0));
	
	return (des-des_start);
}


/*
 *��ѹ����ӡ
 */
void uncompress_print(const unsigned char *src,const unsigned char *src_end, unsigned char deviceNum)
{ 
  unsigned char line_buf[20] = {0x00}; //��Ž�ѹ��һ�е�����
	unsigned char count = 0x00;  //��Ŵ���ѹ���ֽ���
	unsigned char bit_count = 0x00;   //�ֽ��ڲ��ļ���
	const unsigned char row = *src;
	const unsigned char col = *(src+1);
	unsigned char i = 0; //�洢����������
	
		//����λͼ��ӡָ��
#ifdef ORIGINAL_PRINT
							PRINT_FAST_BITMAP(0,col,0,row,0, deviceNum)
#endif
#ifdef DOUBLE_PRINT
							PRINT_FAST_BITMAP(0,col*2,0,row*2,0, deviceNum)
#endif
#ifdef FOURFLOD_PRINT
							PRINT_FAST_BITMAP(0,col*4,0,row*4,0, deviceNum)
#endif 
	
	src += 2;
	while(src!=src_end+1)
	{
		count  = *src & 0x7F;
#ifdef SEC_COMPRESS		
		if(*src == 0x80)
			count = 200;
#endif
		if(*src & 0x80)  //Ϊ1�ĵ�
		{
			
			while(count >0)
			{
				if(bit_count != 0)   //��һ���ֽ��ڻ�����0����1����0��ĩ�˽�����д
				{
					line_buf[i] |=  uncompress_map[bit_count-1];
					bit_count--;
					count--;
					if(!bit_count)
					{
						i++;
						if(i == col)
						{
	#ifdef ORIGINAL_PRINT
							original_print(row,col,line_buf, deviceNum);
	#endif
	#ifdef DOUBLE_PRINT
							double_print(row,col,line_buf, deviceNum);
	#endif
	#ifdef FOURFLOD_PRINT
							fourflod_print(row,col,line_buf, deviceNum);
	#endif 
							i = 0;
						}	
					}
				}
				
				if(!bit_count && count>=8)
				{
						line_buf[i] = 0xFF; 
						i++;
						if(i == col)
						{
	#ifdef ORIGINAL_PRINT
							original_print(row,col,line_buf, deviceNum);
	#endif
	#ifdef DOUBLE_PRINT
							double_print(row,col,line_buf,deviceNum);
	#endif
	#ifdef  FOURFLOD_PRINT
							fourflod_print(row,col,line_buf,deviceNum);
	#endif 
							
							i = 0;
						}	
						
						count -= 8;
				}
				else if(!bit_count && count < 8)
				{
					bit_count = 8;
					
					while(count > 0)
					{
						line_buf[i] |= uncompress_map[bit_count-1];
						bit_count--;
						count--;
					}
				}	
			}
		}
		else   //Ϊ0�ĵ�
		{
			while(count >0)
			{
				if(bit_count != 0)
				{
					line_buf[i] &=  ~uncompress_map[bit_count-1];
					bit_count--;
					count--;
					if(!bit_count)
					{
						i++;
						if(i == col)
						{
	#ifdef ORIGINAL_PRINT
							original_print(row,col,line_buf, deviceNum);
	#endif
	#ifdef DOUBLE_PRINT
							double_print(row,col,line_buf,deviceNum);
	#endif
	#ifdef  FOURFLOD_PRINT
							fourflod_print(row,col,line_buf,deviceNum);
	#endif 
							
							i = 0;
						}	
					}
				}
				
				if(!bit_count && count>=8)
				{
						line_buf[i] = 0x00; 
						i++;
						if(i == col)
						{
	#ifdef ORIGINAL_PRINT
							original_print(row,col,line_buf, deviceNum);
	#endif
	#ifdef DOUBLE_PRINT
							double_print(row,col,line_buf,deviceNum);
	#endif
	#ifdef  FOURFLOD_PRINT
							fourflod_print(row,col,line_buf,deviceNum);
	#endif 
							
							i = 0;
						}	
							count -= 8;
				}
				else if(!bit_count && count < 8)
				{
					bit_count = 8;
					
					while(count > 0)
					{
						line_buf[i] &= ~uncompress_map[bit_count-1];
						bit_count--;
						count--;
					}
				}
					
			}
		}
		src++;
	}
}
