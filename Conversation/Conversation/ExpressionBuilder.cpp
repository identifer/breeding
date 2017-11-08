#include <stack>
#include <algorithm>
#include <cctype>
#include "cxx.h"
#include "ExpressionBuilder.h"

std::string TrimWhitespace(const std::string& expression)
{
	std::string newString = expression;
	auto newEnd = std::remove_if(newString.begin(), newString.end(),
		[](char c) -> bool { return isspace(c) != 0; });

	newString.erase(newEnd, newString.end());
	return newString;
}

bool IsValidVariableName(const std::string& variableName)
{
	if (!std::isalpha(variableName[0]) && variableName[0] != '_')
		return false;

	for (char c : variableName)
	{
		if (!std::isalpha(c) && !std::isdigit(c) && c != '_')
			return false;
	}
	return true;
}

bool IsOperand(ExpressionBuilder::TToken type)
{
	return type == ExpressionBuilder::TToken::VALUE ||
		type == ExpressionBuilder::TToken::VARIABLE ||
		type == ExpressionBuilder::TToken::FUNCTION;
}
bool IsLValidToken(ExpressionBuilder::TToken t) { return t == ExpressionBuilder::TToken::RBRACKET || IsOperand(t); };

bool IsRValidToken(ExpressionBuilder::TToken t) { return t == ExpressionBuilder::TToken::LBRACKET || IsOperand(t); };

bool ExpressionBuilder::IsNToken(Token& t)
{
	return t.mType == TToken::OPERATOR && (
		t.mExtra == TOperator::SUB ||
		t.mExtra == TOperator::NEG ||
		t.mExtra == TOperator::NOT);
}

bool OperatorPrior(ExpressionBuilder::TOperator lhs, ExpressionBuilder::TOperator rhs)
{
	if (lhs == ExpressionBuilder::NOT || lhs == ExpressionBuilder::NEG)
	{
		return rhs >= ExpressionBuilder::MUL;
	}
	return (lhs == ExpressionBuilder::MUL) ? lhs < rhs : false;
}

struct OperandParser
{
	OperandParser(const std::string& expr);

	operator bool() const { return mType != Invalid; }

	bool IsValue() const { return mType == Value; }

	enum Type { Value, Variable, Invalid } mType = Invalid;

	Variant mValue;
	std::string mTarget;
	std::string mVariable;
};

ExpressionBuilder::ExpressionBuilder(const std::string& expression)
{
#define EnsureStep(func) if(!(func)) { return; }
	//���ַ���ת����ΪTokens
	//1 ���ַ���������пո�ȥ��
	//2 �޲���ȫ��ת�������ж� �����Ƿ���ԣ�and,or,operator�Ҳ಻���������
	//3 ��[EXP][LB]...[RB]ת����[FUNC]���ݹ�ת������
	//4 ��[EXP]�ֽ��[VALUE]����[VARIABLE]
	EnsureStep(BuildTokenList(expression));

	//��������Ƿ�����
	//1 �Ը����н��м��
	//  �ж�operator, comparison, and, or ���Ҳ������Ƿ����
	//  �ж�assignment����Ƿ���[VARIABLE]���Ҳ��Ƿ����ֵ
	//  �ж�and, or ����TOKEN�Ƿ����
	//  ��[minus]����ת��[neg]
	//2 ����ͬ���㷨������к����������������������и�ֵ����
	EnsureStep(ValidateTokenList());

	//����׺ʽ����˳��ת���ɺ�׺ʽ˳��
	//����ȥ��[LB][RB]�Ա��ʽ��Ӱ��
	EnsureStep(ConverToRPNList());

	//���ݺ�׺ʽ�������νṹ
	//  �ϲ�������ֵ��[VALUE][OP][VALUE]�ڵ�
	//  �ϲ�������ֵ��[EXP][AND/OR][EXP]�ڵ�
	//  �ϲ����е�[AND/OR]�ڵ�
	EnsureStep(BuildExpressionTree());
#undef EnsureStep
}

ExpressionBuilder::~ExpressionBuilder()
{
	cxx::SafeRelease(mTree);
}

bool ExpressionBuilder::BuildTokenList(const std::string& expression)
{
	std::string clearExpression = TrimWhitespace(expression);

	if (clearExpression.length() == 0)
	{
		mErrorStep = Step::TrimExpressionString;
		mErrorReason = Reason::NullExpression;
		return false;
	}
	else if (!SerializeTokens(clearExpression))
	{
		mErrorStep = Step::SerializeTokens;
		return false;
	}
	else if (!ConvertFunctionTokens(mTokenList))
	{
		mErrorStep = Step::ConvertFunctionTokens;
		return false;
	}
	else
	{
		if (!ConvertExpressionTokens(mTokenList))
		{
			mErrorStep = Step::ConvertExpressionTokens;
			return false;
		}

		for (Function& function : mFunctionTable)
		{
			for (Function::Param& param : function.mParams)
			{
				if (!ConvertExpressionTokens(param))
				{
					mErrorStep = Step::ConvertExpressionTokens;
					return false;
				}
			}
		}

		return true;
	}
}

/*
ֻ���дʷ�ת���������﷨������м��
�����������ԣ��Ͳ������Ҳ�������Ƿ�������
*/
bool ExpressionBuilder::SerializeTokens(const std::string& expression)
{
	auto itCur = expression.begin();
	auto itEnd = expression.end();
	auto itExpressionBeg = itCur;
	auto AddStringToken = [&] {
		if (itExpressionBeg < itCur)
		{
			unsigned int index = static_cast<unsigned int>(mStringTable.size());
			mTokenList.push_back(Token(VARIABLE, index));
			mStringTable.push_back(std::string(itExpressionBeg, itCur));
			return true;
		}
		else
		{
			return false;
		}
	};
	unsigned int bracketIndent = 0;

	for (auto itNext = itCur + 1; itCur != itEnd; ++itCur)
	{
		itNext = itCur + 1;
		switch (*itCur)
		{
		case '(':
			if (itNext == itEnd)
			{
				mErrorReason = Reason::BracketNotMatch;
				return false;
			}
			else
			{
				AddStringToken();
				mTokenList.push_back(Token(LBRACKET, ++bracketIndent));
				itExpressionBeg = itNext;
			}
			break;
		case ')':
			if (bracketIndent == 0)
			{
				mErrorReason = Reason::BracketNotMatch;
				return false;
			}
			else
			{
				AddStringToken();
				mTokenList.push_back(Token(RBRACKET, bracketIndent--));
				itExpressionBeg = itNext;
			}
			break;
		case '&':
			if (itNext == itEnd || *itNext != '&')
			{
				mErrorReason = Reason::UnrecognizedSymbol;
				return false;
			}
			else if (++itNext == itEnd)
			{
				mErrorReason = Reason::UnexpectedEnd;
				return false;
			}
			else
			{
				AddStringToken();
				mTokenList.push_back(Token(AND));
				itExpressionBeg = itNext;
				++itCur;
			}
			break;
		case '|':
			if (itNext == itEnd || *itNext != '|')
			{
				mErrorReason = Reason::UnrecognizedSymbol;
				return false;
			}
			else if (++itNext == itEnd)
			{
				mErrorReason = Reason::UnexpectedEnd;
				return false;
			}
			else
			{
				AddStringToken();
				mTokenList.push_back(Token(OR));
				itExpressionBeg = itNext;
				++itCur;
			}
			break;
		case ',':
			if (bracketIndent == 0)
			{
				mErrorReason = Reason::MisplacedComma;
				return false;
			}
			else if (itNext == itEnd)
			{
				mErrorReason = Reason::UnexpectedEnd;
				return false;
			}
			else
			{
				AddStringToken();
				mTokenList.push_back(Token(COMMA, bracketIndent));
				itExpressionBeg = itNext;
			}
			break;
		case '>':
			if (itNext == itEnd)
			{
				mErrorReason = Reason::UnexpectedEnd;
				return false;
			}
			else if (*itNext == '=')
			{
				if (++itNext == itEnd)
				{
					mErrorReason = Reason::UnexpectedEnd;
					return false;
				}
				else
				{
					AddStringToken();
					mTokenList.push_back(Token(COMPARISON, GEQUAL));
					itExpressionBeg = itNext;
					++itCur;
				}
			}
			else
			{
				AddStringToken();
				mTokenList.push_back(Token(COMPARISON, GREATER));
				itExpressionBeg = itNext;
			}
			break;
		case '<':
			if (itNext == itEnd)
			{
				mErrorReason = Reason::UnexpectedEnd;
				return false;
			}
			else if (*itNext == '=')
			{
				if (++itNext == itEnd)
				{
					mErrorReason = Reason::UnexpectedEnd;
					return false;
				}
				else
				{
					AddStringToken();
					mTokenList.push_back(Token(COMPARISON, LEQUAL));
					itExpressionBeg = itNext;
					++itCur;
				}
			}
			else
			{
				AddStringToken();
				mTokenList.push_back(Token(COMPARISON, LESS));
				itExpressionBeg = itNext;
			}
			break;
		case '!':
			if (itNext == itEnd)
			{
				mErrorReason = Reason::UnexpectedEnd;
				return false;
			}
			else if (*itNext == '=')
			{
				if (++itNext == itEnd)
				{
					mErrorReason = Reason::UnexpectedEnd;
					return false;
				}
				else
				{
					AddStringToken();
					mTokenList.push_back(Token(COMPARISON, NEQUAL));
					itExpressionBeg = itNext;
					++itCur;
				}
			}
			else
			{
				AddStringToken();
				mTokenList.push_back(Token(OPERATOR, NOT));
				itExpressionBeg = itNext;
			}
			break;
		case '+':
			if (itNext == itEnd)
			{
				mErrorReason = Reason::UnexpectedEnd;
				return false;
			}
			else if (*itNext == '=')
			{
				if (++itNext == itEnd)
				{
					mErrorReason = Reason::UnexpectedEnd;
					return false;
				}
				else
				{
					AddStringToken();
					mTokenList.push_back(Token(ACTION, ADDSET));
					itExpressionBeg = itNext;
					++itCur;
				}
			}
			else
			{
				AddStringToken();
				mTokenList.push_back(Token(OPERATOR, ADD));
				itExpressionBeg = itNext;
			}
			break;
		case '-':
			if (itNext == itEnd)
			{
				mErrorReason = Reason::UnexpectedEnd;
				return false;
			}
			else if (*itNext == '=')
			{
				if (++itNext == itEnd)
				{
					mErrorReason = Reason::UnexpectedEnd;
					return false;
				}
				else
				{
					AddStringToken();
					mTokenList.push_back(Token(ACTION, SUBSET));
					itExpressionBeg = itNext;
					++itCur;
				}
			}
			else
			{
				AddStringToken();
				mTokenList.push_back(Token(OPERATOR, SUB));
				itExpressionBeg = itNext;
			}
			break;
		case '*':
			if (itNext == itEnd)
			{
				mErrorReason = Reason::UnexpectedEnd;
				return false;
			}
			else if (*itNext == '=')
			{
				if (++itNext == itEnd)
				{
					mErrorReason = Reason::UnexpectedEnd;
					return false;
				}
				else
				{
					AddStringToken();
					mTokenList.push_back(Token(ACTION, MULSET));
					itExpressionBeg = itNext;
					++itCur;
				}
			}
			else
			{
				AddStringToken();
				mTokenList.push_back(Token(OPERATOR, MUL));
				itExpressionBeg = itNext;
			}
			break;
		case '=':
			if (itNext == itEnd)
			{
				mErrorReason = Reason::UnexpectedEnd;
				return false;
			}
			else if (*itNext == '=')
			{
				if (++itNext == itEnd)
				{
					mErrorReason = Reason::UnexpectedEnd;
					return false;
				}
				else
				{
					AddStringToken();
					mTokenList.push_back(Token(COMPARISON, EQUAL));
					itExpressionBeg = itNext;
					++itCur;
				}
			}
			else
			{
				AddStringToken();
				mTokenList.push_back(Token(ACTION, SET));
				itExpressionBeg = itNext;
			}
			break;
		}//switch
	}//for
	 //for the last expression.
	AddStringToken();

	if (bracketIndent != 0)
	{
		mErrorReason = Reason::BracketNotMatch;
		return false;
	}
	else if (mTokenList.size() == 0)
	{
		mErrorReason = Reason::NullExpression;
		return false;
	}
	else
	{
		return true;
	}
}

/*
��Ҫ�ǰ� [EXP][LB]...[RB] ת����[FUNC]Token
ͨ���ݹ鷽ʽ�����е� FunctionParam Ҳ�����ת��
*/
bool ExpressionBuilder::ConvertFunctionTokens(std::vector<Token>& inoutTokens)
{
	typedef Function::Param Param;

	if (inoutTokens.size() < 3)
	{
		return true;
	}

	std::vector<Token> outTokens;
	for (size_t curr = 0, next = 1, end = inoutTokens.size();
		curr < end; ++curr, next = curr + 1)
	{
		if (curr > end - 3)// at least = 3, func()=>[EXP][LB][RB],
		{
			outTokens.push_back(inoutTokens.at(curr));
			continue;
		}

		Token& cToken = inoutTokens.at(curr);
		Token& nToken = inoutTokens.at(next);
		bool functionPredication = cToken.mType == VARIABLE && nToken.mType == LBRACKET;
		if (!functionPredication)
		{
			outTokens.push_back(cToken);
			continue;
		}

		size_t rb = next + 1;
		std::vector<Param> paramList(1);
		for (; rb < end; rb++)
		{
			Token& token = inoutTokens.at(rb);
			bool sameIndent = nToken.mExtra == token.mExtra;
			if (token.mType == RBRACKET && sameIndent)
			{
				break;
			}
			else if (token.mType == COMMA && sameIndent)
			{
				paramList.push_back(Param());
			}
			else
			{
				paramList.back().push_back(token);
			}
		}

		if (rb == end)
		{
			mErrorReason = Reason::UnexpectedEnd;
			return false;
		}
		else
		{
			curr = rb;

			for (Param& param : paramList)
			{
				if (!ConvertFunctionTokens(param))
				{
					mErrorReason = Reason::InvalidFunctionParam;
					return false;
				}
			}

			unsigned int funcIndex = static_cast<unsigned int>(mFunctionTable.size());
			std::string& funcName = mStringTable[cToken.mExtra];
			if (IsValidVariableName(funcName))
			{
				mFunctionTable.push_back(Function(cToken.mExtra));
				mFunctionTable.back().mParams.swap(paramList);
				outTokens.push_back(Token(FUNCTION, funcIndex));
			}
			else
			{
				mErrorReason = Reason::InvalidFunctionName;
				return false;
			}
		}
	}

	inoutTokens.swap(outTokens);
	return true;
}

/*
����ConvertFunctionTokens()֮��ʣ�µ�[EXPӦ�ö��� Variable��Value��
���ǰ���ͷ��ֵ������������Variant�� �Է����ߵļ�����ж�
*/
bool ExpressionBuilder::ConvertExpressionTokens(std::vector<Token>& inoutTokens)
{
	for (Token& token : inoutTokens)
	{
		if (token.mType == VARIABLE)
		{
			std::string& expr = mStringTable[token.mExtra];
			OperandParser parser(expr);
			if (parser)
			{
				if (parser.IsValue())
				{
					token.mType = VALUE;
					token.mExtra = static_cast<unsigned int>(mValueTable.size());
					mValueTable.push_back(parser.mValue);
				}
				else
				{
					token.mExtra = static_cast<unsigned int>(mVariableTable.size());
					mVariableTable.push_back(Variable(parser.mTarget, parser.mVariable));
				}
			}
			else
			{
				mErrorReason = Reason::InvalidOperand;
				return false;
			}
		}
	}
	return true;
}

bool ExpressionBuilder::ValidateTokenList()
{
	if (!ValidateTokens(mTokenList, false))
	{
		mErrorStep = Step::ValidateTokenSequance;
		return false;
	}

	for (Function& function : mFunctionTable)
	{
		for (Function::Param& param : function.mParams)
		{
			if (param.size() > 0 && !ValidateTokens(param, true))
			{
				mErrorStep = Step::ValidateTokenSequance;
				mErrorReason = Reason::InvalidFunctionParam;
				return false;
			}
		}
	}

	return true;
}

bool ExpressionBuilder::ValidateTokens(std::vector<Token>& inTokenList, bool isParamList)
{
	if (inTokenList.size() == 1)
	{
		if (IsOperand(inTokenList.at(0).mType))
		{
			return true;
		}
		else
		{
			mErrorReason = Reason::InvalidExpression;
			return false;
		}
	}
	else if (inTokenList.size() == 2)
	{
		Token& ftoken = inTokenList.at(0);
		Token& stoken = inTokenList.at(1);
		TToken ftype = ftoken.mType;
		TToken stype = stoken.mType;

		if (ftype == OPERATOR && IsOperand(stype))
		{
			if (ftoken.mExtra == SUB)
			{
				ftoken.mExtra = NEG;
				return true;
			}
			else if (ftoken.mExtra == NOT)
			{
				return true;
			}
			else
			{
				mErrorReason = Reason::InvalidOperator;
				return false;
			}
		}
		else
		{
			mErrorReason = Reason::InvalidExpression;
			return false;
		}
	}
	else
	{
		auto itCur = inTokenList.begin();
		auto itNext = itCur + 1;
		auto itEnd = inTokenList.end() - 1;

		if (itCur->mType == OPERATOR && itCur->mExtra == SUB)
		{
			if (IsRValidToken(itNext->mType))
			{
				itCur->mExtra = NEG;
				++itCur;
			}
			else
			{
				mErrorReason = Reason::InvalidOperator;
				return false;
			}
		}
		else
		{
			++itCur;
		}

		bool action = false;
		for (auto itPrev = itCur - 1, itNext = itCur + 1;
			itCur != itEnd; ++itCur, ++itNext, ++itPrev)
		{
			TToken pType = itPrev->mType;
			TToken cType = itCur->mType;
			TToken nType = itNext->mType;

			if (cType == OPERATOR)
			{
				if (!IsRValidToken(nType))
				{
					if (!IsNToken(*itNext))
					{
						mErrorReason = Reason::InvalidOperator;
						return false;
					}
				}
				else if (!IsLValidToken(pType))
				{
					if (itCur->mExtra == SUB)
					{
						if (itPrev->mExtra != NEG)
						{
							itCur->mExtra = NEG;
						}
						else
						{
							mErrorReason = Reason::InvalidOperator;
							return false;
						}
					}
					else if (itCur->mExtra != NOT)
					{
						mErrorReason = Reason::InvalidOperator;
						return false;
					}
				}
			}
			else if (cType == COMPARISON)
			{
				if (!IsLValidToken(pType) ||
					!IsRValidToken(nType) && !IsNToken(*itNext))
				{
					mErrorReason = Reason::InvalidComparison;
					return false;
				}
			}
			else if (cType == ACTION)
			{
				if (isParamList)
				{
					//�ⲿ������ԭ��
					return false;
				}
				else if (action || pType != VARIABLE ||
					!IsRValidToken(nType) && !IsNToken(*itNext))
				{
					mErrorReason = Reason::InvalidAction;
					return false;
				}
				else
				{
					action = true;
				}
			}
			else if (cType == AND || cType == OR)
			{
				if (!IsLValidToken(pType) ||
					!IsRValidToken(nType) && !IsNToken(*itNext))
				{
					mErrorReason = Reason::InvalidOperator;
					return false;
				}
			}
		}

		return true;
	}
}

bool ExpressionBuilder::ConverToRPNList()
{
	if (mTokenList.size() > 1 && !ConvertToRPN(mTokenList))
	{
		mErrorStep = Step::ConvertRPNNList;
		return false;
	}

	for (Function& function : mFunctionTable)
	{
		for (Function::Param& param : function.mParams)
		{
			if (param.size() > 1 && !ConvertToRPN(param))
			{
				mErrorStep = Step::ConvertRPNNList;
				mErrorReason = Reason::InvalidFunctionParam;
				return false;
			}
		}
	}
	return true;
}

bool ExpressionBuilder::ConvertToRPN(std::vector<Token>& inoutTokens)
{
	std::stack<Token> opTokenStack;
	std::vector<Token> outRPNTokens;

	auto itCur = inoutTokens.begin();
	auto itEnd = inoutTokens.end();
	for (; itCur != itEnd; ++itCur)
	{
		switch (itCur->mType)
		{
		case FUNCTION:
		case VARIABLE:
		case VALUE:
			outRPNTokens.push_back(*itCur);
			break;
		case LBRACKET:
			opTokenStack.push(*itCur);
			break;
		case RBRACKET:
			while (opTokenStack.size() > 0 && opTokenStack.top().mType != LBRACKET)
			{
				outRPNTokens.push_back(opTokenStack.top());
				opTokenStack.pop();
			}
			//������ƥ��ģ�ջ��һ����������
			opTokenStack.pop();
			break;
		case COMPARISON:
			while (opTokenStack.size() > 0 && opTokenStack.top().mType == OPERATOR)
			{
				outRPNTokens.push_back(opTokenStack.top());
				opTokenStack.pop();
			}
			opTokenStack.push(*itCur);
			break;
		case OPERATOR:
			while (opTokenStack.size() > 0 &&
				opTokenStack.top().mType == OPERATOR &&
				OperatorPrior((TOperator)opTokenStack.top().mExtra, (TOperator)itCur->mExtra))
			{
				outRPNTokens.push_back(opTokenStack.top());
				opTokenStack.pop();
			}
			opTokenStack.push(*itCur);
			break;
		case ACTION:
			if (opTokenStack.size() > 0)
			{
				TToken lastType = opTokenStack.top().mType;
				if (lastType == OPERATOR || lastType == COMPARISON)
				{
					mErrorReason = Reason::InvalidAction;
					return false;
				}
			}
			opTokenStack.push(*itCur);
			break;
		case OR:
			while (opTokenStack.size() > 0 && (
				opTokenStack.top().mType == AND ||
				opTokenStack.top().mType == COMPARISON ||
				opTokenStack.top().mType == ACTION ||
				opTokenStack.top().mType == OPERATOR))
			{
				outRPNTokens.push_back(opTokenStack.top());
				opTokenStack.pop();
			}
			opTokenStack.push(*itCur);
			break;
		case AND:
			while (opTokenStack.size() > 0 && (
				opTokenStack.top().mType == COMPARISON ||
				opTokenStack.top().mType == ACTION ||
				opTokenStack.top().mType == OPERATOR))
			{
				outRPNTokens.push_back(opTokenStack.top());
				opTokenStack.pop();
			}
			opTokenStack.push(*itCur);
			break;
		}
	}

	//�������ŵ�һ�������
	//�Ѿ������һ�������ŵĴ�������pop�����ˡ�
	while (opTokenStack.size() > 0)
	{
		outRPNTokens.push_back(opTokenStack.top());
		opTokenStack.pop();
	}

	inoutTokens.swap(outRPNTokens);
	return true;
}

bool ExpressionBuilder::BuildExpressionTree()
{
	mTree = BuildTree(mTokenList, false);
	if (mTree == nullptr)
	{
		mErrorStep = Step::ConvertRPNNList;
		return false;
	}
	else
	{
		return true;
	}
}

void operator+=(ExpressionNode::Children& lhs, ExpressionNode::Children& rhs)
{
	for (ExpressionNode* child : rhs)
	{
		lhs.push_back(child);
	}
	rhs.clear();
}

ExpressionNode* ExpressionBuilder::MakeOrNode(ExpressionNode* lhs, ExpressionNode* rhs)
{
	if (lhs->mType == OR && rhs->mType == OR)
	{
		lhs->mChildren += rhs->mChildren;
		rhs->Release();
		return lhs;
	}
	else if (lhs->mType == VALUE && rhs->mType == VALUE)
	{
		Variant& lop = mValueTable[lhs->mExtra];
		Variant& rop = mValueTable[rhs->mExtra];

		lop = lop.To<bool>() || rop.To<bool>();
		rhs->Release();
		return lhs;
	}
	else if (lhs->mType == OR)
	{
		if (rhs->mType == VALUE)
		{
			Variant& rop = mValueTable[rhs->mExtra];
			if (rop.To<bool>())
			{
				rop = true;
				lhs->Release();
				return rhs;
			}
			else
			{
				rhs->Release();
				return lhs;
			}
		}
		else
		{
			lhs->mChildren.push_back(rhs);
			return lhs;
		}
	}
	else if (rhs->mType == OR)
	{
		if (lhs->mType == VALUE)
		{
			Variant& lop = mValueTable[lhs->mExtra];
			if (lop.To<bool>())
			{
				lop = true;
				rhs->Release();
				return lhs;
			}
			else
			{
				lhs->Release();
				return rhs;
			}
		}
		else
		{
			rhs->mChildren.insert(rhs->mChildren.begin(), lhs);
			return rhs;
		}
	}
	else
	{
		ExpressionNode* parent = ExpressionBuilderHelper::GenerateNode(OR, 0);
		parent->mChildren.push_back(lhs);
		parent->mChildren.push_back(rhs);
		return parent;
	}
}

ExpressionNode* ExpressionBuilder::MakeAndNode(ExpressionNode* lhs, ExpressionNode* rhs)
{
	if (lhs->mType == AND && rhs->mType == AND)
	{
		lhs->mChildren += rhs->mChildren;
		rhs->Release();
		return lhs;
	}
	else if (lhs->mType == VALUE && rhs->mType == VALUE)
	{
		Variant& lop = mValueTable[lhs->mExtra];
		Variant& rop = mValueTable[rhs->mExtra];

		lop = lop.To<bool>() && rop.To<bool>();
		rhs->Release();
		return lhs;
	}
	else if (lhs->mType == AND)
	{
		if (rhs->mType == VALUE)
		{
			Variant& rop = mValueTable[rhs->mExtra];
			if (rop.To<bool>())
			{
				rhs->Release();
				return lhs;
			}
			else
			{
				rhs = false;
				lhs->Release();
				return rhs;
			}
		}
		else
		{
			lhs->mChildren.push_back(rhs);
			return lhs;
		}
	}
	else if (rhs->mType == AND)
	{
		if (lhs->mType == VALUE)
		{
			Variant& lop = mValueTable[lhs->mExtra];
			if (lop.To<bool>())
			{
				lhs->Release();
				return rhs;
			}
			else
			{
				lop = false;
				rhs->Release();
				return lhs;
			}
		}
		else
		{
			rhs->mChildren.insert(rhs->mChildren.begin(), lhs);
			return rhs;
		}
	}
	else
	{
		ExpressionNode* parent = ExpressionBuilderHelper::GenerateNode(AND, 0);
		parent->mChildren.push_back(lhs);
		parent->mChildren.push_back(rhs);
		return parent;
	}
}

ExpressionNode* ExpressionBuilder::MakeOperatorNode(TOperator op, ExpressionNode* rhs)
{
	if (rhs->mType == VALUE)
	{
		Variant& operand = mValueTable[rhs->mExtra];
		if (op == NOT)
		{
			operand = !operand;
		}
		else if (op == NEG)
		{
			operand = -operand;
		}
		return rhs;
	}
	else
	{
		ExpressionNode* parent = ExpressionBuilderHelper::GenerateNode(OPERATOR, op);
		parent->mChildren.push_back(rhs);
		return parent;
	}
}

ExpressionNode* ExpressionBuilder::MakeOperatorNode(TOperator op, ExpressionNode* lhs, ExpressionNode* rhs)
{
	if (lhs->mType == VALUE && rhs->mType == VALUE)
	{
		Variant& lop = mValueTable[lhs->mExtra];
		Variant& rop = mValueTable[rhs->mExtra];
		if (op == MUL)
		{
			lop *= rop;
		}
		else if (op == ADD)
		{
			lop += rop;
		}
		else if (op == SUB)
		{
			lop -= rop;
		}
		rhs->Release();
		return lhs;
	}
	else
	{
		ExpressionNode* parent = ExpressionBuilderHelper::GenerateNode(OPERATOR, op);
		parent->mChildren.push_back(lhs);
		parent->mChildren.push_back(rhs);
		return parent;
	}
}

ExpressionNode* ExpressionBuilder::BuildTree(std::vector<Token>& inTokens, bool isFunctionParam)
{
	std::stack<ExpressionNode*> expNodeStack;

	auto fallback = cxx::MakeScopeGuard([&] {
		while (expNodeStack.size() > 0)
		{
			expNodeStack.top()->Release();
			expNodeStack.pop();
		}
	});

	auto itCur = inTokens.begin();
	auto itEnd = inTokens.end();
	for (; itCur != itEnd; ++itCur)
	{
		switch (itCur->mType)
		{
		case VARIABLE:
		case VALUE:
			expNodeStack.push(ExpressionBuilderHelper::GenerateNode(itCur->mType, itCur->mExtra));
			break;
		case FUNCTION:
		{
			Function& function = mFunctionTable[itCur->mExtra];
			auto node = ExpressionBuilderHelper::GenerateNode(FUNCTION, itCur->mExtra);
			for (Function::Param& param : function.mParams)
			{
				if (param.size() == 0)
					continue;

				ExpressionNode* paramNode = BuildTree(param, true);
				if (paramNode == nullptr)
				{
					paramNode->Release();
					mErrorReason = Reason::InvalidFunctionParam;
					return nullptr;
				}
				else
				{
					node->mChildren.push_back(paramNode);
				}
			}
			expNodeStack.push(node);
			break;
		}
		case OR:
			if (expNodeStack.size() < 2)
			{
				mErrorReason = Reason::UnexpectedEnd;
				return nullptr;
			}
			else
			{
				ExpressionNode* rhs = expNodeStack.top(); expNodeStack.pop();
				ExpressionNode* lhs = expNodeStack.top(); expNodeStack.pop();

				if (lhs->mType == ACTION || rhs->mType == ACTION)
				{
					lhs->Release();
					rhs->Release();
					mErrorReason = Reason::InvalidExpression;
					return nullptr;
				}
				else
				{
					expNodeStack.push(MakeOrNode(lhs, rhs));
				}
			}
			break;
		case AND:
			if (expNodeStack.size() < 2)
			{
				mErrorReason = Reason::UnexpectedEnd;
				return nullptr;
			}
			else
			{
				ExpressionNode* rhs = expNodeStack.top();	expNodeStack.pop();
				ExpressionNode* lhs = expNodeStack.top();	expNodeStack.pop();
				if (lhs->mType == ACTION || rhs->mType == ACTION)
				{
					lhs->Release();
					rhs->Release();
					mErrorReason = Reason::InvalidExpression;
					return nullptr;
				}
				else
				{
					expNodeStack.push(MakeAndNode(lhs, rhs));
				}
			}
			break;
		case COMPARISON:
			if (expNodeStack.size() < 2)
			{
				mErrorReason = Reason::UnexpectedEnd;
				return nullptr;
			}
			else
			{
				ExpressionNode* rhs = expNodeStack.top(); expNodeStack.pop();
				ExpressionNode* lhs = expNodeStack.top(); expNodeStack.pop();

				if (lhs->mType == ACTION || rhs->mType == ACTION)
				{
					lhs->Release();
					rhs->Release();
					mErrorReason = Reason::InvalidExpression;
					return nullptr;
				}
				else
				{
					ExpressionNode* parent = ExpressionBuilderHelper::GenerateNode(COMPARISON, itCur->mExtra);
					parent->mChildren.push_back(lhs);
					parent->mChildren.push_back(rhs);
					expNodeStack.push(parent);
				}
			}
			break;
		case ACTION:
			if (expNodeStack.size() < 2)
			{
				mErrorReason = Reason::UnexpectedEnd;
				return nullptr;
			}
			else
			{
				ExpressionNode* rhs = expNodeStack.top();	expNodeStack.pop();
				ExpressionNode* lhs = expNodeStack.top();	expNodeStack.pop();

				if (lhs->mType != VARIABLE || rhs->mType == ACTION)
				{
					lhs->Release();
					rhs->Release();
					mErrorReason = Reason::InvalidExpression;
					return nullptr;
				}
				else
				{
					ExpressionNode* parent = ExpressionBuilderHelper::GenerateNode(ACTION, itCur->mExtra);
					parent->mChildren.push_back(lhs);
					parent->mChildren.push_back(rhs);
					expNodeStack.push(parent);
				}
			}
			break;
		case OPERATOR:
			if (itCur->mExtra == NOT || itCur->mExtra == NEG)
			{
				if (expNodeStack.size() < 1)
				{
					mErrorReason = Reason::UnexpectedEnd;
					return nullptr;
				}
				else if (expNodeStack.top()->mType == ACTION)
				{
					mErrorReason = Reason::InvalidExpression;
					return nullptr;
				}
				else
				{
					ExpressionNode* rhs = expNodeStack.top(); expNodeStack.pop();
					if (rhs->mType == ACTION)
					{
						rhs->Release();
						mErrorReason = Reason::InvalidExpression;
						return nullptr;
					}
					else
					{
						expNodeStack.push(MakeOperatorNode((TOperator)itCur->mExtra, rhs));
					}
				}
			}
			else
			{
				if (expNodeStack.size() < 2)
				{
					mErrorReason = Reason::UnexpectedEnd;
					return nullptr;
				}
				else
				{
					ExpressionNode* rhs = expNodeStack.top(); expNodeStack.pop();
					ExpressionNode* lhs = expNodeStack.top(); expNodeStack.pop();

					if (lhs->mType == ACTION || rhs->mType == ACTION)
					{
						lhs->Release();
						rhs->Release();
						mErrorReason = Reason::InvalidExpression;
						return nullptr;
					}
					else
					{
						expNodeStack.push(MakeOperatorNode((TOperator)itCur->mExtra, lhs, rhs));
					}
				}
			}
			break;
		}//switch
	}

	if (expNodeStack.size() == 0)
	{
		mErrorReason = Reason::NoExpressionTreeRoot;
		return nullptr;
	}
	else if (expNodeStack.size() > 1)
	{
		mErrorReason = Reason::InvalidExpression;
		return nullptr;
	}
	else
	{
		fallback.Dismiss();
		return expNodeStack.top();
	}
}

void ExpressionNode::Release()
{
	for (ExpressionNode* pNode : mChildren)
	{
		pNode->Release();
	}
	mChildren.clear();
	ExpressionBuilderHelper::Destroy(this);
}

OperandParser::OperandParser(const std::string & expression)
{
	unsigned char c = expression.at(0);
	if (expression == "true")
	{
		mType = Value;
		mValue = true;
	}
	else if (expression == "false")
	{
		mType = Value;
		mValue = false;
	}
	else
	{
		size_t length = expression.length();
		size_t dotPos = expression.find('.');
		bool dotExist = dotPos != expression.npos;
		bool dotInside = dotExist && dotPos != 0 && dotPos != length - 1;

		if (std::isalpha(c) && dotInside)
		{
			mTarget = expression.substr(0, dotPos);
			mVariable = expression.substr(dotPos + 1);
			if (IsValidVariableName(mTarget) &&
				IsValidVariableName(mVariable))
			{
				mType = Variable;
			}
		}
		else if (!dotExist)
		{
			//int
			bool allDigit = true;
			bool forceFloat = false;
			int number = 0;
			for (size_t cur = 0; cur < length; ++cur)
			{
				unsigned char c = expression.at(cur);
				if (!std::isdigit(c))
				{
					if (cur == length - 1 &&
						(c == 'f' || c == 'F'))
					{
						forceFloat = true;
					}
					else
					{
						allDigit = false;
						break;
					}
				}
				else
				{
					number = number * 10 + (c - '0');
				}
			}

			if (allDigit)
			{
				if (forceFloat)
				{
					mType = Value;
					mValue = static_cast<float>(number);
				}
				else
				{
					mType = Value;
					mValue = number;
				}
			}
		}
		else if (dotInside)
		{
			//float
			bool allDigit = true;
			int integer = 0;
			int numer = 0;
			int denom = 1;
			size_t cur = 0;
			size_t beg = 0;

			for (size_t cur = 0; cur < dotPos; ++cur)
			{
				unsigned char c = expression.at(cur);
				if (!std::isdigit(c))
				{
					allDigit = false;
					break;
				}
				else
				{
					integer = integer * 10 + (c - '0');
				}
			}

			for (size_t cur = dotPos + 1; cur < length; ++cur)
			{
				unsigned char c = expression.at(cur);
				bool fEnd = cur == length - 1 && (c == 'f' || c == 'F');
				if (!std::isdigit(c) && !fEnd)
				{
					allDigit = false;
					break;
				}
				else if (!fEnd)
				{
					numer = numer * 10 + (c - '0');
					denom *= 10;
				}
			}

			if (allDigit)
			{
				mType = Value;
				mValue = integer + (static_cast<float>(numer) * 1.0f / denom);
			}
		}
	}
}

std::vector<ExpressionNode*> ExpressionBuilderHelper::sReserveNodes;

ExpressionNode * ExpressionBuilderHelper::GenerateNode(TToken type, unsigned int extra)
{
	if (sReserveNodes.size() == 0)
	{
		return new ExpressionNode(type, extra);
	}
	else
	{
		ExpressionNode* pNode = sReserveNodes.back();
		sReserveNodes.pop_back();

		pNode->mType = type;
		pNode->mExtra = extra;

		return pNode;
	}
}

void ExpressionBuilderHelper::Destroy(ExpressionNode* pNode)
{
	sReserveNodes.push_back(pNode);
}

void ExpressionBuilderHelper::Release()
{
	for (ExpressionNode* pNode : sReserveNodes)
	{
		delete pNode;
	}
	sReserveNodes.clear();
}
