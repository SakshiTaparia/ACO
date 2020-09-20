/************************************************************************************
 * Copyright (C) 2016                                                               *
 * TETCOS, Bangalore. India                                                         *
 *                                                                                  *
 * Tetcos owns the intellectual property rights in the Product and its content.     *
 * The copying, redistribution, reselling or publication of any or all of the       *
 * Product or its content without express prior written consent of Tetcos is        *
 * prohibited. Ownership and / or any other right relating to the software and all  *
 * intellectual property rights therein shall remain at all times with Tetcos.      *
 *                                                                                  *
 * Author:    Shashi Kant Suman                                                     *
 *                                                                                  *
 * ---------------------------------------------------------------------------------*/
#include "main.h"
#include "IEEE802_11.h"
#include "IEEE802_11_Phy.h"
#include "IEEE802_11_MAC_Frame.h"
#include "IEEE802_11_PhyFrame.h"
#include "NetSim_utility.h"
#include "Medium.h"

//Function prototype
static double fn_NetSim_IEEE802_11_CalculateBER(PIEEE802_11_PHY_VAR phy,double dReceivedPower);


#define SPEED_OF_LIGHT 299.792458 // meter per micro sec
static double calculate_propagation_delay(NetSim_PACKET* packet)
{
	NETSIM_ID tx=packet->nTransmitterId;
	NETSIM_ID rx=packet->nReceiverId;
	double d=fn_NetSim_Utilities_CalculateDistance(DEVICE_POSITION(tx),DEVICE_POSITION(rx));
	return d/SPEED_OF_LIGHT;
}

/**
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
This function is used to configure the PhySical Layer parameters of device.
Configure the Preamble and PLCP header length.
Configure Control and broadcast frame data rate.
Calculate DIFS, EIFS and RIFS values.
Assign CS Threshold Value.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
int fn_NetSim_IEEE802_11_PHY_Init(NETSIM_ID nDeviceId,NETSIM_ID nInterfaceId)
{
	PIEEE802_11_PHY_VAR pstruPhyVar=IEEE802_11_PHY(nDeviceId,nInterfaceId);
	
	//Turn on the radio
	pstruPhyVar->radio.radioState = RX_ON_IDLE;

	switch(pstruPhyVar->PhyType)
	{
	case OFDM: //Table 18-17—OFDM PHY characteristics page 1623
		switch((int)pstruPhyVar->dChannelBandwidth)
		{
		case 20:
			pstruPhyVar->plmeCharacteristics.aPLCPHeaderLength = 4 ;	//MicroSecs
			pstruPhyVar->plmeCharacteristics.aPreambleLength = 16;	//MicroSecs
			pstruPhyVar->plmeCharacteristics.aCCATime = 4;			//MicroSecs
			pstruPhyVar->plmeCharacteristics.aMACProcessingDelay = 2;//MicroSecs
			pstruPhyVar->plmeCharacteristics.aMPDUDurationFactor = 0;
			pstruPhyVar->plmeCharacteristics.aMPDUMaxLength = 4095;
			pstruPhyVar->plmeCharacteristics.aPHY_RX_START_Delay = 25;//MicroSecs
			pstruPhyVar->plmeCharacteristics.aRxPLCPDelay = 1;		//MicroSecs
			pstruPhyVar->plmeCharacteristics.aRxRFDelay = 1;			//MicroSecs
			pstruPhyVar->plmeCharacteristics.aRxTxSwitchTime = 1;	// MicroSecs
			pstruPhyVar->plmeCharacteristics.aRxTxTurnaroundTime = 2;//MicroSecs
			pstruPhyVar->plmeCharacteristics.aTxPLCPDelay = 1;		//MicroSecs
			pstruPhyVar->plmeCharacteristics.aTxRampOnTime = 1;		//MicroSecs				
			//Assign Control frame and broadcast frame data rate
			pstruPhyVar->dControlFrameDataRate = CONTRL_FRAME_RATE_11A_AND_G;	
			pstruPhyVar->dBroadcastFrameDataRate = CONTRL_FRAME_RATE_11A_AND_G;
			break;
		case 10:
			pstruPhyVar->plmeCharacteristics.aPLCPHeaderLength = 8 ;	//MicroSecs
			pstruPhyVar->plmeCharacteristics.aPreambleLength = 32;	//MicroSecs
			pstruPhyVar->plmeCharacteristics.aCCATime = 8;			//MicroSecs
			pstruPhyVar->plmeCharacteristics.aMACProcessingDelay = 2;//MicroSecs
			pstruPhyVar->plmeCharacteristics.aMPDUDurationFactor = 0;
			pstruPhyVar->plmeCharacteristics.aMPDUMaxLength = 4095;
			pstruPhyVar->plmeCharacteristics.aPHY_RX_START_Delay = 49;//MicroSecs
			pstruPhyVar->plmeCharacteristics.aRxPLCPDelay = 1;		//MicroSecs
			pstruPhyVar->plmeCharacteristics.aRxRFDelay = 1;			//MicroSecs
			pstruPhyVar->plmeCharacteristics.aRxTxSwitchTime = 1;	// MicroSecs
			pstruPhyVar->plmeCharacteristics.aRxTxTurnaroundTime = 2;//MicroSecs
			pstruPhyVar->plmeCharacteristics.aTxPLCPDelay = 1;		//MicroSecs
			pstruPhyVar->plmeCharacteristics.aTxRampOnTime = 1;		//MicroSecs				
			//Assign Control frame and broadcast frame data rate
			pstruPhyVar->dControlFrameDataRate = CONTRL_FRAME_RATE_11P;	
			pstruPhyVar->dBroadcastFrameDataRate = CONTRL_FRAME_RATE_11P;
			break;
		}
		fn_NetSim_IEEE802_11_OFDMPhy_SetEDThreshold(pstruPhyVar);
		break;
	case HT:
		fn_NetSim_IEEE802_11n_OFDM_MIMO_init(nDeviceId,nInterfaceId);
		fn_NetSim_IEEE802_11_HTPhy_SetEDThreshold(pstruPhyVar);
		break;
	case VHT:
		fn_NetSim_IEEE802_11ac_OFDM_MIMO_init(nDeviceId,nInterfaceId);
		fn_NetSim_IEEE802_11_HTPhy_SetEDThreshold(pstruPhyVar);
		break;
	case DSSS:
	default:
		pstruPhyVar->plmeCharacteristics.aPLCPHeaderLength = 48;	//MicroSecs 
		pstruPhyVar->plmeCharacteristics.aPreambleLength = 144;	//MicroSecs 
		pstruPhyVar->plmeCharacteristics.aCCATime = 15;			//MicroSecs
		pstruPhyVar->plmeCharacteristics.aMACProcessingDelay = 2;//MicroSecs
		pstruPhyVar->plmeCharacteristics.aMPDUDurationFactor = 0;
		pstruPhyVar->plmeCharacteristics.aMPDUMaxLength = 8192;
		pstruPhyVar->plmeCharacteristics.aPHY_RX_START_Delay = 192;//MicroSecs
		pstruPhyVar->plmeCharacteristics.aRxPLCPDelay = 1;		//MicroSecs
		pstruPhyVar->plmeCharacteristics.aRxRFDelay = 1;			//MicroSecs
		pstruPhyVar->plmeCharacteristics.aRxTxSwitchTime = 4;	// MicroSecs
		pstruPhyVar->plmeCharacteristics.aRxTxTurnaroundTime = 5;//MicroSecs
		pstruPhyVar->plmeCharacteristics.aTxPLCPDelay = 1;		//MicroSecs
		pstruPhyVar->plmeCharacteristics.aTxRampOnTime = 1;		//MicroSecs	
		pstruPhyVar->dControlFrameDataRate = CONTRL_FRAME_RATE_11B;	//2
		pstruPhyVar->dBroadcastFrameDataRate = CONTRL_FRAME_RATE_11B;//2
		fn_NetSim_IEEE802_11_DSSPhy_SetEDThreshold(pstruPhyVar);
		break;
	}
	
	pstruPhyVar->SIFS = pstruPhyVar->plmeCharacteristics.aSIFSTime;
	pstruPhyVar->DIFS = pstruPhyVar->plmeCharacteristics.aSIFSTime + (2 * pstruPhyVar->plmeCharacteristics.aSlotTime);
	pstruPhyVar->EIFS = pstruPhyVar->plmeCharacteristics.aSIFSTime + pstruPhyVar->DIFS + (int) (ACK_SIZE * 8.0 / pstruPhyVar->dControlFrameDataRate) + 1;
	pstruPhyVar->PIFS =  pstruPhyVar->plmeCharacteristics.aSIFSTime +  pstruPhyVar->plmeCharacteristics.aSlotTime;
	return 0;
}

bool isMediumIdle(NETSIM_ID d, NETSIM_ID in)
{
	return medium_isIdle(d, in);
}

double get_preamble_time(PIEEE802_11_PHY_VAR phy)
{
	double dPreambleTime = 0;
	switch(phy->PhyProtocol)
	{
	case IEEE_802_11b:
	case IEEE_802_11a:
	case IEEE_802_11g:
	case IEEE_802_11p:
		dPreambleTime = (double)(phy->plmeCharacteristics.aPreambleLength+phy->plmeCharacteristics.aPLCPHeaderLength);
		break;
	case IEEE_802_11n:
		dPreambleTime = get_11n_preamble_time(phy);
		break;
	case IEEE_802_11ac:
		dPreambleTime = get_11ac_preamble_time(phy);
		break;
	default:
		fnNetSimError("Unknown phy protocol %d in %s.",phy->PhyProtocol,__FUNCTION__);
	}
	return dPreambleTime;
}

int fn_NetSim_IEEE802_11_PhyOut()
{
	NETSIM_ID srcid=pstruEventDetails->nDeviceId;
	NETSIM_ID srcif=pstruEventDetails->nInterfaceId;

	PIEEE802_11_PHY_VAR phy = IEEE802_11_CURR_PHY;
	NetSim_PACKET* packet=pstruEventDetails->pPacket;
	double time = pstruEventDetails->dEventTime;
	double dTransmissionTime;
	double dPropagationDelay;
	double dPreambleTime;
	int flag=0;
	NetSim_PACKET* list = fn_NetSim_Packet_CopyPacketList(pstruEventDetails->pPacket);
	NetSim_PACKET* last=NULL;
	bool transmitflag;

	list->pstruPhyData->dArrivalTime = pstruEventDetails->dEventTime;
	list->pstruPhyData->dStartTime = pstruEventDetails->dEventTime;
	
	while(packet)
	{
		NETSIM_ID destId = packet->nReceiverId;
		NETSIM_ID destif = fn_NetSim_Stack_GetConnectedInterface(srcid, srcif, destId);

		last = packet;

		packet->pstruPhyData->dArrivalTime=pstruEventDetails->dEventTime;
		packet->pstruPhyData->dStartTime=time;
		packet->pstruPhyData->dEndTime=time;
	
		packet->pstruPhyData->dPayload=packet->pstruMacData->dPacketSize;
		packet->pstruPhyData->dOverhead=0;
		packet->pstruPhyData->dPacketSize=packet->pstruPhyData->dPayload+
			packet->pstruPhyData->dOverhead;

		fn_NetSim_IEEE802_11_SetDataRate(srcid, srcif,
										 destId, destif,
										 packet, packet->pstruPhyData->dArrivalTime);

		fn_NetSim_IEEE802_11_Add_Phy_Header(packet);
	
		packet->pstruPhyData->nPhyMedium=PHY_MEDIUM_WIRELESS;

		if(!flag)
			dPreambleTime = get_preamble_time(phy);
		else
			dPreambleTime = 0;

		dTransmissionTime = fn_NetSim_IEEE802_11_CalculateTransmissionTime(packet->pstruPhyData->dPayload
																		   ,pstruEventDetails->nDeviceId,
																		   pstruEventDetails->nInterfaceId);
		if(packet->nReceiverId)
			dPropagationDelay = max(calculate_propagation_delay(packet),0.01);
		else
			dPropagationDelay = 0.01;
	
		time+=dTransmissionTime+dPreambleTime;
		packet->pstruPhyData->dStartTime=time;
		packet->pstruPhyData->dEndTime=packet->pstruPhyData->dStartTime+dPropagationDelay;

		if(wireshark_trace.convert_sim_to_real_packet && 
			DEVICE_MACLAYER(pstruEventDetails->nDeviceId,pstruEventDetails->nInterfaceId)->isWiresharkWriter)
		{
			wireshark_trace.convert_sim_to_real_packet(packet,
				wireshark_trace.pcapWriterlist[pstruEventDetails->nDeviceId-1][pstruEventDetails->nInterfaceId-1],
				pstruEventDetails->dEventTime);
		}
		packet = packet->pstruNextPacket;
		flag=1;
	}

	if (!validate_processing_time(time,
								  pstruEventDetails->nDeviceId,
								  pstruEventDetails->nInterfaceId))
		return -1;

	double nav = calculate_nav(pstruEventDetails->nDeviceId,
							   pstruEventDetails->nInterfaceId,
							   pstruEventDetails->pPacket);

	//fn_NetSim_IEEE802_11_UpdatePowerduetoTxStart(srcid, srcif,pstruEventDetails->dEventTime,nav);

	packet = pstruEventDetails->pPacket;
	// Transmit the packet
	if(packet->nReceiverId)
	{
		transmitflag = fn_NetSim_IEEE802_11_TransmitFrame(packet,srcid,srcif,pstruEventDetails->dEventTime);
		if(!isIEEE802_11_CtrlPacket(packet) &&
		   !isMulticastPacket(packet) &&
		   !isBroadcastPacket(packet))
			fn_NetSim_IEEE802_11_CSMACA_AddAckTimeOut(last,srcid,srcif);
		else if(packet->nControlDataType == WLAN_RTS)
			fn_NetSim_IEEE802_11_RTS_CTS_AddCTSTimeOut(packet,srcid,srcif);

		if(!transmitflag) //Packet is not transmitted
			fn_NetSim_Packet_FreePacket(packet);
	}
	else
	{
		fn_NetSim_IEEE802_11_TransmitBroadcastFrame(packet,srcid,srcif);
		fn_NetSim_Packet_FreePacket(packet);
	}
	return 0;
}

int fn_NetSim_IEEE802_11_PhyIn()
{
	double dFadingPower=0;
	NetSim_PACKET* packet = pstruEventDetails->pPacket;
	ptrIEEE802_11_PHY_HDR hdr = PACKET_PHYPROTOCOLDATA(packet);
	PIEEE802_11_PHY_VAR phy = IEEE802_11_CURR_PHY;
	PIEEE802_11_MAC_VAR srcMac;
	PIEEE802_11_PHY_VAR srcPhy;
	double ber;
	PACKET_STATUS status;
	NETSIM_ID ifid;
	bool morefrag;

	morefrag = is_more_fragment_coming(packet);

	//Wireshark
	if(wireshark_trace.convert_sim_to_real_packet && 
		DEVICE_MACLAYER(pstruEventDetails->nDeviceId,pstruEventDetails->nInterfaceId)->isWiresharkWriter)
	{
		wireshark_trace.convert_sim_to_real_packet(pstruEventDetails->pPacket,
			wireshark_trace.pcapWriterlist[pstruEventDetails->nDeviceId-1][pstruEventDetails->nInterfaceId-1],
			pstruEventDetails->dEventTime);
	}

	ifid = fn_NetSim_Stack_GetConnectedInterface(pstruEventDetails->nDeviceId,
												 pstruEventDetails->nInterfaceId,
												 packet->nTransmitterId);
	
	srcPhy = IEEE802_11_PHY(packet->nTransmitterId, ifid);
	srcMac = IEEE802_11_MAC(packet->nTransmitterId, ifid);
	if (!morefrag)
	{
		if (set_radio_state(packet->nTransmitterId, ifid,
							RX_ON_IDLE, pstruEventDetails->nDeviceId,
							phy->firstPacketTransmissionId ? phy->firstPacketTransmissionId : hdr->transmissionId))
			set_mac_state_after_txend(srcMac);
	}
	else
	{
		if (is_first_packet(packet))
			phy->firstPacketTransmissionId = hdr->transmissionId;

	}
	medium_notify_packet_received(packet);


	double pdbm = GET_RX_POWER_dbm(packet->nTransmitterId,
								   ifid,
								   pstruEventDetails->nDeviceId,
								   pstruEventDetails->nInterfaceId,
								   packet->pstruPhyData->dArrivalTime);

	if (isIEEE802_11_CtrlPacket(packet) || !morefrag)
		set_radio_state(pstruEventDetails->nDeviceId, pstruEventDetails->nInterfaceId,
			RX_ON_IDLE, packet->nTransmitterId,
			phy->firstPacketTransmissionId ? phy->firstPacketTransmissionId : hdr->transmissionId);
	
	// comment the ignoring code of out of coverage packet
	/*if (pdbm < srcPhy->dCurrentRxSensitivity_dbm)
	{
		set_radio_state(pstruEventDetails->nDeviceId, pstruEventDetails->nInterfaceId,
						RX_ON_IDLE, packet->nTransmitterId, 
						phy->firstPacketTransmissionId ? phy->firstPacketTransmissionId : hdr->transmissionId);
		return -1; // dest is not in range
	}*/

	
	// Our code for ignore Vehicle TO Vehicle communication
	// Height of antenna for vehicle is 1m and height of RSU is 6m
	if (phy->dAntennaHeight == srcPhy->dAntennaHeight )
	{
		
		set_radio_state(pstruEventDetails->nDeviceId, pstruEventDetails->nInterfaceId,
			RX_ON_IDLE, packet->nTransmitterId,
			phy->firstPacketTransmissionId ? phy->firstPacketTransmissionId : hdr->transmissionId);
		return -1; // Ignore vehicle packet
	}
	// Ignore the rebroadcast packet coming from RSU 
	if (packet->nSourceId != packet->nTransmitterId)
	{
		set_radio_state(pstruEventDetails->nDeviceId, pstruEventDetails->nInterfaceId,
			RX_ON_IDLE, packet->nTransmitterId,
			phy->firstPacketTransmissionId ? phy->firstPacketTransmissionId : hdr->transmissionId);
		return -1;
	}
	
	/*
	if (isIEEE802_11_CtrlPacket(packet) || !morefrag)
		set_radio_state(pstruEventDetails->nDeviceId, pstruEventDetails->nInterfaceId,
						RX_ON_IDLE, packet->nTransmitterId,
						phy->firstPacketTransmissionId ? phy->firstPacketTransmissionId : hdr->transmissionId);

	*/
	// Our code for cover only packet in transmission range
	// Transmission range in form of receiving sensitity is set for different antenna gains

	//fprintf(stderr, "----> %f  ", pdbm);

	//change in code
	//receiving power at different dbi


// calculate the distance between RSU and vehicle
	double d = fn_NetSim_Utilities_CalculateDistance(DEVICE_POSITION(packet->nTransmitterId), DEVICE_POSITION(pstruEventDetails->nDeviceId));

// packet error if out of range
	if (phy->dAntennaGain == 6.0 && d > 243)
	{
			if (!isIEEE802_11_CtrlPacket(packet) && is_first_packet(packet))
				phy->firstpacketstatus = PacketStatus_Error;
			// call function to write packet trace and calculate metrics
			packet->pstruPhyData->nPacketErrorFlag = PacketStatus_Error;
			packet->nPacketStatus = PacketStatus_Error;
			fn_NetSim_WritePacketTrace(packet);
			fn_NetSim_Metrics_Add(packet);
			fn_NetSim_Packet_FreePacket(packet);
			goto RET_PHYIN;
			
					
	}

	if (phy->dAntennaGain == 9.0 && d > 338)
	{
		
			if (!isIEEE802_11_CtrlPacket(packet) && is_first_packet(packet))
				phy->firstpacketstatus = PacketStatus_Error;
			// call function to write packet trace and calculate metrics
			packet->pstruPhyData->nPacketErrorFlag = PacketStatus_Error;
			packet->nPacketStatus = PacketStatus_Error;
			fn_NetSim_WritePacketTrace(packet);
			fn_NetSim_Metrics_Add(packet);
			fn_NetSim_Packet_FreePacket(packet);
			goto RET_PHYIN;
		
	}
	if (phy->dAntennaGain == 12.0 && d > 503)
	{
		
			if (!isIEEE802_11_CtrlPacket(packet) && is_first_packet(packet))
				phy->firstpacketstatus = PacketStatus_Error;
			// call function to write packet trace and calculate metrics
			packet->pstruPhyData->nPacketErrorFlag = PacketStatus_Error;
			packet->nPacketStatus = PacketStatus_Error;
			fn_NetSim_WritePacketTrace(packet);
			fn_NetSim_Metrics_Add(packet);
			fn_NetSim_Packet_FreePacket(packet);
			goto RET_PHYIN;
		
	}
	

	//--------------------------------------------------------------------

	if(phy->firstpacketstatus==PacketStatus_Collided)
		packet->nPacketStatus=PacketStatus_Collided;
	else if(phy->firstpacketstatus==PacketStatus_Error)
		packet->nPacketStatus=PacketStatus_Error;

	if(packet->nPacketStatus==PacketStatus_Collided)
	{
		if(!isIEEE802_11_CtrlPacket(packet) && is_first_packet(packet))
			phy->firstpacketstatus = PacketStatus_Collided;

		fn_NetSim_WritePacketTrace(packet);
		fn_NetSim_Metrics_Add(packet);
		fn_NetSim_Packet_FreePacket(packet);
		goto RET_PHYIN;
	}
	
	dFadingPower = propagation_calculate_fadingloss(propagationHandle,
													packet->nTransmitterId,
													ifid,
													pstruEventDetails->nDeviceId,
													pstruEventDetails->nInterfaceId);
	pdbm -= dFadingPower;
	
	ber = fn_NetSim_IEEE802_11_CalculateBER(srcPhy,pdbm);

	status = fn_NetSim_Packet_DecideError(ber,packet->pstruPhyData->dPacketSize);

	if(status == PacketStatus_Error) 
	{ 
		if(!isIEEE802_11_CtrlPacket(packet) && is_first_packet(packet))
			phy->firstpacketstatus = PacketStatus_Collided;

		// call function to write packet trace and calculate metrics
		packet->nPacketStatus= PacketStatus_Collided;
		fn_NetSim_WritePacketTrace(packet);  
		fn_NetSim_Metrics_Add(packet);
		fn_NetSim_Packet_FreePacket(packet);
		goto RET_PHYIN;
	}
	else //Packet is not error. Continue		
	{
		// call function to write packet trace and calculate metrics
		packet->pstruPhyData->nPacketErrorFlag = PacketStatus_NoError;			
		packet->nPacketStatus =PacketStatus_NoError; 	
		fn_NetSim_WritePacketTrace(packet); 		
		fn_NetSim_Metrics_Add(packet);
		//pstruEventDetails->nSubEventType = RECEIVE_ACK;

	}				
	
	packet->pstruPhyData->dArrivalTime =packet->pstruPhyData->dEndTime;
	packet->pstruPhyData->dStartTime =packet->pstruPhyData->dEndTime;
	packet->pstruPhyData->dPayload = packet->pstruPhyData->dPacketSize - packet->pstruPhyData->dOverhead;
	packet->pstruPhyData->dPacketSize = packet->pstruPhyData->dPayload;
	packet->pstruPhyData->dOverhead = 0;
	packet->pstruPhyData->dEndTime =packet->pstruPhyData->dEndTime;
	// Add MAC IN event
	switch(packet->nControlDataType)
	{
	case WLAN_ACK:	
		pstruEventDetails->nSubEventType = RECEIVE_ACK;
		break;
	case WLAN_BlockACK:
		pstruEventDetails->nSubEventType = RECEIVE_BLOCK_ACK;
		break;
	case WLAN_RTS:	
		pstruEventDetails->nSubEventType = RECEIVE_RTS;
		break;
	case WLAN_CTS:
		pstruEventDetails->nSubEventType = RECEIVE_CTS;
		break;
	default:
		pstruEventDetails->nSubEventType = RECEIVE_MPDU;
		break;
	}		
	pstruEventDetails->nEventType = MAC_IN_EVENT;
	pstruEventDetails->pPacket = packet;
	fnpAddEvent(pstruEventDetails);


RET_PHYIN:
	if (!morefrag)
	{
		phy->firstPacketTransmissionId = 0;
		phy->firstpacketstatus = PacketStatus_NoError;
	}
	return 0;
}

/**
Calculate and return Transmission time for one packet.
*/
double fn_NetSim_IEEE802_11_CalculateTransmissionTime(double size, NETSIM_ID nDevId, NETSIM_ID nInterfaceId)
{
	double dTxTime = 0.0;	
	PIEEE802_11_PHY_VAR pstruPhy = IEEE802_11_PHY(nDevId,nInterfaceId);	
	
	switch (pstruPhy->PhyProtocol)
	{
	case IEEE_802_11b:
		return ceil(size *(8 / pstruPhy->PHY_TYPE.dsssPhy.dDataRate));
		break;
	case IEEE_802_11a:
	case IEEE_802_11g:
	case IEEE_802_11p:
		return ceil(size *(8 / pstruPhy->PHY_TYPE.ofdmPhy.dDataRate));
	case IEEE_802_11n:
	case IEEE_802_11ac:
		return ceil(size *(8 / pstruPhy->PHY_TYPE.ofdmPhy_11n.dDataRate));
	default:
		fnNetSimError("IEEE802.11--- Unknown phy protocol type %d in Calculate Transmission Time\n", pstruPhy->PhyProtocol);
		break;
	}
	return dTxTime;	
}


bool CheckFrequencyInterfrence(double dFrequency1,double dFrequency2,double bandwidth)
{
	if(dFrequency1 > dFrequency2)
	{
		if( (dFrequency1 - dFrequency2) >= bandwidth )
			return false; // no interference
		else 
			return true; // interference
	}
	else
	{
		if( (dFrequency2 - dFrequency1) >= bandwidth )
			return false; // no interference
		else 
			return true; // interference
	}
}

/**
Transmit the packet in the Medium, i.e from Physical out to Physical In.
While transmitting check whether the Receiver radio state is CHANNEL_IDLE and also 
is the receiver is reachable, that is not an out off reach. If both condition 
satisfied then add the PHY IN EVENT, else drop the frame.
*/
bool fn_NetSim_IEEE802_11_TransmitFrame(NetSim_PACKET* pstruPacket, NETSIM_ID nDevId, NETSIM_ID nInterfaceId,double time)
{
	PIEEE802_11_PHY_VAR srcPhy=IEEE802_11_PHY(nDevId,nInterfaceId);
	PIEEE802_11_PHY_VAR destPhy;

	NetSim_PACKET *pstruPacketList=pstruPacket;
	ptrIEEE802_11_PHY_HDR hdr = PACKET_PHYPROTOCOLDATA(pstruPacket);
	NetSim_EVENTDETAILS pevent;
	NETSIM_ID destId=pstruPacket->nReceiverId;
	NETSIM_ID destif=fn_NetSim_Stack_GetConnectedInterface(nDevId,nInterfaceId,destId);

	destPhy=IEEE802_11_PHY(destId,destif);
	
	//Update the transmitter and receiver
	while(pstruPacketList)
	{
		pstruPacketList->nReceiverId = destId;
		pstruPacketList->nTransmitterId = nDevId;
		pstruPacketList=pstruPacketList->pstruNextPacket;
	}

	// Change the Receiver state
	set_radio_state(destId, destif, RX_ON_BUSY, nDevId, hdr->transmissionId);

	if (!set_radio_state(nDevId, nInterfaceId, TRX_ON_BUSY, destId, hdr->transmissionId))
		return false; // src is off

	destPhy->dCurrentRxSensitivity_dbm=srcPhy->dCurrentRxSensitivity_dbm;
	pstruPacketList=pstruPacket;
	while(pstruPacketList)
	{
		pstruPacket=pstruPacketList;
		pstruPacketList = pstruPacketList->pstruNextPacket;
		pstruPacket->pstruNextPacket=NULL;

		memcpy(&pevent,pstruEventDetails,sizeof* pstruEventDetails);
		pevent.dEventTime = pstruPacket->pstruPhyData->dEndTime;
		pevent.dPacketSize = pstruPacket->pstruPhyData->dPacketSize;
		if(pstruPacket->pstruAppData)
		{
			pevent.nApplicationId = pstruPacket->pstruAppData->nApplicationId;
			pevent.nSegmentId = pstruPacket->pstruAppData->nSegmentId;
		}
		else
		{
			pevent.nApplicationId = 0;
			pevent.nSegmentId = 0;
		}
		pevent.nDeviceId = destId;
		pevent.nDeviceType = DEVICE_TYPE(destId);
		pevent.nEventType = PHYSICAL_IN_EVENT;
		pevent.nInterfaceId = destif;
		pevent.nPacketId = pstruPacket->nPacketId;		
		pevent.nProtocolId = MAC_PROTOCOL_IEEE802_11;
		pevent.pPacket=pstruPacket;
		pevent.nSubEventType = 0;
		fnpAddEvent(&pevent);
		medium_notify_packet_send(pstruPacket,
								  nDevId,
								  nInterfaceId,
								  destId,
								  destif);
	}
	return true;
}

int fn_NetSim_IEEE802_11_TransmitBroadcastFrame(NetSim_PACKET* pstruPacket, NETSIM_ID nDevId, NETSIM_ID nInterfaceId)
{
	NetSim_LINKS* link;
	NETSIM_ID i;
	link=DEVICE_PHYLAYER(nDevId,nInterfaceId)->pstruNetSimLinks;
	switch(link->nLinkType)
	{
	case LinkType_P2MP:
		{
			if(link->puniDevList.pstrup2MP.nCenterDeviceId !=nDevId)
			{
				bool transmitflag;
				NetSim_PACKET* packet= fn_NetSim_Packet_CopyPacketList(pstruPacket);
				packet->nReceiverId=link->puniDevList.pstrup2MP.nCenterDeviceId;
				transmitflag = fn_NetSim_IEEE802_11_TransmitFrame(packet,nDevId,nInterfaceId,packet->pstruPhyData->dArrivalTime);
				if(!transmitflag)
					fn_NetSim_Packet_FreePacket(packet);
			}
			else
			{
				for(i=0;i<link->puniDevList.pstrup2MP.nConnectedDeviceCount-1;i++)
				{
					bool transmitflag;
					NetSim_PACKET* packet= fn_NetSim_Packet_CopyPacketList(pstruPacket);
					packet->nReceiverId=link->puniDevList.pstrup2MP.anDevIds[i];
					transmitflag = fn_NetSim_IEEE802_11_TransmitFrame(packet,nDevId,nInterfaceId,packet->pstruPhyData->dArrivalTime);
					if(!transmitflag)
						fn_NetSim_Packet_FreePacket(packet);
				}
			}
		}
		break;
	case LinkType_MP2MP:
		for(i=0;i<link->puniDevList.pstruMP2MP.nConnectedDeviceCount;i++)
		{
			if(link->puniDevList.pstruMP2MP.anDevIds[i]!=nDevId)
			{
				bool transmitflag;
				NetSim_PACKET* packet= fn_NetSim_Packet_CopyPacketList(pstruPacket);
				packet->nReceiverId=link->puniDevList.pstruMP2MP.anDevIds[i];
				transmitflag = fn_NetSim_IEEE802_11_TransmitFrame(packet,nDevId,nInterfaceId,packet->pstruPhyData->dArrivalTime);
				if(!transmitflag)
					fn_NetSim_Packet_FreePacket(packet);
			}
		}
		break;
	case LinkType_P2P:
		{
			bool transmitflag;
			NetSim_PACKET* packet= fn_NetSim_Packet_CopyPacketList(pstruPacket);
			if(link->puniDevList.pstruP2P.nFirstDeviceId!=nDevId)
				pstruPacket->nReceiverId=link->puniDevList.pstruP2P.nFirstDeviceId;
			else if(link->puniDevList.pstruP2P.nSecondDeviceId!=nDevId)
				pstruPacket->nReceiverId=link->puniDevList.pstruP2P.nSecondDeviceId;
			transmitflag = fn_NetSim_IEEE802_11_TransmitFrame(packet,nDevId,nInterfaceId,packet->pstruPhyData->dArrivalTime);
			if(!transmitflag)
				fn_NetSim_Packet_FreePacket(packet);
		}
		break;
	default:
		fnNetSimError("Unknown link type in %s",__FUNCTION__);
		break;
	}
	return 0;
}

/**
This function used to compute the data rate with respect to the total received power
set the received power value using received power range table for modulation(dbm)
*/
int fn_NetSim_IEEE802_11_SetDataRate(NETSIM_ID txId, NETSIM_ID txIf,
									 NETSIM_ID rxId, NETSIM_ID rxIf,
									 NetSim_PACKET* packet, double time)
{
	PIEEE802_11_PHY_VAR phy = IEEE802_11_PHY(txId, txIf);

	switch (phy->PhyProtocol)
	{
		case IEEE_802_11b:
			fn_NetSim_IEEE802_11_DSSSPhy_DataRate(txId, txIf, rxId, packet, time);
			break;
		case IEEE_802_11a:
		case IEEE_802_11g:
		case IEEE_802_11p:
			fn_NetSim_IEEE802_11_OFDMPhy_DataRate(txId, txIf, rxId, packet, time);
			break;
		case IEEE_802_11n:
		case IEEE_802_11ac:
			fn_NetSim_IEEE802_11_HTPhy_DataRate(txId, txIf, rxId, packet, time);
			break;
		default:
			fnNetSimError("IEEE802.11--- Unknown protocol %d in %s\n", phy->PhyProtocol, __FUNCTION__);
			break;
	}
	medium_update_device(txId, txIf,
						 phy->dFrequency,
						 phy->dChannelBandwidth,
						 phy->dCurrentRxSensitivity_dbm,
						 phy->dEDThreshold);

	if (rxId)
		medium_update_device(rxId, rxIf,
							 phy->dFrequency,
							 phy->dChannelBandwidth,
							 phy->dCurrentRxSensitivity_dbm,
							 phy->dEDThreshold);
	return 0;
}

/**
This function is used to calculate the Bit Error Rate (BER). The BER calculation 
depends on the Channel characteristics, and received power. So compare the Total 
received power with the Receiver sensitivity for the particular data rate. Then 
calculate the BER with respect to particular modulation.		 
*/
static double fn_NetSim_IEEE802_11_CalculateBER(PIEEE802_11_PHY_VAR phy,double dReceivedPower)
{
	switch(phy->PhyType)
	{
	case DSSS:	
		return calculate_BER(phy->PHY_TYPE.dsssPhy.modulation,dReceivedPower,phy->dChannelBandwidth);
		break;
	case OFDM:	
		return calculate_BER(phy->PHY_TYPE.ofdmPhy.modulation,dReceivedPower,phy->dChannelBandwidth);
		break;
	case HT:
		return calculate_BER(phy->PHY_TYPE.ofdmPhy_11n.modulation,dReceivedPower,phy->dChannelBandwidth);
		break;
	case VHT:
		return calculate_BER(phy->PHY_TYPE.ofdmPhy_11ac.modulation,dReceivedPower,phy->dChannelBandwidth);
		break;
	default:
		fnNetSimError("IEEE802.11-- Unknown PHY TYPE %d in %s\n",phy->PhyType,__FUNCTION__);
		return 0;
		break;
	}
}
