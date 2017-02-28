#pragma once

#if defined _WIN32
#ifdef CONV_DLLEXPORT
#define CONV_API __declspec(dllexport)
#else
#define CONV_API __declspec(dllimport)
#endif
#else
#ifdef DLL_EXPORTS
#define CONV_API __attributes((visibility("default")))
#else
#define CONV_API 
#endif
#endif

#define kVariantInvalid -1
#define kVariantNone    0
#define kVariantBool    1
#define kVariantInt     2
#define kVariantFloat   3

#define kFixedParam     0
#define kMoreEqualParam 1
#define kLessEqual      2
#define kRangeParam     3

struct Context;
struct FunctionStack;
struct FunctionAdapter;
struct Save;
struct Conversation;
struct Question;
struct Answer;

typedef int(*FunctionHandler)(const char* name, FunctionStack*);
typedef int(*FunctionQuery)(const char* name);

extern "C"
{
	// Context
	CONV_API Context* CreateContext();

	CONV_API void ReleaseContext(Context*);

	CONV_API Save* GetContextGlobalSave(Context*);

	CONV_API Save* GetContextPlayerSave(Context*);

	CONV_API Save* GetContextConversationSave(Context*);

	CONV_API Save* GetContextNPCSave(Context*);

	CONV_API void SetContextNPCSave(Context*, Save* save);

	CONV_API void SetExternalFunctionHandler(Context*, FunctionHandler);

	CONV_API void SetExternalProcessHandler(Context*, FunctionHandler);

	CONV_API void SetExternalFunctionQuery(Context*, FunctionQuery);

	CONV_API void SetExternalProcessQuery(Context*, FunctionQuery);

	// FunctionStack
	CONV_API int GetFunctionStackPreferReturnType(FunctionStack*);

	CONV_API int GetFunctionStackParamCount(FunctionStack*);

	CONV_API int GetFunctionStackMaxParamType(FunctionStack*);

	CONV_API int GetFunctionStackParamTypeByIndex(FunctionStack*, int index);

	CONV_API int GetFunctionStackParamAsBool(FunctionStack*, int index);

	CONV_API int GetFunctionStackParamAsInt(FunctionStack*, int index);

	CONV_API float GetFunctionStackParamAsFloat(FunctionStack*, int index);

	CONV_API void SetFunctionStackReturnBool(FunctionStack*, int);

	CONV_API void SetFunctionStackReturnInt(FunctionStack*, int);

	CONV_API void SetFunctionStackReturnFloat(FunctionStack*, float);

	// Save
	CONV_API Save* CreateCustomSave();

	CONV_API void ReleaseCustomSave(Save* save);

	CONV_API int SaveHasValue(Save*, const char* name);

	CONV_API int GetSaveValueCount(Save*);

	CONV_API const char* GetSaveValueNameByIndex(Save*, int index);

	CONV_API int GetSaveValueType(Save*, const char* name);

	CONV_API int GetSaveValueAsBoolean(Save*, const char* name);

	CONV_API int GetSaveValueAsInt(Save*, const char* name);

	CONV_API float GetSaveValueAsFloat(Save*, const char* name);

	CONV_API int GetSaveValueToBoolean(Save*, const char* name);

	CONV_API int GetSaveValueToInt(Save*, const char* name);

	CONV_API float GetSaveValueToFloat(Save*, const char* name);

	CONV_API void SetSaveValueBoolean(Save*, const char* name, int);

	CONV_API void SetSaveValueInt(Save*, const char* name, int);

	CONV_API void SetSaveValueFloat(Save*, const char* name, float);

	CONV_API void SetSaveValueReadOnly(Save*, const char* name, int);

	// Conversation
	CONV_API Conversation* CreateConversationFromXML(const char* filePath);

	CONV_API void ReleaseConversation(Conversation*);

	CONV_API Question* StartConversation(Context*, Conversation*);

	CONV_API Question* SelectNextConversationBranch(Conversation*, Answer* selected);

	CONV_API Save* GetConversationSave(Conversation*);

	// Question
	CONV_API const char* GetQuestionName(Question*);

	CONV_API const char* GetQuestionText(Question*);

	CONV_API int GetQuestionAnswerCount(Question*);

	CONV_API Answer* GetQuestionAnswerByIndex(Question*, unsigned int index);

	// Answer
	CONV_API int GetAnswerID(Answer*);

	CONV_API const char* GetAnswerText(Answer*);

	CONV_API int IsAnswerAvailable(Answer*);
}
