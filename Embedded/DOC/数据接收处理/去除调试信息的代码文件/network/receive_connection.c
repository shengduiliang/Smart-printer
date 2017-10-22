/**
 * @name        void receive_connection(struct netconn *conn)
 * @brief       �����绺������ȡ���ݲ��������Ƿ���һ����ȷ�����Σ���������Ƿ�Ϊ�Ӽ�������Ӧ�ı��ػ�������������
 * @parameter   struct netconn *conn:ȫ�ֵ�TCP����
 * @description ���øú������߳����ȼ�Ӧ���ڽ����ӡ�����̵߳����ȼ���Ŀ����Ϊ�˱�֤��������������
 */
void receive_connection(struct netconn *conn)
{
    struct netbuf *order_netbuf = NULL;
    extern OS_EVENT *Print_Queue_Sem;
    extern OS_EVENT *Recon_To_Server_Sem;
    err_t err;
    u8_t hash;//���ι�ϣֵ

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

            while(len > 0 && batch_head_current_len < MAX_BATCH_HEAD_LENGTH)
            {
                if(batch[0] != '\xaa'){//һֱ��Ѱ��������ͷ������һֱѰ��
                    find_substr_head(&data, "\xaa\x55", &len, 2);
                }

                if(len > 0){//len����0�����⵽����ͷ����dataָ�뻹ָ��Ļ���������ʣ������
                    batch[batch_head_current_len++] = *data++;
                    count++;                    
                    if(batch_head_current_len == MAX_BATCH_HEAD_LENGTH){
                        if(*(batch + BATCH_TAIL_OFFSET) == '\x55' && *(batch + BATCH_TAIL_OFFSET + 1) == '\xaa'){//��ʾ������β
                            int i;
                            flag = 1;
                            ANALYZE_DATA_2B((batch + BATCH_NUMBER_OFFSET), batch_number);//��ȡ���κ�
                            Analyze_Batch_Info_Table(batch, batch_number);//���ν��

                            hash = get_batch_hash(batch_number);
                            len -= count;

                            batch_total_len = 20 + len;
                            if (batch_total_len >= batch_info_table[hash].batch_length){
                                u16_t sub_len;
                                sub_len =  len - (batch_total_len - batch_info_table[hash].batch_length);//�����绺������ȡ������һ�����ε����ݣ���ֻ����ǰ�������

                                if(batch_number != last_batch_number){
                                    put_in_buf(data, sub_len, batch_info_table[hash].preservation);//���涩��������������

                                    /*�ͷ��ź�����֪ͨ��ӡ�������µ���������*/
                                    OSSemPost(Print_Queue_Sem);
                                    OSSemPost(Batch_Rec_Sem);
                                }

                                begin_new_batch();
                                leave_len = len - sub_len;                  

                                NON_BASE_SEND_STATUS(batch_status, BATCH_ENTER_BUF, batch_number);//��������״̬�����뻺����

                                if(leave_len > 0)//����ʣ��������������ؼ���Ƿ���һ���µ�����
                                    goto start;
                                else
                                    break;

                            }else{
                                if(batch_number != last_batch_number)
                                    put_in_buf(data, len, batch_info_table[hash].preservation);//���涩��������������
                            }
                        }else{
                            begin_new_batch();
                        }
                    }
                    if(count == len){//���ݶ�ȡ�꣬��������
                        count = 0;
                        len = 0;
                        break;
                    }
                }
            }
            if(!(netbuf_next(order_netbuf) > 0)){
                goto delete;
            }
        }

        do
        {
            if(flag == 0)//1��ʾ�Ѿ���ȡ������ͷ������û��
                goto start;

            netbuf_data(order_netbuf, (void **)&data, &len);   
            batch_total_len += len;

            if (batch_total_len >= batch_info_table[hash].batch_length){
                u16_t sub_len;
                sub_len =  len - (batch_total_len - batch_info_table[hash].batch_length);//�����绺������ȡ������һ�����ε����ݣ���ֻ����ǰ�������

                if(batch_number != last_batch_number){
                    put_in_buf(data, sub_len, batch_info_table[hash].preservation);//���涩��������������

                    /*�ͷ��ź�����֪ͨ��ӡ�������µ���������*/
                    OSSemPost(Print_Queue_Sem);
                    OSSemPost(Batch_Rec_Sem);
                }
                begin_new_batch();
                NON_BASE_SEND_STATUS(batch_status, BATCH_ENTER_BUF, batch_number);//��������״̬�����뻺����
                leave_len = len - sub_len;                                                          
                if(leave_len > 0)//����ʣ��������������ؼ���Ƿ���һ���µ�����
                    goto start;
            }
            else{
                if(batch_number != last_batch_number)
                    put_in_buf(data, len, batch_info_table[hash].preservation);//������ͨ����������������
            }
        }while(netbuf_next(order_netbuf) > 0);
delete:
        netbuf_delete(order_netbuf);
    }
}

