class TrapSpawnBase extends ItemBase
{
	int   							m_InitWaitTime; 			//After this time after deployment, the trap is activated
	int  							m_UpdateWaitTime; 			//Time when timer will be called again until success or not succes catch is done
	float 							m_DefectRate; 				//Added damage after trap activation
	bool 							m_IsFoldable;
	bool							m_CanCatch = false;
	float 							m_MinimalDistanceFromPlayersToCatch;
	
	float							m_BaitCatchProb; // The probability (0-100) to catch something when bait is attached
	float							m_NoBaitCatchProb; // The probability (0-100) to catch something when no bait is attached
	float							m_FinalCatchProb; // The selected catch probability (0-100) -> Will be overriden no need to allocate
	
	vector 							m_PreyPos; // The position where prey will be spawned -> Will be overriden later
	
	protected EntityAI				m_Bait;

	protected bool 					m_IsActive;
	protected bool 					m_IsDeployed;
	protected bool 					m_IsInProgress;
	
	ref Timer 						m_Timer;
	
	string 							m_AnimationPhaseSet;
	string 							m_AnimationPhaseTriggered;
	string 							m_AnimationPhaseUsed;

	const string 					m_PlaceableWaterType = UAWaterType.ALL;

	bool m_WaterSurfaceForSetup;	//if trap can be installed on water surface (cannot be detected via getsurfacetype)
	ref multiMap<string, float>	m_CatchesPond;	//array of catches that can be catched in trap - string key, float catch_chance
	ref multiMap<string, float>	m_CatchesSea;	//array of catches that can be catched in trap - string key, float catch_chance
	ref multiMap<string, float>	m_CatchesGroundAnimal;	//array of catches that can be catched in trap - string key, float catch_chance
	
	ref protected EffectSound 	m_DeployLoopSound;
	
	// ===============================================================
	// =====================  DEPRECATED  ============================
	// ===============================================================
	ref Timer 						m_PrevTimer;
	bool 							m_NeedInstalation;
	bool 							m_BaitNeeded;
	bool 							m_IsUsable;
	private ItemBase 				m_Catch;
	ref Timer 						m_AlignCatchTimer;
	string 							m_InfoSetup;
	// ===============================================================
	
	void TrapSpawnBase()
	{
		m_DefectRate 							= 10; 			//Added damage after trap activation
		m_UpdateWaitTime 						= 10;
		m_InitWaitTime 							= 10;
		m_NeedInstalation 						= true;
		m_BaitNeeded 							= true;
		m_IsFoldable 							= false;
		m_MinimalDistanceFromPlayersToCatch 	= 0;

		m_BaitCatchProb 						= 95;
		m_NoBaitCatchProb 						= 50;
		
		m_AnimationPhaseSet 					= "";
		m_AnimationPhaseTriggered 				= "";
		m_AnimationPhaseUsed 					= "";
		
		m_CatchesPond 							= NULL;
		m_CatchesSea 							= NULL;
		m_CatchesGroundAnimal 					= NULL;
		
		RegisterNetSyncVariableBool("m_IsSoundSynchRemote");
		RegisterNetSyncVariableBool("m_IsDeploySound");
		RegisterNetSyncVariableBool("m_IsActive");
		RegisterNetSyncVariableBool("m_IsDeployed");
	}

	void ~TrapSpawnBase()
	{
		SEffectManager.DestroyEffect( m_DeployLoopSound );
	}
	
	override void OnStoreSave( ParamsWriteContext ctx )
	{
		super.OnStoreSave( ctx );
		
		ctx.Write( m_IsActive );
		
		ctx.Write( m_IsInProgress );
		
		ctx.Write( m_IsDeployed );
	}
	
	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{   
		if ( !super.OnStoreLoad(ctx, version) )
			return false;
		
		bool b_is_active = false;
		if ( !ctx.Read( b_is_active ) )
			b_is_active = false;
		
		bool b_is_in_progress = false;
		if ( !ctx.Read( b_is_in_progress ) )
			b_is_in_progress = false;
		
		bool b_is_deployed = false;
		if ( !ctx.Read( b_is_deployed ) )
			b_is_deployed = false;
		
		if ( b_is_active )
		{
			SetActive();
		}
		
		if ( b_is_in_progress && !b_is_active )
		{
			StartActivate( NULL );
		}
		
		SetDeployed( b_is_deployed );

		return true;
	}

	//! this event is called all variables are synchronized on client
    override void OnVariablesSynchronized()
    {
        super.OnVariablesSynchronized();
		
		if ( IsDeploySound() && IsDeployed() )
		{
			PlayDeploySound();
		}
		
		if ( CanPlayDeployLoopSound() )
		{
			PlayDeployLoopSound();
		}
					
		if ( m_DeployLoopSound && !CanPlayDeployLoopSound() )
		{
			StopDeployLoopSound();
		}
	}
	
	bool IsActive()
	{
		return m_IsActive;
	}
	
	bool IsDeployed()
	{
		return m_IsDeployed;
	}
	
	void SetDeployed( bool newState )
	{
		m_IsDeployed = newState;
		
		if ( newState == true )
		{
			if ( m_AnimationPhaseSet != "" && m_AnimationPhaseTriggered != "" )
			{
				SetAnimationPhase( m_AnimationPhaseSet, 1 );
				SetAnimationPhase( m_AnimationPhaseTriggered, 0 );
				SetAnimationPhase( m_AnimationPhaseUsed, 1 );
			}
		}
		else
		{
			if ( m_AnimationPhaseSet != "" && m_AnimationPhaseTriggered != "" )
			{
				SetAnimationPhase( m_AnimationPhaseSet, 0 );
				SetAnimationPhase( m_AnimationPhaseTriggered, 1 );
				SetAnimationPhase( m_AnimationPhaseUsed, 1 );
			}
		}
		
		SetSynchDirty();
	}

	override bool IsTakeable()
	{
		return true;
	}

	bool IsPlaceableAtPosition( vector position )
	{
		string surface_type;
		GetGame().SurfaceGetType3D( position[0], position[1], position[2], surface_type);

		// check surface
		return GetGame().IsSurfaceDigable(surface_type);
	}

	void SetupTrap()
	{ 
		if ( GetGame().IsServer() )
		{
			if ( GetHierarchyRootPlayer() && GetHierarchyRootPlayer().CanDropEntity( this ) )
			{
				SetupTrapPlayer( PlayerBase.Cast( GetHierarchyRootPlayer() ) );
			}
		}
	}

	void SetupTrapPlayer( PlayerBase player, bool set_position = true )
	{ 
		if ( GetGame().IsServer() )
		{
			if ( set_position )
			{
				vector trapPos = player.GetPosition() + ( player.GetDirection() * 0.5 );
				trapPos[1] = GetGame().SurfaceRoadY( trapPos[0], trapPos[2] );
				SetPosition( trapPos );
			}
			
			SetDeployed( true );
		}
	}
	
	void Fold()
	{
		if ( GetGame().IsServer() && m_IsFoldable == true )
		{
			SetInactive();
		}
	}

	// Deal damage to trap on specific events
	void AddDefect()
	{
		if ( GetGame().IsServer() )
		{
			DecreaseHealth( "", "", m_DefectRate );
		}
	}
	
	void StartActivate( PlayerBase player )		{ }
	
	// IsTakeable is used to hide tooltips as well, so we use a custom method instead
	// Used to prevent players from taking traps which should be set for catching
	bool CanBeTaken()
	{
		if ( !IsDeployed() || ( GetInventory().AttachmentCount() == 0 && IsDeployed() ) )
		{
			return true;
		}
		
		return false;
	}

	override bool CanPutInCargo( EntityAI parent )
	{
		super.CanPutInCargo( parent );
		return CanBeTaken();
	}
	
	override bool CanPutIntoHands( EntityAI parent )
	{
		super.CanPutIntoHands( parent );
		return CanBeTaken();
	}
	
	// Set animation phases according to state
	void SetActive()
	{
		if ( GetGame().IsServer() && !IsActive() )
		{
			m_IsActive = true;

			if ( m_AnimationPhaseSet != "" && m_AnimationPhaseTriggered != "" )
			{
				SetAnimationPhase( m_AnimationPhaseSet, 1 );
				SetAnimationPhase( m_AnimationPhaseTriggered, 0 );
				SetAnimationPhase( m_AnimationPhaseUsed, 1 );
			}

			// When activated we start timer for catch check
			m_Timer = new Timer( CALL_CATEGORY_GAMEPLAY );
			m_Timer.Run( m_InitWaitTime, this, "SpawnCatch" );
			
			SetSynchDirty();
		}
	}

	void SetInactive()
	{
		if ( GetGame().IsServer() )
		{
			// We stop timers as the trap is no longer active, then update visuals
			m_IsActive = false;
			if ( m_Timer )
			{
				m_Timer.Stop();
			}
			
			SetDeployed( false );
			
			SetSynchDirty();
		}
	}

	void SetUsed()
	{
		if ( GetGame().IsServer() )
		{
			// We updated state, visuals and stop timers
			m_IsActive = false;
			m_IsDeployed = false;
			if ( m_Timer )
			{
				m_Timer.Stop();
			}
			
			if ( m_AnimationPhaseSet != "" && m_AnimationPhaseTriggered != "" )
			{
				SetAnimationPhase( m_AnimationPhaseSet, 1 );
				SetAnimationPhase( m_AnimationPhaseTriggered, 1 );
				SetAnimationPhase( m_AnimationPhaseUsed, 0 );
			}
			
			SetSynchDirty();
		}
	}
	
	// Used to check catch conditions and spawn relevant prey
	void SpawnCatch()
	{
		// Only server side, let's make sure
		if ( GetGame().IsClient() )
			return;
		
		// If we can't get CEApi we won't be able to test proximity
		if ( !GetCEApi() )
			return;
		
		int i;
		m_Bait = null;
		
		// We check if we have bait and can catch
		m_CanCatch = SetCanCatch( m_Bait );
		
		SetIsDeploySound( false );
		SetSynchDirty();
		
		// We check the distance from players through CEApi if that check is enabled
		if ( m_MinimalDistanceFromPlayersToCatch > 0 )
		{
			if ( !GetCEApi().AvoidPlayer( GetPosition(), m_MinimalDistanceFromPlayersToCatch ) && m_CanCatch )
			{
				m_CanCatch = false;
				
				// We could not catch yet, let's retest until we can
				if ( m_Timer )
					m_Timer.Stop();
				
				m_Timer = new Timer( CALL_CATEGORY_GAMEPLAY );
				m_Timer.Run( m_UpdateWaitTime, this, "SpawnCatch" );
				
				return; // No need to go further, let's just wait for next check
			}
		}
		
		// We get the probability to catch depending on bait presence
		if ( m_Bait )
			m_FinalCatchProb = m_BaitCatchProb;
		else
			m_FinalCatchProb = m_NoBaitCatchProb;
		
		// We get the position of prey when it will spawn
		m_PreyPos = GetPosition();
		if ( MemoryPointExists("Prey_Position") )
		{
			m_PreyPos = ModelToWorld( GetMemoryPointPos("Prey_Position") );
		}
	}

	// Used to set if the given trap can catch a prey in it's current state
	// Also outs the current bait to check for specific catches if required
	bool SetCanCatch( out EntityAI bait )
	{
		int slotIdx = InventorySlots.GetSlotIdFromString("Trap_Bait");
		bait = GetInventory().FindAttachment( slotIdx );
		
		if ( bait )
		{
			Edible_Base edibleBait = Edible_Base.Cast( bait );
			if ( edibleBait )
			{
				if ( !edibleBait.GetFoodStage().IsFoodBurned() && !edibleBait.GetFoodStage().IsFoodRotten() )
					return true;
				else
					return false; // We have invalid bait, no catch
			}
		}
		// We are allowed to catch with no bait
		return true;
	}
	
	// Used to set the quantity of the catch ( later used when preparing fish )
	void CatchSetQuant( ItemBase catch )
	{
		if ( catch.HasQuantity() )
		{
			// Random quantity between 50% and 100%
			float coef = Math.RandomFloatInclusive(0.5, 1.0);
			float item_quantity = catch.GetQuantityMax() * coef;
			item_quantity = Math.Round(item_quantity);
			catch.SetQuantity( item_quantity );
		}
	}

	override void OnItemLocationChanged( EntityAI old_owner, EntityAI new_owner ) 
	{
		super.OnItemLocationChanged( old_owner, new_owner );
		
		if ( GetGame().IsServer() )
		{
			// throw trap from vicinity if the trap does not need installation ( action required )
			if ( new_owner == NULL && m_NeedInstalation == false )
			{
				SetActive();
			}
			else if ( old_owner == NULL && new_owner != NULL )
			{
				if ( m_IsFoldable )
				{
					Fold();
				}
				else
				{
					SetInactive();
				}
			}
		}
	}
	
	// Generic water check, no real distinction between pond or sea
	bool IsSurfaceWater( vector position )
	{
		string surface_type;
		GetGame().SurfaceGetType3D( position[0], position[1], position[2], surface_type );
		
		// check water surface
		if ( m_WaterSurfaceForSetup )
		{
			bool isSea;
			// Check if we are in sea
			if ( m_PlaceableWaterType == UAWaterType.ALL )
				isSea = ( position[1] <= ( GetGame().SurfaceGetSeaLevel() + 0.001 ) );
			// Check if sea or the water type if not sea
			if ( isSea || surface_type.Contains( m_PlaceableWaterType ) )
				return true;
		}
		return false;
	}
	
	// Can only receive attachment if deployed
	override bool CanDisplayAttachmentSlot( int slot_id )
	{
		super.CanDisplayAttachmentSlot( slot_id );
		return IsDeployed();
	}
	
	override bool CanReceiveAttachment( EntityAI attachment, int slotId )
	{
		super.CanReceiveAttachment( attachment, slotId );
		return IsDeployed();
	}
	
	override void EEItemAttached( EntityAI item, string slot_name )
	{
		super.EEItemAttached( item, slot_name );
		
		// In the case the trap was still deployed, enable "quick re-arm" by attaching
		if ( IsDeployed() && slot_name == "Trap_Bait" )
		{
			SetActive();
		}
	}
	
	//================================================================
	// ADVANCED PLACEMENT
	//================================================================
	
	override void OnPlacementComplete( Man player, vector position = "0 0 0", vector orientation = "0 0 0" )
	{
		super.OnPlacementComplete( player, position, orientation );
		
		if ( GetGame().IsServer() )
		{
			SetupTrapPlayer( PlayerBase.Cast( player ), false );
			SetIsDeploySound( true );			
			SetActive();
		}	
	}
	
	void PlayDeployLoopSound()
	{		
		if ( !GetGame().IsDedicatedServer() )
		{		
			if ( !m_DeployLoopSound || !m_DeployLoopSound.IsSoundPlaying() )
			{
				m_DeployLoopSound = SEffectManager.PlaySound( GetLoopDeploySoundset(), GetPosition() );
			}
		}
	}
	
	void StopDeployLoopSound()
	{
		if ( !GetGame().IsDedicatedServer() )
		{	
			m_DeployLoopSound.SetSoundFadeOut(0.5);
			m_DeployLoopSound.SoundStop();
		}
	}
	
	// We add the action to deploy a trap laid on ground
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionActivateTrap);
		AddAction(ActionDeployHuntingTrap);
	}
	
	
	// ===============================================================
	// =====================  DEPRECATED  ============================
	// ===============================================================
	
	bool CanPutInInventory( EntityAI  player ) { }
	
	void AlignCatch( ItemBase obj, string catch_name ) { }
	
	bool IsPlaceable()
	{
		if ( GetGame().IsServer() )
		{
			if ( GetHierarchyRootPlayer() != NULL && GetHierarchyRootPlayer().GetHumanInventory().GetEntityInHands() == this )
			{
				PlayerBase player = PlayerBase.Cast( GetHierarchyRootPlayer() );
				
				vector player_pos = player.GetPosition();
				vector aim_pos = player.GetAimPosition();
				
				if ( vector.DistanceSq(player_pos, aim_pos) <= ( 1.5 * 1.5 ) )
				{
					return IsPlaceableAtPosition( aim_pos );
				}
			}
		}
		
		return false;
	}
	
	override bool CanBePlaced( Man player, vector position )
	{
		return IsPlaceableAtPosition( position );
	}

	override string CanBePlacedFailMessage( Man player, vector position )
	{
		return "Trap can't be placed on this surface type.";
	}
	// ===============================================================
}
