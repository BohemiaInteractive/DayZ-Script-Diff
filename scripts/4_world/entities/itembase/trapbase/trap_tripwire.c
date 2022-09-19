// Wire type is used in the case of decrafting to give back the correct base Ingredient
enum eWireMaterial
{
	WIRE 		= 0,
	BARBED_WIRE = 1,
	ROPE 		= 2
}

class TripwireTrap : TrapBase
{
	// Current state of the tripwire
	static const int 	FOLDED = 3;
	static const int 	DEPLOYED = 2;
	static const int 	TRIGGERED = 1;

	int 		m_State = FOLDED;
	private int m_WireMaterial;
	
	protected bool   m_ResultOfAdvancedPlacing;
	protected vector m_TriggerPosition;
	protected vector m_TriggerOrientation;
	
	void TripwireTrap()
	{
		m_DamagePlayers = 0; 			//How much damage player gets when caught
		m_InitWaitTime = 0; 			//After this time after deployment, the trap is activated
		m_DefectRate = 15;
		m_NeedActivation = false;
		m_AnimationPhaseGrounded = "inventory";
		m_AnimationPhaseSet = "placing";
		m_AnimationPhaseTriggered = "triggered";
		m_InfoActivationTime = string.Format("#STR_TripwireTrap0%1#STR_TripwireTrap1", m_InitWaitTime.ToString());  // nefunguje dynamicke vyrazy mimo funkcii	
	
		RegisterNetSyncVariableInt("m_State");
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
				
		if ( IsPlaceSound() )
		{
			PlayPlaceSound();
		}
	}
	
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		
		ctx.Write( m_State );
	}
	
	//----------------------------------------------------------------
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{   
		if ( !super.OnStoreLoad(ctx, version) )
			return false;
		
		int state = FOLDED;
		if ( !ctx.Read( state ) )
			state = FOLDED;
		
		SetState( state );
		RefreshState();

		return true;
	}
	
	override void CreateTrigger()
	{
		m_TrapTrigger = TripWireTrigger.Cast(GetGame().CreateObjectEx("TripWireTrigger", GetPosition(), SPAWN_FLAGS));
		vector mins = "-0.75 0.3 -0.01";
		vector maxs = "0.75 0.32 0.01";
		m_TrapTrigger.SetOrientation(GetOrientation());
		m_TrapTrigger.SetExtents(mins, maxs);
		m_TrapTrigger.SetParentObject(this);
	}
	
	override void OnSteppedOn(EntityAI victim)
	{
		if (!victim)
		{
			return;
		}
		
		if (!victim.GetAllowDamage())
		{
			return;
		}

		// We must deal some damage, here 5 shock as melee damage in order to trigger hit animation
		if (GetGame().IsServer())
		{
			victim.ProcessDirectDamage(DT_CLOSE_COMBAT, this, "", "TripWireHit", "0 0 0", 1);
			SetState(TRIGGERED);
			SetInactive(false);
		}
		
		// We play the trap trigger sound
		#ifndef SERVER
		EffectSound sound = SEffectManager.PlaySound("TripwireTrap_Trigger_SoundSet", GetPosition());
		sound.SetAutodestroy(true);
		#endif
	}
	
	override void OnItemLocationChanged(EntityAI old_owner, EntityAI new_owner)
	{
		super.OnItemLocationChanged(old_owner, new_owner);
		
		PlayerBase player = PlayerBase.Cast( new_owner );
		if (player)
		{
			StartDeactivate(player);
		}
	}
	
	override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		super.EEItemLocationChanged(oldLoc, newLoc);
		
		if (m_ResultOfAdvancedPlacing)
		{
			if (oldLoc.GetType() == InventoryLocationType.GROUND && newLoc.GetType() == InventoryLocationType.GROUND)
			{
				SetActive();
				m_TrapTrigger.SetPosition(m_TriggerPosition);
				m_TrapTrigger.SetOrientation(m_TriggerOrientation);
			}
			
			m_ResultOfAdvancedPlacing = false;
		}
	}
	
	override void SetInactive(bool stop_timer = true)
	{
		super.SetInactive(stop_timer);

		// de-attach attachments after "activating them"
		for (int att = 0; att < GetInventory().AttachmentCount(); att++)
		{
			ItemBase attachment = ItemBase.Cast(GetInventory().GetAttachmentFromIndex(att));
			if (attachment)
			{
				if (attachment.IsLockedInSlot())
				{
					attachment.UnlockFromParent();
				}
				
				attachment.OnActivatedByItem(this);
				GetInventory().DropEntity(InventoryMode.SERVER, this, attachment);
			}
		}
	}
	
	void SetState(int state_ID)
	{
		m_State = state_ID;
	}
	
	int GetState()
	{
		return m_State;
	}
	
	void SetWireType( int wireType )
	{
		m_WireMaterial = wireType;
	}
	
	int GetWireType()
	{
		return m_WireMaterial;
	}
	

	override void RefreshState()
	{
		super.RefreshState();
		
		if (GetState() == FOLDED)
		{
			FoldTripWire();
		}
	}
	
	override void SetupTrapPlayer( PlayerBase player, bool set_position = true )
	{
		super.SetupTrapPlayer( player, set_position );
		SetState(DEPLOYED);
	}
	
	override void StartDeactivate(PlayerBase player)
	{
		super.StartDeactivate(player);
		
		DeleteTrigger();
		SetState(FOLDED);
	}
	
	// We do not want players to attach charges before trap is deployed
	override bool CanReceiveAttachment( EntityAI attachment, int slotId )
	{
		if ( GetState() != DEPLOYED )
			return false;

		return super.CanReceiveAttachment( attachment, slotId );
	}
	
	// As players cannot attch charges, we do not display the attachment slot before it is necessary
	override bool CanDisplayAttachmentSlot( int slot_id )
	{
		if ( GetState() != DEPLOYED )
			return false;

		return super.CanDisplayAttachmentSlot( slot_id );
	}
	
	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);
		
		SetTakeable(false);
	}
	
	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
		
		SetTakeable(false);
	}
	
	override void EEKilled(Object killer)
	{
		if (m_TrapTrigger)
		{
			StartDeactivate(null);
		}
		
		super.EEKilled(killer);
	}
	
	// We reset the animation phases to see the tripwire as folded
	void FoldTripWire()
	{
		if ( m_AnimationPhaseGrounded != "" )
		{
			SetAnimationPhase( m_AnimationPhaseSet, 1 );
			
			if ( m_AnimationPhaseTriggered != m_AnimationPhaseGrounded ) 
			{
				SetAnimationPhase( m_AnimationPhaseTriggered, 1 );
			}
			
			SetAnimationPhase( m_AnimationPhaseGrounded, 0 );
		}
	}
	
	override void OnInventoryEnter( Man player )
	{
		SetState( FOLDED );
	}
	
	#ifdef PLATFORM_WINDOWS
	// How one sees the tripwire when in vicinity
	override int GetViewIndex()
	{
		if ( MemoryPointExists( "invView2" ) )
		{		
			InventoryLocation il = new InventoryLocation;
			GetInventory().GetCurrentInventoryLocation( il );
			InventoryLocationType type = il.GetType();
			switch ( type )
			{
				case InventoryLocationType.CARGO:
				{
					return 0;
				}
				case InventoryLocationType.ATTACHMENT:
				{
					return 1;
				}
				case InventoryLocationType.HANDS:
				{
					return 0;
				}
				case InventoryLocationType.GROUND:
				{
					// Different view index depending on deployment state 
					if ( GetState() == DEPLOYED )
						return 1;
					else if ( GetState() == TRIGGERED )
						return 2;
					
					// When folded
					return 0;
				}
				case InventoryLocationType.PROXYCARGO:
				{
					return 0;
				}
				default:
				{
					if ( GetState() == DEPLOYED )
						return 1;
					else if ( GetState() == TRIGGERED )
						return 2;
					
					// When folded
					return 0;
				}
			}
		}
		return 0;
	}
	#endif
	
	//================================================================
	// ADVANCED PLACEMENT
	//================================================================
	
	// On placement complete, set state, play sound, create trigger and synch to client
	override void OnPlacementComplete(Man player, vector position = "0 0 0", vector orientation = "0 0 0")
	{
		super.OnPlacementComplete(player, position, orientation);
		
		SetIsPlaceSound(true);
		if (GetGame().IsServer())
		{
			SetState(DEPLOYED);
			
			m_TriggerPosition			= position;
			m_TriggerOrientation		= orientation;
			m_ResultOfAdvancedPlacing	= true;
		}
	}
	
	override void OnPlacementCancelled(Man player)
	{
		super.OnPlacementCancelled(player);
		
		SetState(FOLDED);
		
		m_ResultOfAdvancedPlacing = false;
	}
	
	override bool IsDeployable()
	{
		return true;
	}
	
	// Tripwire cannot be taken if deployed with attachment
	override bool IsTakeable()
	{
		return !IsRuined() && (GetState() != DEPLOYED || (GetInventory().AttachmentCount() == 0 && GetState() == DEPLOYED));
	}
	
	override string GetDeploySoundset()
	{
		return "tripwire_deploy_SoundSet";
	}
	
	override string GetLoopDeploySoundset()
	{
		return "tripwiretrap_deploy_SoundSet";
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionTogglePlaceObject);
		AddAction(ActionDeployObject);
	}
	
	// ====================================
	// ===========  DEPRECATED  ===========
	// ====================================
	
	void UpdateProxySelections()
	{
		if ( GetInventory().AttachmentCount() > 0)
		{
			ItemBase attachment = ItemBase.Cast( GetInventory().GetAttachmentFromIndex(0) );
			
			if ( attachment )
			{
				// Hide all proxies
				for (int i = 1; i <= 3; i++)
				{
					HideSelection("s" + i + "_charge");
				}
				
				// Now show the one we need to see
				string proxy_to_show = string.Format("s%1_charge", GetState() ); 
				//Print(proxy_to_show);
				ShowSelection( proxy_to_show );
			}
		}
	}

#ifdef DEVELOPER	
	//================================================================
	// DEBUG
	//================================================================
			
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		SetState(DEPLOYED);
		StartActivate(null);
	}
	
	override void GetDebugButtonNames(out string button1, out string button2, out string button3, out string button4)
	{
		button1 = "Activate";
		button2 = "Deactivate";
	}
	
	override void OnDebugButtonPressServer(int button_index)
	{
		switch (button_index)
		{
			case 1:
				StartActivate(null);
			break;
			case 2:
				SetInactive();
			break;
		}
		
	}
#endif
}

class TripwireTrapDeployed : TripwireTrap
{
	
}
