class ScriptInvokerTests : TestFramework
{
	ref ScriptInvoker m_Invoker;
	int m_InvokeCount;
	
	//---------------------------------------------------------------------------
	// Ctor - Decides the tests to run
	//---------------------------------------------------------------------------
	void ScriptInvokerTests()
	{
		m_Invoker = new ScriptInvoker();
		
		//AddInitTest("TestFirstUnique");
		//AddInitTest("TestSecondUnique");
		//AddInitTest("TestInsertRemoveUnique");
		//AddInitTest("TestInsertUniqueImmediate");
		//AddInitTest("TestClearRunning");
		//AddInitTest("TestInvokeRunning");
		AddInitTest("TestInsertRunning");
	}
	
	//---------------------------------------------------------------------------
	// Dtor
	//---------------------------------------------------------------------------
	void ~ScriptInvokerTests()
	{
		
	}
	
	//---------------------------------------------------------------------------
	// Tests
	//---------------------------------------------------------------------------
	// Test first insert flagged as unique
	TFResult TestFirstUnique()
	{
		InvokeReset();
		
		bool insert1 = m_Invoker.Insert(InvokeLog, EScriptInvokerInsertFlags.UNIQUE);
		Assert(insert1);
		bool insert2 = m_Invoker.Insert(InvokeLog);
		Assert(!insert2);
		
		m_Invoker.Invoke("TestFirstUnique");
		bool count = Assert(m_InvokeCount == 1);
		
		InvokeReset();
		
		return BTFR(insert1 && !insert2 && count);
	}
	
	//---------------------------------------------------------------------------
	// Test second insert flagged as unique
	TFResult TestSecondUnique()
	{
		InvokeReset();
		
		bool insert1 = m_Invoker.Insert(InvokeLog);
		Assert(insert1);
		bool insert2 = m_Invoker.Insert(InvokeLog, EScriptInvokerInsertFlags.UNIQUE);
		Assert(!insert2);
		
		m_Invoker.Invoke("TestSecondUnique");
		bool count = Assert(m_InvokeCount == 1);
		
		InvokeReset();
		
		return BTFR(insert1 && !insert2 && count);
	}
	
	//---------------------------------------------------------------------------
	// Test inserting and removing of unique
	TFResult TestInsertRemoveUnique()
	{
		InvokeReset();
		
		bool insert1 = m_Invoker.Insert(InvokeLog, EScriptInvokerInsertFlags.UNIQUE);
		Assert(insert1);
		bool remove1 = m_Invoker.Remove(InvokeLog);
		Assert(remove1);
		bool insert2 = m_Invoker.Insert(InvokeLog);
		Assert(insert2);
		
		m_Invoker.Invoke("TestInsertRemoveUnique");
		bool count = Assert(m_InvokeCount == 1);
		
		InvokeReset();
		
		return BTFR(insert1 && remove1 && insert2 && count);
	}
	
	//---------------------------------------------------------------------------
	// Test inserting and of unique and immediate
	TFResult TestInsertUniqueImmediate()
	{
		InvokeReset();
		
		bool insert1 = m_Invoker.Insert(InvokeLog, EScriptInvokerInsertFlags.UNIQUE);
		Assert(insert1);
		bool insert2 = m_Invoker.Insert(InvokeLog, EScriptInvokerInsertFlags.IMMEDIATE);
		Assert(insert2);
		
		m_Invoker.Invoke("TestInsertUniqueImmediate");
		bool count = Assert(m_InvokeCount == 1);
		
		InvokeReset();
		
		return BTFR(insert1 && !insert2 && count);
	}
	
	//---------------------------------------------------------------------------
	// Test clearing while invoking
	TFResult TestClearRunning()
	{
		InvokeReset();
		
		m_Invoker.Insert(InvokeClear);
		m_Invoker.Insert(InvokeLog);
		
		m_Invoker.Invoke("TestClearRunning");
		bool count = Assert(m_InvokeCount == 1);
		
		InvokeReset();
		
		return BTFR(count);
	}
	
	//---------------------------------------------------------------------------
	// Test invoke while invoking (will result in overflow)
	TFResult TestInvokeRunning()
	{
		InvokeReset();
		
		m_Invoker.Insert(InvokeInvoke);
		
		m_Invoker.Invoke("TestInvokeRunning");
		
		InvokeReset();
		
		return BTFR(false); // This can never succeed, it will never even reach this point
	}
	
	//---------------------------------------------------------------------------
	// Test insert while invoking
	TFResult TestInsertRunning()
	{
		InvokeReset();
		
		m_Invoker.Insert(InvokeInsert);
		
		m_Invoker.Invoke("TestInvokeRunning");
		Print(m_InvokeCount);
		bool count = Assert(m_InvokeCount == 129);
		
		InvokeReset();
		
		return BTFR(count);
	}

	
	//---------------------------------------------------------------------------
	// Helpers
	//---------------------------------------------------------------------------
	void InvokeLog(string s)
	{
		//Debug.TFLog(s, this, "InvokeLog");
		++m_InvokeCount;
	}
	
	void InvokeReset()
	{
		m_Invoker.Clear();
		m_InvokeCount = 0;
	}
	
	void InvokeClear(string s)
	{
		InvokeLog(s);
		m_Invoker.Clear();
	}
	
	void InvokeInvoke(string s)
	{
		InvokeLog(s);
		m_Invoker.Invoke(s);
	}
	
	void InvokeInsert(string s)
	{
		InvokeLog(s);
		m_Invoker.Insert(InvokeInsert, EScriptInvokerInsertFlags.IMMEDIATE);
	}
}