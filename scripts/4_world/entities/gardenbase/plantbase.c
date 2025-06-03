enum EPlantState
{
	PAUSED = 0,
	GROWING,
	MATURE,
	SPOILED,
	DRY
}

class PlantBase extends ItemBase
{
	private float 	m_SprayUsage; // How much spray is needed to stop infestation of plant
	
	private float 	m_InfestationChance;
	
	private int 	m_GrowthStagesCount; 
	private int 	m_CropsCount;
	private bool 	m_HasCrops;
	private string 	m_CropsType; 
	private float 	m_PlantMaterialMultiplier; 
	
	private int 	m_PlantStateIndex;
	private float 	m_CurrentPlantMaterialQuantity;
	protected EPlantState m_PlantState;
	
	private bool 	m_IsInfested;
	private float 	m_SprayQuantity;
	bool 			m_MarkForDeletion = false;
	
	int 	m_DeleteDryPlantTime; 			// For how long in seconds can an unwatered plant exist before it disappears
	int 	m_SpoiledRemoveTime;			// For how long in seconds a spoiled plant will exist
	int 	m_FullMaturityTime;				// How much time needs plant to be full grown in seconds
	int 	m_SpoilAfterFullMaturityTime;	// How long in seconds it takes for plant to be spoiled after it is full grown
	float 	m_StateChangeTime;				// For how long in seconds will plant stay in one state before its going to next state
	
	protected ref Timer m_TimeTicker;
	protected float m_TimeTracker;
	
	private GardenBase m_GardenBase = NULL;
	private ref Slot m_Slot = NULL;
	
	private PluginHorticulture m_ModuleHorticulture;
	
	private const float SPOIL_AFTER_MATURITY_TIME = 14400.0; //The time it takes for a fully grown plant to spoil, in seconds (4 hours)
	private const int TICK_FREQUENCY = 1; // seconds
	
	// debug
	static int m_DebugFullMaturityTime;
	static int m_DebugSpoilTime;
	static int m_DebugSpoilRemoveTime;
	static int m_DebugDeleteDryTime;
	static float m_DebugTickSpeedMultiplier = 1;
		
	void PlantBase()
	{
		m_ModuleHorticulture = PluginHorticulture.Cast( GetPlugin( PluginHorticulture ) );
		
		m_SprayUsage = 5;
		m_DeleteDryPlantTime = (60 * 10) + Math.RandomInt(0, 60 * 2);
		m_SpoiledRemoveTime = (60 * 20) + Math.RandomInt(0, 60 * 5);
				
		string plant_type = this.GetType();
		m_GrowthStagesCount = GetGame().ConfigGetInt( "cfgVehicles " + plant_type + " Horticulture GrowthStagesCount" );
		m_CropsCount = GetGame().ConfigGetInt( "cfgVehicles " + plant_type + " Horticulture CropsCount" );
		GetGame().ConfigGetText( "cfgVehicles " + plant_type + " Horticulture CropsType", m_CropsType );
		
		if (m_GrowthStagesCount == 0)
			m_GrowthStagesCount = 1;
		
		m_InfestationChance = 0.2 / m_GrowthStagesCount; //Must be between 0 and 1

		m_PlantStateIndex = -1;
		m_CurrentPlantMaterialQuantity = 0;
		m_IsInfested = false;
		m_SprayQuantity = 0.0;
		m_HasCrops = true;
		
		SetTakeable( false );
		
		RegisterNetSyncVariableBool("m_HasCrops");
		RegisterNetSyncVariableInt("m_PlantState");
		RegisterNetSyncVariableInt("m_PlantStateIndex");
		
		if (GetGame().IsServer())
		{
			m_TimeTicker = new Timer( CALL_CATEGORY_SYSTEM );
			m_TimeTicker.Run(TICK_FREQUENCY, this, "Tick", NULL, true);
		}
	}

	void ~PlantBase()
	{
		if (m_TimeTicker)
			m_TimeTicker.Stop();
		
		if (!m_MarkForDeletion && !IsPendingDeletion())
		{
			DestroyPlant();
		}
	}
	
	void Init( GardenBase garden_base, float fertility, float harvesting_efficiency, float water )
	{
		m_GardenBase = garden_base;
		
		if (m_DebugFullMaturityTime != 0)
			m_FullMaturityTime = m_DebugFullMaturityTime;
		else
			m_FullMaturityTime += Math.RandomInt(-60,180);
		
		if (m_DebugSpoilTime != 0)
			m_SpoilAfterFullMaturityTime = m_DebugSpoilTime;
		else 
			m_SpoilAfterFullMaturityTime = SPOIL_AFTER_MATURITY_TIME;
		
		if (m_DebugSpoilRemoveTime != 0)
			m_SpoiledRemoveTime = m_DebugSpoilRemoveTime;
		
		if (m_DebugDeleteDryTime != 0)
			m_DeleteDryPlantTime = m_DebugDeleteDryTime;

		m_StateChangeTime = m_FullMaturityTime / (m_GrowthStagesCount - 2);

		float count = m_CropsCount * fertility * harvesting_efficiency;
		m_CropsCount = (int)Math.Ceil( count );

		m_PlantMaterialMultiplier = 0.1 * harvesting_efficiency;
		
		float rain_intensity = GetGame().GetWeather().GetRain().GetActual();
		
		if (m_PlantState < EPlantState.MATURE  &&  !NeedsWater())		
		{
			SetPlantState(EPlantState.GROWING);
			GrowthTimerTick(); // first tick happens straight away
		}
		
		if (rain_intensity <= 0.0)
		{		
			if (NeedsWater())
				SetPlantState(EPlantState.PAUSED);
		}
	}
	
	void Tick()
	{
		m_TimeTracker += m_TimeTicker.GetDuration() * m_DebugTickSpeedMultiplier;
		
		switch (m_PlantState)
		{
			case (EPlantState.GROWING):
				if (m_TimeTracker >= m_StateChangeTime)
					GrowthTimerTick();
					
				break;
						
			case (EPlantState.MATURE):
				if (m_TimeTracker >= m_SpoilAfterFullMaturityTime)
					SetSpoiled();
			
				break;
			
			case (EPlantState.SPOILED):
				if (m_TimeTracker >= m_SpoiledRemoveTime)
					RemoveSlot();
				
				break;
			
			case (EPlantState.DRY):
				if (m_TimeTracker >= m_DeleteDryPlantTime)
					RemoveSlot();
			
				break;
		}
		
	}
	
	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		if ( !super.OnStoreLoad( ctx, version ) )
			return false;
		
		GardenBase garden = GardenBase.Cast( GetHierarchyParent() );

		int slot_index = -1;
		if (!ctx.Read(slot_index))
			return false;
		
		Slot slot = garden.GetSlotByIndex(slot_index);

		SetSlot(slot);

		if ( !OnStoreLoadCustom( ctx, version ) )
			return false;

		return true;
	}

	override void OnStoreSave( ParamsWriteContext ctx )
	{
		super.OnStoreSave( ctx );
		
		Slot slot = GetSlot();
		
		if (slot)
		{
			int slot_index = slot.GetSlotIndex();
			slot.SetPlant(this); // hack
			
			ctx.Write( slot_index );
			
			OnStoreSaveCustom( ctx );
		}
		else
		{
			ErrorEx("[Warning] A plant existed without a garden. Therefore it was deleted from the world to prevent issues! Position: " + GetPosition(), ErrorExSeverity.INFO);
			GetGame().ObjectDelete(this); // Plants that exist without a garden must be deleted. Otherwise they might cause problems.
		}
	}
	
	string GetCropsType()
	{
		return m_CropsType;
	}

	bool OnStoreLoadCustom(ParamsReadContext ctx, int version)
	{
		if (!ctx.Read(m_SprayUsage))
			return false;
			
		if (!ctx.Read(m_DeleteDryPlantTime))
			return false;
			
		if (!ctx.Read(m_SpoiledRemoveTime))
			return false;
			
		if (!ctx.Read(m_FullMaturityTime))
			return false;
			
		if (!ctx.Read(m_SpoilAfterFullMaturityTime))
			return false;
			
		if (!ctx.Read(m_StateChangeTime))
			return false;
			
		if (!ctx.Read(m_InfestationChance))
			return false;
			
		if (!ctx.Read(m_GrowthStagesCount))
			return false;
			
		if (!ctx.Read(m_CropsCount))
			return false;
			
		if (!ctx.Read(m_CropsType))
			return false;
			
		if (!ctx.Read(m_PlantMaterialMultiplier))
			return false;
			
		if (!ctx.Read(m_PlantState))
			return false;
			
		if (!ctx.Read(m_PlantStateIndex))
			return false;
			
		if (!ctx.Read(m_CurrentPlantMaterialQuantity))
			return false;

		if (!ctx.Read(m_IsInfested))
			return false;
			
		if (!ctx.Read(m_SprayQuantity))
			return false;
		
		bool loadBool;	// deprec
		if (!ctx.Read(loadBool))
			return false;
		
		float loadFloat = 0.0;
		if (!ctx.Read(loadFloat))	// deprec
			return false;
		
		loadFloat = 0.0;
		if (ctx.Read(loadFloat))
		{
			if (loadFloat > 0.0)
				m_TimeTracker = loadFloat;	// spoil
		}
		else
		{
			return false;
		}
		
		loadFloat = 0.0;
		if (ctx.Read(loadFloat))
		{
			if (loadFloat > 0.0)
				m_TimeTracker = loadFloat; // spoil delete
		}
		else
		{
			return false;
		}
		
		loadFloat = 0.0;
		if (ctx.Read(loadFloat))
		{
			if ( loadFloat > 0.0 )
				m_TimeTracker = loadFloat;	// dry delete
		}
		else
		{
			return false;
		}
		
		if (m_StateChangeTime != (m_FullMaturityTime / (m_GrowthStagesCount - 2)))
		{
			m_StateChangeTime = m_FullMaturityTime / (m_GrowthStagesCount - 2);
			ErrorEx("[Warning] A plant loaded from storage had a corrupted state change time. Time was now adjusted! Position: " + GetPosition(), ErrorExSeverity.INFO);
		}
		
		if (version >= 142)
		{
			if (!ctx.Read(m_HasCrops))
				return false;
		}

		UpdatePlant();
		return true;
	}

	void OnStoreSaveCustom( ParamsWriteContext ctx )
	{
		ctx.Write(m_SprayUsage);
		ctx.Write(m_DeleteDryPlantTime);
		ctx.Write(m_SpoiledRemoveTime);
		ctx.Write(m_FullMaturityTime);
		ctx.Write(m_SpoilAfterFullMaturityTime);
		ctx.Write(m_StateChangeTime);
		ctx.Write(m_InfestationChance);
		ctx.Write(m_GrowthStagesCount);
		ctx.Write(m_CropsCount);
		ctx.Write(m_CropsType);
		ctx.Write(m_PlantMaterialMultiplier);
		ctx.Write(m_PlantState);
		ctx.Write(m_PlantStateIndex);
		ctx.Write(m_CurrentPlantMaterialQuantity);
		ctx.Write(m_IsInfested);
		ctx.Write(m_SprayQuantity);
		
		bool saveBool = false;	// deprec
		ctx.Write( saveBool );	
		
		float saveFloat = 0.0;	// deprec
		ctx.Write( saveFloat );
		
		saveFloat = 0.0;
		if (m_PlantState == EPlantState.MATURE)
		{
			saveFloat = m_TimeTracker;
		}
		ctx.Write( saveFloat );
		
		saveFloat = 0.0;
		if (m_PlantState == EPlantState.SPOILED)
		{
			saveFloat = m_TimeTracker;
		}
		ctx.Write( saveFloat );
		
		saveFloat = 0.0;
		if (m_PlantState == EPlantState.DRY)
		{
			saveFloat = m_TimeTracker;
		}
		ctx.Write( saveFloat );
		
		ctx.Write(m_HasCrops);
	}
	
	void PrintValues()
	{
		Print("PRINT ALL VALUES OF PLANT...");
		Print(this);
		
		Print(m_GrowthStagesCount);
		Print(m_HasCrops);
		Print(typename.EnumToString(EPlantState, m_PlantState));
		Print(m_PlantStateIndex);
		Print(m_CurrentPlantMaterialQuantity);
		Print(m_IsInfested);
		Print(m_SprayQuantity);
		Print(m_Slot);
		Print(m_GardenBase);
		Print(m_StateChangeTime);
		Print(m_FullMaturityTime);
		Print(m_SpoilAfterFullMaturityTime);
		Print(m_TimeTracker);
		Print("----------------------------------------------------------");
	}
	
	override bool CanPutInCargo( EntityAI parent )
	{
		return super.CanPutInCargo(parent);
	}

	override bool CanPutIntoHands( EntityAI parent )
	{
		return super.CanPutIntoHands(parent);
	}

	override bool CanRemoveFromHands( EntityAI parent )
	{
		return false;
	}

	void ChangeInfestation( bool is_infested )
	{
		m_IsInfested = is_infested;
		
		string plant_type = GetType();
		PlantMaterialHealth material = m_ModuleHorticulture.GetPlantMaterial( plant_type );
		
		if ( m_IsInfested )
		{
			if ( material.m_InfestedTex != "" )
			{
				SetObjectTexture( 0, material.m_InfestedTex );
			}
			if ( material.m_InfestedMat != "" )
			{
				SetObjectMaterial( 0, material.m_InfestedMat );
			}
		}
		else 
		{
			if ( material.m_HealthyTex != "" )
			{
				SetObjectTexture( 0, material.m_HealthyTex );
			}
			if ( material.m_HealthyMat != "" )
			{
				SetObjectMaterial( 0, material.m_HealthyMat );
			}
		}
	}

	void UpdatePlant()
	{
		if ( m_PlantStateIndex > 0 )
		{
			string plant_state_index = m_PlantStateIndex.ToStringLen(2); 
			string prev_plant_state_index = ( m_PlantStateIndex - 1 ).ToStringLen( 2 );
			
			// HIDING PREVIOUS PLANT STATE AND SHOWING THE CURRENT ONE
			ShowSelection( "plantStage_" + plant_state_index ); // SHOW!
			HideSelection( "plantStage_" + prev_plant_state_index ); // HIDE!
			
			// HIDING PREVIOUS CROPS STATE AND SHOWING THE CURRENT ONE
			
			if ( HasCrops() )
			{
				ShowSelection( "plantStage_" + plant_state_index + "_crops" ); // SHOW!
				HideSelection( "plantStage_" + prev_plant_state_index + "_crops" ); // HIDE!
			}
			else
			{
				HideSelection( "plantStage_" + plant_state_index + "_crops" ); // HIDE!
				HideSelection( "plantStage_" + prev_plant_state_index + "_crops" ); // HIDE!
			}

			// HIDING PREVIOUS SHADOW STATE AND SHOWING THE CURRENT ONE
			ShowSelection( "plantStage_" + plant_state_index + "_shadow" ); // SHOW!
			HideSelection( "plantStage_" + prev_plant_state_index + "_shadow" ); // HIDE!
		}
		
		float float_plant_state_index = (float)m_PlantStateIndex;
		m_CurrentPlantMaterialQuantity = m_PlantMaterialMultiplier * float_plant_state_index;
	}

	void GrowthTimerTick()
	{
		m_TimeTracker = 0;
		
		if ( m_PlantStateIndex < m_GrowthStagesCount - 2 )
		{
			m_PlantStateIndex++;
			UpdatePlant();
			SetSynchDirty();
			
			float infestation_rnd = Math.RandomFloat01();
			if ( m_InfestationChance > infestation_rnd )
				ChangeInfestation(true);
				
			if ( m_PlantStateIndex == m_GrowthStagesCount - 2 )
			{
				if (m_IsInfested)
					SetDry();
				else
					SetPlantState(EPlantState.MATURE);
			}
		}
	}
	
	void SetSpoiled()
	{
		if (m_PlantState != EPlantState.SPOILED)
		{
			m_PlantStateIndex++;
			UpdatePlant();
			SetPlantState(EPlantState.SPOILED);
		}
	}
	
	void SetDry()
	{
		if (m_PlantState != EPlantState.DRY)
		{
			m_PlantStateIndex++;
			UpdatePlant();
			SetPlantState(EPlantState.DRY);
		}
	}
	
	//NEW METHOD FOR PLANT SPRAYING
	void SprayPlant( float consumed_quantity )
	{
		//Rework this to have something smooth
		m_SprayQuantity += consumed_quantity;
		
		if (m_SprayQuantity >= m_SprayUsage)
		{			
			m_IsInfested = false;
			m_InfestationChance = 0;
			
			ChangeInfestation( false );
			UpdatePlant();
		}
	}

	void RemovePlantEx( vector pos )
	{
		if ( GetGame() && GetGame().IsServer() )
		{
			UnlockFromParent();
			
			if ( m_CurrentPlantMaterialQuantity > 0.0 )
			{
				ItemBase item = ItemBase.Cast( GetGame().CreateObjectEx( "PlantMaterial", pos, ECE_PLACE_ON_SURFACE ) );
				item.SetQuantity( m_CurrentPlantMaterialQuantity * 1000.0 );
			}
			
			RemoveSlot();
		}
	}
	
	void DestroyPlant()
	{
		if ( GetGame() && GetGame().IsServer() )
		{
			UnlockFromParent();
			
			RemoveSlot();
		}
	}

	void Harvest( PlayerBase player )
	{		
		if (IsHarvestable())
		{
			for ( int i = 0; i < m_CropsCount; i++ )
			{
				vector pos = player.GetPosition();
				ItemBase item = ItemBase.Cast( GetGame().CreateObjectEx( m_CropsType, pos, ECE_PLACE_ON_SURFACE ) );
				item.SetQuantity( item.GetQuantityMax() );
			}
		}
		
		m_HasCrops = false;

		SetSynchDirty();

		UpdatePlant();
		m_GardenBase.SyncSlots();
	}
	
	void SetPlantState(int state)
	{
		m_PlantState = state;
		m_TimeTracker = 0;
		SetSynchDirty();
	}
	
	EPlantState GetPlantState()
	{
		return m_PlantState;
	}
	
	int GetPlantStateIndex()
	{
		return m_PlantStateIndex;
	}
	
	float GetWater()
	{
		if ( GetSlot() )
			return GetSlot().GetWater();

		return 0;
	}
	
	float GetWaterMax()
	{
		if ( GetSlot() )
			return GetSlot().GetWaterUsage();

		return 0;
	}
	
	bool NeedsWater()
	{
		Slot slotPlant = m_Slot;
		
		if ( m_PlantState == EPlantState.PAUSED && slotPlant && slotPlant.GetWater() < slotPlant.GetWaterUsage() )
			return true;
		
		return false;
	}

	float GetSprayQuantity()
	{
		return m_SprayQuantity;
	}	

	float GetSprayUsage()
	{
		return m_SprayUsage;
	}	
	
	void RemoveSlot()
	{
		GardenBase garden = GardenBase.Cast( GetHierarchyParent() );
		
		if ( garden )		
			garden.RemoveSlotPlant( this );
	}
	
	void SetSlot(Slot slot)
	{
		if ( slot )
		{
			m_Slot = slot;
		}
	}
	
	Slot GetSlot()
	{
		return m_Slot;
	}
	
	void SetGarden(GardenBase gardenBase)
	{
		m_GardenBase = gardenBase;
	}
	
	GardenBase GetGarden()
	{
		return m_GardenBase;
	}

	bool IsSprayable()
	{
		if (m_PlantState == EPlantState.GROWING && m_SprayQuantity < m_SprayUsage)
			return true;
		
		return false;
	}
	
	bool IsHarvestable()
	{
		if (m_PlantState == EPlantState.MATURE && m_HasCrops)
			return true;
		
		return false;
	}
		
	bool HasCrops()
	{
		return m_HasCrops;
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionHarvestCrops);
		AddAction(ActionRemovePlant);
	}
		
	void DebugSetTimes(int maturity, int spoil, int spoilRemove, int dryDelete)
	{
		if (maturity != 0)
		{
			m_FullMaturityTime = maturity;
			m_StateChangeTime = m_FullMaturityTime / (m_GrowthStagesCount - 2);
		}
		
		if (spoil != 0)
			m_SpoilAfterFullMaturityTime = spoil;
		
		if (spoilRemove != 0)
			m_SpoiledRemoveTime = spoilRemove;
		
		if (dryDelete != 0)
			m_DeleteDryPlantTime = dryDelete;
	}
	
	static void DebugSetGlobalTimes(int maturity, int spoil, int spoilRemove, int dryDelete)
	{
		m_DebugFullMaturityTime = maturity;
		m_DebugSpoilTime = spoil;
		m_DebugSpoilRemoveTime = spoilRemove;
		m_DebugDeleteDryTime = dryDelete;
	}
	
	static void DebugSetTickSpeedMultiplier(float multiplier)
	{
		m_DebugTickSpeedMultiplier = multiplier;
	}
	
	// DEPRECATED
	static const int STATE_DRY 		= 0;
	static const int STATE_GROWING 	= 1;
	static const int STATE_MATURE 	= 2;
	static const int STATE_SPOILED 	= 3;
	
	ref Timer m_SpoiledRemoveTimer;
	ref Timer m_DeleteDryPlantTimer;
	ref Timer m_SpoilAfterFullMaturityTimer;
	ref Timer m_GrowthTimer;
	ref Timer m_InfestationTimer;
	
	void DeleteDryPlantTick();
	void SpoiledRemoveTimerTick();
	void InfestationTimerTick();
	void CheckWater();
	bool IsMature();
	bool IsSpoiled();
	bool IsDry();
	bool IsGrowing();
	bool NeedsSpraying();
	void RemovePlant();
	string StopInfestation( float consumed_quantity )
	{
		return "";
	}
}