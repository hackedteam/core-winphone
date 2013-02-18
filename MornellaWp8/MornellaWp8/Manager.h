#include "Common.h"
#include <string>
#include <exception>
#include "JSON/JSON.h"

#ifndef __Manager_h__
#define __Manager_h__

class Manager { 
	private: 
		HANDLE hMutex;

	public:
		Manager();
		~Manager();
};

#endif