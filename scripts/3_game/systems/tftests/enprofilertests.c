class EnProfilerTests : TestFramework
{
	//! Remember this, so we can restore the previous state before the test!
	bool m_bWasProfilerEnabled;
	
	//---------------------------------------------------------------------------
	// Ctor - Decides the tests to run
	//---------------------------------------------------------------------------
	void EnProfilerTests()
	{
		m_bWasProfilerEnabled = EnProfiler.IsEnabledC();
		
		AddInitTest("TestToggling");
		AddInitTest("TestTogglingImmediate");
		AddInitTest("TestSetFlags");
		AddInitTest("TestClearFlags");
		AddInitTest("TestAddFlags");
		AddInitTest("TestModule");
		AddInitTest("TestClassTimeData");
		AddInitTest("TestClassCountData");
		AddInitTest("TestFuncTimeData");
		AddInitTest("TestFuncCountData");		
	}
	
	//---------------------------------------------------------------------------
	// Dtor
	//---------------------------------------------------------------------------
	void ~EnProfilerTests()
	{
		EnProfiler.Enable(m_bWasProfilerEnabled, true);
	}
	
	//---------------------------------------------------------------------------
	// Tests
	//---------------------------------------------------------------------------
	// Test toggling state
	TFResult TestToggling()
	{
		bool currentlyEnabled = EnProfiler.IsEnabledP();
		EnProfiler.Enable(!currentlyEnabled);
		if (Assert(currentlyEnabled != EnProfiler.IsEnabledP()))
		{
			EnProfiler.Enable(currentlyEnabled);				
			return BTFR(Assert(currentlyEnabled == EnProfiler.IsEnabledP()));
		}
		
		return NTFR(TFR.FAIL);
	}
	
	//---------------------------------------------------------------------------
	// Test toggling immediate state
	TFResult TestTogglingImmediate()
	{
		bool currentlyEnabled = EnProfiler.IsEnabledC();
		EnProfiler.Enable(!currentlyEnabled, true);
		if (Assert(currentlyEnabled != EnProfiler.IsEnabledC()))
		{
			EnProfiler.Enable(currentlyEnabled, true);				
			return BTFR(Assert(currentlyEnabled == EnProfiler.IsEnabledC()));
		}
		
		return NTFR(TFR.FAIL);
	}
	
	//---------------------------------------------------------------------------
	// Test SetFlags/GetFlags
	TFResult TestSetFlags()
	{
		int currentFlags = EnProfiler.GetFlags();
		
		for (int i = 0; i < EnumTools.GetEnumSize(EnProfilerFlags); ++i)
		{
			int flags = EnumTools.GetEnumValue(EnProfilerFlags, i);
			EnProfiler.SetFlags(flags);
			
			if (!Assert(EnProfiler.GetFlags() == flags))
			{
				EnProfiler.SetFlags(currentFlags);
				return NTFR(TFR.FAIL);
			}
			
			for (int j = 0; j < EnumTools.GetEnumSize(EnProfilerFlags); ++j)
			{
				flags |= EnumTools.GetEnumValue(EnProfilerFlags, j);
				EnProfiler.SetFlags(flags);
				
				if (!Assert(EnProfiler.GetFlags() == flags))
				{
					EnProfiler.SetFlags(currentFlags);
					return NTFR(TFR.FAIL);
				}
			}
		}
		
		// Let's test some bogus
		EnProfiler.SetFlags(-333);
		int bogusFlags = EnProfiler.GetFlags();
		bogusFlags &= ~EnProfilerFlags.ALL;
		if (!Assert(bogusFlags == 0))
		{
			EnProfiler.SetFlags(currentFlags);
			return NTFR(TFR.FAIL);
		}
		
		bogusFlags = EnProfiler.SetFlags(6003);
		bogusFlags &= ~EnProfilerFlags.ALL;
		if (!Assert(bogusFlags == 0))
		{
			EnProfiler.SetFlags(currentFlags);
			return NTFR(TFR.FAIL);
		}
		
		// Reset
		EnProfiler.SetFlags(currentFlags);		
		return NTFR(TFR.SUCCESS);
	}
	
	//---------------------------------------------------------------------------
	// Test removing of flags
	TFResult TestClearFlags()
	{
		int currentFlags = EnProfiler.GetFlags();
						
		EnProfiler.SetFlags(EnProfilerFlags.RECURSIVE);
		
		if (!Assert(EnProfiler.RemoveFlags(EnProfilerFlags.RECURSIVE) == EnProfilerFlags.NONE))
		{
			EnProfiler.SetFlags(currentFlags);
			return NTFR(TFR.FAIL);
		}
		
		EnProfiler.SetFlags(EnProfilerFlags.RECURSIVE | EnProfilerFlags.RESET);
		EnProfiler.RemoveFlags(EnProfilerFlags.RECURSIVE | EnProfilerFlags.RESET);
		
		if (!Assert(EnProfiler.GetFlags() == EnProfilerFlags.NONE))
		{
			EnProfiler.SetFlags(currentFlags);
			return NTFR(TFR.FAIL);
		}
		
		EnProfiler.SetFlags(EnProfilerFlags.RECURSIVE | EnProfilerFlags.RESET);
		EnProfiler.ClearFlags();
		
		if (!Assert(EnProfiler.GetFlags() == EnProfilerFlags.NONE))
		{
			EnProfiler.SetFlags(currentFlags);
			return NTFR(TFR.FAIL);
		}
		
		// Reset
		EnProfiler.SetFlags(currentFlags);		
		return NTFR(TFR.SUCCESS);
	}
	
	//---------------------------------------------------------------------------
	// Test adding of flags
	TFResult TestAddFlags()
	{
		int currentFlags = EnProfiler.GetFlags();
						
		EnProfiler.ClearFlags();
		
		// Return should match resulting flags
		if (!Assert(EnProfiler.AddFlags(EnProfilerFlags.RESET) == EnProfiler.GetFlags()))
		{
			EnProfiler.SetFlags(currentFlags);
			return NTFR(TFR.FAIL);
		}
		
		if (!Assert(EnProfiler.GetFlags() == EnProfilerFlags.RESET))
		{
			EnProfiler.SetFlags(currentFlags);
			return NTFR(TFR.FAIL);
		}
		
		if (!Assert(EnProfiler.AddFlags(EnProfilerFlags.RECURSIVE) == (EnProfilerFlags.RESET | EnProfilerFlags.RECURSIVE)))
		{
			EnProfiler.SetFlags(currentFlags);
			return NTFR(TFR.FAIL);
		}
		
		// Reset
		EnProfiler.SetFlags(currentFlags);		
		return NTFR(TFR.SUCCESS);
	}
	
	//---------------------------------------------------------------------------
	// Test module
	TFResult TestModule()
	{
		// File lives in Game, use it while testing
		const EnProfilerModule eptModule = EnProfilerModule.GAME;
		
		// This was added at the same time as this API, so check if it works as well
		string nameOfCurrentModule = Type().GetModule();
		if (!Assert(nameOfCurrentModule != ""))
		{
			return NTFR(TFR.FAIL);
		}
		
		// We know we are in Game, so use it as a test
		EnProfilerModule currentModule;
		if (!Assert(EnProfiler.NameToModule(nameOfCurrentModule, currentModule)))
		{
			return NTFR(TFR.FAIL);
		}
		
		if (!Assert(currentModule == eptModule))
		{
			return NTFR(TFR.FAIL);
		}
		
		// Test if setting and getting works
		EnProfilerModule currentlyProfiledModule = EnProfiler.GetModule();		
		EnProfiler.SetModule(eptModule);		
		
		if (!Assert(EnProfiler.GetModule() == eptModule))
		{
			EnProfiler.SetModule(currentlyProfiledModule);
			return NTFR(TFR.FAIL);
		}
		
		// Data to restore
		int currentFlags = EnProfiler.GetFlags();
		bool wasEnabled = EnProfiler.RequestImmediateData();
		
		// Make sure we are only profiling Game and that the data is clean
		// Only valid for the Get...Per... methods, as they need to be sorted
		EnProfiler.RemoveFlags(EnProfilerFlags.RECURSIVE);
		
		// GetTickTime() returns in seconds, so gather the results in seconds too
		int resolution = EnProfiler.GetTimeResolution();
		EnProfiler.SetTimeResolution(1);
		
		// Time to sleeb
		float previousTime = EnProfiler.GetTimeOfFunc("Sleep", Type(), true);
		float timeSlept = Sleep(0.3);
		float postTime = EnProfiler.GetTimeOfFunc("Sleep", Type(), true);
		float diff = postTime - previousTime - timeSlept;
		
		// Restore
		EnProfiler.SetTimeResolution(resolution);		
		
		// We called the function, so it must have some time
		if (!Assert(postTime > 0))
		{
			EnProfiler.SetFlags(currentFlags);
			
			if (!wasEnabled)
				EnProfiler.Enable(false, true);
			
			EnProfiler.SetModule(currentlyProfiledModule);
			
			return NTFR(TFR.FAIL);
		}
		
		if (!Assert(diff < 0.00001))
		{
			EnProfiler.SetFlags(currentFlags);			
			
			if (!wasEnabled)
				EnProfiler.Enable(false, true);
			
			EnProfiler.SetModule(currentlyProfiledModule);
			
			return NTFR(TFR.FAIL);
		}
		
		// Clean the session
		EnProfiler.ResetSession(true);
		
		// Something from a different module should not get sorted, so just fire something from a different module
		for (int i = 0; i < 1000; ++i)
		{
			EnumTools.StringToEnum(EnProfilerModule, "MISSION_CUSTOM");
		}
		
		// Sort and gather the data and validate if it is correct
		EnProfiler.SortData();	
		array<ref EnProfilerTimeFuncPair> timePerFunc = {};
		EnProfiler.GetTimePerFunc(timePerFunc);
		
		Debug.TFLog("Game fncs:", this, "TestModule");
		
		int funcCount = timePerFunc.Count();
		for (int j = 0; j < funcCount; ++j)
		{
			EnProfilerTimeFuncPair tfp = timePerFunc[j];
			Debug.TFLog(string.Format("	time: %1 | fnc: %2", tfp.param1, tfp.param2), this, "TestModule");
			// We are currently profiling Game, so this Core function shouldn't be present
			if (!Assert(tfp.param2 != "EnumTools::StringToEnum"))
			{
				EnProfiler.SetFlags(currentFlags);
				
				if (!wasEnabled)
					EnProfiler.Enable(false, true);
				
				EnProfiler.SetModule(currentlyProfiledModule);
				
				return NTFR(TFR.FAIL);
			}
		}
		
		array<ref EnProfilerTimeClassPair> timePerClass = {};
		EnProfiler.GetTimePerClass(timePerClass);
		
		int classCount = timePerClass.Count();
		for (int k = 0; k < classCount; ++k)
		{
			typename type = timePerClass[k].param2;
			EnProfilerModule classModule;
			if (!Assert(EnProfiler.NameToModule(type.GetModule(), classModule)))
			{
				EnProfiler.SetFlags(currentFlags);
				
				if (!wasEnabled)
					EnProfiler.Enable(false, true);
				
				EnProfiler.SetModule(currentlyProfiledModule);
				
				return NTFR(TFR.FAIL);
			}
			
			// Only classes from Game should be present
			if (!Assert(classModule == eptModule))
			{
				EnProfiler.SetFlags(currentFlags);
				
				if (!wasEnabled)
					EnProfiler.Enable(false, true);
				
				EnProfiler.SetModule(currentlyProfiledModule);
				
				return NTFR(TFR.FAIL);
			}
		}
		
		// Now let's check if we can gather the data of what we called earlier by switching the profiled module without resetting the session
		EnProfiler.SetModule(EnProfilerModule.CORE, false);
		EnProfiler.SortData();
		timePerFunc.Clear(); // Let's reuse the array, but the Get...Per... methods only appends to the array, clearing is our responsibility
		EnProfiler.GetTimePerFunc(timePerFunc);
		
		bool found = false;
		
		Debug.TFLog("Core fncs:", this, "TestModule");
		
		funcCount = timePerFunc.Count();
		for (int l = 0; l < funcCount; ++l)
		{
			EnProfilerTimeFuncPair tfpc = timePerFunc[l];
			Debug.TFLog(string.Format("	time: %1 | fnc: %2", tfpc.param1, tfpc.param2), this, "TestModule");
			// We are currently profiling Core, so this Core function should be present
			if (tfpc.param2 == "EnumTools::StringToEnum")
			{
				found = true;
				break;
			}
		}
		
		Assert(found);
		
		// Test some bogus
		EnProfilerModule mod = EnProfiler.GetModule();
		EnProfiler.SetModule(-333);
		bool success = Assert(EnProfiler.GetModule() == mod);
		EnProfiler.SetModule(6003);
		success &= Assert(EnProfiler.GetModule() == mod);
		
		EnProfiler.SetFlags(currentFlags);
		EnProfiler.SetModule(currentlyProfiledModule);
		
		if (!wasEnabled)
			EnProfiler.Enable(false, true);
		
		return BTFR(success && found);
	}
	
	//---------------------------------------------------------------------------
	// Test to see if class time data is correct
	TFResult TestClassTimeData()
	{
		// We should restore this when done
		int resolution = EnProfiler.GetTimeResolution();
		bool wasEnabled = EnProfiler.RequestImmediateData();
		
		// GetTickTime() returns in seconds, so gather the results in seconds too
		EnProfiler.SetTimeResolution(1);
		
		// Create the classes
		EPTHelperClass clss = new EPTHelperClass();
		
		// Time to stress
		float previousTime = EnProfiler.GetTimeOfClass(clss.Type(), true);
		float timeStressed = clss.DoEverything();
		float postTime =  EnProfiler.GetTimeOfClass(clss.Type(), true);
		float postTimeStatic = EnProfiler.GetTimeOfClass(StaticGetType(EPTHelperClass), true);
		float timeProfiled = postTime - previousTime;
		float diff = Math.AbsFloat(timeProfiled - timeStressed);
		
		Debug.TFLog(string.Format("Profiling result: stressed: %1 | profiled: %2 | diff: %3", timeStressed, timeProfiled, diff), this, "TestClassTimeData");
		
		// Restore
		EnProfiler.SetTimeResolution(resolution);
		if (!wasEnabled)
			EnProfiler.Enable(false, true);
		
		// We called the function, so it must have some time
		if (!Assert(postTime > 0))
		{
			return NTFR(TFR.FAIL);
		}
		
		if (!Assert(postTime == postTimeStatic))
		{
			return NTFR(TFR.FAIL);
		}
		
		if (!Assert(diff < 0.001))
		{
			return NTFR(TFR.FAIL);
		}
		
		return NTFR(TFR.SUCCESS);
	}
	
	//---------------------------------------------------------------------------
	// Test to see if class count data is correct
	TFResult TestClassCountData()
	{
		const int allocAmount = 9;
		const int releaseAmount = 6;
		int remainingAmount = allocAmount - releaseAmount;
		
		// We should restore this when done
		bool wasEnabled = EnProfiler.RequestImmediateData();
		
		// Time to test
		int previousAlloc = EnProfiler.GetAllocationsOfClass(StaticGetType(EPTHelperClass), true);
		int previousInstances = EnProfiler.GetInstancesOfClass(StaticGetType(EPTHelperClass), true);

		array<ref EPTHelperClass> instanceArr = {};
		for (int i = 0; i < allocAmount; ++i)
		{
			instanceArr.Insert(new EPTHelperClass());
		}
		
		for (int j = 0; j < releaseAmount; ++j)
		{
			delete instanceArr[j];
		}
		
		int postAlloc = EnProfiler.GetAllocationsOfClass(StaticGetType(EPTHelperClass), true);
		int postInstances = EnProfiler.GetInstancesOfClass(StaticGetType(EPTHelperClass), true);
		
		int alloced = postAlloc - previousAlloc;
		int instances = postInstances - previousInstances;
		
		Debug.TFLog(string.Format("Profiling result: alloc: %1 | instances: %2", alloced, instances), this, "TestClassCountData");
		
		// Restore
		if (!wasEnabled)
			EnProfiler.Enable(false, true);
		
		// Time to check
		if (!Assert(alloced == allocAmount))
		{
			return NTFR(TFR.FAIL);
		}
		
		if (!Assert(instances == remainingAmount))
		{
			return NTFR(TFR.FAIL);
		}
		
		return NTFR(TFR.SUCCESS);
	}
	
	//---------------------------------------------------------------------------
	// Test to see if func time data is correct
	TFResult TestFuncTimeData()
	{
		// We should restore this when done
		int resolution = EnProfiler.GetTimeResolution();
		bool wasEnabled = EnProfiler.RequestImmediateData();
		
		// GetTickTime() returns in seconds, so gather the results in seconds too
		EnProfiler.SetTimeResolution(1);
		
		// Time to stress
		float previousTime = EnProfiler.GetTimeOfFunc("StringFormat", Type(), true);
		float timeStressed = StringFormat();
		float postTime = EnProfiler.GetTimeOfFunc("StringFormat", Type(), true);
		float timeProfiled = postTime - previousTime;
		float diff = Math.AbsFloat(timeProfiled - timeStressed);
		
		float previousTime2 = EnProfiler.GetTimeOfFunc("StringConcat", Type(), true);
		float timeStressed2 = StringConcat();
		float postTime2 = EnProfiler.GetTimeOfFunc("StringConcat", Type(), true);
		float timeProfiled2 = postTime2 - previousTime2;
		float diff2 = Math.AbsFloat(timeProfiled2 - timeStressed2);
		
		Debug.TFLog(string.Format("Profiling result: StringFormat: %1 | StringConcat: %2", timeProfiled, timeProfiled2), this, "TestFuncTimeData");
		
		// Restore
		EnProfiler.SetTimeResolution(resolution);
		if (!wasEnabled)
		{
			EnProfiler.Enable(false, true);
		}
		
		// We called the function, so it must have some time
		if (!Assert(postTime > 0))
		{
			return NTFR(TFR.FAIL);
		}
		
		if (!Assert(diff < 0.001))
		{
			return NTFR(TFR.FAIL);
		}
		
		if (!Assert(postTime2 > 0))
		{
			return NTFR(TFR.FAIL);
		}
		
		if (!Assert(diff2 < 0.001))
		{
			return NTFR(TFR.FAIL);
		}
		
		// I know that string.Format is faster than additive concatenation
		if (!Assert(timeProfiled < timeProfiled2))
		{
			return NTFR(TFR.FAIL);
		}
		
		return NTFR(TFR.SUCCESS);
	}
	
	//---------------------------------------------------------------------------
	// Test to see if func count data is correct
	TFResult TestFuncCountData()
	{
		// We should restore this when done
		bool wasEnabled = EnProfiler.RequestImmediateData();
		
		// Time to count		
		
		// - CallFunction
		int previousCountCF = EnProfiler.GetCountOfFunc("TestFuncCountDataHelper", Type(), true);
		GetGame().GameScript.CallFunction(this, "TestFuncCountDataHelper", null, 0);
		int postCountCF = EnProfiler.GetCountOfFunc("TestFuncCountDataHelper", Type(), true);
		
		int callCountCF = postCountCF - previousCountCF;
		
		// - CallFunctionParams
		int previousCountCFP = EnProfiler.GetCountOfFunc("TestFuncCountDataHelper", Type(), true);
		GetGame().GameScript.CallFunctionParams(this, "TestFuncCountDataHelper", null, null);
		int postCountCFP =  EnProfiler.GetCountOfFunc("TestFuncCountDataHelper", Type(), true);
		
		int callCountCFP = postCountCFP - previousCountCFP;
		
		// - Regular call
		int previousCountRG = EnProfiler.GetCountOfFunc("TestFuncCountDataHelper", Type(), true);
		TestFuncCountDataHelper();
		int postCountRG =  EnProfiler.GetCountOfFunc("TestFuncCountDataHelper", Type(), true);
		
		int callCountRG = postCountRG - previousCountRG;
		
		// - Call
		int previousCountC = EnProfiler.GetCountOfFunc("TestFuncCountDataHelper", Type(), true);
		GetGame().GameScript.Call(this, "TestFuncCountDataHelper", 0);
		int postCountC =  EnProfiler.GetCountOfFunc("TestFuncCountDataHelper", Type(), true);
		
		int callCountC = postCountC - previousCountC;
		
		// - Garbage		
		int callCountNon = EnProfiler.GetCountOfFunc("Non Existing Method", Type(), true);
		
		// - Static
		int previousCountS = EnProfiler.GetCountOfFunc("TestFuncCountDataHelperStatic", Type(), true);
		TestFuncCountDataHelperStatic();
		int postCountS = EnProfiler.GetCountOfFunc("TestFuncCountDataHelperStatic", Type(), true);
		
		int callCountS = postCountS - previousCountS;
		
		// - Global
		int previousCountG = EnProfiler.GetCountOfFuncG("GetDayZGame", true);
		GetDayZGame();
		int postCountG = EnProfiler.GetCountOfFuncG("GetDayZGame", true);
	
		int callCountG = postCountG - previousCountG;
		
		// - Global proto
		// Not tracked, so don't need to compare before and after, should always be 0
		ErrorEx("Testing global proto call", ErrorExSeverity.INFO);
		int callCountGP = EnProfiler.GetCountOfFuncG("ErrorEx", true);
		
		// - Static proto
		// Not tracked, so don't need to compare before and after, should always be 0
		int callCountSP = EnProfiler.GetCountOfFunc("GetCountOfFunc", StaticGetType(EnProfiler), true);
		
		// - proto
		// Not tracked, so don't need to compare before and after, should always be 0
		GetGame().GetHostName();
		int callCountP = EnProfiler.GetCountOfFunc("GetHostName", GetGame().Type(), true);
		
		// - proto native
		// Not tracked, so don't need to compare before and after, should always be 0
		GetGame().IsServer();
		int callCountPN = EnProfiler.GetCountOfFunc("IsServer", GetGame().Type(), true);
		
		// - static proto native
		// Not tracked, so don't need to compare before and after, should always be 0
		ErrorModuleHandler.GetInstance();
		int callCountSPN = EnProfiler.GetCountOfFunc("GetInstance", StaticGetType(ErrorModuleHandler), true);
		
		// Restore
		if (!wasEnabled)
		{
			EnProfiler.Enable(false, true);
		}
		
		// Do the checks
		
		// - CallFunction
		if (!Assert(callCountCF == 1))
		{
			return NTFR(TFR.FAIL);
		}
		
		// - CallFunctionParams
		if (!Assert(callCountCFP == 1))
		{
			return NTFR(TFR.FAIL);
		}
		
		// - Regular call
		if (!Assert(callCountRG == 1))
		{
			return NTFR(TFR.FAIL);
		}
		
		// - Call
		if (!Assert(callCountC == 1))
		{
			return NTFR(TFR.FAIL);
		}
		
		// - Garbage
		if (!Assert(callCountNon == -1))
		{
			return NTFR(TFR.FAIL);
		}
		
		// - Static
		if (!Assert(callCountS == 1))
		{
			return NTFR(TFR.FAIL);
		}
		
		// - Global
		if (!Assert(callCountG == 1))
		{
			return NTFR(TFR.FAIL);
		}
		
		// - Global proto
		if (!Assert(callCountGP == 0))
		{
			return NTFR(TFR.FAIL);
		}
		
		// - Static proto
		if (!Assert(callCountSP == 0))
		{
			return NTFR(TFR.FAIL);
		}
		
		// - proto
		if (!Assert(callCountP == 0))
		{
			return NTFR(TFR.FAIL);
		}
		
		// - proto native
		if (!Assert(callCountPN == 0))
		{
			return NTFR(TFR.FAIL);
		}
		
		// - static proto native
		if (!Assert(callCountSPN == 0))
		{
			return NTFR(TFR.FAIL);
		}
		
		return NTFR(TFR.SUCCESS);
	}
	
	//---------------------------------------------------------------------------
	// Helpers
	//---------------------------------------------------------------------------
	// Snore
	float Sleep(float timeS)
	{		
		float startTime = GetGame().GetTickTime();
		while (GetGame().GetTickTime() - startTime < timeS)
		{
			// Zzz
		}		
		
		return GetGame().GetTickTime() - startTime;
	}
	
	//---------------------------------------------------------------------------
	// Example stress method
	float StringFormat()
	{		
		float startTime = GetGame().GetTickTime();
		
		for (int i = 0; i < 1000; ++i)
		{
			string example = string.Format("This %1 is %2 just %3 an %4 example %5", i, Type(), this, startTime, "lorem ipsum 1 2 3");
		}	
		
		return GetGame().GetTickTime() - startTime;
	}
	
	//---------------------------------------------------------------------------
	// Example stress method 2
	float StringConcat()
	{		
		float startTime = GetGame().GetTickTime();
		
		for (int i = 0; i < 1000; ++i)
		{
			string example = "This " + i + " is " + Type() + " just " + this + " an " + startTime + " example " + "lorem ipsum 1 2 3";
		}
		
		return GetGame().GetTickTime() - startTime;
	}
	
	//---------------------------------------------------------------------------
	// To make sure it is only ever called in that test
	void TestFuncCountDataHelper()
	{
		int dummy = 3;
	}
	
	//---------------------------------------------------------------------------
	// To make sure it is only ever called in that test
	static void TestFuncCountDataHelperStatic()
	{
		int dummy = 3;
	}
}

class EPTHelperClass
{
	float Sleep2(float timeS)
	{		
		float startTime = GetGame().GetTickTime();
		while (GetGame().GetTickTime() - startTime < timeS)
		{
			// Zzz
		}		
		
		return GetGame().GetTickTime() - startTime;
	}
	float SleepAgain(float timeS)
	{		
		float startTime = GetGame().GetTickTime();
		while (GetGame().GetTickTime() - startTime < timeS)
		{
			// Zzz
		}		
		
		return GetGame().GetTickTime() - startTime;
	}
	
	float DoEverything()
	{
		float startTime = GetGame().GetTickTime();
		
		Sleep2(3);
		SleepAgain(3);
		
		return GetGame().GetTickTime() - startTime;
	}
}