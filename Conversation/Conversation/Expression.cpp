#include <algorithm>
#include "Expression.h"
#include "ExpressionBuilder.h"
#include "Conversation.h"

struct EvaluableExpression : public Expression
{
	virtual bool CanBeExecuted(Context& context) const override { return false; }

	virtual bool Execute(Context& context) override { return false; }
};

struct AssignmentExpression : public Expression
{
	virtual bool CanBeEvaluated(Context& context) const override { return false; }

	virtual bool CanBeExecuted(Context& context) const override
	{
		SaveValue* value = mChildren[0]->GetValue(context, true);
		return value != nullptr &&
			!value->ReadOnly &&
			mChildren.at(1)->CanBeEvaluated(context);
	}

	virtual bool Evaluate(Context& context, Variant& result) override { return false; }
};

struct GEqualComparison : public EvaluableExpression
{
	virtual bool CanBeEvaluated(Context& context) const override
	{
		return mChildren.at(0)->CanBeEvaluated(context) &&
			mChildren.at(1)->CanBeEvaluated(context);
	}

	virtual bool Evaluate(Context& context, Variant& result) override
	{
		Variant l, r;
		if (!mChildren.at(0)->Evaluate(context, l) ||
			!mChildren.at(1)->Evaluate(context, r))
		{
			return false;
		}
		else
		{
			result = l >= r;
			return true;
		}
	}
};

struct LEqualComparison : public EvaluableExpression
{
	virtual bool CanBeEvaluated(Context& context) const override
	{
		return mChildren.at(0)->CanBeEvaluated(context) &&
			mChildren.at(1)->CanBeEvaluated(context);
	}

	virtual bool Evaluate(Context& context, Variant& result) override
	{
		Variant l, r;
		if (!mChildren.at(0)->Evaluate(context, l) ||
			!mChildren.at(1)->Evaluate(context, r))
		{
			return false;
		}
		else
		{
			result = l <= r;
			return true;
		}
	}
};

struct NEqualComparison : public EvaluableExpression
{
	virtual bool CanBeEvaluated(Context& context) const override
	{
		return mChildren.at(0)->CanBeEvaluated(context) &&
			mChildren.at(1)->CanBeEvaluated(context);
	}

	virtual bool Evaluate(Context& context, Variant& result) override
	{
		Variant l, r;
		if (!mChildren.at(0)->Evaluate(context, l) ||
			!mChildren.at(1)->Evaluate(context, r))
		{
			return false;
		}
		else
		{
			result = l != r;
			return true;
		}
	}
};

struct GreaterComparison : public EvaluableExpression
{
	virtual bool CanBeEvaluated(Context& context) const override
	{
		return mChildren.at(0)->CanBeEvaluated(context) &&
			mChildren.at(1)->CanBeEvaluated(context);
	}

	virtual bool Evaluate(Context& context, Variant& result) override
	{
		Variant l, r;
		if (!mChildren.at(0)->Evaluate(context, l) ||
			!mChildren.at(1)->Evaluate(context, r))
		{
			return false;
		}
		else
		{
			result = l > r;
			return true;
		}
	}
};

struct LessComparison : public EvaluableExpression
{
	virtual bool CanBeEvaluated(Context& context) const override
	{
		return mChildren.at(0)->CanBeEvaluated(context) &&
			mChildren.at(1)->CanBeEvaluated(context);
	}

	virtual bool Evaluate(Context& context, Variant& result) override
	{
		Variant l, r;
		if (!mChildren.at(0)->Evaluate(context, l) ||
			!mChildren.at(1)->Evaluate(context, r))
		{
			return false;
		}
		else
		{
			result = l < r;
			return true;
		}
	}
};

struct EqualComparison : public EvaluableExpression
{
	virtual bool CanBeEvaluated(Context& context) const override
	{
		return mChildren.at(0)->CanBeEvaluated(context) &&
			mChildren.at(1)->CanBeEvaluated(context);
	}

	virtual bool Evaluate(Context& context, Variant& result) override
	{
		Variant l, r;
		if (!mChildren.at(0)->Evaluate(context, l) ||
			!mChildren.at(1)->Evaluate(context, r))
		{
			return false;
		}
		else
		{
			result = l == r;
			return true;
		}
	}
};

struct AddOperator : public EvaluableExpression
{
	virtual bool CanBeEvaluated(Context& context) const override
	{
		return mChildren.at(0)->CanBeEvaluated(context) &&
			mChildren.at(1)->CanBeEvaluated(context);
	}

	virtual bool Evaluate(Context& context, Variant& result) override
	{
		Variant r;
		if (!mChildren.at(0)->Evaluate(context, result) ||
			!mChildren.at(1)->Evaluate(context, r))
		{
			return false;
		}
		else
		{
			result += r;
			return true;
		}
	}
};

struct SubOperator : public EvaluableExpression
{
	virtual bool CanBeEvaluated(Context& context) const override
	{
		return mChildren.at(0)->CanBeEvaluated(context) &&
			mChildren.at(1)->CanBeEvaluated(context);
	}

	virtual bool Evaluate(Context& context, Variant& result) override
	{
		Variant r;
		if (!mChildren.at(0)->Evaluate(context, result) ||
			!mChildren.at(1)->Evaluate(context, r))
		{
			return false;
		}
		else
		{
			result -= r;
			return true;
		}
	}
};

struct MultipleOperator : public EvaluableExpression
{
	virtual bool CanBeEvaluated(Context& context) const override
	{
		return mChildren.at(0)->CanBeEvaluated(context) &&
			mChildren.at(1)->CanBeEvaluated(context);
	}

	virtual bool Evaluate(Context& context, Variant& result) override
	{
		Variant r;
		if (!mChildren.at(0)->Evaluate(context, result) ||
			!mChildren.at(1)->Evaluate(context, r))
		{
			return false;
		}
		else
		{
			result *= r;
			return true;
		}
	}
};

struct NotOperator : public EvaluableExpression
{
	virtual bool CanBeEvaluated(Context& context) const override { return true; }

	virtual bool Evaluate(Context& context, Variant& result) override
	{
		Variant l;
		if (mChildren.at(0)->CanBeEvaluated(context) &&
			mChildren.at(0)->Evaluate(context, l))
		{
			result = !l;
			return true;
		}
		else
		{
			result = Variant(true);
		}
		return true;
	}
};

struct NegativeOperator : public EvaluableExpression
{
	virtual bool CanBeEvaluated(Context& context) const override
	{
		return mChildren.at(0)->CanBeEvaluated(context);
	}

	virtual bool Evaluate(Context& context, Variant& result) override
	{
		Variant l;
		if (!mChildren.at(0)->Evaluate(context, l))
		{
			return false;
		}
		else
		{
			result = -l;
			return true;
		}
	}
};

struct SetAction : public AssignmentExpression
{
	virtual bool Execute(Context& context) override
	{
		SaveValue* result = mChildren.at(0)->GetValue(context, true);
		return mChildren.at(1)->Evaluate(context, result->Value);
	}
};

struct AddSetAction : public AssignmentExpression
{
	virtual bool Execute(Context& context) override
	{
		SaveValue* result = mChildren.at(0)->GetValue(context, true);
		Variant rhs(result->Value.GetType());
		if (mChildren.at(1)->Evaluate(context, rhs))
		{
			result->Value += rhs;
			return true;
		}
		else
		{
			return false;
		}
	}
};

struct SubSetAction : public AssignmentExpression
{
	virtual bool Execute(Context& context) override
	{
		SaveValue* result = mChildren.at(0)->GetValue(context, true);
		Variant rhs(result->Value.GetType());
		if (mChildren.at(1)->Evaluate(context, rhs))
		{
			result->Value -= rhs;
			return true;
		}
		else
		{
			return false;
		}
	}
};

struct MultipleSetAction : public AssignmentExpression
{
	virtual bool Execute(Context& context) override
	{
		SaveValue* result = mChildren.at(0)->GetValue(context, true);
		Variant rhs(result->Value.GetType());
		if (mChildren.at(1)->Evaluate(context, rhs))
		{
			result->Value *= rhs;
			return true;
		}
		else
		{
			return false;
		}
	}
};

struct AndCondition : public EvaluableExpression
{
	virtual bool CanBeEvaluated(Context& context) const override
	{
		for (Expression* pChildCondition : mChildren)
		{
			if (!pChildCondition->CanBeEvaluated(context))
				return false;
		}
		return true;
	}

	virtual bool CanBeExecuted(Context& context) const override
	{
		for (Expression* pChildCondition : mChildren)
		{
			if (pChildCondition->CanBeExecuted(context))
				return true;
		}
		return false;
	}

	virtual bool Evaluate(Context& context, Variant& result) override
	{
		Variant subResult(Variant::Type::Bool);
		for (Expression* pChildCondition : mChildren)
		{
			if (!pChildCondition->CanBeEvaluated(context))
				return false;

			if (!pChildCondition->Evaluate(context, subResult))
				return false;

			if (!subResult.To<bool>())
			{
				result = false;
				return true;
			}
		}

		result = true;
		return true;
	}
};

struct OrCondition : public EvaluableExpression
{
	virtual bool CanBeEvaluated(Context& context) const override
	{
		for (Expression* pChildCondition : mChildren)
		{
			if (!pChildCondition->CanBeEvaluated(context))
				return false;
		}
		return true;
	}

	virtual bool CanBeExecuted(Context& context) const override
	{
		for (Expression* pChildCondition : mChildren)
		{
			if (pChildCondition->CanBeExecuted(context))
				return true;
		}
		return false;
	}

	virtual bool Evaluate(Context& context, Variant& result) override
	{
		Variant subResult(Variant::Type::Bool);
		for (Expression* pChildCondition : mChildren)
		{
			if (!pChildCondition->CanBeEvaluated(context))
				return false;

			if (!pChildCondition->Evaluate(context, subResult))
				return false;

			if (subResult.To<bool>())
			{
				result = true;
				return true;
			}
		}

		result = false;
		return true;
	}
};

struct ValueExpression : public EvaluableExpression
{
	virtual bool CanBeEvaluated(Context& context) const override { return true; }

	virtual bool Evaluate(Context& context, Variant& result) override
	{
		result = mValue;
		return true;
	}

	ValueExpression(Variant& value) : mValue(value) { }

	Variant mValue;
};

struct VariableExpression : public EvaluableExpression
{
	enum Scope { Global, Player, Conversation, NPC, Others };

	virtual bool CanBeEvaluated(Context& context) const override
	{
		return mTarget != Others;
	}

	virtual SaveValue* GetValue(Context& context, bool created) const override
	{
		Save* save = nullptr;

		if (mTarget == Global)
		{
			save = context.Global;
		}
		else if (mTarget == Player)
		{
			save = context.Player;
		}
		else if (mTarget == Conversation)
		{
			save = context.Conversation;
		}
		else if (mTarget == NPC)
		{
			save = context.NPC;
		}

		if (save != nullptr)
		{
			if (created || save->HasValue(mVariableName))
			{
				return &(save->GetValueByName(mVariableName));
			}
		}

		return nullptr;
	}

	virtual bool Evaluate(Context& context, Variant& result) override
	{
		SaveValue* v = GetValue(context, false);
		if (v != nullptr)
		{
			result = v->Value;
		}
		else
		{
			result = Variant();
		}
		return true;
	}

	VariableExpression(const std::string& target, const std::string& variable)
		: mVariableName(variable)
	{
		if (target == "global")
		{
			mTarget = Global;
		}
		else if (target == "player")
		{
			mTarget = Player;
		}
		else if (target == "conversation")
		{
			mTarget = Conversation;
		}
		else if (target == "npc")
		{
			mTarget = NPC;
		}
		else
		{
			mTarget = Others;
		}
	}

	Scope mTarget;
	std::string mVariableName;
};

struct FunctionExpression : public Expression
{
	virtual bool CanBeEvaluated(Context& context) const override
	{
		return context.HaveFunction(mFunctionName);
	}

	virtual bool CanBeExecuted(Context& context) const override
	{
		return context.HaveProcess(mFunctionName);
	}

	virtual bool Evaluate(Context& context, Variant& result) override
	{
		size_t paramCount = mChildren.size();

		if (paramCount == 0)
		{
			return context.CallFunction(mFunctionName, result);
		}
		else
		{
			std::vector<Variant> params(paramCount);
			for (unsigned int i = 0; i < paramCount; i++)
			{
				if (!mChildren.at(i)->CanBeEvaluated(context))
				{
					return false;
				}

				if (!mChildren.at(i)->Evaluate(context, params[i]))
				{
					return false;
				}
			}
			return context.CallFunction(mFunctionName, result, params);
		}

		return false;
	}

	virtual bool Execute(Context& context) override
	{
		size_t paramCount = mChildren.size();

		if (paramCount == 0)
		{
			return context.CallProcess(mFunctionName);
		}
		else
		{
			std::vector<Variant> params(paramCount);
			for (unsigned int i = 0; i < paramCount; i++)
			{
				if (!mChildren.at(i)->CanBeEvaluated(context))
				{
					return false;
				}

				if (!mChildren.at(i)->Evaluate(context, params[i]))
				{
					return false;
				}
			}
			return context.CallProcess(mFunctionName, params);
		}

	}

	FunctionExpression(const std::string& name) : mFunctionName(name) { }

	std::string mFunctionName;
};


Expression* Create(ExpressionNode* pNode, ExpressionBuilder& builder)
{
	typedef ExpressionBuilder::TToken TToken;
	typedef ExpressionBuilder::TComparison TComparison;
	typedef ExpressionBuilder::TOperator TOperator;
	typedef ExpressionBuilder::TAction TAction;

	Expression* pBaseNode = nullptr;
	switch (pNode->mType)
	{
	case TToken::VALUE:
		return new ValueExpression(builder.mValueTable[pNode->mExtra]);
	case TToken::VARIABLE:
	{
		ExpressionBuilder::Variable& var = builder.mVariableTable[pNode->mExtra];
		return new VariableExpression(var.mTarget, var.mVariable);
	}
	case TToken::FUNCTION:
	{
		ExpressionBuilder::Function& function = builder.mFunctionTable[pNode->mExtra];
		std::string& functionName = builder.mStringTable[function.mNameIndex];
		pBaseNode = new FunctionExpression(functionName);
		break;
	}
	case TToken::AND:
	{
		pBaseNode = new AndCondition();
		break;
	}
	case TToken::OR:
	{
		pBaseNode = new OrCondition();
		break;
	}
	case TToken::COMPARISON:
	{
		switch (pNode->mExtra)
		{
		case TComparison::GEQUAL:
			pBaseNode = new GEqualComparison();
			break;
		case TComparison::LEQUAL:
			pBaseNode = new LEqualComparison();
			break;
		case TComparison::NEQUAL:
			pBaseNode = new NEqualComparison();
			break;
		case TComparison::GREATER:
			pBaseNode = new GreaterComparison();
			break;
		case TComparison::LESS:
			pBaseNode = new LessComparison();
			break;
		case TComparison::EQUAL:
			pBaseNode = new EqualComparison();
			break;
		}
		break;
	}
	case TToken::ACTION:
	{
		switch (pNode->mExtra)
		{
		case TAction::ADDSET:
			pBaseNode = new AddSetAction();
			break;
		case TAction::SUBSET:
			pBaseNode = new SubSetAction();
			break;
		case TAction::MULSET:
			pBaseNode = new MultipleSetAction();
			break;
		case TAction::SET:
			pBaseNode = new SetAction();
			break;
		}
		break;
	}
	case TToken::OPERATOR:
	{
		switch (pNode->mExtra)
		{
		case TOperator::ADD:
			pBaseNode = new AddOperator();
			break;
		case TOperator::SUB:
			pBaseNode = new SubOperator();
			break;
		case TOperator::MUL:
			pBaseNode = new MultipleOperator();
			break;
		case TOperator::NEG:
			pBaseNode = new NegativeOperator();
			break;
		case TOperator::NOT:
			pBaseNode = new NotOperator();
			break;
		}
		break;
	}
	}

	if (pBaseNode != nullptr)
	{
		for (ExpressionNode* pChild : pNode->mChildren)
		{
			Expression* pChildNode = Create(pChild, builder);
			if (pChildNode != nullptr)
			{
				pBaseNode->mChildren.push_back(pChildNode);
			}
			else
			{
				delete pBaseNode;
				return nullptr;
			}
		}
	}

	return pBaseNode;
}

Expression* Expression::Create(const std::string & expression)
{
	ExpressionBuilder builder(expression);
	if (builder)
	{
		Expression* pExpression = ::Create(builder.mTree, builder);
		if (pExpression != nullptr)
		{
			return pExpression;
		}
	}
	return nullptr;
}

void Expression::Release()
{
	ExpressionBuilderHelper::Release();
}

Expression::~Expression()
{
	for (Expression* child : mChildren)
	{
		delete child;
	}
	mChildren.clear();
}
