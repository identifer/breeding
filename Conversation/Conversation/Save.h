#pragma once
#include <vector>
#include <map>
#include <set>
#include <string>
#include "variant.h"

struct SaveValue
{
	explicit SaveValue() : ReadOnly(false) { }

	bool ReadOnly = false;
	Variant Value;
};

struct Save
{
	bool HasValue(const std::string& name);

	SaveValue& GetValueByName(const std::string& name);

	const char* GetValueNameByIndex(int index);

	int GetValueCount() const { return static_cast<int>(mValueNames.size()); }

	void SetValue(const std::string& name, bool v);

	void SetValue(const std::string& name, int v);

	void SetValue(const std::string& name, float v);

	void Clear();

private:
	std::vector<std::string> mValueNames;
	std::map<std::string, SaveValue> mValues;
};

