//#define DIAG_NOTIFIER_LOGS
enum eNotifiers
{
	NTF_HEALTHY,
	NTF_BLEEDISH,
	NTF_HUNGRY,
	NTF_THIRSTY,
	NTF_STUFFED,
	NTF_SICK,
	NTF_WETNESS,
	NTF_WARMTH,
	NTF_FEVERISH,
	NTF_BLOOD,
	NTF_LIVES,
	NTF_STAMINA,
	//NTF_AGENT_INFECTION,
	NTF_PILLS,
	NTF_HEARTBEAT,
	NTF_FRACTURE,
	NTF_LEGS,
	//----------------------------
	NTF_COUNT,// !!! LAST ITEM !!!
}

class NotifiersManager
{
	static const int 					MAX_COUNT = 64;
	ref array<ref NotifierBase> 		m_Notifiers;
	NotifierBase 						m_NotifiersStatic[MAX_COUNT]; //introduced as a seperate array to allow for fast lookup, keeping the old one for quick looping through but also to keep modding compatibility
	PlayerBase							m_Player;
	ref VirtualHud						m_VirtualHud;
	int									m_MinTickTime;
	string 								m_System = "Notifiers";
	private int 						m_LastPolledIndex = 0;
	private ref array<int> 				m_NotifierIDs = new array<int>();
	
	void NotifiersManager(PlayerBase player)
	{
		m_Player = player;
		m_Notifiers = new array<ref NotifierBase>;
		m_MinTickTime = MIN_TICK_NOTIFIERS;
		m_LastPolledIndex = 0;
		
		Init();
	}

	void Init()
	{
		m_Notifiers.Insert(new HealthNotfr(this));
		m_Notifiers.Insert(new HungerNotfr(this));
		m_Notifiers.Insert(new ThirstNotfr(this));
		m_Notifiers.Insert(new StuffedNotfr(this));
		m_Notifiers.Insert(new SickNotfr(this));
		m_Notifiers.Insert(new WetnessNotfr(this));
		m_Notifiers.Insert(new WarmthNotfr(this));
		m_Notifiers.Insert(new FeverNotfr(this));
		m_Notifiers.Insert(new BloodNotfr(this));
		m_Notifiers.Insert(new PillsNotfr(this));
		m_Notifiers.Insert(new HeartbeatNotfr(this));
		m_Notifiers.Insert(new FracturedLegNotfr(this));
		m_Notifiers.Insert(new InjuredLegNotfr(this));
	}
	
	void RegisterItself(int notifier_id, NotifierBase modifier)
	{
		if (notifier_id >= MAX_COUNT)
		{
			ErrorEx("Out of bounds for notifier id: " + notifier_id, ErrorExSeverity.ERROR);
		}
		else
		{
			m_NotifiersStatic[notifier_id] = modifier;
			m_NotifierIDs.Insert(notifier_id);
			#ifdef DIAG_NOTIFIER_LOGS
			ErrorEx(string.Format("Added notifier %1 with id=%2", modifier, notifier_id), ErrorExSeverity.INFO);
			#endif
		}
	}
	
	PlayerBase GetPlayer()
	{
		return m_Player;
	}

	VirtualHud GetVirtualHud()
	{
		return m_VirtualHud;
	}
	
	NotifierBase FindNotifier(int type)
	{
		return m_NotifiersStatic[type];
	}

	void ActivateByType(int notifier, bool triggerEvent = true)
	{
		FindNotifier(notifier).SetActive(true);
	}

	void DeactivateByType(int notifier, bool triggerEvent = true)
	{
		FindNotifier(notifier).SetActive(false);
	}

	void OnScheduledTick()
	{
		if (!GetPlayer().IsPlayerSelected())
			return;
		
		TickNotifiers();
	}
	
	void TickNotifiers()
	{
		int notifierCount = m_Notifiers.Count();
		#ifdef DIAG_NOTIFIER_LOGS
		if (notifierCount == 0)
		{
			ErrorEx("Notifier count is 0", ErrorExSeverity.ERROR);
			return;
		}
		#endif
		
		// Wrap around if we've reached the end
		if (m_LastPolledIndex >= notifierCount)
		{
			#ifdef DIAG_NOTIFIER_LOGS
			ErrorEx(string.Format("Last poll index is %1 and notifiers count is %2. Restet poll index!", m_LastPolledIndex, notifierCount), ErrorExSeverity.INFO);
			#endif
			m_LastPolledIndex = 0;
		}
		
		int notifierID = m_NotifierIDs[m_LastPolledIndex];
		#ifdef DIAG_NOTIFIER_LOGS
		ErrorEx(string.Format("Got notifier ID %1 for poll index %2.", notifierID, m_LastPolledIndex), ErrorExSeverity.INFO);
		#endif
		// Get current notifier to process
		NotifierBase currentNotifier = m_NotifiersStatic[notifierID];
		if (currentNotifier && currentNotifier.IsActive())
		{
			#ifdef DIAG_NOTIFIER_LOGS
			ErrorEx(string.Format("Got notifier %1 with ID %2.", currentNotifier, notifierID), ErrorExSeverity.INFO);
			#endif
			int currentTime = g_Game.GetTime();
			
			// Only tick if it's time (using the notifier's own interval)
			if (currentNotifier.IsTimeToTick(currentTime))
			{
				#ifdef DIAG_NOTIFIER_LOGS
				ErrorEx(string.Format("Time to tick notifier %1.", currentNotifier), ErrorExSeverity.INFO);
				#endif
				currentNotifier.OnTick(currentTime);
			}
			#ifdef DIAG_NOTIFIER_LOGS
			else
			{
				ErrorEx(string.Format("Skip tick of notifier %1 ..", currentNotifier), ErrorExSeverity.INFO);
			}
			#endif
		}
		#ifdef DIAG_NOTIFIER_LOGS
		else
		{
			if (!currentNotifier)
				ErrorEx(string.Format("Could not get any notifier with ID %1.", notifierID), ErrorExSeverity.ERROR);
			else
				ErrorEx(string.Format("Could get notifier %1 with ID %2 but notifier is inactive!", currentNotifier, notifierID), ErrorExSeverity.INFO);
		}
		#endif
		
		// Move to next notifier for next tick
		m_LastPolledIndex++;
	}
}