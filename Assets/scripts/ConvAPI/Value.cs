namespace ConvAPI
{
    public class Value
    {
        private Save mSave = null;
        private string mName = "";

        internal Value(string name, Save save)
        {
            mName = name;
            mSave = save;
        }

        public TValue ValueType
        {
            get
            {
                return ConversationAPI.GetSaveValueType(mSave.ImplementPtr, mName);
            }
        }

        public bool IsNone { get { return ValueType == TValue.None; } }

        public bool IsInt { get { return ValueType == TValue.Int; } }

        public bool IsBoolean { get { return ValueType == TValue.Boolean; } }

        public bool IsFloat { get { return ValueType == TValue.Float; } }

        public int Int
        {
            get { return ConversationAPI.GetSaveValueAsInt(mSave.ImplementPtr, mName); }

            set { ConversationAPI.SetSaveValue(mSave.ImplementPtr, mName, value); }
        }

        public bool Boolean
        {
            get { return ConversationAPI.GetSaveValueAsBoolean(mSave.ImplementPtr, mName); }

            set { ConversationAPI.SetSaveValue(mSave.ImplementPtr, mName, value); }
        }

        public float Float
        {
            get { return ConversationAPI.GetSaveValueAsFloat(mSave.ImplementPtr, mName); }

            set { ConversationAPI.SetSaveValue(mSave.ImplementPtr, mName, value); }
        }

        public bool ReadOnly
        {
            set { ConversationAPI.SetSaveValueReadOnly(mSave.ImplementPtr, mName, value); }
        }
        public int ToInt()
        {
            return ConversationAPI.GetSaveValueToInt(mSave.ImplementPtr, mName);
        }

        public bool ToBoolean()
        {
            return ConversationAPI.GetSaveValueToBoolean(mSave.ImplementPtr, mName) == 1;
        }

        public float ToFloat()
        {
            return ConversationAPI.GetSaveValueToFloat(mSave.ImplementPtr, mName);
        }
    }
}
