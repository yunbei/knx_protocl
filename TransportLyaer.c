#include <stdlib.h>
#include <string.h>

#include "KnxInterface.h"

typedef struct {
    UINT16 length;
    ADDRESS group_addr[100];
} GroupAddrTable;

typedef struct {
    GroupAddrTable connection_number_list;
    int connection_timeout;
    int acknowledgement_timeout;
    int max_rep_count;
} TL_PARAMETER;

static TL_INF *TL_inf;
static TL_PARAMETER TL_param;

//  connection oriented data
typedef enum {
    CONN_CLOSED,
    CONN_OPEN_IDLE,
    CONN_OPEN_WAIT
} CONN_STATE;

typedef void (*TL_CONN_ACTION)(void);

static TL_CONN_ACTION TL_conn_action[15];

typedef struct {
    // int event;
    CONN_STATE next_status;
    int action_idx;
} TL_CONN_SM;

typedef struct {
    ADDRESS connection_address;
    CONN_STATE status;
    UINT8 SeqNoSend;
    UINT8 SeqNoRcv;
    UINT8 connection_timeout_timer;
    UINT8 acknowledgment_timeout_timer;
    UINT8 rep_count;
    UINT8 valid;
} TL_CONN_SESSION;

static TL_CONN_SM TL_conn_SM[][3] = {
//              CLOSED               OPEN_IDLE               OPEN_WAIT
/* 0 */    { {CONN_OPEN_IDLE, 1},   {CONN_CLOSED, 6},       {CONN_CLOSED, 6} },  // event 0
/* 1 */    { {CONN_OPEN_IDLE, 1},   {CONN_OPEN_IDLE, 10},   {CONN_OPEN_WAIT, 10} },  // event 1
/* 2 */    { {CONN_CLOSED, 0},      {CONN_CLOSED, 5},      {CONN_OPEN_WAIT, 5} },  // event 2
/* 3 */    { {CONN_CLOSED, 0},      {CONN_OPEN_IDLE, 0},    {CONN_OPEN_WAIT, 0} },  // event 3
/* 4 */    { {CONN_CLOSED, 10},     {CONN_OPEN_IDLE, 2},    {CONN_OPEN_WAIT, 2} },  // event 4
/* 5 */    { {CONN_CLOSED, 10},     {CONN_OPEN_IDLE, 3},    {CONN_OPEN_WAIT, 3} },  // event 5
/* 6 */    { {CONN_CLOSED, 10},     {CONN_OPEN_IDLE, 4},    {CONN_OPEN_WAIT, 4} },  // event 6
/* 7 */    { {CONN_CLOSED, 10},     {CONN_OPEN_IDLE, 10},   {CONN_OPEN_WAIT, 10} },  // event 7
/* 8 */    { {CONN_CLOSED, 10},     {CONN_CLOSED, 6},       {CONN_OPEN_IDLE, 8} },  // event 8
/* 9 */    { {CONN_CLOSED, 10},     {CONN_CLOSED, 6},       {CONN_CLOSED, 6} },  // event 9
/*10 */    { {CONN_CLOSED, 10},     {CONN_OPEN_IDLE, 10},   {CONN_OPEN_WAIT, 10} },  // event 10
/*11 */    { {CONN_CLOSED, 10},     {CONN_CLOSED, 6},       {CONN_CLOSED, 6} },  // event 11
/*12 */    { {CONN_CLOSED, 10},     {CONN_CLOSED, 6},       {CONN_OPEN_WAIT, 9} },  // event 12
/*13 */    { {CONN_CLOSED, 10},     {CONN_CLOSED, 6},       {CONN_CLOSED, 6} },  // event 13
/*14 */    { {CONN_CLOSED, 10},      {CONN_OPEN_IDLE, 10},   {CONN_OPEN_WAIT, 10} },  // event 14
/*15 */    { {CONN_CLOSED, 5},      {CONN_OPEN_WAIT, 7},    {CONN_CLOSED, 6} },  // event 15
/*16 */    { {CONN_CLOSED, 0},      {CONN_CLOSED, 6},       {CONN_CLOSED, 6} },  // event 16
/*17 */    { {CONN_CLOSED, 0},      {CONN_OPEN_IDLE, 0},    {CONN_OPEN_WAIT, 9} },  // event 17
/*18 */    { {CONN_CLOSED, 0},      {CONN_OPEN_IDLE, 0},    {CONN_CLOSED, 6} },  // event 18
/*19 */    { {CONN_CLOSED, 0},      {CONN_OPEN_IDLE, 13},   {CONN_OPEN_WAIT, 13} },  // event 19
/*20 */    { {CONN_CLOSED, 0},      {CONN_CLOSED, 5},       {CONN_CLOSED, 5} },  // event 20
/*21 */    { {CONN_CLOSED, 0},      {CONN_OPEN_IDLE, 0},    {CONN_OPEN_WAIT, 0} },  // event 21
/*22 */    { {CONN_CLOSED, 0},      {CONN_OPEN_IDLE, 0},    {CONN_OPEN_WAIT, 0} },  // event 22
/*23 */    { {CONN_CLOSED, 0},      {CONN_OPEN_IDLE, 0},    {CONN_OPEN_WAIT, 0} },  // event 23
/*24 */    { {CONN_CLOSED, 0},      {CONN_OPEN_IDLE, 0},    {CONN_OPEN_WAIT, 0} },  // event 24
/*25 */    { {CONN_OPEN_IDLE, 12},  {CONN_CLOSED, 6},       {CONN_CLOSED, 6} },  // event 25
/*26 */    { {CONN_CLOSED, 15},     {CONN_CLOSED, 14},      {CONN_CLOSED, 14} },  // event 26
/*27 */    { {CONN_CLOSED, 0},      {CONN_OPEN_IDLE, 0},    {CONN_OPEN_WAIT, 0} }   // event 27
};

#define TL_CONNSESS_MAX 20
static TL_CONN_SESSION TL_connSess[TL_CONNSESS_MAX];

static void TL_CONNECT_SM(ADDRESS addr, int e, int refused)
{
    int i;
    for (int i = 0; i < TL_CONNSESS_MAX; i++) {
        if (addr == TL_connSess[i].address) {
            if (!TL_connSess[i].valid) {
                break;
            }
        }
    }

    if (i < TL_CONNSESS_MAX) {
        int action_idx = TL_conn_SM[e][TL_connSess[i].status].action_idx;
        TL_conn_action[action_idx]();
        TL_connSess[i].status = TL_conn_SM[e][TL_connSess[i].status].next_status;
    }
}

ADDRESS TL_tsap2addr(TSAP tsap)
{
    GroupAddrTable *grat = &TL_param.connection_number_list;
    if (tsap+1 > grat->length) {
        // TODO: error handle
    }

    DEBUG("TL_tsap2addr %x, %x\n", tsap, grat->group_addr[tsap]);
    return grat->group_addr[tsap];
}

TSAP TL_addr2tsap(ADDRESS addr)
{
    GroupAddrTable *grat = &TL_param.connection_number_list;
    for (TSAP i = 0; i < grat->length; i++) {
        if (addr == grat->group_addr[i]) {
            return i;
        }
    }
    return TSAP_FAIL;
}

static ADDRESS TL_getIndividualAddr()
{
    return TL_param.connection_number_list.group_addr[0];
}

static void TL_param_init()
{
    memset(&TL_param, 0, sizeof(TL_param));
}

// T_Data_Group Service
static int TL_dataGroup_req(TL_CALL_PARAM *param, ACK_REQUEST ack_request)
{
    NL_INF* nl = NL_getInterface();

    //  group req, 0b1XXXXXXX 0b000000XX
    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)param->tsdu;
    fl->address_type = 1; // MULTICAST_ADDRESS;
    fl->tl_ctrl = 0x0;

    NL_CALL_PARAM n_param;
    n_param.destination_address = TL_tsap2addr(param->tsap);
    n_param.hop_count_type = param->hop_count_type;
    n_param.nsdu = param->tsdu;
    n_param.octet_count = param->octet_count;
    n_param.priority = param->priority;
    n_param.source_address = TL_getIndividualAddr();
    DEBUG("TL_dataGroup_req %x \n", n_param.source_address);

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

    DEBUG("TL_dataGroup_ind, %x \n", param->tsap);
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
    n_param.source_address = TL_getIndividualAddr();

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
    AL_setAPCI(fl, 0);

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
    AL_setAPCI(fl, 4);

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
    TL_param_init();

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
    TL_inf->dataIndividual_req = TL_dataIndividual_req;
    TL_inf->dataIndividual_con = TL_dataIndividual_con;
    TL_inf->dataIndividual_ind = TL_dataIndividual_ind;

    TL_inf->connect_req = TL_connect_req;
    TL_inf->connect_con = TL_connect_con;
    TL_inf->connect_ind = TL_connect_ind;
    TL_inf->disconnect_req = TL_disconnect_req;
    TL_inf->disconnect_con = TL_disconnect_con;
    TL_inf->disconnect_ind = TL_disconnect_ind;
}

// MOCK function
void MOCK_setIndividualAddr(ADDRESS addr)
{
    GroupAddrTable *grat = &TL_param.connection_number_list;
    grat->group_addr[0] = addr;
}

void MOCK_addGrAT(ADDRESS addr)
{
    GroupAddrTable *grat = &TL_param.connection_number_list;
    if (grat->length == 0) {
        grat->length++;
    }
    grat->group_addr[grat->length] = addr;
    grat->length++;
}