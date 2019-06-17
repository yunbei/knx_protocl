#include <stdlib.h>

#include "KnxInterface.h"

typedef struct {

} GroupAddrTable[100];

typedef struct {
    GroupAddrTable connection_number_list;
    int connection_timeout;
    int acknowledgement_timeout;
    int max_rep_count;
} TL_PARAMETER;

static TL_INF *TL_inf;
static TL_PARAMETER TL_param;

ADDRESS TL_tsap2addr(TSAP tsap)
{
    // TODO:
    return tsap;
}

TSAP TL_addr2tsap(ADDRESS addr)
{
    // TODO:
    return addr;
}

// T_Data_Group Service
static int TL_dataGroup_req(TL_CALL_PARAM *param, ACK_REQUEST ack_request)
{
    NL_INF* nl = NL_getInterface();

    //  group req, 0b1XXXXXXX 0b000000XX
    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)param->tsdu;
    fl->address_type = MULTICAST_ADDRESS;
    fl->tl_ctrl = 0x0;

    NL_CALL_PARAM n_param;
    n_param.destination_address = TL_tsap2addr(param->tsap);
    n_param.hop_count_type = param->hop_count_type;
    n_param.nsdu = param->tsdu;
    n_param.octet_count = param->octet_count;
    n_param.priority = param->priority;

    nl->dataGroup_req(&n_param, ack_request);

    return 0;
}

static void TL_dataGroup_con(TL_CALL_PARAM *param, int n_status)
{
    // TODO:
}

static void TL_dataGroup_ind(TL_CALL_PARAM *param)
{
    // TODO:
    AL_INF* al = AL_getInterface();

    al->dataGroup_ind_cb(param);
}

// T_Data_Tag_Group service
static int TL_dataTagGroup_req(TL_CALL_PARAM *param, ACK_REQUEST ack_request)
{
    NL_INF* nl = NL_getInterface();

    //  tag group req, 0b1XXXXXXX 0b000001XX
    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)param->tsdu;
    fl->address_type = MULTICAST_ADDRESS;
    fl->tl_ctrl = 0x1;

    NL_CALL_PARAM n_param;
    n_param.destination_address = TL_tsap2addr(param->tsap);
    n_param.hop_count_type = param->hop_count_type;
    n_param.nsdu = param->tsdu;
    n_param.octet_count = param->octet_count;
    n_param.priority = param->priority;

    nl->dataGroup_req(&n_param, ack_request);

    return 0;
}

static void TL_dataTagGroup_con(TL_CALL_PARAM *param, int n_status)
{
    // TODO:
}

static void TL_dataTagGroup_ind(TL_CALL_PARAM *param)
{
    // TODO:
}

// T_Data_Broadcast Service
static int TL_dataBroadcast_req(TL_CALL_PARAM *param, ACK_REQUEST ack_request)
{
    NL_INF* nl = NL_getInterface();

    //  tag group req, 0b1XXXXXXX 0b000001XX
    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)param->tsdu;
    fl->address_type = MULTICAST_ADDRESS;
    fl->tl_ctrl = 0x0;

    NL_CALL_PARAM n_param;
    n_param.destination_address = 0;
    n_param.hop_count_type = param->hop_count_type;
    n_param.nsdu = param->tsdu;
    n_param.octet_count = param->octet_count;
    n_param.priority = param->priority;

    nl->dataBroadcast_req(&n_param, ack_request);

    return 0;
}

static void TL_dataBroadcast_con(TL_CALL_PARAM *param, int n_status)
{
    // TODO:
}

static void TL_dataBroadcast_ind(TL_CALL_PARAM *param)
{
    // TODO:
}

// T_Data_SystemBroadcast
static int TL_dataSystemBroadcast_req(TL_CALL_PARAM *param, ACK_REQUEST ack_request)
{
    NL_INF* nl = NL_getInterface();

    //  TODO:
    // FRAME_LAYOUT *fl = (FRAME_LAYOUT *)param->tsdu;
    // fl->address_type = 1;
    // fl->tl_ctrl = (0x0 << 2)|(fl->tl_ctrl&0x3);

    NL_CALL_PARAM n_param;
    // n_param.destination_address = TL_tsap2addr(param->tsap);
    n_param.hop_count_type = param->hop_count_type;
    n_param.nsdu = param->tsdu;
    n_param.octet_count = param->octet_count;
    n_param.priority = param->priority;

    nl->dataSystemBroadcast_req(&n_param, ack_request);

    return 0;
}

static void TL_dataSystemBroadcast_con(TL_CALL_PARAM *param, int n_status)
{
    // TODO:
}

static void TL_dataSystemBroadcast_ind(TL_CALL_PARAM *param)
{
    // TODO:
}

// T_Data_Individual
static int TL_dataIndividual_req(TL_CALL_PARAM *param, ACK_REQUEST ack_request)
{
    NL_INF* nl = NL_getInterface();

    //  TODO:
    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)param->tsdu;
    fl->address_type = INDIVIDUAL_ADDRESS;
    fl->tl_ctrl = (0x0 << 2)|(fl->tl_ctrl&0x3);

    NL_CALL_PARAM n_param;
    n_param.destination_address = TL_tsap2addr(param->tsap);;
    n_param.hop_count_type = param->hop_count_type;
    n_param.nsdu = param->tsdu;
    n_param.octet_count = param->octet_count;
    n_param.priority = param->priority;

    nl->dataIndividual_req(&n_param, ack_request);

    return 0;
}

static void TL_dataIndividual_con(TL_CALL_PARAM *param, int n_status)
{
    // TODO:
}

static void TL_dataIndividual_ind(TL_CALL_PARAM *param)
{
    // TODO:
}

// T_Connect Service
static int TL_connect_req(ADDRESS destination_address, MEDIA_ACCESS_PRIORITY priority)
{
    if (priority != PRIORITY_SYSTEM) {
        // TODO: error handle
    }

    NL_INF* nl = NL_getInterface();

    DATA_TYPE tsdu = pdu_alloc();
    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)tsdu;
    fl->address_type = INDIVIDUAL_ADDRESS;
    // octoet 6: 0b1000_0000
    fl->tl_ctrl = 0x20;
    fl->ap_ci = 0;

    NL_CALL_PARAM n_param;
    n_param.destination_address = destination_address;
    n_param.hop_count_type = HOP_COUNT_NLP;
    n_param.nsdu = tsdu;
    n_param.octet_count = 6;
    n_param.priority = priority;

    nl->dataIndividual_req(&n_param, ACK_REQUESTED);

    return 0;
}

static void TL_connect_con(ADDRESS destination_address, TSAP tsap, int n_status)
{
    // TODO:
}

static void TL_connect_ind(TSAP tsap)
{
    // TODO: if enable connect
    // TODO:
}

// T_Disconnect Service
static int TL_disconnect_req(ADDRESS destination_address, MEDIA_ACCESS_PRIORITY priority)
{
    if (priority != PRIORITY_SYSTEM) {
        // TODO: error handle
    }

    NL_INF* nl = NL_getInterface();

    DATA_TYPE tsdu = (DATA_TYPE)malloc(120); // FIXME: buffer alloc
    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)tsdu;
    fl->address_type = INDIVIDUAL_ADDRESS;
    // octoet 6: 0b1000_0001
    fl->tl_ctrl = 0x20;
    fl->ap_ci = 0x04;

    NL_CALL_PARAM n_param;
    n_param.destination_address = destination_address;
    n_param.hop_count_type = HOP_COUNT_NLP;
    n_param.nsdu = tsdu;
    n_param.octet_count = 6;
    n_param.priority = priority;

    nl->dataIndividual_req(&n_param, ACK_REQUESTED);

    return 0;
}

static void TL_disconnect_con(ADDRESS destination_address, TSAP tsap, int n_status)
{
    // TODO:
}

static void TL_disconnect_ind(TSAP tsap)
{
    // TODO: if enable connect
    // TODO:
}

TL_INF* TL_getInterface()
{
    return TL_inf;
}

void TL_init()
{
    TL_inf = (TL_INF *)malloc(sizeof(TL_INF));

    TL_inf->dataGroup_req = TL_dataGroup_req;
    TL_inf->dataGroup_con = TL_dataGroup_con;
    TL_inf->dataGroup_ind = TL_dataGroup_ind;
    TL_inf->dataTagGroup_req = TL_dataTagGroup_req;
    TL_inf->dataTagGroup_con = TL_dataTagGroup_con;
    TL_inf->dataTagGroup_ind = TL_dataTagGroup_ind;
    TL_inf->dataBroadcast_req = TL_dataBroadcast_req;
    TL_inf->dataBroadcast_con = TL_dataBroadcast_con;
    TL_inf->dataBroadcast_ind = TL_dataBroadcast_ind;
    TL_inf->dataSystemBroadcast_req = TL_dataSystemBroadcast_req;
    TL_inf->dataSystemBroadcast_con = TL_dataSystemBroadcast_con;
    TL_inf->dataSystemBroadcast_ind = TL_dataSystemBroadcast_ind;

    TL_inf->connect_req = TL_connect_req;
    TL_inf->connect_con = TL_connect_con;
    TL_inf->connect_ind = TL_connect_ind;
}
