#include "Configuration.h"

Configuration::Configuration(JSONObject jConf) {
	json = jConf;

#ifdef _DEBUG
	WCHAR msg[128];
	map<std::wstring, JSONValue*>::const_iterator iter;

	//wprintf(L"\tConfiguration: \n");
	OutputDebugString(L"\tConfiguration: \n");

	for (iter = json.begin(); iter != json.end(); ++iter) {
		//wprintf(L"\t\t%s: %s\n", iter->first.c_str(), iter->second->Stringify().c_str());
		swprintf_s(msg, L"\t\t%s: %s\n", iter->first.c_str(), iter->second->Stringify().c_str());OutputDebugString(msg);
	}

	//wprintf(L"\n");
	OutputDebugString(L"\n");
#endif

}

Configuration::~Configuration() {

}

INT Configuration::getInt(const wstring& field) {
	JSONValue *c = json[field];
	
	if (c == NULL || c->IsNumber() == FALSE) {
#ifdef _DEBUG
		OutputDebugString(L"eccezione gestita: ");
#endif
		throw new exception();
	}

	return static_cast<INT>(c->AsNumber());
}

INT Configuration::getIntFromArray(const wstring& arrayName, const wstring& field) {
	JSONObject::const_iterator iter;

	JSONValue *c = json[arrayName];

	if (c == NULL || c->IsObject() == FALSE) {
#ifdef _DEBUG
		OutputDebugString(L"eccezione gestita: ");
#endif
		throw new exception();
	}

	JSONObject arr = c->AsObject();
	JSONValue *val = arr[field];

	if (val == NULL || val->IsNumber() == FALSE) {
#ifdef _DEBUG
		OutputDebugString(L"eccezione gestita: ");
#endif
		throw new exception();
	}
	return static_cast<INT>(val->AsNumber());
}

BOOL Configuration::getBool(const wstring& field) {
	JSONValue *c = json[field];

	if (c == NULL || c->IsBool() == FALSE) {
#ifdef _DEBUG
		OutputDebugString(L"eccezione gestita: ");
#endif
		throw new exception();
	}

	return static_cast<BOOL>(c->AsBool());
}

BOOL Configuration::getBoolFromArray(const wstring& arrayName, const wstring& field) {
	JSONObject::const_iterator iter;

	JSONValue *c = json[arrayName];

	if (c == NULL || c->IsObject() == FALSE) {
#ifdef _DEBUG
		OutputDebugString(L"eccezione gestita: ");
#endif
		throw new exception();
	}

	JSONObject arr = c->AsObject();
	JSONValue *val = arr[field];

	if (val == NULL || val->IsBool() == FALSE) {
#ifdef _DEBUG
		OutputDebugString(L"eccezione gestita: ");
#endif
		throw new exception();
	}

	return static_cast<BOOL>(val->AsBool());
}

JSONObject Configuration::getObjectFromArray(const wstring& arrayName, const wstring& field) {
	JSONValue *c = json[arrayName];

	if (c == NULL || c->IsObject() == FALSE) {
#ifdef _DEBUG
		OutputDebugString(L"eccezione gestita: ");
#endif
		throw new exception();
	}

	JSONObject arr = c->AsObject();
	JSONValue *val = arr[field];

	if (val == NULL || val->IsObject() == FALSE) {
#ifdef _DEBUG
		OutputDebugString(L"eccezione gestita: ");
#endif
		throw new exception();
	}

	return val->AsObject();
}

BOOL Configuration::getBoolFromObject(JSONObject& obj, const wstring& field) {
	JSONValue *val = obj[field];

	if (val == NULL || val->IsBool() == FALSE) {
#ifdef _DEBUG
		OutputDebugString(L"eccezione gestita: ");
#endif
		throw new exception();
	}

	return static_cast<BOOL>(val->AsBool());
}

const wstring& Configuration::getStringFromObject(JSONObject& obj, const wstring& field) {
	JSONValue *val = obj[field];

	if (val == NULL || val->IsString() == FALSE) {
#ifdef _DEBUG
		OutputDebugString(L"eccezione gestita: ");
#endif
		throw new exception();
	}

	return val->AsString();
}

double Configuration::getDouble(const wstring& field) {
	JSONValue *c = json[field];

	if (c == NULL || c->IsNumber() == FALSE) {
#ifdef _DEBUG
		OutputDebugString(L"eccezione gestita: ");
#endif
		throw new exception();
	}

	return static_cast<double>(c->AsNumber());
}

double Configuration::getDoubleFromArray(const wstring& arrayName, const wstring& field) {
	JSONObject::const_iterator iter;

	JSONValue *c = json[arrayName];

	if (c == NULL || c->IsObject() == FALSE) {
#ifdef _DEBUG
		OutputDebugString(L"eccezione gestita: ");
#endif
		throw new exception();
	}

	JSONObject arr = c->AsObject();
	JSONValue *val = arr[field];

	if (val == NULL || val->IsNumber() == FALSE) {
#ifdef _DEBUG
		OutputDebugString(L"eccezione gestita: ");
#endif
		throw new exception();
	}

	return static_cast<double>(val->AsNumber());
}

const wstring& Configuration::getString(const wstring& field) {
	JSONValue *c = json[field];

	if (c == NULL || c->IsString() == FALSE) {
#ifdef _DEBUG
		OutputDebugString(L"eccezione gestita: ");
#endif
		throw new exception();
	}

	return c->AsString();
}

const wstring& Configuration::getStringFromArray(const wstring& arrayName, const wstring& field) {
	JSONObject::const_iterator iter;

	JSONValue *c = json[arrayName];

	if (c == NULL || c->IsObject() == FALSE) {
#ifdef _DEBUG
		OutputDebugString(L"eccezione gestita: ");
#endif
		throw new exception();
	}

	JSONObject arr = c->AsObject();
	JSONValue *val = arr[field];

	if (val == NULL || val->IsString() == FALSE) {
#ifdef _DEBUG
		OutputDebugString(L"eccezione gestita: ");
#endif
		throw new exception();
	}

	return val->AsString();
}