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
/*++


Module Name:

    pm.h

Abstract:

    Definitions and API for the WCE Power Manager component.

Revision History:

--*/

#pragma once

#include <winioctl.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

//**********************************************************************
// WinCE Device Interface GUIDs for Power Manager controlled devices.
// NEVER CHANGE THESE VALUES! They indicate that a device is 
// power manageable. The device MUST expose an appropriate class
// via the IClass registry key or the AdvertiseInterface() API to 
// receive power management IOCTLs.  For example,
//   "IClass"=multi_sz:"{A32942B7-920C-486b-B0E6-92A702A99B35}"
// OEMs may define other classes in addition to the ones listed here.
//**********************************************************************

#define PMCLASS_GENERIC_DEVICE          TEXT("{A32942B7-920C-486b-B0E6-92A702A99B35}")
#define PMCLASS_NDIS_MINIPORT           TEXT("{98C5250D-C29A-4985-AE5F-AFE5367E5006}")
#define PMCLASS_BLOCK_DEVICE            TEXT("{8DD679CE-8AB4-43c8-A14A-EA4963FAA715}")
#define PMCLASS_DISPLAY                 TEXT("{EB91C7C9-8BF6-4a2d-9AB8-69724EED97D1}")

// backwards compatibility definitions from the PM's CE .NET release
#define DEVCLASS_POWER_MANAGER_STRING   PMCLASS_GENERIC_DEVICE
#define DEVCLASS_POWER_MANAGER_GUID     { 0xA32942B7, 0x920C, 0x486b, { 0xB0, 0xE6, 0x92, 0xA7, 0x02, 0xA9, 0x9B, 0x35 } }

//*****************************************************************************
// D E F I N E S
//*****************************************************************************

// Power Manager's System Power key
#define PWRMGR_REG_KEY           TEXT("SYSTEM\\CurrentControlSet\\Control\\Power")
#define PM_SUPPORT_PB_RELEASE    TEXT("SupportPowerButtonRelease")

//
// System Power (Source/State/Option) Flags
//

// upper bytes: common power state bits
#define POWER_STATE(f)           ((f) &  0xFFFF0000)        // power state mask
#define POWER_STATE_ON           (DWORD)(0x00010000)        // on state
#define POWER_STATE_OFF          (DWORD)(0x00020000)        // no power, full off
#define POWER_STATE_CRITICAL     (DWORD)(0x00040000)        // critical off
#define POWER_STATE_BOOT         (DWORD)(0x00080000)        // boot state
#define POWER_STATE_IDLE         (DWORD)(0x00100000)        // idle state
#define POWER_STATE_SUSPEND      (DWORD)(0x00200000)        // suspend state
#define POWER_STATE_UNATTENDED   (DWORD)(0x00400000)        // Unattended state.
#define POWER_STATE_RESET        (DWORD)(0x00800000)        // reset state
#define POWER_STATE_USERIDLE     (DWORD)(0x01000000)        // user idle state
#define POWER_STATE_BACKLIGHTON  (DWORD)(0x02000000)        // device scree backlight on
#define POWER_STATE_PASSWORD     (DWORD)(0x10000000)        // This state is password protected.

#define PM_DEFAULT_SZ       TEXT("Default")
#define PM_FLAGS_SZ         TEXT("Flags")
#define MAX_STATE_NAMEL     MAX_PATH    // max system power state name length

//
// Power Requirement Flags
//
#define POWER_NAME              (DWORD)(0x00000001) // default
#define POWER_FORCE             (DWORD)(0x00001000)
#define POWER_DUMPDW            (DWORD)(0x00002000)        // Calling CaptureDumpFileOnDevice() before entering this state.

//
// POWER IOCTLS
//
// We are NOT APCI, we just borrow this unused code from winioctl.h
#define FILE_DEVICE_POWER   FILE_DEVICE_ACPI    

/*
Required
InBuf:  PPOWER_RELATIONSHIP - defines the target device for parent/bus drivers, else NULL
OutBuf: PPOWER_CAPABILITIES - defines the devices power caps

If a driver fails this ioctl the PM assumes the target driver does not support power ioctls.
*/
#define IOCTL_POWER_CAPABILITIES    \
    CTL_CODE(FILE_DEVICE_POWER, 0x400, METHOD_BUFFERED, FILE_ANY_ACCESS)

/* ++
Required
InBuf:  PPOWER_RELATIONSHIP   - defines the target device for parent/bus drivers, else NULL
OutBuf: PCEDEVICE_POWER_STATE - returns the device's current state (Dx).

PM will only send this ioctl to drivers that support the power ioctls.
-- */
#define IOCTL_POWER_GET             \
    CTL_CODE(FILE_DEVICE_POWER, 0x401, METHOD_BUFFERED, FILE_ANY_ACCESS)

/* ++
Required
InBuf:  PPOWER_RELATIONSHIP   - defines the target device for parent/bus drivers, else NULL
OutBuf: PCEDEVICE_POWER_STATE - device state (Dx) in which to put the device.

If the driver does not support the proposed Dx then it should write it's adjusted Dx
into the OutBuf (Dx).

PM will only send this ioctl to drivers that support the power ioctls.
-- */
#define IOCTL_POWER_SET             \
    CTL_CODE(FILE_DEVICE_POWER, 0x402, METHOD_BUFFERED, FILE_ANY_ACCESS)

/*++
Required
InBuf:  PPOWER_RELATIONSHIP   - defines the target device for parent/bus drivers, else NULL
OutBuf: PCEDEVICE_POWER_STATE - device state (Dx) that the system is querying for a 
                                pending IOCTL_POWER_SET operation.
                                
To veto the query the driver should write PwrDeviceUnspecified (-1)
into the OutBuf (Dx), else PM assumes the driver accepted.

PM will only send this ioctl to drivers that support the power ioctls.
-- */
#define IOCTL_POWER_QUERY           \
    CTL_CODE(FILE_DEVICE_POWER, 0x403, METHOD_BUFFERED, FILE_ANY_ACCESS)

/*
Required
InBuf:  NULL 
OutBuf: NULL

PM does not care the return value from this IOCTL.   It's there to let the Parent device
to register all devices it controls.
*/
#define IOCTL_REGISTER_POWER_RELATIONSHIP    \
    CTL_CODE(FILE_DEVICE_POWER, 0x406, METHOD_BUFFERED, FILE_ANY_ACCESS)


//*****************************************************************************
// T Y P E D E F S
//*****************************************************************************

//
// Device Power States
//
typedef enum _CEDEVICE_POWER_STATE {
    PwrDeviceUnspecified = -1,
    D0 = 0, // Full On: full power,  full functionality
    D1,     // Low Power On: fully functional at low power/performance
    D2,     // Standby: partially powered with automatic wake
    D3,     // Sleep: partially powered with device initiated wake
    D4,     // Off: unpowered
    PwrDeviceMaximum
} CEDEVICE_POWER_STATE, *PCEDEVICE_POWER_STATE;

#define DX_MASK(Dx)  (0x00000001 << Dx)

#define VALID_DX(dx)  ( dx > PwrDeviceUnspecified && dx < PwrDeviceMaximum)

//
// Device or OAL Power Capabilities
//
typedef struct _POWER_CAPABILITIES {
    UCHAR DeviceDx;
    UCHAR WakeFromDx;
    UCHAR InrushDx;
    DWORD Power[PwrDeviceMaximum];
    DWORD Latency[PwrDeviceMaximum];
    DWORD Flags;
} POWER_CAPABILITIES, *PPOWER_CAPABILITIES;

#define POWER_CAP_PARENT        0x00000001      // parent/bus driver

//
// Defines the target of IOCTL_POWER_Xxx commands to parent/bus drivers
// if there is a relationship established via RegisterPowerRelationship.
//
typedef struct _POWER_RELATIONSHIP {
    HANDLE  hParent;    // Handle to parent node
    LPCWSTR pwsParent;  // Named parent node, e.g. "NDS0:"
    HANDLE  hChild;     // Handle to child node, returned from RegisterPowerRelationship
    LPCWSTR pwsChild;   // Named child node, e.g. "NE20001"

} POWER_RELATIONSHIP, *PPOWER_RELATIONSHIP;


//
// Power Broadcast Types -- there are up to 32 of these
//
#define PBT_TRANSITION          0x00000001  // broadcast specifying system power state transition
#define PBT_RESUME              0x00000002  // broadcast notifying a resume, specifies previous state
#define PBT_POWERSTATUSCHANGE   0x00000004  // power supply switched to/from AC/DC
#define PBT_POWERINFOCHANGE     0x00000008  // some system power status field has changed

#define PBT_SUSPENDKEYPRESSED   0x00000100  // Suspend Key has been pressed.

// OEMS may define power notifications starting with this ID and
// going up by powers of 2.
#define PBT_OEMBASE             0x00010000

// This bitmask indicates that an application would like to receive all
// types of power notifications.
#define POWER_NOTIFY_ALL        0xFFFFFFFF

//
// Power Broadcast -- this is a variable length structure.
//
typedef struct _POWER_BROADCAST {
    DWORD       Message;    // one of PBT_Xxx
    DWORD       Flags;      // one of POWER_STATE_Xxx
    DWORD       Length;     // byte count of data starting at SystemPowerStateName
    WCHAR       SystemPowerState[1];    // variable length field, must be smaller than MAX_PATH + 1
} POWER_BROADCAST, *PPOWER_BROADCAST;

// This structure is used instead of a string name (SystemPowerState) in
// the POWER_BROADCAST, if the broadcast is of type PBT_POWERINFOCHANGE.
//
// For example:
//  PPOWER_BROADCAST ppb;
//  PPOWER_BROADCAST_POWER_INFO ppbpi = 
//        (PPOWER_BROADCAST_POWER_INFO) ppb->SystemPowerState;
//
typedef struct _POWER_BROADCAST_POWER_INFO {
    // levels available in battery flag fields, see BatteryDrvrGetLevels()
    DWORD       dwNumLevels;

    // see GetSystemPowerStatusEx2()
    DWORD       dwBatteryLifeTime;
    DWORD       dwBatteryFullLifeTime;
    DWORD       dwBackupBatteryLifeTime;
    DWORD       dwBackupBatteryFullLifeTime;
    BYTE        bACLineStatus;
    BYTE        bBatteryFlag;
    BYTE        bBatteryLifePercent;
    BYTE        bBackupBatteryFlag;
    BYTE        bBackupBatteryLifePercent;
} POWER_BROADCAST_POWER_INFO, *PPOWER_BROADCAST_POWER_INFO;
    
//*****************************************************************************
// P R O T O S
//*****************************************************************************
DWORD
GetSystemPowerState(
    LPWSTR  pBuffer,
    DWORD   dwBufChars,
    PDWORD  pdwFlags 
	);

DWORD
SetSystemPowerState(
	LPCWSTR pwsSystemState,
	DWORD   StateFlags,
    DWORD   Options
	);

HANDLE
SetPowerRequirement(
	PVOID                   pvDevice,
    CEDEVICE_POWER_STATE    DeviceState,	
    ULONG                   DeviceFlags,
    PVOID                   pvSystemState,
    ULONG                   StateFlags    
	);

DWORD
ReleasePowerRequirement(
	HANDLE hPowerReq
    );

HANDLE
RequestPowerNotifications(
    HANDLE  hMsgQ,
    DWORD   Flags
    );

DWORD
StopPowerNotifications(
    HANDLE h
    );

DWORD
DevicePowerNotify(
	PVOID                   pvDevice,
	CEDEVICE_POWER_STATE    DeviceState,
    DWORD                   Flags
    );

HANDLE
RegisterPowerRelationship(
    PVOID pvParent, 
    PVOID pvChild,
    PPOWER_CAPABILITIES pCaps,
    DWORD Flags
    );

DWORD
ReleasePowerRelationship(
    HANDLE hChild
    );

DWORD
SetDevicePower(
    PVOID pvDevice, 
    DWORD dwDeviceFlags, 
    CEDEVICE_POWER_STATE dwState
    );

DWORD
GetDevicePower(
    PVOID pvDevice, 
    DWORD dwDeviceFlags,
    PCEDEVICE_POWER_STATE pdwState    
    );

#ifdef WINCEMACRO
#include <mpm.h>
#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus */

