class FlammableBase : ItemBase
{
	void FlammableBase()
	{
		Init();
	}
	
	private SoundOnVehicle	m_LoopSoundEntity;
	Particle 				m_FireParticle;
	bool					m_CanReceiveUpgrade; // Synchronized variable
	bool					m_IsBeingDestructed = false;
	
	float 					m_BurnTimePerRagEx;
	float 					m_BurnTimePerFullLardEx;
	float 					m_BurnTimePerFullFuelDoseEx;
	float 					m_MaxConsumableLardQuantityEx;
	float 					m_MaxConsumableFuelQuantityEx;
	float 					m_WaterEvaporationByFireIntensityEx = 0.001;
	int 					m_StartFadeOutOfLightAtQuantityEx = 3;
	
	int 					m_RagsUpgradedCount;//how many rags were upgraded with fuel/lard
	bool					m_ConsumeRagFlipFlop;//are we burning rag or fuel/lard
	vector 					m_ParticleLocalPos = Vector(0, 0.50, 0);
	
	string					m_DecraftResult = "WoodenStick";
	TorchLight				m_Light;
	bool					m_WasLit;//was this item ever lit ? (used for correct material setting after reconnect for extinguished flammable items)
	
	protected ref UniversalTemperatureSource m_UTSource;
	protected ref UniversalTemperatureSourceSettings m_UTSSettings;
	protected ref UniversalTemperatureSourceLambdaConstant m_UTSLConstant;
	
	override void DeferredInit()
	{
		if(m_RagsUpgradedCount)
		{
			LockRags(true);
		}
	}
	
	void Init()
	{
		if ( m_BurnTimePerRagEx == 0 || m_BurnTimePerFullLardEx == 0 || m_MaxConsumableLardQuantityEx == 0 || m_MaxConsumableFuelQuantityEx == 0 )
		{
			string cfg_path = "CfgVehicles " + GetType();
			m_BurnTimePerRagEx = GetGame().ConfigGetFloat( cfg_path + " burnTimePerRag" );
			m_BurnTimePerFullLardEx = GetGame().ConfigGetFloat( cfg_path + " burnTimePerFullLardDose" );
			m_BurnTimePerFullFuelDoseEx = GetGame().ConfigGetFloat( cfg_path + " burnTimePerFullFuelDose" );
			m_MaxConsumableLardQuantityEx = GetGame().ConfigGetFloat( cfg_path + " maxConsumableLardDose" );
			m_MaxConsumableFuelQuantityEx = GetGame().ConfigGetFloat( cfg_path + " maxConsumableFuelDose" );
		}
		RegisterNetSyncVariableBool("m_CanReceiveUpgrade");
	}
	
	override void EEInit()
	{		
		super.EEInit();
		
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
 			m_UTSSettings 					= new UniversalTemperatureSourceSettings();
			m_UTSSettings.m_Updateable		= true;
			m_UTSSettings.m_UpdateInterval	= 1;
			m_UTSSettings.m_TemperatureMin	= 0;
			m_UTSSettings.m_TemperatureMax	= 300;
			m_UTSSettings.m_RangeFull		= 0.5;
			m_UTSSettings.m_RangeMax		= 1;
			m_UTSSettings.m_TemperatureCap	= 5;
			
			m_UTSLConstant					= new UniversalTemperatureSourceLambdaConstant();
			m_UTSource						= new UniversalTemperatureSource(this, m_UTSSettings, m_UTSLConstant);
		}

	}
	
	override vector GetUniversalTemperatureSourcePosition()
	{
		if (GetHierarchyRoot())
		{
			return GetHierarchyRoot().GetPosition();
		}

		return super.GetPosition();
	}
	
	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		if ( m_LoopSoundEntity != NULL && GetGame() != NULL )
		{
			GetGame().ObjectDelete( m_LoopSoundEntity );
		}
		
		StopAllParticles();
	}

	override bool CanReceiveAttachment(EntityAI attachment, int slotId)
	{
		ItemBase att = ItemBase.Cast(GetInventory().FindAttachment(slotId));
		if (att && att.IsFullQuantity())
			return false;
		
		return super.CanReceiveAttachment(attachment, slotId);
	}
	
	override bool CanPutInCargo( EntityAI parent )
	{
		if( !super.CanPutInCargo(parent) ) {return false;}
		return CanBeTakenAsCargo();
	}
	
	override bool CanReleaseAttachment(EntityAI attachment)
	{
		if( !super.CanReleaseAttachment( attachment ) )
			return false;
		return !GetCompEM().IsWorking();
	}
	
	override bool CanRemoveFromCargo(EntityAI parent)
	{
		return CanBeTakenAsCargo();
	}
	
	override bool CanPutAsAttachment (EntityAI parent)
	{
		return !GetCompEM().IsWorking();
	}
	
	bool CanBeTakenAsCargo()
	{
		// Don't let players burn their pockets!
		return !GetCompEM().IsWorking();
	}
	
	override bool IsIgnited()
	{
		return GetCompEM().IsWorking();
	}
	
	override bool CanIgniteItem(EntityAI ignite_target = NULL)
	{
		return GetCompEM().IsWorking();
	}
	
	override bool HasFlammableMaterial()
	{
		return true;
	}
	
	// Checkes if Torch can be ignited
	override bool CanBeIgnitedBy(EntityAI igniter = NULL)
	{
		if ( !GetCompEM().CheckWetness() )
			return false;
		
		ItemBase rag = GetRag();
		
		if (rag  &&  GetCompEM().GetEnergy() < GetCompEM().GetEnergyUsage() * GetCompEM().GetUpdateInterval() )
		{
			float wetness = rag.GetWet();
			bool is_dry_enough = wetness <= 1-GetCompEM().GetWetnessExposure();
			if (!is_dry_enough)
			{
				return false;
			}
		}
		
		if ( !GetCompEM().CanWork() )
			return false;
		
		if ( GetCompEM().GetEnergy() < 3 )
			return false;
		
		PlayerBase player = PlayerBase.Cast(GetHierarchyRootPlayer());
		if (player)
		{
			if (this != player.GetItemInHands())//we are in player's inventory, but not in his hands
			{
				return false;
			}
		}
		
		return true;
	}
	
	void UpdateCheckForReceivingUpgrade()
	{
		if ( GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
			m_CanReceiveUpgrade = GetRagQuantity() > 0 && m_RagsUpgradedCount < GetRagQuantity() || (m_BurnTimePerRagEx * (m_RagsUpgradedCount + GetRagQuantity()) - GetCompEM().GetEnergy()) > 1;
			SetSynchDirty();
		}
	}
	
	override void OnIgnitedThis(EntityAI fire_source)
	{
		if ( !GetCompEM().HasEnoughStoredEnergy() )
		{
			ConsumeRag();
		}
		
		GetCompEM().SwitchOn();
	}
	
	override void OnSwitchOn()
	{
		if (!GetCompEM().HasEnoughStoredEnergy())
		{
			GetCompEM().SwitchOff();
		}
		
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			m_UTSource.SetActive(true);
		}
	}
	
	override void OnSwitchOff()
	{
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			m_UTSource.SetActive(false);
		}
	}
	
	void SetTorchDecraftResult(string type)
	{
		m_DecraftResult = type; //not persistent for the moment
	}
	
	bool ConsumeRag()
	{
		ItemBase rag = GetRag();
		
		if (rag)
		{
			if (rag.GetQuantity() <= 1)
			{
				LockRags(false); // Unlock attachment slot before deletion. Otherwise it will get stuck locked and unusable.
			}
			
			rag.AddQuantity(-1);
			rag.SetHealth(0);
			//GetCompEM().AddEnergy( m_BurnTimePerRagEx );
			return true;
		}
		
		return false;
	}
	
	void ConsumeLard(Lard lard)
	{
		if (lard)
		{
			float lard_quant = lard.GetQuantity();
			
			float available_lard_quant = lard_quant;
			
			if ( available_lard_quant > m_MaxConsumableLardQuantityEx )
				available_lard_quant = m_MaxConsumableLardQuantityEx;
			
			float available_lard_coef = available_lard_quant / m_MaxConsumableLardQuantityEx;
			
			float add_energy = m_BurnTimePerFullLardEx * available_lard_coef;
			float add_energy_coef = 1;
			
			float energy_limit = GetCompEM().GetEnergyMax() - GetCompEM().GetEnergy();
			
			if (add_energy > energy_limit )
			{
				add_energy_coef = energy_limit / add_energy;
				add_energy = energy_limit;
				available_lard_quant = available_lard_quant * add_energy_coef;
			}
			
			GetCompEM().AddEnergy( add_energy );
			lard.AddQuantity(-available_lard_quant);
			
			CalculateQuantity();
			
			UpdateCheckForReceivingUpgrade();
		}
	}

	void Upgrade(ItemBase source)
	{
		if (!GetRag())
		{
			return;
		}
		RuinRags();
		LockRags(true);
		float torchCurrentEnergy = GetCompEM().GetEnergy();
		float sourceQuantity = 100000;//for upgrade from environment(gas pump)
		
		if (source)
		{
			sourceQuantity = source.GetQuantity();
		}
		
		float maxOverallCapacity = GetRagQuantity() * 2 * m_BurnTimePerRagEx;
		//float maxUpgradeCapacity = GetRagQuantity() * m_BurnTimePerRagEx;
		float currentlyUpgraded =  Math.Max(0, torchCurrentEnergy - (GetRagQuantity() * m_BurnTimePerRagEx));
		//float freeUpgradeCapacity = maxUpgradeCapacity - currentlyUpgraded;
		float freeUpgradeCapacity = maxOverallCapacity - torchCurrentEnergy;
		float upgradeQuantity = Math.Min(freeUpgradeCapacity, sourceQuantity);
		int upgradedRags = Math.Ceil((upgradeQuantity + currentlyUpgraded) / m_BurnTimePerRagEx);
		upgradedRags = Math.Min(GetRagQuantity(), upgradedRags);
		m_RagsUpgradedCount = upgradedRags;
		GetCompEM().AddEnergy(upgradeQuantity);
		m_ConsumeRagFlipFlop = 0;//consume fuel first
		if (source)
		{
			source.AddQuantity(-upgradeQuantity);
		}
		CalculateQuantity();
		UpdateCheckForReceivingUpgrade();
	}
	
	
	
	void ConsumeFuelFromBottle(ItemBase vessel)
	{
		if (vessel)
		{
			float vessel_quant = vessel.GetQuantity();
			
			float available_vessel_quant = vessel_quant;
			
			if ( available_vessel_quant > m_MaxConsumableFuelQuantityEx )
				available_vessel_quant = m_MaxConsumableFuelQuantityEx;
			
			float available_vessel_coef = available_vessel_quant / m_MaxConsumableFuelQuantityEx;
			
			float add_energy = m_BurnTimePerFullFuelDoseEx * available_vessel_coef;
			float add_energy_coef = 1;
			
			float energy_limit = GetCompEM().GetEnergyMax() - GetCompEM().GetEnergy();
			
			if (add_energy > energy_limit )
			{
				add_energy_coef = energy_limit / add_energy;
				add_energy = energy_limit;
				available_vessel_quant = available_vessel_quant * add_energy_coef;
			}
			
			GetCompEM().AddEnergy( add_energy );
			vessel.AddQuantity(-available_vessel_quant);
			
			CalculateQuantity();
			
			UpdateCheckForReceivingUpgrade();
		}
	}
	
	void ConsumeFuelFromGasStation()
	{
		float add_energy = m_BurnTimePerFullFuelDoseEx;
		float add_energy_coef = 1;
		
		float energy_limit = GetCompEM().GetEnergyMax() - GetCompEM().GetEnergy();
		
		if (add_energy > energy_limit )
		{
			add_energy_coef = energy_limit / add_energy;
			add_energy = energy_limit;
		}
		
		GetCompEM().AddEnergy( add_energy );
		CalculateQuantity();
		UpdateCheckForReceivingUpgrade();
	}
	
	void RuinRags()
	{
		ItemBase rag = GetRag();
		
		if (rag)
		{
			rag.SetHealth(1); //does not actually ruin rags, combining would be impossible
		}
	}
	
	// Inventory manipulation
	override void OnInventoryExit(Man player)
	{
		super.OnInventoryExit(player);
		
		StandUp();
	}
	
	// Stands up the torch, if possible. Returns true on success.
	bool StandUp()
	{
		string surface_type;
		vector position = GetPosition();
		GetGame().SurfaceGetType ( position[0], position[2], surface_type );
		bool is_surface_soft = GetGame().IsSurfaceDigable(surface_type);
		
		if ( is_surface_soft && !IsRuined() )
		{
			vector ori_rotate = "0 0 0";
			ori_rotate[0] = Math.RandomFloat(0, 360);
			ori_rotate[1] = Math.RandomFloat(0, 10);
			SetOrientation(ori_rotate);
			
			return true; // I am standing up
		}
		
		return false; // I am NOT standing up
	}
	
	void CalculateQuantity()
	{
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			SetQuantityNormalized(GetCompEM().GetEnergy0To1());
		}
	}

	bool CanReceiveUpgrade()
	{
		return m_CanReceiveUpgrade;
	}
	
	void CraftingInit(float quantity)
	{
		GetCompEM().SetEnergy(m_BurnTimePerRagEx * quantity);
		m_CanReceiveUpgrade = true;
		SetSynchDirty();
	}
	
	
	override void EEItemAttached( EntityAI item, string slot_name ) 
	{
		super.EEItemAttached( item, slot_name );
		CalculateQuantity();
		UpdateCheckForReceivingUpgrade();
	}
	
	
	override void EEItemDetached( EntityAI item, string slot_name ) 
	{
		super.EEItemDetached( item, slot_name );

		if (m_IsBeingDestructed)
		{
			if (GetGame().IsServer() || !GetGame().IsMultiplayer())
			{
				EntityAI rags = EntityAI.Cast(GetGame().CreateObjectEx(item.GetType(), GetPosition(), ECE_PLACE_ON_SURFACE));
				if( rags )
					MiscGameplayFunctions.TransferItemProperties(item, rags);
			}
			return;
		}
		
		CalculateQuantity();
		UpdateCheckForReceivingUpgrade();
		GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( TryTransformIntoStick, 100);
	}
	
	bool CanTransformIntoStick()
	{
		if ((GetGame().IsServer() || !GetGame().IsMultiplayer()) && !IsIgnited() && !GetRag() && !IsSetForDeletion() )
			return true;
		else
			return false;
	}
	
	void TryTransformIntoStick()
	{
		PlayerBase player = PlayerBase.Cast(GetHierarchyRootPlayer());
		if ( m_IsBeingDestructed || (player && player.IsPlayerDisconnected()) )
			return;
				
		if ( CanTransformIntoStick() )
		{
			m_IsBeingDestructed = true;
			if (player)
			{
				// Transform object into wooden stick
				StopAllParticles();
				
				TorchLambda lambda = new TorchLambda(this, m_DecraftResult);
				player.ServerReplaceItemInHandsWithNew(lambda);
			}
			else
			{
				// Create wooden stick and delete Torch
				vector pos = GetPosition();
				vector ori = GetOrientation();
				
				if ( GetType() == "WoodenStick" )
					ori = ori + Vector(0,90,0);
				
				ItemBase stick = ItemBase.Cast( GetGame().CreateObjectEx(m_DecraftResult, pos, ECE_PLACE_ON_SURFACE) );
				ApplyResultModifications(stick);
				stick.SetPosition(pos);
				stick.PlaceOnSurface();
				
				if ( stick.GetType() == "LongWoodenStick" )
				{
					pos = stick.GetPosition() + Vector(0,-0.3,0);
					stick.SetPosition(pos);
				}
				
				stick.SetOrientation(ori);
				GetGame().ObjectDelete(this);
			}
		}
	}
	
	
	override void OnWorkStart()
	{
		m_WasLit = true;
		LockRags(true);
		UpdateMaterial();
	}
	
	void StopAllParticles()
	{
		if (m_FireParticle)
		{
			m_FireParticle.Stop();
		}
	}
	
	Rag GetRag()
	{
		return Rag.Cast( GetAttachmentByType(Rag) );
	}
	
	void LockRags(bool do_lock)
	{
		ItemBase rag = GetRag();
		if (rag)
		{
			if (do_lock)
			{
				rag.LockToParent();
			}
			else if (!m_RagsUpgradedCount)
			{
				rag.UnlockFromParent();
			}
		}
	}
	
	void UpdateLight()
	{
		if (!m_Light)
		{
			m_Light = TorchLight.Cast( ScriptedLightBase.CreateLight( TorchLight, Vector(0,0,0), 1 ) );
			m_Light.AttachOnObject(this, m_ParticleLocalPos + Vector (0,0.2,0));
		}
		
		if (m_FireParticle)
		{
			Object direct_particle = m_FireParticle.GetDirectParticleEffect();
			
			if (direct_particle  &&  direct_particle != m_Light.GetAttachmentParent())
			{
				m_Light.AttachOnObject(direct_particle, Vector(0,0.2,0));
			}	
		}
		
		float update_interval = GetCompEM().GetUpdateInterval();
		
		if (GetQuantity() <= m_StartFadeOutOfLightAtQuantityEx)
		{
			float brightness_coef = GetQuantity() / m_StartFadeOutOfLightAtQuantityEx;
			float radius_coef = GetQuantity() / m_StartFadeOutOfLightAtQuantityEx;
			
			if (radius_coef < m_StartFadeOutOfLightAtQuantityEx/10)
				radius_coef = m_StartFadeOutOfLightAtQuantityEx/10;
			
			if (brightness_coef < m_StartFadeOutOfLightAtQuantityEx/10)
				brightness_coef = m_StartFadeOutOfLightAtQuantityEx/10;
			
			m_Light.FadeBrightnessTo(m_Light.m_TorchBrightness * brightness_coef, update_interval);
			m_Light.FadeRadiusTo(m_Light.m_TorchRadius * radius_coef, update_interval);
		}
		else
		{
			m_Light.FadeBrightnessTo(m_Light.m_TorchBrightness, update_interval);
			m_Light.FadeRadiusTo(m_Light.m_TorchRadius, update_interval);
		}
	}

	override void OnItemInHandsPlayerSwimStart(PlayerBase player)
	{
		GetCompEM().SwitchOff();
	}
	
	
	override void OnWork( float consumed_energy )
	{
		UpdateMaterial();
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			if (GetCompEM().GetEnergy() < ((GetRagQuantity() + m_RagsUpgradedCount) - 1) * m_BurnTimePerRagEx)
			{
				if (m_RagsUpgradedCount==0)//always burn rag
				{
					ConsumeRag();
				}
				else if (m_ConsumeRagFlipFlop)//burn rag
				{
					ConsumeRag();
					m_ConsumeRagFlipFlop = !m_ConsumeRagFlipFlop;
				}
				else//burn lard/fuel
				{
					m_RagsUpgradedCount--;
					m_ConsumeRagFlipFlop = !m_ConsumeRagFlipFlop;
				}
			}
			if (GetRag() && GetCompEM().GetEnergy() == 0 && GetRagQuantity() > 0)
			{
				GetRag().SetQuantity(0);
			}
			RuinRags();
			
			CalculateQuantity();
			
			UpdateCheckForReceivingUpgrade();
			
			AddWet( -m_WaterEvaporationByFireIntensityEx * GetCompEM().GetUpdateInterval() );
			
			Rag rag = GetRag();
			
			if ( rag )
			{
				rag.AddWet( -m_WaterEvaporationByFireIntensityEx * GetCompEM().GetUpdateInterval() );
			}
		}
		
		if ( !m_LoopSoundEntity && GetGame() && ( !GetGame().IsDedicatedServer() ) )
		{
			m_LoopSoundEntity = PlaySoundLoop(GetSoundName(), 50);
		}
		
		// Effect scaling by fuel
		if ( !GetGame().IsDedicatedServer() )
		{
			UpdateLight();
			UpdateParticle();
		}
	}
	
	
	string GetSoundName()
	{
		return "torchLoop";
	}
	
	void UpdateParticle()
	{
		if ( GetQuantity() < 40 )
		{
			if (!m_FireParticle)
				m_FireParticle = ParticleManager.GetInstance().PlayOnObject(ParticleList.TORCH_T1, this, m_ParticleLocalPos);
			
			float scale = GetQuantity() / 40;
			
			if (scale > 1)
				scale = 1;
			
			if (scale < 0.25)
				scale = 0.25;
			
			m_FireParticle.ScaleParticleParamFromOriginal(EmitorParam.SIZE, scale);
			m_FireParticle.ScaleParticleParamFromOriginal(EmitorParam.VELOCITY, scale);
			m_FireParticle.ScaleParticleParamFromOriginal(EmitorParam.VELOCITY_RND, scale);
		}
		else
		{
			if ( !m_FireParticle  ||  m_FireParticle.GetParticleID() != ParticleList.TORCH_T2 )
			{
				// Executes once when fire particle starts or changes
				
				if (m_FireParticle)
					m_FireParticle.Stop();
				
				m_FireParticle = ParticleManager.GetInstance().PlayOnObject(ParticleList.TORCH_T2, this, m_ParticleLocalPos);
			}
		}
	
	}
	
	override void OnWorkStop()
	{
		UpdateMaterial();
		if (m_Light)
			m_Light.FadeOut();
		
		if ( m_LoopSoundEntity && GetGame() && ( !GetGame().IsDedicatedServer() ) )
		{
			GetGame().ObjectDelete( m_LoopSoundEntity );
			m_LoopSoundEntity = NULL;
		}
		
		if ( m_FireParticle)
		{
			m_FireParticle.Stop();
			m_FireParticle = NULL;
		}
		
		CalculateQuantity();
		UpdateCheckForReceivingUpgrade();
		
		LockRags(false);
		
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			if (GetRag() && GetCompEM().GetEnergy() == 0 && GetRagQuantity() > 0)
			{
				GetRag().SetQuantity(0);
			}
		}
		
		TryTransformIntoStick();
	}
	
	// COMBAT
	// This needs refactor!
	override int GetMeleeMode()
	{
		if ( GetCompEM().IsWorking() )
			return 3; // ???
		else
			return 0; // ???
	}

	override int GetMeleeHeavyMode()
	{
		if ( GetCompEM().IsWorking() )
			return 4; // ???
		else
			return 1; // ???
	}
	
	override int GetMeleeSprintMode()
	{
		if ( GetCompEM().IsWorking() )
			return 5; // ???
		else
			return 2; // ???
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionLightItemOnFire);
		AddAction(ActionUpgradeTorchFromGasPump);
	}
	
	override void OnAttachmentQuantityChangedEx(ItemBase item, float delta)
	{
		super.OnAttachmentQuantityChangedEx(item, delta);
		if (delta > 0)
		{
			GetCompEM().AddEnergy(m_BurnTimePerRagEx * delta);
			CalculateQuantity();
			UpdateCheckForReceivingUpgrade();
		}
	}
		
	
	override bool DisassembleOnLastDetach()
	{
		return true;
	}
	
	override void OnDebugSpawn()
	{
		GetInventory().CreateAttachment("Rag");
		CraftingInit(GetRagQuantity());
		CalculateQuantity();
	}
	
	int GetRagQuantity()
	{
		if (GetRag())
		{
			return Math.Round(GetRag().GetQuantity());
		}
		return 0;
	}
	
	string GetBurningMaterial()
	{
		return "";
	}
	
	string GetBurntMaterial()
	{
		return "";
	}
	
	void UpdateMaterial()
	{
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			if (GetCompEM().IsWorking())
			{
				if (GetBurningMaterial())
				{
					SetObjectMaterial(0, GetBurningMaterial());
				}
			}
			else if (m_WasLit)
			{
				if (GetBurntMaterial())
				{
					SetObjectMaterial(0, GetBurntMaterial());
				}
			}
		}
	}
	
	
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		ctx.Write(m_WasLit);
	}
	
	
	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		if (!super.OnStoreLoad(ctx, version))
		{
			return false;
		}
		if (version >= 130)
		{
			if (!ctx.Read( m_WasLit ))
			{
				return false;
			}
		}
		UpdateMaterial();
		return true;
	}
	
	void ApplyResultModifications(ItemBase result)
	{
		result.SetQuantity(1);
	}
	
	//-------------
	// DEBUG BELLOW
	//-------------
	
	#ifdef DEVELOPER
	override void GetDebugButtonNames(out string button1, out string button2, out string button3, out string button4)
	{
		button1 = "Ignite";
	}
	
	override void OnDebugButtonPressServer(int button_index)
	{
		if (button_index == 1)
		{
			OnIgnitedThis(null);
		}
	}

	override string GetDebugText()
	{
		string debug_output;
		
		if( GetGame().IsDedicatedServer())
		{
			debug_output+="m_RagsUpgradedCount:"+m_RagsUpgradedCount+"\n";
			debug_output+="m_ConsumeRagFlipFlop:"+m_ConsumeRagFlipFlop+"\n";
			if (IsIgnited() && m_ConsumeRagFlipFlop)
			{
				debug_output+="Burning rag \n";
			}
			else if (IsIgnited())
			{
				debug_output+="Burning lard/fuel \n";
			}
		}
		else
		{
			
		}
		return debug_output;
	}
	#endif
}


class Torch : FlammableBase
{
	//legacy vars which cannot be moved/removed
	static float 			m_BurnTimePerRag;
	static float 			m_BurnTimePerFullLard;
	static float 			m_BurnTimePerFullFuelDose;
	static float 			m_MaxConsumableLardQuantity;
	static float 			m_MaxConsumableFuelQuantity;
	static float 			m_WaterEvaporationByFireIntensity = 0.001;
	static int 				m_StartFadeOutOfLightAtQuantity = 3;
	

	override void Init()
	{
		super.Init();
		
		//for legacy reasons
		m_BurnTimePerRag = m_BurnTimePerRagEx;
		m_BurnTimePerFullLard = m_BurnTimePerFullLardEx;
		m_BurnTimePerFullFuelDose = m_BurnTimePerFullFuelDoseEx;
		m_MaxConsumableLardQuantity = m_MaxConsumableLardQuantityEx;
		m_MaxConsumableFuelQuantity = m_MaxConsumableFuelQuantityEx;
	}
	
	// !Called on CHILD when it's attached to parent.
	override void OnWasAttached( EntityAI parent, int slot_id )
	{
		super.OnWasAttached(parent, slot_id);
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
			LockRags(true);
	}
		
	// !Called on CHILD when it's detached from parent.
	override void OnWasDetached( EntityAI parent, int slot_id )
	{
		super.OnWasDetached(parent, slot_id);
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
			LockRags(false);
	}
	
	
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		ctx.Write(m_ConsumeRagFlipFlop);
		ctx.Write(m_RagsUpgradedCount);
	}
	
	
	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		if (!super.OnStoreLoad(ctx, version))
		{
			return false;
		}
		
		if (version >= 129)
		{
			if (!ctx.Read( m_ConsumeRagFlipFlop ))
			{
				return false;
			}
		
			if (!ctx.Read( m_RagsUpgradedCount ))
			{
				return false;
			}
		}
		UpdateCheckForReceivingUpgrade();
		return true;
	}
};

class TorchLambda : ReplaceItemWithNewLambdaBase
{
	override void CopyOldPropertiesToNew (notnull EntityAI old_item, EntityAI new_item)
	{
		super.CopyOldPropertiesToNew(old_item, new_item);

		ItemBase stick;
		FlammableBase flammable = FlammableBase.Cast(old_item);
		Class.CastTo(stick, new_item);
		if (stick && flammable)
		{
			flammable.ApplyResultModifications(stick);
		}
	}
};