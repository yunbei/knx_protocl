#include <stdlib.h>

#include "KnxInterface.h"

typedef struct {

} GroupAddrAssociationTable[100];

typedef struct {
    GroupAddrAssociationTable gaat;
    int verify_flag;
} AL_PARAMETER;

TSAP AL_aspa2tsap(ASAP asap)
{
    // TODO:
    return asap;
}

ASAP AL_tsap2asap(TSAP tsap)
{
    // TODO:
    return tsap;
}

typedef struct {

} AL_PARAMETER;

static AL_INF *AL_inf;
static AL_PARAMETER AL_param;


static int AL_groupValueRead_req(AL_CALL_PARAM *param, ACK_REQUEST ack_request)
{
    // TODO:
    TL_INF *tl = TL_getInterface();

    TL_CALL_PARAM t_param;
    t_param.hop_count_type = param->hop_count_type;
    t_param.priority = param->priority;
    t_param.tsap = AL_aspa2tsap(param->asap);
    t_param.tsdu = pdu_alloc();
    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)t_param.tsdu;
    fl->ap_ci = 0;

    tl->dataGroup_req(&t_param, ack_request);
}

static void AL_groupValueRead_lcon(AL_CALL_PARAM *param, int n_status)
{
    // TODO:
}

static void AL_groupValueRead_ind(AL_CALL_PARAM *param)
{
    // TODO:
}

static int AL_groupValueRead_res(AL_CALL_PARAM *param, ACK_REQUEST ack_request)
{
    // TODO:
}

static void AL_groupValueRead_rcon(AL_CALL_PARAM *param, int n_status)
{
    // TODO:
}

static void AL_groupValueRead_acon(AL_CALL_PARAM *param)
{
    // TODO:
}





// CALLBACK
// Data_Group Service
static void AL_dataGroup_con_cb(TL_CALL_PARAM *param, int n_status);
static void AL_dataGroup_ind_cb(TL_CALL_PARAM *param);

// Data_Tag_Group service
static void AL_dataTagGroup_con_cb(TL_CALL_PARAM *param, int n_status);
static void AL_dataTagGroup_ind_cb(TL_CALL_PARAM *param);

// Data_Broadcast Service
static void AL_dataBroadcast_con_cb(TL_CALL_PARAM *param, int n_status);
static void AL_dataBroadcast_ind_cb(TL_CALL_PARAM *param);

// T_Data_SystemBroadcast
static void AL_dataSystemBroadcast_con_cb(TL_CALL_PARAM *param, int n_status);
static void AL_dataSystemBroadcast_ind_cb(TL_CALL_PARAM *param);

// T_Data_Individual
static void AL_dataIndividual_con_cb(TL_CALL_PARAM *param, int n_status);
static void AL_dataIndividual_ind_cb(TL_CALL_PARAM *param);

// T_Connect Service
static void AL_connect_con_cb(ADDRESS destination_address, TSAP tsap, int n_status);
static void AL_connect_ind_cb(TSAP tsap);

// T_Disconnect Service
static void AL_disconnect_con_cb(ADDRESS destination_address, TSAP tsap, int n_status);
static void AL_disconnect_ind_cb(TSAP tsap);

// T_Data_Connected Service
static void AL_dataConnected_con_cb(TSAP tsap);
static void AL_dataConnected_ind_cb(int octet_count, MEDIA_ACCESS_PRIORITY priority, TSAP tsap, DATA_TYPE tsdu);


AL_INF* AL_getInterface()
{
    return AL_inf;
}

void AL_init()
{
    AL_inf = (AL_INF *)malloc(sizeof(AL_inf));
    // TODO: init default interface member
}