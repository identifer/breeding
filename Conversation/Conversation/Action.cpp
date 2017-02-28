#include <cctype>
#include "Conversation.h"
#include "pugixml/pugixml.hpp"
#include "Expression.h"
#include "cxx.h"

bool Action::Parse(pugi::xml_node& node)
{
	std::string expression = node.attribute("do").as_string();
	mExpression = Expression::Create(expression);
	if (mExpression != nullptr)
	{
		auto nodeCondition = node.attribute("if");
		if (nodeCondition)
		{
			mCondition = Expression::Create(nodeCondition.as_string());
		}

		return true;
	}
	else
	{
		return true;
	}
}

Action* Action::Create(pugi::xml_node& node)
{
	Action* action = new Action();
	if (action->Parse(node))
	{
		return action;
	}
	else
	{
		delete action;
		return nullptr;
	}
}

Action::~Action()
{
	cxx::SafeDelete(mCondition);
	delete mExpression;
}

bool Action::IsAvailable(Context& context)
{
	return CheckCondition(context, mCondition);
}

void Action::Execute(Context& context)
{
	if (IsAvailable(context) &&
		mExpression->CanBeExecuted(context))
	{
		mExpression->Execute(context);
	}
}
