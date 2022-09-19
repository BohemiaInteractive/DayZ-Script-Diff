class SymptomBase
{
	const float MAX_TIME_ACTIVE_SAVEGUARD = 20;
	int m_Priority;
	SoundOnVehicle m_SoundObject;
	bool m_PlayedSound;
	bool m_IsActivated;
	PlayerBase m_Player;
	float m_ServerUpdateInterval = 1;
	float m_ServerUpdateDelta;
	bool m_IsTemplate = true;
	float m_ActivatedTime;
	int m_ID;//ID for the type of Symptom
	int m_UID;//unique ID
	bool m_IsClientOnly;
	bool m_DestroyOnAnimFinish = true;
	bool m_DestroyRequested = false;
	int m_SymptomType = -1;
	bool m_IsPersistent = false;
	SymptomManager m_Manager;
	bool m_SyncToClient = false;
	float m_Duration;
	bool m_AnimPlayRequested;
	int m_MaxCount = -1;//how many symptoms of this type can be queued up at the same time, '-1' for unlimited

	SymptomCB m_AnimCallback;
	
	ref array<Param> m_PersistentParams = new array<Param>;
	
	void SymptomBase()
	{
	}
	
	void ~SymptomBase()
	{

	}

	void Init(SymptomManager manager, PlayerBase player, int uid)
	{
		m_Manager = manager;
		m_Player = player;
		m_UID = uid;
		m_IsTemplate = false;
		OnInit();
	}
	
	int GetMaxCount()
	{
		return m_MaxCount;
	}
	
	int GetUID()
	{
		return m_UID;
	}

	void OnOwnerKilled()
	{
		
		
	}
	
	bool CanBeInterupted()
	{
		if(m_AnimPlayRequested) 
		{
			//Print("--------- preventing interrupt ---------");
			return false;
		}
		return true;
	}
	
	bool IsClientOnly()
	{
		return m_IsClientOnly;
	}
	
	
	void SetDuration(float duration)
	{
		m_Duration = duration;
	}
	
	float GetDuration()
	{
		return m_Duration;
	}
	
	string GetName()
	{
		return this.ClassName();		
	}

	SymptomManager GetManager()
	{
		return m_Manager;
	}
	
	int GetType()
	{
		return m_ID;
	}
	
	void SetParam(Param p)
	{
	
	}

	bool IsSyncToClient()
	{
		return m_SyncToClient;		
	}

	void GetPersistentParams(array<Param> params)
	{
		for(int i = 0; i < m_PersistentParams.Count(); i++)
		{
			params.Insert(m_PersistentParams.Get(i));
		}
	}

	void MakeParamObjectPersistent(Param object)
	{
		if( !GetGame().IsServer() && !GetGame().IsMultiplayer() ) return;
		
		m_PersistentParams.Insert(object);
	}
	
	bool IsPersistent()
	{
		return m_IsPersistent;
	}
	
	bool IsPrimary()
	{
		if( m_SymptomType == SymptomTypes.PRIMARY)
			return true;
		else return false;
	}
	
	PlayerBase GetPlayer()
	{
		return m_Player;		
	}

	int GetPriority()
	{
		return m_Priority;
	}
	
	bool OnConstructed(SymptomManager manager)
	{
		
	}
	
	void OnDestructed()
	{
		if( IsActivated() ) Deactivate();
		if( GetManager() ) m_Manager.OnSymptomExit(this, m_UID);
	}

	void Activate()
	{
		m_IsActivated = true;
		if( GetGame() && GetGame().IsServer() )
		{
			OnGetActivatedServer(m_Player);
			if( GetGame().IsMultiplayer() )
			{
				if( IsSyncToClient() ) 
					SyncToClientActivated( GetType(), GetUID() );
				#ifdef DEVELOPER
				GetManager().SendServerDebugToClient();
				#endif
			}
		}
		if( !GetGame().IsDedicatedServer() )
		{
			OnGetActivatedClient(m_Player);
		}
	}
	
	void Deactivate()
	{
		if( !GetGame() ) return;
		m_IsActivated = false;
		if( GetGame().IsServer() ) 
		{
			OnGetDeactivatedServer(m_Player);
			if( GetGame().IsMultiplayer() && IsSyncToClient() )
			{
				SyncToClientDeactivated( GetType(), GetUID() );
			}
		}
		if( !GetGame().IsDedicatedServer() )
		{
			OnGetDeactivatedClient(m_Player);
		}
		
	}
	
	bool IsActivated()
	{
		return m_IsActivated;
	}
	
	
	
	void Update(float deltatime)
	{
		if( GetGame().IsServer() ) 
		{
			
			m_ServerUpdateDelta += deltatime;
			if(m_ServerUpdateDelta > m_ServerUpdateInterval )
			{
				m_ActivatedTime += m_ServerUpdateDelta;
				OnUpdateServer(m_Player, m_ServerUpdateDelta);
				m_ServerUpdateDelta = 0;
			}
		}
		if( GetGame().IsClient() )
		{
			OnUpdateClient(m_Player, deltatime);
		}
		if( GetGame().IsServer() && !GetGame().IsMultiplayer() && !GetGame().IsMissionMainMenu() )
		{
			OnUpdateClient(m_Player, deltatime);
		}
		CheckDestroy();
	}
	
	void PlayAnimationFB(int animation, int stance_mask, float running_time = -1)
	{
		DayZPlayerSyncJunctures.SendPlayerSymptomFB(m_Player, animation, GetType() , stance_mask, running_time );
		m_AnimPlayRequested = true;
	}
	
	void PlayAnimationADD(int type)
	{
		DayZPlayerSyncJunctures.SendPlayerSymptomADD(m_Player, type, GetType());
		m_AnimPlayRequested = true;
	}
		
	void PlaySound(EPlayerSoundEventID id)
	{
		GetPlayer().RequestSoundEvent(id);
		m_PlayedSound = true;
	}

	void SyncToClientActivated( int SYMPTOM_id, int uid )
	{
		if( !GetPlayer() ) return;
		
		CachedObjectsParams.PARAM2_INT_INT.param1 = SYMPTOM_id;
		CachedObjectsParams.PARAM2_INT_INT.param2 = uid;
		GetGame().RPCSingleParam(GetPlayer(), ERPCs.RPC_PLAYER_SYMPTOM_ON, CachedObjectsParams.PARAM2_INT_INT,true,GetPlayer().GetIdentity() );
	}
	
	void SyncToClientDeactivated( int SYMPTOM_id, int uid )
	{
		if( !GetPlayer() ) return;
		CachedObjectsParams.PARAM2_INT_INT.param1 = SYMPTOM_id;
		CachedObjectsParams.PARAM2_INT_INT.param2 = uid;
		GetGame().RPCSingleParam(GetPlayer(), ERPCs.RPC_PLAYER_SYMPTOM_OFF, CachedObjectsParams.PARAM2_INT_INT,true,GetPlayer().GetIdentity() );
	}

	void CheckSoundFinished()
	{
		if(GetGame().IsServer())
		{
			if(m_PlayedSound && m_ActivatedTime >= m_Duration)
				RequestDestroy();
		}
	}
	
	void CheckDestroy()
	{
		CheckSoundFinished();
		if( IsPrimary() && m_ActivatedTime > MAX_TIME_ACTIVE_SAVEGUARD)
		{
			RequestDestroy();
		}
		
		if (m_DestroyRequested) 
			Destroy();
	}
	
	SmptAnimMetaBase SpawnAnimMetaObject()
	{
		return null;
	}
	

	void RequestDestroy()
	{
		m_DestroyRequested = true;
		//if(!IsActivated() ) Destroy();
	}

	void Destroy()
	{
		if(!m_IsTemplate)
			OnDestructed();
	}
	
	//!gets called upon animation Symptom exit
	void AnimationFinish()
	{
		//Print("*********** OnAnimationFinish ************");
		if( m_DestroyOnAnimFinish ) RequestDestroy();
		OnAnimationFinish();
	}
	
	void AnimationPlayFailed()
	{
		OnAnimationPlayFailed();
		AnimationFinish();
	}
	
	void AnimationStart()
	{
		OnAnimationStart();
	}
	
	protected void OnAnimationFinish();
	protected void OnAnimationStart();
	protected void OnAnimationPlayFailed();

	
	//!this is just for the Symptom parameters set-up and is called even if the Symptom doesn't execute, don't put any gameplay code in here
	void OnInit(){}
	
	//!gets called every frame
	void OnUpdateServer(PlayerBase player, float deltatime){}
	bool CanActivate(){return true;}//server only
	//!gets called every frame
	void OnUpdateClient(PlayerBase player, float deltatime){}
	//!gets called once on an Symptom which is being activated
	void OnGetActivatedServer(PlayerBase player){}
	void OnGetActivatedClient(PlayerBase player){}
	//!only gets called once on an active Symptom that is being deactivated
	void OnGetDeactivatedServer(PlayerBase player){}
	void OnGetDeactivatedClient(PlayerBase player){}
}
