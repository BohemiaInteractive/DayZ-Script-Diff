//#ifdef DIAG_DEVELOPER
//#define BUNKER_BROADCAST_CHEAT_CODE
//#endif

class BunkerBroadcastSchedulerPersistenceData
{
	int m_LastReportHour 	= -1;
	int m_LastReportMinute 	= -1;
}

class BunkerBroadcastHandlerPersistenceData
{
	ref TIntStringMap m_LastActiveBunkers = new TIntStringMap();
}

class BunkerBroadcastPersistenceStorage : Managed
{
	private const string STORAGE_PATH = "$mission:BunkerBroadcastPersistenceStorage.bin";

	ref BunkerBroadcastSchedulerPersistenceData m_SchedulerData;
	ref BunkerBroadcastHandlerPersistenceData m_HandlerData;

	protected static ref BunkerBroadcastPersistenceStorage m_Instance;
	
	private bool m_Loaded;

	// --------------------------------------------------------------------------------

	private void BunkerBroadcastPersistenceStorage()
	{
		m_HandlerData 	= new BunkerBroadcastHandlerPersistenceData();
		m_SchedulerData = new BunkerBroadcastSchedulerPersistenceData();
		
		m_Loaded = Load();
	}

	// --------------------------------------------------------------------------------
	
	static BunkerBroadcastPersistenceStorage GetInstance()
	{
		if (!m_Instance)
			m_Instance = new BunkerBroadcastPersistenceStorage();
		
		return m_Instance;
	}

	// --------------------------------------------------------------------------------
	
	bool IsLoaded()
	{
		return m_Loaded;
	}
	
	// --------------------------------------------------------------------------------
	
	bool Load()
	{
		FileSerializer file = new FileSerializer();
		if (file.Open(STORAGE_PATH, FileMode.READ))
		{
			file.Read(m_HandlerData);
			file.Read(m_SchedulerData);
			file.Close();
			
			return true;
		}
		
		return false;
	}

	// --------------------------------------------------------------------------------
	
	void Save()
	{
		FileSerializer file = new FileSerializer();
		if (file.Open(STORAGE_PATH, FileMode.WRITE))
		{
			file.Write(m_HandlerData);
			file.Write(m_SchedulerData);
			file.Close();			
		}
	}
}

class BunkerBroadcastLockCodeMode
{
	const int DIGITSONLY = 0;
	const int ALPHANUMERIC = 1;
}

typedef array<ref BunkerBroadcastComponentBase> BunkerComponentsArray;

class BunkerBroadcastHandler : Managed
{
	protected const int LOCK_CODE_HISTORY_LENGTH = 5; 	//! how many of previously generated codes are stored (prevents the reuse)

	protected const int UPDATE_BATCH_SIZE 	= 10;	 	//! max batch size
	protected const float UPDATE_TIME 		= 10.0; 	//! how often is update called [seconds]

	protected ref map<int, ref BunkerBroadcastComponentBase> m_RegisteredBunkers;
	protected ref BunkerComponentsArray m_ActiveBunkers;
	protected ref map<int, ref BunkerBroadcastComponentBase> m_RegisterQueue;
	protected ref map<int, ref BunkerBroadcastComponentBase> m_UnregisterQueue;
	
	protected bool m_ConfigLoaded;
	protected bool m_CanUpdate;
	protected bool m_ProcessComponents;
	protected bool m_ProcessingFinished;
	protected int m_NextToProcessIdx;
	protected float m_UpdateTimer;

	protected BunkerBroadcastPersistenceStorage m_PersitenceStorage;
	protected BunkerBroadcastScheduler m_BroadcastScheduler;
	protected ref BunkerBroadcastConfigInstance m_Config;
	
	protected ref SimpleCircularBuffer<string> m_LockCodeHistory;
	protected ref SimpleCircularBuffer<int> m_ActiveBunkersHistory;
	
	protected ref ScriptInvoker m_OnActiveBunkersSelected;
	
	private static ref BunkerBroadcastHandler m_Instance;
	
	private int m_LastActiveBunkersCount;
	private int m_ActiveBunkersFromStorageCount;
	
	#ifdef DIAG_DEVELOPER
	static ref BunkerComponentsArray m_RegisteredBunkersStorage;
	static ref TIntArray m_BunkersHistoryStorage;
	static bool m_LoggingEnabled;
	#endif

	// --------------------------------------------------------------------------------
	
	static BunkerBroadcastHandler GetInstance()
	{
		if (!m_Instance)
			m_Instance = new BunkerBroadcastHandler();
		
		return m_Instance;
	}

	// --------------------------------------------------------------------------------

	private void BunkerBroadcastHandler()
	{
		m_ConfigLoaded = CfgBunkerBroadcastHandler.IsInitialized();
		if (!m_ConfigLoaded)
			return;

		m_Config = CfgBunkerBroadcastHandler.GetData();

		m_OnActiveBunkersSelected = new ScriptInvoker();
		
		m_LockCodeHistory 		= new SimpleCircularBuffer<string>(LOCK_CODE_HISTORY_LENGTH, "0000");	//! history of lock codes selected
		m_ActiveBunkersHistory 	= new SimpleCircularBuffer<int>(2, -1);									//! history of previously active bunkers

		m_ProcessingFinished 	= true;
		m_RegisteredBunkers 	= new map<int, ref BunkerBroadcastComponentBase>();
		m_ActiveBunkers 		= new BunkerComponentsArray();
		m_RegisterQueue 		= new map<int, ref BunkerBroadcastComponentBase>();
		m_UnregisterQueue 		= new map<int, ref BunkerBroadcastComponentBase>();
		m_NextToProcessIdx 		= 0;
		m_UpdateTimer 			= 0;
		
		#ifdef DIAG_DEVELOPER
		m_RegisteredBunkersStorage 	= new BunkerComponentsArray();
		m_BunkersHistoryStorage		= new TIntArray();
		#endif
		
		m_PersitenceStorage = BunkerBroadcastPersistenceStorage.GetInstance();
		m_LastActiveBunkersCount = m_PersitenceStorage.m_HandlerData.m_LastActiveBunkers.Count();

		// temporarily keeps number of bunkers added from storage
		m_ActiveBunkersFromStorageCount = 0;

		m_BroadcastScheduler = BunkerBroadcastScheduler.GetInstance(this);
		if (m_BroadcastScheduler)
		{
			m_BroadcastScheduler.GetOnAskForNewBunkers().Insert(SelectNew);
			m_BroadcastScheduler.CheckActivePeriod();
		}
	}
	
	void ~BunkerBroadcastHandler()
	{
		if (m_BroadcastScheduler)
			m_BroadcastScheduler.GetOnAskForNewBunkers().Remove(SelectNew);
	}

	// --------------------------------------------------------------------------------

	void Register(BunkerBroadcastComponentBase component)
	{
		if (!m_ConfigLoaded)
			return;

		//! already active (custom persistence storage)
		int componentId = component.GetId();
		string lockCode;
		if (m_PersitenceStorage.m_HandlerData.m_LastActiveBunkers.Find(componentId, lockCode))
		{
			// necessary to postpone while loaded and activated from storage
			Param2<string, bool> params = new Param2<string, bool>(lockCode, true);
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLaterByName(component, "OnActivate", 1000, false, params);

			m_RegisteredBunkers.Set(componentId, component);
			m_ActiveBunkers.Insert(component);

			#ifdef ENABLE_LOGGING
			Log(string.Format("Directly registering (store load) - object=%1, id=%2, pos=%3, code=%4", component, componentId, component.GetPosition(), lockCode));
			#endif
			
			m_ActiveBunkersFromStorageCount++;
	
			return;
		}
		
		if (m_RegisterQueue.Contains(componentId))
		{
			#ifdef ENABLE_LOGGING
			Log(string.Format("Already in register queue - object=%1, id=%2, pos=%3", component, componentId, component.GetPosition()));
			#endif	
			return;
		}
		
		m_RegisterQueue.Set(componentId, component);

		m_CanUpdate = true;
	}

	// --------------------------------------------------------------------------------

	void Unregister(BunkerBroadcastComponentBase component)
	{
		if (!m_ConfigLoaded)
			return;

		int componentId = component.GetId();
		if (m_UnregisterQueue.Contains(componentId))
		{
			#ifdef ENABLE_LOGGING
			Log(string.Format("Already in un-register queue - object=%1, id=%2, pos=%3", component, componentId, component.GetPosition()));
			#endif
			return;
		}

		m_UnregisterQueue.Set(componentId, component);
	}
	
	// --------------------------------------------------------------------------------
	
	ScriptInvoker GetOnActiveBunkersSelected()
	{
		return m_OnActiveBunkersSelected;
	}

	// --------------------------------------------------------------------------------
	
	BunkerComponentsArray GetRegisteredBunkers()
	{
		BunkerComponentsArray bunkers = new BunkerComponentsArray();
		if (m_RegisteredBunkers)
		{
			foreach (int id, BunkerBroadcastComponentBase component : m_RegisteredBunkers)
				bunkers.Insert(component);
		}

		return bunkers;
	}

	// --------------------------------------------------------------------------------
	
	TIntArray GetActiveBunkersHistory()
	{
		TIntArray history = new TIntArray();
		
		if (m_ActiveBunkersHistory)
			history = m_ActiveBunkersHistory.GetValues();
		
		return history;
	}
	
	// --------------------------------------------------------------------------------
	
	BunkerComponentsArray GetActiveBunkers()
	{
		return m_ActiveBunkers;
	}

	// --------------------------------------------------------------------------------
	
	void Update(float timeslice)
	{
		if (!m_ConfigLoaded)
			return;
		
		if (m_LastActiveBunkersCount == m_ActiveBunkersFromStorageCount && m_ActiveBunkersFromStorageCount > 0)
		{
			m_ActiveBunkersFromStorageCount = -1; // resets the count; no more processing
			m_OnActiveBunkersSelected.Invoke();
			m_CanUpdate = true;
			return;
		}

		if (!m_CanUpdate)
			return;

		if (m_BroadcastScheduler)
			m_BroadcastScheduler.OnUpdate(timeslice);
		
		if (m_ProcessComponents)
		{
			m_ProcessingFinished = ProcessBatch();
			
			if (m_ProcessingFinished)
			{
				int registeredCountRound = Math.Max(Math.Round(m_RegisteredBunkers.Count() / 2), m_Config.bunkerSettings.numberOfActive);
				if (registeredCountRound > 2)
					m_ActiveBunkersHistory.Resize(registeredCountRound, -1);

				Reset();
			}
		}
		else
		{
			m_UpdateTimer += timeslice;
			if (m_UpdateTimer >= UPDATE_TIME)
			{
				#ifdef ENABLE_LOGGING
				Log(string.Format("General update (%1s)", UPDATE_TIME));
				#endif
				
				HandleChangedComponents();
				CheckUpdating();
				
				if (!m_BroadcastScheduler.IsActivePeriod())
				{
					#ifdef ENABLE_LOGGING
					//Log("We are outside of active period, deactivating bunkers");
					#endif
					
					//! not active bunkers and storage has processed without results
					if (m_ActiveBunkers.Count() > 0 && m_ActiveBunkersFromStorageCount == 0)
						Deactivate();
				}			
		
				#ifdef DIAG_DEVELOPER
				BunkerComponentsArray bunkerList = GetRegisteredBunkers();
				TIntArray bunkersHistory = GetActiveBunkersHistory();
				SyncBunkersInfo(bunkerList, bunkersHistory);
				#endif

				if (m_BroadcastScheduler)
					m_BroadcastScheduler.CheckActivePeriod();

				m_ProcessComponents = m_CanUpdate;

				if (!m_CanUpdate)
					Reset();
			}
		}
	}

	// --------------------------------------------------------------------------------
	
	protected void SelectNew()
	{
		Deactivate();
		int numOfActivatedBunkers = Activate();

		if (numOfActivatedBunkers > 0)
			m_OnActiveBunkersSelected.Invoke();
		#ifdef DIAG_DEVELOPER
		else
			#ifdef ENABLE_LOGGING
			Log("No active bunker selected");
			#endif
		#endif
	}

	// --------------------------------------------------------------------------------
	
	private int Activate()
	{
		BunkerComponentsArray selectedBunkers = SelectRandomBunkers(GetRegisteredBunkers(), m_Config.bunkerSettings.numberOfActive);

		foreach (BunkerBroadcastComponentBase bunker : selectedBunkers)
		{
			string lockCode = GenerateLockCode();
			bunker.OnActivate(lockCode);
			m_ActiveBunkers.Insert(bunker);
		}
		
		return m_ActiveBunkers.Count();
	}

	// --------------------------------------------------------------------------------
	
	private void Deactivate()
	{
		BunkerComponentsArray toRemove = new BunkerComponentsArray();
		foreach (BunkerBroadcastComponentBase bunker : m_ActiveBunkers)
		{
			int componentId = bunker.GetId();
			#ifdef ENABLE_LOGGING
			Log(string.Format("Deactivating bunker:%1", componentId));
			#endif
			bunker.OnDeactivate();
			toRemove.Insert(bunker);

			if (m_RegisteredBunkers.Count() >= (m_Config.bunkerSettings.numberOfActive / 2))
				m_ActiveBunkersHistory.Add(bunker.GetId());
		}
		
		foreach (BunkerBroadcastComponentBase bunkerToRemove : toRemove)
			m_ActiveBunkers.RemoveItem(bunkerToRemove);
	}

	// --------------------------------------------------------------------------------
	
	protected string GenerateLockCode()
	{
		TStringArray lockCodeArray = new TStringArray();
		string lockCode;
		
		#ifdef DIAG_DEVELOPER
		#ifdef BUNKER_BROADCAST_CHEAT_CODE
		return "1234";
		#endif
		#endif

		bool needGenerate = true;
		//int iterationNumber = 0;
		while (needGenerate /*&& iterationNumber > <= max number of iterations?*/ )
		{
			//iterationNumber++;
			for (int i = 0; i < m_Config.bunkerSettings.lockCodeLength; ++i)
			{
				int charCode = GetRandomChar();
				lockCodeArray.Insert(charCode.AsciiToString());
			}

			lockCode = string.Join("", lockCodeArray);
			if (m_LockCodeHistory.GetValues().Find(lockCode) > INDEX_NOT_FOUND)
			{
				needGenerate = true;
				continue;
			}
			
			needGenerate = false;
			//iterationNumber = 0;
			break;
		}
		
		//if (iterationNumber > 0)
		//	lockCode = GetFallbackCodeFromStaticArray();
		
		return lockCode;
	}

	// --------------------------------------------------------------------------------
	
	//! returns 'true' when all the batches are finished
	protected bool ProcessBatch()
	{
		int count 	= m_RegisteredBunkers.Count();
		int target 	= (int)Math.Clamp(m_NextToProcessIdx + UPDATE_BATCH_SIZE, 0, count);
		
		TIntArray indices = m_RegisteredBunkers.GetKeyArray();

		for (m_NextToProcessIdx; m_NextToProcessIdx < target; m_NextToProcessIdx++)
		{
			if (m_RegisteredBunkers.Contains(indices[m_NextToProcessIdx]))
			{
				m_RegisteredBunkers[indices[m_NextToProcessIdx]].OnUpdate(m_UpdateTimer);
				BunkerBroadcastComponentBase component = m_RegisteredBunkers[indices[m_NextToProcessIdx]];
				component.OnUpdate(m_UpdateTimer);
			}
		}
		
		bool returnValue = target == count;
		if (returnValue)
			m_NextToProcessIdx = 0;
		
		return returnValue;
	}

	// --------------------------------------------------------------------------------
	
	protected void Reset()
	{
		m_ProcessComponents 	= false;
		m_ProcessingFinished 	= true;

		m_UpdateTimer = 0;
	}

	// --------------------------------------------------------------------------------
	
	protected void HandleChangedComponents()
	{
		foreach (int uid, BunkerBroadcastComponentBase componentToUnregister : m_UnregisterQueue)
		{
			if (!componentToUnregister)
			{
				#ifdef ENABLE_LOGGING
				Log(string.Format("Invalid bunker to unregister (%1).", componentToUnregister));
				#endif
				continue;
			}
			
			if (!m_RegisteredBunkers.Contains(uid))
			{
				#ifdef ENABLE_LOGGING
				Log(string.Format("Bunker (id=%1) is not registered, cannot unregister.", uid));
				#endif
				continue;
			}
			
			m_RegisteredBunkers.Remove(uid);
			m_UnregisterQueue.Remove(uid);
		}
		
		foreach (int rid, BunkerBroadcastComponentBase componentToRegister : m_RegisterQueue)
		{
			if (!componentToRegister)
			{
				#ifdef ENABLE_LOGGING
				Log(string.Format("Invalid bunker to register (%1)", componentToRegister));
				#endif
				continue;
			}
			
			if (m_RegisteredBunkers.Contains(rid))
			{
				#ifdef ENABLE_LOGGING
				Log(string.Format("Bunker (id=%1) is already registered", rid));
				#endif
				continue;
			}
			
			m_RegisteredBunkers.Set(rid, componentToRegister);
			m_RegisterQueue.Remove(rid);
		}
	}

	// --------------------------------------------------------------------------------

	#ifdef DIAG_DEVELOPER
	static void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
	{
		switch (rpc_type)
		{
			case ERPCs.DIAG_MISC_RADIOGAMEPLAY_BUNKERS_LIST:
				if (!m_RegisteredBunkersStorage)
					m_RegisteredBunkersStorage = new BunkerComponentsArray();

				if (!m_BunkersHistoryStorage)
					m_BunkersHistoryStorage = new TIntArray();
			
				m_RegisteredBunkersStorage.Clear();
				m_BunkersHistoryStorage.Clear();
				ctx.Read(m_RegisteredBunkersStorage);
				ctx.Read(m_BunkersHistoryStorage);
				break;
		}
	}

	// --------------------------------------------------------------------------------
	
	private void SyncBunkersInfo(BunkerComponentsArray bunkers, TIntArray bunkersHistory)
	{
		if (bunkers && bunkers.Count() > 0 )
		{
			ScriptRPC rpc = new ScriptRPC();
			rpc.Write(bunkers);
			rpc.Write(bunkersHistory);
			rpc.Send(null, ERPCs.DIAG_MISC_RADIOGAMEPLAY_BUNKERS_LIST, true, null);
		}
	}
	#endif
	
	#ifdef ENABLE_LOGGING
	private static void Log(string message, bool indented = false)
	{
		#ifdef DIAG_DEVELOPER
		if (m_LoggingEnabled || LogManager.IsRadioGameplayLogEnabled())
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

	// --------------------------------------------------------------------------------
	
	protected void CheckUpdating()
	{
		m_CanUpdate = m_RegisteredBunkers.Count() != 0;
	}
	
	// --------------------------------------------------------------------------------
	
	private int GetRandomChar()
    {
		int charSet = 0;
		if (m_Config.bunkerSettings.lockCodeMode == BunkerBroadcastLockCodeMode.ALPHANUMERIC)
			charSet = Math.RandomIntInclusive(0, 1);

        int charCode;

        if (charSet == 0)
            charCode = Math.RandomInt(48, 58); // Digit: '0' (48) to '9' (57)
        else
            charCode = Math.RandomInt(97, 123); // Lowercase: 'a' (97) to 'z' (122)
		
		return charCode;
	}

	// --------------------------------------------------------------------------------
	
	private BunkerComponentsArray SelectRandomBunkers(BunkerComponentsArray bunkers, int amount)
	{
	    BunkerComponentsArray available = new BunkerComponentsArray();
	
	    // Build list of available elements
		bunkers.ShuffleArray();
		#ifdef ENABLE_LOGGING
		Log("Random bunker selection:");
		#endif
	    foreach (BunkerBroadcastComponentBase bunker : bunkers)
	    {
			if (!bunker)
				continue;
	        if (m_ActiveBunkersHistory.GetValues().Find(bunker.GetId()) == INDEX_NOT_FOUND)
	            available.Insert(bunker);
			#ifdef ENABLE_LOGGING
			else
				Log(string.Format("Bunker was already used (%1)", bunker.GetId()), true);
			#endif
	    }
		
	    int n = available.Count();
	    if (amount > n)
	        amount = n; // Or handle error/return empty
	
	    BunkerComponentsArray selected = new BunkerComponentsArray();
	    TIntArray used = new TIntArray();

	    while (selected.Count() < amount)
	    {
	        int randomIndex = Math.RandomInt(0, n);
	        // Ensure unique selection
	        bool alreadyUsed = false;
	        foreach (int u : used)
	        {
	            if (u == randomIndex)
	            {
	                alreadyUsed = true;
	                break;
	            }
	        }

	        if (!alreadyUsed)
	        {
	            selected.Insert(available[randomIndex]);
				#ifdef ENABLE_LOGGING
				Log(string.Format("Selecting bunker (%1)", available[randomIndex].GetId()), true);
				#endif
	            used.Insert(randomIndex);
	        }
	    }
		
	    return selected;
	}

	// --------------------------------------------------------------------------------
}
