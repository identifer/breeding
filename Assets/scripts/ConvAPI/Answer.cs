using System;

namespace ConvAPI
{
    public class Answer
    {
        private IntPtr mImplementPtr = IntPtr.Zero;
        private int mID = 0;
        private string mText = "";

        internal Answer(IntPtr implPtr)
        {
            mImplementPtr = implPtr;
            mID = ConversationAPI.GetAnswerID(mImplementPtr);
            mText = ConversationAPI.GetAnswerText(mImplementPtr);
        }

        internal IntPtr ImplementPtr { get { return mImplementPtr; } }

        public int ID { get { return mID; } }

        public string Text { get { return mText; } }

        public bool IsAvailable
        {
            get { return ConversationAPI.IsAnswerAvailable(mImplementPtr); }
        }
    }
}