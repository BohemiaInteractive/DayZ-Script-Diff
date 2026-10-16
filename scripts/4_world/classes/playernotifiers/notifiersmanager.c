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
	NTF_COUNT// !!! LAST ITEM !!!

}

class NotifiersManager
{
	static const int				MAX_COUNT = 64;

	ref array<ref NotifierBase>		m_Notifiers;
	NotifierBase					m_NotifiersStatic[MAX_COUNT]; //! Fast lookup, kept for compatibility
	PlayerBase						m_Player;
	ref VirtualHud					m_VirtualHud;
	int								m_MinTickTime;
	string							m_System = "Notifiers";

	//! Round-robin index
	private int m_LastPolledIndex = 0;

	//! Notifier ids in registration order
	private ref array<int> m_NotifierIDs = new array<int>();

	//! Event-driven immediate refreshes
	//! Processed before normal round-robin, but still only one notifier per manager tick
	private ref array<int> m_ImmediateNotifierIDs = new array<int>();
	private bool m_ImmediatePending[MAX_COUNT];

	void NotifiersManager(PlayerBase player)
	{
		m_Player = player;
		m_Notifiers = new array<ref NotifierBase>;
		m_MinTickTime = MIN_TICK_NOTIFIERS;
		m_LastPolledIndex = 0;

		for (int i = 0; i < MAX_COUNT; i++)
		{
			m_ImmediatePending[i] = false;
		}

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
		m_Notifiers.Insert(new FracturedLegNotfr(this));
		m_Notifiers.Insert(new InjuredLegNotfr(this));
	}

	protected bool IsNotifierIDValid(int notifier_id)
	{
		return notifier_id >= 0 && notifier_id < MAX_COUNT;
	}

	void RegisterItself(int notifier_id, NotifierBase modifier)
	{
		if (!IsNotifierIDValid(notifier_id))
		{
			ErrorEx("Out of bounds for notifier id: " + notifier_id, ErrorExSeverity.ERROR);
			return;
		}

		m_NotifiersStatic[notifier_id] = modifier;

		if (m_NotifierIDs.Find(notifier_id) == -1)
		{
			m_NotifierIDs.Insert(notifier_id);
		}

	#ifdef DIAG_NOTIFIER_LOGS
		ErrorEx(string.Format("Added notifier %1 with id=%2", modifier, notifier_id), ErrorExSeverity.INFO);
	#endif
	}

	PlayerBase GetPlayer()
	{
		return m_Player;
	}

	VirtualHud GetVirtualHud()
	{
		if (!m_VirtualHud)
		{
			m_VirtualHud = m_Player.GetVirtualHud();
		}

		return m_VirtualHud;
	}

	NotifierBase FindNotifier(int type)
	{
		if (!IsNotifierIDValid(type))
		{
	    #ifdef DIAG_NOTIFIER_LOGS
			ErrorEx(string.Format("Invalid notifier id requested: %1.", type), ErrorExSeverity.ERROR);
	    #endif
			return null;
		}

		return m_NotifiersStatic[type];
	}

	bool HasNotifier(int type)
	{
		return FindNotifier(type) != null;
	}

	bool IsNotifierActive(int type)
	{
		NotifierBase notifier = FindNotifier(type);
		if (!notifier)
			return false;

		return notifier.IsActive();
	}

	protected void ClearImmediatePending(int notifier_id)
	{
		if (!IsNotifierIDValid(notifier_id))
			return;

		m_ImmediatePending[notifier_id] = false;
	}

	void RequestImmediateUpdate(int notifier_id)
	{
		if (!IsNotifierIDValid(notifier_id))
			return;

		NotifierBase notifier = FindNotifier(notifier_id);
		if (!notifier || !notifier.IsActive())
			return;

		if (m_ImmediatePending[notifier_id])
			return;

		m_ImmediatePending[notifier_id] = true;
		m_ImmediateNotifierIDs.Insert(notifier_id);

	#ifdef DIAG_NOTIFIER_LOGS
		ErrorEx(string.Format("Queued immediate notifier update for id=%1.", notifier_id), ErrorExSeverity.INFO);
	#endif
	}

	bool SetNotifierActive(int notifier_id, bool state, bool immediate = false)
	{
		NotifierBase notifier = FindNotifier(notifier_id);
		if (!notifier)
		{
	    #ifdef DIAG_NOTIFIER_LOGS
			ErrorEx(string.Format("SetNotifierActive failed, notifier %1 not found.", notifier_id), ErrorExSeverity.ERROR);
	    #endif
			return false;
		}

		bool changed = (notifier.IsActive() != state);

		notifier.SetActive(state);

		if (!state)
		{
			ClearImmediatePending(notifier_id);
		}
		else if (changed && immediate)
		{
			RequestImmediateUpdate(notifier_id);
		}

		return changed;
	}

	void ActivateByType(int notifier, bool triggerEvent = true)
	{
		//! Request one immediate refresh on activation
		SetNotifierActive(notifier, true, triggerEvent);
	}

	void DeactivateByType(int notifier, bool triggerEvent = true)
	{
		SetNotifierActive(notifier, false, false);
	}

	protected bool ProcessImmediateNotifier(int currentTime)
	{
		while (m_ImmediateNotifierIDs.Count() > 0)
		{
			int lastIndex = m_ImmediateNotifierIDs.Count() - 1;
			int notifierID = m_ImmediateNotifierIDs.Get(lastIndex);
			m_ImmediateNotifierIDs.Remove(lastIndex);
			m_ImmediatePending[notifierID] = false;

			NotifierBase currentNotifier = m_NotifiersStatic[notifierID];
			if (!currentNotifier || !currentNotifier.IsActive())
			{
	        #ifdef DIAG_NOTIFIER_LOGS
				ErrorEx(string.Format("Skipping immediate notifier id=%1 (missing or inactive).", notifierID), ErrorExSeverity.INFO);
	        #endif
				continue;
			}

	    #ifdef DIAG_NOTIFIER_LOGS
			ErrorEx(string.Format("Immediate tick for notifier %1 with ID %2.", currentNotifier, notifierID), ErrorExSeverity.INFO);
	    #endif

			currentNotifier.OnTick(currentTime);
			return true;
		}

		return false;
	}

	protected void TickSingleNotifier(int currentTime)
	{
		int notifierCount = m_NotifierIDs.Count();

	#ifdef DIAG_NOTIFIER_LOGS
		if (notifierCount == 0)
		{
			ErrorEx("Notifier count is 0", ErrorExSeverity.ERROR);
			return;
		}
	#endif

		if (m_LastPolledIndex >= notifierCount)
		{
	    #ifdef DIAG_NOTIFIER_LOGS
			ErrorEx(string.Format("Last poll index is %1 and notifier id count is %2. Reset poll index.", m_LastPolledIndex, notifierCount), ErrorExSeverity.INFO);
	    #endif
			m_LastPolledIndex = 0;
		}

		int notifierID = m_NotifierIDs[m_LastPolledIndex];

	#ifdef DIAG_NOTIFIER_LOGS
		ErrorEx(string.Format("Got notifier ID %1 for poll index %2.", notifierID, m_LastPolledIndex), ErrorExSeverity.INFO);
	#endif

		NotifierBase currentNotifier = m_NotifiersStatic[notifierID];

		if (currentNotifier && currentNotifier.IsActive())
		{
	    #ifdef DIAG_NOTIFIER_LOGS
			ErrorEx(string.Format("Got notifier %1 with ID %2.", currentNotifier, notifierID), ErrorExSeverity.INFO);
	    #endif

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

		m_LastPolledIndex++;
	}

	void OnScheduledTick()
	{
		if (!GetPlayer().IsPlayerSelected())
			return;

		TickNotifiers();
	}

	void TickNotifiers()
	{
		int currentTime = g_Game.GetTime();

		//! Process exactly one immediate notifier OR process exactly one normal round-robin notifier
		if (ProcessImmediateNotifier(currentTime))
			return;

		TickSingleNotifier(currentTime);
	}
}