#include <cctype>
#include <algorithm>
#include <random>
#include "Conversation.h"
#include "pugixml/pugixml.hpp"
#include "cxx.h"

std::default_random_engine g_RandomEngine;

template<class T>
T RandomF(T min, T max)
{
	if (min > max)
	{
		std::uniform_real_distribution<T> distribution(max, min);
		return distribution(g_RandomEngine);
	}
	else if (max > min)
	{
		std::uniform_real_distribution<T> distribution(min, max);
		return distribution(g_RandomEngine);
	}
	return min;
}

template<class T>
T RandomI(T min, T max)
{
	if (min > max)
	{
		std::uniform_int_distribution<T> distribution(max, min);
		return distribution(g_RandomEngine);
	}
	else if (max > min)
	{
		std::uniform_int_distribution<T> distribution(min, max);
		return distribution(g_RandomEngine);
	}
	return min;
}

template<class T>
T RandomExceptF(T min, T max, T except)
{
	T result = RandomF(min, max);
	while (result == except || fabs(result - except) < 10e-6)
	{
		result = RandomF(min, max);
	}
	return result;
}

template<class T>
T RandomExceptI(T min, T max, T except)
{
	T result = RandomI(min, max);
	while (result == except)
	{
		result = RandomI(min, max);
	}
	return result;
}

Variant RandomWrapper(const std::vector<Variant>& params)
{
	if (params[0].GetType() == Variant::Type::Float)
	{
		return Variant(RandomF<float>(params[0].To<float>(), params[1].To<float>()));
	}
	else
	{
		return Variant(RandomI<int>(params[0].To<int>(), params[1].To<int>()));
	}
}

Variant RandomExceptWrapper(const std::vector<Variant>& params)
{
	if (params[0].GetType() == Variant::Type::Float)
	{
		return Variant(RandomExceptF<float>(params[0].To<float>(), params[1].To<float>(), params[2].To<float>()));
	}
	else
	{
		return Variant(RandomExceptI<int>(params[0].To<int>(), params[1].To<int>(), params[2].To<int>()));
	}
}

Variant Random0N(const std::vector<Variant>& params)
{
	return Variant(RandomI<int>(0, params[0].To<int>()));
}

Variant Random1N(const std::vector<Variant>& params)
{
	return Variant(RandomI<int>(1, params[0].To<int>()));
}

Variant Random01(const std::vector<Variant>& params)
{
	return Variant(RandomF(0.0f, 1.0f));
}

Variant Max(const std::vector<Variant>& params)
{
	Variant result = params[0];
	for (const Variant& v : params)
	{
		if (result > v) result = v;
	}
	return result;
}

Variant Min(const std::vector<Variant>& params)
{
	Variant result = params[0];
	for (const Variant& v : params)
	{
		if (result < v) result = v;
	}
	return result;
}

Variant Mod(const std::vector<Variant>& params)
{
	const Variant& lhs = params[0];
	const Variant& rhs = params[1];
	return Variant(lhs.As<int>() % rhs.As<int>());
}

Variant Ceil(const std::vector<Variant>& params)
{
	return Variant(ceil(params[0].To<float>()));
}

Variant Floor(const std::vector<Variant>& params)
{
	return Variant(floorf(params[0].To<float>()));
}

Variant FloorToInt(const std::vector<Variant>& params)
{
	return Variant((int)(floorf(params[0].To<float>()) + 0.01f));
}

Variant RandomIn(const std::vector<Variant>& params)
{
	int index = RandomI<int>(0, static_cast<int>(params.size() - 1));
	return Variant(params[index]);
}

void Quit(const std::vector<Variant>& params)
{
	Context* pContext = Context::GetCurrent();
	if (pContext != nullptr)
	{
		pContext->Conversation->GetValueByName("quit").Value = true;
	}
}

std::stack<Context*> Context::mCurrentContext;

Context::Context()
{
	Global = new Save();
	Player = new Save();

	FunctionAdapter* pFuncQuit = FunctionAdapter::Create(Quit);
	Register("quit", pFuncQuit);

	FunctionAdapter* pFuncRandom = FunctionAdapter::CreateFixed(RandomWrapper, Variant::Type::Float, Variant::Type::Float, 2);
	Register("random", pFuncRandom);

	FunctionAdapter* pFuncRandomExcept = FunctionAdapter::CreateFixed(RandomExceptWrapper, Variant::Type::Float, Variant::Type::Float, 3);
	Register("random_except", pFuncRandomExcept);

	FunctionAdapter* pFuncRandom0N = FunctionAdapter::CreateFixed(Random0N, Variant::Type::Float, Variant::Type::Float, 1);
	Register("random", pFuncRandom0N);
	Register("random0", pFuncRandom0N);
	Register("random0n", pFuncRandom0N);

	FunctionAdapter* pFuncRandom1N = FunctionAdapter::CreateFixed(Random1N, Variant::Type::Float, Variant::Type::Float, 1);
	Register("random1", pFuncRandom1N);
	Register("random1n", pFuncRandom0N);

	FunctionAdapter* pFuncRandom01 = FunctionAdapter::CreateFixed(Random01, Variant::Type::Float, Variant::Type::None, 0);
	Register("random01", pFuncRandom01);

	FunctionAdapter* pFuncRandon = FunctionAdapter::CreateMoreEqual(RandomIn, Variant::Type::Float, Variant::Type::Float, 2);
	Register("random_in", pFuncRandon);

	FunctionAdapter* pFuncMin = FunctionAdapter::CreateMoreEqual(Min, Variant::Type::Float, Variant::Type::Float, 2);
	Register("min", pFuncMin);

	FunctionAdapter* pFuncMax = FunctionAdapter::CreateMoreEqual(Max, Variant::Type::Int, Variant::Type::Int, 2);
	Register("max", pFuncMax);

	FunctionAdapter* pFuncMod = FunctionAdapter::CreateFixed(Mod, Variant::Type::Int, Variant::Type::Int, 2);
	Register("mod", pFuncMod);

	FunctionAdapter* pFuncCeil = FunctionAdapter::CreateFixed(Ceil, Variant::Type::Float, Variant::Type::Float, 1);
	Register("ceil", pFuncCeil);

	FunctionAdapter* pFuncFloor = FunctionAdapter::CreateFixed(Floor, Variant::Type::Float, Variant::Type::Float, 1);
	Register("floor", pFuncFloor);

	FunctionAdapter* pFuncFloorToInt = FunctionAdapter::CreateFixed(FloorToInt, Variant::Type::Int, Variant::Type::Float, 1);
	Register("floor", pFuncFloorToInt);
	Register("floori", pFuncFloorToInt);
	Register("floor2int", pFuncFloorToInt);
}

Context::~Context()
{
	delete Global;
	delete Player;

	for (auto& list : mProcessTable)
	{
		for (FunctionAdapter* pFunction : list.second)
		{
			mRemovedFunctions.insert(pFunction);
		}
	}

	for (auto& list : mFunctionTable)
	{
		for (FunctionAdapter* pFunction : list.second)
		{
			mRemovedFunctions.insert(pFunction);
		}
	}

	for (FunctionAdapter* pAdapter : mRemovedFunctions)
	{
		delete pAdapter;
	}
}

bool CheckCondition(Context& context, Expression* pExpression)
{
	if (pExpression == nullptr)
		return true;

	if (!pExpression->CanBeEvaluated(context))
		return true;

	Variant result(false);
	if (pExpression->Evaluate(context, result))
	{
		return result.To<bool>();
	}
	else
	{
		return false;
	}
}

void Context::Register(const std::string& aliasName, FunctionAdapter* pAdapter)
{
	if (pAdapter != nullptr && aliasName != "")
	{
		FunctionTable& table = pAdapter->IsProcess() ? mProcessTable : mFunctionTable;
		FunctionList& list = table[aliasName];
		auto ret = list.insert(pAdapter);
		if (!ret.second)
		{
			mRemovedFunctions.insert(*ret.first);
			list.erase(ret.first);
			list.insert(pAdapter);
		}
	}
}

void Context::Remove(const std::string &aliasName, FunctionAdapter* pAdapter)
{
	if (pAdapter != nullptr)
	{
		FunctionTable& table = pAdapter->IsProcess() ? mProcessTable : mFunctionTable;

		FunctionList& list = table[aliasName];
		auto ret = list.find(pAdapter);
		if (ret != list.end())
		{
			mRemovedFunctions.insert(pAdapter);
			list.erase(ret);
		}
	}
}

void Context::RemoveAll(FunctionAdapter* pAdapter)
{
	if (pAdapter != nullptr)
	{
		FunctionTable& table = pAdapter->IsProcess() ? mProcessTable : mFunctionTable;

		for (auto& pair : table)
		{
			FunctionList& list = pair.second;
			auto ret = list.find(pAdapter);
			if (ret != list.end())
			{
				list.erase(ret);
			}
		}
		mRemovedFunctions.erase(pAdapter);
	}
}

bool Context::CallProcess(const std::string & funcName)
{
	FunctionStack stack(Variant::Type::None);
	return Invoke(funcName, stack,
		mExternalProcessHandler,
		mProcessTable);
}

bool Context::CallProcess(const std::string & funcName, std::vector<Variant>& params)
{
	if (params.size() == 0)
	{
		return CallProcess(funcName);
	}
	else
	{
		FunctionStack stack(Variant::Type::None, params);
		return Invoke(funcName, stack,
			mExternalProcessHandler,
			mProcessTable);

	}
}

bool Context::CallFunction(const std::string & funcName, Variant & result)
{
	FunctionStack stack(result);
	return Invoke(funcName, stack,
		mExternalFunctionHandler,
		mFunctionTable);
}

bool Context::CallFunction(const std::string & funcName, Variant & result, std::vector<Variant>& params)
{
	if (params.size() == 0)
	{
		return CallFunction(funcName, result);
	}
	else
	{
		FunctionStack stack(result, params);
		return Invoke(funcName, stack,
			mExternalFunctionHandler,
			mFunctionTable);
	}
}

bool Context::HaveProcess(const std::string & funcName)
{
	return HaveExternalProcess(funcName) ||
		HaveLocalProcess(funcName);
}

bool Context::HaveFunction(const std::string & funcName)
{
	return HaveExternalFunction(funcName) ||
		HaveLocalFunction(funcName);
}

bool Context::HaveExternalProcess(const std::string & funcName)
{
	if (mExternalProcessQuery != nullptr)
	{
		int exist = mExternalProcessQuery(funcName.c_str());
		return 0 != exist;
	}
	else
	{
		return false;
	}
}

bool Context::HaveLocalProcess(const std::string & funcName)
{
	return mProcessTable.count(funcName) > 0 &&
		mProcessTable[funcName].size() > 0;
}

bool Context::HaveExternalFunction(const std::string & funcName)
{
	if (mExternalFunctionQuery != nullptr)
	{
		int exist = mExternalFunctionQuery(funcName.c_str());

		return 0 != exist;
	}
	else
	{
		return false;
	}
}

bool Context::HaveLocalFunction(const std::string & funcName)
{
	return mFunctionTable.count(funcName) > 0 &&
		mFunctionTable[funcName].size() > 0;
}

bool Context::Invoke(const std::string& name, FunctionStack& stack, FunctionHandler handler, FunctionTable& table)
{
	bool successed = false;
	if (handler != nullptr)
	{
		successed = (0 != handler(name.c_str(), &stack));
	}

	if (!successed && table.count(name) > 0)
	{
		FunctionList& list = table[name];
		if (list.size() > 0)
		{
			FunctionAdapter* suitable = nullptr;
			for (FunctionAdapter* function : list)
			{
				if (function->IsSuitable(stack))
				{
					suitable = function;
					break;
				}
			}

			if (suitable != nullptr)
			{
				if (!stack.IsSameParamType())
				{
					stack.FormatSameParamType();
				}
				(*suitable)(stack);
				return true;
			}
		}
	}
	return false;
}
