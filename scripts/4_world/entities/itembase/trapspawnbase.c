class TrapSpawnBase extends ItemBase
{
	bool 							m_IsFoldable;
	bool							m_CanCatch = false;
	
	//configurable stuff
	bool 							m_NeedInstalation;
	int   							m_InitWaitTimeMin;
	int   							m_InitWaitTimeMax;
	int  							m_UpdateWaitTime; 			//! Catch evaluation interval
	int  							m_SpawnUpdateWaitTime; 		//! Catch spawn and player check interval (expensive-ish)
	int  							m_MaxActiveTime; 			//! Max time of trap activity (seconds)
	float 							m_BaitLossFraction; 		//! Normalized bait qty reduction on unsuccessful catch
	float 							m_DefectRate; 				//! Absolute damage dealt to trap when used
	float 							m_MinimalDistanceFromPlayersToCatch; //! duh
	
	//derived stuff
	private int   					m_InitWaitTime; 			//! After this time after deployment, the trap is activated
	private int 					m_AdjustedMaxActiveTime; 	//! Adjusted by init wait time, when appropriate
	private int  					m_ElapsedTime;
	private int  					m_ActivationTime;
	private int  					m_RollSuccessTime;
	private float 					m_CurrentMinimalDistance;
	private float 					m_CurrentlyUsedDelta;
	private bool 					m_IsCatchSuccessful;
	private int 					m_CatchEnviroMask = 0;
	private int 					m_YieldItemIdxLocal = -1;
	private int 					m_YieldItemIdx = -1;
	private int 					m_CatchParticleEffecterId = -1;
	
	vector 							m_PreyPos; // The position where prey will be spawned -> Will be overriden later
	
	protected bool 					m_IsActive;
	protected bool 					m_IsPastWaitingTime;
	protected bool 					m_IsDeployed;
	
	ref Timer 						m_Timer;
	
	string 							m_AnimationPhaseSet;
	string 							m_AnimationPhaseTriggered;
	string 							m_AnimationPhaseUsed;

	protected ref array<string>		m_PlaceableWaterSurfaceList;
	
	protected ref CatchingContextTrapsBase 	m_CatchingContext;
	
	#ifdef DEVELOPER
	int m_dbgAttemptCount = 0;
	#endif
	
	void TrapSpawnBase()
	{
		InitTrapValues();
		
		RegisterNetSyncVariableBool("m_IsActive");
		RegisterNetSyncVariableBool("m_IsDeployed");
		RegisterNetSyncVariableInt("m_YieldItemIdx");
		
		//DEPRECATED stuff below, legacy reasons only
		m_CatchesPond 							= new multiMap<string, float>; //yields now in WorldData.InitYieldBank
		m_CatchesSea 							= new multiMap<string, float>; //yields now in WorldData.InitYieldBank
		m_CatchesGroundAnimal 					= new multiMap<string, float>; //yields now in WorldData.InitYieldBank
	}
	
	void ~TrapSpawnBase()
	{
		if (m_Timer)
		{
			m_Timer.Stop();
			delete m_Timer;
		}
		
		ClearCatchingComponent();
	}
	
	void InitTrapValues()
	{
		m_DefectRate 							= 10; 			//Added damage after trap activation
		m_BaitLossFraction 						= 0.2;
		m_UpdateWaitTime 						= 60;
		m_SpawnUpdateWaitTime 					= 30;
		m_InitWaitTimeMin 						= 10;
		m_InitWaitTimeMax 						= 10;
		m_MaxActiveTime 						= 1200;
		m_NeedInstalation 						= true;
		m_IsFoldable 							= false;
		m_IsCatchSuccessful 					= false;
		m_MinimalDistanceFromPlayersToCatch 	= 0;
		
		m_AnimationPhaseSet 					= "";
		m_AnimationPhaseTriggered 				= "";
		m_AnimationPhaseUsed 					= "";
		
		m_PlaceableWaterSurfaceList 			= new array<string>();
		m_PlaceableWaterSurfaceList.Insert(UAWaterType.SEA);
		m_PlaceableWaterSurfaceList.Insert(UAWaterType.FRESH);
	}
	
	override void OnStoreSave( ParamsWriteContext ctx )
	{
		super.OnStoreSave( ctx );
		
		ctx.Write( m_IsActive );
		
		ctx.Write( m_IsDeployed );
		
		ctx.Write(m_CatchEnviroMask);
	}
	
	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{   
		if ( !super.OnStoreLoad(ctx, version) )
			return false;
		
		m_IsStoreLoad = true;
		
		bool b_is_active = false;
		if ( !ctx.Read( b_is_active ) )
			b_is_active = false;
		
		bool b_is_in_progress = false;
		if (version < 139)
		{
			if ( !ctx.Read( b_is_in_progress ) )
				b_is_in_progress = false;
		}
		
		bool b_is_deployed = false;
		if ( !ctx.Read( b_is_deployed ) )
			b_is_deployed = false;
		
		if (version >= 139)
		{
			int enviroMask;
			if (ctx.Read(enviroMask))
				m_CatchEnviroMask = enviroMask;
		}
		
		if (b_is_active)
		{
			InitCatchingComponent();
			SetActive();
		}
		
		SetDeployed( b_is_deployed );
		
		m_IsStoreLoad = false;
		return true;
	}

	//! this event is called all variables are synchronized on client
    override void OnVariablesSynchronized()
    {
        super.OnVariablesSynchronized();
				
		if (m_YieldItemIdx != m_YieldItemIdxLocal)
		{
			m_YieldItemIdxLocal = m_YieldItemIdx;
			if (m_YieldItemIdxLocal != -1)
				OnCatchSpawnClient();
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
	
	void UpdatePreyPos()
	{
		m_PreyPos = GetPosition();
		if (MemoryPointExists("Prey_Position"))
			m_PreyPos = ModelToWorld(GetMemoryPointPos("Prey_Position"));
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
	
	void ResetActiveProgress()
	{
		m_ActivationTime = GetGame().GetTickTime();
		m_ElapsedTime = 0;
		m_CurrentMinimalDistance = m_MinimalDistanceFromPlayersToCatch;
		
		#ifdef DEVELOPER
		m_dbgAttemptCount = 0;
		#endif
	}
	
	void ResetRunningTimerProgress()
	{
		if (m_IsPastWaitingTime)
		{
			m_AdjustedMaxActiveTime = m_MaxActiveTime;
			RunTrappingTimer(m_UpdateWaitTime,"EvaluateCatch");
		}
		else
		{
			m_AdjustedMaxActiveTime = m_MaxActiveTime + m_InitWaitTime;
			RunTrappingTimer(m_InitWaitTime + m_UpdateWaitTime,"EvaluateCatch");
		}
	}
	
	//! generic trapping launcher for traps, use this to store delta info
	void RunTrappingTimer(float duration, string fnName)
	{
		if (!m_Timer)
			m_Timer = new Timer(CALL_CATEGORY_SYSTEM);
		else
			m_Timer.Stop();
		
		#ifdef DEVELOPER
		if (IsCLIParam("trapsQuick"))
		{
			m_CurrentlyUsedDelta = 1;
			m_Timer.Run(1, this, fnName);
		}
		else
		#endif
		{
			m_CurrentlyUsedDelta = duration;
			m_Timer.Run(duration, this, fnName);
		}
	}
	
	// Set animation phases according to state
	void SetActive()
	{
		UpdatePreyPos();
		
		if ( GetGame().IsServer() && !IsActive() )
		{
			SetCatchSuccessful(false);
			m_IsActive = true;
			m_IsPastWaitingTime = false;
			m_YieldItemIdx = -1;
			
			if (m_CatchParticleEffecterId >= 0)
				SEffectManager.DestroyEffecterParticleServer(m_CatchParticleEffecterId);
			m_CatchParticleEffecterId = -1;
			
			SetSynchDirty();
			
			if ( m_AnimationPhaseSet != "" && m_AnimationPhaseTriggered != "" )
			{
				SetAnimationPhase( m_AnimationPhaseSet, 1 );
				SetAnimationPhase( m_AnimationPhaseTriggered, 0 );
				SetAnimationPhase( m_AnimationPhaseUsed, 1 );
			}
			
			ResetActiveProgress();
			m_InitWaitTime = Math.RandomFloatInclusive(m_InitWaitTimeMin,m_InitWaitTimeMax);
			if (!m_IsStoreLoad) //presumably activated by the player, store load initializes component separately
			{
				InitCatchingComponent();
				UpdateTrapEnviroMask();
				
				RunTrappingTimer(m_InitWaitTime + m_UpdateWaitTime,"EvaluateCatch");
				m_AdjustedMaxActiveTime = m_MaxActiveTime + m_InitWaitTime;
			}
			else //presumed store load
			{
				SetTrapEnviroMask(m_CatchEnviroMask);
				
				RunTrappingTimer(m_UpdateWaitTime,"EvaluateCatch");
				m_AdjustedMaxActiveTime = m_MaxActiveTime;
			}
		}
	}

	void SetInactive()
	{
		if ( GetGame().IsServer() )
		{
			// We stop timers as the trap is no longer active, then update visuals
			m_IsActive = false;
			
			ClearCatchingComponent();
			
			if ( m_Timer )
			{
				m_Timer.Stop();
			}
			
			m_IsPastWaitingTime = false;
			
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
			
			// Deal damage to trap
			AddDefect();
			HandleBaitLoss();
			DetachAllAttachments();
			
			ClearCatchingComponent();
			
			if ( m_Timer )
			{
				m_Timer.Stop();
			}
			
			m_IsPastWaitingTime = false;
			
			if ( m_AnimationPhaseSet != "" && m_AnimationPhaseTriggered != "" )
			{
				SetAnimationPhase( m_AnimationPhaseSet, 1 );
				SetAnimationPhase( m_AnimationPhaseTriggered, 1 );
				SetAnimationPhase( m_AnimationPhaseUsed, 0 );
			}
			
			m_CatchingContext = null;
			
			SetSynchDirty();
		}
	}
	
	void IncreaseElapsedTime()
	{
		m_ElapsedTime += m_CurrentlyUsedDelta;
		
		#ifdef DEVELOPER
		if (IsCLIParam("catchingLogs"))
		{
			Print("dbgTrapz | delta: " + m_CurrentlyUsedDelta);
			Print("dbgTrapz | m_ElapsedTime: " + m_ElapsedTime);
			Print("dbgTrapz | m_AdjustedMaxActiveTime: " + m_AdjustedMaxActiveTime);
		}
		#endif
	}
	
	void AdjustDetectionRange()
	{
		if (m_CurrentMinimalDistance > 0)
		{
			float time = m_ElapsedTime - m_RollSuccessTime;
			float timeLimit = m_AdjustedMaxActiveTime - m_RollSuccessTime;
			time = Math.InverseLerp(0,timeLimit,time);
			time = Easing.EaseInQuad(time);
			m_CurrentMinimalDistance = Math.Lerp(m_MinimalDistanceFromPlayersToCatch,0,time);
			m_CurrentMinimalDistance = Math.Clamp(m_CurrentMinimalDistance,0,m_MinimalDistanceFromPlayersToCatch);
			
			#ifdef DEVELOPER
			if (IsCLIParam("catchingLogs"))
			{
				Print("dbgTrapz | adjusted distance: " + m_CurrentMinimalDistance + "/" + m_MinimalDistanceFromPlayersToCatch + " | LERP progress: " + time);
			}
			#endif
		}
	}
	
	void EvaluateCatch()
	{
		#ifdef DEVELOPER
		m_dbgAttemptCount++;
		#endif
		
		m_IsPastWaitingTime = true;
		IncreaseElapsedTime();
		
		#ifdef DEVELOPER
		if (IsCLIParam("catchingLogs"))
		{
			Print("dbgTrapz | m_dbgAttemptCount: " + m_dbgAttemptCount + "/" + (m_MaxActiveTime/m_UpdateWaitTime));
		}
		#endif
		
		bool success = false;
		m_CanCatch = SetCanCatch(m_Bait);
		
		if (m_CanCatch)
		{
			if (m_CatchingContext.RollCatch())
			{
				success = true;
				
				#ifdef DEVELOPER
				if (IsCLIParam("catchingLogs"))
				{
					Print("dbgTrapz | success!!!");
					Print("---------------------");
				}
				#endif
			}
		}
		
		#ifdef DEVELOPER
		string dbgSuccessOverride;
		if (GetCLIParam("trapsSuccessOverride",dbgSuccessOverride))
		{
			if (dbgSuccessOverride == "true" || dbgSuccessOverride.ToInt() == 1)
				success = true;
			else if (dbgSuccessOverride == "false" || dbgSuccessOverride.ToInt() == 0)
				success = false;
		}
		#endif
		
		m_Timer.Stop();
		
		#ifdef DEVELOPER
		if (m_ElapsedTime >= m_AdjustedMaxActiveTime || (IsCLIParam("trapsQuick") && !success))
		#else
		if (m_ElapsedTime >= m_AdjustedMaxActiveTime)
		#endif
		{
			SetUsed();
			return;
		}
		
		if (success)
		{
			m_RollSuccessTime = m_ElapsedTime;
			m_CurrentlyUsedDelta = 0;
			TrySpawnCatch();
		}
		else
		{
			RunTrappingTimer(m_UpdateWaitTime,"EvaluateCatch");
		}
	}
	
	bool IsPlayerInVicinity()
	{
		if (!GetCEApi())
		{
			Debug.Log("CE not enabled, player avoidance not available!");
			return false;
		}
		
		return !GetCEApi().AvoidPlayer(GetPosition(), m_CurrentMinimalDistance);
	}
	
	void TrySpawnCatch()
	{
		IncreaseElapsedTime();
		AdjustDetectionRange();
		
		#ifdef DEVELOPER
		if (IsCLIParam("trapsQuick") || m_CurrentMinimalDistance <= 0 || !IsPlayerInVicinity())
		#else
		if (m_CurrentMinimalDistance <= 0 || !IsPlayerInVicinity())
		#endif
		{
			SpawnCatch();
		}
		else if (m_ElapsedTime < m_AdjustedMaxActiveTime)
		{
			RunTrappingTimer(m_SpawnUpdateWaitTime,"TrySpawnCatch");
		}
	}
	
	// Actually spawns the prey
	void SpawnCatch()
	{
		// Only server side, let's make sure
		if (GetGame().IsMultiplayer() && GetGame().IsClient())
			return;
		
		UpdatePreyPos();
				
		ItemBase catch;
		if (m_CanCatch)
		{
			catch = ItemBase.Cast(m_CatchingContext.SpawnAndSetupCatch(m_YieldItemIdx,m_PreyPos));
			
			OnCatchSpawnServer();
			SetCatchSuccessful(catch != null);
			// We change the trap state and visuals
			SetUsed();
		}
		
		SetSynchDirty();
	}
	
	void SetCatchSuccessful(bool successful)
	{
		m_IsCatchSuccessful = successful;
	}
	
	void OnCatchSpawnServer()
	{
		PlayCatchEffectsServer();
	}
	
	void OnCatchSpawnClient()
	{
		UpdatePreyPos(); //previously set on server only
		
		PlayCatchEffectsClient();
	}
	
	protected void PlayCatchEffectsServer()
	{
		if (m_YieldItemIdx == -1)
			return;
		
		YieldItemBase yItem = GetGame().GetMission().GetWorldData().GetCatchYieldBank().GetYieldItemByIdx(m_YieldItemIdx);
		
		PlayCatchNoise(yItem);
		PlayCatchParticleSynced(yItem);
	}
	
	protected void PlayCatchEffectsClient()
	{
		if (m_YieldItemIdx == -1)
			return;
		
		YieldItemBase yItem = GetGame().GetMission().GetWorldData().GetCatchYieldBank().GetYieldItemByIdx(m_YieldItemIdx);
		PlayCatchSound(yItem);
	}
	
	protected void PlayCatchSound(YieldItemBase yItem)
	{
		if (yItem.GetCatchDeathSoundset() != "")
			SEffectManager.PlaySoundEnviroment(yItem.GetCatchDeathSoundset(),m_PreyPos);
	}
	
	protected void PlayCatchNoise(YieldItemBase yItem)
	{
		string noiseType = yItem.GetCatchAINoise();
		if (noiseType == "")
			return;
		
		NoiseParams m_NoisePar = new NoiseParams();
		m_NoisePar.Load(noiseType);
		float noiseMultiplier = yItem.GetCatchAINoiseBaseStrength();
		noiseMultiplier *= NoiseAIEvaluate.GetNoiseReduction(GetGame().GetWeather());
		GetGame().GetNoiseSystem().AddNoiseTarget(m_PreyPos, 5, m_NoisePar, noiseMultiplier);
	}
	
	protected void PlayCatchParticleSynced(YieldItemBase yItem)
	{
		int particleId = yItem.GetCatchParticleID();
		if (particleId == ParticleList.INVALID)
			return;
		
		if (m_CatchParticleEffecterId < 0)
		{
			m_CatchParticleEffecterId = SEffectManager.CreateParticleServer(m_PreyPos, new ParticleEffecterParameters("ParticleEffecter", 5, particleId));
		}
		else
		{
			SEffectManager.ReinitParticleServer(m_CatchParticleEffecterId, new ParticleEffecterParameters("ParticleEffecter", 5, particleId)); //reinit here, since particleId might differ
			SEffectManager.ReactivateParticleServer(m_CatchParticleEffecterId);
		}
	}
	
	//Pre-roll validation, bait compatibility handled between YieldItems and bait type
	bool SetCanCatch( out EntityAI bait )
	{
		return m_CatchingContext.IsValid();
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
			
			if (m_YieldItemIdx != -1) //resets sound effect idx
			{
				m_YieldItemIdx = -1;
				SetSynchDirty();
			}
		}
	}
	
	// Generic water check, no real distinction between pond or sea
	bool IsSurfaceWater(vector position)
	{
		string surfaceType;
		GetGame().SurfaceGetType3D(position[0], position[1], position[2], surfaceType);
		
		return Surface.AllowedWaterSurface(position[1] + 0.1, surfaceType, m_PlaceableWaterSurfaceList);
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
		
		if (IsActive() && GetGame().IsServer())
		{
			ResetActiveProgress();
			m_CatchingContext.UpdateDataAndMasks();
			m_CatchingContext.GenerateResult();
			ResetRunningTimerProgress();
		}
	}
	
	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached( item, slot_name );
		
		if (IsActive() && GetGame().IsServer())
		{
			ResetActiveProgress();
			m_CatchingContext.UpdateDataAndMasks();
			m_CatchingContext.GenerateResult();
			ResetRunningTimerProgress();
		}
	}
	
	void InitCatchingComponent();
	
	void ClearCatchingComponent()
	{
		if (m_CatchingContext)
			delete m_CatchingContext;
	}
	
	void UpdateTrapEnviroMask()
	{
		m_CatchEnviroMask = m_CatchingContext.UpdateTrapEnviroMask();
	}
	
	void SetTrapEnviroMask(int value)
	{
		m_CatchingContext.SetTrapEnviroMask(value);
	}
	
	void HandleBaitLoss()
	{
		if (m_CatchingContext)
		{
			if (m_IsCatchSuccessful)
				m_CatchingContext.RemoveBait();
			else
				m_CatchingContext.ReduceBaitQty(m_BaitLossFraction);
		}
	}
	
	//! detaches everything on catching end (some slots may not be accessible when folded)
	void DetachAllAttachments()
	{
		int count = GetInventory().AttachmentCount();
		if (count > 0)
		{
			EntityAI att;
			for (int i = 0; i < count; i++)
			{
				att = GetInventory().GetAttachmentFromIndex(i);
				GetInventory().DropEntity(InventoryMode.SERVER,this,att);
			}
		}
	}
	
	//================================================================
	// ADVANCED PLACEMENT
	//================================================================
	
	override void OnPlacementComplete(Man player, vector position = "0 0 0", vector orientation = "0 0 0")
	{
		super.OnPlacementComplete(player, position, orientation);
		
		if (GetGame().IsServer())
		{
			vector rotation_matrix[3];
			float direction[4];
			Math3D.YawPitchRollMatrix(orientation, rotation_matrix);
			Math3D.MatrixToQuat(rotation_matrix, direction);
			InventoryLocation source = new InventoryLocation;
			InventoryLocation destination = new InventoryLocation;
			
			if (GetInventory().GetCurrentInventoryLocation(source))
			{		
				destination.SetGroundEx(this, position, direction);
				if (GetGame().IsMultiplayer())
				{
					player.ServerTakeToDst(source, destination);
					SetupTrapPlayer(PlayerBase.Cast(player), false);
				}
				else // singleplayer
				{
					PlayerBase.Cast(player).GetDayZPlayerInventory().RedirectToHandEvent(InventoryMode.LOCAL, source, destination);
					GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SetupTrapPlayer, 100, false, PlayerBase.Cast(player), false);
				}
			}
			
			InitCatchingComponent();
			UpdateTrapEnviroMask();
			SetActive();
		}
	}
	
	
	bool IsPlaceable()
	{
		if ( GetGame().IsServer() )
		{
			InventoryLocation loc = new InventoryLocation();
			GetInventory().GetCurrentInventoryLocation(loc);
			if (loc.GetType() == InventoryLocationType.HANDS)
			{
				PlayerBase player = PlayerBase.Cast(GetHierarchyRootPlayer());
				
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
		return IsPlaceableAtPosition(position);
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
	
	const string 					m_PlaceableWaterType //!DEPRECATED
	
	ref Timer 						m_PrevTimer;//!DEPRECATED
	bool 							m_BaitNeeded;//!DEPRECATED
	bool 							m_IsUsable;//!DEPRECATED
	private ItemBase 				m_Catch;//!DEPRECATED, no reason to keep the information as member
	ref Timer 						m_AlignCatchTimer;//!DEPRECATED
	string 							m_InfoSetup;//!DEPRECATED
	protected bool 					m_IsInProgress;//!DEPRECATED
	protected ref EffectSound 		m_DeployLoopSound;//!DEPRECATED
	protected EntityAI				m_Bait;
	float							m_BaitCatchProb; //!DEPRECATED
	float							m_NoBaitCatchProb; //!DEPRECATED
	float							m_FinalCatchProb; //!DEPRECATED
	
	bool m_WaterSurfaceForSetup;	//!DEPRECATED
	ref multiMap<string, float>	m_CatchesPond;	//!DEPRECATED
	ref multiMap<string, float>	m_CatchesSea;	//!DEPRECATED
	ref multiMap<string, float>	m_CatchesGroundAnimal;	//!DEPRECATED
	
	//!DEPRECATED
	bool CanPutInInventory(EntityAI  player);
	//!DEPRECATED
	void AlignCatch(ItemBase obj, string catch_name);
	///!DEPRECATED
	void CatchSetQuant(ItemBase catch);
	void PlayDeployLoopSound();	//!DEPRECATED
	void StopDeployLoopSound();	//!DEPRECATED
	// ===============================================================
}
