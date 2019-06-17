#include <stdlib.h>

#include "KnxInterface.h"

typedef struct {
    ADDRESS Domain_Address;
    ADDRESS Individual_Address;
    int Group_Address_Table;    // TODO: need redefine type
    int Zone_Address_Table; // TODO: need redefine type
    int nak_retry;
    int busy_retry;
    ADDRESS poll_Group_Address;
    int response_slot_number;
    int Data_Link_Layer_mode;   // TODO: maybe a enum
} DL_PARAMETER;

static DL_INF *DL_inf;
static DL_PARAMETER DL_param;

// L_Data service
static int DL_data_req(DL_CALL_PARAM *param, ACK_REQUEST ack_request)
{
    // TODO:

    return 0;
}

static void DL_data_con(DL_CALL_PARAM *param, int l_status)
{
    NL_INF* nl = NL_getInterface();
    NL_CALL_PARAM n_param;
    n_param.destination_address = param->destination_address;
    // OCTET5* oct5 = (OCTET5*)&param->lsdu[5];
    // n_param.hop_count_type = oct5->hop_count == 7 ? HOP_COUNT_EQL_7 : HOP_COUNT_NOT_EQL_7;
    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)param->lsdu;
    n_param.hop_count_type = fl->hop_count == 7 ? HOP_COUNT_EQL_7 : HOP_COUNT_NOT_EQL_7;
    n_param.nsdu = param->lsdu;
    n_param.octet_count = param->octet_count;
    n_param.priority = param->priority;
    n_param.source_address = param->source_address;
    if (param->address_type == INDIVIDUAL_ADDRESS) {
        nl->dataIndividual_con(&n_param, l_status);
    } else if (param->address_type == MULTICAST_ADDRESS) {
        if (param->destination_address == 0) { // TODO: Destination Address = ´0´
            nl->dataBroadcast_con(&n_param, l_status);
        } else {
            nl->dataGroup_con(&n_param, l_status);
        }
    }

    // TODO:
}

static void DL_data_ind(DL_CALL_PARAM *param, ACK_REQUEST ack_request)
{
    NL_INF* nl = NL_getInterface();
    NL_CALL_PARAM n_param;
    n_param.destination_address = param->destination_address;
    // OCTET5* oct5 = (OCTET5*)&param->lsdu[5];
    // n_param.hop_count_type = oct5->hop_count == 7 ? HOP_COUNT_EQL_7 : HOP_COUNT_NOT_EQL_7;
    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)param->lsdu;
    n_param.hop_count_type = fl->hop_count == 7 ? HOP_COUNT_EQL_7 : HOP_COUNT_NOT_EQL_7;

    n_param.nsdu = param->lsdu;
    n_param.octet_count = param->octet_count;
    n_param.priority = param->priority;
    n_param.source_address = param->source_address;
    if (param->address_type == INDIVIDUAL_ADDRESS) {
        nl->dataIndividual_ind(&n_param);
    } else if (param->address_type == MULTICAST_ADDRESS) {
        if (param->destination_address == 0) {
            nl->dataBroadcast_ind(&n_param);
        } else {
            nl->dataGroup_ind(&n_param);
        }
    }

    // TODO:
}

// L_SystemBroadcast
static int DL_systemBroadcast_req(DL_CALL_PARAM *param, ACK_REQUEST ack_request)
{
    // check
    if (param->address_type != MULTICAST_ADDRESS) { // FIXME: multicast or group ???
        return -1;
    }

    if (param->destination_address != 0x0) {
        return -2;
    }



    // TODO:

    return 0;
}

static void DL_systemBroadcast_con(DL_CALL_PARAM *param, int l_status)
{
    // TODO:
}

static void DL_systemBroadcast_ind(DL_CALL_PARAM *param, ACK_REQUEST ack_request)
{
    NL_INF* nl = NL_getInterface();
    NL_CALL_PARAM n_param;
    n_param.destination_address = param->destination_address;
    // OCTET5* oct5 = (OCTET5*)&param->lsdu[5];
    // n_param.hop_count_type = oct5->hop_count == 7 ? HOP_COUNT_EQL_7 : HOP_COUNT_NOT_EQL_7;
    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)param->lsdu;
    n_param.hop_count_type = fl->hop_count == 7 ? HOP_COUNT_EQL_7 : HOP_COUNT_NOT_EQL_7;
    n_param.nsdu = param->lsdu;
    n_param.octet_count = param->octet_count;
    n_param.priority = param->priority;
    n_param.source_address = param->source_address;
    nl->dataSystemBroadcast_ind(&n_param);
}

// L_Poll_Data-service
// TODO:

// L_Busmon Service
static void DL_busmon_ind(int l_status, int time_stamp, DATA_TYPE lsdu)
{
    // TODO:
}

// L_Service_Information Service
static void DL_serviceInformation_ind()
{
    // TODO:
}

DL_INF* DL_getInterface()
{
    return DL_inf;
}

void DL_init()
{
    DL_inf = (DL_INF *)malloc(sizeof(DL_inf));
    // TODO: init default interface member
    DL_inf->data_req = DL_data_req;
    DL_inf->data_con = DL_data_con;
    DL_inf->data_ind = DL_data_ind;
    DL_inf->systemBroadcast_req = DL_systemBroadcast_req;
    DL_inf->systemBroadcast_con = DL_systemBroadcast_con;
    DL_inf->systemBroadcast_ind = DL_systemBroadcast_ind;
    // TODO: init poll data interface
    DL_inf->busmon_ind = DL_busmon_ind;
    DL_inf->serviceInformation_ind = DL_serviceInformation_ind;
}