#include "Conversation.h"
#include "pugixml/pugixml.hpp"
#include "cxx.h"

Branch* Branch::Create(const pugi::xml_node& node)
{
	Branch* branch = new Branch();
	if (branch->Parse(node))
	{
		return branch;
	}
	else
	{
		delete branch;
		return nullptr;
	}
}

Branch::~Branch()
{
	cxx::SafeDelete(mJumpCondition);
}

bool Branch::CheckAvailable(Context& context) const
{
	return CheckCondition(context, mJumpCondition);
}

bool Branch::Parse(const pugi::xml_node& nodeBranch)
{
	mQuestionName = nodeBranch.attribute("question").as_string();
	if (mQuestionName == "")
		return false;

	auto attribCondition = nodeBranch.attribute("if");
	if (attribCondition)
	{
		mJumpCondition = Expression::Create(attribCondition.as_string());
		return mJumpCondition != nullptr;
	}
	else
	{
		return true;
	}
}