using System;
using System.Reflection;
using System.Collections.Generic;

namespace ConvAPI
{
    public class FunctionAdapter : IEquatable<FunctionAdapter>
    {
        enum TFunction { Fixed, MoreEqual, LessEqual, Range };

        MethodInfo mFunction;
        TFunction mFunctionType;
        TValue mReturnType;

        //Flexible Function
        TValue mMaxParamType = TValue.None;
        int mParamCountMin = 0;
        int mParamCountMax = 0;

        //Fixed Function
        List<TValue> mParamTypes = null;
        object[] mParamList = null;

        FunctionAdapter(MethodInfo function, TValue returnType, List<TValue> paramList)
        {
            mFunction = function;
            mFunctionType = TFunction.Fixed;
            mReturnType = returnType;
            mParamTypes = paramList;
            if (ParamCount > 0)
            {
                mParamList = new object[ParamCount];
            }
            else
            {
                paramList = null;
            }
        }

        FunctionAdapter(MethodInfo function, TFunction funtionType, TValue returnType, TValue maxParamType, int paramCountMin, int paramCountMax)
        {
            mFunction = function;
            mFunctionType = funtionType;
            mReturnType = returnType;
            mMaxParamType = maxParamType;
            mParamCountMin = paramCountMin;
            mParamCountMax = paramCountMax;
            mParamList = new object[1];
        }

        public bool IsSuitable(FunctionStack stack)
        {
            if (IsFixed)
            {
                if (ParamCount != stack.ParamCount)
                {
                    return false;
                }
                for (int i = 0, n = ParamCount; i < n; i++)
                {
                    if (mParamTypes[i] < stack.GetParamType(i))
                    {
                        return false;
                    }
                }
            }
            else
            {
                if (stack.MaxParamType > mMaxParamType)
                {
                    return false;
                }

                if (mFunctionType == TFunction.MoreEqual &&
                    stack.ParamCount < mParamCountMin)
                {
                    return false;
                }
                else if (mFunctionType == TFunction.LessEqual &&
                    stack.ParamCount > mParamCountMin)
                {
                    return false;
                }
                else if (mFunctionType == TFunction.Range &&
                    (stack.ParamCount < mParamCountMin ||
                    stack.ParamCount > mParamCountMax))
                {
                    return false;
                }
            }

            if (mReturnType < stack.PreferReturnType)
            {
                return false;
            }

            return true;
        }

        public void Invoke(FunctionStack stack)
        {
            if (IsFixed)
            {
                //构造参数列表
                if (mParamList != null)
                {
                    for (int i = 0, n = ParamCount; i < n; i++)
                    {
                        mParamList[i] = GetParam(stack, i);
                    }
                }
            }
            else
            {
                mParamList[0] = stack;
            }

            //函数调用
            if (HaveReturnValue)
            {
                object returnValue = mFunction.Invoke(null, mParamList);
                SetReturnValue(returnValue, stack);
            }
            else
            {
                mFunction.Invoke(null, mParamList);
            }
        }

        public string Name { get { return mFunction.Name; } }

        public bool HaveReturnValue { get { return mReturnType != TValue.None; } }

        public bool HaveParameters { get { return mParamTypes.Count != 0; } }

        public static FunctionAdapter[] CreateAll(Type belongClass)
        {
            MethodInfo[] methods = belongClass.GetMethods(BindingFlags.Static | BindingFlags.Public | BindingFlags.NonPublic);
            List<FunctionAdapter> list = new List<FunctionAdapter>();
            foreach (MethodInfo info in methods)
            {
                TValue retType = ToValueType(info.ReturnType);
                if (retType == TValue.Invalid)
                {
                    continue;
                }

                bool successed = true;
                ParameterInfo[] paramList = info.GetParameters();
                List<TValue> paramTypes = new List<TValue>();
                foreach (ParameterInfo p in paramList)
                {
                    TValue pType = ToValueType(p.ParameterType);
                    if (pType == TValue.Invalid)
                    {
                        successed = false;
                        break;
                    }

                    paramTypes.Add(pType);
                }

                if (!successed)
                {
                    continue;
                }

                FunctionAdapter adapter = new FunctionAdapter(info, retType, paramTypes);
                list.Add(adapter);
            }
            return list.ToArray();
        }

        public static FunctionAdapter Create(Type belongClass, string functionName)
        {
            MethodInfo method = GetFixedFunction(belongClass, functionName);
            if (method == null)
            {
                return null;
            }

            ParameterInfo[] paramList = method.GetParameters();
            List<TValue> paramTypes = new List<TValue>();
            foreach (ParameterInfo p in paramList)
            {
                TValue pType = ToValueType(p.ParameterType);
                if (pType == TValue.Invalid)
                {
                    return null;
                }

                paramTypes.Add(pType);
            }

            FunctionAdapter adapter = new FunctionAdapter(method, ToValueType(method.ReturnType), paramTypes);
            return adapter;
        }

        public static FunctionAdapter CreateMoreEqual(Type belongClass, string functionName, TValue maxParamType, int paramCount)
        {
            if (maxParamType == TValue.None)
            {
                return null;
            }

            MethodInfo method = GetFunctionStackCallback(belongClass, functionName);
            if (method == null)
            {
                return null;
            }

            FunctionAdapter adapter = new FunctionAdapter(method, TFunction.MoreEqual, ToValueType(method.ReturnType), maxParamType, paramCount, paramCount);
            return adapter;

        }

        public static FunctionAdapter CreateLessEqual(Type belongClass, string functionName, TValue maxParamType, int paramCount)
        {
            if (maxParamType == TValue.None || paramCount == 0)
            {
                return null;
            }

            MethodInfo method = GetFunctionStackCallback(belongClass, functionName);
            if (method == null)
            {
                return null;
            }

            FunctionAdapter adapter = new FunctionAdapter(method, TFunction.LessEqual, ToValueType(method.ReturnType), maxParamType, paramCount, paramCount);
            return adapter;
        }

        public static FunctionAdapter CreateRange(Type belongClass, string functionName, TValue maxParamType, int paramCountMin, int paramCountMax)
        {
            if (maxParamType == TValue.None || paramCountMin >= paramCountMax || paramCountMin == 0)
            {
                return null;
            }

            MethodInfo method = GetFunctionStackCallback(belongClass, functionName);
            if (method == null)
            {
                return null;
            }

            FunctionAdapter adapter = new FunctionAdapter(method, TFunction.Range, ToValueType(method.ReturnType), maxParamType, paramCountMin, paramCountMax);
            return adapter;
        }


        int ParamCount { get { return mParamTypes.Count; } }

        TValue ReturnType { get { return mReturnType; } }

        TValue MaxParamType { get { return mMaxParamType; } }

        bool IsFixed { get { return mFunctionType == TFunction.Fixed; } }

        object GetParam(FunctionStack stack, int index)
        {
            switch (mParamTypes[index])
            {
                default: return null;
                case TValue.Boolean: return stack.GetParamAsBool(index);
                case TValue.Int: return stack.GetParamAsInt(index);
                case TValue.Float: return stack.GetParamAsFloat(index);
            }
        }

        void SetReturnValue(object returnValue, FunctionStack stack)
        {
            if (returnValue != null && returnValue.GetType() != typeof(void))
            {
                TValue retType = ToValueType(returnValue.GetType());
                switch (retType)
                {
                    case TValue.Boolean:
                        stack.ReturnValueB = (bool)returnValue;
                        break;
                    case TValue.Int:
                        stack.ReturnValueI = (int)returnValue;
                        break;
                    case TValue.Float:
                        stack.ReturnValueF = (float)returnValue;
                        break;
                }
            }
        }

        static MethodInfo GetFunctionStackCallback(Type belongs, string name)
        {
            MethodInfo method = belongs.GetMethod(name, BindingFlags.Static | BindingFlags.NonPublic | BindingFlags.Public);
            if (method == null)
            {
                return null;
            }

            ParameterInfo[] paramList = method.GetParameters();
            if (paramList.Length == 1 && paramList[0].ParameterType == typeof(FunctionStack))
            {
                TValue retType = ToValueType(method.ReturnType);
                if (retType != TValue.Invalid)
                {
                    return method;
                }
                else
                {
                    return null;
                }
            }
            else
            {
                return null;
            }
        }

        static MethodInfo GetFixedFunction(Type belongs, string name)
        {
            MethodInfo method = belongs.GetMethod(name, BindingFlags.Static | BindingFlags.NonPublic | BindingFlags.Public);
            if (method == null)
            {
                return null;
            }

            TValue retType = ToValueType(method.ReturnType);
            if (retType != TValue.Invalid)
            {
                return method;
            }
            else
            {
                return null;
            }
        }

        static TValue ToValueType(Type t)
        {
            if (t == typeof(void)) return TValue.None;
            if (t == typeof(bool)) return TValue.Boolean;
            if (t == typeof(int)) return TValue.Int;
            if (t == typeof(float)) return TValue.Float;
            return TValue.Invalid;
        }

        public bool Equals(FunctionAdapter other)
        {
            if (other == null ||
                mFunctionType != other.mFunctionType ||
                mReturnType != other.mReturnType)
            {
                return false;
            }

            if (IsFixed)
            {
                if (ParamCount == other.ParamCount)
                {
                    for (int i = 0, n = ParamCount; i < n; i++)
                    {
                        if (mParamTypes[i] != other.mParamTypes[i])
                        {
                            return false;
                        }
                    }
                    return true;
                }
                return false;
            }
            else
            {
                return mMaxParamType == other.mMaxParamType &&
                    mParamCountMin == other.mParamCountMin &&
                    mParamCountMax == other.mParamCountMax;
            }
        }

        public static bool operator <(FunctionAdapter lhs, FunctionAdapter rhs)
        {
            if (lhs.mFunctionType < rhs.mFunctionType)
            {
                return true;
            }
            if (lhs.mFunctionType > rhs.mFunctionType)
            {
                return false;
            }

            if (lhs.IsFixed)
            {
                var lp = lhs.mParamTypes;
                var rp = rhs.mParamTypes;

                if (lp.Count < rp.Count)
                {
                    return true;
                }

                if (lp.Count > rp.Count)
                {
                    return false;
                }

                for (int i = 0, n = lp.Count; i < n; i++)
                {
                    if (lp[i] < rp[i])
                    {
                        return true;
                    }

                    if (lp[i] > rp[i])
                    {
                        return false;
                    }
                }
            }
            else
            {
                if (lhs.mFunctionType != TFunction.Range)
                {
                    if (lhs.mParamCountMin < rhs.mParamCountMin)
                    {
                        return true;
                    }

                    if (lhs.mParamCountMin > rhs.mParamCountMin)
                    {
                        return true;
                    }
                }
                else
                {

                    int lcount = lhs.mParamCountMax - lhs.mParamCountMin;
                    int rcount = rhs.mParamCountMax - rhs.mParamCountMin;

                    if (lcount < rcount)
                    {
                        return true;
                    }

                    if (lcount > rcount)
                    {
                        return false;
                    }

                    if (lhs.mParamCountMin > rhs.mParamCountMin)
                    {
                        return true;
                    }

                    if (lhs.mParamCountMin < rhs.mParamCountMin)
                    {
                        return true;
                    }

                    if (lhs.mParamCountMax < rhs.mParamCountMax)
                    {
                        return true;
                    }

                    if (lhs.mParamCountMax > rhs.mParamCountMax)
                    {
                        return true;
                    }
                }
            }

            if (lhs.ReturnType < rhs.ReturnType)
            {
                return true;
            }

            if (lhs.ReturnType > rhs.ReturnType)
            {
                return false;
            }

            return false;
        }

        public static bool operator >(FunctionAdapter lhs, FunctionAdapter rhs)
        {
            if (lhs.mFunctionType > rhs.mFunctionType)
            {
                return true;
            }
            if (lhs.mFunctionType < rhs.mFunctionType)
            {
                return false;
            }

            if (lhs.IsFixed)
            {
                var lp = lhs.mParamTypes;
                var rp = rhs.mParamTypes;

                if (lp.Count > rp.Count)
                {
                    return true;
                }

                if (lp.Count < rp.Count)
                {
                    return false;
                }

                for (int i = 0, n = lp.Count; i < n; i++)
                {
                    if (lp[i] > rp[i])
                    {
                        return true;
                    }

                    if (lp[i] < rp[i])
                    {
                        return false;
                    }
                }
            }
            else
            {
                if (lhs.mFunctionType != TFunction.Range)
                {
                    if (lhs.mParamCountMin > rhs.mParamCountMin)
                    {
                        return true;
                    }

                    if (lhs.mParamCountMin < rhs.mParamCountMin)
                    {
                        return true;
                    }
                }
                else
                {

                    int lcount = lhs.mParamCountMax - lhs.mParamCountMin;
                    int rcount = rhs.mParamCountMax - rhs.mParamCountMin;

                    if (lcount > rcount)
                    {
                        return true;
                    }

                    if (lcount < rcount)
                    {
                        return false;
                    }

                    if (lhs.mParamCountMin > rhs.mParamCountMin)
                    {
                        return true;
                    }

                    if (lhs.mParamCountMin < rhs.mParamCountMin)
                    {
                        return true;
                    }

                    if (lhs.mParamCountMax < rhs.mParamCountMax)
                    {
                        return true;
                    }

                    if (lhs.mParamCountMax > rhs.mParamCountMax)
                    {
                        return true;
                    }
                }
            }

            if (lhs.ReturnType > rhs.ReturnType)
            {
                return true;
            }

            if (lhs.ReturnType < rhs.ReturnType)
            {
                return false;
            }

            return false;
        }

        public override int GetHashCode()
        {
            var hashCode = 1599767462;
            hashCode = hashCode * -1521134295 + EqualityComparer<MethodInfo>.Default.GetHashCode(mFunction);
            hashCode = hashCode * -1521134295 + mReturnType.GetHashCode();
            hashCode = hashCode * -1521134295 + EqualityComparer<List<TValue>>.Default.GetHashCode(mParamTypes);
            return hashCode;
        }
    }
}