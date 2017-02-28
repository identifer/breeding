using System.Collections.Generic;

namespace ConvAPI
{
    public class FunctionTable
    {
        class FunctionList : List<FunctionAdapter>, IComparer<FunctionAdapter>
        {
            public int Compare(FunctionAdapter x, FunctionAdapter y)
            {
                if (x == null && y == null) return 0;
                if (x == null) return -1;
                if (y == null) return 1;
                if (x == y) return 0;
                if (x < y) return -1;
                return 1;
            }

            public void Insert(FunctionAdapter adapter)
            {
                if (Count == 0)
                {
                    Add(adapter);
                }
                else
                {
                    /*
                     * The zero-based index of item in the sorted List<T>, if item is found; 
                     * otherwise, a negative number that is the bitwise complement of the index of the next element
                     * that is larger than item or, if there is no larger element, the bitwise complement of Count.
                     */
                    int index = BinarySearch(adapter, this);
                    if (index < 0)
                    {
                        Insert(~index, adapter);
                    }
                    else
                    {
                        this[index] = adapter;
                    }
                }
            }
        }

        Dictionary<string, FunctionList> mProcessTable = new Dictionary<string, FunctionList>();
        Dictionary<string, FunctionList> mFunctionTable = new Dictionary<string, FunctionList>();

        public void Register(FunctionAdapter adapter, string aliasName)
        {
            if (adapter == null || aliasName == null)
            {
                return;
            }

            if (adapter.HaveReturnValue)
            {
                RegisterFunction(aliasName, adapter);
            }
            else
            {
                RegisterProcess(aliasName, adapter);
            }
        }

        public bool InvokeFunction(string name, FunctionStack stack)
        {
            return Invoke(name, stack, mFunctionTable);
        }

        public bool InvokeProcess(string name, FunctionStack stack)
        {
            return Invoke(name, stack, mProcessTable);
        }

        public bool HaveProcess(string name)
        {
            return mProcessTable.ContainsKey(name) && mProcessTable[name].Count > 0;
        }

        public bool HaveFunction(string name)
        {
            return mFunctionTable.ContainsKey(name) && mFunctionTable[name].Count > 0;
        }

        void RegisterProcess(string name, FunctionAdapter adapter)
        {
            if (!mProcessTable.ContainsKey(name))
            {
                mProcessTable.Add(name, new FunctionList());
            }

            mProcessTable[name].Insert(adapter);
        }

        void RegisterFunction(string name, FunctionAdapter adapter)
        {
            if (!mFunctionTable.ContainsKey(name))
            {
                mFunctionTable.Add(name, new FunctionList());
            }

            mFunctionTable[name].Insert(adapter);
        }

        bool Invoke(string name, FunctionStack stack, Dictionary<string, FunctionList> table)
        {
            if (table.ContainsKey(name))
            {
                FunctionList list = table[name];
                foreach (FunctionAdapter adapter in list)
                {
                    if (adapter.IsSuitable(stack))
                    {
                        adapter.Invoke(stack);
                        return true;
                    }
                }
            }

            return false;
        }
    }

}
