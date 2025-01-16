class AutotestConfigHandler
{
	static ref AutotestConfigJson m_Data = new AutotestConfigJson();

	static bool LoadData(string path)
	{		
		string errorMessage;
		if (!JsonFileLoader<AutotestConfigJson>.LoadFile(path, m_Data, errorMessage))
		{
			AutoTestFixture.LogRPT(errorMessage);
			return false;
		}
		
		return true;
	}
	
	static set<string> GetSuites()
	{
		return m_Data.TestSuites;
	}
}
