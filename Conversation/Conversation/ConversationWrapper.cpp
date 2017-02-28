#include "ConversationWrapper.h"
#include "Conversation.h"
#include "ExpressionBuilder.h"
#include "pugixml/pugixml.hpp"
#include "cxx.h"

extern "C"
{
	//Context
	Context* CreateContext()
	{
		return new Context();
	}

	void ReleaseContext(Context* pContext)
	{
		cxx::SafeDelete(pContext);
		ExpressionBuilderHelper::Release();
	}

	Save* GetContextGlobalSave(Context* pContext)
	{
		return pContext != nullptr ? pContext->Global : nullptr;
	}

	Save* GetContextPlayerSave(Context* pContext)
	{
		return pContext != nullptr ? pContext->Player : nullptr;
	}

	Save* GetContextConversationSave(Context* pContext)
	{
		return pContext != nullptr ? pContext->Conversation : nullptr;
	}

	Save* GetContextNPCSave(Context* pContext)
	{
		return pContext != nullptr ? pContext->NPC : nullptr;
	}

	void SetContextNPCSave(Context* pContext, Save* pSave)
	{
		if (pContext != nullptr)
		{
			pContext->NPC = pSave;
		}
	}

	void SetExternalFunctionHandler(Context* pContext, FunctionHandler pHandler)
	{
		if (pContext != nullptr)
		{
			pContext->SetExternalFunctionHandler(pHandler);
		}
	}

	void SetExternalProcessHandler(Context* pContext, FunctionHandler pHandler)
	{
		if (pContext != nullptr)
		{
			pContext->SetExternalProcessHandler(pHandler);
		}
	}

	void SetExternalFunctionQuery(Context* pContext, FunctionQuery pQuery)
	{
		if (pContext != nullptr)
		{
			pContext->SetExternalFunctionQuery(pQuery);
		}
	}

	void SetExternalProcessQuery(Context* pContext, FunctionQuery pQuery)
	{
		if (pContext != nullptr)
		{
			pContext->SetExternalProcessQuery(pQuery);
		}
	}

	//FunctionStack
	int GetFunctionStackPreferReturnType(FunctionStack* pFunctionStack)
	{
		if (pFunctionStack != nullptr)
		{
			return static_cast<int>(pFunctionStack->GetPreferReturnType());
		}
		else
		{
			return kVariantInvalid;
		}
	}

	int GetFunctionStackParamCount(FunctionStack* pFunctionStack)
	{
		if (pFunctionStack != nullptr)
		{
			return pFunctionStack->GetParamCount();
		}
		else
		{
			return 0;
		}
	}

	int GetFunctionStackMaxParamType(FunctionStack* pFunctionStack)
	{
		if (pFunctionStack != nullptr)
		{
			return static_cast<int>(pFunctionStack->GetMaxParamType());
		}
		else
		{
			return kVariantInvalid;
		}
	}

	int GetFunctionStackParamTypeByIndex(FunctionStack* pFunctionStack, int index)
	{
		if (pFunctionStack != nullptr &&
			index >= 0 && index < pFunctionStack->GetParamCount())
		{
			const Variant& v = pFunctionStack->Params()[index];
			return static_cast<int>(v.GetType());
		}
		else
		{
			return kVariantInvalid;
		}
	}

	int GetFunctionStackParamAsBool(FunctionStack* pFunctionStack, int index)
	{
		if (pFunctionStack != nullptr &&
			index >= 0 && index < pFunctionStack->GetParamCount())
		{
			const Variant& v = pFunctionStack->Params()[index];
			return v.To<bool>();
		}
		else
		{
			return false;
		}
	}

	int GetFunctionStackParamAsInt(FunctionStack* pFunctionStack, int index)
	{
		if (pFunctionStack != nullptr &&
			index >= 0 && index < pFunctionStack->GetParamCount())
		{
			const Variant& v = pFunctionStack->Params()[index];
			return v.To<int>();
		}
		else
		{
			return 0;
		}
	}

	float GetFunctionStackParamAsFloat(FunctionStack* pFunctionStack, int index)
	{
		if (pFunctionStack != nullptr &&
			index >= 0 && index < pFunctionStack->GetParamCount())
		{
			const Variant& v = pFunctionStack->Params()[index];
			return v.To<float>();
		}
		else
		{
			return 0.0f;
		}
	}

	void SetFunctionStackReturnBool(FunctionStack* pFunctionStack, int v)
	{
		if (pFunctionStack != nullptr)
		{
			pFunctionStack->mReturnValue = (v != 0);
		}
	}

	void SetFunctionStackReturnInt(FunctionStack* pFunctionStack, int v)
	{
		if (pFunctionStack != nullptr)
		{
			pFunctionStack->mReturnValue = v;
		}
	}

	void SetFunctionStackReturnFloat(FunctionStack* pFunctionStack, float v)
	{
		if (pFunctionStack != nullptr)
		{
			pFunctionStack->mReturnValue = v;
		}
	}

	//Save
	Save* CreateCustomSave()
	{
		return new Save();
	}

	void ReleaseCustomSave(Save* pSave)
	{
		cxx::SafeDelete(pSave);
	}

	int SaveHasValue(Save* save, const char* name)
	{
		return (save != nullptr && name != nullptr && save->HasValue(name)) ? 1 : 0;
	}

	int GetSaveValueCount(Save* save)
	{
		return (save != nullptr) ? (save->GetValueCount()) : 0;
	}

	const char* GetSaveValueNameByIndex(Save* save, int index)
	{
		return (save != nullptr) ? (save->GetValueNameByIndex(index)) : "";
	}

	int GetSaveValueType(Save* save, const char* name)
	{
		if (save != nullptr && name != nullptr && save->HasValue(name))
		{
			auto type = save->GetValueByName(name).Value.GetType();
			switch (type)
			{
			case Variant::Type::None: return kVariantNone;
			case Variant::Type::Bool: return kVariantBool;
			case Variant::Type::Int:  return kVariantInt;
			case Variant::Type::Float:return kVariantFloat;
			}
		}
		return kVariantInvalid;
	}

	int GetSaveValueAsBoolean(Save* save, const char* name)
	{
		if (save != nullptr && name != nullptr && save->HasValue(name))
		{
			return (save->GetValueByName(name).Value.As<bool>() ? 1 : 0);
		}
		else
		{
			return 0;
		}
	}

	int GetSaveValueAsInt(Save* save, const char* name)
	{
		if (save != nullptr && name != nullptr && save->HasValue(name))
		{
			return save->GetValueByName(name).Value.As<int>();
		}
		else
		{
			return 0;
		}
	}

	float GetSaveValueAsFloat(Save* save, const char* name)
	{
		if (save != nullptr && name != nullptr && save->HasValue(name))
		{
			return save->GetValueByName(name).Value.As<float>();
		}
		else
		{
			return 0.0f;
		}
	}

	int GetSaveValueToBoolean(Save* save, const char* name)
	{
		if (save != nullptr && name != nullptr && save->HasValue(name))
		{
			return save->GetValueByName(name).Value.To<bool>() ? 1 : 0;
		}
		else
		{
			return 0;
		}
	}

	int GetSaveValueToInt(Save* save, const char* name)
	{
		if (save != nullptr && name != nullptr && save->HasValue(name))
		{
			return save->GetValueByName(name).Value.To<int>();
		}
		else
		{
			return 0;
		}
	}

	float GetSaveValueToFloat(Save* save, const char* name)
	{
		if (save != nullptr && name != nullptr && save->HasValue(name))
		{
			return save->GetValueByName(name).Value.To<float>();
		}
		else
		{
			return 0.0f;
		}
	}

	void SetSaveValueBoolean(Save* save, const char* name, int v)
	{
		if (save != nullptr && name != nullptr)
		{
			save->SetValue(name, (v != 0));
		}
	}

	void SetSaveValueInt(Save* save, const char* name, int v)
	{
		if (save != nullptr && name != nullptr)
		{
			save->SetValue(name, v);
		}
	}

	void SetSaveValueFloat(Save* save, const char* name, float v)
	{
		if (save != nullptr && name != nullptr)
		{
			save->SetValue(name, v);
		}
	}

	CONV_API void SetSaveValueReadOnly(Save *save, const char * name, int c)
	{
		if (save != nullptr && name != nullptr)
		{
			SaveValue& value = save->GetValueByName(name);
			value.ReadOnly = (c != 0);
		}
	}

	//Conversation
	Conversation* CreateConversationFromXML(const char* filePath)
	{
		pugi::xml_document xmlFile;
		auto result = xmlFile.load_file(filePath, pugi::parse_full);
		if (result.status != pugi::status_ok)
		{
			return nullptr;
		}

		return Conversation::Create(xmlFile);
	}

	void ReleaseConversation(Conversation* pConversation)
	{
		cxx::SafeDelete(pConversation);
	}

	Question* StartConversation(Context* pContext, Conversation* pConversation)
	{
		return pConversation != nullptr ? pConversation->Start(pContext) : nullptr;
	}

	Question* SelectNextConversationBranch(Conversation* pConversation, Answer* selectedAnswer)
	{
		return pConversation != nullptr ? pConversation->SelectNextBranch(selectedAnswer) : nullptr;
	}
	
	Save* GetConversationSave(Conversation* pConversation)
	{
		return pConversation != nullptr ? pConversation->GetSave() : nullptr;
	}
	
	//Question
	const char* GetQuestionName(Question* pQuestion)
	{
		return pQuestion != nullptr ? pQuestion->GetName().c_str() : "";
	}

	const char* GetQuestionText(Question* pQuestion)
	{
		return pQuestion != nullptr ? pQuestion->GetText().c_str() : "";
	}

	int GetQuestionAnswerCount(Question* pQuestion)
	{
		return pQuestion != nullptr ? pQuestion->GetAnswerCount() : 0;
	}

	Answer* GetQuestionAnswerByIndex(Question* pQuestion, unsigned int index)
	{
		return pQuestion != nullptr ? pQuestion->GetAnswerByIndex(index) : nullptr;
	}

	//Answer
	int GetAnswerID(Answer* pAnswer)
	{
		return pAnswer != nullptr ? pAnswer->GetID() : 0;
	}

	const char* GetAnswerText(Answer* pAnswer)
	{
		return pAnswer != nullptr ? pAnswer->GetText().c_str() : "";
	}

	int IsAnswerAvailable(Answer* pAnswer)
	{
		return pAnswer != nullptr && pAnswer->IsAvailable() ? 1 : 0;
	}
}