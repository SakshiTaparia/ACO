/************************************************************************************
 * Copyright (C) 2014                                                               *
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
#ifndef _NETSIM_APPLICATION_H_
#define _NETSIM_APPLICATION_H_

 //#define AES_ENCRYPT
 //#define DES_ENCRYPT


#include "main.h"
#include "Stack.h"
#define roundoff(d) ((long)(d+0.5))
#define MAX_TTL 255
#define MAX_PORT 65535

#define VANET_APP_RANDOM_WAIT_TIME_DEFAULT	5*MILLISECOND // SAE j2735
#define RANDOM_STARTUP_DELAY				0.1*SECOND

unsigned int nApplicationCount;
typedef struct stru_Application_Info APP_INFO;
typedef struct stru_Application_DataInfo APP_DATA_INFO;
typedef struct stru_Application_VoiceInfo APP_VOICE_INFO;
typedef struct stru_Application_VideoInfo APP_VIDEO_INFO;
typedef struct stru_Application_HTTPInfo APP_HTTP_INFO;
typedef struct stru_Application_EMAILInfo APP_EMAIL_INFO;
typedef struct stru_Application_P2PInfo APP_P2P_INFO;
typedef struct stru_Application_CallInfo APP_CALL_INFO;
typedef	struct stru_Application_EmulationInfo APP_EMULATION_INFO;
typedef struct stru_Application_COAPInfo APP_COAP_INFO;
/// Enumeration for application events
typedef enum
{
	event_APP_START = PROTOCOL_APPLICATION * 100 + 1,
	event_APP_END,
	event_APP_RESTART,
}APP_EVENT;

typedef enum
{
	Encryption_None,
	Encryption_XOR,
	Encryption_TEA,
	Encryption_AES,
	Encryption_DES,
}ENCRYPTION;

/// Enumeration for application control packets
typedef enum
{
	packet_HTTP_REQUEST = PROTOCOL_APPLICATION * 100 + 1,
	packet_COAP_REQUEST = PROTOCOL_APPLICATION * 100 + 2,
}APP_CONTROL_PACKET;

/// Enumeration for application state.
typedef enum
{
	appState_Started,
	appState_Paused,
	appState_Terminated,
}APP_STATE;

/// Structure to store socket information
struct stru_SocketInfo
{
	NETSIM_ID nSourceId;
	UINT destCount;
	NETSIM_ID* nDestinationId;
	NETSIM_ID nSourcePort;
	NETSIM_ID nDestPort;
	ptrSOCKETINTERFACE s;
	_ele* ele;
};
#define SOCKETINFO_ALLOC() (struct stru_SocketInfo*)list_alloc(sizeof(struct stru_SocketInfo),offsetof(struct stru_SocketInfo,ele))
/// Structure to store application information
struct stru_Application_Info
{
	//config variable
	NETSIM_ID id;
	NETSIM_ID nConfigId;
	TRANSMISSION_TYPE nTransmissionType;
	APPLICATION_TYPE nAppType;
	unsigned int nSourceCount;
	unsigned int nDestCount;
	NETSIM_ID* sourceList;
	NETSIM_ID* destList;
	double dStartTime;
	double dEndTime;
	double dGenerationRate;
	char* name;
	QUALITY_OF_SERVICE qos;
	TRANSPORT_LAYER_PROTOCOL trxProtocol;

	unsigned long long int nPacketId;
	UINT16 sourcePort;
	UINT16 destPort;
	APP_STATE nAppState;
	void* appMetrics;
	void* appData;

	ENCRYPTION encryption;
	struct stru_SocketInfo* socketInfo;

	//Broadcast Application
	NETSIM_ID* recvList;
	unsigned int* recvPort;

	//Startup delay
	bool isStartupDelay;

	//Multicast application
	NETSIM_IPAddress multicastDestIP;

};
APP_INFO** appInfo;

/// Structure for Data information such as packet size and inter arrival time,this is applicable for custom,FTP,Database Traffic
struct stru_Application_DataInfo
{
	DISTRIBUTION packetSizeDistribution;
	double dPacketSize;
	DISTRIBUTION IATDistribution;
	double dIAT;
};

/// Structure for Data information such as packet size and inter arrival time,this is applicable for Vanet application
typedef struct stru_Application_BSMInfo
{
	DISTRIBUTION packetSizeDistribution;
	double dPacketSize;
	DISTRIBUTION IATDistribution;
	double dIAT;
	double dRandomWaitTime;
}APP_BSM_INFO, *PAPP_BSM_INFO;

/// Structure for voice information such as packet size, inter arrival time and service type,this is applicable for voice traffic
struct stru_Application_VoiceInfo
{
	DISTRIBUTION packetSizeDistribution;
	double dPacketSize;
	DISTRIBUTION IATDistribution;
	double dIAT;
	SERVICE_TYPE nServiceType;
	SUPPRESSION_MODEL nSuppressionModel;
	/*for Deterministic*/
	int nSuccessRatio;
	/*For Markov*/
	int nTSL;//Talk spurt length
	double dSAF;//Speech activity factor
};
/// structure to store the video application information
struct stru_Application_VideoInfo
{
	VIDEO_MODEL nVidModel;
	int fps;
	int ppf;
	double mu;
	double sigma;
	double const_a;
	double const_b;
	double eta;
	int Msource;
	int ql;
	double beta_I;
	double beta_P;
	double beta_B;
	double gamma_I;
	double gamma_P;
	double gamma_B;
	double eta_I;
	double eta_P;
	double eta_B;
	char* info;
};
///Structure to store the emulation application information
struct stru_Application_EmulationInfo
{
	bool isMulticast;
	bool isBroadcast;
	UINT count;
	NETSIM_ID nDestinationId;
	NETSIM_ID* nSourceId;
	NETSIM_ID nSourcePort;
	NETSIM_ID nDestinationPort;
	NETSIM_IPAddress* realSourceIP;
	NETSIM_IPAddress realDestIP;
	NETSIM_IPAddress* simSourceIP;
	NETSIM_IPAddress simDestIP;
};
/// Structure to store the HTTP application information
struct stru_Application_HTTPInfo
{
	double pageIAT;
	DISTRIBUTION pageIATDistribution;
	unsigned int pageCount;
	double pageSize;
	DISTRIBUTION pageSizeDistribution;
};

/// Structure to store the COAP application information	

struct stru_Application_COAPInfo
{
	double pageIAT;
	DISTRIBUTION pageIATDistribution;
	double pageSize;
	DISTRIBUTION pageSizeDistribution;
	bool ackRequired;
	bool multicastResponse;
	int variableResponseTime; //in miliseconds
	int piggybackedTime; //in miliseconds
	int ackResponseTime; //in milliseconds
	int ackTimeOut; //in milliseconds
	double ackRandomFactor;
	int maxRetransmit;
	int nstart;
	int defaultLeisure;
	int probingRate;

	struct stru_NetSim_COAP_data* pCOAPData;
};

/// Structure to store email application information
struct stru_Application_EMAILInfo
{
	struct stru_emailinfo
	{
		double dDuration;
		DISTRIBUTION durationDistribution;
		double dEmailSize;
		DISTRIBUTION sizeDistribution;
	}SEND, RECEIVE;
};

/// Structure for Seeder list of peer_to_peer traffic
typedef struct stru_Application_P2P_SeederList
{
	NetSim_PACKET* packet;
	unsigned int nSeederCount;
	NETSIM_ID* nDeviceIds;
}SEEDER_LIST;
/// Structure for peer list of peer_to_peer traffic
typedef struct stru_Application_p2P_PeerList
{
	NETSIM_ID nDeviceId;
	unsigned int yetToReceiveCount;
	unsigned int* flag;
}PEER_LIST;
/// Structure fot peer_to_peer application
struct stru_Application_P2PInfo
{
	double dFileSize;
	DISTRIBUTION fizeSizeDistribution;
	double dPiecesSize;

	unsigned int nPieceCount;	//Pieces count
	NetSim_PACKET** pieceList; //Pieces
	SEEDER_LIST** seederList; //For each pieces
	PEER_LIST** peerList;	//For each peers
};
/// Structure for Erlang_call application
struct stru_Application_CallInfo
{
	DISTRIBUTION callDurationDistribution;
	double dCallDuration;
	DISTRIBUTION callIATDistribution;
	double dCallIAT;
	int** nCallStatus;
	double **dCallEndTime;
	NetSim_PACKET*** nextPacket;
	APP_VOICE_INFO VoiceInfo;

	unsigned long long int** nEventId;
	unsigned long long int** nAppoutevent;
	int(*fn_BlockCall)(APP_INFO* appInfo, NETSIM_ID nSourceId, NETSIM_ID nDestinationId, double time);
};

//For Email.c
typedef struct stru_email_detail
{
	int type;
	APP_INFO* appInfo;
}DETAIL;

#include "NetSim_Plot.h"
//Statistics
bool nApplicationThroughputPlotFlag;
char* szApplicationThroughputPlotVal;
bool nApplicationThroughputPlotRealTime;
struct stru_GenericPlot** genericAppPlot;
bool* nAppPlotFlag;
double* dPayloadReceived;

NetSim_PACKET* fn_NetSim_Application_GeneratePacket(APP_INFO* info,
													double ldArrivalTime,
													NETSIM_ID nSourceId,
													UINT destCount,
													NETSIM_ID* nDestination,
													unsigned long long int nPacketId,
													APPLICATION_TYPE nAppType,
													QUALITY_OF_SERVICE nQOS,
													unsigned int sourcePort,
													unsigned int destPort);

int fn_NetSim_Application_GenerateNextPacket(APP_INFO* appInfo,
											 NETSIM_ID nSource,
											 UINT destCount,
											 NETSIM_ID* nDestination,
											 double time);

/* Distribution Function */
_declspec(dllexport) int fnDistribution(DISTRIBUTION nDistributionType, double *fDistOut,
										unsigned long *uSeed, unsigned long *uSeed1, double* args);

/* Random number generator */
_declspec(dllexport) int fnRandomNo(long lm, double *fRandNo, unsigned long *uSeed, unsigned long *uSeed1);

/* HTTP Application */
int fn_NetSim_Application_StartHTTPAPP(APP_INFO* appInfo, double time);
int fn_NetSim_Application_ConfigureHTTPTraffic(APP_INFO* appInfo, void* xmlNetSimNode);
int fn_NetSim_Application_HTTP_ProcessRequest(APP_INFO* pstruappinfo, NetSim_PACKET* pstruPacket);

/* COAP Application*/
int fn_NetSim_Application_StartCOAPAPP(APP_INFO* appInfo, double time);
int fn_NetSim_Application_ConfigureCOAPTraffic(APP_INFO* appInfo, void* xmlNetSimNode);
int fn_NetSim_Application_COAP_ProcessRequest(APP_INFO* pstruappinfo, NetSim_PACKET* pstruPacket);

/* Video Application */
int fn_NetSim_Application_StartVideoAPP(APP_INFO* appInfo, double time);
int fn_NetSim_Application_ConfigureVideoTraffic(APP_INFO* appInfo, void* xmlNetSimNode);
_declspec(dllexport) int fn_NetSim_TrafficGenerator_Video(APP_VIDEO_INFO* info,
														  double* fPacketSize,
														  double* ldArrival,
														  unsigned long* uSeed,
														  unsigned long* uSeed1,
														  unsigned long* uSeed2,
														  unsigned long* uSeed3);

/* Voice Application */
int fn_NetSim_Application_StartVoiceAPP(APP_INFO* appInfo, double time);
int fn_NetSim_Application_ConfigureVoiceTraffic(APP_INFO* appInfo, void* xmlNetSimNode);
_declspec(dllexport) int fn_NetSim_TrafficGenerator_Voice(APP_VOICE_INFO* info,
														  double* fSize,
														  double* ldArrival,
														  unsigned long* uSeed,
														  unsigned long* uSeed1,
														  unsigned long* uSeed2,
														  unsigned long* uSeed3);

/* Peer To Peer Application */
int fn_NetSim_Application_P2P_GenerateFile(APP_INFO* appInfo);
int fn_NetSim_Application_P2P_InitSeederList(APP_INFO* appInfo);
int fn_NetSim_Application_P2P_InitPeers(APP_INFO* appInfo);
int fn_NetSim_Application_P2P_SendNextPiece(APP_INFO* appInfo, NETSIM_ID destination, double time);
int fn_NetSim_Application_StartP2PAPP(APP_INFO* appInfo, double time);
int fn_NetSim_Application_ConfigureP2PTraffic(APP_INFO* appInfo, void* xmlNetSimNode);
int fn_NetSim_Application_P2P_MarkReceivedPacket(APP_INFO* pstruappinfo, NetSim_PACKET* pstruPacket);

/* Email Application */
int fn_NetSim_Application_StartEmailAPP(APP_INFO* appInfo, double time);
int fn_NetSim_Application_ConfigureEmailTraffic(APP_INFO* appInfo, void* xmlNetSimNode);
APP_INFO* fn_NetSim_Application_Email_GenerateNextPacket(DETAIL* detail, NETSIM_ID nSourceId, NETSIM_ID nDestinationId, double time);

/* Custom, FTP, Database Application */
int fn_NetSim_Application_StartDataAPP(APP_INFO* appInfo, double time);
int fn_NetSim_Application_ConfigureDataTraffic(APP_INFO* appInfo, void* xmlNetSimNode);
int fn_NetSim_Application_ConfigureDatabaseTraffic(APP_INFO* appInfo, void* xmlNetSimNode);
int fn_NetSim_Application_ConfigureFTPTraffic(APP_INFO* appInfo, void* xmlNetSimNode);
_declspec(dllexport) int fn_NetSim_TrafficGenerator_Custom(APP_DATA_INFO* info,
														   double* fSize,
														   double* ldArrival,
														   unsigned long* uSeed,
														   unsigned long* uSeed1,
														   unsigned long* uSeed2,
														   unsigned long* uSeed3);

/* Erlang Call */
int fn_NetSim_Application_ErlangCall_StartCall(NetSim_EVENTDETAILS* pstruEventDetails);
int fn_NetSim_Application_ErlangCall_EndCall(NetSim_EVENTDETAILS* pstruEventDetails);
int fn_NetSim_Application_StartErlangCallAPP(APP_INFO* appInfo, double time);
int fn_NetSim_Application_ConfigureErlangCallTraffic(APP_INFO* appInfo, void* xmlNetSimNode);

/* Emulation */
int fn_NetSim_Application_ConfigureEmulationTraffic(APP_INFO* appInfo, void* xmlNetSimNode);
void fn_NetSim_Emulation_StartApplication(APP_INFO* appInfo);

/* Vanet Application*/
int fn_NetSim_Application_StartBSM(APP_INFO* appInfo, double time);
int fn_NetSim_Application_BSM(PAPP_BSM_INFO info,
							  double* fSize,
							  double* ldArrival,
							  unsigned long* uSeed,
							  unsigned long* uSeed1,
							  unsigned long* uSeed2,
							  unsigned long* uSeed3);
bool add_sae_j2735_payload(NetSim_PACKET* packet, APP_INFO* info);
void process_saej2735_packet(NetSim_PACKET* packet);

//Multicast
void add_multicast_route(APP_INFO* info);
void join_multicast_group(APP_INFO* info, double time);


/* Application API's */
_declspec(dllexport) int fn_NetSim_Application_Init_F(struct stru_NetSim_Network *NETWORK_Formal,
													  NetSim_EVENTDETAILS *pstruEventDetails_Formal,
													  char *pszAppPath_Formal,
													  char *pszWritePath_Formal,
													  int nVersion_Type,
													  void **fnPointer);
_declspec(dllexport) int fn_NetSim_Application_Configure_F(void** var);
_declspec(dllexport) int fn_NetSim_Application_Metrics_F(PMETRICSWRITER metricsWriter);
int fn_NetSim_App_RestartApplication();
_declspec(dllexport) int fn_NetSim_Application_Plot(NetSim_PACKET* pstruPacket);
PACKET_TYPE fn_NetSim_Application_GetPacketTypeBasedOnApplicationType(APPLICATION_TYPE nAppType);
ptrSOCKETINTERFACE fnGetSocket(NETSIM_ID nAppId,
							   NETSIM_ID nSourceId,
							   UINT destCount,
							   NETSIM_ID* nDestinationId,
							   NETSIM_ID nSourcePort,
							   NETSIM_ID nDestPort);

//Application metrics
void free_app_metrics(APP_INFO* appInfo);
void appmetrics_src_add(APP_INFO* appInfo, NetSim_PACKET* packet);
void appmetrics_dest_add(APP_INFO* appInfo, NetSim_PACKET* packet, NETSIM_ID dest);
int fn_NetSim_Application_Metrics_F(PMETRICSWRITER metricsWriter);


//Application Interface function
void fnCreatePort(APP_INFO* info);
int fnCreateSocketBuffer(APP_INFO* appInfo);

int fn_NetSim_Add_DummyPayload(NetSim_PACKET* packet, APP_INFO*);

//Encryption
char xor_encrypt(char ch, long key);

/*****REBROADCAST *****/
#define REBROADCAST
void rebroadcast_add_packet_to_info(NetSim_PACKET* packet,
	double time);
void rebroadcast_packet(NetSim_PACKET* packet,
	NETSIM_ID devId,
	double time);


#endif
