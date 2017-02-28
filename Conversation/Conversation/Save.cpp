#include "Save.h"
#include "cxx.h"

bool Save::HasValue(const std::string& name)
{
	return mValues.count(name) > 0;
}

SaveValue& Save::GetValueByName(const std::string& name)
{
	if (!HasValue(name))
	{
		mValueNames.push_back(name);
		mValues.insert(std::make_pair(name, SaveValue()));
	}
	return mValues[name];
}

const char* Save::GetValueNameByIndex(int index)
{
	if (index >= 0 && index < GetValueCount())
	{
		return mValueNames[index].c_str();
	}
	else
	{
		return "";
	}
}

void Save::Clear()
{
	mValueNames.clear();
	mValues.clear();
}

void Save::SetValue(const std::string& name, bool v)
{
	if (HasValue(name))
	{
		mValues[name].Value = v;
	}
	else
	{
		SaveValue& sv = GetValueByName(name);
		sv.ReadOnly = false;
		sv.Value = Variant(v);
	}
}
void Save::SetValue(const std::string& name, int v)
{
	if (HasValue(name))
	{
		mValues[name].Value = v;
	}
	else
	{
		SaveValue& sv = GetValueByName(name);
		sv.ReadOnly = false;
		sv.Value = Variant(v);
	}
}

void Save::SetValue(const std::string& name, float v)
{
	if (HasValue(name))
	{
		mValues[name].Value = v;
	}
	else
	{
		SaveValue& sv = GetValueByName(name);
		sv.ReadOnly = false;
		sv.Value = Variant(v);
	}
}