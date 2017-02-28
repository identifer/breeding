#pragma once

template<class...> struct SizeOf;

template<class T> struct SizeOf<T>
{
	static constexpr const int Value = sizeof(T);
};

template <class T, class... Tn> struct SizeOf<T, Tn...>
{
	static constexpr const int Value =
		SizeOf<Tn...>::Value > sizeof(T)
		? SizeOf<Tn...>::Value
		: sizeof(T);
};

class Variant
{
public:
	enum class Type : unsigned int { None, Bool, Int, Float };

	Variant(Type t = Type::None);

	explicit Variant(int v);

	explicit Variant(float v);

	explicit Variant(bool v);

	Variant(const Variant& other);
	
	Variant& operator=(int v);

	Variant& operator=(float v);

	Variant& operator=(bool v);

	Variant& operator=(const Variant& rhs);

	Variant& operator+=(const Variant& rhs);

	Variant& operator-=(const Variant& rhs);

	Variant& operator*=(const Variant& rhs);

	bool operator>(const Variant& rhs) const;

	bool operator<(const Variant& rhs) const;

	bool operator>=(const Variant& rhs) const;

	bool operator<=(const Variant& rhs) const;

	bool operator==(const Variant& rhs) const;

	bool operator!=(const Variant& rhs) const;

	bool operator!() const;

	Variant operator-() const;

	explicit operator float() const;

	explicit operator int() const;

	explicit operator bool() const;

	inline Type GetType() const { return mType; }

	template <class T>
	inline T As() const { return Ref<T>(); }

	template <class T>
	inline T To() const { return (T)(*this); }

	template <class T>
	inline T Convert() const { *this = To<T>(); }

	void ConvertTo(Type type);

private:
	typedef unsigned char byte;
	typedef SizeOf<int, float, bool> VariantSize;

	Type mType = Type::None;
	Variant::byte mValue[VariantSize::Value];

	template <class T>
	inline T* Ptr() const { return (T*)mValue; }

	template <class T>
	inline T& Ref() const { return *Ptr<T>(); }
};

inline Variant::Type MaxT(Variant::Type l, Variant::Type r)
{
	return l > r ? l : r;
}

inline Variant::Type MaxT(Variant::Type l, Variant::Type r, Variant::Type s)
{
	return MaxT(MaxT(l, r), s);
}
