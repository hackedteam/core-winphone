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
#ifndef __MSGQUEUE_H__
#define __MSGQUEUE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//
// prototypes
//
HANDLE WINAPI CreateMsgQueue(LPCWSTR lpName, LPMSGQUEUEOPTIONS lpOptions);
HANDLE WINAPI OpenMsgQueue(HANDLE hSrcProc, HANDLE hMsgQ, LPMSGQUEUEOPTIONS lpOptions);
BOOL WINAPI ReadMsgQueue(HANDLE hMsgQ, __out_bcount(cbBufferSize) LPVOID lpBuffer, DWORD cbBufferSize,
                LPDWORD lpNumberOfBytesRead, DWORD dwTimeout, DWORD *pdwFlags);
BOOL WINAPI WriteMsgQueue(HANDLE hMsgQ, LPVOID lpBuffer, DWORD cbDataSize,
                DWORD dwTimeout, DWORD dwFlags);
BOOL WINAPI GetMsgQueueInfo(HANDLE hMsgQ, LPMSGQUEUEINFO lpInfo);

///BOOL WINAPI CloseMsgQueue(HANDLE hMsgQ);

#ifdef __cplusplus
}
#endif

#endif // __MSGQUEUE_H__

