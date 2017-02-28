using System;
using System.Collections.Generic;

namespace ConvAPI
{
    public class Conversation : IDisposable
    {
        
        private IntPtr mImplementPtr = IntPtr.Zero;
        private Save save = null;
        private Dictionary<IntPtr, Question> questions = new Dictionary<IntPtr, Question>();
        
        internal Conversation(IntPtr implPtr)
        {
            mImplementPtr = implPtr;

            save = new Save(ConversationAPI.GetConversationSave(ImplementPtr));
        }

        internal IntPtr ImplementPtr { get { return mImplementPtr; } }

        static public Conversation CreateFromXML(string filePath)
        {
            IntPtr convPtr = ConversationAPI.CreateConversationFromXML(filePath);
            return (convPtr != IntPtr.Zero) ? (new Conversation(convPtr)) : null;
        }
        
        public Question StartConversation(Context context)
        {
            IntPtr questionPtr = ConversationAPI.StartConversation(context.ImplementPtr, ImplementPtr);
            return GetQuestion(questionPtr);
        }

        public Question SelectNextConversationBranch(Answer selectedAnswer)
        {
            IntPtr questionPtr = ConversationAPI.SelectNextConversationBranch(mImplementPtr, selectedAnswer.ImplementPtr);
            return GetQuestion(questionPtr);
        }

        public Save Save
        {
            get { return save; }
        }

        Question GetQuestion(IntPtr ptr)
        {
            if (ptr == IntPtr.Zero)
            {
                return null;
            }

            if (!questions.ContainsKey(ptr))
            {
                questions.Add(ptr, new Question(ptr));
            }

            return questions[ptr];
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

                // TODO: 释放未托管的资源(未托管的对象)并在以下内容中替代终结器。
                // TODO: 将大型字段设置为 null。
                if (mImplementPtr != IntPtr.Zero)
                {
                    ConversationAPI.ReleaseConversation(mImplementPtr);
                    mImplementPtr = IntPtr.Zero;
                    questions.Clear();
                }
                disposedValue = true;
            }
        }

        // TODO: 仅当以上 Dispose(bool disposing) 拥有用于释放未托管资源的代码时才替代终结器。
        ~Conversation()
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