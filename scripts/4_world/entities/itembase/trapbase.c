enum SoundTypeTrap
{
	ACTIVATING			= 5,
}

class TrapBase extends ItemBase
{
	#ifdef SERVER
	protected const int SPAWN_FLAGS 				= ECE_CREATEPHYSICS;
	#else
	protected const int SPAWN_FLAGS 				= ECE_LOCAL;
	#endif
	
	protected const int DAMAGE_TRIGGER_MINE 		= 75;
	protected const float UPDATE_TIMER_INTERVAL 	= 0.05;

	int   m_InitWaitTime; 			//After this time after deployment, the trap is activated
	bool m_NeedActivation;			//If activation of trap is needed
	float m_DefectRate; 			//Added damage after trap activation
	float m_DamagePlayers; 			//How much damage player gets when caught
	float m_DamageOthers; 			//How much damage player gets when caught

	bool m_AddActivationDefect;		// Damage trap after activation
	bool m_AddDeactivationDefect;	// Damage trap after deactivation
	protected bool m_IsActive;		// True means that the trap is ready to detonate
	protected bool m_IsInProgress;

	protected bool m_Disarmed = false; //! DEPRECATED Used for explosive traps to prevent detonation after destroying through disarm action

	bool m_WasActivatedOrDeactivated;

	string m_AnimationPhaseGrounded;
	string m_AnimationPhaseSet;
	string m_AnimationPhaseTriggered;

	string m_InfoSetup;
	string m_InfoDeactivated;
	string m_InfoDamageManipulation;
	string m_InfoDamage;
	string m_InfoActivationTime;

	protected ref Timer m_Timer;
	protected ref Timer m_UpdateTimer;
	protected TrapTrigger m_TrapTrigger;
	
	protected ref array<int> m_ClothingDmg;
	protected ref EffectSound 	m_DeployLoopSound;	
	
	void TrapBase()
	{
		m_IsInProgress = false;
		m_NeedActivation = true;
		m_InitWaitTime = 5; 			//After this time after deployment, the trap is activated
		m_DefectRate = 15; 				//Added damage after trap activation
		m_DamagePlayers = 25; 			//How much damage player gets when caught
		m_DamageOthers = 100; 			//How much damage others gets when caught
		
		m_AddActivationDefect = false;
		m_AddDeactivationDefect = false;

		m_WasActivatedOrDeactivated = false;

		m_AnimationPhaseGrounded = "";
		m_AnimationPhaseSet = "";
		m_AnimationPhaseTriggered = "";
		
		m_InfoSetup = 				"#STR_TrapBase0";
		m_InfoDeactivated = 		"#STR_TrapBase1";
		m_InfoDamageManipulation =  "#STR_TrapBase2";
		m_InfoDamage =				"#STR_TrapBase3";
		m_InfoActivationTime = 		"#STR_TrapBase4" + m_InitWaitTime.ToString() + "#STR_TrapBase5";
		
		m_UpdateTimer				= new ref Timer();	//! timer calling OnUpdate in configured interval
		
		RegisterNetSyncVariableBool("m_IsActive");
		RegisterNetSyncVariableBool("m_IsInProgress");
		RegisterNetSyncVariableBool("m_IsSoundSynchRemote");
		RegisterNetSyncVariableBool("m_IsDeploySound");		
	}
	
	void ~TrapBase()
	{
		SEffectManager.DestroyEffect( m_DeployLoopSound );
	}
	
	void OnUpdate(EntityAI victim);
	
	//! this event is called all variables are synchronized on client
    override void OnVariablesSynchronized()
    {
        super.OnVariablesSynchronized();
		
		if ( IsDeploySound() )
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
		
		if ( GetGame().IsMultiplayer() )
		{
			if (m_IsActive)
			{
				SetActive();
			}
			
			if (m_IsInProgress && !m_IsActive)
			{
				StartActivate(null);
			}
		}
	}
	
	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);

		//GetGame() can be sometimes NULL when turning off server
		if ( GetGame() && m_TrapTrigger )
		{
			GetGame().ObjectDelete( m_TrapTrigger );
			m_TrapTrigger = NULL;
		}
	}

	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		
		ctx.Write(m_IsActive);
		
		ctx.Write(m_IsInProgress);
	}
	
	//----------------------------------------------------------------
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{   
		if ( !super.OnStoreLoad(ctx, version) )
			return false;
		
		bool b_is_active = false;
		if ( !ctx.Read( b_is_active ) )
			b_is_active = false;
		
		bool b_is_in_progress = false;
		if ( !ctx.Read( b_is_in_progress ) )
			b_is_in_progress = false;
		
		if ( b_is_active )
		{
			SetActive();
		}
		
		if (b_is_in_progress &&  !b_is_active)
		{
			StartActivate(null);
		}

		return true;
	}
	
	bool IsActive()
	{
		return m_IsActive && m_IsInProgress == false && GetHierarchyRootPlayer() == null;
	}

	bool IsInactive()
	{
		return !IsActive() && m_IsInProgress == false && GetHierarchyRootPlayer() == null;
	}

	// trap cannot be taken when is activated
	override bool IsTakeable()
	{
		if ( m_IsInProgress == false && !IsActive() )
		{
			return true;
		}
		
		return false;
	}

	bool IsActivable()
	{
		return !IsActive() && GetHierarchyRootPlayer() == null && GetHierarchyParent() == null && m_IsInProgress == false && !IsRuined() && m_NeedActivation;
	}

	bool IsPlaceable()
	{
		if ( GetHierarchyRootPlayer() != null && GetHierarchyRootPlayer().GetHumanInventory().GetEntityInHands() == this )
		{
			PlayerBase player = PlayerBase.Cast( GetHierarchyRootPlayer() );
			
			vector player_pos = player.GetPosition();
			vector aim_pos = player.GetAimPosition();
							
			if ( vector.DistanceSq( player_pos, aim_pos ) <= ( Math.SqrFloat( 1.5 ) ) )
			{
				return IsPlaceableAtPosition( aim_pos );
			}
		}
		
		return false;
	}

	bool IsPlaceableAtPosition( vector position )
	{
		if ( GetGame().SurfaceIsSea( position[0], position[2] ) )
		{
			return false;
		}
		else if ( GetGame().SurfaceIsPond( position[0], position[2] ) )
		{
			return false;
		}
		
		return true;
	}
	
	void Disarm()
	{
		SetInactive(false);
		RefreshState();
		GetGame().RPCSingleParam(this, ERPCs.RPC_TRAP_DISARM, null, true);

		OnDisarm();
	}
	
	//! also called from RPC on client
	void OnDisarm();
	
	void SnapOnObject(EntityAI victim)
	{
		if ( GetGame().IsServer() )
		{
			if ( m_Timer )
			{
				m_Timer.Stop();
			}
			
			RefreshState();

			if (m_DamagePlayers > 0)
			{
				if (victim)
				{
					if ( victim.IsInherited(SurvivorBase))
					{
						victim.DecreaseHealth("", "", m_DamagePlayers);
					}
					else if (victim.IsInherited(DayZCreatureAI))
					{
						victim.DecreaseHealth("", "", m_DamageOthers);
					}
					else if (victim.IsInherited(ItemBase))
					{
						ItemBase victim_item = ItemBase.Cast(victim);
						float damage_coef = 1;
						
						if (victim_item.HasQuantity() && victim_item.GetQuantityMax() != 0 && victim_item.GetQuantity() > 0)
						{
							damage_coef = victim_item.GetQuantityMax() / victim_item.GetQuantity(); // Lower quantity increases damage exposure
						}
						
						if (damage_coef > 0)
						{
							int item_size_x = 1;
							int item_size_y = 1;
							GetGame().GetInventoryItemSize(victim_item, item_size_x, item_size_y);
							
							float add_damage = 300 * damage_coef / Math.Clamp(item_size_x * item_size_y, 1, int.MAX);
							victim_item.DecreaseHealth("", "", add_damage);
						}
					}
				}
			}
		
			Synch(victim);
		}

		OnSteppedOn(victim);
	}
	
	void RemoveFromObject(EntityAI victim)
	{
		OnSteppedOut(victim);
		Synch(null);
	}
	
	void OnSteppedOn(EntityAI victim)
	{
		SetInactive(false);
	}
	
	void OnSteppedOut(EntityAI victim); 	//! keeping "step" here for consistency only

	// Synchronizes states
	protected void Synch(EntityAI victim)
	{
		if (GetGame().IsServer())
		{
			SetSynchDirty();

			if (victim && !victim.GetAllowDamage())
			{
				return;
			}

			Param1<EntityAI> p = new Param1<EntityAI>(victim);
			GetGame().RPCSingleParam(this, ERPCs.RPC_TRAP_VICTIM, p, true);
		}
		
	}	
	
	// On server -> client synchronization
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		
		if ( !GetGame().IsDedicatedServer() )
		{
			switch (rpc_type)
			{
				case ERPCs.RPC_TRAP_VICTIM:
				
					ref Param1<EntityAI> p_victim = new Param1<EntityAI>(NULL);
					
					if (ctx.Read(p_victim))
					{
						if (p_victim.param1)
						{
							SnapOnObject(p_victim.param1);
						}
					}
					
				break;
				
				case ERPCs.RPC_TRAP_DISARM:
					OnDisarm();
				break;
				
				case SoundTypeTrap.ACTIVATING:
			
					ref Param1<bool> p = new Param1<bool>(false);
					
					if (ctx.Read(p))
					{
						bool play = p.param1;
					}
					
					if ( play )
					{
						PlayDeployLoopSound();
					}
					
					if ( !play )
					{
						StopDeployLoopSound();
					}
			
				break;
			}
		}
	}
	
	void RefreshState()
	{
		if ( !m_WasActivatedOrDeactivated )
		{
			return;
		}
		
		if ( GetGame().IsServer() )
		{
			// item is owned by player
			if ( GetHierarchyRootPlayer() != NULL && m_AnimationPhaseGrounded != "" )
			{
				SetAnimationPhase( m_AnimationPhaseSet, 1 );
				if ( m_AnimationPhaseTriggered != m_AnimationPhaseGrounded ) 
				{
					SetAnimationPhase( m_AnimationPhaseTriggered, 1 );
				}
				SetAnimationPhase( m_AnimationPhaseGrounded, 0 );
			}
			// item is set active
			else if ( IsActive() )
			{
				if ( m_AnimationPhaseGrounded != "" )
				{
					SetAnimationPhase( m_AnimationPhaseGrounded, 1 );
				}
				if ( m_AnimationPhaseSet != "" && m_AnimationPhaseTriggered != "" )
				{
					SetAnimationPhase( m_AnimationPhaseTriggered, 1 );
					SetAnimationPhase( m_AnimationPhaseSet, 0 );
				}
			}
			// item is inactive and not owned by player (on the ground)
			else if ( IsInactive() )
			{
				if ( m_AnimationPhaseGrounded != "" &&  m_AnimationPhaseTriggered != m_AnimationPhaseGrounded )
				{
					SetAnimationPhase( m_AnimationPhaseGrounded, 1 );
				}
				if ( m_AnimationPhaseSet != "" && m_AnimationPhaseTriggered != "" )
				{
					SetAnimationPhase( m_AnimationPhaseSet, 1 );
					SetAnimationPhase( m_AnimationPhaseTriggered, 0 );
				}
			}
		}
	}

	void SetupTrap()
	{ 
		if ( GetGame().IsServer() )
		{
			if ( GetHierarchyRootPlayer() && GetHierarchyRootPlayer().CanDropEntity( this ) )  // kvoli desyncu
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
				Error("Mojmir: TODO");
				player.LocalDropEntity( this );
				
				vector trapPos = ( player.GetDirection() ) * 1.5;
				trapPos[1] = 0;
				SetPosition( player.GetPosition() + trapPos );
			}
					
			if ( m_NeedActivation == false )
			{
				SetActive();
			}
			//player.MessageStatus( m_InfoSetup );
		}
	}

	void AddDefect()
	{
		if ( GetGame().IsServer() )
		{
			DecreaseHealth( "", "", m_DefectRate );
		}
	}
	
	void SetActive()
	{
		m_WasActivatedOrDeactivated = true;
		
		m_IsInProgress = false;
		m_IsActive = true;
	
		if (m_AddActivationDefect)
		{
			AddDefect();
		}
			
		if (GetGame().IsServer())
		{
			CreateTrigger();
			RefreshState();
			Synch(null);
		}
		
		OnActivate();
	}

	void OnActivate();

	void StartActivate( PlayerBase player )
	{
		if ( GetGame().IsServer() )
		{
			m_Timer = new Timer( CALL_CATEGORY_SYSTEM );
			HideSelection("safety_pin");
			
			if ( m_InitWaitTime > 0 )
			{
				m_IsInProgress = true;
				m_Timer.Run( m_InitWaitTime, this, "SetActive" );
			
				Synch(null);
			}
			else
			{
				SetActive();
			}
		}
	}
	
	void StartDeactivate(PlayerBase player);

	void SetInactive(bool stop_timer = true)
	{
		m_WasActivatedOrDeactivated = true;
		
		m_IsActive = false;
		if (m_Timer && stop_timer)
		{
			m_Timer.Stop();
		}
		
		if (m_AddDeactivationDefect)
		{
			AddDefect();
		}
		
		DeleteTrigger();
		RefreshState();
		Synch(null);
	}
	
	void CreateTrigger()
	{
		if (Class.CastTo(m_TrapTrigger, GetGame().CreateObjectEx("TrapTrigger", GetPosition(), SPAWN_FLAGS)))
		{
			vector mins = "-0.01 -0.05 -0.01";
			vector maxs = "0.01 0.5 0.01";
			m_TrapTrigger.SetOrientation(GetOrientation());
			m_TrapTrigger.SetExtents(mins, maxs);
			m_TrapTrigger.SetParentObject(this);
		}
	}
	
	void DeleteTrigger()
	{
		GetGame().ObjectDelete(m_TrapTrigger);
		m_TrapTrigger = null;
	}

	override void OnItemLocationChanged(EntityAI old_owner, EntityAI new_owner) 
	{
		super.OnItemLocationChanged(old_owner, new_owner);
		
		if (GetGame().IsServer())
		{
			RefreshState();

			// TAKE ACTIVE TRAP FROM VICINITY 
			if (old_owner == NULL && new_owner != NULL && IsActive())
			{
				// TAKE INTO HANDS
				if ( new_owner.IsPlayer() )
				{
					SnapOnObject(new_owner);
				}
				else if (new_owner.GetHierarchyRootPlayer())
				{
					SnapOnObject(new_owner.GetHierarchyRootPlayer());
				}
			}
		}
		
	}

	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);
		
		if ( GetGame().IsServer() )
		{
			RefreshState();
		}
	}	
	
	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
		
		if ( GetGame().IsServer() )
		{
			RefreshState();
		}
	}
	
	override bool CanPutInCargo( EntityAI parent )
	{
		if ( !super.CanPutInCargo(parent) )
		{
			return false;
		}

		return IsTakeable();
	}
	
	override bool CanPutIntoHands( EntityAI parent ) 
	{
		if ( !super.CanPutIntoHands( parent ) )
		{
			return false;
		}

		return IsTakeable();
	}

	override bool CanRemoveFromHands( EntityAI parent ) 
	{
		return IsTakeable();
	}
	
	override bool CanBePlaced( Man player, vector position )
	{
		return IsPlaceableAtPosition( position );
	}

	override string CanBePlacedFailMessage( Man player, vector position )
	{
		return "Trap can't be placed on this surface type.";
	}
	
	//! DEPRECATED Set if trap can be disarmed using ActionClapBearTrapWithThisItem
	bool CanBeClapped()
	{
		return true;
	}
	
	//Set if trap can be disarmed using trap-specific action
	bool CanBeDisarmed()
	{
		return false;
	}
	
	//! DEPRECATED
	void SetDisarmed( bool disarmed )
	{
		m_Disarmed = disarmed;
	}
	
	//! DEPRECATED
	bool GetDisarmed()
	{
		return m_Disarmed;
	}
	
	//================================================================
	// ADVANCED PLACEMENT
	//================================================================
		
	void PlayDeployLoopSound()
	{		
		#ifndef SERVER
		if (!m_DeployLoopSound || !m_DeployLoopSound.IsSoundPlaying())
		{
			m_DeployLoopSound = SEffectManager.PlaySound(GetLoopDeploySoundset(), GetPosition());
		}
		#endif
	}
	
	void StopDeployLoopSound()
	{
		#ifndef SERVER
		if (m_DeployLoopSound)
		{
			m_DeployLoopSound.SetSoundFadeOut(0.5);
			m_DeployLoopSound.SoundStop();
		}
		#endif
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionActivateTrap);
	}
	
	// HELPERS
	protected EntityAI GetClosestCarWheel(EntityAI victim)
	{
		//! carscript specific handling (not all traps are uses this)
		vector trapPosXZ = GetPosition();
		trapPosXZ[1] = 0;

		GameInventory inv = victim.GetInventory();
		for (int i = 0; i < inv.AttachmentCount(); i++)
		{
			//! ruined wheel, bail out
			EntityAI wheelEntity = inv.GetAttachmentFromIndex(i);
			if (wheelEntity && wheelEntity.Type() == CarWheel_Ruined)
			{
				continue;
			}
			
			//! ignore all spare wheel
			int slotId;
			string slotName;		
			wheelEntity.GetInventory().GetCurrentAttachmentSlotInfo(slotId, slotName);
			slotName.ToLower();
			if (slotName.Contains("spare"))
			{
				continue
			}

			//! actual, healthy wheel, let it pass
			if (wheelEntity && wheelEntity.IsInherited(CarWheel))
			{
				vector entPosXZ = wheelEntity.GetPosition();
				entPosXZ[1] = 0;
				if (vector.Distance(trapPosXZ, entPosXZ) < 1)
				{
					return wheelEntity;
				}
			}
		}
		
		return null;
	}
	
	protected void DamageClothing(PlayerBase player)
	{
		//Array used to find all relevant information about currently equipped clothes
		array<ClothingBase> equippedClothes = new array<ClothingBase>;

		equippedClothes.Insert(ClothingBase.Cast(player.GetItemOnSlot("LEGS")));
		equippedClothes.Insert(ClothingBase.Cast(player.GetItemOnSlot("BACK")));
		equippedClothes.Insert(ClothingBase.Cast(player.GetItemOnSlot("VEST")));
		equippedClothes.Insert(ClothingBase.Cast(player.GetItemOnSlot("HeadGear")));
		equippedClothes.Insert(ClothingBase.Cast(player.GetItemOnSlot("Mask")));
		equippedClothes.Insert(ClothingBase.Cast(player.GetItemOnSlot("BODY")));
		equippedClothes.Insert(ClothingBase.Cast(player.GetItemOnSlot("FEET")));
		equippedClothes.Insert(ClothingBase.Cast(player.GetItemOnSlot("GLOVES")));

		//Damage all currently equipped clothes
		for (int i = 0; i < equippedClothes.Count(); i++)
		{
			//If no item is equipped on slot, slot is ignored
			if (equippedClothes[i] == null)
			{
				continue;
			}

			equippedClothes[i].DecreaseHealth(m_ClothingDmg[i], false);
		}
	}
}
