class BarbedWire extends ItemBase
{
	// Sounds lists
	const static int		SOUNDS_SPARK_COUNT							= 4;
	const static int		SOUNDS_CUT_COUNT							= 3;
	const static int		SOUNDS_COLLISION_COUNT						= 4;
	const static int		SOUNDS_SHOCK_COUNT							= 4;
	const static float		RANDOM_SPARK_INTERVAL						= 5.0; // TO DO! Currently not used.
	
	const static string 		m_SoundsSpark[SOUNDS_SPARK_COUNT] 			= {"electricFenceSpark1", "electricFenceSpark2", "electricFenceSpark3", "electricFenceSpark4"};
	const static string 		m_SoundsCut[SOUNDS_CUT_COUNT] 				= {"barbedFenceCut1", "barbedFenceCut2", "barbedFenceCut3"};
	const static string 		m_SoundsCollision[SOUNDS_COLLISION_COUNT]	= {"barbedFenceCollision1", "barbedFenceCollision2", "barbedFenceCollision3", "barbedFenceCollision4"};
	const static string 		m_SoundsShock[SOUNDS_SHOCK_COUNT] 			= {"electricFenceShock1", "electricFenceShock2", "electricFenceShock3", "electricFenceShock4"};
	const static string 		m_SoundBuzzLoop 							= "electricFenceBuzzLoop1";
	ref protected EffectSound 	m_DeployLoopSound;
	
	SoundOnVehicle m_BuzzSoundLoop;
	
	ref Timer m_SparkEvent;
	protected ref AreaDamageManager m_AreaDamage;
	
	protected bool m_TriggerActive;
	protected bool m_IsPlaced;
	
	//mounting
	protected bool m_IsMounted;
	protected bool m_LastMountedState;
	const string 				SOUND_MOUNT	= "putDown_BarbedWire_SoundSet";
	protected EffectSound 		m_MountSound;
	
	
	void BarbedWire()
	{
		m_SparkEvent 	= new Timer( CALL_CATEGORY_SYSTEM );
		m_TriggerActive = false;
		m_IsPlaced 		= false;
		
		//synchronized variables
		RegisterNetSyncVariableBool( "m_IsSoundSynchRemote" );	
		RegisterNetSyncVariableBool( "m_IsDeploySound" );
		RegisterNetSyncVariableBool( "m_IsMounted" );
	}
	
	void ~BarbedWire()
	{
		SEffectManager.DestroyEffect( m_DeployLoopSound );
	}
	
	override void EEInit()
	{
		super.EEInit();
	
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( UpdateAttachmentSlot, 100, false );
	}
	
	bool IsMounted()
	{
		return GetSlotLockedState();
	}
	
	protected bool GetSlotLockedState()
	{
		BaseBuildingBase base_building = BaseBuildingBase.Cast( GetHierarchyParent() );
		if ( base_building )
		{
			InventoryLocation inventory_location = new InventoryLocation;
			GetInventory().GetCurrentInventoryLocation( inventory_location );			
			return base_building.GetInventory().GetSlotLock( inventory_location.GetSlot() );
		}
		
		return false;	
	}
	
	void SetMountedState( bool is_mounted )
	{
		bsbDebugPrint("[bsb] " + GetDebugName(this) + " SetMountedState mounted=" + is_mounted);
		
		//lock slot
		m_IsMounted = is_mounted;
		LockAttachmentSlot( is_mounted );
		SetTakeable( !is_mounted );
		
		//synchronize
		Synchronize();
	}
	
	protected void UpdateAttachmentSlot()
	{
		BaseBuildingBase base_building = BaseBuildingBase.Cast( GetHierarchyParent() );
		if ( base_building )
		{
			InventoryLocation inventory_location = new InventoryLocation;
			GetInventory().GetCurrentInventoryLocation( inventory_location );			
			bool is_mounted = base_building.GetInventory().GetSlotLock( inventory_location.GetSlot() );
			string slot_name = InventorySlots.GetSlotName( inventory_location.GetSlot() );
			
			base_building.UpdateAttachmentVisuals( slot_name, is_mounted );
			base_building.UpdateAttachmentPhysics( slot_name, is_mounted );
		}
	}

	protected void LockAttachmentSlot( bool lock_state )
	{
		BaseBuildingBase base_building = BaseBuildingBase.Cast( GetHierarchyParent() );
		if ( base_building )
		{
			InventoryLocation inventory_location = new InventoryLocation;
			GetInventory().GetCurrentInventoryLocation( inventory_location );			
			base_building.GetInventory().SetSlotLock( inventory_location.GetSlot(), lock_state );
			//string slot_name = InventorySlots.GetSlotName( inventory_location.GetSlot() );
			//base_building.UpdateAttachmentVisuals( slot_name, lock_state );
			//base_building.UpdateAttachmentPhysics( slot_name, lock_state );
		}
	}
		
	// --- SYNCHRONIZATION
	void Synchronize()
	{
		if ( GetGame().IsServer() )
		{
			SetSynchDirty();
		}
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		if ( ( m_IsMounted && !m_LastMountedState ) || ( !m_IsMounted && m_LastMountedState ) )
		{
			//Play sound
			PlaySoundSet( m_MountSound, SOUND_MOUNT, 0.1, 0.1 );
		}
		m_LastMountedState = m_IsMounted;
		
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
	
	// --- EVENTS
	override void OnStoreSave( ParamsWriteContext ctx )
	{   
		super.OnStoreSave( ctx );
	}
	
	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		if ( !super.OnStoreLoad( ctx, version ) )
			return false;
		
		//--- Barbed wire data ---
		//is mounted (removed in ver. 105)
		if ( version < 105 )
		{
			float is_mounted;
			if ( !ctx.Read( is_mounted ) )
			{
				return false;
			}			
		}
		//---
		
		return true;
	}

	override void AfterStoreLoad()
	{	
		super.AfterStoreLoad();
				
		//set mounted state based on locked slot after everything is loaded
		SetMountedState( GetSlotLockedState() );
	}

	// ---
	override void OnWorkStart()
	{
		SoundBuzzLoopStart();
		if (m_TriggerActive)
			{ DestroyDamageTrigger(); }

		if (m_IsPlaced)
		{
			//TimerRandomSpark();
			CreateElectrifiedDamageTrigger();
		}
	}	
	
	override void OnWorkStop()
	{
		SoundBuzzLoopStop();
		if (m_TriggerActive)
			{ DestroyDamageTrigger(); }
		
		if (m_IsPlaced)
			{ CreateDamageTrigger(); }
		
		m_SparkEvent.Stop();
	}

	override void OnWork( float consumed_energy ) {}

	override void OnIsPlugged(EntityAI source_device)
	{
		SoundCut();
	}

	override void OnIsUnplugged( EntityAI last_energy_source )
	{
		if (m_TriggerActive)
			{ DestroyDamageTrigger(); }
		SoundCut();
	}
	
	override void OnInventoryEnter(Man player)
	{
		super.OnInventoryEnter(player);
		HideSelection("placing");
		ShowSelection("zbytek");
		if (m_TriggerActive)
			{ DestroyDamageTrigger(); }
		GetCompEM().UnplugThis();
		GetCompEM().UnplugAllDevices();
	}
	
	// Area Damage triggers
	// ---------------------------------------------------------			
	protected void CreateElectrifiedDamageTrigger()
	{
		m_AreaDamage = new AreaDamageRegular(this);
		m_AreaDamage.SetExtents("-1 0 -0.4", "1 0.7 0.4");
		m_AreaDamage.SetLoopInterval(0.3);
		m_AreaDamage.SetHitZones({"RightLeg", "LeftLeg", "RightFoot", "LeftFoot"});
		m_AreaDamage.SetAmmoName("BarbedWireHit");
		m_AreaDamage.Spawn();
		m_TriggerActive = true;
	}
	
	protected void CreateDamageTrigger()
	{
		m_AreaDamage = new AreaDamageOneTime(this);
		m_AreaDamage.SetExtents("-1 0 -0.4", "1 0.7 0.4");
		m_AreaDamage.SetHitZones({"RightLeg", "LeftLeg", "RightFoot", "LeftFoot"});
		m_AreaDamage.SetAmmoName("BarbedWireHit");
		m_AreaDamage.Spawn();
		m_TriggerActive = true;
	}
	
	protected void DestroyDamageTrigger()
	{
		m_AreaDamage.Destroy();
		m_TriggerActive = false;
	}
	// ---------------------------------------------------------
	
	// Controls spawn of random sparks
	/*
	protected void TimerRandomSpark() // TO DO: Come up with randomized functionality.
	{
		if ( GetCompEM().IsSwitchedOn() )
		{
			int plugged_devices = GetCompEM().GetEnergySource().GetCompEM().GetPluggedDevicesCount();
			float rnd_time = Math.RandomFloat(0.3, RANDOM_SPARK_INTERVAL / plugged_devices + 1.0);
			m_SparkEvent.Run(rnd_time + 0.3, this, "Spark", NULL, true);
		}
	}
	*/

	// Spawns spark particle effect and plays sound.
	void Spark()
	{
		ParticleManager.GetInstance().PlayOnObject( ParticleList.BARBED_WIRE_SPARKS, this);
		SoundSpark();
	}
	
	
	// SOUNDS
	// ---------------------------------------------------------
	void SoundCut()
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() ) // client side
		{
			int random_index = Math.RandomInt(0, SOUNDS_CUT_COUNT);
			string sound_type = m_SoundsCut[random_index];
			PlaySound(sound_type, 50);
		}
	}

	// Plays sound
	void SoundSpark()
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() ) // client side
		{
			int random_index = Math.RandomInt(0, SOUNDS_SPARK_COUNT);
			string sound_type = m_SoundsSpark[random_index];
			PlaySound(sound_type, 50);
		}
	}

	// Plays sound
	void SoundBuzzLoopStart()
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() ) // client side
		{
			if (!m_BuzzSoundLoop)
			{
				m_BuzzSoundLoop = PlaySoundLoop(m_SoundBuzzLoop, 50);
			}
		}
	}

	// Stops sound
	void SoundBuzzLoopStop()
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() ) // client side
		{
			if (m_BuzzSoundLoop)
			{
				GetGame().ObjectDelete(m_BuzzSoundLoop);
				m_BuzzSoundLoop = NULL;
			}
		}
	}
	
	// Plays an electric shock sound
	void SoundElectricShock()
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() ) // client side
		{
			int random_index = Math.RandomInt(0, SOUNDS_SHOCK_COUNT);
			string sound_type = m_SoundsShock[random_index];
			PlaySound(sound_type, 50);
		}
	}
	
	// Plays a collision sound
	void SoundCollision()
	{
		if ( !GetGame().IsServer()  ||  !GetGame().IsMultiplayer() ) // client side
		{
			int random_index = Math.RandomInt(0, SOUNDS_COLLISION_COUNT);
			string sound_type = m_SoundsCollision[random_index];
			PlaySound(sound_type, 50);
		}
	}
	// ---------------------------------------------------------

	// Area Damage Pre/Post actions	
	// ---------------------------------------------------------
	override void PreAreaDamageActions()
	{
		if ( GetCompEM().IsPlugged() && GetCompEM().IsSwitchedOn() )
		{
			Spark();
			SoundElectricShock();
		}
		SoundCollision();
	}
	
	override void PostAreaDamageActions() 
	{
		//dmg to barbed wire here
		MiscGameplayFunctions.DealAbsoluteDmg(this, 1000);
	}
	// ---------------------------------------------------------	
	
	
	// TODO: proper handling can be done once the ticket DAYZ-26145 is resolved
	override void OnItemLocationChanged(EntityAI old_owner, EntityAI new_owner)
	{
		super.OnItemLocationChanged(old_owner, new_owner);

		if (m_TriggerActive)
		{
			DestroyDamageTrigger();
			m_IsPlaced = false;
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
			ShowAllSelections();
			HideSelection("zbytek");
	
			if (!GetHierarchyParent())
			{
				if (GetCompEM().IsPlugged() && GetCompEM().IsWorking() )
					{ CreateElectrifiedDamageTrigger(); }
				else
					{ CreateDamageTrigger(); }
				m_IsPlaced = true;
			}
			
			SetIsDeploySound( true );
		}	
	}
	
	override string GetDeploySoundset()
	{
		return "placeBarbedWire_SoundSet";
	}
	
	override string GetLoopDeploySoundset()
	{
		return "barbedwire_deploy_SoundSet";
	}
	
	override void SetActions()
	{
		super.SetActions();

		AddAction(ActionRestrainTarget);
		AddAction(ActionRestrainSelf);
	}	
}