/*!
\defgroup ScriptTestingFramework Script Testing Framework
\addtogroup ScriptTestingFramework
@{

\page Page_System_ScriptTestingFramework Script Testing Framework
\tableofcontents

\section Introduction Introduction

- Provides a unified and simple interface that emphasizes the smallest amount
of boiler plate code possible.

- The collection and instantiation of its primitives is performed right after
the script compilation.

- Within the framework a SINGLE test harness derived class can exist. The
harness runs the tests and contains API to access them.

- The test units are compiled to Suites. These provide additional API for
environmental control.

\section SimpleTests Simple tests

Can be perfomed in form of annotated <b>free functions</b>.

<em>Note: Notice the Suite name in the attribute.</em>

@code
[Test("MyFirstTestSuite")]
TestResultBase MyFooBarTest() { return TestBoolResult(5 > 3); }
@endcode

\section StatefulTests Stateful tests
More elaborate tests that need some state and will run for several ticks have
to be defined as TestBase derived classes. Your logic has to be ran through
__step methods__.

\subsection StepMethods Step methods
- You can name your step methods however you like.
- They have to be annotated by the [Step(Stage)] attribute which pairs the step
with a stage.

\subsection Stages Stages
- They divide the steps into groups that express the initialization and
finalization process.
- Stages are executed in order Setup -> Main -> TearDown.
- Methods in stages are executed in order of definition.

\subsection ReturnValues Return values
- void -> Will get executed only once.
- bool -> Will get executed every tick until true is returned.

\subsection Result Result
- Result is set via API member method of TestBase `SetResult(TestResultBase)`.
- Setting a result which evaluates to failure will terminate the stage.

\subsection FailureUnwind Failure unwind
- If the Setup stage fails the test only terminates and TearDown is not called.
- Main stage failure will trigger the TearDown.
- TearDown failure will do nothing.

\subsection Timeout Timeout
- The tests won't timeout by default. The value may be specified via Test
attribute.
- The timeout counter resets for every stage method.
- If the stage method times out the TimeoutResult is set (it evaluates to
failure and the failure unwind process starts).

@code
[Test("MyFirstTestSuite", timeoutS: 2, timeoutMs: 250)]
class MyAsyncTest : TestBase
{
	// Simple blocking initialization.
	[Step(EStage.Setup)]
	void Initialize() { ... }

	// Async test which is waiting for result for several frames.
	[Step(EStage.Main)]
	bool Pool() { ... }

	// Finalization process waiting for result for several frames.
	[Step(EStage.TearDown)]
	bool FinalizeA() { ... }

	// Simple blocking finalization call.
	[Step(EStage.TearDown)]
	void FinalizeB() { ... }
}
@endcode

*/

//-----------------------------------------------------------------------------
//! Attribute used for tests annotation and assignment to Suites.
class Test
{
	string Suite;

	int TimeoutS;
	int TimeoutMs;
	int SortOrder;

	//! Defines a suite the test belongs to, its timeout value and order within the suite.
	void Test(string suite, int timeoutS = 0, int timeoutMs = 0, int sortOrder = 0)
	{
		Suite = suite;
		TimeoutS = timeoutS;
		TimeoutMs = timeoutMs;
		SortOrder = sortOrder;
	}
}

//-----------------------------------------------------------------------------
//! Stage definition used in conjunction with Step attribute.
enum EStage
{
	Setup,
	Main,
	TearDown
}

//-----------------------------------------------------------------------------
//! Attribute which marks a method as part of the testing process.
class Step
{
	EStage Stage;

	void Step(EStage stage = EStage.Main)
	{
		Stage = stage;
	}
}

//-----------------------------------------------------------------------------
//! Collection and main interface of the Testing framework.
class TestHarness : Managed
{
	//! Starts the testing process. Returns true when all tests have finished. If
	//! some of them are still in progress false is reported.
	proto native static bool Run();
	//! Generates a xml report.
	proto static string Report();
	//! Returns number of test suites.
	proto native static int GetNSuites();
	//! Returns a test suite.
	proto native static TestSuite GetSuite(int handle);
	//! Returns currently active TestSuite or null when none is active.
	proto native static TestSuite ActiveSuite();
	//! Returns true when all tests and suites finished.
	proto native static bool Finished();
	//! Starts up the testing process and initializes the structures.
	proto native static void Begin();
	//! Finalizes the testing process.
	proto native static void End();
}

//-----------------------------------------------------------------------------
//! Collection of tests.
class TestSuite : Managed
{
	//! Sets the suite result. Failure can result in specialized behavior described
	//! in TestResultBase.
	proto native void SetResult(TestResultBase res);
	//! Returns the number for tests within this suite.
	proto native int GetNTests();
	//! Returns a test.
	proto native TestBase GetTest(int handle);
	//! Enables/Disables the suites. Disabled suites won't run at all.
	proto native void SetEnabled(bool val);
	//! Enabled flag getter.
	proto native bool IsEnabled();
	//! Suite class name getter. Strictly for UI porposes!
	proto string GetName();
	//! Callback for user defined initialization. Called for all suites during TestHarness.Begin().
	protected void OnInit();
}

//-----------------------------------------------------------------------------
//! Test base class. 
class TestBase : Managed
{
	//! Sets the test result. Failure can result in specialized behavior described
	//! in TestResultBase.
	proto native void SetResult(TestResultBase res);
	//! Result getter.
	proto native TestResultBase GetResult();
	//! Enables/Disables the test. Disabled tests won't run at all.
	proto native void SetEnabled(bool val);
	//! Enabled flag getter.
	proto native bool IsEnabled();
	//! Test name getter. Strictly for UI porposes!
	proto string GetName();
}

//-----------------------------------------------------------------------------
//! Base class for test results. This way you report back to the system.
//! More complex failure types with descriptions can be reported by
//! implementation of FailureText in format of junit
//! [https://llg.cubic.org/docs/junit/].
class TestResultBase : Managed
{
	//! Return true of the result means failure.
	bool Failure() { return NativeFailure(); } 
	//! Text used for xml report output.
	string FailureText() { return NativeFailureText(); }

	// Script forwarding to cpp. Otherwise the script overloading wouldn't be able
	// to call the native base implementation.
	// ----------------- vvv ----------------- 
	proto native bool NativeFailure();
	proto native string NativeFailureText();
	// ----------------- ^^^ ----------------- 
}

/*!
 * @}
 */

//-----------------------------------------------------------------------------
// EXAMPLES
//-----------------------------------------------------------------------------
/*
//-----------------------------------------------------------------------------
//! Basic test result.
class TestBoolResult : TestResultBase
{
	bool Value;

	void TestBoolResult(bool val) { Value = val; }

	override bool Failure() { return !Value; }

	override string FailureText()
	{
		// junit kind of error report. (simple)
		return "<failure type=\"BoolResult\">Failed</failure>";
	}
}

//-----------------------------------------------------------------------------
class MyHarness : TestHarness
{
}

//-----------------------------------------------------------------------------
class MyTestSuite : TestSuite
{
	int cnt;

	[Step(EStage.Setup)]
	void Prep()
	{
		Print("MyTestSuite::Prep");
		cnt = 3;
	}

	[Step(EStage.Setup)]
	bool Count()
	{
		--cnt;
		Print("MyTestSuite::Count: cnt=" + cnt);
		return cnt == 0;
	}

	[Step(EStage.TearDown)]
	bool CountUp()
	{
		++cnt;
		Print("MyTestSuite::CountUp: cnt=" + cnt);
		return cnt == 10;
	}
}

//-----------------------------------------------------------------------------
[Test("MyTestSuite")]
TestResultBase MyTest()
{
	Print("MyFuncTest");
	return new TestBoolResult(true);
}

//-----------------------------------------------------------------------------
[Test("MyTestSuite")]
class MyAsyncTest : TestBase
{
	int counter;

	[Step(EStage.Main)]
	void Set()
	{
		counter = 10;
	}

	[Step(EStage.Main)]
	bool Pool() 
	{
		Print("AsyncTest::Pool::counter=" + counter);

		if(counter == 0)
		{
			Print("AsyncTest::Pool::Result");
			SetResult(new TestBoolResult(false));
			return true;
		}

		Print("AsyncTest::Pool::Progress");

		counter--;
		return false;
	}
}
*/
