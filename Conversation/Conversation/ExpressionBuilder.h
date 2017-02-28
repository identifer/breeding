#pragma once
#include <string>
#include <vector>
#include "variant.h"

struct ExpressionNode;

struct ExpressionBuilder
{
	enum class Step : int
	{
		TrimExpressionString,
		SerializeTokens,
		ConvertFunctionTokens,
		ConvertExpressionTokens,
		ValidateTokenSequance,
		ConvertRPNNList,
		BuildExpressionTree,
		Finish
	};

	enum class Reason : int
	{
		NoError,
		NullExpression,
		UnexpectedEnd,
		BracketNotMatch,
		UnrecognizedSymbol,
		MisplacedComma,
		InvalidVariableName,
		InvalidFunctionName,
		InvalidFunctionParam,
		InvalidOperator,
		InvalidAction,
		InvalidComparison,
		InvalidOperand,
		InvalidExpression,
		NoExpressionTreeRoot,
	};

	enum TToken { LBRACKET, RBRACKET, VALUE, VARIABLE, FUNCTION, AND, OR, COMMA, COMPARISON, ACTION, OPERATOR };

	enum TOperator { NOT, NEG, MUL, ADD, SUB };

	enum TComparison { GREATER, LESS, GEQUAL, LEQUAL, NEQUAL, EQUAL };

	enum TAction { ADDSET, SUBSET, MULSET, SET };

	ExpressionBuilder(const std::string& expression);

	~ExpressionBuilder();

	operator bool() const { return mErrorStep == Step::Finish && mErrorReason == Reason::NoError; }

	Step GetErrorStep() const { return mErrorStep; }

	Reason GetErrorReason() const { return mErrorReason; }

	ExpressionNode* mTree = nullptr;

	struct Token
	{
		Token(TToken t, unsigned int extra = 0)
			: mType(t), mExtra(extra) { }

		TToken mType;
		unsigned int mExtra;
	};

	struct Function
	{
		typedef std::vector<Token> Param;

		Function(unsigned int nameIndex)
			: mNameIndex(nameIndex) { }

		unsigned int mNameIndex;
		std::vector<Param> mParams;
	};

	struct Variable
	{
		Variable(const std::string& target, const std::string& variable)
			: mTarget(target), mVariable(variable) { }

		std::string mTarget;
		std::string mVariable;
	};

	std::vector<std::string> mStringTable;
	std::vector<Function> mFunctionTable;
	std::vector<Variant> mValueTable;
	std::vector<Variable> mVariableTable;

private:
	bool BuildTokenList(const std::string& exp);
	bool ValidateTokenList();
	bool ConverToRPNList();
	bool BuildExpressionTree();

	Step mErrorStep = Step::Finish;
	Reason mErrorReason = Reason::NoError;
	std::vector<Token> mTokenList;

	bool SerializeTokens(const std::string& statement);
	bool ConvertFunctionTokens(std::vector<Token>& list);
	bool ConvertExpressionTokens(std::vector<Token>& list);
	bool ValidateTokens(std::vector<Token>& list, bool param);
	bool ConvertToRPN(std::vector<Token>& inoutTokens);
	ExpressionNode* BuildTree(std::vector<Token>& inTokens, bool isFunctionParam);
	ExpressionNode* MakeOperatorNode(TOperator, ExpressionNode* rhs);
	ExpressionNode* MakeOperatorNode(TOperator op, ExpressionNode* lhs, ExpressionNode* rhs);
	ExpressionNode* MakeOrNode(ExpressionNode* lhs, ExpressionNode* rhs);
	ExpressionNode* MakeAndNode(ExpressionNode* lhs, ExpressionNode* rhs);
	bool IsNToken(ExpressionBuilder::Token& t);
};

struct ExpressionNode
{
	typedef ExpressionBuilder::TToken TToken;
	typedef std::vector<ExpressionNode*> Children;

	ExpressionNode(TToken type, unsigned int extra)
		: mType(type), mExtra(extra) { }

	void Release();

	TToken mType;
	unsigned int mExtra;
	Children mChildren;
};

struct ExpressionBuilderHelper
{
	typedef ExpressionBuilder::TToken TToken;

	static ExpressionNode* GenerateNode(TToken type, unsigned int extra);
	static void Destroy(ExpressionNode*);
	static void Release();

private:
	static std::vector<ExpressionNode*> sReserveNodes;
};