#pragma once

struct FunctionStack
{
	FunctionStack(Variant& result, std::vector<Variant>& params);

	FunctionStack(Variant::Type type, std::vector<Variant>& params);

	FunctionStack(Variant& result);

	FunctionStack(Variant::Type type);

	inline int GetParamCount() const { return static_cast<int>(mParams.size()); }

	inline bool IsSameParamType() const { return mSameParamType; }

	inline Variant::Type GetPreferReturnType() const { return mPreferReturnType; }

	inline Variant::Type GetMaxParamType() const { return mMaxParamType; }

	void FormatSameParamType();

	Variant& mReturnValue;

	inline const std::vector<Variant>& Params() { return mParams; }

private:
	FunctionStack(const FunctionStack&) = delete;

	void SetMaxParamType();

	bool mSameParamType = true;
	Variant::Type mPreferReturnType = Variant::Type::None;
	Variant::Type mMaxParamType = Variant::Type::None;
	std::vector<Variant>& mParams;

	static std::vector<Variant> sDummyParamList;
	static Variant sDummyReturnValue;
};

struct FunctionAdapter
{
	typedef void(*Process)(const std::vector<Variant>&);
	typedef Variant(*Function)(const std::vector<Variant>&);

	enum class ParamCount { Fixed, MoreEqual, LessEqual, Range };

	static FunctionAdapter* Create(Process pointer);
	static FunctionAdapter* Create(Function pointer, Variant::Type rType);
	static FunctionAdapter* CreateFixed(Process pointer, Variant::Type pType, unsigned int paramCount);
	static FunctionAdapter* CreateFixed(Function pointer, Variant::Type rType, Variant::Type pType, unsigned int paramCount);
	static FunctionAdapter* CreateMoreEqual(Process pointer, Variant::Type pType, unsigned int moreEqual);
	static FunctionAdapter* CreateMoreEqual(Function pointer, Variant::Type rType, Variant::Type pType, unsigned int moreEqual);
	static FunctionAdapter* CreateLessEqual(Process pointer, Variant::Type pType, unsigned int lessEqual);
	static FunctionAdapter* CreateLessEqual(Function pointer, Variant::Type rType, Variant::Type pType, unsigned int lessEqual);
	static FunctionAdapter* CreateRange(Process pointer, Variant::Type pType, unsigned int minEqual, unsigned int maxEqual);
	static FunctionAdapter* CreateRange(Function pointer, Variant::Type rType, Variant::Type pType, unsigned int minEqual, unsigned int maxEqual);

	FunctionAdapter(const FunctionAdapter&) = delete;

	bool IsSuitable(FunctionStack& stack);

	void operator()(FunctionStack& pStack);

	bool IsProcess() const;

	inline Variant::Type GetReturnType() const { return mReturnType; }

	inline Variant::Type GetParamType() const { return mParamType; }

	inline ParamCount GetParamCount() const { return mParamCount; }

	inline unsigned int GetParamCountMin() const { return mParamCountMin; }

	inline unsigned int GetParamCountMax() const { return mParamCountMax; }

	inline unsigned int GetRangeParamCount() const { return mParamCountMax - mParamCountMin; }

private:
	FunctionAdapter(Process function, Variant::Type pType, ParamCount pCount, unsigned int pMin, unsigned int pMax);
	FunctionAdapter(Function pointer, Variant::Type rType, Variant::Type pType, ParamCount pCount, unsigned int pMin, unsigned int pMax);

	const Process mProcess;
	const Function mFunction;

	const Variant::Type mReturnType;
	const Variant::Type mParamType;
	const ParamCount mParamCount;
	const unsigned int mParamCountMin;
	const unsigned int mParamCountMax;
};

struct FunctionSorter
{
	bool operator()(FunctionAdapter* lhs, FunctionAdapter* rhs) const;
};
