#include "Conversation.h"
#include "pugixml/pugixml.hpp"
#include "cxx.h"

Answer* Answer::Create(Question* pQuestion, const pugi::xml_node& node)
{
	Answer* pAnswer = new Answer(pQuestion);
	if (pAnswer->Parse(node))
	{
		return pAnswer;
	}
	else
	{
		delete pAnswer;
		return nullptr;
	}
}

Answer::Answer(Question * pQuestion)
	: mParent(pQuestion)
{

}

Answer::~Answer()
{
	cxx::SafeDelete(mShownCondition);

	for (auto& action : mActions)
	{
		delete action;
	}

	for (auto& branch : mJumpBranches)
	{
		delete branch;
	}
}

std::string Answer::GetPredictedBranchName() const
{
	if (mParent->GetParent().IsGoing())
	{
		for (auto& branch : mJumpBranches)
		{
			if (branch->CheckAvailable(mParent->GetParent().GetContext()))
			{
				return branch->GetQuestionName();
			}
		}
	}

	return "";
}

void Answer::DoActions()
{
	if (mParent->GetParent().IsGoing())
	{
		for (auto& action : mActions)
		{
			action->Execute(mParent->GetParent().GetContext());
		}
	}
}

bool Answer::IsAvailable()
{
	if (mParent->GetParent().IsGoing())
	{
		return CheckCondition(mParent->GetParent().GetContext(), mShownCondition);
	}
	else
	{
		return false;
	}
}

bool Answer::Parse(const pugi::xml_node& nodeAnswer)
{
	mID = nodeAnswer.attribute("id").as_int(0);
	mText = nodeAnswer.attribute("text").as_string();
	if (mText == "")
		return false;

	auto attribCondition = nodeAnswer.attribute("if");
	if (attribCondition)
	{
		mShownCondition = Expression::Create(attribCondition.as_string());
	}

	for (auto nodeChild = nodeAnswer.first_child(); nodeChild; nodeChild = nodeChild.next_sibling())
	{
		std::string nodeName = nodeChild.name();
		if (nodeName == "action")
		{
			Action* action = Action::Create(nodeChild);
			if (action != nullptr)
			{
				mActions.push_back(action);
			}
		}
		else if (nodeName == "branch")
		{
			Branch* branch = Branch::Create(nodeChild);
			if (branch != nullptr)
			{
				mJumpBranches.push_back(branch);
			}
		}
	}

	return true;
}
