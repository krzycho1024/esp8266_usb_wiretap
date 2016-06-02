#include "esp_common.h"
#include "gpio.h"
#include "tcp_server.c"

static inline unsigned get_ccount(void)
{
	unsigned r;
	asm volatile ("rsr %0, ccount" : "=r"(r));
	return r;
}

enum UsbState
{
	SE0,
	J,
	K,
	SE1,
};

static inline enum UsbState getState(int status)
{
	status = status & 0x3000;
	if (status == 0x1000)
	{
		return K;
	}
	else if (status == 0x2000)
	{
		return J;
	}
	else if (!status)
	{
		return SE0;
	}
	else
	{
		return SE1;
	}
}

void usb_handler()
{
	//system_update_cpu_freq(160);

	//GPIO_OUTPUT_SET(14, 1);
	int i = 0, pos = 1;
	unsigned int stateTick[128];
	char state[128];

	state[0] = J;
	state[1] = J;

	while (state[0] != SE0 || state[1] != SE0)
	{
		state[1] = state[0];
		state[0] = getState(GPIO_REG_READ(GPIO_IN_ADDRESS));
	}

	for (i = 0; i < 600 && pos < 128; i++)
	{
		state[pos] = getState(GPIO_REG_READ(GPIO_IN_ADDRESS));
		stateTick[pos] = get_ccount();
		if (state[pos] != state[pos - 1])
		{
			pos++;
		}
	}

	unsigned char out[16];
	for (i = 0; i < 16; i++)
	{
		out[i] = 0;
	}

	for (i = 1; i < pos - 1; i++)
	{
		int st = stateTick[i + 1] - stateTick[i];
		if (st < 53 && (state[i] == SE0 || state[i] == SE1))
		{
			stateTick[i] += st / 2;
			stateTick[i + 1] = stateTick[i];
		}
	}

	for (i = 1; i < pos;i++)
	{
		if (state[i] == K)
		{
			break;
		}
	}

	int ctr;
	int bit;
	int c = 0;
	int oneCount = 0;
	int pr = 0;
	int ctrState = 0;
	for (; i < pos - 1; i++)
	{
		ctr = stateTick[i + 1] - stateTick[i];

		while (ctr > 53)
		{
			ctr -= 106;

			if (state[i] == K)
			{
				bit = 1;
			}
			else if (state[i] == J)
			{
				bit = 0;
			}
			else
			{
				i = pos;
				break;
			}

			int p;
			if (!pr && bit)
			{
				p = 0;
			}
			else if (!pr)
			{
				p = 1;
			}
			else if (!bit)
			{
				p = 0;
			}
			else
			{
				p = 1;
			}
			pr = bit;
			if (p == 1)
			{
				oneCount++;
			}
			else
			{
				oneCount = 0;
			}
			if (oneCount == 6)
			{
				oneCount = 0;
			}
			else
			{
				out[c / 8] += p << (c % 8);
				c++;
			}
		}
	}
	//GPIO_OUTPUT_SET(14, 0);

	//if ((out[0] == 128 && out[1] != 90) || out[0] != 128)
	//{
	//	printf("d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", out[0], out[1], out[2], out[3], out[4], out[5], out[6], out[7], out[8], out[9], out[10], out[11], out[12], out[13], out[14], out[15]);
	//}
	//printf("d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", out[0], out[1], out[2], out[3], out[4], out[5], out[6], out[7], out[8], out[9], out[10], out[11], out[12], out[13], out[14], out[15]);

	//if (out[0] == 128 && (out[1] == 195 || out[1] == 75))
	//{
		//espconn_sent(client, out, 16);
	if (out[0] == 128 && out[1] == 90)
	{
		return;
	}
	if(c==0)
	{
		return;
	}

	if (out[0] == 128 && (out[1] == 195 || out[1] == 75))
	{
		//printf("d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", out[0], out[1], out[2], out[3], out[4], out[5], out[6], out[7], out[8], out[9], out[10], out[11], out[12], out[13], out[14], out[15]);
		tcp_server_multi_send(out);
	}
}

void main_int_handler()
{
	system_update_cpu_freq(160);
	uint32 gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
	//if ((gpio_status & BIT(12)))
	//{
		//printf("triggered on int: %d\n", i);
		//disable interrupt
		gpio_pin_intr_state_set(GPIO_ID_PIN(12), GPIO_PIN_INTR_DISABLE);
		// call func here

		usb_handler();
		system_update_cpu_freq(80);

		//clear interrupt status
		GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status & BIT(12));

		// restore
		gpio_pin_intr_state_set(GPIO_ID_PIN(12), GPIO_PIN_INTR_ANYEDGE);
	//}
}

#define WIFI_APSSID	"ESP8266"
#define WIFI_APPASSWORD	"12345678"
static char macaddr[6];

void user_init(void)
{
	system_update_cpu_freq(80);

	//_xt_isr_unmask(1 << ETS_GPIO_INUM);

	//gpio_intr_handler_register(main_int_callback);
	////gpio_intr_handler_register(main_int_callback);
	//registerInterrupt(12, GPIO_PIN_INTR_ANYEDGE);
	//registerInterrupt(2, GPIO_PIN_INTR_ANYEDGE);

	//read_input();
	//xTaskCreate(readInput, "read_input", 256, NULL, 2, NULL);

	GPIO_ConfigTypeDef gpio_in_cfg;                                    //Define GPIO Init Structure
	gpio_in_cfg.GPIO_IntrType = GPIO_PIN_INTR_ANYEDGE;                 //Falling edge trigger
	gpio_in_cfg.GPIO_Mode = GPIO_Mode_Input;                           //Input mode
	gpio_in_cfg.GPIO_Pin = GPIO_Pin_12;                               // Enable GPIO
	gpio_config(&gpio_in_cfg);                                         //Initialization function

	gpio_in_cfg.GPIO_IntrType = GPIO_PIN_INTR_DISABLE;                 //Falling edge trigger
	gpio_in_cfg.GPIO_Mode = GPIO_Mode_Input;                           //Input mode
	gpio_in_cfg.GPIO_Pin = GPIO_Pin_13;                               // Enable GPIO
	gpio_config(&gpio_in_cfg);                                         //Initialization function

	//gpio_in_cfg.GPIO_IntrType = GPIO_PIN_INTR_DISABLE;                 //Falling edge trigger
	//gpio_in_cfg.GPIO_Mode = GPIO_Mode_Output;                           //Input mode
	//gpio_in_cfg.GPIO_Pin = GPIO_Pin_14;                               // Enable GPIO
	//gpio_config(&gpio_in_cfg);

	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, GPIO_Pin_12);
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, GPIO_Pin_13);
	//GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, GPIO_Pin_14);
	gpio_intr_handler_register(main_int_handler, NULL);                   // Register the interrupt function
	_xt_isr_unmask(1 << ETS_GPIO_INUM);                                //Enable the GPIO interrupt

	//GPIO_OUTPUT_SET(12, 0);
	//GPIO_OUTPUT_SET(13, 0);
	//GPIO_OUTPUT_SET(14, 0);

	if (wifi_get_opmode() != SOFTAP_MODE)
	{
		wifi_set_opmode(SOFTAP_MODE);
	}

	if(!wifi_softap_dhcps_stop()) {
		printf("Failed to wifi_softap_dhcps_stop()");
	}

	struct softap_config apConfig;
	struct ip_info ipinfo;

	char ssid[33];
	char password[33];

	wifi_get_macaddr(SOFTAP_IF, macaddr);
	wifi_softap_get_config(&apConfig);
	memset(apConfig.ssid, 0, sizeof(apConfig.ssid));
	sprintf(ssid, "%s", WIFI_APSSID);
	memcpy(apConfig.ssid, ssid, strlen(ssid));

	memset(apConfig.password, 0, sizeof(apConfig.password));
		sprintf(password, "%s", WIFI_APPASSWORD);
		memcpy(apConfig.password, password, strlen(password));
		apConfig.authmode = AUTH_WPA2_PSK;
		apConfig.channel = 7;
		apConfig.max_connection = 255;
		apConfig.ssid_hidden = 0;
		apConfig.beacon_interval = 100;
		if (!wifi_softap_set_config(&apConfig)) {
			printf("Failed to wifi_softap_set_config()");
		}
	
	IP4_ADDR(&ipinfo.ip, 10, 10, 10, 1);
	IP4_ADDR(&ipinfo.gw, 10, 10, 10, 1);
	IP4_ADDR(&ipinfo.netmask, 255, 255, 255, 0);
	espconn_dns_setserver(0, &ipinfo.ip);
	if (!wifi_set_ip_info(SOFTAP_IF, &ipinfo)) {
		printf("Failed to wifi_set_ip_info()");
	}

	if (!wifi_set_phy_mode(PHY_MODE_11G))
	{
		printf("Failed to wifi_set_phy_mode()");
	}
	
	if (!wifi_softap_dhcps_start())
	{
		printf("Failed to wifi_softap_dhcps_start()");
	}
		



	printf("SDK version:%s\n", system_get_sdk_version());
	printf("Time:%ld\n", system_get_time());
	printf("Chip id = 0x%x\n", system_get_chip_id());
	printf("CPU freq = %d MHz\n", system_get_cpu_freq());
	printf("Free heap size = %d\n", system_get_free_heap_size());
	printf("mode: %d\n", wifi_get_opmode());


	user_tcpserver_init(1080);

	//GPIO_OUTPUT_SET(14, 0);
}

