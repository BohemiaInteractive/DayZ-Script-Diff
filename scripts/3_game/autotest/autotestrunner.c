/*
	This class is just a convenience wrapper for using the actual TestHarness.
*/
class AutotestRunner
{
	private static bool m_IsRunning;
	private static bool m_IsDone;
	
	static bool IsRunning()
    {
    	return m_IsRunning;
	}
        
	static bool IsDone()
	{
		return m_IsDone;
	}
        
	static void Start()
    {
		if (m_IsRunning)
	    {
	    	ErrorEx("TestAutotest already running!");
	        return;
		}
		
		AutoTestFixture.SetWorldName();
	        
	    // Enabled suites from JSON config provided on CLI (autotest param)
	    set<string> enabledSuites = new set<string>();
	    enabledSuites = AutotestConfigHandler.GetSuites();
	        
	    // Iterate through all suites and activate 'enabled' ones (list to be provided by config?)
	    int numSuites = TestHarness.GetNSuites();
		if (numSuites == 0)
		{
			AutoTestFixture.LogRPT("No TestSuites to run.");
			m_IsDone = true;
			return;		
		}

	    for (int i = 0; i < numSuites; ++i)
	    {
			TestSuite suite = TestHarness.GetSuite(i);                                              
			bool isEnabled  = enabledSuites.Find(suite.GetName()) != -1 && suite.IsEnabled();
			suite.SetEnabled(isEnabled);
			//AutoTestFixture.LogRPT(string.Format("Suite '%1' activation state set to: %2", suite.GetName(), isEnabled));
		}
	        
	    // Start running active TestSuite
	    m_IsRunning = true;
	    TestHarness.Begin();            
	}
        
	static void Update(float deltaTime)
	{
		if (!m_IsRunning) 
			return;
			
		if (!TestHarness.Finished())
		{
			bool isDone = TestHarness.Run();
			if (isDone)
			{
				string errorMessage;
				if (!AutoTestFixture.SaveXMLReport(TestHarness.Report(), errorMessage))
					AutoTestFixture.LogRPT(errorMessage);
					
				TestHarness.End();
				m_IsRunning = false;
				m_IsDone = true;
			}
		}
	}
}

/*
	Script wrapper for TestResultBase that allows script provided kind and message in elegant way.
*/
class CustomResult : TestResultBase
{
	private bool   m_Success;
	private string m_FailureText;
	private string m_FailureKind;
	
	override bool Failure()
	{
		return !m_Success;
	}
	
	// string FailureTextNativeFormat(string type, string userTxt);
	// That will return the formatted string
	override string FailureText()
	{
		return string.Format("<failure type=\"%1\">%2</failure>", m_FailureKind, m_FailureText);
	}
	
	void CustomResult(bool success, string text = "User provided error!", string kind = "Failure")
	{
		m_Success     = success;
		m_FailureText = text;
		m_FailureKind = kind;
	}
}

/* Suite is a collection of tests.  */
/*
class FooTestSuite : TestSuite
{
	// !!!
	// Be careful, if you leave the suite empty - no error is given and it will just ommit everything.
	// !!!
        
	[Step(EStage.Setup)]
	void FooSetup()
	{
		Print("FooTestSuite is setting up... Tests can commence now..");
	}
	
	[Step(EStage.TearDown)]
	void FooFinish()
	{
		Print("FooTestSuite is finishing up ... Tests are done..");
	}
}
*/

/* Test is registered within suite via the usage of the Test attribute. */
/*
[Test("FooTestSuite")]
class FooTest1 : TestBase
{
	private int       m_Value;
	private const int k_TargetValue = 10;
	
	[Step(EStage.Setup)]
	void Setup()
	{
		m_Value = k_TargetValue;
	}
	
	[Step(EStage.Main)]
	void Main()
	{
		if ( m_Value != k_TargetValue )
		{
			SetResult( new CustomResult(false, string.Format("Expected value: %1, Actual value: %2", k_TargetValue, m_Value)) );
		}
		else
		{
			SetResult( new CustomResult(true, "Successfull!") );
		}
	}
	
	[Step(EStage.TearDown)]
	void Cleanup()
	{
		m_Value = 0;
	}
}

/* Test is registered within suite via the usage of the Test attribute. */
/*
[Test("FooTestSuite")]
class FooTest2 : TestBase
{
	private int       m_Value;
	private const int k_TargetValue = 10;
	
	[Step(EStage.Setup)]
	void Setup()
	{
		m_Value = 11; // intentionally wrong
	}
	
	[Step(EStage.Main)]
	void Main()
	{
		if ( m_Value != k_TargetValue )
		{
			SetResult( new CustomResult(false, string.Format("Expected value: %1, Actual value: %2", k_TargetValue, m_Value)) );
		}
		else
		{
			SetResult( new CustomResult(true, "Successfull!") );
		}
	}
	
	[Step(EStage.TearDown)]
	void Cleanup()
	{
		m_Value = 0;
	}
}

class BarTestSuite : TestSuite
{       
	[Step(EStage.Setup)]
	void A();
}

[Test("BarTestSuite")]
class BarTest1 : TestBase
{
	[Step(EStage.Setup)]
	void Setup();
	
	[Step(EStage.Main)]
	void Main()
	{
	        SetResult( new CustomResult(true, "Successfull!") );
	}
	
	[Step(EStage.TearDown)]
	void Cleanup();
}
*/