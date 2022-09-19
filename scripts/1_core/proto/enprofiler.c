/**
 * \defgroup Profiler Enforce Script profiling API
 *	\warning Only available on developer and diag builds
 * @{
 */

//! Flags that influences the behaviour of the EnProfiler API, applied through ...Flags functions
enum EnProfilerFlags
{
	//! No flags
	NONE,
	//! When present, will reset [PD] on sorting, otherwise will accumulate on top of it
	RESET,
	//! Whether to profile child modules
	RECURSIVE,
	//! All flags enabled
	ALL
};

//! Current base scripted modules 
enum EnProfilerModule
{
	//! 1_Core
	CORE,
	//! 2_GameLib
	GAMELIB,
	//! 3_Game
	GAME,
	//! 4_World
	WORLD,
	//! 5_Mission
	MISSION,
	//! init.c
	MISSION_CUSTOM,
	//! Can be returned from some methods
	ERROR,
};

/**
*\brief There are 3 states which can be toggled that governs whether script profiling is enabled or not
*	\note The reason for this is because when it is enabled in debug menu, or through this API without 'immediate', it will only be enabled the next frame
*/
enum EnProfilerEnabledFlags
{
	//! No flags, has value 0, so will count as false in conditions
	NONE,
	//! Script profiling UI is enabled in WIN+ALT debug menu, when this is true, it will override SCRP
	DIAG,
	//! It has been set to being always enabled through EnProfiler (SCRipt Profiler)
	SCRP,
	//! Whether profiling is currently truly happening (SCRipt Context)
	SCRC,
};

typedef Param2<float, typename> EnProfilerTimeClassPair;
typedef Param2<int, typename> EnProfilerCountClassPair;

typedef Param2<float, string> EnProfilerTimeFuncPair;
typedef Param2<int, string> EnProfilerCountFuncPair;

/**
*\brief Set of methods for accessing script profiling data
*	\note To enable profiling on game launch, use parameter "-profile"
*	\note Any mention of "[DM]" in this context will mean the WIN+ALT debug menu
*	\note Any mention of "[CI]" in this context will mean Class Instances count
*	\note Any mention of "[SR]" in this context will mean a Session Reset (See ResetSession for more information)
*	\note Any mention of "[SD]" in this context will mean a Sorted Data, which is the data which supplies Get...Per... functions (the ones that give an array)
*	\note Any mention of "[PD]" in this context will mean a Profiling Data, which is the time, count and allocation information which is stored on the class/func itself
*	\warning [PD] is only calculated AFTER a function call is finished, there is no continuous timer running
*	\note 'proto' methods are not tracked, but will contribute to a script methods total time
*/
class EnProfiler
{
	/**
	\brief Enable the gathering of script profiling data
	\note DEFAULT: disabled (unless launched with "-profile", then it is default enabled)
	\note This is separate from the one in [DM], so toggling it in [DM] will not affect this, and toggling it here will not affect [DM]
	\note It will ignore the call if trying to set the current state, except when "immediate" is used
		\param enable \p bool Whether to enable or disable, if it was previously not enabled, it will cause [SR]
			\note Disabling does not cause [SR], so all data will stay intact
		\param immediate \p bool When true will instantly start/stop profiling, otherwise it will apply it at the end of the frame (to have one stable point in time)
			\warning Keep in mind that when using immediate, it will not be the data of the entire frame, which can skew data if not kept in mind		
		\param sessionReset \p bool When set to false, no [SR] will trigger, regardless of situation
	
	@code
		// Simple enable, will start profiling the next frame
		// Will cause [SR] if !IsEnabledP() before this call
		EnProfiler.Enable(true);

		// Immediate enable, will start profiling immediately
		// Will cause [SR] if !IsEnabledP() before this call
		EnProfiler.Enable(true, true);

		// Immediate disable, will stop profiling immediately
		// Disabling will never cause [SR], preserving data
		EnProfiler.Enable(false, true);

		// Simple disable, will not profile the next frame (but still finish profiling the current one)
		// Disabling will never cause [SR], preserving data
		EnProfiler.Enable(false);
	@endcode
	*/
	static proto void Enable(bool enable, bool immediate = false, bool sessionReset = true);
	
	/**
	\brief Return if script profiling is enabled
	\note Helper methods below
		\return \p int Flags regarding the current state

	@code
		int isScriptProfilingEnabled = EnProfiler.IsEnabled();
	@endcode
	*/
	static proto int IsEnabled();
	
	/**
	\brief Return if script profiling is enabled through [DM]
		\return \p bool Whether script profiling is enabled through [DM]
	
	@code
		bool isScriptProfilingDiagEnabled = EnProfiler.IsEnabledD();
	@endcode
	*/
	static bool IsEnabledD()
	{
		return (IsEnabled() & EnProfilerEnabledFlags.DIAG);
	}
	
	/**
	\brief Return if script profiling is enabled through EnProfiler
	\note When using "-profile" launch parameter, it will enable it through EnProfiler, so this will return true
		\return \p bool Whether script profiling is enabled through script profiler
	
	@code
		bool isScriptProfilingToggleEnabled = EnProfiler.IsEnabledP();
	@endcode
	*/
	static bool IsEnabledP()
	{
		return (IsEnabled() & EnProfilerEnabledFlags.SCRP);
	}
	
	/**
	\brief Return if script profiling is actually turned on inside of the script context
	\note When using "-profile" launch parameter, it will enable it through EnProfiler, so this will return true
		\return \p bool Whether script is being profiled as of this moment
	
	@code
		bool isScriptProfilingEnabled = EnProfiler.IsEnabledC();
	@endcode
	*/
	static bool IsEnabledC()
	{
		return (IsEnabled() & EnProfilerEnabledFlags.SCRC);
	}
	
	/**
	\brief The internal sorting that happens at the end of the frame (so it is NOT necessary to call this manually) to supply Get...Per... functions 
	\note This will clear the previous [SD] and sort the [PD] currently available at this moment
	\note Flags apply to this
	\warning Keep in mind that EnProfilerFlags.RESET will clear all [PD] after this is called
	
	@code
		// Sorting all the currently available [PD], populating [SD]
		EnProfiler.SortData();

		// If flag EnProfilerFlags.RESET is enabled, then this will return 0 now even if it has been called, as [PD] has been cleared
		// This goes for any Get...Of... function (Except for [CI], the counter persists)
		EnProfiler.GetTimeOfFunc("Sleep", EnProfilerTests, true);
	@endcode
	*/
	static proto void SortData();
	
	/**
	\brief Perform [SR], clearing SessionFrame, ProfiledSessionFrames, [SD] and [PD] (except for [CI])
	\note Can also be triggered by a variety of other functions in this API
	\note When triggered by the other functions, it will call with fullReset = false
		\param fullReset \p bool Whether to clear [PD] of all modules, when false it will only clear the [PD] according to current settings
	
	@code
		// Considering the settings: SetFlags(EnProfilerFlags.NONE) and SetModule(EnProfilerModule.GAME)
		// The following call will only clear [PD] of 3_Game
		EnProfiler.ResetSession();

		// Considering the settings: SetFlags(EnProfilerFlags.RECURSIVE) and SetModule(EnProfilerModule.WORLD)
		// The following call will clear [PD] of 3_Game, 4_World, 5_Mission and their children
		EnProfiler.ResetSession();

		// The following call resets [PD] across all modules
		EnProfiler.ResetSession(true);
	@endcode
	*/
	static proto void ResetSession(bool fullReset = false);
	
	
	
	/** \name EnProfilerFlags
		Set of functions to configure the currently active EnProfilerFlags
	*/
	//@{
	
	/**
	\brief Override the currently used set of EnProfilerFlags across the API
	\note DEFAULT: EnProfilerFlags.ALL
		\param flags \p int The combination of desired EnProfilerFlags to override the currently used set
		\param sessionReset \p bool When set to false, no [SR] will trigger, regardless of situation
		\return \p int The currently used set of EnProfilerFlags after the function call
	
	@code
		// No RESET flag, [PD] will be accumulated across frames
		// No RECURSIVE flag, only the curently profiled module will be sorted
		EnProfiler.SetFlags(EnProfilerFlags.NONE);

		// RESET flag, [PD] will be reset after sorting
		// No RECURSIVE flag, only the curently profiled module will be sorted
		EnProfiler.SetFlags(EnProfilerFlags.RESET);

		// RESET flag, [PD] will be reset after sorting
		// RECURSIVE flag, all modules will be sorted
		EnProfiler.SetFlags(EnProfilerFlags.ALL);
	@endcode
	*/
	static proto int SetFlags(int flags, bool sessionReset = true);
	
	/**
	\brief Get the currently used flags across the API
		\return \p int The currently used set of EnProfilerFlags
	
	@code
		int flags = EnProfiler.GetFlags();

		if (flags & EnProfilerFlags.RECURSIVE)
		{
			Print("Currently profiling all modules.");
		}
	@endcode
	*/
	static proto int GetFlags();
	
	/**
	\brief Check if the flags are set
	\note Is effectively the same as the code displayed in GetFlags example, but without the bitwise operation
		\param flags \p int The combination of EnProfilerFlags to check if present
		\return \p bool If the flags are set
	
	@code
		if (EnProfiler.IsFlagsSet(EnProfilerFlags.ALL))
		{
			Print("Currently all flags are enabled.");
		}

		if (EnProfiler.IsFlagsSet(EnProfilerFlags.RECURSIVE))
		{
			Print("Currently profiling all modules.");
		}
	@endcode
	*/
	static proto bool IsFlagsSet(int flags);
	
	/**
	\brief Add flags to the currently used set of EnProfilerFlags across the API
	\note Simply a helper method to quickly add EnProfilerFlags
		\param flags \p int The combination of desired EnProfilerFlags to be added to the currently used set
		\param sessionReset \p bool When set to false, no [SR] will trigger, regardless of situation
		\return \p int The currently used set of EnProfilerFlags after the function call
	
	@code
		// In the case where the current set of EnProfilerFlags is EnProfilerFlags.RESET
		EnProfiler.AddFlags(EnProfilerFlags.RECURSIVE);
		// The resulting set of flags now will be EnProfilerFlags.RESET | EnProfilerFlags.RECURSIVE
		// As the above is pretty much the same as the following
		// EnProfiler.SetFlags(EnProfiler.GetFlags() | EnProfilerFlags.RECURSIVE);
		// But a much cleaner and faster alternative (bitwise operations in script is ~10x slower than C++)
	@endcode
	*/
	static proto int AddFlags(int flags, bool sessionReset = true);
	
	/**
	\brief Remove flags from the currently used set of EnProfilerFlags across the API
	\note Simply a helper method to quickly remove EnProfilerFlags
		\param flags \p int The combination of desired EnProfilerFlags to be added to the currently used set
		\param sessionReset \p bool When set to false, no [SR] will trigger, regardless of situation
		\return \p int The currently used set of EnProfilerFlags after the function call
	
	@code
		// In the case where the current set of EnProfilerFlags is EnProfilerFlags.RESET
		EnProfiler.RemoveFlags(EnProfilerFlags.RESET);
		// The resulting set of flags now will be EnProfilerFlags.NONE
		// As the above is pretty much the same as the following
		// EnProfiler.SetFlags(EnProfiler.GetFlags() & ~EnProfilerFlags.RECURSIVE);
		// But a much cleaner and faster alternative (bitwise operations in script is ~10x slower than C++)
	@endcode
	*/
	static proto int RemoveFlags(int flags, bool sessionReset = true);
	
	/**
	\brief Remove all flags from the currently used set of EnProfilerFlags across the API
	\note Simply a helper method to quickly remove all EnProfilerFlags
		\param sessionReset \p bool When set to false, no [SR] will trigger, regardless of situation
		\return \p int The currently used set of EnProfilerFlags after the function call
	
	@code
		// In the case where the current set of EnProfilerFlags is EnProfilerFlags.RESET
		EnProfiler.ClearFlags();
		// The resulting set of flags now will be EnProfilerFlags.NONE
		// As the above is pretty much the same as the following
		// EnProfiler.SetFlags(EnProfilerFlags.NONE);
		// But a much cleaner and implicit alternative
	@endcode
	*/
	static proto int ClearFlags(bool sessionReset = true);
	
	//@}
	
	
	
	/** \name EnProfilerModule
		Set of functions to configure the currently profiled EnProfilerModule
	*/
	//@{
	
	/**
	\brief Set the module to be profiled
	\note DEFAULT: EnProfilerModule.CORE
	\note When session reset is enabled, it will only reset the module which it is currently being set to, previous module data will be untouched
		\param module \p EnProfilerModule The module to profile
		\param sessionReset \p bool When set to false, no [SR] will trigger, regardless of situation
	
	@code
		EnProfiler.SetModule(EnProfilerModule.WORLD);
	@endcode
	*/
	static proto void SetModule(EnProfilerModule module, bool sessionReset = true);
	
	/**
	\brief Get the currently profiled module
		\return \p EnProfilerModule The currently profiled module
	
	@code
		EnProfilerModule module = EnProfiler.GetModule();
	@endcode
	*/
	static proto EnProfilerModule GetModule();
	
	/**
	\brief Helper to convert EnProfilerModule to string
		\param module \p EnProfilerModule The module to get the name of
		\return \p string The name of the module
	
	@code
		string moduleName = EnProfiler.ModuleToName(EnProfilerModule.GAME);
	@endcode
	*/
	static proto owned string ModuleToName(EnProfilerModule module);
	
	/**
	\brief Convert string to EnProfilerModule
		\param moduleName \p string The name of the module
		\param module \p EnProfilerModule The enum value of the module or EnProfilerModule.ERROR if not found
		\return \p bool Whether the module was found 
	
	@code
		// Get the name of the module of the current class
		string nameOfCurrentModule = Type().GetModule();
		EnProfilerModule module;

		// Convert it to the enum value
		if (EnProfiler.NameToModule(nameOfCurrentModule, module))
		{
			EnProfiler.SetModule(module);
		}
		else
		{
			ErrorEx(string.Format("Could not find EnProfilerModule: %1", nameOfCurrentModule));
		}
	@endcode
	*/
	static proto bool NameToModule(string moduleName, out EnProfilerModule module);
	
	//@}
	
	
	
	/**
	\brief Set the interval for the [SD] to update
	\note DEFAULT: 0
	\note [DM] has the following values: {0, 5, 10, 20, 30, 50, 60, 120, 144}; When an interval not part of this list is set, [DM] will be set to "CUSTOM_INTERVAL"
	\note Does not affect the gathering of [PD], this will happen continuously as the profiling is enabled
	\note This also delays the [SR] caused by EnProfilerFlags.RESET
		\param interval \p int Amount of frames to wait before [SD] is updated
		\param sessionReset \p bool When set to false, no [SR] will trigger, regardless of situation
	
	@code
		// This will make it so that [SD] is updated every 60 frames
		EnProfiler.SetInterval(60);
	@endcode
	*/
	static proto void SetInterval(int interval, bool sessionReset = true);
	
	/**
	\brief Get the currently set interval
		\return \p int The currently set interval
	
	@code
		int currentInterval = EnProfiler.GetInterval();
	@endcode
	*/
	static proto int GetInterval();
	
	
	
	/**
	\brief Set the resolution of the fetched Time data
	\note DEFAULT: 100000
	\note [DM] has the following values: {100000, 1000000, 1, 10, 100, 1000, 10000}; These are the only values available, otherwise it will round up to one in the list
	\note Does not affect any data itself, only the fetching and displaying of it (therefore, no [SR] is ever triggered by this method)
		\param resolution \p int The nth resolution of a second
	
	@code
		// Have all time being reported in 1 second
		EnProfiler.SetTimeResolution(1);

		// Have all time being reported in 1000th of a second (ms)
		EnProfiler.SetTimeResolution(1000);
	@endcode
	*/
	static proto void SetTimeResolution(int resolution);
	
	/**
	\brief Get the currently set time resolution
		\return \p int The currently set resolution
	
	@code
		int currentTimeResolution = EnProfiler.GetTimeResolution();
	@endcode
	*/
	static proto int GetTimeResolution();
	
	
	
	/**
	\brief Enable/disable returning calculated averages
	\note DEFAULT: false
	\note When EnProfilerFlags.RESET flag is not present, it will divide by the session frame
	\note When an interval is set, it will divide by the interval
	\note Does not affect any data itself, only the fetching and displaying of it (therefore, no [SR] is ever triggered by this method)
	\note [CI] will never be an average, it will always be the current count of the instance (allocations will be the value of how many times an instance is created)
		\param enable \p bool Whether to enable or disable
	
	@code
		// For example, take the situation where we only reset every 60 frames
		EnProfiler.AddFlags(EnProfilerFlags.RESET);
		EnProfiler.SetInterval(60);
		EnProfiler.EnableAverage(true);

		// And a method is called once per frame, gathering the count of that function will be 1
		// Or if a method is called twice per frame, gathering the count of that function will be 2
		// Or if a method is 3 times every 3 frames, gathering the count of that function will be 1
		// ...
		// So you get the average amount of times the method is called per frame, out of the sample of 60 frames
	@endcode
	*/
	static proto void EnableAverage(bool enable);
	
	/**
	\brief Check if returning of average data is enabled
		\return \p bool Whether returning of average data is enabled
	
	@code
		bool isDataAverage = EnProfiler.IsAverage();
	@endcode
	*/
	static proto bool IsAverage();
	
	
	
	/**
	\brief Print out [SD] to script log
	
	@code
		EnProfiler.Dump();
	@endcode
	*/
	static proto void Dump();
	
	
	
	/** \name Frame data
		Set of functions to obtain information about frame counts
	*/
	//@{
	
	/**
	\brief Get the total amount of frames passed
		\return \p int The total amount of frames passed
	
	@code
		int gameFrame = EnProfiler.GetGameFrame();
	@endcode
	*/
	static proto int GetGameFrame();
	
	/**
	\brief Get the total amount of frames in this profiling session
	\note This will only differ from GetProfiledSessionFrames when there is an Interval set
		\return \p int The total amount of frames in this profiling session
	
	@code
		int sessionFrame = EnProfiler.GetSessionFrame();
	@endcode
	*/
	static proto int GetSessionFrame();
	
	/**
	\brief Get the total amount of frames across all profiling session
	\note This will only differ from GetProfiledFrames when there was an Interval set at some point
		\return \p int The total amount of frames across all profiling session
	
	@code
		int totalFrames = EnProfiler.GetTotalFrames();
	@endcode
	*/
	static proto int GetTotalFrames();
	
	/**
	\brief Get the total amount of frames profiled in this profiling session
	\note This will only differ from GetSessionFrame when there is an Interval set
		\return \p int The total amount of frames profiled in this profiling session
	
	@code
		int profiledSessionFrames = EnProfiler.GetProfiledSessionFrames();
	@endcode
	*/
	static proto int GetProfiledSessionFrames();
	
	/**
	\brief Get the total amount of frames profiled across all profiling session
	\note This will only differ from GetTotalFrames when there was an Interval set at some point
		\return \p int The total amount of frames profiled across all profiling session
	
	@code
		int totalProfiledFrames = EnProfiler.GetProfiledFrames();
	@endcode
	*/
	static proto int GetProfiledFrames();
	
	//@}
	

	
	/** \name Sorted data
		Set of functions to obtain [SD]
		\warning Data is appended to the array, it will not clear any previous data already existing in the array
		\note Read SortData as well for more information regarding [SD]
	*/
	//@{
	
	/**
	\brief Obtain [SD] for Time Per Class
		\param outArr \p array<ref EnProfilerTimeClassPair> Array sorted by time consumed by a class
		\param count \p int The maximum amount of entries wanted
	
	@code
		// In this example the array will be filled with the 20 most time intensive classes
		// If there are less than 20 classes which consumed time, it will output that number of classes instead
		array<ref EnProfilerTimeClassPair> timePerClass = {};
		EnProfiler.GetTimePerClass(timePerClass, 20);

		// In this example the array will be filled with all classes sorted by time
		array<ref EnProfilerTimeClassPair> timePerClass2 = {};
		EnProfiler.GetTimePerClass(timePerClass2);
	@endcode
	*/
	static proto void GetTimePerClass(notnull out array<ref EnProfilerTimeClassPair> outArr, int count = int.MAX);
	
	/**
	\brief Obtain [SD] for Allocations Per Class
		\param outArr \p array<ref EnProfilerCountClassPair> Array sorted by number of allocations of a class
		\param count \p int The maximum amount of entries wanted
	
	@code
		array<ref EnProfilerCountClassPair> allocPerClass = {};
		EnProfiler.GetAllocationsPerClass(allocPerClass, 20);
	@endcode
	*/
	static proto void GetAllocationsPerClass(notnull out array<ref EnProfilerCountClassPair> outArr, int count = int.MAX);
	
	/**
	\brief Obtain [SD] for Instances Per Class
		\param outArr \p array<ref EnProfilerCountClassPair> Array sorted by number of instances of a class
		\param count \p int The maximum amount of entries wanted
	
	@code
		array<ref EnProfilerCountClassPair> instancesPerClass = {};
		EnProfiler.GetInstancesPerClass(instancesPerClass, 20);
	@endcode
	*/
	static proto void GetInstancesPerClass(notnull out array<ref EnProfilerCountClassPair> outArr, int count = int.MAX);
	
	/**
	\brief Obtain [SD] for Time Per Function
		\param outArr \p array<ref EnProfilerTimeFuncPair> Array sorted by time consumed by a function
		\param count \p int The maximum amount of entries wanted
	
	@code
		array<ref EnProfilerTimeFuncPair> timePerFunc = {};
		EnProfiler.GetTimePerFunc(timePerFunc, 20);
	@endcode
	*/
	static proto void GetTimePerFunc(notnull out array<ref EnProfilerTimeFuncPair> outArr, int count = int.MAX);
	
	/**
	\brief Obtain [SD] for Count Per Function
		\param outArr \p array<ref EnProfilerCountFuncPair> Array sorted by amount of times a function was called
		\param count \p int The maximum amount of entries wanted
	
	@code
		array<ref EnProfilerCountFuncPair> countPerFunc = {};
		EnProfiler.GetCountPerFunc(countPerFunc, 20);
	@endcode
	*/
	static proto void GetCountPerFunc(notnull out array<ref EnProfilerCountFuncPair> outArr, int count = int.MAX);
	
	//@}
	
	
	
	/** \name Specific data
		Set of functions to obtain specific data
	*/
	//@{
	
	/**
	\brief Obtain [SD] or [PD] regarding the time a specific class consumed
		\param clss \p typename Typename of desired class
		\param immediate \p bool When true, it will pull from [SD], when false it will pull from [PD]
		\return \p float Time consumed by the specified class
	
	@code
		// Consider the class
		EPTHelperClass clss = new EPTHelperClass();

		// Some functions being called here...
	
		// Gathering of data can be done through
		float timeOfClass = EnProfiler.GetTimeOfClass(clss.Type(), true);

		// Or when you have no variable/reference
		float timeOfClass2 = EnProfiler.GetTimeOfClass(StaticGetType(EPTHelperClass), true);
	@endcode
	*/
	static proto float GetTimeOfClass(typename clss, bool immediate = false);
	
	/**
	\brief Obtain [SD] or [PD] regarding the allocations of a specific class
		\param clss \p typename Typename of desired class
		\param immediate \p bool When true, it will pull from [SD], when false it will pull from [PD]
		\return \p int Allocations of the specified class
	
	@code
		int allocationsOfClass = EnProfiler.GetAllocationsOfClass(StaticGetType(EPTHelperClass), true);
	@endcode
	*/
	static proto int GetAllocationsOfClass(typename clss, bool immediate = false);
	
	/**
	\brief Obtain [SD] or [PD] regarding the [CI] of a specific class
		\param clss \p typename Typename of desired class
		\param immediate \p bool When true, it will pull from [SD], when false it will pull from [PD]
		\return \p int [CI] of the specified class
	
	@code
		int instancesOfClass = EnProfiler.GetInstancesOfClass(StaticGetType(EPTHelperClass), true);
	@endcode
	*/
	static proto int GetInstancesOfClass(typename clss, bool immediate = false);
	
	/**
	\brief Obtain [SD] or [PD] regarding the time consumed by a specific function
		\param funct \p string Function name
		\param clss \p typename Typename of class the function belongs to
		\param immediate \p bool When true, it will pull from [SD], when false it will pull from [PD]
		\return \p float Time consumed by the specified function or -1 when function was not found
	
	@code
		float timeOfFunc = EnProfiler.GetTimeOfFunc("StringFormat", StaticGetType(EnProfilerTests), true);
	@endcode
	*/
	static proto float GetTimeOfFunc(string funct, typename clss, bool immediate = false);
	
	/**
	\brief Obtain [SD] or [PD] regarding the time consumed by a specific global function
		\param funct \p string Function name
		\param immediate \p bool When true, it will pull from [SD], when false it will pull from [PD]
		\return \p float Time consumed by the specified function or -1 when function was not found
	
	@code
		float timeOfFunc = EnProfiler.GetTimeOfFuncG("ErrorEx", true);
	@endcode
	*/
	static proto float GetTimeOfFuncG(string funct, bool immediate, bool immediate = false);
	
	/**
	\brief Obtain [SD] or [PD] regarding the amount of times a specific function was called
		\param funct \p string Function name
		\param clss \p typename Typename of class the function belongs to
		\param immediate \p bool When true, it will pull from [SD], when false it will pull from [PD]
		\return \p int Amount of calls to the specified function or -1 when function was not found
	
	@code
		int callCountOfFunc = EnProfiler.GetCountOfFunc("StringFormat", StaticGetType(EnProfilerTests), true);
	@endcode
	*/
	static proto int GetCountOfFunc(string funct, typename clss, bool immediate = false);
	
	/**
	\brief Obtain [SD] or [PD] regarding the amount of times a specific function was called
		\param funct \p string Function name
		\param immediate \p bool When true, it will pull from [SD], when false it will pull from [PD]
		\return \p int Amount of calls to the specified function or -1 when function was not found
	
	@code
		int callCountOfFunc = EnProfiler.GetCountOfFuncG("ErrorEx", true);
	@endcode
	*/
	static proto int GetCountOfFuncG(string funct, bool immediate = false);
	
	//@}
	
	
	
	/** \name Misc
		Set of helper functions
	*/
	//@{
	
	/**
	\brief Helper method to ascertain the profiler will record [PD] right after this call
		\return \p bool Whether it was enabled before or not
	
	@code
		bool wasEnabled = EnProfiler.RequestImmediateData();
	@endcode
	*/
	static bool RequestImmediateData()
	{
		// I only care if it is actually profiling right now, so C
		bool wasEnabled = IsEnabledC();
		
		if (!wasEnabled)
		{
			// I want the data, and I want it now, so immediate
			Enable(true, true);
		}
		
		return wasEnabled;
	}
	
	//@}
};

 //@}
