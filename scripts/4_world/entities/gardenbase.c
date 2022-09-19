class GardenBase extends ItemBase //BuildingSuper
{
	// Paths to slot textures. Slots can have multiple states, so multiple textures must be generated
	static const string SLOT_TEXTURE_DIGGED_WET_LIME		= "dz\\gear\\cultivation\\data\\soil_digged_wet_lime_CO.paa";
	static const string SLOT_TEXTURE_DIGGED_WET_PLANT		= "dz\\gear\\cultivation\\data\\soil_digged_wet_plant_CO.paa";
	
	// Wet/dry material
	static const string SLOT_MATERIAL_WET					= "dz\\gear\\cultivation\\data\\soil_cultivated_wet.rvmat";
	static const string SLOT_MATERIAL_DRY					= "dz\\gear\\cultivation\\data\\soil_cultivated.rvmat";
	
	static const string SLOT_MATERIAL_LIMED_WET				= "dz\\gear\\cultivation\\data\\soil_cultivated_limed_wet.rvmat";
	static const string SLOT_MATERIAL_LIMED_DRY				= "dz\\gear\\cultivation\\data\\soil_cultivated_limed.rvmat";
	static const string SLOT_MATERIAL_COMPOST_WET			= "dz\\gear\\cultivation\\data\\soil_cultivated_compost_wet.rvmat";
	static const string SLOT_MATERIAL_COMPOST_DRY			= "dz\\gear\\cultivation\\data\\soil_cultivated_compost.rvmat";
	
	// slot names -> MUST BE LOWERCASE
	private static const string SLOT_SELECTION_DIGGED_PREFIX 	= "seedbase_";
	private static const string SLOT_SELECTION_COVERED_PREFIX 	= "slotCovered_";
	private static const string SLOT_MEMORY_POINT_PREFIX 		= "slot_";
	private static const string SLOT_SEEDBASE_PREFIX 			= "seedbase_";
	
	
	private static const int 	CHECK_RAIN_INTERVAL 			= 15;
	
	protected ref array<ref Slot> m_Slots;
	protected int 	m_SlotFertilityState = 0; 	//Used to store fertility state of all slots
	protected int 	m_SlotWateredState = 0; 	//Used to store fertility state of all slots
	protected int 	m_MaxWateredStateVal = 0; 	//Used to store fertility state of all slots
	protected float m_DefaultFertility = 1;
	ref Timer 		m_CheckRainTimer;
	
	private static ref map<string,string> m_map_slots; // For the 'attachment slot -> plant slot' conversion. It is possible that this will be removed later.
	
	void GardenBase()
	{		
		RegisterNetSyncVariableInt("m_SlotFertilityState");
		RegisterNetSyncVariableInt("m_SlotWateredState");
		
		m_map_slots = new map<string,string>;
		
		SetEventMask(EntityEvent.INIT); // Enable EOnInit event
		
		// Prepare m_map_slots
		for (int i = 1;  i <= GetGardenSlotsCount() ;  ++i)
		{
			// m_map_slots is supposed to be: <input, output>
			string input = SLOT_SEEDBASE_PREFIX + i.ToString();
			string output = SLOT_MEMORY_POINT_PREFIX;
			
			if (i < 10)
				output = output + "0"; // Example: '1' changes to '01'
			
			output = output + i.ToString();
			
			m_map_slots.Set(input, output);
		}
		
		if ( GetGame().IsServer() )
		{
			CheckRainStart();
		}
		
		InitializeSlots();
		
		SetMaxWaterStateVal();
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		SyncSlots();
	}
	
	override bool HasProxyParts()
	{
		return true;
	}
	
	override int GetHideIconMask()
	{
		return EInventoryIconVisibility.HIDE_VICINITY;
	}
	
	void SetBaseFertility(float value)
	{
		m_DefaultFertility = value;
	}
	
	float GetBaseFertility()
	{
		return m_DefaultFertility;
	}
	
	override void EOnInit(IEntity other, int extra)
	{
		CheckRainTick();
		UpdateTexturesOnAllSlots();
	}

	void InitializeSlots()
	{
		m_Slots = new array<ref Slot>;
		int slots_count = GetGardenSlotsCount();
		
		for ( int i = 0; i < slots_count; i++ )
		{
			Slot slot = new Slot(GetBaseFertility());
			slot.SetSlotIndex(i);
			int i1 = i + 1;
			string name = "SeedBase_" + i1;
			int slot_id = InventorySlots.GetSlotIdFromString(name);
			slot.SetSlotId(slot_id);
			slot.SetGarden(this);
			slot.m_State = Slot.STATE_DIGGED;
			m_Slots.Insert( slot );
		}
	}
	
	void SetMaxWaterStateVal()
	{
		int state = 0;
		
		for ( int i = 0; i < m_Slots.Count(); i++ )
		{
			state += 1 * Math.Pow( 2, i );
		}
		
		m_MaxWateredStateVal = state;
	}
	
	int GetMaxWaterStateVal()
	{
		return m_MaxWateredStateVal;
	}
	
	void UpdateTexturesOnAllSlots()
	{
		int slots_count = GetGardenSlotsCount();
		
		for ( int i = 0; i < slots_count; i++ )
		{
			UpdateSlotTexture(i);
		}
	}

	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		if ( version <= 118 )
			return true;
		
		if ( !super.OnStoreLoad( ctx, version ) )
			return false;
		
		if ( version < 102 )
		{
			float some_value;
			ctx.Read( some_value ); // compatibility check
		}
		
		int slots_count = GetGardenSlotsCount();
		
		for ( int i = 0; i < slots_count; i++ )
		{
			Slot slot = m_Slots.Get( i );

			if ( !slot.OnStoreLoadCustom( ctx, version ) )
				return false;

			//Slot textures will be updated after store load
			//UpdateSlotTexture( i );
		}
		
		if ( version >= 119 )
			ctx.Read( m_SlotFertilityState );
		
		if ( version >= 120 )
			ctx.Read( m_SlotWateredState );
		
		return true;
	}
	
	override void AfterStoreLoad()
	{
		super.AfterStoreLoad();
	}
	
	override void EEOnAfterLoad()
	{
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( SyncSlots, 500, false, this );
		super.EEOnAfterLoad();
	}
	
	void SyncSlots()
	{
		for ( int i = 0; i < GetGardenSlotsCount(); i++ )
		{
			// Read relevant bit
			int fertilityState = (m_SlotFertilityState >> i) & 1;
			m_Slots[i].SetFertilityState(fertilityState);
			
			int wateredState = (m_SlotWateredState >> i) & 1;
			m_Slots[i].SetWateredState( wateredState );
			
			if ( fertilityState == eFertlityState.NONE )
			{
				m_Slots[i].SetFertilityType( "" );
				m_Slots[i].SetFertilizerQuantity( 0 );
			}
			
			if ( wateredState == eWateredState.DRY )
			{
				m_Slots[i].SetWater( 0 );
			}
			
			UpdateSlotTexture( i );
		}
		
		if ( GetGame().IsServer() )
		{
			SetSynchDirty();
		}
	}

	override void OnStoreSave( ParamsWriteContext ctx )
	{
		super.OnStoreSave( ctx );
		
		int slots_count = GetGardenSlotsCount();
		
		for ( int i = 0; i < slots_count; i++ )
		{
			Slot slot = m_Slots.Get( i );
			
			slot.OnStoreSaveCustom( ctx );
		}
		
		ctx.Write(m_SlotFertilityState);
		
		ctx.Write( m_SlotWateredState );
	}
	
	void PrintSlots()
	{
		Debug.Log("PRINT ALL SLOTS FROM...");
		Debug.Log("" + this);
		int slots = GetInventory().GetAttachmentSlotsCount();
		Debug.Log("Nb Slots : " + slots);
		
		for ( int i = 0; i < slots ; i++ )
		{
			Slot slot = m_Slots.Get(i);
			Debug.Log("i : " + i);
			Debug.Log("Slot : " + slot);

			float slot_fertility = slot.GetFertility();
			float slot_fertility_usage = slot.GetFertilityMax();
			string slot_fertility_type = slot.GetFertilityType();
			float slot_water = slot.GetWater();
			float slot_water_usage = slot.GetWaterUsage();
			ItemBase slot_seed = slot.GetSeed();
			ItemBase slot_plant = slot.GetPlant();
			float slot_state= slot.GetState();
			float slot_slot_Index = slot.GetSlotIndex();
			float slot_slot_ID = slot.GetSlotId();
			int   slot_wateredState = slot.GetWateredState();
			int	  slot_FertilityState = slot.GetFertilityState();
			
			Debug.Log("Fertility : " + slot_fertility);
			Debug.Log("Fertility Usage : " + slot_fertility_usage);
			Debug.Log("Fertility Type : " + slot_fertility_type);
			Debug.Log("Fertility State : " + slot_FertilityState);
			Debug.Log("Water : " + slot_water);
			Debug.Log("Water Usage : " + slot_water_usage);
			Debug.Log("Watered State : " + slot_wateredState);
			Debug.Log("Seed : " + slot_seed);
			Debug.Log("Plant : " + slot_plant);
			Debug.Log("State : " + slot_state);
			Debug.Log("Slot Index : " + slot_slot_Index);
			Debug.Log("Slot ID : " + slot_slot_ID);
			Debug.Log("///////////////////////////");
		}
		
		Debug.Log("END OF ALL SLOTS FOR...");
		Debug.Log("" + this);
	}
	
	override bool CanPutInCargo( EntityAI parent )
	{
		if ( !super.CanPutInCargo(parent) ) {return false;}
		return false;
	}

	override bool CanPutIntoHands( EntityAI player )
	{
		if ( !super.CanPutIntoHands( parent ) )
		{
			return false;
		}
		return false;
	}

	override bool CanRemoveFromHands( EntityAI player )
	{
		return false;
	}

	int GetGardenSlotsCount()
	{
		return 0;
	}

	bool CanPlantSeed( string selection_component )
	{
		Slot slot = GetSlotBySelection( selection_component );
		
		if ( slot != NULL && slot.m_State == Slot.STATE_DIGGED )
		{
			return  true;
		}
		else
		{
			return false;
		}
	}
	
	// Converts attachment slot name into plant slot name. Example: 'seedbase_1' -> 'component02'
	string ConvertAttSlotToPlantSlot(string attach_slot)
	{
		// Give result
		if ( m_map_slots.Contains(attach_slot) )
		{
			string return_value = m_map_slots.Get(attach_slot);
			return return_value;
		}
		
		return "";
	}
	
	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);
		
		string path = string.Format("CfgVehicles %1 Horticulture PlantType", item.GetType());
		bool IsItemSeed = GetGame().ConfigIsExisting(path); // Is this item a seed?
		
		int slot_id = InventorySlots.GetSlotIdFromString(slot_name);
		
		if ( IsItemSeed ) 
		{
			string converted_slot_name;
			
			vector pos = GetPosition();
			int index = GetSlotIndexByAttachmentSlot( slot_name );
			
			if (index < 10)
			{
				converted_slot_name = SLOT_MEMORY_POINT_PREFIX + "0" + index.ToString();
			}
			else
			{
				converted_slot_name = SLOT_MEMORY_POINT_PREFIX + index.ToString();
			}
			
			PlantSeed( ItemBase.Cast( item ), converted_slot_name);
		}
		else if (g_Game.IsClient())
		{
			Slot slot = GetSlotByIndex(GetSlotIndexByAttachmentSlot( slot_name) - 1);
			if (slot)
			{
				slot.SetPlant(PlantBase.Cast( item ));
				slot.m_State = Slot.STATE_PLANTED;
			}
		}
	}
	
	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
		
		slot_name.ToLower();

		string path = string.Format("CfgVehicles %1 Horticulture PlantType", item.GetType());
		bool IsItemSeed = GetGame().ConfigIsExisting(path); // Is this item a seed?
		
		string converted_slot_name = ConvertAttSlotToPlantSlot(slot_name);
		Slot slot = GetSlotBySelection(converted_slot_name);
		
		if (slot)
		{
			if (IsItemSeed)
			{
				slot.SetSeed(NULL);				
			}
			
			slot.SetState(Slot.STATE_DIGGED);		
		}		
	}
	
	// Plants the seed into slot (selection_component)
	void PlantSeed( ItemBase seed, string selection_component )
	{
		int slot_index = GetSlotIndexBySelection( selection_component );
		
		if ( slot_index != -1 )
		{
			PluginHorticulture module_horticulture = PluginHorticulture.Cast( GetPlugin( PluginHorticulture ) );
			string plant_type = module_horticulture.GetPlantType( seed );
			
			Slot slot = m_Slots.Get( slot_index );
			slot.SetState(Slot.STATE_PLANTED);
			slot.m_PlantType = plant_type;
			slot.SetSeed(seed);
			
			if ( !slot.NeedsWater() )
			{
				seed.LockToParent();
				//Take some small amount of time before making a plant out of seeds
				Timer growthTimer = NULL;
				growthTimer = new Timer( CALL_CATEGORY_SYSTEM );
				Param createPlantParam = new Param1<Slot>(slot);
				growthTimer.Run( 0.1, this, "CreatePlant", createPlantParam, false ); //Use a const for timer delay
			}
		}
	}
	
	// Creates a plant
	void CreatePlant(Slot slot )
	{
		if (g_Game.IsServer())
		{
			ItemBase seed = slot.GetSeed();
			seed.UnlockFromParent();
			GetGame().ObjectDelete(seed);

			PlantBase plant = PlantBase.Cast( GetInventory().CreateAttachmentEx( slot.m_PlantType, slot.GetSlotId()) );
		
			int slot_index = slot.GetSlotIndex();
			slot.SetPlant(plant);						
			plant.Init( this, slot.GetFertility(), slot.m_HarvestingEfficiency, slot.GetWater() );
			//ShowSelection(SLOT_SELECTION_COVERED_PREFIX + (slot_index + 1).ToStringLen(2));
				
			plant.LockToParent();
		}
	}
		
	void Fertilize( PlayerBase player, ItemBase item, float consumed_quantity, string selection_component )
	{
		Slot slot = GetSlotBySelection( selection_component );
		
		if ( slot != NULL )
		{
			string item_type = item.GetType();
			
			if ( slot.GetFertilityType() == ""  ||  slot.GetFertilityType() == item_type )
			{
				slot.SetFertilityType(item_type);
				
				float add_energy_to_slot = GetGame().ConfigGetFloat( string.Format("cfgVehicles %1 Horticulture AddEnergyToSlot", item_type) );
				slot.m_FertilizerUsage = GetGame().ConfigGetFloat( string.Format("cfgVehicles %1 Horticulture ConsumedQuantity", item_type) );
				
				float coef = Math.Clamp( consumed_quantity / slot.m_FertilizerUsage, 0.0, 1.0 );
				add_energy_to_slot = coef * add_energy_to_slot;
				
				slot.m_FertilizerQuantity += consumed_quantity;
				slot.m_Fertility += add_energy_to_slot;
				
				if ( slot.GetFertilizerQuantity() >= slot.GetFertilizerQuantityMax() )
				{
					int slot_index = slot.GetSlotIndex();
					
					if (slot_index > -1)
					{
						UpdateSlotTexture( slot_index );
						slot.SetFertilityState(eFertlityState.FERTILIZED);
						// Set relevant bit
						m_SlotFertilityState |= slot.GetFertilityState() << slot.GetSlotIndex();
						
						//TODO Boris: Add soft skill 2.0
						//PluginExperience module_exp = GetPlugin(PluginExperience);
						//slot.m_HarvestingEfficiency = module_exp.GetExpParamNumber(player, PluginExperience.EXP_FARMER_FERTILIZATION, "efficiency");
					}
				}
			}
			else
			{
				slot.SetFertilizerQuantity(0);
				slot.SetFertilityType("");
				slot.SetFertilityState(eFertlityState.NONE);
			}
			SetSynchDirty();
		}
	}

	bool IsCorrectFertilizer( ItemBase item, string selection_component )
	{
		Slot slot = GetSlotBySelection( selection_component );
		
		if ( slot != NULL )
		{
			string item_type = item.GetType();
			
			if ( slot.GetFertilityType() == "" || slot.GetFertilityType() == item_type )
			{
				return true;
			}
		}
		
		return false;
	}

	bool NeedsFertilization( string selection_component )
	{
		Slot slot = GetSlotBySelection( selection_component );
		
		if ( slot )
		{
			if ( slot.GetFertilityState() == eFertlityState.NONE )
			{
				return true;
			}
		}
		
		return false;
	}
	
	void SlotWaterStateUpdate( Slot slot )
	{
		// Set relevant bit
		m_SlotWateredState |= slot.GetWateredState() << slot.GetSlotIndex();
		SetSynchDirty();
	}

	void UpdateSlotTexture( int slot_index )
	{
		// TO DO: Fix DAYZ-30633 here!
		Slot slot = m_Slots.Get( slot_index );
		
		// Show / Hide selections according to DIGGED or COVERED states.
		
		if ( slot.IsDigged()  ||  slot.IsPlanted() )
		{
			string str_hide = SLOT_SELECTION_COVERED_PREFIX + (slot_index + 1).ToStringLen(2);
			string str_show = SLOT_SELECTION_DIGGED_PREFIX + (slot_index + 1).ToStringLen(1);
			
			HideSelection( str_hide );
			ShowSelection( str_show );
		}		
		
		if ( slot.GetFertilityType() != "" )
		{
			SetSlotTextureFertilized( slot_index, slot.GetFertilityType() );
		}
		else 
		{
			SetSlotTextureDigged( slot_index );
		}
	}

	void SetSlotTextureDigged( int slot_index )
	{
		TStringArray textures = GetHiddenSelectionsTextures();
		
		string str_digged = SLOT_SELECTION_DIGGED_PREFIX + (slot_index + 1).ToStringLen(1);
		
		ShowSelection( str_digged );
		string texture = textures.Get(0);
		SetObjectTexture( slot_index, texture );
		
		Slot slot = m_Slots.Get( slot_index );
		
		if ( slot.GetWateredState() == 0 )
		{
			// Set dry material
			SetObjectMaterial( slot_index, SLOT_MATERIAL_DRY );
		}
		else
		{
			// Set wet material
			SetObjectMaterial( slot_index, SLOT_MATERIAL_WET );
		}
	}

	void SetSlotTextureFertilized( int slot_index, string item_type )
	{
		TStringArray textures = GetHiddenSelectionsTextures();
		
		int tex_id = GetGame().ConfigGetInt( string.Format("cfgVehicles %1 Horticulture TexId", item_type) );
		
		string str_show = SLOT_SELECTION_DIGGED_PREFIX + (slot_index + 1).ToStringLen(2);
		
		ShowSelection( str_show );
		SetObjectTexture( slot_index, textures.Get(tex_id) );
		
		Slot slot = m_Slots.Get( slot_index );
		
		int slot_index_offset = 0;
		
		// Set material according to dry / wet states
		if ( slot.GetWateredState() == 0 )
		{
			// Set dry material for garden lime
			if ( slot.GetFertilityType() == "GardenLime" )
			{
				SetObjectMaterial( slot_index + slot_index_offset, SLOT_MATERIAL_LIMED_DRY ); 
			}
			else if ( slot.GetFertilityType() == "PlantMaterial" )
			{
				SetObjectMaterial( slot_index + slot_index_offset, SLOT_MATERIAL_COMPOST_DRY ); 
			}
		}
		else
		{
			// Set dry material for compost
			if ( slot.GetFertilityType() == "GardenLime" )
			{
				SetObjectMaterial( slot_index + slot_index_offset, SLOT_MATERIAL_LIMED_WET ); 
			}
			else if ( slot.GetFertilityType() == "PlantMaterial" )
			{
				SetObjectMaterial( slot_index + slot_index_offset, SLOT_MATERIAL_COMPOST_WET );
			}
		}
	}
	
	void RemoveSlot( int index )
	{
		if ( m_Slots != NULL )
		{	
			Slot slot = m_Slots.Get( index );
			PlantBase plant = slot.GetPlant();
			
			if ( plant )
			{
				plant.UnlockFromParent();
				plant.m_MarkForDeletion = true;
				GetGame().ObjectDelete( plant );
			}
			
			slot.Init( GetBaseFertility() );
			
			// Clear relevant bit
			slot.SetFertilityState(eFertlityState.NONE);
			m_SlotFertilityState &= ~(1 << slot.GetSlotIndex());
			
			slot.SetWateredState( eWateredState.DRY );
			m_SlotWateredState &= ~(1 << slot.GetSlotIndex());
			
			SetSynchDirty();
			
			HideSelection( SLOT_SELECTION_COVERED_PREFIX + (index + 1).ToStringLen(2) );
			UpdateSlotTexture( index );
		}
	}

	void RemoveSlotPlant( Object plant )
	{
		int index = GetSlotIndexByPlant( plant );
		if ( index >= 0 )
		{
			RemoveSlot( index );
		}
	}

	Slot GetSlotBySelection( string selection_component )
	{
		int slot_index = GetSlotIndexBySelection( selection_component );
		
		if ( slot_index > -1 )
		{
			return m_Slots.Get( slot_index );
		}
		else
		{
			return NULL;
		}
	}

	// Returns slot array index by selection, starting from 0 as the first one.
	int GetSlotIndexBySelection( string selection_component )
	{
		int slot_index = -1;
		
		if ( m_Slots != NULL )
		{
			string selection_component_lower = selection_component; 
			selection_component_lower.ToLower();
			
			int start = selection_component_lower.IndexOf( SLOT_MEMORY_POINT_PREFIX );
			
			if ( start > -1 )
			{
				start += SLOT_MEMORY_POINT_PREFIX.Length();
				int end = start + 2;
				int length = selection_component.Length();
				
				if ( length >= end )
				{
					int length_add = length - end; // Hack-fix for inconsistent component names in p3d
					int length_from_end = 2 + length_add;
					string num_str = selection_component.Substring( start, length_from_end );
					slot_index = num_str.ToInt();
					
					slot_index = slot_index - 1;
				}
			}
		}
		
		return slot_index;
	}
	
	int GetSlotIndexByAttachmentSlot( string att_slot )
	{
		int slot_index = -1;
		
		int start = "SeedBase_".Length();
		int end = att_slot.Length();//start + 2;
		int len = end - start;
		
		string num_str = att_slot.Substring( start, len );
		slot_index = num_str.ToInt();
		
		return slot_index;
	}

	int GetSlotIndexByPlant( Object plant )
	{
		if ( m_Slots != NULL )
		{
			for ( int i = 0; i < m_Slots.Count(); i++ )
			{
				PlantBase found_plant = m_Slots.Get(i).GetPlant();
				
				if ( found_plant == plant )
				{
					return i;
				}
			}
		}
		
		return -1;
	}
	
	int GetNearestSlotIDByState( vector position, int slot_state)
	{
		float nearest_distance = 1000.0;
		int nearest_slot_index = -1;
		int slots_count = GetGardenSlotsCount();
		for ( int i = 0; i < slots_count; i++ )
		{
			Slot slot = m_Slots.Get(i); // Move this line by a scope higher in this function after debugging
			
			vector slot_pos = GetSlotPosition( i );
			float current_distance = vector.Distance( position, slot_pos );
			
			if ( current_distance < nearest_distance )
			{
				if ( slot != NULL && slot.m_State == slot_state )
				{
					nearest_distance = current_distance;
					nearest_slot_index = i;
				}
			}
		}
		
		return nearest_slot_index;
	}

	vector GetSlotPosition( int index )
	{
		string memory_point = SLOT_MEMORY_POINT_PREFIX + (index + 1).ToStringLen(2);
		vector pos = this.GetSelectionPositionMS( memory_point );
		
		return this.ModelToWorld( pos );
	}
	
	void CheckRainStart()
	{
		if ( !m_CheckRainTimer )
			m_CheckRainTimer = new Timer( CALL_CATEGORY_SYSTEM );
		
		m_CheckRainTimer.Run( CHECK_RAIN_INTERVAL, this, "CheckRainTick", NULL, true );
	}
	
	void CheckRainTick()
	{
		float rain_intensity = GetGame().GetWeather().GetRain().GetActual();
			
		float wetness = rain_intensity * 20 * CHECK_RAIN_INTERVAL;
		
		if (rain_intensity > 1  ||  rain_intensity < 0)
			wetness = 0; // hackfix for weird values returned by weather system
		
		if (wetness == 0)
			wetness = -0.1 * CHECK_RAIN_INTERVAL;
		
		int slots_count = GetGardenSlotsCount();
		
		if ( rain_intensity > 0 )
		{
			WaterAllSlots();
			SetSynchDirty();
		}
		
		for ( int i = 0; i < slots_count; i++ )
		{
			if ( m_Slots )
			{
				Slot slot = m_Slots.Get( i );
				
				if ( slot )
				{
					slot.GiveWater( wetness * Math.RandomFloat01() );
				}
			}
		}
	}
	
	//Used to update
	void WaterAllSlots()
	{
		int state = 0;
		
		for ( int i = 0; i < m_Slots.Count(); i++ )
		{
			state += 1 * Math.Pow( 2, i );
		}
		
		SetSlotWateredState( state );
	}
	
	array<ref Slot> GetSlots()
	{
		return m_Slots;
	}
	
	Slot GetSlotByIndex( int index )
	{
		return m_Slots.Get(index);
	}
	
	int GetSlotWateredState()
	{
		return m_SlotWateredState;
	}
	
	void SetSlotWateredState( int newState )
	{
		m_SlotWateredState = newState;
	}
	
	override void SetActions()
	{
		AddAction(ActionHarvestCrops);
		AddAction(ActionRemovePlant);
		AddAction(ActionRemoveSeed);
	}
}