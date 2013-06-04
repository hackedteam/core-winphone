#pragma once
// MANAGEMENT - Questa classe richiede l'utilizzo di CoInitializeEx(NULL, 0) e CoUninitialize()
// Queste chiamate vanno pero' effettuate PRIMA di costruire la classe e DOPO averla distrutta
// altrimenti i CComPtr vengono rimossi dopo la CoUninitialize() ed il programma crasha.

#include <Windows.h>
/***
#include <atlbase.h>
#include <imaging.h>

#define SNAPSHOT_DEFAULT_BITSPIXEL		16
#define SNAPSHOT_DEFAULT_TIMESTEP		1000
#define SNAPSHOT_DEFAULT_JPEG_QUALITY	75
#define BYTESPERLINE(Width, BPP) ((WORD)((((DWORD)(Width) * (DWORD)(BPP) + 31) >> 5)) << 2)

#define SNAPSHOT_SAFE_EXIT()				\
	if (hDCMem && hBmpStock)				\
	SelectObject(hDCMem, hBmpStock);		\
	if (*hDIB)								\
	DeleteObject(*hDIB);					\
	if (hDCMem)								\
	DeleteDC(hDCMem);		
	***/
class CMSnapShot
{
public:
	CMSnapShot();
	virtual ~CMSnapShot();
	void Run();
///	void SetQuality(DWORD uQuality){ m_lQuality = (LONG) uQuality; }

private:
/***
	BOOL	m_bInit, m_bEncInit;
	UINT	m_x, m_y;
	LONG	m_lQuality;
	RECT	m_rExtents;
	HDC		m_hDCScreen;
	HWND	m_hWndDesktop;

	BITMAPFILEHEADER	m_bfh;
	LPBITMAPINFO		m_pbmi;

	GUID m_guidJpeg;
	CComPtr<IImagingFactory> m_pImgFactory;	
***/
	void	_Run();
	BOOL	_Init();
	BOOL	_InitBMP();
	BOOL	_CreateDIBSection(HBITMAP *hDCMem, LPBYTE &lpBuf);
	ULONG	_SaveBufferToImage(LPBYTE lpInBuf, LPBYTE* lpOutBuf);
};