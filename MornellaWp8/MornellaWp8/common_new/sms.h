//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this sample source code is subject to the terms of the Microsoft
// license agreement under which you licensed this sample source code. If
// you did not accept the terms of the license agreement, you are not
// authorized to use this sample source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the LICENSE.RTF on your install media or the root of your tools installation.
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES OR INDEMNITIES.
//
//////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation. All rights reserved.            //
//////////////////////////////////////////////////////////////////////////


#ifndef _SMS_H_
#define _SMS_H_


#include <windows.h>


#ifdef __cplusplus
extern "C" {
#endif


//
// Errors
//
#define FACILITY_SMS                           0x200

// Specific registration errors (for SmsSetMessageNotification, SmsClearMessageNotification)
#define SMS_E_INVALIDPROTOCOL                  MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x0001)
#define SMS_E_REGISTRATIONEXISTS               MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x0002)
#define SMS_S_NOSUCHREGISTRATION               MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_SMS, 0x0003)

// Specific messaging errors (for SmsOpen, SmsSendMessage, SmsGetSMSC, etc.)
#define SMS_E_TOOMUCHDATA                      MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x0100)
#define SMS_E_INVALIDDATA                      MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x0101)
#define SMS_E_BUFFERTOOSMALL                   MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x0102)
#define SMS_E_PROVIDERSPECIFICBUFFERWRONGSIZE  MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x0103)
#define SMS_E_TIMEUNAVAILABLE                  MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x0104)
#define SMS_E_RECEIVEHANDLEALREADYOPEN         MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x0105)
#define SMS_E_DESTINATIONOUTOFSVC              MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x0106)
#define SMS_E_INVALIDADDRESS                   MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x0107)
#define SMS_E_MSGBARREDBYOPERATOR              MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x0108)
#define SMS_E_MSGCALLBARRED                    MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x0109)
#define SMS_E_NOSCSUBSCRIPTION                 MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x010a)
#define SMS_E_SCBUSY                           MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x010b)
#define SMS_E_SVCNOTSUBSCRIBED                 MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x010c)
#define SMS_E_UNASSIGNEDNUMBER                 MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x010d)
#define SMS_E_UNKNOWNSCADDRESS                 MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x010e)
#define SMS_E_UNIDENTIFIEDSUBCRIBER            MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x010f)
#define SMS_E_FDNRESTRICT                      MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x0110)

// General SMS messaging errors
#define SMS_E_MISC                             MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x0200)
#define SMS_E_PASSWORD                         MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x0201)
#define SMS_E_SIM                              MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x0202)
#define SMS_E_NETWORKACCESS                    MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x0203)
#define SMS_E_NETWORK                          MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x0204)
#define SMS_E_MOBILE                           MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x0205)
#define SMS_E_NETWORKUNSUPPORTED               MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x0206)
#define SMS_E_MOBILEUNSUPPORTED                MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x0207)
#define SMS_E_BADPARAM                         MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x0208)
#define SMS_E_STORAGE                          MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x0209)
#define SMS_E_SMSC                             MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x020a)
#define SMS_E_DESTINATION                      MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x020b)
#define SMS_E_DESTINATIONUNSUPPORTED           MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x020c)
#define SMS_E_RADIOUNAVAILABLE                 MAKE_HRESULT(SEVERITY_ERROR,   FACILITY_SMS, 0x020d)


//
// Constants
//
#define SMS_DATAGRAM_SIZE            (140)
#define SMS_BROADCAST_DATAGRAM_SIZE  (82)
#define SMS_MAX_APPNAME_LENGTH       (MAX_PATH)
#define SMS_MAX_PARAMS_LENGTH        (MAX_PATH)
#define SMS_MAX_PROTOCOLNAME_LENGTH  (MAX_PATH)
#define SMS_MAX_MESSAGEUID_SIZE      (1024)
#define SMS_MAX_ADDRESS_LENGTH       (256)
#define SMS_MAX_SUBADDRESS_SIZE      (256)
// dwMessageModes for SmsOpen
#define SMS_MODE_RECEIVE              (0x00000001)
#define SMS_MODE_SEND                 (0x00000002)
// dwOptions for SmsSendMessage
#define SMS_OPTION_DELIVERY_NONE      (0x00000000)
#define SMS_OPTION_DELIVERY_NO_RETRY  (0x00000001)
// dwMessageOptions for TEXT_PROVIDER_SPECIFIC_DATA
#define PS_MESSAGE_OPTION_NONE          (0x00000000)
#define PS_MESSAGE_OPTION_REPLYPATH     (0x00000001)
#define PS_MESSAGE_OPTION_STATUSREPORT  (0x00000002)
#define PS_MESSAGE_OPTION_DISCARD       (0x00000004)
// dwMessageStatus0 and dwMessageStatus1 for SmsGetMessageStatus and the SMS status-message provider (SMS_MSGTYPE_STATUS)
// Message status is unknown iff dwMessageStatus0 and dwMessageStatus1 are both set to MESSAGE_STATUS_UNKNOWN
#define MESSAGE_STATUS_UNKNOWN                  (0x00000000)
// Valid bits for dwMessageStatus0
#define MESSAGE_STATUS_0_RECEIVEDBYSME            (0x00000001)
#define MESSAGE_STATUS_0_FORWARDEDTOSME           (0x00000002)
#define MESSAGE_STATUS_0_REPLACEDBYSC             (0x00000004)
#define MESSAGE_STATUS_0_CONGESTION_TRYING        (0x00000008)
#define MESSAGE_STATUS_0_SMEBUSY_TRYING           (0x00000010)
#define MESSAGE_STATUS_0_SMENOTRESPONDING_TRYING  (0x00000020)
#define MESSAGE_STATUS_0_SVCREJECTED_TRYING       (0x00000040)
#define MESSAGE_STATUS_0_QUALITYUNAVAIL_TRYING    (0x00000080)
#define MESSAGE_STATUS_0_SMEERROR_TRYING          (0x00000100)
#define MESSAGE_STATUS_0_CONGESTION               (0x00000200)
#define MESSAGE_STATUS_0_SMEBUSY                  (0x00000400)
#define MESSAGE_STATUS_0_SMENOTRESPONDING         (0x00000800)
#define MESSAGE_STATUS_0_SVCREJECTED              (0x00001000)
#define MESSAGE_STATUS_0_QUALITYUNAVAIL_TEMP      (0x00002000)
#define MESSAGE_STATUS_0_SMEERROR                 (0x00004000)
#define MESSAGE_STATUS_0_REMOTEPROCERROR          (0x00008000)
#define MESSAGE_STATUS_0_INCOMPATIBLEDEST         (0x00010000)
#define MESSAGE_STATUS_0_CONNECTIONREJECTED       (0x00020000)
#define MESSAGE_STATUS_0_NOTOBTAINABLE            (0x00040000)
#define MESSAGE_STATUS_0_NOINTERNETWORKING        (0x00080000)
#define MESSAGE_STATUS_0_VPEXPIRED                (0x00100000)
#define MESSAGE_STATUS_0_DELETEDBYORIGSME         (0x00200000)
#define MESSAGE_STATUS_0_DELETEDBYSC              (0x00400000)
#define MESSAGE_STATUS_0_NOLONGEREXISTS           (0x00800000)
#define MESSAGE_STATUS_0_QUALITYUNAVAIL           (0x01000000)
#define MESSAGE_STATUS_0_RESERVED_COMPLETED       (0x02000000)
#define MESSAGE_STATUS_0_RESERVED_TRYING          (0x04000000)
#define MESSAGE_STATUS_0_RESERVED_ERROR           (0x08000000)
#define MESSAGE_STATUS_0_RESERVED_TMPERROR        (0x10000000)
#define MESSAGE_STATUS_0_SCSPECIFIC_COMPLETED     (0x20000000)
#define MESSAGE_STATUS_0_SCSPECIFIC_TRYING        (0x40000000)
#define MESSAGE_STATUS_0_SCSPECIFIC_ERROR         (0x80000000)
// Valid bits for dwMessageStatus1
#define MESSAGE_STATUS_1_SCSPECIFIC_TMPERROR      (0x00000001)
// Language identifiers for SMS_BROADCAST_RANGES.dwBroadcastMsgLangs
#define SMS_DCSLANG_UNKNOWN                       (0x00000001)
#define SMS_DCSLANG_GERMAN                        (0x00000002)
#define SMS_DCSLANG_ENGLISH                       (0x00000004)
#define SMS_DCSLANG_ITALIAN                       (0x00000008)
#define SMS_DCSLANG_FRENCH                        (0x00000010)
#define SMS_DCSLANG_SPANISH                       (0x00000020)
#define SMS_DCSLANG_DUTCH                         (0x00000040)
#define SMS_DCSLANG_SWEDISH                       (0x00000080)
#define SMS_DCSLANG_DANISH                        (0x00000100)
#define SMS_DCSLANG_PORTUGUESE                    (0x00000200)
#define SMS_DCSLANG_FINNISH                       (0x00000400)
#define SMS_DCSLANG_NORWEGIAN                     (0x00000800)
#define SMS_DCSLANG_GREEK                         (0x00001000)
#define SMS_DCSLANG_TURKISH                       (0x00002000)
#define SMS_DCSLANG_HUNGARIAN                     (0x00004000)
#define SMS_DCSLANG_POLISH                        (0x00008000)
#define SMS_DCSLANG_CZECH                         (0x00010000)
#define SMS_DCSLANG_HEBREW                        (0x00020000)
#define SMS_DCSLANG_ARABIC                        (0x00040000)
#define SMS_DCSLANG_RUSSIAN                       (0x00080000)
#define SMS_DCSLANG_ICELANDIC                     (0x00100000)
#define SMS_DCSLANG_ALL                           (0x001fffff)

// Valid flags for SMS_BROADCAST_RANGES.dwParams
#define SMS_PARAM_SBR_BROADCASTMSGIDS             (0x00000001)
#define SMS_PARAM_SBR_BROADCASTMSGLANGS           (0x00000002)
#define SMS_PARAM_SBR_ACCEPTIDS                   (0x00000004)
//Valid values for ProtocolID
#define SMS_MSGPROTOCOL_UNKNOWN                     (0x00000000)
#define SMS_MSGPROTOCOL_SMETOSME                    (0x00000001)
#define SMS_MSGPROTOCOL_IMPLICIT                    (0x00000002)
#define SMS_MSGPROTOCOL_TELEX                       (0x00000003)
#define SMS_MSGPROTOCOL_TELEFAX_GROUP3              (0x00000004)
#define SMS_MSGPROTOCOL_TELEFAX_GROUP4              (0x00000005)
#define SMS_MSGPROTOCOL_VOICEPHONE                  (0x00000006)
#define SMS_MSGPROTOCOL_ERMES                       (0x00000007)
#define SMS_MSGPROTOCOL_PAGING                      (0x00000008)
#define SMS_MSGPROTOCOL_VIDEOTEX                    (0x00000009)
#define SMS_MSGPROTOCOL_TELETEX                     (0x0000000a)
#define SMS_MSGPROTOCOL_TELETEX_PSPDN               (0x0000000b)
#define SMS_MSGPROTOCOL_TELETEX_CSPDN               (0x0000000c)
#define SMS_MSGPROTOCOL_TELETEX_PSTN                (0x0000000d)
#define SMS_MSGPROTOCOL_TELETEX_ISDN                (0x0000000e)
#define SMS_MSGPROTOCOL_UCI                         (0x0000000f)
#define SMS_MSGPROTOCOL_MSGHANDLING                 (0x00000010)
#define SMS_MSGPROTOCOL_X400                        (0x00000011)
#define SMS_MSGPROTOCOL_EMAIL                       (0x00000012)
#define SMS_MSGPROTOCOL_SCSPECIFIC1             (0x00000013)
#define SMS_MSGPROTOCOL_SCSPECIFIC2             (0x00000014)
#define SMS_MSGPROTOCOL_SCSPECIFIC3             (0x00000015)
#define SMS_MSGPROTOCOL_SCSPECIFIC4             (0x00000016)
#define SMS_MSGPROTOCOL_SCSPECIFIC5             (0x00000017)
#define SMS_MSGPROTOCOL_SCSPECIFIC6             (0x00000018)
#define SMS_MSGPROTOCOL_SCSPECIFIC7             (0x00000019)
#define SMS_MSGPROTOCOL_GSMSTATION                  (0x0000001a)
#define SMS_MSGPROTOCOL_SM_TYPE0                    (0x0000001b)
#define SMS_MSGPROTOCOL_RSM_TYPE1                   (0x0000001c)
#define SMS_MSGPROTOCOL_RSM_TYPE2                   (0x0000001d)
#define SMS_MSGPROTOCOL_RSM_TYPE3                   (0x0000001e)
#define SMS_MSGPROTOCOL_RSM_TYPE4                   (0x0000001f)
#define SMS_MSGPROTOCOL_RSM_TYPE5                   (0x00000020)
#define SMS_MSGPROTOCOL_RSM_TYPE6                   (0x00000021)
#define SMS_MSGPROTOCOL_RSM_TYPE7                   (0x00000022)
#define SMS_MSGPROTOCOL_RETURNCALL                  (0x00000023)
#define SMS_MSGPROTOCOL_ME_DOWNLOAD                 (0x00000024)
#define SMS_MSGPROTOCOL_DEPERSONALIZATION           (0x00000025)
#define SMS_MSGPROTOCOL_SIM_DOWNLOAD                (0x00000026)


//
// Enumerations
//
enum SMS_ADDRESS_TYPE {
    SMSAT_UNKNOWN=0,
    SMSAT_INTERNATIONAL,
    SMSAT_NATIONAL,
    SMSAT_NETWORKSPECIFIC,
    SMSAT_SUBSCRIBER,
    SMSAT_ALPHANUMERIC,
    SMSAT_ABBREVIATED
};
enum SMS_DATA_ENCODING {
    SMSDE_OPTIMAL=0,
    SMSDE_GSM,
    SMSDE_UCS2,
};
enum PROVIDER_SPECIFIC_MESSAGE_CLASS {
    PS_MESSAGE_CLASS0 = 0,
    PS_MESSAGE_CLASS1,
    PS_MESSAGE_CLASS2,
    PS_MESSAGE_CLASS3,
    PS_MESSAGE_CLASSUNSPECIFIED,
};
enum PROVIDER_SPECIFIC_REPLACE_OPTION {
    PSRO_NONE = 0,
    PSRO_REPLACE_TYPE1,
    PSRO_REPLACE_TYPE2,
    PSRO_REPLACE_TYPE3,
    PSRO_REPLACE_TYPE4,
    PSRO_REPLACE_TYPE5,
    PSRO_REPLACE_TYPE6,
    PSRO_REPLACE_TYPE7,
    PSRO_RETURN_CALL,
    PSRO_DEPERSONALIZATION,
};


//
// Types
//
typedef DWORD SMS_HANDLE;
typedef DWORD SMS_MESSAGE_ID;
#define INVALID_MESSAGE_ID ((SMS_MESSAGE_ID)0xffffffff)
// Registration structure used by SmsSetMessageNotification and SmsClearMessageNotification
typedef struct smsregistrationdata_tag {
    DWORD   cbSize;
    TCHAR   tszAppName[SMS_MAX_APPNAME_LENGTH];
    TCHAR   tszParams[SMS_MAX_PARAMS_LENGTH];
    TCHAR   tszProtocolName[SMS_MAX_PROTOCOLNAME_LENGTH];
} SMSREGISTRATIONDATA, *LPSMSREGISTRATIONDATA;
// SMS addressing information
typedef struct sms_address_tag {
    SMS_ADDRESS_TYPE smsatAddressType;
    TCHAR ptsAddress[SMS_MAX_ADDRESS_LENGTH];
} SMS_ADDRESS, *LPSMS_ADDRESS;
// SMS status message information
typedef struct sms_status_information_tag {
    SMS_MESSAGE_ID smsmidMessageID;
    DWORD dwMessageStatus0;
    DWORD dwMessageStatus1;
    SMS_ADDRESS smsaRecipientAddress;
    SYSTEMTIME stServiceCenterTimeStamp;  // (UTC time)
    SYSTEMTIME stDischargeTime;  // (UTC time)
} SMS_STATUS_INFORMATION, *LPSMS_STATUS_INFORMATION;
// SMS broadcast message range information
typedef struct sms_range_tag {
    DWORD dwMinimum;
    DWORD dwMaximum;
} SMS_RANGE, *LPSMS_RANGE;
// SMS broadcast message ranges information
// Use #pragma to avoid "warning C4200: nonstandard extension used : zero-sized array in struct/union
#pragma warning(disable:4200)
typedef struct sms_broadcast_ranges_tag {
    DWORD cbSize;
    DWORD dwParams;
    DWORD dwNumRanges;
    DWORD dwBroadcastMsgLangs;
    BOOL bAccept;
    SMS_RANGE smsrBroadcastRanges[];
} SMS_BROADCAST_RANGES, *LPSMS_BROADCAST_RANGES;
#pragma warning(default:4200)


//
// SMS message types (for use with SmsOpen)
//

// Text message type
#define SMS_MSGTYPE_TEXT TEXT("Microsoft Text SMS Protocol")
// Provider-specific data for use with SmsSendMessage and SmsReadMessage

// Bitfield values for the extended parameters of the text provider structure.
#define TEXTPSEXTPARM_NONE         (0x00000000)
#define TEXTPSEXTPARM_CALLBACK     (0x00000001)
#define TEXTPSEXTPARM_PRIORITY     (0x00000002)
#define TEXTPSEXTPARM_ALL          (0x00000003)

// The priority enum should match up with the RIL_MSGPRIORITY_* defines.
// If any new values are added, the Priority mapping functions of sms_txtshared_cdma.cpp
// should also be adjusted.
enum TEXT_PROVIDER_SPECIFIC_PRIORITY_TYPE {
    TEXTPSPRI_NONE = 0,
    TEXTPSPRI_NORMAL = 1,
    TEXTPSPRI_INTERACTIVE,
    TEXTPSPRI_URGENT,
    TEXTPSPRI_EMERGENCY
};

typedef struct text_provider_specific_data_tag {
    DWORD dwMessageOptions;
    PROVIDER_SPECIFIC_MESSAGE_CLASS psMessageClass;
    PROVIDER_SPECIFIC_REPLACE_OPTION psReplaceOption;
    DWORD dwHeaderDataSize;
    BYTE pbHeaderData[SMS_DATAGRAM_SIZE];   // For concatenated messages, only the header from the first segment is returned.
    BOOL fMessageContainsEMSHeaders;        // At least one segment of this message contains EMS headers.
                                            // Only set if EMS handler installed.
    DWORD dwProtocolID;                     // PID of incoming message, or desired PID of outgoing message.
                                            // Applies only to GSM.  Set to SMS_MSGPROTOCOL_UNKNOWN if psReplaceOption
                                            // is not PSRO_NONE.
    DWORD dwExtParams;                                // Bitfield of valid additional structure parameters (all structure
                                                      // values above are considered always valid).
    TEXT_PROVIDER_SPECIFIC_PRIORITY_TYPE tpsPriority; // Applies only to CDMA IS637. Priority indicator.
    SMS_ADDRESS smsaCallback;                         // Applies only to CDMA IS637. Callback number
} TEXT_PROVIDER_SPECIFIC_DATA;

// Class 2 Text message type
#define SMS_MSGTYPE_CLASS2 TEXT("Microsoft Class2 SMS Protocol")
typedef struct class2_provider_specific_data_tag {
    DWORD dwMessageOptions;
    PROVIDER_SPECIFIC_MESSAGE_CLASS psMessageClass;
    PROVIDER_SPECIFIC_REPLACE_OPTION psReplaceOption;
    DWORD dwHeaderDataSize;
    BYTE pbHeaderData[SMS_DATAGRAM_SIZE];   // For concatenated messages, only the header from the first segment is returned.
    BOOL fMessageContainsEMSHeaders;        // At least one segment of this message contains EMS headers.
                                            // Only set if EMS handler installed.
    DWORD dwProtocolID;                     // PID of incoming message, or desired PID of outgoing message.
                                            // Applies only to GSM.  Set to SMS_MSGPROTOCOL_UNKNOWN if psReplaceOption
                                            // is not PSRO_NONE.
    DWORD dwLocation;
    DWORD dwIndex;
} CLASS2_PROVIDER_SPECIFIC_DATA;


// Notification message type
#define SMS_MSGTYPE_NOTIFICATION TEXT("Microsoft Notification SMS Protocol (Receive Only)")
// Provider-specific data for use with SmsReadMessage
enum NOTIFICATION_PROVIDER_SPECIFIC_MSG_WAITING_TYPE {
    NOTIFICATIONPSMWT_NONE = 0,
    NOTIFICATIONPSMWT_GENERIC,
    NOTIFICATIONPSMWT_VOICEMAIL,
    NOTIFICATIONPSMWT_FAX,
    NOTIFICATIONPSMWT_EMAIL,
    NOTIFICATIONPSMWT_OTHER,
};
#define NOTIFICATIONPS_NUM_MSG_WAITING_UNKNOWN  (-1)
#define NOTIFICATIONPS_NUM_MSG_WAITING_NONZERO  (-2)
enum NOTIFICATION_PROVIDER_SPECIFIC_INDICATOR_TYPE {
    NOTIFICATIONPSIT_NONE = 0,
    NOTIFICATIONPSIT_LINE1 = 1,
    NOTIFICATIONPSIT_LINE2 = 2,
};
typedef struct notification_provider_specific_data_tag {
    DWORD dwMessageOptions;
    PROVIDER_SPECIFIC_MESSAGE_CLASS psMessageClass;
    PROVIDER_SPECIFIC_REPLACE_OPTION psReplaceOption;
    NOTIFICATION_PROVIDER_SPECIFIC_MSG_WAITING_TYPE npsMsgWaitingType;
    int iNumberOfMessagesWaiting;
    NOTIFICATION_PROVIDER_SPECIFIC_INDICATOR_TYPE npsIndicatorType;
} NOTIFICATION_PROVIDER_SPECIFIC_DATA;

// WDP message type
#define SMS_MSGTYPE_WDP TEXT("Microsoft WDP SMS Protocol")
// Provider-specific data for use with SmsSendMessage and SmsReadMessage
enum WDP_PROVIDER_SPECIFIC_PORT_ADDRESSING {
    WDPPSPA_8_BIT_PORT_NUMBERS = 0,
    WDPPSPA_16_BIT_PORT_NUMBERS,
};
typedef struct wdp_provider_specific_data_tag {
    WDP_PROVIDER_SPECIFIC_PORT_ADDRESSING wdppsPortAddressing;
    WORD wDestinationPort;
    WORD wOriginatorPort;
} WDP_PROVIDER_SPECIFIC_DATA;

// WCMP message type
#define SMS_MSGTYPE_WCMP TEXT("Microsoft WCMP SMS Protocol")
// Provider-specific data for use with SmsSendMessage and SmsReadMessage
enum WCMP_PROVIDER_SPECIFIC_MESSAGE_TYPE {
    WCMPPSMT_UNSUPPORTED = 0,
    WCMPPSMT_PORT_UNREACHABLE,
    WCMPPSMT_MESSAGE_TOO_BIG,
    WCMPPSMT_ECHO_REQUEST,
    WCMPPSMT_ECHO_REPLY,
};
typedef struct wcmp_provider_specific_data_tag {
    WCMP_PROVIDER_SPECIFIC_MESSAGE_TYPE wcmppsMessageType;
    WORD wParam1;
    WORD wParam2;
    WORD wParam3;
    SMS_ADDRESS smsaAddress;
} WCMP_PROVIDER_SPECIFIC_DATA;

// Status message type
#define SMS_MSGTYPE_STATUS TEXT("Microsoft Status Message SMS Protocol (Receive Only)")
// Provider-specific data for use with SmsReadMessage
typedef struct status_provider_specific_data_tag {
    SMS_STATUS_INFORMATION smssiStatusInformation;
} STATUS_PROVIDER_SPECIFIC_DATA;

// Broadcast message type
#define SMS_MSGTYPE_BROADCAST TEXT("Microsoft Broadcast Message SMS Protocol (Receive Only)")
enum BROADCAST_PROVIDER_SPECIFIC_GEOGRAPHICAL_SCOPE {
    BPSGS_UNKNOWN = 0,
    BPSGS_CELL_DISPLAY_IMMEDIATE,
    BPSGS_CELL,
    BPSGS_PLMN,
    BPSGS_LOCATION_AREA,
};
// Provider-specific data for use with SmsReadMessage
typedef struct broadcast_provider_specific_data_tag {
    WORD wMessageID;
    WORD wMessageCode;
    BROADCAST_PROVIDER_SPECIFIC_GEOGRAPHICAL_SCOPE bpsgsGeographicalScope;
    WORD wUpdateNumber;
} BROADCAST_PROVIDER_SPECIFIC_DATA;

// Raw message type
#define SMS_MSGTYPE_RAW TEXT("Microsoft Raw SMS Protocol (Receive Only)")
// Provider-specific data for use with SmsReadMessage
typedef struct raw_provider_specific_data_tag {
    DWORD dwHeaderDataSize;
    BYTE pbHeaderData[SMS_DATAGRAM_SIZE];
} RAW_PROVIDER_SPECIFIC_DATA;


//
// APIs for SMS.dll
//
HRESULT SmsSetMessageNotification (
    const SMSREGISTRATIONDATA* psmsrd
);

HRESULT SmsClearMessageNotification (
    const LPCTSTR tszProtocolName
);

// Open the SMS Messaging component for read and/or write access
//
// Each protocol may only have one handle open with SMS_MODE_RECEIVE.
// Additional attempts to get RECEIVE mode on a given protocol will result in
// SMS_E_RECEIVEHANDLEALREADYOPEN.
//
// Currently all protocols have applications whith open RECEIVE handles.  As a
// result, using the SMS API to receive SMS messages is not supported.  Attempts
// to do so may interfere with the proper operation of Inbox, WAP, or other SMS
// applications.
//
// The IMailRuleClient interface in cemapi.h may be used to access received
// text SMS messages.
HRESULT SmsOpen (
    const LPCTSTR ptsMessageProtocol,
    const DWORD dwMessageModes,
    SMS_HANDLE* const psmshHandle,
    HANDLE* const phMessageAvailableEvent
);

// Close a handle to the SMS messaging component
HRESULT SmsClose (
    const SMS_HANDLE smshHandle
);

// Send an SMS message
HRESULT SmsSendMessage (
    const SMS_HANDLE smshHandle,
    const SMS_ADDRESS* const psmsaSMSCAddress,
    const SMS_ADDRESS* const psmsaDestinationAddress,
    const SYSTEMTIME* const pstValidityPeriod,  // (Values in this structure are expressed relative to the current time)
    __in_bcount(dwDataSize) const BYTE* const pbData,
    const DWORD dwDataSize,
    __in_bcount(dwProviderSpecificDataSize) const BYTE* const pbProviderSpecificData,
    const DWORD dwProviderSpecificDataSize,
    const SMS_DATA_ENCODING smsdeDataEncoding,
    const DWORD dwOptions,
    SMS_MESSAGE_ID* psmsmidMessageID
);

// Determine an upper-bound for the size of the buffer needed by the next call to SmsReadMessage
HRESULT SmsGetMessageSize (
    const SMS_HANDLE smshHandle,
    DWORD* const pdwDataSize
);

// Read an SMS message (the appropriate size of the buffer can be found via a call to SmsGetMessageSize)
HRESULT SmsReadMessage (
    const SMS_HANDLE smshHandle,
    SMS_ADDRESS* const psmsaSMSCAddress,
    SMS_ADDRESS* const psmsaSourceAddress,
    SYSTEMTIME* const pstReceiveTime,  // (UTC time)
    __out_bcount(dwBufferSize) BYTE* const pbBuffer,
    DWORD dwBufferSize,
    __out_bcount(dwProviderSpecificDataBuffer) BYTE* const pbProviderSpecificBuffer,
    DWORD dwProviderSpecificDataBuffer,
    DWORD* pdwBytesRead
);

// Waits to receive a status-report for an SMS message
HRESULT SmsGetMessageStatus (
    const SMS_HANDLE smshHandle,
    SMS_MESSAGE_ID smsmidMessageID,
    SMS_STATUS_INFORMATION* const psmssiStatusInformation,
    const DWORD dwTimeout
);

// Get the default SMS Service Center address
HRESULT SmsGetSMSC (
    SMS_ADDRESS* const psmsaSMSCAddress
);

// Set the default SMS Service Center address
HRESULT SmsSetSMSC (
    const SMS_ADDRESS* const psmsaSMSCAddress
);

// Get the range of broadcast messages to listen for
HRESULT SmsGetBroadcastMsgRanges (
    SMS_BROADCAST_RANGES* const psmsbrBroadcastRanges
);

// Set the range of broadcast messages to listen for
HRESULT SmsSetBroadcastMsgRanges (
    const SMS_BROADCAST_RANGES* const psmsbrBroadcastRanges
);

// Get the device's phone number for SMS
HRESULT SmsGetPhoneNumber (
    SMS_ADDRESS* const psmsaAddress
);

// Approximate the system time based on the time indicated by the SMSC in the last status-report message
HRESULT SmsGetTime (
    SYSTEMTIME* const ptsCurrentTime,  // (UTC time)
    DWORD* const pdwErrorMargin
);


#ifdef __cplusplus
}
#endif


#endif  // _SMS_H_
