class Spotlight extends ItemBase
{
	private bool	m_IsFolded;
	private bool 	m_EvaluateDeployment;
	SpotlightLight 	m_Light;
	
	static vector 	m_LightLocalPosition 	= "0 1.50668 0.134863"; // We can't use GetMemoryPointPos() on this object, so we need to remember light position like this instead.
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
	
	static const int 	ID_GLASS_UNFOLDED					= 3;
	static const int 	ID_REFLECTOR_UNFOLDED				= 4;
	static const int 	ID_GLASS_FOLDED						= 5;
	static const int 	ID_REFLECTOR_FOLDED					= 6;
	
	static string 		LIGHT_OFF_GLASS 			= "dz\\gear\\camping\\Data\\spotlight_glass.rvmat";
	static string 		LIGHT_OFF_REFLECTOR 		= "dz\\gear\\camping\\Data\\spotlight.rvmat";
	static string 		LIGHT_ON_GLASS 				= "dz\\gear\\camping\\Data\\spotlight_glass_on.rvmat";
	static string 		LIGHT_ON_REFLECTOR 			= "dz\\gear\\camping\\Data\\spotlight_glass_on.rvmat";
	
	//sound
	const string 				SOUND_TURN_ON		= "spotlight_turn_on_SoundSet";
	const string 				SOUND_TURN_OFF		= "spotlight_turn_off_SoundSet";
	
	protected EffectSound 		m_SoundTurnOn;
	protected EffectSound 		m_SoundTurnOff;	
	
	ref protected EffectSound 	m_DeployLoopSound;

	/*
		Spotlight, folded and unfolded.
	*/
	
	void Spotlight()
	{
		m_EvaluateDeployment = false;

		RegisterNetSyncVariableBool("m_IsSoundSynchRemote");
		RegisterNetSyncVariableBool("m_IsDeploySound");
		RegisterNetSyncVariableBool("m_IsFolded");
	}
	
	void ~Spotlight()
	{
		SEffectManager.DestroyEffect( m_DeployLoopSound );
	}
	
	override bool IsElectricAppliance()
	{
		return true;
	}
	
	override void OnInitEnergy()
	{
		if ( GetCompEM().IsPlugged() )
		{
			if (!GetHierarchyRoot())
				Unfold();
			else
				Fold(true);
		}
		/*else if ( IsHologram() )
			Unfold();*/
		else
			Fold();
		
		UpdateAllSelections();
	}
	
	override void EOnInit(IEntity other, int extra)
	{
		super.EOnInit(other, extra);
		
		if ( !IsHologram() ) //todo; object can't know it's a hologram at this point!
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
		
		//sound (client only)
		SoundTurnOn();
	}

	override void OnSwitchOff()
	{
		super.OnSwitchOff();
		
		//sound (client only)
		SoundTurnOff();
		
		if (m_Light)
		{
			m_Light.FadeOut(0.05);
			m_Light = null;
		}
	}
	
	override void OnWorkStart()
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() ) // client side
		{
			m_Light = SpotlightLight.Cast( ScriptedLightBase.CreateLight( SpotlightLight, "0 0 0") );
			m_Light.AttachOnObject(this, m_LightLocalPosition, m_LightLocalOrientation);
		}
		
		UpdateAllSelections();
		/*SetObjectMaterial(ID_GLASS_UNFOLDED, LIGHT_ON_GLASS);
		SetObjectMaterial(ID_REFLECTOR_UNFOLDED, LIGHT_ON_REFLECTOR);
		SetObjectMaterial(ID_GLASS_FOLDED, LIGHT_ON_GLASS);
		SetObjectMaterial(ID_REFLECTOR_FOLDED, LIGHT_ON_REFLECTOR);*/
	}

	override void OnWorkStop()
	{
		if ( !GetGame().IsServer() || !GetGame().IsMultiplayer() ) // client side
		{
			if (m_Light)
				m_Light.FadeOut();
		}
		
		UpdateAllSelections();
		/*SetObjectMaterial(ID_GLASS_UNFOLDED, LIGHT_OFF_GLASS);
		SetObjectMaterial(ID_REFLECTOR_UNFOLDED, LIGHT_OFF_REFLECTOR);
		SetObjectMaterial(ID_GLASS_FOLDED, LIGHT_OFF_GLASS);
		SetObjectMaterial(ID_REFLECTOR_FOLDED, LIGHT_OFF_REFLECTOR);*/
	}
	
	// Called when this device is picked up
	override void OnItemLocationChanged( EntityAI old_owner, EntityAI new_owner ) 
	{
		super.OnItemLocationChanged(old_owner, new_owner);
		
		//skips folding of currently deployed spotlight
		if ( m_EvaluateDeployment || IsBeingPlaced() )
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
		if ( player && player.GetItemInHands() == this )
		{
			Fold(GetCompEM().IsPlugged());
			//player.TogglePlacingLocal();
		}
	}

	override void OnIsPlugged(EntityAI source_device)
	{
		Unfold(); //todo
		UpdateAllSelections();
	}
	
	void UpdateAllSelections()
	{
		bool is_plugged = GetCompEM().IsPlugged();		
		PlayerBase player = PlayerBase.Cast(GetHierarchyRootPlayer());
		bool is_in_hands = player && player.GetItemInHands() == this;
		HideAllSelections();
		
		if ( IsFolded() || is_in_hands ) //todo
		{
			HideSelection( SEL_REFLECTOR_COMP_U );
			
			ShowSelection( SEL_INVENTORY );
			ShowSelection( SEL_GLASS_F );
			ShowSelection( SEL_REFLECTOR_F );
			
			if (is_plugged)
			{
				ShowSelection( SEL_CORD_PLUGGED_F );
			}
			else
			{
				ShowSelection( SEL_CORD_FOLDED_F );
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
			ShowSelection( SEL_PLACING );
			ShowSelection( SEL_REFLECTOR_U );
			ShowSelection( SEL_GLASS_U );
			ShowSelection( SEL_REFLECTOR_COMP_U );
			
			if (is_plugged)
			{
				ShowSelection( SEL_CORD_PLUGGED_U );
			}
			else
			{
				ShowSelection( SEL_CORD_FOLDED_U );
			}
			
			if (!IsHologram())
			{
				if (GetCompEM().IsWorking())
				{
					SetObjectMaterial(ID_GLASS_UNFOLDED, LIGHT_ON_GLASS);
					SetObjectMaterial(ID_REFLECTOR_UNFOLDED, LIGHT_ON_REFLECTOR);
					//SetObjectMaterial(ID_GLASS_FOLDED, LIGHT_ON_GLASS);
					//SetObjectMaterial(ID_REFLECTOR_FOLDED, LIGHT_ON_REFLECTOR);
				}
				else
				{
					SetObjectMaterial(ID_GLASS_UNFOLDED, LIGHT_OFF_GLASS);
					SetObjectMaterial(ID_REFLECTOR_UNFOLDED, LIGHT_OFF_REFLECTOR);
					//SetObjectMaterial(ID_GLASS_FOLDED, LIGHT_OFF_GLASS);
					//SetObjectMaterial(ID_REFLECTOR_FOLDED, LIGHT_OFF_REFLECTOR);
				}
			}
		}
	}
	
	override void OnIsUnplugged( EntityAI last_energy_source )
	{
		UpdateAllSelections();
	}

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
	}

	void Fold(bool keep_connected = false)
	{
		m_IsFolded = true;
		if (!keep_connected)
		{
			GetCompEM().SwitchOff();
			GetCompEM().UnplugThis();
		}
		SetSynchDirty();
		
		UpdateAllSelections();
	}

	void Unfold()
	{
		m_IsFolded = false;
		SetSynchDirty();
		
		UpdateAllSelections();
	}

	override void OnStoreSave(ParamsWriteContext ctx)
	{   
		super.OnStoreSave(ctx);
		
		// Save folded/unfolded state
		ctx.Write( m_IsFolded );
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{   
		if ( !super.OnStoreLoad(ctx, version) )
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

	override bool CanPutInCargo( EntityAI parent )
	{
		if( !super.CanPutInCargo(parent) ) {return false;}
		
		return !GetCompEM().IsPlugged(); //GetCompEM().IsCordFolded(); 
	}

	override bool CanPutIntoHands ( EntityAI player ) 
	{
		if( !super.CanPutIntoHands( parent ) )
		{
			return false;
		}
		// Commented out so Reposition action is possible to execute
		return true; //GetCompEM().IsCordFolded();
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
		PlaySoundSet( m_SoundTurnOn, SOUND_TURN_ON, 0, 0 );
	}

	protected void SoundTurnOff()
	{
		PlaySoundSet( m_SoundTurnOff, SOUND_TURN_OFF, 0, 0 );
	}

	//================================================================
	// ADVANCED PLACEMENT
	//================================================================
	override void OnPlacementStarted( Man player )
	{
		super.OnPlacementStarted( player );
		
		ref array<string> array_of_selections = {SEL_CORD_PLUGGED_F, SEL_CORD_FOLDED_F};
		PlayerBase player_PB = PlayerBase.Cast( player );
		
		if( GetGame().IsMultiplayer() && GetGame().IsServer() )
		{
			player_PB.GetHologramServer().SetSelectionToRefresh( array_of_selections );		
		}
		else
		{
			player_PB.GetHologramLocal().SetSelectionToRefresh( array_of_selections );
		}
	}
	
	override void OnPlacementComplete( Man player, vector position = "0 0 0", vector orientation = "0 0 0" )
	{
		super.OnPlacementComplete( player, position, orientation );
		
		if ( GetGame().IsServer() )
		{
			SetIsDeploySound( true );
		}
		Unfold();
		//hack
		/*if (m_Light)
		{
			Print("pos1: " + GetPosition());
			Print("pos2: " + position);
			vector diff = position - GetPosition();
			Print(diff);
			m_Light.AttachOnObject(this, diff + m_LightLocalPosition, orientation);
		}*/
		m_EvaluateDeployment = true;
	}
	
	override void OnPlacementCancelled( Man player )
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
		if ( !m_DeployLoopSound || !GetGame().IsDedicatedServer() )
		{		
			m_DeployLoopSound = SEffectManager.PlaySound( GetLoopDeploySoundset(), GetPosition());
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