class Spotlight extends ItemBase
{
	private bool	m_IsFolded;
	private bool 	m_EvaluateDeployment;
	SpotlightLight 	m_Light;
	
	static vector 	m_LightLocalPosition;
	static vector 	m_LightLocalOrientation = "0 0 0";
	
	// Spotlight can be extended and compressed
	static const string SEL_REFLECTOR_COMP_U 		= "reflector";
	static const string SEL_CORD_FOLDED_U 			= "cord_folded";
	static const string SEL_CORD_PLUGGED_U 			= "cord_plugged";
	static const string SEL_CORD_PLUGGED_F 			= "spotlight_folded_cord_plugged";
	static const string SEL_CORD_FOLDED_F 			= "spotlight_folded_cord_folded";
	
	static const string SEL_INVENTORY 				= "inventory";
	static const string SEL_PLACING 				= "placing";
	static const string SEL_GLASS_F 				= "glass_folded";
	static const string SEL_GLASS_U 				= "glass_unfolded";
	static const string SEL_REFLECTOR_F				= "reflector_folded";
	static const string SEL_REFLECTOR_U				= "reflector_unfolded";
	
	static const int 	ID_GLASS_UNFOLDED			= 3;
	static const int 	ID_REFLECTOR_UNFOLDED		= 4;
	static const int 	ID_GLASS_FOLDED				= 5;
	static const int 	ID_REFLECTOR_FOLDED			= 6;
	
	static string 		LIGHT_OFF_GLASS 			= "dz\\gear\\camping\\Data\\spotlight_glass.rvmat";
	static string 		LIGHT_OFF_REFLECTOR 		= "dz\\gear\\camping\\Data\\spotlight.rvmat";
	static string 		LIGHT_ON_GLASS 				= "dz\\gear\\camping\\Data\\spotlight_glass_on.rvmat";
	static string 		LIGHT_ON_REFLECTOR 			= "dz\\gear\\camping\\Data\\spotlight_glass_on.rvmat";
	
	//sound
	const string 				SOUND_TURN_ON		= "spotlight_turn_on_SoundSet";
	const string 				SOUND_TURN_OFF		= "spotlight_turn_off_SoundSet";
	
	protected EffectSound 	m_SoundTurnOn;
	protected EffectSound 	m_SoundTurnOff;
	protected EffectSound 	m_DeployLoopSound;

	//Spotlight, folded and unfolded
	void Spotlight()
	{
		m_EvaluateDeployment = false;

		RegisterNetSyncVariableBool("m_IsSoundSynchRemote");
		RegisterNetSyncVariableBool("m_IsDeploySound");
		RegisterNetSyncVariableBool("m_IsFolded");
	}
	
	void ~Spotlight()
	{
		if (m_DeployLoopSound)
			SEffectManager.DestroyEffect(m_DeployLoopSound);
	}
	
	override bool IsElectricAppliance()
	{
		return true;
	}
	
	override void OnInitEnergy()
	{
		if (GetCompEM().IsPlugged())
		{
			if (!GetHierarchyRoot())
				Unfold();
			else
				Fold(true);
		}
		else
			Fold();
		
		UpdateAllSelections();
	}
	
	override void EOnInit(IEntity other, int extra)
	{
		if (!IsHologram())
		{
			UpdateAllSelections();
			HideSelection(SEL_CORD_FOLDED_F);
		}
		else
		{
			Unfold();
		}
	}
	
	//--- POWER EVENTS
	override void OnSwitchOn()
	{
		super.OnSwitchOn();
		
		SoundTurnOn();
	}

	override void OnSwitchOff()
	{
		super.OnSwitchOff();
		
		SoundTurnOff();
		
		if (m_Light)
		{
			m_Light.FadeOut(0.05);
			m_Light = null;
		}
	}
	
	override void OnWorkStart()
	{
		#ifndef SERVER
		m_Light = SpotlightLight.Cast(ScriptedLightBase.CreateLight(SpotlightLight, "0 0 0"));
		m_Light.AttachOnObject(this, m_LightLocalPosition, m_LightLocalOrientation);
		#endif
		
		UpdateAllSelections();
	}

	override void OnWorkStop()
	{
		#ifndef SERVER
		if (m_Light)
			m_Light.FadeOut();
		#endif
		
		UpdateAllSelections();
	}
	
	// Called when this device is picked up
	override void OnItemLocationChanged(EntityAI old_owner, EntityAI new_owner)
	{
		super.OnItemLocationChanged(old_owner, new_owner);
		
		//skips folding of currently deployed spotlight
		if (m_EvaluateDeployment || IsBeingPlaced())
		{
			m_EvaluateDeployment = false;
			if (m_Light)
			{
				m_Light.AttachOnObject(this, m_LightLocalPosition, m_LightLocalOrientation);
			}

			Unfold();

			return;
		}
		
		// When the item is picked up by a player
		PlayerBase player = PlayerBase.Cast(new_owner);
		if (player && player.GetItemInHands() == this)
		{
			Fold(GetCompEM().IsPlugged());
		}
	}

	override void OnIsPlugged(EntityAI source_device)
	{
		Unfold();
		UpdateAllSelections();
	}
	
	protected void UpdateAllSelections()
	{
		bool isPlugged = GetCompEM().IsPlugged();
		PlayerBase player = PlayerBase.Cast(GetHierarchyRootPlayer());
		bool isInHands = player && player.GetItemInHands() == this;
		HideAllSelections();
		
		m_LightLocalPosition = GetSelectionPositionLS("beamStart");
		
		if (IsFolded() || isInHands)
		{
			HideSelection(SEL_REFLECTOR_COMP_U);
			
			ShowSelection(SEL_INVENTORY);
			ShowSelection(SEL_GLASS_F);
			ShowSelection(SEL_REFLECTOR_F);
			
			if (isPlugged)
			{
				ShowSelection(SEL_CORD_PLUGGED_F);
			}
			else
			{
				ShowSelection(SEL_CORD_FOLDED_F);
			}
			
			if (!IsHologram())
			{
				if (GetCompEM().IsWorking())
				{
					SetObjectMaterial(ID_GLASS_UNFOLDED, LIGHT_ON_GLASS);
					SetObjectMaterial(ID_REFLECTOR_UNFOLDED, LIGHT_ON_REFLECTOR);
					SetObjectMaterial(ID_GLASS_FOLDED, LIGHT_ON_GLASS);
					SetObjectMaterial(ID_REFLECTOR_FOLDED, LIGHT_ON_REFLECTOR);
				}
				else
				{
					SetObjectMaterial(ID_GLASS_UNFOLDED, LIGHT_OFF_GLASS);
					SetObjectMaterial(ID_REFLECTOR_UNFOLDED, LIGHT_OFF_REFLECTOR);
					SetObjectMaterial(ID_GLASS_FOLDED, LIGHT_OFF_GLASS);
					SetObjectMaterial(ID_REFLECTOR_FOLDED, LIGHT_OFF_REFLECTOR);
				}
			}
		}
		else
		{
			ShowSelection(SEL_PLACING);
			ShowSelection(SEL_REFLECTOR_U);
			ShowSelection(SEL_GLASS_U);
			ShowSelection(SEL_REFLECTOR_COMP_U);
			
			if (isPlugged)
			{
				ShowSelection(SEL_CORD_PLUGGED_U);
			}
			else
			{
				ShowSelection(SEL_CORD_FOLDED_U);
			}
			
			if (!IsHologram())
			{
				if (GetCompEM().IsWorking())
				{
					SetObjectMaterial(ID_GLASS_UNFOLDED, LIGHT_ON_GLASS);
					SetObjectMaterial(ID_REFLECTOR_UNFOLDED, LIGHT_ON_REFLECTOR);
				}
				else
				{
					SetObjectMaterial(ID_GLASS_UNFOLDED, LIGHT_OFF_GLASS);
					SetObjectMaterial(ID_REFLECTOR_UNFOLDED, LIGHT_OFF_REFLECTOR);
				}
			}
		}
	}
	
	override void OnIsUnplugged(EntityAI last_energy_source)
	{
		Fold();
		UpdateAllSelections();
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		if (IsDeploySound())
		{
			PlayDeploySound();
		}
		
		if (CanPlayDeployLoopSound())
		{
			PlayDeployLoopSound();
		}
		
		if (m_DeployLoopSound && !CanPlayDeployLoopSound())
		{
			StopDeployLoopSound();
		}
	}

	void Fold(bool keep_connected = false)
	{
		m_IsFolded = true;
		if (!keep_connected)
		{
			GetCompEM().SwitchOff();
			GetCompEM().UnplugThis();
		}

		UpdateAllSelections();
		SetSynchDirty();
	}

	void Unfold()
	{
		m_IsFolded = false;
		UpdateAllSelections();
		SetSynchDirty();
	}

	override void OnStoreSave(ParamsWriteContext ctx)
	{   
		super.OnStoreSave(ctx);
		
		// Save folded/unfolded state
		ctx.Write(m_IsFolded);
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{   
		if (!super.OnStoreLoad(ctx, version))
			return false;
		
		// Load folded/unfolded state
		bool b_is_folded = false;
		if (!ctx.Read(b_is_folded))
			b_is_folded = true;
		
		if (b_is_folded)
		{
			Fold();
		}
		else
		{
			Unfold();
		}

		return true;
	}

	override bool CanPutInCargo(EntityAI parent)
	{
		if (!super.CanPutInCargo(parent)) 
			return false;
		
		return !GetCompEM().IsPlugged();
	}

	override bool CanPutIntoHands(EntityAI player) 
	{
		if (!super.CanPutIntoHands(parent))
		{
			return false;
		}
		// Commented out so Reposition action is possible to execute
		return true;
	}

	bool IsFolded()
	{
		return m_IsFolded;
	}

	//================================================================
	// SOUNDS
	//================================================================
	protected void SoundTurnOn()
	{
		PlaySoundSet(m_SoundTurnOn, SOUND_TURN_ON, 0, 0);
	}

	protected void SoundTurnOff()
	{
		PlaySoundSet(m_SoundTurnOff, SOUND_TURN_OFF, 0, 0);
	}

	//================================================================
	// ADVANCED PLACEMENT
	//================================================================
	override void OnPlacementStarted(Man player)
	{
		super.OnPlacementStarted(player);
		
		array<string> selections = {
			SEL_CORD_PLUGGED_F,
			SEL_CORD_FOLDED_F
		};
		
		PlayerBase playerPB = PlayerBase.Cast(player);
		foreach (string selection : selections)
		{		
			if (GetGame().IsMultiplayer() && GetGame().IsServer())
			{
				playerPB.GetHologramServer().SetSelectionToRefresh(selection);		
			}
			else
			{
				playerPB.GetHologramLocal().SetSelectionToRefresh(selection);
			}
		}
	}
	
	override void OnPlacementComplete(Man player, vector position = "0 0 0", vector orientation = "0 0 0")
	{
		super.OnPlacementComplete(player, position, orientation);
		
		if (GetGame().IsServer())
		{
			SetIsDeploySound(true);
		}

		Unfold();
		m_EvaluateDeployment = true;
	}
	
	override void OnPlacementCancelled(Man player)
	{
		super.OnPlacementCancelled(player);
		
		Fold(true);
	}
	
	override bool IsDeployable()
	{
		return true;
	}
	
	override string GetDeploySoundset()
	{
		return "placeSpotlight_SoundSet";
	}
	
	override string GetLoopDeploySoundset()
	{
		return "spotlight_deploy_SoundSet";
	}
	
	void PlayDeployLoopSound()
	{		
		if (!GetGame().IsDedicatedServer() && !m_DeployLoopSound)
		{		
			m_DeployLoopSound = SEffectManager.PlaySound(GetLoopDeploySoundset(), GetPosition());
			m_DeployLoopSound.SetAutodestroy(true);
		}
	}
	
	void StopDeployLoopSound()
	{
		if (!GetGame().IsDedicatedServer())
		{	
			m_DeployLoopSound.SetSoundFadeOut(0.5);
			m_DeployLoopSound.SoundStop();
		}
	}
	
	override int GetViewIndex()
	{
		if (MemoryPointExists( "invView2" ) && !m_IsFolded)
		{
			return 1;
		}

		return 0;
	}
	
	override void SetActions()
	{
		super.SetActions();

		RemoveAction(ActionTakeItemToHands);
		
		AddAction(ActionClapBearTrapWithThisItem);
		AddAction(ActionPlugIn);
		AddAction(ActionTogglePlaceObject);
		AddAction(ActionUnplugThisByCord);
		AddAction(ActionTurnOnSpotlight);
		AddAction(ActionTurnOffSpotlight);
		AddAction(ActionRepositionPluggedItem);
		AddAction(ActionDeployObject);
		AddAction(ActionTakeItemToHands);
	}
}