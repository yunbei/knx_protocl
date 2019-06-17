#include <stdlib.h>

#include "KnxInterface.h"

typedef struct {
    int hop_count;
    int device_type; // TODO: need redefine type
} NL_PARAMETER;

static NL_INF *NL_inf;
static NL_PARAMETER NL_param;

// N_Data_Individual service
static int NL_dataIndividual_req(NL_CALL_PARAM *param, ACK_REQUEST ack_request)
{
    DL_INF* dl = DL_getInterface();

    DL_CALL_PARAM d_param;
    d_param.address_type = INDIVIDUAL_ADDRESS;
    d_param.destination_address = param->destination_address;
    d_param.frame_format = STANDARD_FORMAT; // TODO: whereis frame format come from ???
    d_param.lsdu = param->nsdu;
    d_param.octet_count = param->octet_count;
    d_param.priority = param->priority;
    d_param.source_address = 0; // TODO: whereis source address come from ???

    //  FIXME: set hop count
    // OCTET5* oct5 = (OCTET5*)&d_param.lsdu[5];
    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)param->nsdu;
    // oct5->hop_count = param->hop_count_type == HOP_COUNT_7 ? 7 : param->hop_count_type;
    fl->hop_count = param->hop_count_type == HOP_COUNT_7 ? 7 : param->hop_count_type;
    dl->data_req(&d_param, ack_request);

    return 0;
}

static void NL_dataIndividual_con(NL_CALL_PARAM *param, int n_status)
{
    // TODO:
    TL_INF* tl = TL_getInterface();

    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)param->nsdu;
    unsigned int addr_type = fl->address_type;
    if (addr_type != INDIVIDUAL_ADDRESS) {
        // TODO: handle error
    }

    int is_ctrl = !!(fl->tl_ctrl&0x20);
    if (is_ctrl) {
        int is_ack = !!(fl->tl_ctrl&0x10);
        if (is_ack) {
            // TODO:
        } else {
            int is_disconn = !!(fl->ap_ci&0x4);
            if (is_disconn) {
                // TODO:
                tl->disconnect_con(param->priority, TL_addr2tsap(param->source_address), n_status);
            } else {
                // TODO:
                tl->connect_con(param->priority, TL_addr2tsap(param->source_address), n_status);
            }
        }
    } else {
        int seqno = !!(fl->tl_ctrl&0x10);
        if (seqno) {
            // TODO:
        } else {
            TL_CALL_PARAM t_param;
            t_param.hop_count_type = param->hop_count_type;
            t_param.octet_count = param->octet_count;
            t_param.priority = param->priority;
            t_param.tsap = TL_addr2tsap(param->destination_address);
            t_param.source_address = param->source_address;
            tl->dataIndividual_con(&t_param, n_status);
        }
    }
}

static void NL_dataIndividual_ind(NL_CALL_PARAM *param)
{
    TL_INF* tl = TL_getInterface();

    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)param->nsdu;
    unsigned int addr_type = fl->address_type;
    if (addr_type != INDIVIDUAL_ADDRESS) {
        // TODO: handle error
    }

    int is_ctrl = !!(fl->tl_ctrl&0x20);
    if (is_ctrl) {
        int is_ack = !!(fl->tl_ctrl&0x10);
        if (is_ack) {
            // TODO:
        } else {
            int is_disconn = !!(fl->ap_ci&0x4);
            if (is_disconn) {
                // TODO:
                tl->disconnect_ind(TL_addr2tsap(param->source_address));
            } else {
                // TODO:
                tl->connect_ind(TL_addr2tsap(param->source_address));
            }
        }
    } else {
        int seqno = !!(fl->tl_ctrl&0x10);
        if (seqno) {
            // TODO:
        } else {
            TL_CALL_PARAM t_param;
            t_param.hop_count_type = param->hop_count_type;
            t_param.octet_count = param->octet_count;
            t_param.priority = param->priority;
            t_param.tsap = TL_addr2tsap(param->destination_address);
            t_param.source_address = param->source_address;
            tl->dataIndividual_ind(&t_param);
        }
    }

    // fl->tl_ctrl = (0x0 << 2)|(fl->tl_ctrl&0x3);




    // TODO:
}

// N_Data_Group service
static int NL_dataGroup_req(NL_CALL_PARAM *param, ACK_REQUEST ack_request)
{
    DL_INF* dl = DL_getInterface();

    DL_CALL_PARAM d_param;
    d_param.address_type = MULTICAST_ADDRESS;
    d_param.destination_address = param->destination_address;
    d_param.frame_format = STANDARD_FORMAT; // TODO: whereis frame format come from ???
    d_param.lsdu = param->nsdu;
    d_param.octet_count = param->octet_count;
    d_param.priority = param->priority;
    d_param.source_address = 0; // TODO: whereis source address come from ???

    //  FIXME: set hop count
    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)param->nsdu;
    fl->hop_count = param->hop_count_type == HOP_COUNT_7 ? 7 : NL_param.hop_count;

    dl->data_req(&d_param, ack_request);

    return 0;
}

static void NL_dataGroup_con(NL_CALL_PARAM *param, int n_status)
{
    TL_INF *tl = TL_getInterface();

    TL_CALL_PARAM t_param;
    t_param.hop_count_type = param->hop_count_type;
    t_param.octet_count = param->octet_count;
    t_param.priority = param->priority;
    t_param.tsap = TL_addr2tsap(param->destination_address);
    t_param.source_address = param->source_address;

    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)param->nsdu;
    if (fl->tl_ctrl == 0) {
        tl->dataGroup_con(&t_param, n_status);
    } else {
        tl->dataTagGroup_con(&t_param, n_status);
    }
}

static void NL_dataGroup_ind(NL_CALL_PARAM *param)
{
    TL_INF *tl = TL_getInterface();

    TL_CALL_PARAM t_param;
    t_param.hop_count_type = param->hop_count_type;
    t_param.octet_count = param->octet_count;
    t_param.priority = param->priority;
    t_param.tsap = TL_addr2tsap(param->destination_address);
    t_param.source_address = param->source_address;

    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)param->nsdu;
    if (fl->tl_ctrl == 0) {
        tl->dataGroup_ind(&t_param);
    } else {
        tl->dataTagGroup_ind(&t_param);
    }
    // TODO:
}

// N_Data_Broadcast service
static int NL_dataBroadcast_req(NL_CALL_PARAM *param, ACK_REQUEST ack_request)
{
    DL_INF* dl = DL_getInterface();

    DL_CALL_PARAM d_param;
    d_param.address_type = MULTICAST_ADDRESS;
    d_param.destination_address = 0x0;  // TODO: Destination Address = ´0´
    d_param.frame_format = STANDARD_FORMAT; // TODO: whereis frame format come from ???
    d_param.lsdu = param->nsdu;
    d_param.octet_count = param->octet_count;
    d_param.priority = param->priority;
    d_param.source_address = 0; // TODO: whereis source address come from ???

    //  FIXME: set hop count
    // OCTET5* oct5 = (OCTET5*)&d_param.lsdu[5];
    // oct5->hop_count = param->hop_count_type == 7 ? 7 : param->hop_count_type;
    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)param->nsdu;
    fl->hop_count = param->hop_count_type == HOP_COUNT_7 ? 7 : NL_param.hop_count;

    dl->data_req(&d_param, ack_request);

    return 0;
}

static void NL_dataBroadcast_con(NL_CALL_PARAM *param, int n_status)
{
    TL_INF *tl = TL_getInterface();

    TL_CALL_PARAM t_param;
    t_param.hop_count_type = param->hop_count_type;
    t_param.octet_count = param->octet_count;
    t_param.priority = param->priority;
    t_param.source_address = param->source_address;

    tl->dataBroadcast_con(&t_param, n_status);
}

static void NL_dataBroadcast_ind(NL_CALL_PARAM *param)
{
    TL_INF *tl = TL_getInterface();

    TL_CALL_PARAM t_param;
    t_param.hop_count_type = param->hop_count_type;
    t_param.octet_count = param->octet_count;
    t_param.priority = param->priority;
    t_param.source_address = param->source_address;

    tl->dataBroadcast_ind(&t_param);
}

// N_Data_SystemBroadcast service
static int NL_dataSystemBroadcast_req(NL_CALL_PARAM *param, ACK_REQUEST ack_request)
{
    DL_INF* dl = DL_getInterface();

    DL_CALL_PARAM d_param;
    d_param.address_type = MULTICAST_ADDRESS;
    d_param.destination_address = 0x0;
    d_param.frame_format = STANDARD_FORMAT; // TODO: whereis frame format come from ???
    d_param.lsdu = param->nsdu;
    d_param.octet_count = param->octet_count;
    d_param.priority = param->priority;
    d_param.source_address = 0; // TODO: whereis source address come from ???

    //  FIXME: set hop count
    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)param->nsdu;
    fl->hop_count = param->hop_count_type == HOP_COUNT_7 ? 7 : NL_param.hop_count;

    dl->systemBroadcast_req(&d_param, ack_request);

    return 0;
}

static void NL_dataSystemBroadcast_con(NL_CALL_PARAM *param, int n_status)
{
    TL_INF *tl = TL_getInterface();

    TL_CALL_PARAM t_param;
    t_param.hop_count_type = param->hop_count_type;
    t_param.octet_count = param->octet_count;
    t_param.priority = param->priority;
    t_param.source_address = param->source_address;

    tl->dataBroadcast_con(&t_param, n_status);
}

static void NL_dataSystemBroadcast_ind(NL_CALL_PARAM *param)
{
    TL_INF *tl = TL_getInterface();

    TL_CALL_PARAM t_param;
    t_param.hop_count_type = param->hop_count_type;
    t_param.octet_count = param->octet_count;
    t_param.priority = param->priority;
    t_param.source_address = param->source_address;

    tl->dataBroadcast_ind(&t_param);
}

NL_INF* NL_getInterface()
{
    return NL_inf;
}

void NL_init()
{
    NL_inf = (NL_INF *)malloc(sizeof(NL_INF));
    // TODO: init default interface member
    NL_inf->dataIndividual_req = NL_dataIndividual_req;
    NL_inf->dataIndividual_con = NL_dataIndividual_con;
    NL_inf->dataIndividual_ind = NL_dataIndividual_ind;
    NL_inf->dataGroup_req = NL_dataGroup_req;
    NL_inf->dataGroup_con = NL_dataGroup_con;
    NL_inf->dataGroup_ind = NL_dataGroup_ind;
    NL_inf->dataBroadcast_req = NL_dataBroadcast_req;
    NL_inf->dataBroadcast_con = NL_dataBroadcast_con;
    NL_inf->dataBroadcast_ind = NL_dataBroadcast_ind;
    NL_inf->dataSystemBroadcast_req = NL_dataSystemBroadcast_req;
    NL_inf->dataSystemBroadcast_con = NL_dataSystemBroadcast_con;
    NL_inf->dataSystemBroadcast_ind = NL_dataSystemBroadcast_ind;
}