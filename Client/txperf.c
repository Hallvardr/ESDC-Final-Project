/*
 * Copyright (c) 2007 Xilinx, Inc.  All rights reserved.
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
#include "lwipopts.h"
#ifdef __arm__
#include "xil_printf.h"
#include "FreeRTOS.h"
#include "task.h"
#endif

#define SEND_BUFSIZE (50)

static char send_buf[SEND_BUFSIZE];

void print_ip();

void
print_txperf_app_header()
{
        xil_printf("%s\r\n", "iperf Server should be running on the PC BEFORE launching app"); 
        xil_printf("%s\r\n","$ iperf -s -i 5 -w 64k (on PC)");
}

void tx_application_thread()
{
	struct ip_addr servaddr;
	int i, sock;
	struct sockaddr_in serv_addr;

	/* Set here the host (PC) IP address */
	IP4_ADDR(&servaddr,  147, 83, 81, 222);

	/* initialize data buffer being sent */
	for (i = 0; i < SEND_BUFSIZE; i++)
		send_buf[i] = (i % 10) + '0';

	if ((sock = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		xil_printf("error creating socket\r\n");
#ifdef OS_IS_FREERTOS
		vTaskDelete(NULL);
#endif
		return;
	}

	memset((void*)&serv_addr, 0, sizeof serv_addr);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(5001);
	serv_addr.sin_addr.s_addr = servaddr.addr;

    print_ip("connect to  ", &servaddr);
    xil_printf("... ");

	if (lwip_connect(sock, (struct sockaddr *)&serv_addr, sizeof (serv_addr)) < 0) {
		xil_printf("error in connect\r\n");
#ifdef OS_IS_FREERTOS
		vTaskDelete(NULL);
#endif
		return;
	}
	print("Connected\n\r");

	/*while (lwip_write(sock, "temp", SEND_BUFSIZE) >= 0) {
					char recv_buf[1500];

					sleep(5);
					print("2");
					if (lwip_read(sock, recv_buf, 1400) <= 0)
					print(recv_buf);

		}*/
while(1){
	sleep(5);

	while (lwip_write(sock, "temp", SEND_BUFSIZE) >= 0) {
		break;
	}

	char recv_buf[1500];

	while (lwip_read(sock, recv_buf, 1400) <= 0){
		break;
	}
	print(recv_buf);
}

	print("TX perf stopped\r\n");
	vTaskDelete(NULL);

}
