#include "Conversation.h"
#include "pugixml/pugixml.hpp"

Question* Question::Create(Conversation* pConversation, const pugi::xml_node &node)
{
	Question* pQuestion = new Question(pConversation);
	if (pQuestion->Parse(node))
	{
		return pQuestion;
	}
	else
	{
		delete pQuestion;
		return nullptr;
	}
}

Question::Question(Conversation * pConversation)
	: mParent(pConversation)
{

}

Question::~Question()
{
	for (auto& answer : mAnswers)
	{
		delete answer;
	}
}

Answer* Question::GetAnswerByIndex(unsigned int index)
{
	if (index < GetAnswerCount())
	{
		return mAnswers[index];
	}
	else
	{
		return nullptr;
	}
}

bool Question::Parse(const pugi::xml_node& nodeQuestion)
{
	mName = nodeQuestion.attribute("name").as_string();
	mText = nodeQuestion.attribute("text").as_string();

	if (mName == "" || mText == "")
		return false;

	for (auto nodeAnswer = nodeQuestion.first_child(); nodeAnswer; nodeAnswer = nodeAnswer.next_sibling())
	{
		if (nodeAnswer.name() != std::string("answer"))
			continue;

		Answer* pAnswer = Answer::Create(this, nodeAnswer);
		if (pAnswer != nullptr)
		{
			mAnswers.push_back(pAnswer);
		}
	}

	return true;
}
