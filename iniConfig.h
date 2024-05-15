#pragma once

class iniConfig
{
	CHAR buffer[1024];
	LPCSTR fileName;

	bool get(LPCSTR section, LPCSTR key, bool allowEmpty = false);
public:
	explicit constexpr iniConfig(LPCSTR file) : buffer(), fileName(file) { }
	void init();

	void removeKey(LPCSTR section, LPCSTR key) const;
	vector<int> getSectionInts(LPCSTR section);

	LPCSTR getStr(LPCSTR section, LPCSTR key, LPCSTR defValue = "");
	int getInt(LPCSTR section, LPCSTR key, int defValue);
	unsigned int getUInt(LPCSTR section, LPCSTR key, unsigned int defValue);
	float getFloat(LPCSTR section, LPCSTR key, float defValue);
	bool getBool(LPCSTR section, LPCSTR key, bool defValue);
	int getEnum(LPCSTR section, LPCSTR key, int defValue, pair<int, LPCSTR> map[], int size);
	vector<int> getInts(LPCSTR section, LPCSTR key);
	vector<float> getFloats(LPCSTR section, LPCSTR key);

	void setStr(LPCSTR section, LPCSTR key, LPCSTR value);
	void setInt(LPCSTR section, LPCSTR key, int value);
	void setUInt(LPCSTR section, LPCSTR key, unsigned int value, bool hex = false);
	void setFloat(LPCSTR section, LPCSTR key, float value);
	void setBool(LPCSTR section, LPCSTR key, bool value);
	void setEnum(LPCSTR section, LPCSTR key, int value, pair<int, LPCSTR> map[], int size);
	void setInts(LPCSTR section, LPCSTR key, vector<int> list);
	void setFloats(LPCSTR section, LPCSTR key, vector<float> list);
};
