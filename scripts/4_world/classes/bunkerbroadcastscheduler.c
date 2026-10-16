class BunkerBroadcastScheduler : Managed
{
	protected bool m_IsActivePeriod;
	protected int m_BatchSelectionTimer;
	protected bool m_BatchSelected;
	protected int m_BatchLifetimeTimer;
	
	protected ref TStringArray m_ReportData;
	protected ref TStringArray m_MorseOutputData;

	protected BunkerBroadcastHandler m_Handler;
	protected BunkerBroadcastConfigInstance m_Config;
	protected BunkerBroadcastPersistenceStorage m_PersitenceStorage;
	
	protected ref BunkerComponentsArray m_ActiveBunkers;
	protected ref ScriptInvoker m_OnAskForNewBunkers;
	
	private int m_LastReportHour, m_LastReportMinute;
	private int m_LastReportDifferenceMinutes;
	
	private bool m_ForceBroadcast;
	
	private static ref BunkerBroadcastScheduler m_Instance;
	
	private ref WorldTimeTracker m_WorldTimeTracker;
	
	static BunkerBroadcastScheduler GetInstance(notnull BunkerBroadcastHandler handler)
	{
		if (!m_Instance)
			m_Instance = new BunkerBroadcastScheduler(handler);
		
		return m_Instance;
	}
	
	private void BunkerBroadcastScheduler(notnull BunkerBroadcastHandler handler)
	{
		m_Handler = handler;
	
		m_PersitenceStorage = BunkerBroadcastPersistenceStorage.GetInstance();
		m_ActiveBunkers 	= new BunkerComponentsArray();
		m_ReportData 		= new TStringArray();
		m_MorseOutputData   = new TStringArray();

		m_OnAskForNewBunkers = new ScriptInvoker();

		m_Config = CfgBunkerBroadcastHandler.GetData();
		
		m_WorldTimeTracker = new WorldTimeTracker();
		
		if (m_PersitenceStorage.IsLoaded())
		{
			LoadLastReportTimeFromStorage();
			m_LastReportDifferenceMinutes = GetDifferenceFromLastReport();
			PrintToRPT("Difference in minutes: " + m_LastReportDifferenceMinutes);
			// TODO@RR force broadcast on server restart if time condition passes
			//if (m_LastReportDifferenceMinutes > THRESHOLD(time calculated from the overall period and number of broadcasts))
			if (m_LastReportDifferenceMinutes > 10)
				m_ForceBroadcast = true;
		}
		else
			PrintToRPT(string.Format("storage does not exist - hour:%1 minute:%2", m_LastReportHour, m_LastReportMinute));
		
		if (m_Handler)
			m_Handler.GetOnActiveBunkersSelected().Insert(BatchSelected);
	}
	
	void ~BunkerBroadcastScheduler()
	{
		if (m_Handler)
			m_Handler.GetOnActiveBunkersSelected().Remove(BatchSelected);
	}
	
	// --------------------------------------------------------------------------------
	
	ScriptInvoker GetOnAskForNewBunkers()
	{
		return m_OnAskForNewBunkers;
	}
	
	// --------------------------------------------------------------------------------

	void CheckActivePeriod()
	{
	    int year, month, day, hour, minute;
	    GetGame().GetWorld().GetDate(year, month, day, hour, minute);
	
	    // Convert to minutes from midnight for simpler comparisons
	    int now = hour * 60 + minute;
	
	    foreach (BunkerBroadcastPeriod period : m_Config.schedulerSettings.periods)
	    {
	        int start = period.GetStartHour() * 60 + period.GetStartMinute();
	        int end   = period.GetEndHour() * 60 + period.GetEndMinute();
	
	        bool active = false;
	
	        if (start < end)
	        {
	            // Same-day range: [start, end]
	            active = (now >= start) && (now <= end);
	        }
	        else if (start > end)
	        {
	            // Wrap-around range: [start, 24h) U [0, end]
	            active = (now >= start) || (now <= end);
	        }
	        else
	        {
	            // start == end
	            // If minutes equal, interpret as full-day active; otherwise zero-length window (inactive)
	            active = true;
	        }
	
	        if (active)
	        {
	            m_IsActivePeriod = true;
	            return;
	        }
	    }
	
	    m_IsActivePeriod = false;
	}

	// --------------------------------------------------------------------------------

	bool IsActivePeriod()
	{
		return m_IsActivePeriod;
	}

	// --------------------------------------------------------------------------------

	void OnUpdate(float timeslice)
	{
		if (!m_IsActivePeriod)
		{
			m_BatchSelected = false;
			return;
		}
		
		int timeTrackerDeltaMinutes;
		m_WorldTimeTracker.TryPollDelta(timeslice, timeTrackerDeltaMinutes);
		m_BatchLifetimeTimer += timeTrackerDeltaMinutes;
		//! batch lifetime control
		if (m_BatchLifetimeTimer >= m_Config.schedulerSettings.batchLifetimeLength)
		{
			m_BatchSelected = false;
			SelectNewBunkers();
			m_BatchLifetimeTimer = 0;
		}

		//! batch not yet selected and distributed
		if (!m_BatchSelected)
		{
			m_BatchSelectionTimer += timeTrackerDeltaMinutes;
			//! fixed bunker selection call each 5mins if no batch is selected
			if(m_BatchSelectionTimer >= 5)
			{
				#ifdef ENABLE_LOGGING
				Log("Enforcing batch selection - inactivity");
				#endif
				m_BatchSelectionTimer = 0;
				SelectNewBunkers();
			}
		}

		if (m_ForceBroadcast || m_LastReportDifferenceMinutes >= m_Config.schedulerSettings.reportInterval)
		{
			if (SendReport(m_ReportData, m_MorseOutputData))
				UpdatePersistentData();
			
			m_ForceBroadcast = false;
		}
		
		m_LastReportDifferenceMinutes = GetDifferenceFromLastReport();
	}

	// --------------------------------------------------------------------------------
	
	private void SelectNewBunkers()
	{
		m_OnAskForNewBunkers.Invoke();
	}

	// --------------------------------------------------------------------------------

	private void BatchSelected()
	{
		m_BatchSelected = true;
		m_ActiveBunkers = m_Handler.GetActiveBunkers();
		UpdatePersistentData();

		m_ReportData = PrepareReport();
		if (SendReport(m_ReportData, m_MorseOutputData))
			UpdatePersistentData();
	}

	// --------------------------------------------------------------------------------

	private TStringArray PrepareReport()
	{
		TStringArray results = new TStringArray();
		foreach (BunkerBroadcastComponentBase active : m_ActiveBunkers)
		{
			TIntArray mapCoords = MapNavigationBehaviour.OrderedPositionNumbersFromGridCoords(active.GetPosition());
			TIntArray xCoords = mapCoords.Slice(0, 2);
			TIntArray yCoords = mapCoords.Slice(3, 5);
			TStringArray xCoordsString = new TStringArray();
			TStringArray yCoordsString = new TStringArray();
			foreach(int ix : xCoords)
				xCoordsString.Insert(ix.ToString());
			
			foreach(int iy : yCoords)
				yCoordsString.Insert(iy.ToString());
			
			string mapCoordsConverted;
			mapCoordsConverted = string.Join("", xCoordsString);
			mapCoordsConverted += string.Join("", yCoordsString);
			results.Insert(mapCoordsConverted);
			results.Insert(active.GetLockCode());
		}
		
		return results;
	}
	
	// --------------------------------------------------------------------------------
	
	private bool SendReport(TStringArray data, TStringArray morseData)
	{
		if (data.Count() == 0)
		{
			//#ifdef ENABLE_LOGGING
			//Log("Nothing to report.");
			//#endif
			return false;
		}
		
		BunkerBroadcastManager bbm = GetGame().GetBunkerBroadcastManager();
		if (bbm) 
		{			
			bbm.SetMessageSpeed(
				m_Config.playbackSettings.dotDuration,
				m_Config.playbackSettings.dashDuration,
				m_Config.playbackSettings.symbolGapDuration,
				m_Config.playbackSettings.wordGapDuration,
			);
			bbm.SetPauseBetweenBunkers(m_Config.playbackSettings.pauseBetweenBunkers);
			bbm.SetBroadcastBunkerRepeatCount(m_Config.playbackSettings.bunkerRepetitions);
			bbm.SetBroadcastTotalRepeatCount(m_Config.playbackSettings.messageRepetitions);
		
			bbm.BroadcastTextAsMorse(m_ReportData, m_MorseOutputData);
			
			#ifdef ENABLE_LOGGING
			Log("Reporting active bunkers:");
			int i;
			for (i = 0; i < data.Count(); i += 2)
			{
				Log(string.Format("position:%1 code:%2", data[i], data[i+1]), true);
			}
			for (i = 0; i < m_MorseOutputData.Count(); i += 2)
			{
				Log(string.Format("Morse position:%1 code:%2", m_MorseOutputData[i], m_MorseOutputData[i+1]), true);
			}
			
			Log("Report sent");
			#endif
			
			return true;
		}

		return false;
	}

	// --------------------------------------------------------------------------------
	
	private void UpdatePersistentData()
	{
		int year, month, day, hour, minute;
		g_Game.GetWorld().GetDate(year, month, day, hour, minute);

		m_PersitenceStorage.m_SchedulerData.m_LastReportHour 	= hour;
		m_PersitenceStorage.m_SchedulerData.m_LastReportMinute 	= minute;
		
		TIntStringMap activeBunkerTempMap = new TIntStringMap();
		m_PersitenceStorage.m_HandlerData.m_LastActiveBunkers.Clear();
		foreach (BunkerBroadcastComponentBase bunker : m_ActiveBunkers)
		{
			activeBunkerTempMap.Insert(bunker.GetId(), bunker.GetLockCode());
		}
		
		m_PersitenceStorage.m_HandlerData.m_LastActiveBunkers.Copy(activeBunkerTempMap);
		m_PersitenceStorage.Save();

		m_LastReportHour 	= hour;
		m_LastReportMinute 	= minute;
		
		m_LastReportDifferenceMinutes = 0.0;
	}

	// --------------------------------------------------------------------------------
	
	private void LoadLastReportTimeFromStorage()
	{
		m_LastReportHour 	= m_PersitenceStorage.m_SchedulerData.m_LastReportHour;
		m_LastReportMinute 	= m_PersitenceStorage.m_SchedulerData.m_LastReportMinute;
		PrintToRPT(string.Format("storage loaded - hour:%1 minute:%2", m_LastReportHour, m_LastReportMinute));
	}

	// --------------------------------------------------------------------------------

	private int GetDifferenceFromLastReport()
	{	
		int year, month, day, hour, minute;
		GetGame().GetWorld().GetDate(year, month, day, hour, minute);
		
		int prevTotalMinutes = m_LastReportHour * 60 + m_LastReportMinute;
		int currTotalMinutes = hour * 60 + minute;
		
		int minuteDiff = currTotalMinutes - prevTotalMinutes;
		
		// Handle wrap-around (e.g., prev=23:55, curr=00:10 → +15m)
		if (minuteDiff < -720) // -12 hours or more, must be a forward rollover
		    minuteDiff += 1440; // add 1 day
		else if (minuteDiff > 720) // +12 hours or more, must be a backward rollover
		    minuteDiff -= 1440; // subtract 1 day

		return minuteDiff;
	}
	
	#ifdef ENABLE_LOGGING
	private static void Log(string message, bool indented = false)
	{
		#ifdef DIAG_DEVELOPER
		if (BunkerBroadcastHandler.m_LoggingEnabled || LogManager.IsRadioGameplayLogEnabled())
		#else
		if (LogManager.IsRadioGameplayLogEnabled())
		#endif
		{
			string indentation = "";
			if (indented)
				indentation = " \\__";

			Debug.RadioGameplayLog(string.Format("%1 %2", indentation, message), m_Instance.ClassName(), "n/a", "n/a", "n/a");
		}
	}
	#endif	
}
