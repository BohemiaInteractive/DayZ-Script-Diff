class BarrelHoles_ColorBase extends FireplaceBase
{
	//Visual animations
	const string ANIMATION_OPENED 			= "LidOff";
	const string ANIMATION_CLOSED			= "LidOn";
	
	protected bool m_IsOpenedClient			= false;
	
	protected ref OpenableBehaviour m_Openable;
	
	void BarrelHoles_ColorBase()
	{
		//Particles - default for FireplaceBase
		PARTICLE_FIRE_START 	= ParticleList.BARREL_FIRE_START;
		PARTICLE_SMALL_FIRE 	= ParticleList.BARREL_SMALL_FIRE;
		PARTICLE_NORMAL_FIRE	= ParticleList.BARREL_NORMAL_FIRE;
		PARTICLE_SMALL_SMOKE 	= ParticleList.BARREL_SMALL_SMOKE;
		PARTICLE_NORMAL_SMOKE	= ParticleList.BARREL_NORMAL_SMOKE;
		PARTICLE_FIRE_END 		= ParticleList.BARREL_FIRE_END;
		PARTICLE_STEAM_END		= ParticleList.BARREL_FIRE_STEAM_2END;

		m_Openable = new OpenableBehaviour(false);
		
		//synchronized variables
		RegisterNetSyncVariableBool("m_Openable.m_IsOpened");
		
		ProcessInvulnerabilityCheck(GetInvulnerabilityTypeString());
		
		m_LightDistance = 50;
	}
	
	override int GetDamageSystemVersionChange()
	{
		return 110;
	}
	
	override string GetInvulnerabilityTypeString()
	{
		return "disableContainerDamage";
	}
	
	override void OnWasAttached( EntityAI parent, int slot_id)
	{
		super.OnWasAttached(parent, slot_id);
		
		Open();
	}
	
	override void OnWasDetached(EntityAI parent, int slot_id)
	{
		super.OnWasDetached(parent, slot_id);
		
		Close();
	}
	
	override bool CanDetachAttachment( EntityAI parent )
	{
		if ( GetNumberOfItems() == 0)
			return true;
		return false;
	}
	
	
	override void OnStoreSave( ParamsWriteContext ctx )
	{   
		super.OnStoreSave( ctx );
		
		ctx.Write( m_Openable.IsOpened() );
	}
	
	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		if ( !super.OnStoreLoad( ctx, version ) )
			return false;
		
		bool opened;
		if ( version >= 110 && !ctx.Read( opened ) )
		{
			return false;
		}
		
		if ( opened )
		{
			OpenLoad();
		}
		else
		{
			CloseLoad();
		}
		
		return true;
	}
	
	override bool IsBarrelWithHoles()
	{
		return true;
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		if ( !IsBeingPlaced() )
		{
			//Refresh particles and sounds
			RefreshFireParticlesAndSounds( false );
			
			//sound sync
			if ( IsSoundSynchRemote() && !IsBeingPlaced() && m_Initialized )
			{
				if ( IsOpen() )
				{
					SoundBarrelOpenPlay();
				}
				
				if ( !IsOpen() )
				{
					SoundBarrelClosePlay();
				}
			}
			
		}

		UpdateVisualState();
	}
	
	//ATTACHMENTS
	override bool CanReceiveAttachment( EntityAI attachment, int slotId )
	{
		ItemBase item = ItemBase.Cast( attachment );
		
		if ( GetHealthLevel() == GameConstants.STATE_RUINED || GetHierarchyRootPlayer() != null )
			return false;

		//direct cooking slots
		if ( !IsOpen() )
		{
			if ( ( item.Type() == ATTACHMENT_CAULDRON ) || ( item.Type() == ATTACHMENT_COOKING_POT ) || ( item.Type() == ATTACHMENT_FRYING_PAN ) || ( item.IsKindOf( "Edible_Base" ) ) )
			{
				return super.CanReceiveAttachment(attachment, slotId);
			}
		}
		else
		{
			if ( IsKindling( item ) || IsFuel( item ) )
			{
				return super.CanReceiveAttachment(attachment, slotId);
			}
		}

		return false;
	}
	
	override bool CanLoadAttachment( EntityAI attachment )
	{
		ItemBase item = ItemBase.Cast( attachment );
		
		if ( GetHealthLevel() == GameConstants.STATE_RUINED )
			return false;

		return super.CanLoadAttachment(attachment);
	}

	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);
		
		ItemBase item_base = ItemBase.Cast(item);
		
		if (IsKindling(item_base) || IsFuel(item_base))
		{
			AddToFireConsumables(item_base);
		}
		
		// direct cooking/smoking slots
		bool edible_base_attached = false;
		switch (slot_name)
		{
		case "DirectCookingA":
			m_DirectCookingSlots[0] = item_base;
			edible_base_attached = true;
		break;
		case "DirectCookingB":
			m_DirectCookingSlots[1] = item_base;
			edible_base_attached = true;
		break;
		case "DirectCookingC":
			m_DirectCookingSlots[2] = item_base;
			edible_base_attached = true;
		break;

		case "SmokingA":
			m_SmokingSlots[0] = item_base;
			edible_base_attached = true;
		break;
		case "SmokingB":
			m_SmokingSlots[1] = item_base;
			edible_base_attached = true;
		break;
		case "SmokingC":
			m_SmokingSlots[2] = item_base;
			edible_base_attached = true;
		break;
		case "SmokingD":
			m_SmokingSlots[3] = item_base;
			edible_base_attached = true;
		break;
		}
		
		// reset cooking time (to prevent the cooking exploit)
		if (GetGame().IsServer() && edible_base_attached)
		{
			Edible_Base edBase = Edible_Base.Cast(item_base);
			if (edBase)
			{
				if (edBase.GetFoodStage())
				{
					edBase.SetCookingTime(0);
				}
			}
		}

		RefreshFireplaceVisuals();
	}
	
	override bool IsPrepareToDelete()
	{
		return false;
	}

	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
		
		ItemBase item_base = ItemBase.Cast(item);
		if (IsKindling(item_base) || IsFuel(item_base))
		{
			RemoveFromFireConsumables(GetFireConsumableByItem(item_base));
		}
		
		// direct cooking / smoking slots
		switch (slot_name)
		{
		case "DirectCookingA":
			m_DirectCookingSlots[0] = null;
		break;
		case "DirectCookingB":
			m_DirectCookingSlots[1] = null;
		break;
		case "DirectCookingC":
			m_DirectCookingSlots[2] = null;
		break;

		case "SmokingA":
			m_SmokingSlots[0] = null;
		break;
		case "SmokingB":
			m_SmokingSlots[1] = null;
		break;
		case "SmokingC":
			m_SmokingSlots[2] = null;
		break;
		case "SmokingD":
			m_SmokingSlots[3] = null;
		break;
		}

		// cookware-specifics (remove audio visuals)
		if (item_base.Type() == ATTACHMENT_CAULDRON || item_base.Type() == ATTACHMENT_COOKING_POT)
		{
			ClearCookingEquipment(item_base);
			Bottle_Base cooking_pot = Bottle_Base.Cast(item);
			cooking_pot.RemoveAudioVisualsOnClient();	
		}
		if (item_base.Type() == ATTACHMENT_FRYING_PAN)
		{
			ClearCookingEquipment(item_base);
			FryingPan frying_pan = FryingPan.Cast(item);
			frying_pan.RemoveAudioVisualsOnClient();
		}

		RefreshFireplaceVisuals();
	}
	
	//CONDITIONS
	//this into/outo parent.Cargo
	override bool CanPutInCargo( EntityAI parent )
	{
		if ( !super.CanPutInCargo( parent ) )
			return false;

		if ( IsBurning() || !IsCargoEmpty() || DirectCookingSlotsInUse() || IsOpen() )
			return false;
		
		return true;
	}

	override bool CanRemoveFromCargo( EntityAI parent )
	{
		return true;
	}

	//cargo item into/outo this.Cargo
	override bool CanReceiveItemIntoCargo( EntityAI item )
	{
		if ( GetHealthLevel() == GameConstants.STATE_RUINED )
			return false;

		if ( !IsOpen() || GetHierarchyParent() )
			return false;

		return super.CanReceiveItemIntoCargo( item );
	}
	
	override bool CanLoadItemIntoCargo( EntityAI item )
	{
		if (!super.CanLoadItemIntoCargo( item ))
			return false;
		
		if ( GetHealthLevel() == GameConstants.STATE_RUINED )
			return false;

		if ( GetHierarchyParent() )
			return false;

		return true;
	}

	override bool CanReleaseCargo( EntityAI cargo )
	{
		return IsOpen();
	}
	
	//hands
	override bool CanPutIntoHands(EntityAI parent)
	{
		if (!super.CanPutIntoHands(parent))
		{
			return false;
		}

		if (IsBurning() || !IsCargoEmpty() || DirectCookingSlotsInUse() || SmokingSlotsInUse())
		{
			return false;
		}
		
		if ( !GetInventory().IsAttachment() && IsOpen() )
		{
			return false;
		}
		
		return true;
	}
	
	//INVENTORY DISPLAY CONDITIONS
	override bool CanDisplayCargo()
	{
		//super
		if( !super.CanDisplayCargo() )
		{
			return false;
		}
		//
		
		return IsOpen();
	}

	override bool CanDisplayAttachmentCategory( string category_name )
	{
		//super
		if( !super.CanDisplayAttachmentCategory( category_name ) )
		{
			return false;
		}
		//
		
		if ( ( category_name == "CookingEquipment" ) || ( category_name == "Smoking" ) )
		{
			return !IsOpen();
		}			
		else
		{
			return IsOpen();
		}
		
		return true;
	}	
	// ---	

	//ACTIONS
	override void Open()
	{
		m_Openable.Open();
		
		m_RoofAbove = false;
		
		SoundSynchRemote();
		SetTakeable(false);
		UpdateVisualState();
	}
	
	void OpenLoad()
	{
		m_Openable.Open();
		m_RoofAbove = false;
		
		SetSynchDirty();
		SetTakeable(false);
		UpdateVisualState();
	}
	
	override void Close()
	{
		m_Openable.Close();
		m_RoofAbove = true;
		
		SoundSynchRemote();
		SetTakeable(true);
		UpdateVisualState();
	}
	
	void CloseLoad()
	{
		m_Openable.Close();
		m_RoofAbove = true;
		
		SetSynchDirty();
		SetTakeable(true);
		UpdateVisualState();
	}
	
	override bool IsOpen()
	{
		return m_Openable.IsOpened();
	}
	
	protected void UpdateVisualState()
	{
		if ( IsOpen() )
		{
			SetAnimationPhase( ANIMATION_OPENED, 0 );
			SetAnimationPhase( ANIMATION_CLOSED, 1 );
		}
		else
		{
			SetAnimationPhase( ANIMATION_OPENED, 1 );
			SetAnimationPhase( ANIMATION_CLOSED, 0 );
		}
	}
	
	//Can extinguish fire
	override bool CanExtinguishFire()
	{
		if ( IsOpen() && IsBurning() )
		{
			return true;
		}
		
		return false;
	}	
	
	//particles
	override bool CanShowSmoke()
	{
		return IsOpen();
	}
	
	// Item-to-item fire distribution
	override bool HasFlammableMaterial()
	{
		return true;
	}
	
	override bool CanBeIgnitedBy( EntityAI igniter = NULL )
	{
		if ( HasAnyKindling() && !IsBurning() && IsOpen() && !GetHierarchyParent() )
		{
			return true;
		}
			
		return false;
	}
	
	override bool CanIgniteItem( EntityAI ignite_target = NULL )
	{
		if ( IsBurning() && IsOpen() )
		{
			return true;
		}
		
		return false;
	}
	
	override bool IsIgnited()
	{
		return IsBurning();
	}
	
	override void OnIgnitedTarget( EntityAI ignited_item )
	{
	}
	
	override void OnIgnitedThis( EntityAI fire_source )
	{	
		//remove grass
		Object cc_object = GetGame().CreateObjectEx( OBJECT_CLUTTER_CUTTER , GetPosition(), ECE_PLACE_ON_SURFACE );
		cc_object.SetOrientation ( GetOrientation() );
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( DestroyClutterCutter, 0.2, false, cc_object );
		
		//start fire
		StartFire(); 
	}

	void SoundBarrelOpenPlay()
	{
		EffectSound sound =	SEffectManager.PlaySound( "barrel_open_SoundSet", GetPosition() );
		sound.SetAutodestroy( true );
	}
	
	void SoundBarrelClosePlay()
	{
		EffectSound sound =	SEffectManager.PlaySound( "barrel_close_SoundSet", GetPosition() );
		sound.SetAutodestroy( true );
	}
	
	void DestroyClutterCutter( Object clutter_cutter )
	{
		GetGame().ObjectDelete( clutter_cutter );
	}	
	
	override bool IsThisIgnitionSuccessful( EntityAI item_source = NULL )
	{
		//check kindling
		if ( !HasAnyKindling() && IsOpen() )
		{
			return false;
		}
		
		//check surface
		if ( IsOnWaterSurface() )
		{
			return false;
		}

		return true;	
	}
	
	//================================================================
	// ADVANCED PLACEMENT
	//================================================================
	
	override string GetPlaceSoundset()
	{
		return "placeBarrel_SoundSet";
	}
	
	override void SetActions()
	{
		AddAction(ActionAttachOnSelection);
		super.SetActions();
		
		AddAction(ActionOpenBarrelHoles);
		AddAction(ActionCloseBarrelHoles);
		AddAction(ActionPlaceObject);
		AddAction(ActionTogglePlaceObject);
	}
	
	override void OnDebugSpawn()
	{
		m_Openable.Open();
		super.OnDebugSpawn();
		m_Openable.Close();
	}
}
