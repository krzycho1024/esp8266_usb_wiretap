#include "esp_common.h"
#include "espconn.h"

LOCAL struct espconn esp_conn;
LOCAL esp_tcp esptcp;

#define SERVER_LOCAL_PORT   1112

LOCAL void tcp_server_multi_send(unsigned char *out)
{
   struct espconn *pesp_conn = &esp_conn;

   remot_info *premot = NULL;
   uint8 count = 0;
   sint8 value = ESPCONN_OK;
   if (espconn_get_connection_info(pesp_conn,&premot,0) == ESPCONN_OK){
      for (count = 0; count < pesp_conn->link_cnt; count ++){
         pesp_conn->proto.tcp->remote_port = premot[count].remote_port;
         
         pesp_conn->proto.tcp->remote_ip[0] = premot[count].remote_ip[0];
         pesp_conn->proto.tcp->remote_ip[1] = premot[count].remote_ip[1];
         pesp_conn->proto.tcp->remote_ip[2] = premot[count].remote_ip[2];
         pesp_conn->proto.tcp->remote_ip[3] = premot[count].remote_ip[3];

		 espconn_sent(pesp_conn, out, 16);
      }
   }
}

LOCAL void ICACHE_FLASH_ATTR
tcp_server_listen(void *arg)
{
    struct espconn *pesp_conn = arg;
    os_printf("tcp_server_listen !!! \r\n");

	espconn_set_opt(pesp_conn, ESPCONN_NODELAY);
	espconn_set_opt(pesp_conn, ESPCONN_COPY);
}

void ICACHE_FLASH_ATTR
user_tcpserver_init(uint32 port)
{
    esp_conn.type = ESPCONN_TCP;
    esp_conn.state = ESPCONN_NONE;
    esp_conn.proto.tcp = &esptcp;
    esp_conn.proto.tcp->local_port = port;
    espconn_regist_connectcb(&esp_conn, tcp_server_listen);

    sint8 ret = espconn_accept(&esp_conn);
}

LOCAL os_timer_t test_timer;
