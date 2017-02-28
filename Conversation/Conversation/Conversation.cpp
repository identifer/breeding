#include "Conversation.h"
#include "pugixml/pugixml.hpp"
#include "cxx.h"

Conversation* Conversation::Create(const pugi::xml_document& xmlFile)
{
	Conversation* conv = new Conversation();
	if (conv->Parse(xmlFile.child("conversation")))
	{
		return conv;
	}
	else
	{
		delete conv;
		return nullptr;
	}
}

Conversation::~Conversation()
{
	for (Action* action : mDefaultActions)
	{
		delete action;
	}

	for (Branch* entrance : mDefaultBranches)
	{
		delete entrance;
	}

	for (auto& question : mQuestionList)
	{
		delete question.second;
	}
}

void Conversation::ExecuteDefaultActions()
{
	for (Action* pAction : mDefaultActions)
	{
		if (pAction->IsAvailable(*mContext))
		{
			pAction->Execute(*mContext);
		}
	}
}

bool Conversation::Parse(const pugi::xml_node& root)
{
	auto nodeDefaultActions = root.child("default_actions");
	if (nodeDefaultActions)
	{
		for (auto nodeAction = nodeDefaultActions.first_child(); nodeAction; nodeAction = nodeAction.next_sibling())
		{
			if (nodeAction.name() != std::string("action"))
				continue;

			Action* action = Action::Create(nodeAction);
			if (action != nullptr)
			{
				mDefaultActions.push_back(action);
			}
		}
	}

	auto nodeDefaultBranches = root.child("default_branches");
	if (nodeDefaultBranches)
	{
		for (auto nodeBranch = nodeDefaultBranches.first_child(); nodeBranch; nodeBranch = nodeBranch.next_sibling())
		{
			if (nodeBranch.name() != std::string("branch"))
				continue;

			Branch* branch = Branch::Create(nodeBranch);
			if (branch != nullptr)
			{
				mDefaultBranches.push_back(branch);
			}
		}

		if (mDefaultBranches.size() == 0)
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	

	auto nodeQuestionList = root.child("question_list");
	if (nodeQuestionList)
	{
		for (auto nodeQuestion = nodeQuestionList.first_child(); nodeQuestion; nodeQuestion = nodeQuestion.next_sibling())
		{
			if (nodeQuestion.name() != std::string("question"))
				continue;

			Question* question = Question::Create(this, nodeQuestion);
			if (question != nullptr)
			{
				mQuestionList.insert(std::make_pair(question->GetName(), question));
			}
		}

		if (mQuestionList.size() == 0)
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

Question* Conversation::Start(Context* pContext)
{
	if (pContext != nullptr)
	{
		mSave.Clear();
		mContext = pContext;
		mContext->Conversation = &mSave;
		if (mContext->NPC != nullptr)
		{
			SaveValue& sv = mContext->NPC->GetValueByName("talk_count");
			sv.ReadOnly = true;
			sv.Value += Variant(1);
		}
		mSave.GetValueByName("quit").ReadOnly = true;
		mSave.GetValueByName("talk_count").ReadOnly = true;
		mSave.GetValueByName("last_answer_id").ReadOnly = true;
		ExecuteDefaultActions();
		return GetDefaultQuestion();
	}
	else
	{
		return nullptr;
	}
}

Question* Conversation::SelectNextBranch(Answer* pSelectedAnswer)
{
	if (pSelectedAnswer != nullptr)
	{
		mSave.GetValueByName("last_answer_id").Value = pSelectedAnswer->GetID();

		mContext->Push();
		pSelectedAnswer->DoActions();
		mContext->Pop();

		if (!mSave.GetValueByName("quit").Value.As<bool>())
		{
			ExecuteDefaultActions();
			auto questionName = pSelectedAnswer->GetPredictedBranchName();
			if (mQuestionList.count(questionName) > 0)
			{
				OnSwitchBranch();
				return mQuestionList[questionName];
			}
			else
			{
				return GetDefaultQuestion();
			}
		}
	}

	mContext = nullptr;
	return nullptr;
}

Question* Conversation::GetDefaultQuestion()
{
	for (auto& branch : mDefaultBranches)
	{
		if (branch->CheckAvailable(*mContext))
		{
			OnSwitchBranch();
			return mQuestionList[branch->GetQuestionName()];
		}
	}

	return nullptr;
}

void Conversation::OnSwitchBranch()
{
	Variant& vcount = mSave.GetValueByName("talk_count").Value;
	vcount += Variant(1);
}
