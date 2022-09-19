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
	//----------------------------
	NTF_COUNT,// !!! LAST ITEM !!!

}


class NotifiersManager
{
	static const int 				MAX_COUNT = 64;
	ref array<ref NotifierBase> 	m_Notifiers;
	ref NotifierBase 				m_NotifiersStatic[MAX_COUNT];//introduced as a seperate array to allow for fast lookup, keeping the old one for quick looping through but also to keep modding compatibility
	PlayerBase						m_Player;
	ref VirtualHud					m_VirtualHud;
	int								m_MinTickTime;
	string 							m_System = "Notifiers";
	
	void NotifiersManager(PlayerBase player)
	{
		m_Player = player;

		m_Notifiers = new array<ref NotifierBase>;

		m_MinTickTime = MIN_TICK_NOTIFIERS;
		Init();
	}

	void Init()
	{
		m_Notifiers.Insert(new HungerNotfr(this));
		m_Notifiers.Insert(new ThirstNotfr(this));
		m_Notifiers.Insert(new WarmthNotfr(this));
		m_Notifiers.Insert(new WetnessNotfr(this));
		m_Notifiers.Insert(new HealthNotfr(this));
		m_Notifiers.Insert(new FeverNotfr(this));
		m_Notifiers.Insert(new SickNotfr(this));
		m_Notifiers.Insert(new StuffedNotfr(this));
		m_Notifiers.Insert(new BloodNotfr(this));
		m_Notifiers.Insert(new PillsNotfr(this));
		m_Notifiers.Insert(new HeartbeatNotfr(this));
		m_Notifiers.Insert(new FracturedLegNotfr(this)); //New addition
		
	}
	
	void RegisterItself(int notifier_id, NotifierBase modifier)
	{
		if(notifier_id >= MAX_COUNT)
			Error("out of bounds for notifier id: " + notifier_id);
		else
			m_NotifiersStatic[notifier_id] = modifier;
	}
	
	PlayerBase GetPlayer()
	{
		return m_Player;
	}

	VirtualHud GetVirtualHud()
	{
		return m_VirtualHud;
	}
	
	NotifierBase FindNotifier( int type )
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
		if( !GetPlayer().IsPlayerSelected() ) return;
		
		TickNotifiers();
	}

	void TickNotifiers()
	{
		int current_time = GetGame().GetTime();

		foreach(NotifierBase notifier: m_Notifiers)
		{
			if ( notifier.IsActive() && notifier.IsTimeToTick(current_time) )
			{
				notifier.OnTick(current_time);
			}
		}		

	}

}