#include<string>
#include<cmath>
#include "Variant.h"

Variant::Variant(Type t)
	: mType(t)
{
	memset(mValue, 0, VariantSize::Value);
}

Variant::Variant(const Variant& other)
	: mType(other.mType)
{
	memcpy(mValue, other.mValue, VariantSize::Value);
}

Variant::Variant(int v)
{
	*this = v;
}

Variant::Variant(float v)
{
	*this = v;
}

Variant::Variant(bool v)
{
	*this = v;
}

Variant& Variant::operator=(int v)
{
	mType = Type::Int;
	Ref<int>() = v;
	return *this;
}

Variant& Variant::operator=(float v)
{
	mType = Type::Float;
	Ref<float>() = v;
	return *this;
}

Variant& Variant::operator=(bool v)
{
	mType = Type::Bool;
	Ref<bool>() = v;
	return *this;
}

Variant& Variant::operator=(const Variant& rhs)
{
	if (&rhs != this)
	{
		mType = rhs.mType;
		memcpy(mValue, rhs.mValue, VariantSize::Value);
	}
	return *this;
}


Variant& Variant::operator+=(const Variant& rhs)
{
	switch (MaxT(mType, rhs.mType))
	{
	case Type::Bool:
		*this = (To<int>() ^ rhs.To<int>()) != 0;
		break;
	case Type::Int:
		*this = To<int>() + rhs.To<int>();
		break;
	case Type::Float:
		*this = To<float>() + rhs.To<float>();
		break;
	}
	return *this;
}

Variant& Variant::operator-=(const Variant& rhs)
{
	switch (MaxT(mType, rhs.mType))
	{
	case Type::Bool:
		*this = (To<int>() ^ rhs.To<int>()) > 0;
		break;
	case Type::Int:
		*this = To<int>() - rhs.To<int>();
		break;
	case Type::Float:
		*this = To<float>() - rhs.To<float>();
		break;
	}
	return *this;
}

Variant& Variant::operator*=(const Variant& rhs)
{
	switch (MaxT(mType, rhs.mType))
	{
	case Type::Bool:
		*this = To<bool>() || rhs.To<bool>();
		break;
	case Type::Int:
		*this = To<int>() * rhs.To<int>();
		break;
	case Type::Float:
		*this = To<float>() * rhs.To<float>();
		break;
	}
	return *this;
}

bool Variant::operator> (const Variant& rhs) const
{
	if (this == &rhs)
	{
		return false;
	}
	else if (rhs.mType == mType)
	{
		switch (mType)
		{
		case Type::Bool: return To<int>() > rhs.To<int>();
		case Type::Int: return As<int>() > rhs.As<int>();
		case Type::Float: return As<float>() > rhs.As<float>();
		}
	}
	else
	{
		switch (MaxT(mType, rhs.mType))
		{
		case Type::Bool:
		case Type::Int: return To<int>() > rhs.To<int>();
		case Type::Float: return To<float>() > rhs.To<float>();
		}
	}
	return false;
}

bool Variant::operator< (const Variant& rhs) const
{
	if (this == &rhs)
	{
		return false;
	}
	else if (rhs.mType == mType)
	{
		switch (mType)
		{
		case Type::Bool: return To<int>() < rhs.To<int>();
		case Type::Int: return As<int>() < rhs.As<int>();
		case Type::Float: return As<float>() < rhs.As<float>();
		}
	}
	else
	{
		switch (MaxT(mType, rhs.mType))
		{
		case Type::Bool:
		case Type::Int: return To<int>() < rhs.To<int>();
		case Type::Float: return To<float>() < rhs.To<float>();
		}
	}
	return false;
}

bool Variant::operator>= (const Variant& rhs) const
{
	return !(*this < rhs);
}

bool Variant::operator<= (const Variant& rhs) const
{
	return !(*this > rhs);
}

bool Variant::operator== (const Variant& rhs) const
{
	if (this == &rhs)
	{
		return true;
	}
	else if (rhs.mType == mType)
	{
		switch (mType)
		{
		default:
		case Type::None: return true;
		case Type::Bool: return As<bool>() == rhs.As<bool>();
		case Type::Int: return As<int>() == rhs.As<int>();
		case Type::Float: return As<float>() == rhs.As<float>();
		}
	}
	else
	{
		switch (MaxT(mType, rhs.mType))
		{
		case Type::None: return true;
		default:
		case Type::Bool: return To<bool>() == rhs.To<bool>();
		case Type::Int: return To<int>() == rhs.To<int>();
		case Type::Float: return fabs(To<float>() - rhs.To<float>()) < 10e-6;
		}
	}
}

bool Variant::operator!= (const Variant& rhs) const
{
	return !(*this == rhs);
}

bool Variant::operator!() const
{
	return !To<bool>();
}

Variant Variant::operator-() const
{
	switch (mType)
	{
	default:
	case Type::None: return Variant();
	case Type::Bool: return As<bool>() ? Variant(-1) : Variant(0);
	case Type::Int: return Variant(-As<int>());
	case Type::Float: return Variant(-As<float>());
	}
}

Variant::operator float() const
{
	switch (mType)
	{
	default:
	case Type::None: return 0.0f;
	case Type::Bool: return As<bool>() ? 1.0f : 0.0f;
	case Type::Int: return static_cast<float>(As<int>());
	case Type::Float: return As<float>();
	}
}

Variant::operator int() const
{
	switch (mType)
	{
	default:
	case Type::None: return 0;
	case Type::Bool: return As<bool>() ? 1 : 0;
	case Type::Int: return As<int>();
	case Type::Float: return static_cast<int>(As<float>());
	}
}

Variant::operator bool() const
{
	switch (mType)
	{
	default:
	case Type::None: return false;
	case Type::Bool: return As<bool>();
	case Type::Int: return As<int>() != 0;
	case Type::Float: return fabs(As<float>()) < 10e-6;
	}
}

void Variant::ConvertTo(Type type)
{
	switch (type)
	{
	default:
	case Type::None:
		*this = Variant();
		break;
	case Type::Bool:
		*this = To<bool>();
		break;
	case Type::Int:
		*this = To<int>();
		break;
	case Type::Float:
		*this = To<float>();
		break;
	}
}
