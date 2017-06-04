/*
 * Copyright (c) 2007-2009 Xilinx, Inc.  All rights reserved.
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION.
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#include <stdio.h>
#include <string.h>

#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwipopts.h"

#include "config_apps.h"
#ifdef __arm__
#include "xil_printf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "xadcps.h"
#include "xil_types.h"
#endif

#define XPAR_AXI_XADC_0_DEVICE_ID 0
static XAdcPs XADCMonInst;
u16_t echo_port = 7;


void print_echo_app_header()
{
 //   xil_printf("%20s %6d %s\r\n", "echo server",
	printf("%20s %6d %s\r\n", "echo server",
                        echo_port,
                        "$ telnet <board_ip> 7");

}

/* thread spawned for each connection */
void process_echo_request(void *p)
{
	int sd = (int)p;
	int RECV_BUF_SIZE = 2048;
	char recv_buf[RECV_BUF_SIZE];
	char tempDataStr[RECV_BUF_SIZE];
	int n, nwrote;

	//xadc code
		XAdcPs_Config *ConfigPtr;
		XAdcPs *XADCInstPtr = &XADCMonInst;
		//status of initialisation
		int Status_ADC;
		//temperature readings
		u32 TempRawData;
		float TempData;
		//init_platform();

		 //XADC initilization
		 ConfigPtr = XAdcPs_LookupConfig(XPAR_AXI_XADC_0_DEVICE_ID);
		 if (ConfigPtr == NULL) {
		 return XST_FAILURE;
		 }
		 Status_ADC = XAdcPs_CfgInitialize(XADCInstPtr,ConfigPtr,ConfigPtr->BaseAddress);
		 if(XST_SUCCESS != Status_ADC){
		 print("ADC INIT FAILED\n\r");
		 return XST_FAILURE;
		 }
		 //self test
		 Status_ADC = XAdcPs_SelfTest(XADCInstPtr);
		if (Status_ADC != XST_SUCCESS) {
		return XST_FAILURE;
		}
		//stop sequencer
		XAdcPs_SetSequencerMode(XADCInstPtr,XADCPS_SEQ_MODE_SINGCHAN);
		 //disable alarms
		 XAdcPs_SetAlarmEnables(XADCInstPtr, 0x0);
		 //configure sequencer to just sample internal on chip parameters
		 XAdcPs_SetSeqInputMode(XADCInstPtr, XADCPS_SEQ_MODE_SAFE);
		 //configure the channel enables we want to monitor

		XAdcPs_SetSeqChEnables(XADCInstPtr,XADCPS_CH_TEMP|XADCPS_CH_VCCINT|XADCPS_CH_VCCAUX|XADCPS_CH_VBRAM|XADCPS_CH_VCCPINT|XADCPS_CH_VCCPAUX|XADCPS_CH_VCCPDRO);


	while (1) {
		/* read a max of RECV_BUF_SIZE bytes from socket */
		if ((n = read(sd, recv_buf, RECV_BUF_SIZE)) < 0) {
			xil_printf("%s: error reading from socket %d, closing socket\r\n", __FUNCTION__, sd);
			break;
		}

		/* break if the recved message = "quit" */
		if (!strncmp(recv_buf, "quit", 4))
			break;

		/* break if client closed connection */
		if (n <= 0)
			break;

		if (!strncmp(recv_buf, "temp", 4)){
		//read&print xadc temp
		 TempRawData = XAdcPs_GetAdcData(XADCInstPtr, XADCPS_CH_TEMP);
		 TempData = XAdcPs_RawToTemperature(TempRawData);
		 //printf("Raw Temp %lu Real Temp %f \n\r", TempRawData, TempData);
		 sprintf(tempDataStr, "Temperature= %f", TempData);

		/* handle request */
		if ((nwrote = write(sd, tempDataStr, n)) < 0) {
			xil_printf("%s: ERROR responding to client echo request. received = %d, written = %d\r\n",
					__FUNCTION__, n, nwrote);
			xil_printf("Closing socket %d\r\n", sd);
			break;
		}
		}
	}

	/* close connection */
	close(sd);
	vTaskDelete(NULL);
}

void echo_application_thread()
{
	int sock, new_sd;
	struct sockaddr_in address, remote;
	int size;

	if ((sock = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return;

	address.sin_family = AF_INET;
	address.sin_port = htons(5001);
	address.sin_addr.s_addr = INADDR_ANY;

	if (lwip_bind(sock, (struct sockaddr *)&address, sizeof (address)) < 0)
		return;

	lwip_listen(sock, 0);

	size = sizeof(remote);

	while (1) {
		if ((new_sd = lwip_accept(sock, (struct sockaddr *)&remote, (socklen_t *)&size)) > 0) {
			sys_thread_new("echos", process_echo_request,
				(void*)new_sd,
				THREAD_STACKSIZE,
				DEFAULT_THREAD_PRIO);
		}
	}
}


