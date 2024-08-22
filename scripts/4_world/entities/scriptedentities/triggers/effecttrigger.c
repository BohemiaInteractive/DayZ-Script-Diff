// Base class for "Effect triggers"
// Registers in TriggerEffectManager and handles parameter setting through cfgEffectArea.json file
class EffectTrigger : CylinderTrigger
{
	int 					m_AroundPartId; // The main particles spawned around player when in trigger
	int 					m_TinyPartId; // The smaller particles spawned around player when in trigger
	int 					m_PPERequester; // The Post Processing used when player is in trigger
	float 					m_DeltaTime;
	float 					m_TimeAccuStay;
	bool 					m_DealDamageFlag;
	TriggerEffectManager 	m_Manager;
	EffectArea 				m_EffectArea;
	int						m_EffectsPriority;
	
	#ifdef DIAG_DEVELOPER
	Shape m_DbgShape;
	bool m_DebugShapeActive;
	#endif
	
	void EffectTrigger()
	{
		RegisterNetSyncVariableInt("m_AroundPartId");
		RegisterNetSyncVariableInt("m_TinyPartId");
		RegisterNetSyncVariableInt("m_PPERequester");
		RegisterNetSyncVariableInt("m_EffectsPriority");
		
		m_Manager = TriggerEffectManager.GetInstance();
		m_Manager.RegisterTriggerType( this );
	}
	
	// ----------------------------------------------
	// 				CUSTOM EVENTS
	// ----------------------------------------------

	void SetLocalEffects( int aroundPartId, int tinyPartId, int ppeRequesterIdx )
	{
		m_AroundPartId = aroundPartId;
		m_TinyPartId = tinyPartId;
		m_PPERequester = ppeRequesterIdx;
		
		SetSynchDirty();
	}
	
	EffectArea GetEffectArea()
	{
		return m_EffectArea;
	}
	
	void Init(EffectArea area, int priority)
	{
		m_EffectArea = area;
		m_EffectsPriority = priority;
	}
	
	int GetEffectsPriority()
	{
		return m_EffectsPriority;
	}
	
	string GetAmbientSoundsetName()
	{
		return "";
	}
	
	void SetupClientEffects(bool enable, notnull PlayerBase player)
	{
		if ( !m_Manager.IsPlayerInTriggerType( player, this ) )
		{
			if (enable)
			{
				player.SetContaminatedEffectEx( true, m_PPERequester, m_AroundPartId, m_TinyPartId, GetAmbientSoundsetName() );
			}
			else
			{
				player.SetContaminatedEffectEx( false, m_PPERequester );
			}
		}
	}
	
	
	// ----------------------------------------------
	// 				TRIGGER EVENTS
	// ----------------------------------------------
	
	override void EOnFrame(IEntity other, float timeSlice)
	{
		m_DeltaTime = timeSlice;
	}
	
	override bool CanAddObjectAsInsider(Object object)
	{
		#ifdef SERVER
		DayZCreatureAI creature = DayZCreatureAI.Cast( object );
		if(creature)
		{
			return !creature.ResistContaminatedEffect();
		}
		else
		{
			PlayerBase player = PlayerBase.Cast(object);
			return player != null;
		}
		#else
		PlayerBase player = PlayerBase.Cast(object);
		return (player && player.IsControlledPlayer());
		#endif
	}
	
	override protected void OnStayClientEvent(TriggerInsider insider, float deltaTime) 
	{
		super.OnStayClientEvent(insider, deltaTime);
		PlayerBase player = PlayerBase.Cast(insider.GetObject());
		if (player)
			player.RequestTriggerEffect(this, m_PPERequester, m_AroundPartId, m_TinyPartId, GetAmbientSoundsetName() );
	}
	
	override void OnEnterServerEvent( TriggerInsider insider )
	{
		super.OnEnterServerEvent( insider );
		
		// We don't need to test the trigger count as Modifiers handle such cases already
		if ( insider )
		{
			PlayerBase playerInsider = PlayerBase.Cast( insider.GetObject() );
			
			if(playerInsider)
			{
				
				m_Manager.OnPlayerEnter( playerInsider, this );
			}

		}
		
	}
	
	override void OnEnterClientEvent( TriggerInsider insider )
	{
		super.OnEnterClientEvent( insider );
		
		if ( insider )
		{
			PlayerBase playerInsider = PlayerBase.Cast( insider.GetObject() );
			
			// We will only handle the controlled player, as effects are only relevant to this player instance
			if (playerInsider && playerInsider.IsControlledPlayer() )
			{
				//SetupClientEffects(true, playerInsider);
				// We then handle the update of player trigger state in manager
				m_Manager.OnPlayerEnter( playerInsider, this );
			}
		}
	}

	
	override void OnLeaveServerEvent( TriggerInsider insider )
	{
		super.OnLeaveServerEvent( insider );
		
		if ( insider )
		{
			PlayerBase playerInsider = PlayerBase.Cast( insider.GetObject() );
			
			if ( playerInsider )
				m_Manager.OnPlayerExit( playerInsider, this );
		}
	}
	
	
	
	override void OnLeaveClientEvent( TriggerInsider insider )
	{
		super.OnLeaveClientEvent( insider );
		
		if ( insider )
		{
			// Make sure you pass the set variable for PPE effect
			// It will not remove the correct one if START and STOP don't point to the same Requester
			PlayerBase playerInsider = PlayerBase.Cast( insider.GetObject() );
			
			// We will only handle the controlled player, as effects are only relevant to this player instance
			if ( playerInsider && playerInsider.IsControlledPlayer() )
			{
				// We first handle the update of player trigger state in manager
				m_Manager.OnPlayerExit( playerInsider, this );
				//SetupClientEffects(false, playerInsider);
			}
		}
	}
	
	// We remove from trigger update DEAD or RESISTANT entities to limit the amount of entities to update
	override bool ShouldRemoveInsider( TriggerInsider insider )
	{
		return !insider.GetObject().IsAlive();
	}
	
	// Used to apply the desired effect to all entities present in one trigger of the specified type
	// NOTE : This is really not optimal, if you want to add new trigger types, you will have to test for them...
	static void TriggerEffect( EntityAI insider, typename triggerType ) {}
		
	#ifdef DIAG_DEVELOPER	
	// overriden so it doesnt refresh the shape every call while keeping the insider coloring functional
	override void DebugDmgTrigger( vector pos, vector orientation, vector min, vector max, float radius, string dmgType, array<ref TriggerInsider> insiders)
	{
		bool enableDebug = DiagMenu.GetBool(DiagMenuIDs.TRIGGER_DEBUG);
		if (enableDebug)
		{
			if (GetGame().IsMultiplayer() && GetGame().IsServer())
				return;
					
			if (!m_DebugShapeActive)
			{
				m_DbgShape = DrawDebugShape(pos, min, max, radius, COLOR_GREEN_A);
				m_DebugShapeActive = true;
			}

			if (GetGame().IsMultiplayer() || GetGame().IsServer())
				m_dbgInsiders = insiders;
		
			if (m_dbgInsiders.Count() > 0)
				m_DbgShape.SetColor(COLOR_YELLOW_A);		
			else 
				m_DbgShape.SetColor(COLOR_GREEN_A);
		}
		else if (m_DebugShapeActive)
		{
			CleanupDebugShapes(dbgTargets);
			m_DebugShapeActive = false;
		}
	}
	#endif
	
}
