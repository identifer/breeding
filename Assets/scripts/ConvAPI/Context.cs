using System;
using AOT;
using System.Runtime.InteropServices;

namespace ConvAPI
{
    public class Context : IDisposable
    {
        // 长期持有自己的delegate防止被gc
        private FunctionHandler mFunctionHandler;
        private FunctionHandler mProcessHandler;
        private FunctionQuery mFunctionQuery;
        private FunctionQuery mProcessQuery;
        private Save mGlobalSave = null;
        private Save mPlayerSave = null;
        private Save mCurrentNPCSave = null;
        private FunctionTable mFunctionTable = new FunctionTable();

        private IntPtr mImplementPtr = IntPtr.Zero;

        internal Context()
        {
            mImplementPtr = ConversationAPI.CreateContext();

            IntPtr global = ConversationAPI.GetContextGlobalSave(ImplementPtr);
            if (global != IntPtr.Zero)
            {
                mGlobalSave = new Save(global);
            }

            IntPtr player = ConversationAPI.GetContextPlayerSave(ImplementPtr);
            if (player != IntPtr.Zero)
            {
                mPlayerSave = new Save(player);
            }

            // 长期持有自己的delegate防止被gc
            mFunctionHandler = new FunctionHandler(OnFunctoinCall);
            mProcessHandler = new FunctionHandler(OnProcessCall);
            mFunctionQuery = new FunctionQuery(OnFunctoinQuery);
            mProcessQuery = new FunctionQuery(OnProcessQuery);

            ConversationAPI.SetFunctionHandler(ImplementPtr, mFunctionHandler);
            ConversationAPI.SetProcessHandler(ImplementPtr, mProcessHandler);
            ConversationAPI.SetFunctionQuery(ImplementPtr, mFunctionQuery);
            ConversationAPI.SetProcessQuery(ImplementPtr, mProcessQuery);
        }

        internal IntPtr ImplementPtr { get { return mImplementPtr; } }
        
        public Save GlobalSave
        {
            get
            {
                if (mGlobalSave == null)
                {
                    IntPtr global = ConversationAPI.GetContextGlobalSave(ImplementPtr);
                    if (global != IntPtr.Zero)
                    {
                        mGlobalSave = new Save(global);
                    }
                }
                return mGlobalSave;
            }
        }

        public Save PlayerSave
        {
            get
            {
                if (mPlayerSave == null)
                {
                    IntPtr player = ConversationAPI.GetContextPlayerSave(ImplementPtr);
                    if (player != IntPtr.Zero)
                    {
                        mPlayerSave = new Save(player);
                    }
                }
                return mPlayerSave;
            }
        }

        public Save ConversationSave
        {
            get
            {
                IntPtr conv = ConversationAPI.GetContextConversationSave(ImplementPtr);
                if (conv != IntPtr.Zero)
                {
                    return new Save(conv);
                }
                else
                {
                    return null;
                }
            }
        }

        public Save NPCSave
        {
            get { return mCurrentNPCSave; }

            set
            {
                mCurrentNPCSave = value;
                ConversationAPI.SetContextNPCSave(ImplementPtr, value.ImplementPtr);
            }
        }

        public void Register(FunctionAdapter adapter, string aliasName)
        {
            mFunctionTable.Register(adapter, aliasName);
        }
        
        public void RegisterAllFixed(Type belongs)
        {
            var list = FunctionAdapter.CreateAll(belongs);
            foreach (FunctionAdapter adapter in list)
            {
                mFunctionTable.Register(adapter, adapter.Name);
            }
        }

        [MonoPInvokeCallbackAttribute(typeof(FunctionHandler))]
        [return: MarshalAs(UnmanagedType.I4)]
        bool OnProcessCall(string name, IntPtr pFunctionStack)
        {
            if (pFunctionStack == IntPtr.Zero)
            {
                return false;
            }
            FunctionStack stack = new FunctionStack(pFunctionStack);
            return mFunctionTable.InvokeProcess(name, stack);
        }

        [MonoPInvokeCallbackAttribute(typeof(FunctionHandler))]
        [return: MarshalAs(UnmanagedType.I4)]
        bool OnFunctoinCall(string name, IntPtr pFunctionStack)
        {
            if (pFunctionStack == IntPtr.Zero)
            {
                return false;
            }
            FunctionStack stack = new FunctionStack(pFunctionStack);
            return mFunctionTable.InvokeFunction(name, stack);
        }

        [MonoPInvokeCallbackAttribute(typeof(FunctionQuery))]
        [return: MarshalAs(UnmanagedType.I4)]
        bool OnProcessQuery(string name)
        {
            return mFunctionTable.HaveProcess(name);
        }

        [MonoPInvokeCallbackAttribute(typeof(FunctionQuery))]
        [return: MarshalAs(UnmanagedType.I4)]
        bool OnFunctoinQuery(string name)
        {
            return mFunctionTable.HaveFunction(name);
        }

        #region IDisposable Support
        private bool disposedValue = false; // 要检测冗余调用

        protected virtual void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                if (disposing)
                {
                    // TODO: 释放托管状态(托管对象)。
                }

                if (ImplementPtr != IntPtr.Zero)
                {
                    ConversationAPI.ReleaseContext(ImplementPtr);
                    mImplementPtr = IntPtr.Zero;
                }
                disposedValue = true;
            }
        }

        // TODO: 仅当以上 Dispose(bool disposing) 拥有用于释放未托管资源的代码时才替代终结器。
        ~Context()
        {
            // 请勿更改此代码。将清理代码放入以上 Dispose(bool disposing) 中。
            Dispose(false);
        }

        // 添加此代码以正确实现可处置模式。
        public void Dispose()
        {
            // 请勿更改此代码。将清理代码放入以上 Dispose(bool disposing) 中。
            Dispose(true);
            // TODO: 如果在以上内容中替代了终结器，则取消注释以下行。
            GC.SuppressFinalize(this);
        }
        #endregion

    }
}