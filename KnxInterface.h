#ifndef KNX_INTERFACE_H
#define KNX_INTERFACE_H

#include <stdio.h>

#define DEBUG(fmt,arg...) \
printf(fmt, ##arg)

typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef UINT16 ADDRESS;
typedef unsigned char* DATA_TYPE;
typedef UINT16 OBJ_TYPE;
typedef UINT16 PID_TYPE;
typedef UINT8 TESTINFO_TYPE;

typedef struct {
    UINT8 ctrl;     //  octect 0
    UINT16 source_addr; // octect 1-2
    UINT16 dest_addr;   // octect 3-4
    unsigned int address_type:1; // octect 5
    unsigned int hop_count:3;
    unsigned int octect_count:4;
    unsigned int  tl_ctrl:6;
    unsigned int ap_ci:4;
    unsigned int ap_ci_data:6;
    unsigned char data[14];
} FRAME_LAYOUT;

DATA_TYPE pdu_alloc();

// Physical Layer definition
// -----------------------------------------------------------

typedef enum {
    INDIVIDUAL_ADDRESS = 0,
    MULTICAST_ADDRESS = 1
} ADDRESS_TYPE;

// Data Link Layer definition
// -----------------------------------------------------------

typedef enum {
    DONT_CARE = 0,
    ACK_REQUESTED = 1
} ACK_REQUEST;

typedef enum {
    PRIORITY_LOW = 0b11,
    PRIORITY_NORMAL = 0b01,
    PRIORITY_URGENT = 0b10,
    PRIORITY_SYSTEM = 0b00
} MEDIA_ACCESS_PRIORITY;

typedef enum {
    STANDARD_FORMAT = 0x00,
    EXTENDED_FORMAT = 0x80,
    LTE_HEE_FORMAT = 0x84
} FRAME_FORMAT;

typedef struct {
    ADDRESS_TYPE address_type;  // TODO: need redefine type
    ADDRESS destination_address;
    FRAME_FORMAT frame_format;
    DATA_TYPE lsdu;  // TODO: need redefine type
    int octet_count;
    MEDIA_ACCESS_PRIORITY priority;  // TODO: need redefine type
    ADDRESS source_address; // only use in data request
} DL_CALL_PARAM;

typedef struct {
    // L_Data service
    int (*data_req)(DL_CALL_PARAM *param, ACK_REQUEST ack_request);
    void (*data_con)(DL_CALL_PARAM *param, int l_status);
    void (*data_ind)(DL_CALL_PARAM *param, ACK_REQUEST ack_request);

    // L_SystemBroadcast
    int (*systemBroadcast_req)(DL_CALL_PARAM *param, ACK_REQUEST ack_request);
    void (*systemBroadcast_con)(DL_CALL_PARAM *param, int l_status);
    void (*systemBroadcast_ind)(DL_CALL_PARAM *param, ACK_REQUEST ack_request);

    // L_Poll_Data-service
    // TODO:

    // L_Busmon Service
    void (*busmon_ind)(int l_status, int time_stamp, DATA_TYPE lsdu);

    // L_Service_Information Service
    void (*serviceInformation_ind)();

} DL_INF;

void DL_init();
DL_INF* DL_getInterface();
void DL_SetMockFd(int fd);

// Network Layer definition
// -----------------------------------------------------------
typedef enum {
    HOP_COUNT_7, // '7'
    HOP_COUNT_NLP, // Network Layer Parameter
    HOP_COUNT_EQL_7, // 'equal to 7'
    HOP_COUNT_NOT_EQL_7 // 'not equal to 7'
} HOP_COUNT_TYPE;


typedef struct {
    ADDRESS destination_address;
    HOP_COUNT_TYPE hop_count_type; // TODO: need redefine type
    int octet_count;
    MEDIA_ACCESS_PRIORITY priority;  // TODO: need redefine type
    DATA_TYPE nsdu;  // TODO: need redefine type

    ADDRESS source_address;
} NL_CALL_PARAM;

typedef struct {
    // N_Data_Individual service
    int (*dataIndividual_req)(NL_CALL_PARAM *param, ACK_REQUEST ack_request);
    void (*dataIndividual_con)(NL_CALL_PARAM *param, int n_status); // FIXME: ack_request is not needed
    void (*dataIndividual_ind)(NL_CALL_PARAM *param);

    // N_Data_Group service
    int (*dataGroup_req)(NL_CALL_PARAM *param, ACK_REQUEST ack_request);
    void (*dataGroup_con)(NL_CALL_PARAM *param, int n_status); // FIXME: ack_request is not needed
    void (*dataGroup_ind)(NL_CALL_PARAM *param);

    // N_Data_Broadcast service
    int (*dataBroadcast_req)(NL_CALL_PARAM *param, ACK_REQUEST ack_request);
    void (*dataBroadcast_con)(NL_CALL_PARAM *param, int n_status); // FIXME: ack_request is not needed
    void (*dataBroadcast_ind)(NL_CALL_PARAM *param);

    // N_Data_Broadcast service
    int (*dataSystemBroadcast_req)(NL_CALL_PARAM *param, ACK_REQUEST ack_request);
    void (*dataSystemBroadcast_con)(NL_CALL_PARAM *param, int n_status); // FIXME: ack_request is not needed
    void (*dataSystemBroadcast_ind)(NL_CALL_PARAM *param);

    // TODO:
} NL_INF;

void NL_init();
NL_INF* NL_getInterface();

// Transport Layer definition
// -----------------------------------------------------------
typedef UINT16 TSAP;

typedef struct {
    HOP_COUNT_TYPE hop_count_type; // TODO: need redefine type
    int octet_count;
    MEDIA_ACCESS_PRIORITY priority;  // TODO: need redefine type
    TSAP tsap;
    DATA_TYPE tsdu;  // TODO: need redefine type

    ADDRESS source_address;
} TL_CALL_PARAM;

ADDRESS TL_tsap2addr(TSAP tsap);
TSAP TL_addr2tsap(ADDRESS addr);

typedef struct {
    // T_Data_Group Service
    int (*dataGroup_req)(TL_CALL_PARAM *param, ACK_REQUEST ack_request);
    void (*dataGroup_con)(TL_CALL_PARAM *param, int n_status);
    void (*dataGroup_ind)(TL_CALL_PARAM *param);

    // T_Data_Tag_Group service
    int (*dataTagGroup_req)(TL_CALL_PARAM *param, ACK_REQUEST ack_request);
    void (*dataTagGroup_con)(TL_CALL_PARAM *param, int n_status);
    void (*dataTagGroup_ind)(TL_CALL_PARAM *param);

    // T_Data_Broadcast Service
    int (*dataBroadcast_req)(TL_CALL_PARAM *param, ACK_REQUEST ack_request);
    void (*dataBroadcast_con)(TL_CALL_PARAM *param, int n_status);
    void (*dataBroadcast_ind)(TL_CALL_PARAM *param);

    // T_Data_SystemBroadcast
    int (*dataSystemBroadcast_req)(TL_CALL_PARAM *param, ACK_REQUEST ack_request);
    void (*dataSystemBroadcast_con)(TL_CALL_PARAM *param, int n_status);
    void (*dataSystemBroadcast_ind)(TL_CALL_PARAM *param);

    // T_Data_Individual
    int (*dataIndividual_req)(TL_CALL_PARAM *param, ACK_REQUEST ack_request);
    void (*dataIndividual_con)(TL_CALL_PARAM *param, int n_status);
    void (*dataIndividual_ind)(TL_CALL_PARAM *param);

    // T_Connect Service
    int (*connect_req)(ADDRESS destination_address, MEDIA_ACCESS_PRIORITY priority);
    void (*connect_con)(ADDRESS destination_address, TSAP tsap, int n_status);
    void (*connect_ind)(TSAP tsap);

    // T_Disconnect Service
    int (*disconnect_req)(ADDRESS destination_address, MEDIA_ACCESS_PRIORITY priority);
    void (*disconnect_con)(ADDRESS destination_address, TSAP tsap, int n_status);
    void (*disconnect_ind)(TSAP tsap);

    // T_Data_Connected Service
    int (*dataConnected_req)(int octet_count, MEDIA_ACCESS_PRIORITY priority, TSAP tsap, DATA_TYPE tsdu);
    void (*dataConnected_con)(TSAP tsap);
    void (*dataConnected_ind)(int octet_count, MEDIA_ACCESS_PRIORITY priority, TSAP tsap, DATA_TYPE tsdu);

    // TODO:
} TL_INF;

void TL_init();
TL_INF* TL_getInterface();

// Application Layer definition
// -----------------------------------------------------------
typedef UINT16 ASAP;

TSAP AL_asap2tsap(ASAP asap);
ASAP AL_tsap2asap(TSAP tsap);

typedef struct {
    HOP_COUNT_TYPE hop_count_type; // TODO: need redefine type
    // int octet_count;
    MEDIA_ACCESS_PRIORITY priority;  // TODO: need redefine type
    ASAP asap;
    // DATA_TYPE asdu;  // TODO: need redefine type

    // ADDRESS source_address;
} AL_CALL_PARAM;

typedef struct {
    int less_than_6bits;
    unsigned int len;
    unsigned char data[14];
} AL_APP_VALUE;

typedef struct {
    // A_GroupValue_Read-service
    int (*groupValueRead_req)(AL_CALL_PARAM *param, ACK_REQUEST ack_request);
    void (*groupValueRead_lcon)(AL_CALL_PARAM *param, int n_status);
    void (*groupValueRead_ind)(AL_CALL_PARAM *param);
    int (*groupValueRead_res)(AL_CALL_PARAM *param, AL_APP_VALUE *value, ACK_REQUEST ack_request);
    void (*groupValueRead_rcon)(AL_CALL_PARAM *param, int n_status);
    void (*groupValueRead_acon)(AL_CALL_PARAM *param, AL_APP_VALUE *value);

    // A_GroupValue_Write-service
    int (*groupValueWrite_req)(AL_CALL_PARAM *param, AL_APP_VALUE *value, ACK_REQUEST ack_request);
    void (*groupValueWrite_lcon)(AL_CALL_PARAM *param, int n_status);
    void (*groupValueWrite_ind)(AL_CALL_PARAM *param, AL_APP_VALUE *value);

    // A_IndividualAddress_Write-service
    int (*IndividualAddressWrite_req)(AL_CALL_PARAM *param, ADDRESS newaddress, ACK_REQUEST ack_request);
    void (*IndividualAddressWrite_lcon)(AL_CALL_PARAM *param, int n_status);
    void (*IndividualAddressWrite_ind)(AL_CALL_PARAM *param, ADDRESS newaddress);

    // A_IndividualAddress_Read-service
    int (*IndividualAddressRead_req)(AL_CALL_PARAM *param, ACK_REQUEST ack_request);
    void (*IndividualAddressRead_lcon)(AL_CALL_PARAM *param, int n_status);
    void (*IndividualAddressRead_ind)(AL_CALL_PARAM *param);
    int (*IndividualAddressRead_res)(AL_CALL_PARAM *param, ADDRESS newaddress, ACK_REQUEST ack_request);
    void (*IndividualAddressRead_rcon)(AL_CALL_PARAM *param, int n_status);
    void (*IndividualAddressRead_acon)(AL_CALL_PARAM *param, ADDRESS newaddress);

    // A_SystemNetworkParameter_Read
    int (*SystemNetworkParameterRead_req)(AL_CALL_PARAM *param, OBJ_TYPE object_type, PID_TYPE pid, TESTINFO_TYPE test_info, ACK_REQUEST ack_request);
    void (*SystemNetworkParameterRead_lcon)(AL_CALL_PARAM *param, int n_status);
    void (*SystemNetworkParameterRead_ind)(AL_CALL_PARAM *param, OBJ_TYPE object_type, PID_TYPE pid, TESTINFO_TYPE test_info);
    int (*SystemNetworkParameterRead_res)(AL_CALL_PARAM *param, OBJ_TYPE object_type, PID_TYPE pid, TESTINFO_TYPE test_info, DATA_TYPE test_result, size_t result_len, ACK_REQUEST ack_request);
    void (*SystemNetworkParameterRead_rcon)(AL_CALL_PARAM *param, int n_status);
    void (*SystemNetworkParameterRead_acon)(AL_CALL_PARAM *param, OBJ_TYPE object_type, PID_TYPE pid, TESTINFO_TYPE test_info, DATA_TYPE test_result, size_t result_len);

    // A_SystemNetworkParameter_Write
    int (*SystemNetworkParameterWrite_req)(AL_CALL_PARAM *param, OBJ_TYPE object_type, PID_TYPE pid, DATA_TYPE value, size_t len, ACK_REQUEST ack_request);
    void (*SystemNetworkParameterWrite_lcon)(AL_CALL_PARAM *param, int n_status);
    void (*SystemNetworkParameterWrite_ind)(AL_CALL_PARAM *param, OBJ_TYPE object_type, PID_TYPE pid, DATA_TYPE value, size_t len);

    // CALLBACK
    // Data_Group Service
    void (*dataGroup_con_cb)(TL_CALL_PARAM *param, int n_status);
    void (*dataGroup_ind_cb)(TL_CALL_PARAM *param);

    // Data_Tag_Group service
    void (*dataTagGroup_con_cb)(TL_CALL_PARAM *param, int n_status);
    void (*dataTagGroup_ind_cb)(TL_CALL_PARAM *param);

    // Data_Broadcast Service
    void (*dataBroadcast_con_cb)(TL_CALL_PARAM *param, int n_status);
    void (*dataBroadcast_ind_cb)(TL_CALL_PARAM *param);

    // T_Data_SystemBroadcast
    void (*dataSystemBroadcast_con_cb)(TL_CALL_PARAM *param, int n_status);
    void (*dataSystemBroadcast_ind_cb)(TL_CALL_PARAM *param);

    // T_Data_Individual
    void (*dataIndividual_con_cb)(TL_CALL_PARAM *param, int n_status);
    void (*dataIndividual_ind_cb)(TL_CALL_PARAM *param);

    // T_Connect Service
    void (*connect_con_cb)(ADDRESS destination_address, TSAP tsap, int n_status);
    void (*connect_ind_cb)(TSAP tsap);

    // T_Disconnect Service
    void (*disconnect_con_cb)(ADDRESS destination_address, TSAP tsap, int n_status);
    void (*disconnect_ind_cb)(TSAP tsap);

    // T_Data_Connected Service
    void (*dataConnected_con_cb)(TSAP tsap);
    void (*dataConnected_ind_cb)(int octet_count, MEDIA_ACCESS_PRIORITY priority, TSAP tsap, DATA_TYPE tsdu);

    // TODO:
} AL_INF;

void AL_init();
AL_INF* AL_getInterface();
#endif