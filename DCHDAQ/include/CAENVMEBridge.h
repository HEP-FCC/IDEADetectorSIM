/*
 * CAENVMEBridge.h
 *
 *  Created on: Sep 4, 2018
 *      Author: helab
 */

#ifndef INCLUDE_CAENVMEBRIDGE_H_
#define INCLUDE_CAENVMEBRIDGE_H_

#include <CAENVMElib.h>
#include <unistd.h>

int32_t  vmeHandle=0;

void ConfigBridge(){

////#ifdef HAVE_CAEN_BRD
//
//  //SET PRESCALING
//  //int presc_fact;
//  //int size = sizeof(int);
//  //db_get_value(hDB, 0, "/Configuration/Prescaling", &presc_fact, &size, TID_INT, TRUE);
//  //CAENVME_SetScalerConf(vmeHandle,presc_fact,1,cvInputSrc0,cvManualSW,cvManualSW);
//  //CAENVME_SetScalerConf(vmeHandle,1,1,cvInputSrc0,cvManualSW,cvManualSW);
//
//  //SET TRG DELAY - use out_3
//  CAENVME_SetPulserConf(vmeHandle,cvPulserB,20,10,cvUnit25ns,1,cvInputSrc0,cvManualSW);
//  CAENVME_SetOutputConf(vmeHandle,cvOutput3,cvInverted,cvActiveHigh,cvMiscSignals);
//
//  //Use In0 and PulserA to generate a busy gate of 250ns for the V1742 board
////  CAENVME_SetPulserConf(vmeHandle,cvPulserA,141,140,cvUnit1600ns,1,cvInputSrc0,cvManualSW);
////  CAENVME_SetOutputConf(vmeHandle,cvOutput1,cvInverted,cvActiveHigh,cvMiscSignals);
//
////#endif

}

int exit_vme(){

  CAENVME_End(vmeHandle);
  return 0;
}

int initBridge(int v1742present) {
	//  std::cout<<"Sys Res "<<CAENVME_SystemReset(vmeHandle);
	//  CAENVME_ResetScalerCount(vmeHandle);
printf("init Bridge\n");
//	sleep(1000);

	unsigned int data;

	//SET POLARITY OF LEDS
	data = 0x7F;
	CAENVME_WriteRegister(vmeHandle,cvLedPolRegClear,data);

	//Invert linac trigger polarity
	//CAENVME_ReadRegister(vmeHandle,cvInMuxRegSet,&data);
	//data |= 0x1;
	//CAENVME_WriteRegister(vmeHandle,cvInMuxRegSet,data);

	////Use In0 and PulserA to generate a busy gate of 250ns for the V1742 board (output on OUT_1)
	//#ifdef HAVE_V1742
	//  CAENVME_ReadRegister(vmeHandle,cvInMuxRegSet,&data);
	//  data &= 0xFFFE; //Set INPUT POLARITY: 0 = Direct
	//  data |= 0x6; //Set PULSER START SOURCE: 01 = IN_0 && PULSER A RESET SOURCE: 1 = Input 0
	//  CAENVME_WriteRegister(vmeHandle,cvInMuxRegSet,data);
	//#endif

	//TAKE OUT_4 from scaler and OUT_0 (busy) FROM OUTPUT REGISTER
	CAENVME_ReadRegister(vmeHandle,cvOutMuxRegSet,&data);
	printf("MUX ---- %d\n",data);
	data |= 0x3; //set out_0 mux to 11
	data |= 0xC; //set out_1 mux to 11
	data |= 0x30; //set out_2 mux to 11
	data |= 0x300; //set out_4 mux to 11
	//  data &= 0x3FF; //set to zero the polarities
	data &= 0x83FF; //set to zero the polarities
	//#ifdef HAVE_V1742
	//  data &= 0xFFFB; //set out_1 to Pulser A Output
	//#endif
	printf("MUX ---- %d\n",data);
	//  data = 0x100; //out_4 mux to 10 (scaler end count)
	CAENVME_WriteRegister(vmeHandle,cvOutMuxRegClear,0x7FFF);//data);
	CAENVME_WriteRegister(vmeHandle,cvOutMuxRegSet,data);
	CAENVME_ReadRegister(vmeHandle,cvOutMuxRegSet,&data);
	printf("MUX ---- %d\n",data);

	//SET OUT_1 and OUT_2 to 1 and OUT_0 to 0 (busy) through OUTPUT REGISTER
	data = 0x40; //clear out_0
	if (v1742present) {
		//  data = 0xC0; //clear out_0,1
		data = 0x1C0; //clear out_0,1,2
	}
	printf("SR (CLR) ---- %d\n",data);
	int ret_stat = CAENVME_WriteRegister(vmeHandle,cvOutRegClear,data);
	CAENVME_ReadRegister(vmeHandle,cvOutRegSet,&data);
	//#ifdef HAVE_V1742
	//
	//#else
	//  data |= 0x80;    //set out_1 to 1
	data &= 0xFF7F;    //set out_1 to 0
	//#endif
	//  data |= 0x100;    //set out_2 to 1
	data |= 0x10;    //scaler gate
	printf("SR ---- %d\n",data);
	CAENVME_WriteRegister(vmeHandle,cvOutRegSet,data);
	CAENVME_ReadRegister(vmeHandle,cvOutRegSet,&data);
	printf("SR ---- %d %d\n",ret_stat,data);
	//#ifdef HAVE_V1742
	//  data = 10<<8; //width = 10 x range , period = 0
	//  printf("PA0R ---- %d\n",data);
	//  CAENVME_WriteRegister(vmeHandle,cvPulserA0,data);
	//  CAENVME_ReadRegister(vmeHandle,cvPulserA0,&data);
	//  printf("PA0R ---- %d\n",data);
	//  CAENVME_ReadRegister(vmeHandle,cvPulserA1,&data);
	//  data &= 0xFC00;
	//  data |= 0x1; //range = 25ns , n pulse = 1
	//  printf("PA1R ---- %d\n",data);
	//  CAENVME_WriteRegister(vmeHandle,cvPulserA1,data);
	//  CAENVME_ReadRegister(vmeHandle,cvPulserA1,&data);
	//  printf("PA1R ---- %d\n",data);
	//#endif


	//#ifdef HAVE_CAEN_BRD
	ConfigBridge();
	//#endif
	printf("init Bridge done\n");
	return 0;
}

int busyOFF() {
//int busyON() {

//printf("Start Busy ON\n");

  unsigned int data;
  //SET OUT_0 to 0 (busy) through OUTPUT REGISTER
//  data = 0x40;
  CAENVME_ReadRegister(vmeHandle,cvOutRegClear,&data);
//  data |= 0x40;
  data |= 0x140;
  CAENVME_WriteRegister(vmeHandle,cvOutRegClear,data);
  do {
    usleep(2);
    CAENVME_ReadRegister(vmeHandle,cvOutRegSet,&data);
//  } while ( (data &= 0x40)!=0x0 );
  } while ( (data &= 0x140)!=0x0 );
//  //------------ test ------------
//  CAENVME_ReadRegister(vmeHandle,cvOutRegClear,&data);
//  data |= 0x100;
//  CAENVME_WriteRegister(vmeHandle,cvOutRegClear,data);
//  do {
//    usleep(2);
//    CAENVME_ReadRegister(vmeHandle,cvOutRegSet,&data);
//  } while ( (data &= 0x100)!=0x0 );
//  usleep(5);
//printf("End Busy ON\n");
  return 0;
}

int busyON() {
//int busyOFF() {

//  std::cerr<<"Start Busy OFF"<<std::endl;

  unsigned int data;
  //SET OUT_0 to 1 (not busy) through OUTPUT REGISTER
  CAENVME_ReadRegister(vmeHandle,cvOutRegSet,&data);
//  data |= 0x40;
  data |= 0x140;
  CAENVME_WriteRegister(vmeHandle,cvOutRegSet,data);
  do {
    usleep(2);
    CAENVME_ReadRegister(vmeHandle,cvOutRegSet,&data);
//  } while ( (data &= 0x40)!=0x40 );
  } while ( (data &= 0x140)!=0x140 );
////------------ test ------------
//  CAENVME_ReadRegister(vmeHandle,cvOutRegSet,&data);
//  data |= 0x100;
//  CAENVME_WriteRegister(vmeHandle,cvOutRegSet,data);
//  do {
//    usleep(2);
//    CAENVME_ReadRegister(vmeHandle,cvOutRegSet,&data);
//  } while ( (data &= 0x100)!=0x100 );

//  usleep(5);
//  std::cerr<<"End Busy OFF"<<std::endl;

  return 0;
}

void EnableGlobalVeto() {
  unsigned int data;
//  std::cout<<"enable GL veto"<<std::endl;
  //SET OUT_1 to 0 (veto) through OUTPUT REGISTER
//  CAENVME_ReadRegister(vmeHandle,cvOutRegSet,&data);
//  data &= 0xFF7F;
//  CAENVME_WriteRegister(vmeHandle,cvOutRegSet,data);
  CAENVME_ReadRegister(vmeHandle,cvOutRegClear,&data);
  data |= 0x80;
  CAENVME_WriteRegister(vmeHandle,cvOutRegClear,data);

}

void DisableGlobalVeto() {
  unsigned int data;
//  std::cout<<"disable GL veto"<<std::endl;
  //SET OUT_1 to 1 (not veto) through OUTPUT REGISTER
  CAENVME_ReadRegister(vmeHandle,cvOutRegSet,&data);
  data |= 0x80;
  CAENVME_WriteRegister(vmeHandle,cvOutRegSet,data);
}


#endif /* INCLUDE_CAENVMEBRIDGE_H_ */
