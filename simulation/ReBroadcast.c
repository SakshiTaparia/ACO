/************************************************************************************
* Copyright (C) 2018                                                               *
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
#include "Application.h"

#define REBROADCAST_PROBABILITY 1.0
#define MAX_WAIT_FOR_REBROADCAST (100*SECOND)

static bool isRebroadcast()
{
	double d = NETSIM_RAND_01();
	if (d <= REBROADCAST_PROBABILITY)
		return true;
	else
		return false;
}

typedef struct stru_broadcast_info
{
	NETSIM_ID appId;
	NETSIM_ID srcId;
	UINT64 packetId;
	double dBroadcastTime;
	struct stru_broadcast_info* next;
}BROADCASTINFO, * ptrBROADCASTINFO;
static ptrBROADCASTINFO headInfo = NULL;
static ptrBROADCASTINFO tailInfo = NULL;

static void cleanup_broadcast_info(double time)
{
	ptrBROADCASTINFO b = headInfo;
	while (b)
	{
		if (time - b->dBroadcastTime > MAX_WAIT_FOR_REBROADCAST)
		{
			headInfo = b->next;
			if (!headInfo)
				tailInfo = NULL;
			free(b);
			b = headInfo;
		}
		else
		{
			break;
		}
	}
}

void rebroadcast_add_packet_to_info(NetSim_PACKET * packet,
	double time)
{
	cleanup_broadcast_info(time);
	ptrBROADCASTINFO b = calloc(1, sizeof * b);
	b->appId = packet->pstruAppData->nApplicationId;
	b->dBroadcastTime = time;
	b->packetId = packet->nPacketId;
	b->srcId = packet->nSourceId;
	if (headInfo)
	{
		tailInfo->next = b;
		tailInfo = b;
	}
	else
	{
		headInfo = b;
		tailInfo = b;
	}
}

static bool isRebroadcastAllowed(NetSim_PACKET * packet,
	NETSIM_ID devId)
{
	if (!isRebroadcast())
		return false;

	ptrBROADCASTINFO t = headInfo;
	while (t)
	{
		if (t->appId == packet->pstruAppData->nApplicationId &&
			t->packetId == packet->nPacketId &&
			t->srcId == devId)
			return false;
		t = t->next;
	}
	return true;
}


static NetSim_PACKET* reset_packet(NetSim_PACKET * packet,
	NETSIM_ID newSrc,
	double time)
{
	NetSim_PACKET* newPacket = fn_NetSim_Packet_CreatePacket(APPLICATION_LAYER);
	newPacket->dEventTime = time;
	newPacket->nSourceId = packet->nSourceId;
	newPacket->pstruNetworkData->szSourceIP = DEVICE_NWADDRESS(newSrc, 1);
	add_dest_to_packet(newPacket, 0);
	newPacket->pstruNetworkData->szDestIP = IP_COPY(packet->pstruNetworkData->szDestIP);
	newPacket->nControlDataType = packet->nControlDataType;
	newPacket->nPacketId = packet->nPacketId;
	newPacket->nPacketPriority = packet->nPacketPriority;
	newPacket->nPacketType = packet->nPacketType;
	newPacket->nQOS = packet->nQOS;
	newPacket->nServiceType = packet->nServiceType;
	strcpy(newPacket->szPacketType, packet->szPacketType);
	newPacket->pstruAppData->dPayload = packet->pstruAppData->dPayload;
	newPacket->pstruAppData->nApplicationId = packet->pstruAppData->nApplicationId;
	newPacket->pstruAppData->nAppType = packet->pstruAppData->nAppType;
	newPacket->pstruAppData->nApplicationProtocol = PROTOCOL_APPLICATION;
	newPacket->pstruTransportData->nDestinationPort = packet->pstruTransportData->nDestinationPort;
	newPacket->pstruTransportData->nSourcePort = packet->pstruTransportData->nSourcePort;
	newPacket->pstruNetworkData->nTTL = MAX_TTL;
	return newPacket;
}

void rebroadcast_packet(NetSim_PACKET * packet,
	NETSIM_ID devId,
	double time)
{
	if (!isRebroadcastAllowed(packet, devId))
	{
		fn_NetSim_Packet_FreePacket(packet);
		return;
	}

	NetSim_PACKET* newPacket = reset_packet(packet, devId, time);
	fn_NetSim_Packet_FreePacket(packet);

	ptrSOCKETINTERFACE s;
	UINT destCount;
	NETSIM_ID* dest = get_dest_from_packet(newPacket, &destCount);
	s = fnGetSocket(pstruEventDetails->nApplicationId,
		newPacket->nSourceId,
		destCount,
		dest,
		newPacket->pstruTransportData->nSourcePort,
		newPacket->pstruTransportData->nDestinationPort);

	if (!s)
	{
		APP_INFO* info = appInfo[newPacket->pstruAppData->nApplicationId - 1];
		NETSIM_ID oldSrc = info->sourceList[0];
		info->sourceList[0] = newPacket->nSourceId;
		//Create new socket for further rebroadcast
		fnCreateSocketBuffer(info);
		info->sourceList[0] = oldSrc;
	}

	pstruEventDetails->nEventType = APPLICATION_OUT_EVENT;
	pstruEventDetails->dPacketSize = newPacket->pstruAppData->dPayload;
	pstruEventDetails->szOtherDetails = appInfo[newPacket->pstruAppData->nApplicationId - 1];
	pstruEventDetails->pPacket = newPacket;
	pstruEventDetails->nProtocolId = PROTOCOL_APPLICATION;
	fnpAddEvent(pstruEventDetails);
}