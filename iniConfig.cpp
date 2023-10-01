#include "stdafx.h"
#include "iniConfig.h"
#include "dinput8.h"

iniConfig::iniConfig(LPCSTR fileName)
{
	this->fileName = fileName;

	SetLastError(ERROR_SUCCESS);
	GetPrivateProfileSectionNamesA(buffer, sizeof buffer, fileName);
	if (GetLastError() == ERROR_FILE_NOT_FOUND)
		LOG("Config: file not found!\n");
}

bool iniConfig::get(LPCSTR section, LPCSTR key, bool allowEmpty)
{
	SetLastError(ERROR_SUCCESS);
	DWORD result = GetPrivateProfileStringA(section, key, nullptr, buffer, sizeof buffer, fileName);
	return GetLastError() != ERROR_FILE_NOT_FOUND && (allowEmpty || result > 0);
}

void iniConfig::removeKey(LPCSTR section, LPCSTR key) const { WritePrivateProfileStringA(section, key, nullptr, fileName); }
std::vector<int> iniConfig::getSectionInts(LPCSTR section)
{
	std::vector<int> keys;
	if (get(section, nullptr, false))
	{
		LPSTR nextKey = buffer;
		while (*nextKey != '\0')
		{
			try { keys.push_back(std::stoi(nextKey)); }
			catch (...) {}
			nextKey = nextKey + strlen(nextKey) + 1;
		}
	}
	return keys;
}

string iniConfig::getStr(LPCSTR section, LPCSTR key, string defValue)
{
	if (get(section, key, defValue.empty()))
		return buffer;
	LOG("Config: %s->%s has invalid value, using default (%s)\n", section, key, defValue.c_str());
	return defValue;
}

int iniConfig::getInt(LPCSTR section, LPCSTR key, int defValue)
{
	try
	{
		if (get(section, key))
			return std::stoi(buffer, nullptr, 0);
	}
	catch (...) {}
	LOG("Config: %s->%s has invalid value, using default (%d)\n", section, key, defValue);
	return defValue;
}

unsigned int iniConfig::getUInt(LPCSTR section, LPCSTR key, unsigned defValue)
{
	try
	{
		if (get(section, key))
			return std::stoul(buffer, nullptr, 0);
	}
	catch (...) {}
	LOG("Config: %s->%s has invalid value, using default (%u)\n", section, key, defValue);
	return defValue;
}

float iniConfig::getFloat(LPCSTR section, LPCSTR key, float defValue)
{
	try
	{
		if (get(section, key))
			return std::stof(buffer, nullptr);
	}
	catch (...) {}
	LOG("Config: %s->%s has invalid value, using default (%.1f)\n", section, key, defValue);
	return defValue;
}

double iniConfig::getDouble(LPCSTR section, LPCSTR key, double defValue)
{
	try
	{
		if (get(section, key))
			return std::stod(buffer, nullptr);
	}
	catch (...) {}
	LOG("Config: %s->%s has invalid value, using default (%.1f)\n", section, key, defValue);
	return defValue;
}

bool iniConfig::getBool(LPCSTR section, LPCSTR key, bool defValue)
{
	try
	{
		if (get(section, key))
		{
			if (_stricmp("true", buffer) == 0 || _stricmp("on", buffer) == 0)
				return true;
			if (_stricmp("false", buffer) == 0 || _stricmp("off", buffer) == 0)
				return false;
		}
	}
	catch (...) {}
	LOG("Config: %s->%s has invalid value, using default (%s)\n", section, key, defValue ? "on": "off");
	return defValue;
}

int iniConfig::getEnum(LPCSTR section, LPCSTR key, int defValue, std::pair<int, LPCSTR> map[], int size)
{
	try
	{
		if (get(section, key))
		{
			for (int i = 0; i < size; i++)
				if (_stricmp(map[i].second, buffer) == 0)
					return map[i].first;
		}
	}
	catch (...) {}
	LOG("Config: %s->%s has invalid value, using default (%d)\n", section, key, defValue);
	return defValue;
}

std::vector<int> iniConfig::getInts(LPCSTR section, LPCSTR key)
{
	try
	{
		if (get(section, key, true))
		{
			std::vector<int> list;
			char *context, *token = strtok_s(buffer, ";", &context);
			while (token != nullptr)
			{
				list.push_back(std::stoi(token));
				token = strtok_s(nullptr, ";", &context);
			}
			return list;
		}
	}
	catch (...) {}
	LOG("Config: %s->%s has invalid value, using default ()\n", section, key);
	return std::vector<int>();
}

std::vector<float> iniConfig::getFloats(LPCSTR section, LPCSTR key)
{
	try
	{
		if (get(section, key, true))
		{
			std::vector<float> list;
			char *context, *token = strtok_s(buffer, ";", &context);
			while (token != nullptr)
			{
				list.push_back(std::stof(token));
				token = strtok_s(nullptr, ";", &context);
			}
			return list;
		}
	}
	catch (...) {}
	LOG("Config: %s->%s has invalid value, using default ()\n", section, key);
	return std::vector<float>();
}

void iniConfig::setStr(LPCSTR section, LPCSTR key, string value) const
{
	WritePrivateProfileStringA(section, key, (" " + value).c_str(), fileName);
}

void iniConfig::setInt(LPCSTR section, LPCSTR key, int value) const { setStr(section, key, std::to_string(value)); }
void iniConfig::setUInt(LPCSTR section, LPCSTR key, unsigned value, bool hex) const
{
	if (hex)
	{
		char buf[16];
		snprintf(buf, sizeof buf, "0x%08X", value);
		setStr(section, key, buf);
	}
	else
		setStr(section, key, std::to_string(value));
}

void iniConfig::setFloat(LPCSTR section, LPCSTR key, float value) const { setStr(section, key, std::to_string(value)); }
void iniConfig::setDouble(LPCSTR section, LPCSTR key, double value) const { setStr(section, key, std::to_string(value)); }
void iniConfig::setBool(LPCSTR section, LPCSTR key, bool value) const { setStr(section, key, value ? "on" : "off"); };
void iniConfig::setEnum(LPCSTR section, LPCSTR key, int value, std::pair<int, LPCSTR> map[], int size) const
{
	for (int i = 0; i < size; i++)
		if (map[i].first == value)
		{
			setStr(section, key, map[i].second);
			return;
		}
	setStr(section, key, std::to_string(value));
}

void iniConfig::setInts(LPCSTR section, LPCSTR key, std::vector<int> list) const
{
	string str;
	for (size_t i = 0; i < list.size(); i++)
	{
		str += std::to_string(list[i]);
		if (i < list.size() - 1)
			str += ";";
	}
	setStr(section, key, str);
}

void iniConfig::setFloats(LPCSTR section, LPCSTR key, std::vector<float> list) const
{
	string str;
	for (size_t i = 0; i < list.size(); i++)
	{
		str += std::to_string(list[i]);
		if (i < list.size() - 1)
			str += ";";
	}
	setStr(section, key, str);
}
