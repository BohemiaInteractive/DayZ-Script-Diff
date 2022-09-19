enum eModifiersTickType//bitmask
{
	TICK 					= 1,
	ACTIVATE_CHECK 			= 2,
	DEACTIVATE_CHECK 		= 4,
}


class ModifierBase
{
	int					m_ID = 0;
	ModifiersManager 	m_Manager;//the manager instance
	string 				m_System = "Modifiers";
	float 				m_ActivatedTime;//overall time this modifier was active
	bool				m_TrackActivatedTime;//should this modifier track overall time it was active ?
	bool				m_IsPersistent;//is this modifier saved to the DB ?
	PlayerBase			m_Player;
	float				m_TickIntervalInactive = 5;
	float				m_TickIntervalActive = 3;
	bool				m_IsActive;
	bool				m_ShouldBeActive;
	float				m_AccumulatedTimeActive;
	float				m_AccumulatedTimeInactive;
	float				m_LastTickedActive;
	int 				m_TickType = (eModifiersTickType.TICK | eModifiersTickType.ACTIVATE_CHECK | eModifiersTickType.DEACTIVATE_CHECK);//some modifiers do not need to check activate condition, as they get activated by request 
	float				m_LastTickedInactive;
	bool				m_IsLocked = false;
	EActivationType		m_ActivationType;
	eModifierSyncIDs	m_SyncID;//max 32 synced modifiers supported, 0 == no sync
	PluginPlayerStatus 		m_ModulePlayerStatus;

	void ModifierBase()
	{
		Class.CastTo(m_ModulePlayerStatus, GetPlugin(PluginPlayerStatus));
	}
	
	void InitBase(PlayerBase player, ModifiersManager manager)
	{
		m_Manager 				= manager;
		m_Player 				= player;
		Init();
	}
	
	void Init(){}


	PlayerBase GetPlayer()
	{
		return m_Player;
	}
	
	bool IsPersistent()
	{
		return m_IsPersistent;
	}

	void MakeParamObjectPersistent(Param object)
	{
		m_Manager.m_ParamList.Insert(object);
	}


	void ResetLastTickTime()
	{
		m_LastTickedActive = 0;
	}

	string GetDebugText()
	{
		return "";
	}
	
	string GetDebugTextSimple()
	{
		return "";
	}
	
	void DisableActivateCheck()
	{
		m_TickType = (m_TickType & ~eModifiersTickType.ACTIVATE_CHECK);
	}	
	
	void DisableDeactivateCheck()
	{
		m_TickType = (m_TickType & ~eModifiersTickType.DEACTIVATE_CHECK);
	}

	void Tick(float delta_time)
	{

		if( !m_IsActive && m_ShouldBeActive )
		{
			Activate();
		}

		if( m_IsActive )
		{
			m_AccumulatedTimeActive += delta_time;
			if( m_AccumulatedTimeActive > m_TickIntervalActive )
			{
				if( m_TickType & eModifiersTickType.DEACTIVATE_CHECK && DeactivateCondition(m_Player) && !IsLocked() )
				{
					Deactivate();
				}
				else//if(m_TickType & eModifiersTickType.TICK)
				{
					m_ActivatedTime += m_AccumulatedTimeActive;
					OnTick(m_Player, m_AccumulatedTimeActive);
				}
				m_AccumulatedTimeActive = 0;
			}
		}
		else if(m_TickType & eModifiersTickType.ACTIVATE_CHECK)
		{
			m_AccumulatedTimeInactive += delta_time;
			if( m_AccumulatedTimeInactive > m_TickIntervalInactive )
			{
				if( ActivateCondition(m_Player) )
				{
					if( !IsLocked() ) 
					{
						ActivateRequest(EActivationType.TRIGGER_EVENT_ON_ACTIVATION);
					}
				}
				m_AccumulatedTimeInactive = 0;
			}
		}
	}
	
	bool IsActive()
	{
		return m_IsActive;
	}
	
	void SetLock(bool state)
	{
		m_IsLocked = state;
	}

	bool IsLocked()
	{
		return m_IsLocked;		
	}
	
	bool IsTrackAttachedTime()
	{
		return m_TrackActivatedTime;
	}
	
	float GetAttachedTime()
	{
		return m_ActivatedTime;
	}
	
	void SetAttachedTime(float time)
	{
		m_ActivatedTime = time;
	}
	
	int GetModifierID()
	{
		return m_ID;
	}
	
	string GetName()
	{
		string name = ClassName();
		int index_start = name.Length() - 4;
		int index_end = name.Length();
		name = name.SubstringInverted(name, index_start, index_end);
		return name;
	}

	bool ActivateCondition(PlayerBase player)
	{
		return false;
	}

	bool DeactivateCondition(PlayerBase player)
	{
		return false;
	}

	//! is called when an inactive modifier gets activated during gameplay, is NOT called on activation upon player server connection(see OnReconnect)
	void OnActivate(PlayerBase player)
	{

	}
	
	//! is called when a modifier is being re-activated upon player server connection, use to activate systems which are not persistent and need to run alongside active modifiers
	void OnReconnect(PlayerBase player)
	{

	}

	void OnDeactivate(PlayerBase player)
	{

	}
	
	void Activate()
	{
		m_IsActive = true;
		m_Player.m_SyncedModifiers = (m_Player.m_SyncedModifiers | m_SyncID);
		if( m_ActivationType == EActivationType.TRIGGER_EVENT_ON_ACTIVATION ) OnActivate(m_Player);
		else if(m_ActivationType == EActivationType.TRIGGER_EVENT_ON_CONNECT ) OnReconnect(m_Player);
		m_Player.SetSynchDirty();
	}
	
	void ActivateRequest(EActivationType trigger)
	{
		m_ShouldBeActive = true;
		m_ActivationType = trigger;
	}

	void Deactivate(bool trigger = true)
	{
		if(!m_IsActive)
			return;
		m_Player.m_SyncedModifiers = (m_Player.m_SyncedModifiers & ~m_SyncID);
		m_ShouldBeActive = false;
		m_IsActive = false;
		m_ActivatedTime = 0;
		if(trigger) 
			OnDeactivate(m_Player);
	}


	void OnStoreSave( ParamsWriteContext ctx )
	{
	}


	private void OnTick(PlayerBase player, float deltaT)
	{
		
	}
}
