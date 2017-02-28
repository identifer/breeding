#include <tuple>
#include "Conversation.h"
#include "Conversation.h"
#include "cxx.h"


std::vector<Variant> FunctionStack::sDummyParamList;
Variant FunctionStack::sDummyReturnValue;

FunctionStack::FunctionStack(Variant& result, std::vector<Variant>& params)
	: mReturnValue(result)
	, mPreferReturnType(result.GetType())
	, mParams(params)
{
	SetMaxParamType();
}

FunctionStack::FunctionStack(Variant::Type type, std::vector<Variant>& params)
	: mReturnValue(sDummyReturnValue)
	, mPreferReturnType(type)
	, mParams(params)
{
	SetMaxParamType();
}

FunctionStack::FunctionStack(Variant & result)
	: mReturnValue(result)
	, mPreferReturnType(result.GetType())
	, mParams(sDummyParamList)
{
	mSameParamType = true;
}

FunctionStack::FunctionStack(Variant::Type type)
	: mReturnValue(sDummyReturnValue)
	, mPreferReturnType(type)
	, mParams(sDummyParamList)
{
	mSameParamType = true;
}

void FunctionStack::FormatSameParamType()
{
	if (!mSameParamType)
	{
		for (Variant& v : mParams)
		{
			v.ConvertTo(mMaxParamType);
		}
		mSameParamType = true;
	}
}

void FunctionStack::SetMaxParamType()
{
	if (mParams.size() > 0)
	{
		mMaxParamType = mParams[0].GetType();
		for (Variant& v : mParams)
		{
			if (v.GetType() > mMaxParamType)
			{
				mMaxParamType = v.GetType();
				mSameParamType = false;
			}
		}
	}
}

FunctionAdapter::FunctionAdapter(Process function, Variant::Type pType, ParamCount pCount, unsigned int pMin, unsigned int pMax)
	: mProcess(function)
	, mFunction(nullptr)
	, mReturnType(Variant::Type::None)
	, mParamType(pType)
	, mParamCount(pCount)
	, mParamCountMin(pMin)
	, mParamCountMax(pMax)
{

}

FunctionAdapter::FunctionAdapter(Function pointer, Variant::Type rType, Variant::Type pType, ParamCount pCount, unsigned int pMin, unsigned int pMax)
	: mProcess(nullptr)
	, mFunction(pointer)
	, mReturnType(rType)
	, mParamType(pType)
	, mParamCount(pCount)
	, mParamCountMin(pMin)
	, mParamCountMax(pMax)
{

}

FunctionAdapter* FunctionAdapter::Create(Process pointer)
{
	if (pointer != nullptr)
	{
		return CreateFixed(pointer, Variant::Type::None, 0);
	}
	else
	{
		return nullptr;
	}
}

FunctionAdapter* FunctionAdapter::Create(Function pointer, Variant::Type retType)
{
	if (pointer != nullptr && retType != Variant::Type::None)
	{
		return CreateFixed(pointer, retType, Variant::Type::None, 0);
	}
	else
	{
		return nullptr;
	}
}

FunctionAdapter * FunctionAdapter::CreateFixed(Process pointer, Variant::Type pType, unsigned int paramCount)
{
	if (pointer != nullptr)
	{
		return new FunctionAdapter(pointer, pType, ParamCount::Fixed, paramCount, paramCount);
	}
	else
	{
		return nullptr;
	}
}

FunctionAdapter* FunctionAdapter::CreateFixed(Function pointer, Variant::Type rType, Variant::Type pType, unsigned int paramCount)
{
	if (pointer != nullptr && rType != Variant::Type::None)
	{
		return new FunctionAdapter(pointer, rType, pType, ParamCount::Fixed, paramCount, paramCount);
	}
	else
	{
		return nullptr;
	}
}

FunctionAdapter * FunctionAdapter::CreateMoreEqual(Process pointer, Variant::Type pType, unsigned int moreEqual)
{
	if (pointer != nullptr)
	{
		return new FunctionAdapter(pointer, pType, ParamCount::MoreEqual, moreEqual, moreEqual);
	}
	else
	{
		return nullptr;
	}
}

FunctionAdapter* FunctionAdapter::CreateMoreEqual(Function pointer, Variant::Type rType, Variant::Type pType, unsigned int moreEqual)
{
	if (pointer != nullptr && rType != Variant::Type::None)
	{
		return new FunctionAdapter(pointer, rType, pType, ParamCount::MoreEqual, moreEqual, moreEqual);
	}
	else
	{
		return nullptr;
	}
}

FunctionAdapter * FunctionAdapter::CreateLessEqual(Process pointer, Variant::Type pType, unsigned int lessEqual)
{
	if (pointer != nullptr && lessEqual > 0)
	{
		return new FunctionAdapter(pointer, pType, ParamCount::LessEqual, lessEqual, lessEqual);
	}
	else
	{
		return nullptr;
	}
}

FunctionAdapter* FunctionAdapter::CreateLessEqual(Function pointer, Variant::Type rType, Variant::Type pType, unsigned int lessEqual)
{
	if (pointer != nullptr && rType != Variant::Type::None && lessEqual > 0)
	{
		return new FunctionAdapter(pointer, rType, pType, ParamCount::LessEqual, lessEqual, lessEqual);
	}
	else
	{
		return nullptr;
	}
}

FunctionAdapter * FunctionAdapter::CreateRange(Process pointer, Variant::Type pType, unsigned int minEqual, unsigned int maxEqual)
{
	if (pointer != nullptr  && minEqual < maxEqual && minEqual > 0)
	{
		return new FunctionAdapter(pointer, pType, ParamCount::Range, minEqual, maxEqual);
	}
	else
	{
		return nullptr;
	}
}

FunctionAdapter* FunctionAdapter::CreateRange(Function pointer, Variant::Type rType, Variant::Type pType, unsigned int minEqual, unsigned int maxEqual)
{
	if (pointer != nullptr  && rType != Variant::Type::None && minEqual < maxEqual && minEqual > 0)
	{
		return new FunctionAdapter(pointer, rType, pType, ParamCount::Range, minEqual, maxEqual);
	}
	else
	{
		return nullptr;
	}
}

bool FunctionAdapter::IsSuitable(FunctionStack& stack)
{
	unsigned int paramCount = stack.GetParamCount();
	if (stack.GetPreferReturnType() == Variant::Type::None && mProcess != nullptr)
	{
		// Process
		if ((mParamCount == ParamCount::Fixed && paramCount != mParamCountMin) ||
			(mParamCount == ParamCount::MoreEqual && paramCount < mParamCountMin) ||
			(mParamCount == ParamCount::LessEqual && paramCount > mParamCountMin) ||
			(mParamCount == ParamCount::Range && (paramCount < mParamCountMin || paramCount > mParamCountMax)))
		{
			return false;
		}

		return  stack.GetMaxParamType() <= mParamType;
	}
	else if(mFunction != nullptr)
		//VFunction
	{
		if ((mParamCount == ParamCount::Fixed && paramCount != mParamCountMin) ||
			(mParamCount == ParamCount::MoreEqual && paramCount < mParamCountMin) ||
			(mParamCount == ParamCount::LessEqual && paramCount > mParamCountMin) ||
			(mParamCount == ParamCount::Range && (paramCount< mParamCountMin || paramCount > mParamCountMax)))
		{
			return false;
		}

		return  stack.GetMaxParamType() <= mParamType && stack.GetPreferReturnType() <= mReturnType;
	}
	else
	{
		return false;
	}
}

void FunctionAdapter::operator()(FunctionStack &pStack)
{
	if (mProcess != nullptr)
	{
		mProcess(pStack.Params());
	}
	else
	{
		pStack.mReturnValue = mFunction(pStack.Params());
	}
}

bool FunctionAdapter::IsProcess() const
{
	return mReturnType == Variant::Type::None;
}

bool FunctionSorter::operator()(FunctionAdapter * lhs, FunctionAdapter * rhs) const
{
	auto compare = cxx::Compare(lhs->GetParamCount(), rhs->GetParamCount());

	//判断参数数量
	if (compare.IsEqual() && lhs->GetParamCount() == FunctionAdapter::ParamCount::Range)
	{
		compare.Then(lhs->GetRangeParamCount(), rhs->GetRangeParamCount())
			.Then(rhs->GetParamCountMin(), lhs->GetParamCountMin()) //min越小，排序越后
			.Then(lhs->GetParamCountMax(), rhs->GetParamCountMax())
			.Then(lhs->GetParamType(), rhs->GetParamType())
			.Then(lhs->GetReturnType(), rhs->GetReturnType());;
	}
	else
	{
		compare.Then(lhs->GetParamCountMin(), rhs->GetParamCountMin())
			.Then(lhs->GetParamType(), rhs->GetParamType())
			.Then(lhs->GetReturnType(), rhs->GetReturnType());;
	}

	return compare.IsLess();
}
