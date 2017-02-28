using System;
using System.Collections.Generic;

namespace ConvAPI
{
    public class FunctionStack
    {
        private IntPtr mImplementPtr = IntPtr.Zero;
        private int mParamCount = 0;
        private TValue mReturnType = TValue.Invalid;
        private TValue mMaxParamType = TValue.Invalid;
        private List<TValue> mParamTypes = new List<TValue>();

        internal FunctionStack(IntPtr implPtr)
        {
            mImplementPtr = implPtr;

            mReturnType = ConversationAPI.GetFunctionStackPreferReturnType(ImplementPtr);
            mMaxParamType = ConversationAPI.GetFunctionStackMaxParamType(ImplementPtr);
            mParamCount = ConversationAPI.GetFunctionStackParamCount(ImplementPtr);
            for (int i = 0; i < ParamCount; i++)
            {
                mParamTypes.Add(ConversationAPI.GetFunctionStackParamTypeByIndex(ImplementPtr, i));
            }
        }

        internal IntPtr ImplementPtr { get { return mImplementPtr; } }

        public int ParamCount { get { return mParamCount; } }

        public TValue PreferReturnType { get { return mReturnType; } }

        public TValue MaxParamType { get { return mMaxParamType; } }

        public TValue GetParamType(int index)
        {
            if (index >= 0 && index < ParamCount)
            {
                return mParamTypes[index];
            }
            else
            {
                return TValue.Invalid;
            }
        }

        public bool GetParamAsBool(int index)
        {
            if (index >= 0 && index < ParamCount)
            {
                return ConversationAPI.GetFunctionStackParamAsBool(ImplementPtr, index);
            }
            else
            {
                return false;
            }
        }

        public int GetParamAsInt(int index)
        {
            if (index >= 0 && index < ParamCount)
            {
                return ConversationAPI.GetFunctionStackParamAsInt(ImplementPtr, index);
            }
            else
            {
                return 0;
            }
        }

        public float GetParamAsFloat(int index)
        {
            if (index >= 0 && index < ParamCount)
            {
                return ConversationAPI.GetFunctionStackParamAsFloat(ImplementPtr, index);
            }
            else
            {
                return 0.0f;
            }
        }

        public bool ReturnValueB
        {
            set { ConversationAPI.SetFunctionStackReturnValue(ImplementPtr, value); }
        }

        public int ReturnValueI
        {
            set { ConversationAPI.SetFunctionStackReturnValue(ImplementPtr, value); }
        }

        public float ReturnValueF
        {
            set { ConversationAPI.SetFunctionStackReturnValue(ImplementPtr, value); }
        }
    }
}