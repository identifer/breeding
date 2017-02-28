using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;

namespace ConvAPI
{
    public class Save : IDisposable
    {
        private Dictionary<string, Value> mValueTable = new Dictionary<string, Value>();

        private IntPtr mImplementPtr = IntPtr.Zero;

        internal IntPtr ImplementPtr { get { return mImplementPtr; } }

        internal Save(IntPtr implPtr)
        {
            mImplementPtr = implPtr;
            disposedValue = true;
        }

        public Save()
        {
            mImplementPtr = ConversationAPI.CreateCustomSave();
        }

        bool HasValue(string name)
        {
            if (name == null || name == "")
            {
                return false;
            }

            if (mValueTable.ContainsKey(name))
            {
                return true;
            }

            return ConversationAPI.SaveHasValue(ImplementPtr, name);
        }

        public int ValueCount { get { return ConversationAPI.GetSaveValueCount(ImplementPtr); } }

        public string GetValueNameByIndex(int index)
        {
            string name = ConversationAPI.GetSaveValueNameByIndex(ImplementPtr, index);
            return name;
        }

        public Value GetValue(string name)
        {
            if (HasValue(name))
            {
                return GetValueFromCache(name);
            }
            return null;
        }

        public void Set(string name, bool value, bool readOnly)
        {
            Value v = GetValueFromCache(name);
            v.Boolean = value;
            v.ReadOnly = readOnly;
        }

        public void Set(string name, int value, bool readOnly)
        {
            Value v = GetValueFromCache(name);
            v.Int = value;
            v.ReadOnly = readOnly;
        }

        public void Set(string name, float value)
        {
            Value v = GetValueFromCache(name);
            v.Float = value;
        }

        public void Set(string name, bool value)
        {
            Value v = GetValueFromCache(name);
            v.Boolean = value;
        }

        public void Set(string name, int value)
        {
            Value v = GetValueFromCache(name);
            v.Int = value;
        }

        public void Set(string name, float value, bool readOnly)
        {
            Value v = GetValueFromCache(name);
            v.Float = value;
            v.ReadOnly = readOnly;
        }

        Value GetValueFromCache(string name)
        {
            if (!mValueTable.ContainsKey(name))
            {
                mValueTable.Add(name, new Value(name, this));
            }
            return mValueTable[name];
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
                if (mImplementPtr != IntPtr.Zero)
                {
                    ConversationAPI.ReleaseCustomSave(mImplementPtr);
                    mImplementPtr = IntPtr.Zero;
                }
                disposedValue = true;
            }
        }

        // TODO: 仅当以上 Dispose(bool disposing) 拥有用于释放未托管资源的代码时才替代终结器。
        ~Save()
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

        #region ISerializable Support
        public void SaveToFile(string fileName)
        {
            IFormatter formatter = new BinaryFormatter();
            FileStream fileStream = new FileStream(fileName, FileMode.Create, FileAccess.Write);
            SaveBinary sb = new SaveBinary(this);
            formatter.Serialize(fileStream, sb);
            fileStream.Close();
        }

        public void LoadFromFile(string fileName)
        {
            IFormatter formatter = new BinaryFormatter();
            FileStream fileStream = new FileStream(fileName, FileMode.Open, FileAccess.Read, FileShare.Read);
            SaveBinary sb = (SaveBinary)formatter.Deserialize(fileStream);
            CopyFrom(sb);
            fileStream.Close();
        }

        [Serializable]
        class ValueBinary : ISerializable
        {
            [field: NonSerialized]
            public string Name = "";

            [field: NonSerialized]
            public TValue Type = TValue.Invalid;

            [field: NonSerialized]
            public bool bValue = false;

            [field: NonSerialized]
            public int iValue = 0;

            [field: NonSerialized]
            public float fValue = 0.0f;

            public ValueBinary(string name, bool value)
            {
                Name = name;
                Type = TValue.Boolean;
                bValue = value;
            }
            public ValueBinary(string name, int value)
            {
                Name = name;
                Type = TValue.Int;
                iValue = value;
            }
            public ValueBinary(string name, float value)
            {
                Name = name;
                Type = TValue.Float;
                fValue = value;
            }
            
            protected ValueBinary(SerializationInfo info, StreamingContext context)
            {
                // Reset the property value using the GetValue method.
                Name = info.GetString("Name");
                Type = (TValue)info.GetValue("Type", typeof(TValue));
                if (Type == TValue.Boolean)
                {
                    bValue = info.GetBoolean("bValue");
                }
                else if (Type == TValue.Int)
                {
                    iValue = info.GetInt32("iValue");
                }
                else if (Type == TValue.Float)
                {
                    fValue = info.GetSingle("fValue");
                }
            }

            public void GetObjectData(SerializationInfo info, StreamingContext context)
            {
                if (Type == TValue.None || Type == TValue.Invalid)
                {
                    return;
                }

                info.AddValue("Name", Name);
                info.AddValue("Type", Type, typeof(TValue));
                if (Type == TValue.Boolean)
                {
                    info.AddValue("bValue", bValue);
                }
                else if (Type == TValue.Int)
                {
                    info.AddValue("iValue", iValue);
                }
                else if (Type == TValue.Float)
                {
                    info.AddValue("fValue", fValue);
                }
            }
        }

        [Serializable]
        class SaveBinary : ISerializable
        {
            [field: NonSerialized]
            public List<ValueBinary> Values = new List<ValueBinary>();

            public SaveBinary(Save save)
            {
                int count = save.ValueCount;
                for (int i = 0; i < count; i++)
                {
                    string name = save.GetValueNameByIndex(i);
                    Value v = save.GetValue(name);
                    if (v != null)
                    {
                        ValueBinary vb = null;
                        if (v.ValueType == TValue.Boolean)
                        {
                            vb = new ValueBinary(name, v.Boolean);
                        }
                        else if (v.ValueType == TValue.Int)
                        {
                            vb = new ValueBinary(name, v.Int);
                        }
                        else if (v.ValueType == TValue.Float)
                        {
                            vb = new ValueBinary(name, v.Float);
                        }

                        if (vb != null)
                        {
                            Values.Add(vb);
                        }
                    }
                }
            }
            
            protected SaveBinary(SerializationInfo info, StreamingContext context)
            {
                int count = info.GetInt32("Value.Count");
                for (int i = 0; i < count; i++)
                {
                    ValueBinary v = (ValueBinary)info.GetValue("Element." + i, typeof(ValueBinary));
                    Values.Add(v);
                }
            }

            public void GetObjectData(SerializationInfo info, StreamingContext context)
            {
                info.AddValue("Value.Count", Values.Count);
                for (int i = 0, n = Values.Count; i < n; i++)
                {
                    info.AddValue("Element." + i, Values[i], typeof(ValueBinary));
                }

            }
        }

        void CopyFrom(SaveBinary binary)
        {
            foreach(ValueBinary vb in binary.Values)
            {
                if (vb.Type == TValue.Boolean)
                {
                    ConversationAPI.SetSaveValue(ImplementPtr, vb.Name, vb.bValue);
                }
                else if (vb.Type == TValue.Int)
                {
                    ConversationAPI.SetSaveValue(ImplementPtr, vb.Name, vb.iValue);
                }
                else if (vb.Type == TValue.Float)
                {
                    ConversationAPI.SetSaveValue(ImplementPtr, vb.Name, vb.fValue);
                }
            }
        }
        #endregion
    }
}
