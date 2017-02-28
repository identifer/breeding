#pragma once
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <set>
#include "Variant.h"
#include "Expression.h"
#include "Function.h"
#include "Save.h"

namespace pugi
{
	class xml_document;
	class xml_node;
}

struct Context;
struct Save;
struct Conversation;
struct Question;
struct Answer;

bool CheckCondition(Context& context, Expression* expr);

struct Action
{
	static Action* Create(pugi::xml_node& node);

	~Action();

	void Execute(Context& context);

	bool IsAvailable(Context& context);

private:
	bool Parse(pugi::xml_node& node);
	Expression* mExpression = nullptr;
	Expression* mCondition = nullptr;
};

struct Branch
{
	static Branch* Create(const pugi::xml_node&);

	~Branch();

	bool CheckAvailable(Context& context) const;

	inline const std::string& GetQuestionName() const { return mQuestionName; }

private:
	bool Parse(const pugi::xml_node&);

	std::string mQuestionName = "";
	Expression* mJumpCondition = nullptr;

};

struct Answer
{
	static Answer* Create(Question* pQuestion, const pugi::xml_node& node);

	Answer(Question* pQuestion);

	~Answer();

	void DoActions();

	std::string GetPredictedBranchName() const;

	bool IsAvailable();

	inline int GetID() const { return mID; }

	inline const std::string& GetText() const { return mText; }

private:
	bool Parse(const pugi::xml_node& node);

	int mID = 0;
	std::string mText = "";
	Expression* mShownCondition = nullptr;
	std::vector<Branch*> mJumpBranches;
	std::vector<Action*> mActions;

	Question* mParent = nullptr;
};

struct Question
{
	static Question* Create(Conversation* pConversation, const pugi::xml_node& node);

	Question(Conversation* pConversation);

	~Question();

	Answer* GetAnswerByIndex(unsigned int index);

	inline const std::string& GetName() const { return mName; }

	inline const std::string& GetText() const { return mText; }

	inline unsigned int GetAnswerCount() const { return static_cast<unsigned int>(mAnswers.size()); }

	inline Conversation& GetParent() const { return *mParent; }

private:
	bool Parse(const pugi::xml_node& node);

	std::string mName = "";
	std::string mText = "";
	std::vector<Answer*> mAnswers;

	Conversation* mParent = nullptr;
};

struct Conversation
{
public:
	static Conversation* Create(const pugi::xml_document& xmlFile);

	~Conversation();

	Question* Start(Context*);

	Question* SelectNextBranch(Answer* selectedAnswer);

	inline Save* GetSave() { return &mSave; }

	inline Context& GetContext() const { return *mContext; }

	inline bool IsGoing() const { return mContext != nullptr; }

	void ExecuteDefaultActions();

private:
	bool Parse(const pugi::xml_node& node);

	Question* GetDefaultQuestion();

	void OnSwitchBranch();

	Save mSave;
	std::vector<Action*> mDefaultActions;
	std::vector<Branch*> mDefaultBranches;
	std::map<std::string, Question*> mQuestionList;
	Context* mContext = nullptr;
};

struct Context
{
	typedef int(*FunctionHandler)(const char* name, FunctionStack*);
	typedef int(*FunctionQuery)(const char* name);

	Context();

	~Context();

	void Register(const std::string& aliasName, FunctionAdapter* pFunction);

	void Remove(const std::string& aliasName, FunctionAdapter* pAdapter);

	void RemoveAll(FunctionAdapter* pAdapter);

	bool CallProcess(const std::string& funcName);

	bool CallProcess(const std::string& funcName, std::vector<Variant>& params);

	bool CallFunction(const std::string& funcName, Variant& result);

	bool CallFunction(const std::string& funcName, Variant& result, std::vector<Variant>& params);

	bool HaveProcess(const std::string& funcName);

	bool HaveFunction(const std::string& funcName);
		
	void SetExternalFunctionHandler(FunctionHandler handler) { mExternalFunctionHandler = handler; }

	void SetExternalProcessHandler(FunctionHandler handler) { mExternalProcessHandler = handler; }

	void SetExternalFunctionQuery(FunctionQuery query) { mExternalFunctionQuery = query; }

	void SetExternalProcessQuery(FunctionQuery query) { mExternalProcessQuery = query; }

	Save* Global = nullptr;
	Save* Player = nullptr;
	Save* NPC = nullptr;
	Save* Conversation = nullptr;

	//.....
	// for global quit()
	//
	inline void Push() { mCurrentContext.push(this); }

	inline void Pop() { if (mCurrentContext.size() > 0) mCurrentContext.pop(); }

	inline static Context* GetCurrent() { return mCurrentContext.size() > 0 ? mCurrentContext.top() : nullptr; }

private:
	typedef std::set<FunctionAdapter*, FunctionSorter> FunctionList;
	typedef std::map<std::string, FunctionList> FunctionTable;

	static std::stack<Context*> mCurrentContext;

	bool HaveExternalProcess(const std::string& funcName);

	bool HaveLocalProcess(const std::string& funcName);

	bool HaveExternalFunction(const std::string& funcName);

	bool HaveLocalFunction(const std::string& funcName);

	bool Invoke(const std::string& name, FunctionStack& stack, FunctionHandler handler, FunctionTable& table);

	
	FunctionTable mProcessTable;
	FunctionTable mFunctionTable;
	std::set<FunctionAdapter*> mRemovedFunctions;

	FunctionHandler mExternalFunctionHandler = nullptr;
	FunctionHandler mExternalProcessHandler = nullptr;
	FunctionQuery mExternalFunctionQuery = nullptr;
	FunctionQuery mExternalProcessQuery = nullptr;
};