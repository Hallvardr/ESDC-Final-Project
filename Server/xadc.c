/*#include <stdio.h>
//#include "platform.h"
#include "xadcps.h"
#include "xil_types.h"
#define XPAR_AXI_XADC_0_DEVICE_ID 0
//void print(char *str);
static XAdcPs XADCMonInst;

void getData()

{
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
 while(1){
 TempRawData = XAdcPs_GetAdcData(XADCInstPtr, XADCPS_CH_TEMP);
 TempData = XAdcPs_RawToTemperature(TempRawData);
 printf("Raw Temp %lu Real Temp %f \n\r", TempRawData, TempData);
 }
 return TempData;
}*/
