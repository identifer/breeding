#ifdef _MSC_VER
#include <windows.h>
#endif
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include "Conversation.h"
#include "ExpressionBuilder.h"
#include "cxx.h"
#include "variant.h"
#include "pugixml\pugixml.hpp"
#include "Expression.h"

const std::string kDialogFolder = "../../Assets/StreamingAssets/Dialogues/";
Conversation* currentConversation = nullptr;
std::vector<Conversation*> allConversations;

std::vector<std::string> xmlFiles;

#ifdef _MSC_VER
std::wstring UTF8_Unicode(std::string utf8)
{
	std::wstring output;
	int length = ::MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
	output.resize(length + 1);
	output[length] = L'\0';
	::MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &(output[0]), length);
	return output;
}

std::wstring ANSI_Unicode(std::string ansi)
{
	std::wstring unicode;
	int length = ::MultiByteToWideChar(CP_ACP, 0, ansi.c_str(), -1, NULL, 0);
	unicode.resize(length + 1);
	unicode[length] = L'\0';
	::MultiByteToWideChar(CP_ACP, 0, ansi.c_str(), -1, &(unicode[0]), length);
	return unicode;
}

std::string Unicode_UTF8(std::wstring unicode)
{
	std::string utf8;
	int length = ::WideCharToMultiByte(CP_UTF8, 0, unicode.c_str(), -1, NULL, 0, NULL, NULL);
	utf8.resize(length + 1);
	utf8[length] = '\0';
	::WideCharToMultiByte(CP_UTF8, 0, unicode.c_str(), -1, &(utf8[0]), length, NULL, NULL);
	return utf8;
}

std::string Unicode_ANSI(std::wstring unicode)
{
	std::string output;
	int length = ::WideCharToMultiByte(CP_ACP, 0, unicode.c_str(), -1, NULL, 0, NULL, NULL);
	output.resize(length + 1);
	output[length] = '\0';
	::WideCharToMultiByte(CP_ACP, 0, unicode.c_str(), -1, &(output[0]), length, NULL, NULL);
	return output;
}


std::string escape(const std::string& utf8)
{
	std::string output = Unicode_ANSI(UTF8_Unicode(utf8));

	auto p = output.find("\\n");
	while (p != output.npos)
	{
		output.replace(p, 2, "\n");
		p = output.find("\\n");
	}
	return output;
}

std::string utf8(const std::string& ansi)
{
	std::string output = Unicode_UTF8(ANSI_Unicode(ansi));
	return output;
}
#else

std::string escape(const std::string& utf8)
{
	std::string output = utf8;
	auto p = output.find("\\n");
	while (p != output.npos)
	{
		output.replace(p, 2, "\n");
		p = output.find("\\n");
	}
	return std::move(output);
}

std::string utf8(const std::string& ansi)
{
	return ansi;
}
#endif

std::ostream& operator<<(std::ostream& out, const Variant& v)
{
	switch (v.GetType())
	{
	default:
	case Variant::Type::None: out << "none"; break;
	case Variant::Type::Int: out << v.As<int>() << " i"; break;
	case Variant::Type::Bool: out << (v.As<bool>() ? "true" : "false"); break;
	case Variant::Type::Float: out << v.As<float>() << " f"; break;
	}
	return out;
}

void ShowProperties(Context* pContext)
{
	struct
	{
		std::string Name;
		Save* SavePtr;
	} blocks[] =
	{
		{ "全局", pContext->Global },
		{ "对话", pContext->Conversation },
		{ "角色", pContext->Player },
		{ "NPC",  pContext->NPC },
	};

	for (auto& p : blocks)
	{
		if (p.SavePtr == nullptr)
			continue;

		int valueCount = p.SavePtr->GetValueCount();
		if (valueCount)
		{
			std::cout << p.Name << "属性：" << std::endl;
			for (int i = 0; i < valueCount; i++)
			{
				std::string valueName = p.SavePtr->GetValueNameByIndex(i);
				Variant v = p.SavePtr->GetValueByName(valueName).Value;
				std::cout << "\t" << valueName << ":" << v << std::endl;
			}

			std::cout << std::endl;
		}
	}
}

void DoConversation(Context* context, Conversation* conv)
{
	currentConversation = conv;

	Question* currentQuestion = conv->Start(context);

	while (currentQuestion != nullptr)
	{
		std::cout << "当前对话人：" << std::endl << std::endl;
		ShowProperties(context);

		std::cout << escape(currentQuestion->GetText())
			<< std::endl << std::endl;


		int answer_count = currentQuestion->GetAnswerCount();
		if (answer_count == 0)
		{
			break;
		}

		int i = 1;
		for (; i <= answer_count; i++)
		{
			Answer* answer = currentQuestion->GetAnswerByIndex(i - 1);
			if (answer->IsAvailable())
			{
				std::cout << i << "-"
					<< escape(answer->GetText())
					<< std::endl;
			}
		}

		std::cout << std::endl << "选择回答： （退出输入别的标号）";
		std::cin >> i;
		if (i <= 0 || i > answer_count)
		{
			break;
		}

		i--;
		Answer* answer = currentQuestion->GetAnswerByIndex(i);
		if (answer->IsAvailable())
		{
			currentQuestion = conv->SelectNextBranch(answer);
			system("cls");
		}
		else
		{
			break;
		}
	}
}

#ifdef _MSC_VER
void TraversalDialogueFolder()
{
	WIN32_FIND_DATAA find_data;
	HANDLE find_ret;
	find_ret = ::FindFirstFileA((kDialogFolder + "*.xml").c_str(), &find_data);
	if (find_ret != INVALID_HANDLE_VALUE)
	{
		do
		{
			if ((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
			{
				continue;
			}

			xmlFiles.push_back(find_data.cFileName);
		} while (TRUE == FindNextFileA(find_ret, &find_data));
		::FindClose(find_ret);
	}
	for (auto& name : xmlFiles)
	{
		pugi::xml_document xmlFile;
		std::string path = kDialogFolder + name;
		auto result = xmlFile.load_file(path.c_str(), pugi::parse_full, pugi::encoding_utf8);
		if (result.status == pugi::status_ok)
		{
			Conversation* dialog = Conversation::Create(xmlFile);
			if (dialog)
			{
				allConversations.push_back(dialog);
			}
			else
			{
				std::cout << "error file:" << name.c_str() << std::endl;
			}
		}
	}
}
#endif

template<class T, typename = std::enable_if_t<!std::is_array_v<T>>>
auto AddChildNode(pugi::xml_node& node, const T& value)
{
	return node.append_child(value);
}

template<class T, typename  = std::enable_if_t<!std::is_array_v<T>>>
void AddNodeAttrib(pugi::xml_node& node, const std::string& name, const T& value)
{
	node.append_attribute(utf8(name).c_str()).set_value(value);
}

void AddNodeAttrib(pugi::xml_node& node, const std::string& name, const std::string& value)
{
	node.append_attribute(utf8(name).c_str()).set_value(utf8(value).c_str());
}

auto AddChildNode(pugi::xml_node& node, const std::string& value)
{
	return node.append_child(utf8(value).c_str());
}

void CreateExampleXML(const std::string& file_name)
{
	pugi::xml_document example_file;

	//create header
	{
		auto decl = AddChildNode(example_file, pugi::node_declaration);
		AddNodeAttrib(decl, "version", "1.0");
		AddNodeAttrib(decl, "encoding", "utf-8");
	}

	//构造子节点
	auto node_dialog = AddChildNode(example_file,"conversation");
	auto node_entrances = AddChildNode(node_dialog, "default_branches");
	auto node_questions = AddChildNode(node_dialog, "question_list");

	//构造入口列表
	{
		{
			auto node_goto = AddChildNode(node_entrances,"branch");
			AddNodeAttrib(node_goto, "if", "conversation.talk_count > 1");
			AddNodeAttrib(node_goto, "question", "another_default");
		}

		{
			auto node_goto = AddChildNode(node_entrances,"branch");
			AddNodeAttrib(node_goto, "question", "default");
		}
	}

	//构造问题列表
	{
		//default question
		{
			auto node_question = AddChildNode(node_questions, "question");
			AddNodeAttrib(node_question, "name", "default");
			AddNodeAttrib(node_question, "text", "你好，陌生人……");

			{
				auto node_answer = AddChildNode(node_question, "answer");
				AddNodeAttrib(node_answer, "id", "1");
				AddNodeAttrib(node_answer, "text", "你是谁？");
				{
					auto node_goto = AddChildNode(node_answer, "branch");
					AddNodeAttrib(node_goto, "question", "who_are_u");
				}
			}

			{
				auto node_answer = AddChildNode(node_question, "answer");
				AddNodeAttrib(node_answer, "id", "2");
				AddNodeAttrib(node_answer, "text", "这儿是哪里？");
				{
					auto node_goto = AddChildNode(node_answer, "branch");
					AddNodeAttrib(node_goto, "question", "where_am_i");
				}
			}
		}

		//question another_default
		{
			auto node_quesion = AddChildNode(node_questions, "question");
			AddNodeAttrib(node_quesion, "name", "another_default");
			AddNodeAttrib(node_quesion, "text", "你还有什么想要了解的么？");

			{
				auto node_answer = AddChildNode(node_quesion, "answer");
				AddNodeAttrib(node_answer, "id", "1");
				AddNodeAttrib(node_answer, "text", "你是谁？");
				AddNodeAttrib(node_answer, "if", "conversation.last_answer_id != 1 && conversation.talk_count < 7");

				{
					auto node_actions = AddChildNode(node_answer, "action");
					AddNodeAttrib(node_actions, "do", "npc.love+=1");
				}

				{
					auto node_goto = AddChildNode(node_answer, "branch");
					AddNodeAttrib(node_goto, "question", "who_are_u");
				}
			}

			{
				auto node_answer = AddChildNode(node_quesion, "answer");
				AddNodeAttrib(node_answer, "id", "2");
				AddNodeAttrib(node_answer, "text", "这儿是哪里？");
				AddNodeAttrib(node_answer, "if", "conversation.last_answer_id != 2 || conversation.talk_count >= 7");

				{
					auto node_actions = AddChildNode(node_answer, "action");
					AddNodeAttrib(node_actions, "do", "player.is_dead != player.is_dead");
				}

				{
					auto node_actions = AddChildNode(node_answer, "action");
					AddNodeAttrib(node_actions, "do", "player.hp = 100");
				}

				{
					auto node_actions = AddChildNode(node_answer, "action");
					AddNodeAttrib(node_actions, "do", "global.sunshine-=2.2");
					AddNodeAttrib(node_actions, "if", "global.sunshine > -10.0");
				}

				{
					auto node_goto = AddChildNode(node_answer, "branch");
					AddNodeAttrib(node_goto, "question", "where_am_i");
				}
			}
		}

		//question answer_who_are_u
		{
			auto node_quesion = AddChildNode(node_questions, "question");
			AddNodeAttrib(node_quesion, "name", "who_are_u");
			AddNodeAttrib(node_quesion, "text", "你猜！嘿嘿嘿...");

			{
				auto node_answer = AddChildNode(node_quesion, "answer");
				AddNodeAttrib(node_answer, "id", "1");
				AddNodeAttrib(node_answer, "text", "好吧……");
			}
		}

		//question answer_where_am_i
		{
			auto node_quesion = AddChildNode(node_questions, utf8("question"));
			AddNodeAttrib(node_quesion, "name", "where_am_i");
			AddNodeAttrib(node_quesion, "text", "oh，问得好，这得你自己去发现");

			{
				auto node_answer = AddChildNode(node_quesion, "answer");
				AddNodeAttrib(node_answer, "id", "2");
				AddNodeAttrib(node_answer, "text", "好吧……");
			}
		}
	}

	example_file.save_file(file_name.c_str());
}

int TestExampleXML()
{
	//构建example
	CreateExampleXML(kDialogFolder + "example.xml");

	//载入对话数据
#ifdef _MSC_VER
	TraversalDialogueFolder();
#else
	pugi::xml_document xmlFile;
	std::string path = kDialogFolder + "example.xml";
	auto result = xmlFile.load_file(path.c_str(), pugi::parse_full, pugi::encoding_utf8);
	if (result.status == pugi::status_ok)
	{
		Conversation* dialog = Conversation::Create(xmlFile);
		if (dialog)
		{
			allConversations.push_back(dialog);
			xmlFiles.push_back("example.xml");
		}
		else
		{
			std::cout << "error file:" << path.c_str() << std::endl;
		}
	}

#endif

	Context* context = new Context();

	//show dialogue
	int conversationCount = static_cast<int>(allConversations.size());
	if (conversationCount > 0)
	{
		while (true)
		{
			int i = 1;
			for (auto& conv : xmlFiles)
			{
				std::cout << i << ". " << conv << std::endl;
				i++;
			}

			std::cout << std::endl << "选择对话人： （退出输入别的标号）";
			std::cin >> i;
			if (i <= 0 || i > conversationCount)
				break;

			system("cls");
			DoConversation(context, allConversations[i - 1]);
		}

		for (int i = 0; i < conversationCount; i++)
		{
			cxx::SafeDelete(allConversations[i]);
		}
	}

	cxx::SafeDelete(context);
	return 0;
}

std::ostream& operator<<(std::ostream& out, const ExpressionBuilder& builder)
{
	if (builder)
	{
		out << "\tsuccessed";
	}
	else
	{
		out << std::endl << "  error step: ";

#define CASE_STEP(x) case ExpressionBuilder::Step::x: out<<#x <<std::endl;break;
		switch (builder.GetErrorStep())
		{
			CASE_STEP(TrimExpressionString);
			CASE_STEP(SerializeTokens);
			CASE_STEP(ConvertFunctionTokens);
			CASE_STEP(ConvertExpressionTokens);
			CASE_STEP(ValidateTokenSequance);
			CASE_STEP(ConvertRPNNList);
			CASE_STEP(BuildExpressionTree);
		}
#undef CASE_STEP

		out << "  reason: ";

#define CASE_REASON(x) case ExpressionBuilder::Reason::x: out<<#x <<std::endl;break;
		switch (builder.GetErrorReason())
		{
			CASE_REASON(NullExpression);
			CASE_REASON(NoExpressionTreeRoot);
			CASE_REASON(UnexpectedEnd);
			CASE_REASON(BracketNotMatch);
			CASE_REASON(UnrecognizedSymbol);
			CASE_REASON(MisplacedComma);
			CASE_REASON(InvalidVariableName);
			CASE_REASON(InvalidFunctionName);
			CASE_REASON(InvalidFunctionParam);
			CASE_REASON(InvalidOperator);
			CASE_REASON(InvalidAction);
			CASE_REASON(InvalidComparison);
			CASE_REASON(InvalidOperand);
			CASE_REASON(InvalidExpression);
		}
#undef CASE_REASON
	}
	return out;
}

void DumpExpressionBuilderInfos()
{
	std::vector<std::string> expressions =
	{
		"123456",
		"-123456",
		"!123456",
		"123.456",
		"-321.654",
		"!321.654",
		"123f",
		"123F",
		"-123f",
		"-123F",
		"!123f",
		"!123F",
		"123.456f",
		"-321.654F",
		"!321.654F",
		"true",
		"false",
		"-false",
		"!true",
		"!!true",
		"global.variable",
		"-global.random",
		"function()",
		"function(1,2,3)",
		"function(mix(), random(3,5),!(3 *6), 5<2)",
		"123+ 345",
		"1+2*3",
		"1*2*3-4*(5-6)",
		"1+2  <=3*4",
		"1+random(3,4) >= function(5.f)",
		"player.npc == 1*(2+4) - random(5,6)",
		"1*(2+4) - random(5,6) != 123.5f",
		"!5 < !6",
		"random(a.y) > !y.z",
		"global.set -= 1+2",
		"global.set += 3 * 6+ 9 *  funcion(5+1.5f, player.npc)",
		"x.y *= 6 + !random(123.0f)",
		"a.z=b.z",
		"!a.z > 5-2 =c.z"
	};


	for (auto cur = expressions.begin(), end = expressions.end(); cur != end; ++cur)
	{
		ExpressionBuilder builder(*cur);
		std::cout << "expr: "
			<< std::setw(30) << std::left << cur->c_str()
			<< builder
			<< std::endl;
	}

	std::cout << std::endl;
	ExpressionBuilderHelper::Release();
}

int main()
{
	DumpExpressionBuilderInfos();
	TestExampleXML();
}