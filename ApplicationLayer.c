#include <stdlib.h>
#include<string.h>

#include "KnxInterface.h"

typedef struct {
    UINT8 entity[200];
} GroupAddrAssociationTable;

typedef struct {
    GroupAddrAssociationTable groat;
    int verify_flag;
} AL_PARAMETER;

static AL_INF *AL_inf;

static AL_PARAMETER AL_param;

// for MOCK
static int AL_groupValueRead_res(AL_CALL_PARAM *param, AL_APP_VALUE *value, ACK_REQUEST ack_request);

TSAP AL_aspa2tsap(ASAP asap)
{
    UINT8 *entity = AL_param.groat.entity;
    for (TSAP i = 0; i < entity[0]; i++) {
        if (entity[2*i+2] == asap) {
            return entity[2*i+1];
        }
    }

    return ASAP_FAIL;
}

ASAP AL_tsap2asap(TSAP tsap)
{
    UINT8 *entity = AL_param.groat.entity;
    for (TSAP i = 0; i < entity[0]; i++) {
        if (entity[2*i+1] == tsap) {
            return entity[2*i+2];
        }
    }

    return ASAP_FAIL;
}



static void AL_param_init()
{
    memset(&AL_param, 0, sizeof(AL_param));
}

void AL_setAPCI(FRAME_LAYOUT *fl, unsigned int value)
{
    fl->ap_ci_h = (value>2)&0b11;
    fl->ap_ci_l = value&0b11;
}

static int AL_getAPCI(FRAME_LAYOUT *fl)
{
    return fl->ap_ci_h<<2|fl->ap_ci_l;
}

// A_GroupValue_Read-service
static int AL_groupValueRead_req(AL_CALL_PARAM *param, ACK_REQUEST ack_request)
{
    TL_INF *tl = TL_getInterface();

    TL_CALL_PARAM t_param;
    t_param.hop_count_type = param->hop_count_type;
    t_param.priority = param->priority;
    t_param.tsap = AL_aspa2tsap(param->asap);
    t_param.tsdu = pdu_alloc();
    t_param.octet_count = 1;
    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)t_param.tsdu;
    // fl->ap_ci = 0;
    AL_setAPCI(fl, 0);
    fl->ap_ci_data = 0;

    tl->dataGroup_req(&t_param, ack_request);

    return 0;
}

static void AL_groupValueRead_lcon(AL_CALL_PARAM *param, int n_status)
{
    // TODO:
}

static void AL_groupValueRead_ind(AL_CALL_PARAM *param)
{
    // TODO:
    DEBUG("AL_SystemNetworkParameterRead_ind %x, %x\n", param->asap, param->hop_count_type);
    // for MOCK
    AL_CALL_PARAM a_param;
    a_param.asap = 1;
    a_param.hop_count_type = HOP_COUNT_7;
    a_param.priority = PRIORITY_LOW;
    AL_APP_VALUE value;
    value.less_than_6bits = 1;
    value.data[0] = 3;
    AL_groupValueRead_res(&a_param, &value, ACK_DONT_CARE);
}

static int AL_groupValueRead_res(AL_CALL_PARAM *param, AL_APP_VALUE *value, ACK_REQUEST ack_request)
{
    if (value == NULL) {
        // TODO: error handle
    }

    TL_INF *tl = TL_getInterface();

    TL_CALL_PARAM t_param;
    t_param.hop_count_type = param->hop_count_type;
    t_param.priority = param->priority;
    t_param.tsap = AL_aspa2tsap(param->asap);
    t_param.tsdu = pdu_alloc();
    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)t_param.tsdu;
    // fl->ap_ci = 1;
    AL_setAPCI(fl, 1);
    if (value->less_than_6bits) {
        fl->ap_ci_data = *value->data;
        t_param.octet_count = 1;
    } else {
        fl->ap_ci_data = 0;
        memcpy(t_param.tsdu+7, value->data, value->len);
        t_param.octet_count = value->len;
    }

    tl->dataGroup_req(&t_param, ack_request);

    return 0;
}

static void AL_groupValueRead_rcon(AL_CALL_PARAM *param, int n_status)
{
    // TODO:
}

static void AL_groupValueRead_acon(AL_CALL_PARAM *param, AL_APP_VALUE *value)
{
    // TODO:
}

// A_GroupValue_Write-service
static int AL_groupValueWrite_req(AL_CALL_PARAM *param,  AL_APP_VALUE *value, ACK_REQUEST ack_request)
{
    TL_INF *tl = TL_getInterface();

    TL_CALL_PARAM t_param;
    t_param.hop_count_type = param->hop_count_type;
    t_param.priority = param->priority;
    t_param.tsap = AL_aspa2tsap(param->asap);
    t_param.tsdu = pdu_alloc();
    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)t_param.tsdu;
    // fl->ap_ci = 2;
    AL_setAPCI(fl, 2);
    if (value->less_than_6bits) {
        fl->ap_ci_data = *value->data;
    } else {
        fl->ap_ci_data = 0;
        memcpy(t_param.tsdu+7, value->data, value->len);
        t_param.octet_count = 7 + value->len;
    }

    tl->dataGroup_req(&t_param, ack_request);

    return 0;
}

static void AL_groupValueWrite_lcon(AL_CALL_PARAM *param, int n_status)
{
    // TODO:
}

static void AL_groupValueWrite_ind(AL_CALL_PARAM *param, AL_APP_VALUE *value)
{
    // TODO:
}


// A_IndividualAddress_Write-service
static int AL_IndividualAddressWrite_req(AL_CALL_PARAM *param, ADDRESS newaddress, ACK_REQUEST ack_request)
{
    if (param->priority != PRIORITY_SYSTEM) {
        // TODO: error handle
    }

    TL_INF *tl = TL_getInterface();

    TL_CALL_PARAM t_param;
    t_param.hop_count_type = param->hop_count_type;
    t_param.priority = param->priority;
    t_param.tsap = AL_aspa2tsap(param->asap);
    t_param.tsdu = pdu_alloc();
    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)t_param.tsdu;
    // fl->ap_ci = 3;
    AL_setAPCI(fl, 3);

    fl->ap_ci_data = 0;
    fl->data[0] = newaddress>>8;
    fl->data[1] = newaddress&0xff;
    t_param.octet_count = 7 + 2;

    tl->dataBroadcast_req(&t_param, ack_request);

    return 0;
}

static void AL_IndividualAddressWrite_lcon(AL_CALL_PARAM *param, int n_status)
{
    // TODO:
}

static void AL_IndividualAddressWrite_ind(AL_CALL_PARAM *param, ADDRESS newaddress)
{
    // TODO:
}

// A_IndividualAddress_Read-service
static int AL_IndividualAddressRead_req(AL_CALL_PARAM *param, ACK_REQUEST ack_request)
{
    if (param->priority != PRIORITY_SYSTEM) {
        // TODO: error handle
    }

    TL_INF *tl = TL_getInterface();

    TL_CALL_PARAM t_param;
    t_param.hop_count_type = param->hop_count_type;
    t_param.priority = param->priority;
    t_param.tsap = AL_aspa2tsap(param->asap);
    t_param.tsdu = pdu_alloc();
    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)t_param.tsdu;
    // fl->ap_ci = 4;
    AL_setAPCI(fl, 4);

    fl->ap_ci_data = 0;
    t_param.octet_count = 7;

    tl->dataBroadcast_req(&t_param, ack_request);

    return 0;
}

static void AL_IndividualAddressRead_lcon(AL_CALL_PARAM *param, int n_status)
{
    // TODO:
}

static void AL_IndividualAddressRead_ind(AL_CALL_PARAM *param)
{
    // TODO:
}

static int AL_IndividualAddressRead_res(AL_CALL_PARAM *param, ADDRESS newaddress, ACK_REQUEST ack_request)
{
    TL_INF *tl = TL_getInterface();

    TL_CALL_PARAM t_param;
    t_param.hop_count_type = param->hop_count_type;
    t_param.priority = param->priority;
    t_param.tsap = AL_aspa2tsap(param->asap);
    t_param.tsdu = pdu_alloc();
    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)t_param.tsdu;
    // fl->ap_ci = 5;
    AL_setAPCI(fl, 5);
    fl->ap_ci_data = 0;
    fl->data[0] = newaddress>>8;
    fl->data[1] = newaddress&0xff;
    t_param.octet_count = 7 + 2;

    tl->dataBroadcast_req(&t_param, ack_request);

    return 0;
}

static void AL_IndividualAddressRead_rcon(AL_CALL_PARAM *param, int n_status)
{
    // TODO:
}

static void AL_IndividualAddressRead_acon(AL_CALL_PARAM *param, ADDRESS newaddress)
{
    // TODO:
}

// A_SystemNetworkParameter_Read
static int AL_SystemNetworkParameterRead_req(AL_CALL_PARAM *param, OBJ_TYPE object_type, PID_TYPE pid, TESTINFO_TYPE test_info, ACK_REQUEST ack_request)
{
    if (param->priority != PRIORITY_SYSTEM) {
        // TODO: error handle
    }

    TL_INF *tl = TL_getInterface();

    TL_CALL_PARAM t_param;
    t_param.hop_count_type = param->hop_count_type;
    t_param.priority = param->priority;
    t_param.tsap = AL_aspa2tsap(param->asap);
    t_param.tsdu = pdu_alloc();
    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)t_param.tsdu;
    // fl->ap_ci = 7;
    AL_setAPCI(fl, 7);
    fl->ap_ci_data = 8;
    fl->data[0] = object_type>>8;
    fl->data[1] = object_type&0xff;
    fl->data[2] = pid>>4;
    fl->data[3] = (pid&0x0f)<<4;
    fl->data[4] = test_info;
    t_param.octet_count = 7 + 5;

    tl->dataBroadcast_req(&t_param, ack_request);

    return 0;
}

static void AL_SystemNetworkParameterRead_lcon(AL_CALL_PARAM *param, int n_status)
{
    // TODO:
}

static void AL_SystemNetworkParameterRead_ind(AL_CALL_PARAM *param, OBJ_TYPE object_type, PID_TYPE pid, TESTINFO_TYPE test_info)
{
    // TODO:
    DEBUG("AL_SystemNetworkParameterRead_ind %x, %x\n", object_type, pid);
}

static int AL_SystemNetworkParameterRead_res(AL_CALL_PARAM *param, OBJ_TYPE object_type, PID_TYPE pid, TESTINFO_TYPE test_info, DATA_TYPE test_result, size_t result_len, ACK_REQUEST ack_request)
{
    TL_INF *tl = TL_getInterface();

    TL_CALL_PARAM t_param;
    t_param.hop_count_type = param->hop_count_type;
    t_param.priority = param->priority;
    t_param.tsap = AL_aspa2tsap(param->asap);
    t_param.tsdu = pdu_alloc();
    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)t_param.tsdu;
    // fl->ap_ci = 7;
    AL_setAPCI(fl, 7);
    fl->ap_ci_data = 9;
    fl->data[0] = object_type>>8;
    fl->data[1] = object_type&0xff;
    fl->data[2] = pid>>4;
    fl->data[3] = (pid&0x0f)<<4;
    t_param.octet_count = 7 + 4;
    if (result_len > 14) {    // result should not too long
        // TODO: error handle
    } else if (result_len != 0) {
        fl->data[4] = test_info;
        memcpy(fl->data+5, test_result, result_len);
        t_param.octet_count += 1 + result_len;
    }

    tl->dataBroadcast_req(&t_param, ack_request);

    return 0;
}

static void AL_SystemNetworkParameterRead_rcon(AL_CALL_PARAM *param, int n_status)
{
    // TODO:
}

static void AL_SystemNetworkParameterRead_acon(AL_CALL_PARAM *param, OBJ_TYPE object_type, PID_TYPE pid, TESTINFO_TYPE test_info, DATA_TYPE test_result, size_t result_len)
{
    // TODO:
}

// A_SystemNetworkParameter_Write
static int AL_SystemNetworkParameterWrite_req(AL_CALL_PARAM *param, OBJ_TYPE object_type, PID_TYPE pid, DATA_TYPE value, size_t len, ACK_REQUEST ack_request)
{
    if (param->priority != PRIORITY_SYSTEM) {
        // TODO: error handle
    }

    TL_INF *tl = TL_getInterface();

    TL_CALL_PARAM t_param;
    t_param.hop_count_type = param->hop_count_type;
    t_param.priority = param->priority;
    t_param.tsap = AL_aspa2tsap(param->asap);
    t_param.tsdu = pdu_alloc();
    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)t_param.tsdu;
    // fl->ap_ci = 7;
    AL_setAPCI(fl, 7);
    fl->ap_ci_data = 10;

    fl->data[0] = object_type>>8;
    fl->data[1] = object_type&0xff;
    fl->data[2] = pid>>4;
    fl->data[3] = (pid&0x0f)<<4;
    memcpy(fl->data + 4, value, len);
    t_param.octet_count = 7 + 4 + len;

    tl->dataBroadcast_req(&t_param, ack_request);

    return 0;
}

static void AL_SystemNetworkParameterWrite_lcon(AL_CALL_PARAM *param, int n_status)
{
    // TODO:
}

static void AL_SystemNetworkParameterWrite_ind(AL_CALL_PARAM *param, OBJ_TYPE object_type, PID_TYPE pid, DATA_TYPE value, size_t len)
{
    // TODO:
}



#ifdef COUPLER_SPEC_SERVICE
// A_IndividualAddressSerialNumber_Read-service
static int AL_IndividualAddressSerialNumberRead_req(AL_CALL_PARAM *param, DATA_TYPE data, ACK_REQUEST ack_request)
{
    if (param->priority != PRIORITY_SYSTEM) {
        // TODO: error handle
    }

    TL_INF *tl = TL_getInterface();

    TL_CALL_PARAM t_param;
    t_param.hop_count_type = param->hop_count_type;
    t_param.priority = param->priority;
    t_param.tsap = AL_aspa2tsap(param->asap);
    t_param.tsdu = pdu_alloc();
    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)t_param.tsdu;
    fl->ap_ci = 4;

    fl->ap_ci_data = 0;
    fl->octect_count = 7;

    tl->dataBroadcast_req(&t_param, ack_request);
}

static void AL_IndividualAddressSerialNumberRead_lcon(AL_CALL_PARAM *param, int n_status)
{
    // TODO:
}

static void AL_IndividualAddressSerialNumberRead_ind(AL_CALL_PARAM *param)
{
    // TODO:
}

static int AL_IndividualAddressSerialNumberRead_res(AL_CALL_PARAM *param, ADDRESS newaddress, ACK_REQUEST ack_request)
{
    TL_INF *tl = TL_getInterface();

    TL_CALL_PARAM t_param;
    t_param.hop_count_type = param->hop_count_type;
    t_param.priority = param->priority;
    t_param.tsap = AL_aspa2tsap(param->asap);
    t_param.tsdu = pdu_alloc();
    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)t_param.tsdu;
    fl->ap_ci = 5;
    fl->ap_ci_data = 0;
    fl->data[0] = newaddress>>8;
    fl->data[1] = newaddress&0xff;
    fl->octect_count = 7 + 2;

    tl->dataGroup_req(&t_param, ack_request);
}

static void AL_IndividualAddressSerialNumberRead_rcon(AL_CALL_PARAM *param, int n_status)
{
    // TODO:
}

static void AL_IndividualAddressSerialNumberRead_acon(AL_CALL_PARAM *param, ADDRESS newaddress)
{
    // TODO:
}
#endif

// CALLBACK
// Data_Group Service
static void AL_dataGroup_con_cb(TL_CALL_PARAM *param, int n_status)
{
    // TODO:
}

static void AL_dataGroup_ind_cb(TL_CALL_PARAM *param)
{
    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)param->tsdu;

    DEBUG("AL_dataGroup_ind_cb %p \n", fl);
    int apci = AL_getAPCI(fl);
    if (apci== 0 && fl->ap_ci_data == 0) {    // A_GroupValue_Read-PDU
        AL_CALL_PARAM a_param;
        a_param.asap = AL_tsap2asap(param->tsap);
        // a_param.asdu = param->tsdu;
        a_param.hop_count_type = param->hop_count_type;
        a_param.priority = param->priority;
        DEBUG("AL_dataGroup_ind_cb 1\n");
        AL_groupValueRead_ind(&a_param);
    } if (apci == 1) { // A_GroupValue_Response-PDU
        AL_CALL_PARAM a_param;
        a_param.asap = AL_tsap2asap(param->tsap);
        // a_param.asdu = param->tsdu;
        a_param.hop_count_type = param->hop_count_type;
        a_param.priority = param->priority;
        AL_APP_VALUE value;
        value.less_than_6bits = fl->octect_count <= 7 ? 1 : 0;
        value.len = fl->octect_count - 7;
        value.len = value.len == 0 ? 1 : value.len;
        memcpy(value.data, param->tsdu + 7, value.len);
        AL_groupValueRead_acon(&a_param, &value);
    } if (apci == 2) { // A_GroupValue_Write-PDU
        AL_CALL_PARAM a_param;
        a_param.asap = AL_tsap2asap(param->tsap);
        // a_param.asdu = param->tsdu;
        a_param.hop_count_type = param->hop_count_type;
        a_param.priority = param->priority;
        AL_APP_VALUE value;
        value.less_than_6bits = fl->octect_count <= 7 ? 1 : 0;
        value.len = fl->octect_count - 7;
        value.len = value.len == 0 ? 1 : value.len;
        memcpy(value.data, param->tsdu + 7, value.len);
        AL_groupValueWrite_ind(&a_param, &value);
    } else {
        // TODO: error handle
    }
}


// Data_Tag_Group service
static void AL_dataTagGroup_con_cb(TL_CALL_PARAM *param, int n_status)
{
    // TODO:
}

static void AL_dataTagGroup_ind_cb(TL_CALL_PARAM *param)
{
    // TODO:
}

// Data_Broadcast Service
static void AL_dataBroadcast_con_cb(TL_CALL_PARAM *param, int n_status)
{
    // TODO:
}

static void AL_dataBroadcast_ind_cb(TL_CALL_PARAM *param)
{
    FRAME_LAYOUT *fl = (FRAME_LAYOUT *)param->tsdu;

    int apci = AL_getAPCI(fl);
    if (apci == 3) {   // A_IndividualAddress_Write-PDU
        AL_CALL_PARAM a_param;
        a_param.asap = AL_tsap2asap(param->tsap);
        // a_param.asdu = param->tsdu;
        a_param.hop_count_type = param->hop_count_type;
        a_param.priority = param->priority;
        ADDRESS newaddress;
        newaddress = fl->data[0] << 8 | fl->data[1];
        AL_IndividualAddressWrite_ind(&a_param, newaddress);
    } else if (apci == 4) {  // A_IndividualAddress_Read-PDU
        AL_CALL_PARAM a_param;
        a_param.asap = AL_tsap2asap(param->tsap);
        // a_param.asdu = param->tsdu;
        a_param.hop_count_type = param->hop_count_type;
        a_param.priority = param->priority;
        AL_IndividualAddressRead_ind(&a_param);
    } else if (apci == 5) {  // A_IndividualAddress_Response-PDU
        AL_CALL_PARAM a_param;
        a_param.asap = AL_tsap2asap(param->tsap);
        // a_param.asdu = param->tsdu;
        a_param.hop_count_type = param->hop_count_type;
        a_param.priority = param->priority;
        ADDRESS newaddress;
        newaddress = fl->data[0] << 8 | fl->data[1];
        AL_IndividualAddressRead_acon(&a_param, newaddress);
    } else if (apci == 7 && fl->ap_ci_data == 0x8) {  // A_SystemNetworkParameter_Read-PDU
        AL_CALL_PARAM a_param;
        a_param.asap = AL_tsap2asap(param->tsap);
        // a_param.asdu = param->tsdu;
        a_param.hop_count_type = param->hop_count_type;
        a_param.priority = param->priority;
        OBJ_TYPE object_type = fl->data[0]<<8|fl->data[1];
        PID_TYPE pid = fl->data[2]<<4|fl->data[3]>>4;
        TESTINFO_TYPE test_info = fl->data[4];
        // newaddress = fl->data[0] << 8 | fl->data[1];

        AL_SystemNetworkParameterRead_ind(&a_param, object_type, pid, test_info);
    } else if (apci == 7 && fl->ap_ci_data == 0x9) {  // A_SystemNetworkParameter_Response-PDU
        AL_CALL_PARAM a_param;
        a_param.asap = AL_tsap2asap(param->tsap);
        // a_param.asdu = param->tsdu;
        a_param.hop_count_type = param->hop_count_type;
        a_param.priority = param->priority;
        OBJ_TYPE object_type = fl->data[0]<<8|fl->data[1];
        PID_TYPE pid = fl->data[2]<<4|fl->data[3]>>4;
        TESTINFO_TYPE test_info = fl->data[4];
        unsigned char test_result[14];
        if (fl->octect_count > 12) {
            memcpy(test_result, fl->data, fl->octect_count - 12);
        }
        // newaddress = fl->data[0] << 8 | fl->data[1];

        AL_SystemNetworkParameterRead_acon(&a_param, object_type, pid, test_info, test_result, fl->octect_count - 12);
    } else if (apci == 7 && fl->ap_ci_data == 0x10) {  // A_SystemNetworkParameter_Write-PDU
        AL_CALL_PARAM a_param;
        a_param.asap = AL_tsap2asap(param->tsap);
        // a_param.asdu = param->tsdu;
        a_param.hop_count_type = param->hop_count_type;
        a_param.priority = param->priority;
        OBJ_TYPE object_type = fl->data[0]<<8|fl->data[1];
        PID_TYPE pid = fl->data[2]<<4|fl->data[3]>>4;
        unsigned char test_result[14];
        if (fl->octect_count > 11) {
            memcpy(test_result, fl->data, fl->octect_count - 11);
        }
        // newaddress = fl->data[0] << 8 | fl->data[1];

        AL_SystemNetworkParameterWrite_ind(&a_param, object_type, pid, test_result, fl->octect_count - 11);
    } else {
        // TODO: error handle
    }
}

// T_Data_SystemBroadcast
static void AL_dataSystemBroadcast_con_cb(TL_CALL_PARAM *param, int n_status)
{
    // TODO:
}

static void AL_dataSystemBroadcast_ind_cb(TL_CALL_PARAM *param)
{
    // TODO:
}

// T_Data_Individual
static void AL_dataIndividual_con_cb(TL_CALL_PARAM *param, int n_status)
{
    // TODO:
}

static void AL_dataIndividual_ind_cb(TL_CALL_PARAM *param)
{
    // TODO:
}

// T_Connect Service
static void AL_connect_con_cb(ADDRESS destination_address, TSAP tsap, int n_status)
{
    // TODO:
}

static void AL_connect_ind_cb(TSAP tsap)
{
    // TODO:
}

// T_Disconnect Service
static void AL_disconnect_con_cb(ADDRESS destination_address, TSAP tsap, int n_status)
{
    // TODO:
}

static void AL_disconnect_ind_cb(TSAP tsap)
{
    // TODO:
}

// T_Data_Connected Service
static void AL_dataConnected_con_cb(TSAP tsap)
{
    // TODO:
}

static void AL_dataConnected_ind_cb(int octet_count, MEDIA_ACCESS_PRIORITY priority, TSAP tsap, DATA_TYPE tsdu)
{
    // TODO:
}


AL_INF* AL_getInterface()
{
    return AL_inf;
}

void AL_init()
{
    AL_param_init();

    AL_inf = (AL_INF *)malloc(sizeof(AL_INF));

    // TODO: init default interface member
    AL_inf->groupValueRead_req = AL_groupValueRead_req;
    AL_inf->groupValueRead_lcon = AL_groupValueRead_lcon;
    AL_inf->groupValueRead_ind = AL_groupValueRead_ind;
    AL_inf->groupValueRead_res = AL_groupValueRead_res;
    AL_inf->groupValueRead_rcon = AL_groupValueRead_rcon;
    AL_inf->groupValueRead_acon = AL_groupValueRead_acon;

    AL_inf->groupValueWrite_req = AL_groupValueWrite_req;
    AL_inf->groupValueWrite_lcon = AL_groupValueWrite_lcon;
    AL_inf->groupValueWrite_ind = AL_groupValueWrite_ind;

    AL_inf->IndividualAddressWrite_req = AL_IndividualAddressWrite_req;
    AL_inf->IndividualAddressWrite_lcon = AL_IndividualAddressWrite_lcon;
    AL_inf->IndividualAddressWrite_ind = AL_IndividualAddressWrite_ind;

    AL_inf->IndividualAddressRead_req = AL_IndividualAddressRead_req;
    AL_inf->IndividualAddressRead_lcon = AL_IndividualAddressRead_lcon;
    AL_inf->IndividualAddressRead_ind = AL_IndividualAddressRead_ind;
    AL_inf->IndividualAddressRead_res = AL_IndividualAddressRead_res;
    AL_inf->IndividualAddressRead_rcon = AL_IndividualAddressRead_rcon;
    AL_inf->IndividualAddressRead_acon = AL_IndividualAddressRead_acon;

    AL_inf->SystemNetworkParameterRead_req = AL_SystemNetworkParameterRead_req;
    AL_inf->SystemNetworkParameterRead_lcon = AL_SystemNetworkParameterRead_lcon;
    AL_inf->SystemNetworkParameterRead_ind = AL_SystemNetworkParameterRead_ind;
    AL_inf->SystemNetworkParameterRead_res = AL_SystemNetworkParameterRead_res;
    AL_inf->SystemNetworkParameterRead_rcon = AL_SystemNetworkParameterRead_rcon;
    AL_inf->SystemNetworkParameterRead_acon = AL_SystemNetworkParameterRead_acon;

    AL_inf->SystemNetworkParameterWrite_req = AL_SystemNetworkParameterWrite_req;
    AL_inf->SystemNetworkParameterWrite_lcon = AL_SystemNetworkParameterWrite_lcon;
    AL_inf->SystemNetworkParameterWrite_ind = AL_SystemNetworkParameterWrite_ind;

    AL_inf->dataGroup_con_cb = AL_dataGroup_con_cb;
    AL_inf->dataGroup_ind_cb = AL_dataGroup_ind_cb;

    AL_inf->dataTagGroup_con_cb = AL_dataTagGroup_con_cb;
    AL_inf->dataTagGroup_ind_cb = AL_dataTagGroup_ind_cb;

    AL_inf->dataBroadcast_con_cb = AL_dataBroadcast_con_cb;
    AL_inf->dataBroadcast_ind_cb = AL_dataBroadcast_ind_cb;

    AL_inf->dataSystemBroadcast_con_cb = AL_dataSystemBroadcast_con_cb;
    AL_inf->dataSystemBroadcast_ind_cb = AL_dataSystemBroadcast_ind_cb;

    AL_inf->dataIndividual_con_cb = AL_dataIndividual_con_cb;
    AL_inf->dataIndividual_ind_cb = AL_dataIndividual_ind_cb;

    AL_inf->connect_con_cb = AL_connect_con_cb;
    AL_inf->connect_ind_cb = AL_connect_ind_cb;

    AL_inf->disconnect_con_cb = AL_disconnect_con_cb;
    AL_inf->disconnect_ind_cb = AL_disconnect_ind_cb;

    AL_inf->dataConnected_con_cb = AL_dataConnected_con_cb;
    AL_inf->dataConnected_ind_cb = AL_dataConnected_ind_cb;
}

// MOCK function
void MOCK_addGrOAT(TSAP tsap, ASAP asap)
{
    UINT8 *entity = AL_param.groat.entity;
    UINT8 len = entity[0];
    entity[2*len+1] = tsap;
    entity[2*len+2] = asap;
    entity[0]++;
}