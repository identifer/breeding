using System;

namespace ConvAPI
{
    public class Question
    {
        private string mName = "";
        private string mText = "";
        private Answer[] mAnswerList = null;

        private IntPtr mImplementPtr = IntPtr.Zero;

        internal Question(IntPtr implPtr)
        {
            mImplementPtr = implPtr;
            mName = ConversationAPI.GetQuestionName(Implement);
            mText = ConversationAPI.GetQuestionText(Implement);
            int answerCount = ConversationAPI.GetQuestionAnswerCount(Implement);
            mAnswerList = new Answer[answerCount];
            for (int i = 0; i < answerCount; i++)
            {
                IntPtr answerPtr = ConversationAPI.GetQuestionAnswerByIndex(mImplementPtr, i);
                if (answerPtr != IntPtr.Zero)
                    mAnswerList[i] = new Answer(answerPtr);
                else
                    mAnswerList[i] = null;
            }
        }

        internal IntPtr Implement { get { return mImplementPtr; } }

        public string Name { get { return mName; } }

        public string Text { get { return mText; } }

        public int AnswerCount { get { return mAnswerList.Length; } }

        public Answer GetAnswerByIndex(int index)
        {
            if (index >= 0 && index < AnswerCount)
            {
                return mAnswerList[index];
            }
            return null;
        }
    }
}