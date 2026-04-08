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
		
		//! universal temperature sources overrides
		m_UTSSettings.m_TemperatureItemCap 		= GameConstants.ITEM_TEMPERATURE_NEUTRAL_ZONE_MIDDLE;
		m_UTSSettings.m_TemperatureCap			= 20;
		
		m_UnderObjectDecalSpawnSettings.m_ScaleMax = 0.44;
		
		m_ThawnSurfaceUnderSupport = true;
	}
	
	override protected void InitializeTemperatureSources()
	{
		m_UTSLFireplace = new UniversalTemperatureSourceLambdaFireplace();
		m_UTSLFireplace.SetSmallFireplaceTemperatureMax(PARAM_SMALL_FIRE_TEMPERATURE);
		m_UTSLFireplace.SetNormalFireplaceTemperatureMax(PARAM_OUTDOOR_FIRE_TEMPERATURE);

		m_UTSource = new UniversalTemperatureSource(this, m_UTSSettings, m_UTSLFireplace);
	}
	
	override bool CanCookOnStick()
	{
		return false;
	}
	
	override bool IsBaseFireplace()
	{
		return true;
	}
	
	override void CreateAreaDamage()
	{
		DestroyAreaDamage();
		
		m_AreaDamage = new AreaDamageLoopedDeferred(this);
		m_AreaDamage.SetDamageComponentType(AreaDamageComponentTypes.HITZONE);
		m_AreaDamage.SetExtents("-0.15 0 -0.15", "0.15 0.75 0.15");
		m_AreaDamage.SetLoopInterval(0.5);
		m_AreaDamage.SetDeferDuration(0.5);
		m_AreaDamage.SetHitZones({"Head","Torso","LeftHand","LeftLeg","LeftFoot","RightHand","RightLeg","RightFoot"});
		m_AreaDamage.SetAmmoName("FireDamage");
		m_AreaDamage.Spawn();
	}
	
	override int GetDamageSystemVersionChange()
	{
		return 110;
	}
	
	override string GetInvulnerabilityTypeString()
	{
		return "disableContainerDamage";
	}
	
	override void OnWasAttached(EntityAI parent, int slot_id)
	{
		super.OnWasAttached(parent, slot_id);
		
		Open();
	}
	
	override void OnWasDetached(EntityAI parent, int slot_id)
	{
		super.OnWasDetached(parent, slot_id);
		
		Close();
	}
	
	override bool CanDetachAttachment(EntityAI parent)
	{
		return GetNumberOfItems() == 0;
	}
	
	
	override void OnStoreSave(ParamsWriteContext ctx)
	{   
		super.OnStoreSave(ctx);
		
		ctx.Write(m_Openable.IsOpened());
	}
	
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;
		
		bool opened;
		if (version >= 110 && !ctx.Read(opened))
		{
			return false;
		}
		
		if (opened)
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

		if (!IsBeingPlaced())
		{
			//Refresh particles and sounds
			RefreshFireParticlesAndSounds(false);
		}
		
		UpdateVisualState();
	}
	
	//ATTACHMENTS
	override bool CanReceiveAttachment(EntityAI attachment, int slotId)
	{
		ItemBase item = ItemBase.Cast(attachment);
		
		if (GetHealthLevel() == GameConstants.STATE_RUINED || GetHierarchyRootPlayer() != null)
			return false;

		//direct cooking slots
		if (!IsOpen())
		{
			if ((item.Type() == ATTACHMENT_CAULDRON) || (item.Type() == ATTACHMENT_COOKING_POT) || (item.Type() == ATTACHMENT_FRYING_PAN) || (item.IsKindOf("Edible_Base")))
			{
				return super.CanReceiveAttachment(attachment, slotId);
			}
		}
		else
		{
			if (IsKindling(item) || IsFuel(item))
			{
				return super.CanReceiveAttachment(attachment, slotId);
			}
		}

		return false;
	}
	
	override bool CanLoadAttachment(EntityAI attachment)
	{
		ItemBase item = ItemBase.Cast(attachment);
		
		if (GetHealthLevel() == GameConstants.STATE_RUINED)
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
		
		if (item_base.IsCookware())
		{
			ClearCookingEquipment(item_base);
			item_base.RemoveAudioVisualsOnClient();	
		}
		
		if (item_base.IsLiquidContainer()) //boiling bottle effects stop
			item_base.RemoveAudioVisualsOnClient();
		
		RefreshFireplaceVisuals();
	}
	
	//CONDITIONS
	//this into/outo parent.Cargo
	override bool CanPutInCargo(EntityAI parent)
	{
		if (!super.CanPutInCargo(parent) || parent.Type().IsInherited(Barrel_ColorBase) || parent.Type().IsInherited(BarrelHoles_ColorBase))
			return false;

		if (IsBurning() || !IsCargoEmpty() || DirectCookingSlotsInUse() || IsOpen())
			return false;
		
		return true;
	}

	override bool CanRemoveFromCargo(EntityAI parent)
	{
		return true;
	}

	//cargo item into/outo this.Cargo
	override bool CanReceiveItemIntoCargo(EntityAI item)
	{
		if (GetHealthLevel() == GameConstants.STATE_RUINED)
			return false;

		if (!IsOpen())
			return false;

		return super.CanReceiveItemIntoCargo(item);
	}
	
	override bool CanLoadItemIntoCargo(EntityAI item)
	{
		if (!super.CanLoadItemIntoCargo(item))
			return false;
		
		if (GetHealthLevel() == GameConstants.STATE_RUINED)
			return false;

		return true;
	}

	override bool CanReleaseCargo(EntityAI cargo)
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
		
		if (!GetInventory().IsAttachment() && IsOpen())
		{
			return false;
		}
		
		return true;
	}
	
	//INVENTORY DISPLAY CONDITIONS
	override bool CanDisplayCargo()
	{
		//super
		if (!super.CanDisplayCargo())
		{
			return false;
		}
		//
		
		return IsOpen();
	}

	override bool CanDisplayAttachmentCategory(string category_name)
	{
		//super
		if (!super.CanDisplayAttachmentCategory(category_name))
		{
			return false;
		}
		//
		
		if ((category_name == "CookingEquipment") || (category_name == "Smoking"))
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
		
		SetTakeable(false);
		UpdateVisualState();
	}
	
	void OpenLoad()
	{
		m_Openable.Open();
		
		SetSynchDirty();
		SetTakeable(false);
		UpdateVisualState();
	}
	
	override void Close()
	{
		m_Openable.Close();
		
		SetTakeable(true);
		UpdateVisualState();
	}
	
	void CloseLoad()
	{
		m_Openable.Close();
		
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
		if (IsOpen())
		{
			SetAnimationPhase(ANIMATION_OPENED, 0);
			SetAnimationPhase(ANIMATION_CLOSED, 1);
		}
		else
		{
			SetAnimationPhase(ANIMATION_OPENED, 1);
			SetAnimationPhase(ANIMATION_CLOSED, 0);
		}
	}
	
	//Can extinguish fire
	override bool CanExtinguishFire()
	{
		if (IsOpen() && IsBurning())
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
	
	override bool CanBeIgnitedBy(EntityAI igniter = NULL)
	{
		if (HasAnyKindling() && !IsBurning() && IsOpen() && !GetHierarchyParent())
		{
			return true;
		}
			
		return false;
	}
	
	override bool CanIgniteItem(EntityAI ignite_target = NULL)
	{
		if (IsBurning() && IsOpen())
		{
			return true;
		}
		
		return false;
	}
	
	override bool IsIgnited()
	{
		return IsBurning();
	}
	
	override void OnIgnitedTarget(EntityAI target_item)
	{
	}
	
	override void OnIgnitedThis(EntityAI fire_source)
	{	
		//remove grass
		Object cc_object = g_Game.CreateObjectEx(OBJECT_CLUTTER_CUTTER , GetPosition(), ECE_PLACE_ON_SURFACE);
		cc_object.SetOrientation (GetOrientation());
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(DestroyClutterCutter, 0.2, false, cc_object);
		
		//start fire
		StartFire(); 
	}

	void DestroyClutterCutter(Object clutter_cutter)
	{
		g_Game.ObjectDelete(clutter_cutter);
	}	
	
	override bool IsThisIgnitionSuccessful(EntityAI item_source = NULL)
	{
		//check kindling
		if (!HasAnyKindling() && IsOpen())
		{
			return false;
		}
		
		//check surface
		if (IsOnWaterSurface())
		{
			return false;
		}

		return true;	
	}
	
	string GetBarrelOpenSoundset()
	{
		return "barrel_open_SoundSet";
	}
	
	string GetBarrelCloseSoundset()
	{
		return "barrel_close_SoundSet";
	}
	
	override string GetDeploySoundset()
	{
		return "placeBarrel_SoundSet";
	}
	
	override void InitItemSounds()
	{
		super.InitItemSounds();
		
		ItemSoundHandler handler = GetItemSoundHandler();
		
		if (GetBarrelOpenSoundset() != string.Empty)
			handler.AddSound(SoundConstants.ITEM_BARREL_OPEN, GetBarrelOpenSoundset());
	
		if (GetBarrelCloseSoundset() != string.Empty)
			handler.AddSound(SoundConstants.ITEM_BARREL_CLOSE, GetBarrelCloseSoundset());
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
	
	// DEPRECATED
	void SoundBarrelOpenPlay();
	void SoundBarrelClosePlay();
}
