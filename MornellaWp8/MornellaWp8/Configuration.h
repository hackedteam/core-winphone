#include "Common.h"
#include <string>
#include <exception>
#include "JSON/JSON.h"

using namespace std;

#ifndef __Configuration_h__
#define __Configuration_h__

class Configuration {
	private:
		JSONObject json;

	public:
		Configuration(JSONObject jConf);
		~Configuration();

		INT getInt(const wstring& field);
		BOOL getBool(const wstring& field);
		double getDouble(const wstring& field);
		const wstring& getString(const wstring& field);

		INT getIntFromArray(const wstring& arrayName, const wstring& field);
		BOOL getBoolFromArray(const wstring& arrayName, const wstring& field);
		double getDoubleFromArray(const wstring& arrayName, const wstring& field);
		const wstring& getStringFromArray(const wstring& arrayName, const wstring& field);
		JSONObject getObjectFromArray(const wstring& arrayName, const wstring& field);
		BOOL getBoolFromObject(JSONObject& obj, const wstring& field);
		const wstring& getStringFromObject(JSONObject& obj, const wstring& field);
};

#endif