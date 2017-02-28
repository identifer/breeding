#pragma once
#include <string>
#include <vector>
#include "variant.h"

struct Context;
struct ExpressionNode;
struct ExpressionBuilder;
struct SaveValue;

struct Expression
{
	static Expression* Create(const std::string & expression);

	static void Release();

	~Expression();

	virtual SaveValue* GetValue(Context& context, bool created) const { return nullptr; }

	virtual bool CanBeEvaluated(Context& context) const = 0;

	virtual bool CanBeExecuted(Context& context) const = 0;

	virtual bool Evaluate(Context& context, Variant& result) = 0;

	virtual bool Execute(Context& context) = 0;

protected:
	friend Expression* Create(ExpressionNode* pNode, ExpressionBuilder& builder);
	std::vector<Expression*> mChildren;
};