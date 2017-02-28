using System;
using System.Runtime.InteropServices;

namespace ConvAPI
{
    // 使用MarshalAs.LPStr来转换const char*的返回值会莫名其妙的崩溃
    // 但是使用Marshal.PtrToStringAnsi就没出过问题，于是手写了个自定义的转换器
    class CustomStringMarshaler : ICustomMarshaler
    {
        public object MarshalNativeToManaged(IntPtr pNativeData)
        {
            string str = Marshal.PtrToStringAnsi(pNativeData);
            return (str != null) ? str : "";
        }

        public IntPtr MarshalManagedToNative(object ManagedObj)
        {
            return IntPtr.Zero;
        }

        public void CleanUpNativeData(IntPtr pNativeData) { }

        public void CleanUpManagedData(object ManagedObj) { }

        public int GetNativeDataSize() { return IntPtr.Size; }

        static readonly CustomStringMarshaler instance = new CustomStringMarshaler();

        public static ICustomMarshaler GetInstance(string cookie) { return instance; }
    }

    public enum TValue : int
    {
        Invalid = -1,
        None = 0,
        Boolean = 1,
        Int = 2,
        Float = 3,
    }

    // 注册函数需要使用 AOT 如下标签
    //[MonoPInvokeCallbackAttribute(typeof(Callback))]

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate bool FunctionHandler(string name, IntPtr pFunctionStack);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate bool FunctionQuery(string name);

    static class ConversationAPI
    {
        // Context
        [DllImport("Conversation", EntryPoint = "CreateContext")]
        public static extern IntPtr CreateContext();

        [DllImport("Conversation", EntryPoint = "ReleaseContext")]
        public static extern void ReleaseContext(IntPtr pContext);

        [DllImport("Conversation", EntryPoint = "GetContextGlobalSave")]
        public static extern IntPtr GetContextGlobalSave(IntPtr pContext);

        [DllImport("Conversation", EntryPoint = "GetContextPlayerSave")]
        public static extern IntPtr GetContextPlayerSave(IntPtr pContext);

        [DllImport("Conversation", EntryPoint = "GetContextConversationSave")]
        public static extern IntPtr GetContextConversationSave(IntPtr pContext);

        [DllImport("Conversation", EntryPoint = "GetContextNPCSave")]
        public static extern IntPtr GetContextNPCSave(IntPtr pContext);

        [DllImport("Conversation", EntryPoint = "SetContextNPCSave")]
        public static extern void SetContextNPCSave(IntPtr pContext, IntPtr pSave);

        [DllImport("Conversation", EntryPoint = "SetExternalFunctionHandler")]
        public static extern void SetFunctionHandler(IntPtr pContext, FunctionHandler pHandler);

        [DllImport("Conversation", EntryPoint = "SetExternalProcessHandler")]
        public static extern void SetProcessHandler(IntPtr pContext, FunctionHandler pHandler);

        [DllImport("Conversation", EntryPoint = "SetExternalFunctionQuery")]
        public static extern void SetFunctionQuery(IntPtr pContext, FunctionQuery pQuery);

        [DllImport("Conversation", EntryPoint = "SetExternalProcessQuery")]
        public static extern void SetProcessQuery(IntPtr pContext, FunctionQuery pQuery);

        // FunctionStack
        [DllImport("Conversation", EntryPoint = "GetFunctionStackPreferReturnType")]
        [return: MarshalAs(UnmanagedType.I4)]
        public static extern TValue GetFunctionStackPreferReturnType(IntPtr pFunctionStack);

        [DllImport("Conversation", EntryPoint = "GetFunctionStackMaxParamType")]
        [return: MarshalAs(UnmanagedType.I4)]
        public static extern TValue GetFunctionStackMaxParamType(IntPtr pFunctionStack);
        
        [DllImport("Conversation", EntryPoint = "GetFunctionStackParamCount")]
        public static extern int GetFunctionStackParamCount(IntPtr pFunctionStack);

        [DllImport("Conversation", EntryPoint = "GetFunctionStackParamTypeByIndex")]
        [return: MarshalAs(UnmanagedType.I4)]
        public static extern TValue GetFunctionStackParamTypeByIndex(IntPtr pFunctionStack, int index);

        [DllImport("Conversation", EntryPoint = "GetFunctionStackParamAsBool")]
        [return: MarshalAs(UnmanagedType.I4)]
        public static extern bool GetFunctionStackParamAsBool(IntPtr pFunctionStack, int index);

        [DllImport("Conversation", EntryPoint = "GetFunctionStackParamAsInt")]
        public static extern int GetFunctionStackParamAsInt(IntPtr pFunctionStack, int index);

        [DllImport("Conversation", EntryPoint = "GetFunctionStackParamAsFloat")]
        public static extern float GetFunctionStackParamAsFloat(IntPtr pFunctionStack, int index);

        [DllImport("Conversation", EntryPoint = "SetFunctionStackReturnBool")]
        public static extern void SetFunctionStackReturnValue(IntPtr pFunctionStack, [MarshalAs(UnmanagedType.I4)]bool value);

        [DllImport("Conversation", EntryPoint = "SetFunctionStackReturnInt")]
        public static extern void SetFunctionStackReturnValue(IntPtr pFunctionStack, int value);

        [DllImport("Conversation", EntryPoint = "SetFunctionStackReturnFloat")]
        public static extern void SetFunctionStackReturnValue(IntPtr pFunctionStack, float value);

        // Save
        [DllImport("Conversation", EntryPoint = "CreateCustomSave")]
        public static extern IntPtr CreateCustomSave();

        [DllImport("Conversation", EntryPoint = "ReleaseCustomSave")]
        public static extern void ReleaseCustomSave(IntPtr pSave);

        [DllImport("Conversation", EntryPoint = "SaveHasValue")]
        [return: MarshalAs(UnmanagedType.I4)]
        public static extern bool SaveHasValue(IntPtr pSave, string name);

        [DllImport("Conversation", EntryPoint = "GetSaveValueCount")]
        public static extern int GetSaveValueCount(IntPtr pSave);

        [DllImport("Conversation", EntryPoint = "GetSaveValueNameByIndex")]
        [return: MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(CustomStringMarshaler))]
        public static extern string GetSaveValueNameByIndex(IntPtr pSave, int index);

        [DllImport("Conversation", EntryPoint = "GetSaveValueType")]
        [return: MarshalAs(UnmanagedType.I4)]
        public static extern TValue GetSaveValueType(IntPtr pSave, string name);

        [DllImport("Conversation", EntryPoint = "GetSaveValueAsBoolean")]
        [return: MarshalAs(UnmanagedType.I4)]
        public static extern bool GetSaveValueAsBoolean(IntPtr pSave, string name);

        [DllImport("Conversation", EntryPoint = "GetSaveValueAsInt")]
        public static extern int GetSaveValueAsInt(IntPtr pSave, string name);

        [DllImport("Conversation", EntryPoint = "GetSaveValueAsFloat")]
        public static extern float GetSaveValueAsFloat(IntPtr pSave, string name);

        [DllImport("Conversation", EntryPoint = "GetSaveValueToBoolean")]
        public static extern int GetSaveValueToBoolean(IntPtr pSave, string name);

        [DllImport("Conversation", EntryPoint = "GetSaveValueToInt")]
        public static extern int GetSaveValueToInt(IntPtr pSave, string name);

        [DllImport("Conversation", EntryPoint = "GetSaveValueToFloat")]
        public static extern float GetSaveValueToFloat(IntPtr pSave, string name);

        [DllImport("Conversation", EntryPoint = "SetSaveValueBoolean")]
        public static extern void SetSaveValue(IntPtr pSave, string name, [MarshalAs(UnmanagedType.I4)]bool v);

        [DllImport("Conversation", EntryPoint = "SetSaveValueInt")]
        public static extern void SetSaveValue(IntPtr pSave, string name, int v);

        [DllImport("Conversation", EntryPoint = "SetSaveValueFloat")]
        public static extern void SetSaveValue(IntPtr pSave, string name, float v);

        [DllImport("Conversation", EntryPoint = "SetSaveValueReadOnly")]
        public static extern void SetSaveValueReadOnly(IntPtr pSave, string name, [MarshalAs(UnmanagedType.I4)]bool v);

        // Conversation
        [DllImport("Conversation", EntryPoint = "CreateConversationFromXML")]
        public static extern IntPtr CreateConversationFromXML(string xmlFilePath);

        [DllImport("Conversation", EntryPoint = "ReleaseConversation")]
        public static extern void ReleaseConversation(IntPtr pConversation);

        [DllImport("Conversation", EntryPoint = "StartConversation")]
        public static extern IntPtr StartConversation(IntPtr pContext, IntPtr pConversation);

        [DllImport("Conversation", EntryPoint = "SelectNextConversationBranch")]
        public static extern IntPtr SelectNextConversationBranch(IntPtr pConversation, IntPtr pSelectedAnswer);

        [DllImport("Conversation", EntryPoint = "GetConversationSave")]
        public static extern IntPtr GetConversationSave(IntPtr pConversation);

        // Question
        [DllImport("Conversation", EntryPoint = "GetQuestionName")]
        [return: MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(CustomStringMarshaler))]
        public static extern string GetQuestionName(IntPtr pQuestion);

        [DllImport("Conversation", EntryPoint = "GetQuestionText")]
        [return: MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(CustomStringMarshaler))]
        public static extern string GetQuestionText(IntPtr pQuestion);

        [DllImport("Conversation", EntryPoint = "GetQuestionAnswerCount")]
        public static extern int GetQuestionAnswerCount(IntPtr pQuestion);

        [DllImport("Conversation", EntryPoint = "GetQuestionAnswerByIndex")]
        public static extern IntPtr GetQuestionAnswerByIndex(IntPtr pQuestion, int index);

        // Answer
        [DllImport("Conversation", EntryPoint = "GetAnswerID")]
        public static extern int GetAnswerID(IntPtr pAnswer);

        [DllImport("Conversation", EntryPoint = "GetAnswerText")]
        [return: MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(CustomStringMarshaler))]
        public static extern string GetAnswerText(IntPtr pAnswer);

        [DllImport("Conversation", EntryPoint = "IsAnswerAvailable")]
        [return: MarshalAs(UnmanagedType.I4)]
        public static extern bool IsAnswerAvailable(IntPtr pAnswer);
    }
}