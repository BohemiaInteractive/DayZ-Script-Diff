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
	private static const float	CREATE_PLANT_DELAY				= 1000.0;
	private static const float 	RAIN_INTENSITY_THRESHOLD		= 0.05;
	
	protected ref array<ref Slot> m_Slots;
	protected int 	m_SlotFertilityState = 0; 	//Used to store fertility state of all slots
	protected int 	m_SlotWateredState = 0; 	//Used to store watered state of all slots
	protected int 	m_MaxWateredStateVal = 0; 	//Used to store water max state of all slots
	
	protected int	m_SlotState = 0;			//Used to handle and sync slot states
	
	protected int m_SlotWaterBitmap0; // Slots 1–4
	protected int m_SlotWaterBitmap1; // Slots 5–8
	protected int m_SlotWaterBitmap2; // Slot 9 (and could hold up to 3 more if needed)
	
	protected int m_SlotFertilizerBitmap0; // Slots 1–4
	protected int m_SlotFertilizerBitmap1; // Slots 5–8
	protected int m_SlotFertilizerBitmap2; // Slot 9 (and could hold up to 3 more if needed)
	
	protected float m_DefaultFertility = 1;
	ref Timer 		m_CheckRainTimer;
	
	private static ref map<string,string> m_map_slots; // For the 'attachment slot -> plant slot' conversion. It is possible that this will be removed later.
	
	void GardenBase()
	{
		RegisterNetSyncVariableInt("m_SlotState");
		RegisterNetSyncVariableInt("m_SlotFertilityState");
		RegisterNetSyncVariableInt("m_SlotWateredState");
		
		RegisterNetSyncVariableInt("m_SlotWaterBitmap0");
		RegisterNetSyncVariableInt("m_SlotWaterBitmap1");
		RegisterNetSyncVariableInt("m_SlotWaterBitmap2");
		
		RegisterNetSyncVariableInt("m_SlotFertilizerBitmap0");
		RegisterNetSyncVariableInt("m_SlotFertilizerBitmap1");
		RegisterNetSyncVariableInt("m_SlotFertilizerBitmap2");
		
		m_map_slots = new map<string,string>;
		
		SetEventMask(EntityEvent.INIT); // Enable EOnInit event
		
		// Prepare m_map_slots
		for (int i = 1; i <= GetGardenSlotsCount(); ++i)
		{
			// m_map_slots is supposed to be: <input, output>
			string input = SLOT_SEEDBASE_PREFIX + i.ToString();
			string output = SLOT_MEMORY_POINT_PREFIX;
			
			if (i < 10)
				output = output + "0"; // Example: '1' changes to '01'
			
			output = output + i.ToString();
			
			m_map_slots.Set(input, output);
		}
				
		InitializeSlots();
		SetMaxWaterStateVal();
		
		if (GetGame().IsServer())
		{
			CheckRainStart();
		}
	}
	
	void ~GardenBase()
	{
		if (GetGame() && m_CheckRainTimer)
		{
			m_CheckRainTimer.Stop();
			m_CheckRainTimer = null;
		}
	}

	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		UpdateSlots();
	}
	
	void UpdateSlots()
	{
		if (!m_Slots || m_Slots.Count() == 0)
			return;
		
		//! Set relevant slot values from synced bitmaps
		int slotsCount = GetGardenSlotsCount();
		for (int i = 0; i < slotsCount; i++)
		{
			Slot slot = m_Slots.Get(i);
			
			// Read relevant bits and set the synced values on the slots
			int fertilityState = (m_SlotFertilityState >> i) & 1;
			slot.SetFertilityState(fertilityState);
			
			int wateredState = (m_SlotWateredState >> i) & 1;
			slot.SetWateredState(wateredState);
			
			int waterQuantity = GetWaterQuantity(i);
			slot.SetWater(waterQuantity);
			
			int fertilizerQuantity = GetFertilizerQuantity(i);
			slot.SetFertilizerQuantity(fertilizerQuantity);
			
			int slotState = GetSlotState(i);
			slot.SetState(slotState);
		}
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
	}

	void InitializeSlots()
	{
		m_Slots = new array<ref Slot>;
		int slots_count = GetGardenSlotsCount();
		
		for (int i = 0; i < slots_count; i++)
		{
			Slot slot = new Slot(GetBaseFertility());
			slot.SetSlotIndex(i);
			int i1 = i + 1;
			string name = "SeedBase_" + i1;
			int slotID = InventorySlots.GetSlotIdFromString(name);
			slot.SetSlotId(slotID);
			slot.SetGarden(this);
			slot.SetState(eGardenSlotState.STATE_DIGGED);
			m_Slots.Insert(slot);
			
			SetWaterQuantity(i, 0);
			SetFertilizerQuantity(i, 0);
		}
		
		UpdateSlots();
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
	
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (version <= 118)
			return true;
		
		if (!super.OnStoreLoad(ctx, version))
			return false;
		
		if (version < 102)
		{
			float some_value;
			ctx.Read(some_value); // compatibility check
		}
		
		int slots_count = GetGardenSlotsCount();
		for (int i = 0; i < slots_count; i++)
		{
			Slot slot = m_Slots.Get(i);
			if (!slot.OnStoreLoadCustom(ctx, version))
				return false;
			
			float waterQuantity = slot.GetWater();
			SetWaterQuantity(i, waterQuantity);
			
			float fertilizerQuantity = slot.GetFertilizerQuantity();
			SetFertilizerQuantity(i, fertilizerQuantity);
		}
		
		if ( version >= 119 )
		{
			if(!ctx.Read( m_SlotFertilityState ))
				return false;
		}
		
		if ( version >= 120 )
		{
			if (!ctx.Read( m_SlotWateredState ))
				return false;
		}			
		
		return true;
	}
	
	override void EEOnAfterLoad()
	{
		super.EEOnAfterLoad();

		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SyncSlots, 500, false, this);
	}
	
	void SyncSlots()
	{
		if (g_Game.IsServer() || !g_Game.IsMultiplayer())
		{
			UpdateSlots();
			
			int slotsCount = GetGardenSlotsCount();
			for (int i = 0; i < slotsCount; ++i)
			{
				UpdateSlotTexture(i);
			}
			
			if (g_Game.IsDedicatedServer())
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
		Debug.Log("" + this + " | Position: " + GetPosition());
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
			int slot_slot_ID = slot.GetSlotId();
			int   slot_wateredState = slot.GetWateredState();
			int	  slot_FertilityState = slot.GetFertilityState();
			
			Debug.Log("Fertility : " + slot_fertility);
			Debug.Log("Fertility Usage : " + slot_fertility_usage);
			Debug.Log("Fertility Type : " + slot_fertility_type);
			Debug.Log("Fertility State : " + slot_FertilityState);
			Debug.Log("Water : " + slot_water);
			Debug.Log("Water Usage : " + slot_water_usage);
			Debug.Log("Watered State : " + typename.EnumToString(eWateredState, slot_wateredState));
			Debug.Log("Seed : " + slot_seed);
			Debug.Log("Plant : " + slot_plant);
			if (slot_plant && PlantBase.Cast(slot_plant))
				PlantBase.Cast(slot_plant).PrintValues();
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

	override bool CanPutIntoHands( EntityAI parent )
	{
		if ( !super.CanPutIntoHands( parent ) )
		{
			return false;
		}
		return false;
	}

	override bool CanRemoveFromHands( EntityAI parent )
	{
		return false;
	}

	int GetGardenSlotsCount()
	{
		return 0;
	}

	bool CanPlantSeed(string selection_component)
	{
		Slot slot = GetSlotBySelection(selection_component);
		if (slot != NULL && slot.GetState() == eGardenSlotState.STATE_DIGGED)
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
		
		if (IsItemSeed) 
		{
			string converted_slot_name;
			
			vector pos = GetPosition();
			int index = GetSlotIndexByAttachmentSlot(slot_name);
			
			if (index < 10)
			{
				converted_slot_name = SLOT_MEMORY_POINT_PREFIX + "0" + index.ToString();
			}
			else
			{
				converted_slot_name = SLOT_MEMORY_POINT_PREFIX + index.ToString();
			}
			
			if (g_Game.IsServer() || !g_Game.IsMultiplayer())
				PlantSeed(ItemBase.Cast(item), converted_slot_name);
		}
		else if (g_Game.IsClient())
		{
			Slot slot = GetSlotByIndex(GetSlotIndexByAttachmentSlot(slot_name) - 1);
			if (slot)
			{
				slot.SetPlant(PlantBase.Cast(item));
				slot.SetState(eGardenSlotState.STATE_PLANTED);
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
			
			slot.SetState(eGardenSlotState.STATE_DIGGED);		
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
			slot.SetState(eGardenSlotState.STATE_PLANTED);
			slot.m_PlantType = plant_type;
			slot.SetSeed(seed);
			
			if ( !slot.NeedsWater() || slot.GetWateredState() == eWateredState.WET)
			{
				//! Adjust slot water
				if (slot.GetWateredState() == eWateredState.WET && slot.NeedsWater())
				{
					int waterMax = slot.GetWaterMax();
					slot.SetWater(waterMax);
				}
				
				seed.LockToParent();
				//Take some small amount of time before making a plant out of seeds
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CreatePlant, CREATE_PLANT_DELAY, false, slot);
			}
		}
	}
	
	// Creates a plant
	void CreatePlant(Slot slot)
	{
		if (g_Game.IsServer() || !g_Game.IsMultiplayer())
		{
			ItemBase seed = slot.GetSeed();
			if (seed)
			{
				seed.UnlockFromParent();
				GetGame().ObjectDelete(seed);
	
				PlantBase plant = PlantBase.Cast( GetInventory().CreateAttachmentEx(slot.m_PlantType, slot.GetSlotId()));
				int slot_index = slot.GetSlotIndex();
				slot.SetPlant(plant);
				slot.SetState(eGardenSlotState.STATE_PLANTED);
				plant.Init(this, slot.GetFertility(), slot.m_HarvestingEfficiency, slot.GetWater());
				plant.LockToParent();
			}
		}
	}
	
	void SetSlotState(int slotIndex, int value)
	{
	    // value should be 1 or 2
	    int shift = slotIndex * 2;
		int mask = 3 << shift;
		int currentState = (m_SlotState & mask) >> shift;
	
	    // Clear out the bits for this slot
	    m_SlotState &= ~mask;
	
	    // Set the new value
	    m_SlotState |= (value << shift);

		if (g_Game.IsServer() && currentState != value)
			SetSynchDirty();
	}
	
	int GetSlotState(int slotIndex)
	{
	    int shift = slotIndex * 2;
		int mask = 3 << shift;
	    int value = (m_SlotState & mask) >> shift;
	    return value;    // Will be 0, 1, or 2
	}
	
	void SlotWaterStateUpdate( Slot slot )
	{
		int idx = slot.GetSlotIndex();
	    int mask = 1 << idx;
		int currentState = (m_SlotWateredState >> idx) & 1;
		int newState = slot.GetWateredState();
	    // Clear the relevant bit first
	    m_SlotWateredState &= ~mask;
	
	    // Set the bit state (1 for WET, 0 for DRY)
	    m_SlotWateredState |= (newState << idx);
		
		if ((g_Game.IsServer() || !g_Game.IsMultiplayer()) && currentState != newState)
		{
			UpdateSlotTexture(slot.GetSlotIndex());
			
			if (g_Game.IsServer())
				SetSynchDirty();
		}
	}
	
	void SlotFertilityStateUpdate(Slot slot)
	{
		int idx = slot.GetSlotIndex();
		int mask = 1 << idx;
		int currentState = (m_SlotFertilityState >> idx) & 1;
		int newState = slot.GetFertilityState();
		
		// Clear the relevant bit first
		m_SlotFertilityState &= ~mask;
		
		// Set the bit state (1 for FERTILIZED, 0 for NONE)
		m_SlotFertilityState |= (newState << idx);
		
		if ((g_Game.IsServer() || !g_Game.IsMultiplayer()) && currentState != newState)
		{
			UpdateSlotTexture(slot.GetSlotIndex());
			
			if (g_Game.IsServer())
				SetSynchDirty();
		}
	}
	
	//! This should be only called on server as the selections are synced to clients in C++
	void UpdateSlotTexture( int slot_index )
	{
		if (!m_Slots || m_Slots.Count() - 1 < slot_index)
			return;
		
		Slot slot = m_Slots.Get( slot_index );
		
		// Show / Hide selections according to DIGGED or COVERED states.
		if ( slot.IsDigged()  ||  slot.IsPlanted() )
		{
			string str_hide = SLOT_SELECTION_COVERED_PREFIX + (slot_index + 1).ToStringLen(2);
			string str_show = SLOT_SELECTION_DIGGED_PREFIX + (slot_index + 1).ToStringLen(1);
			
			HideSelection( str_hide );
			ShowSelection( str_show );
		}		
		
		if ( slot.GetFertilityState() == eFertlityState.FERTILIZED && slot.GetFertilityType() != "" )
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
	
	void RemoveSlot(int index)
	{
		if (m_Slots)
		{
			Slot slot = m_Slots.Get(index);
			PlantBase plant = slot.GetPlant();
			
			if (plant && !plant.IsPendingDeletion())
			{
				plant.UnlockFromParent();
				plant.m_MarkForDeletion = true;
				plant.Delete();
			}
			
			slot.Init(GetBaseFertility());
			
			int idx = slot.GetSlotIndex();
			int mask = 1 << idx;
			
			// Clear the relevant bits from bitmaps
			m_SlotFertilityState &= ~mask;
			m_SlotWateredState &= ~mask;
			
			// Reset states
			m_SlotFertilityState |= (slot.GetFertilityState() << idx);
			m_SlotWateredState |= (slot.GetWateredState() << idx);
			
			// Reset stol quantities
			SetWaterQuantity(index, 0);
			SetFertilizerQuantity(index, 0);
			
			UpdateSlots();
			
			HideSelection(SLOT_SELECTION_COVERED_PREFIX + (index + 1).ToStringLen(2));
			UpdateSlotTexture(idx);
			
			SetSynchDirty();
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
	
	int GetNearestSlotIDByState(vector position, int slot_state)
	{
		float nearest_distance = 1000.0;
		int nearest_slot_index = -1;
		int slots_count = GetGardenSlotsCount();
		for (int i = 0; i < slots_count; i++)
		{
			Slot slot = m_Slots.Get(i); // Move this line by a scope higher in this function after debugging
			vector slot_pos = GetSlotPosition(i);
			float current_distance = vector.Distance(position, slot_pos);
			
			if (current_distance < nearest_distance)
			{
				if (slot != NULL && slot.GetState() == slot_state)
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
		float rainIntensity = GetGame().GetWeather().GetRain().GetActual();
		float wetness = rainIntensity * 20 * CHECK_RAIN_INTERVAL;

		if (rainIntensity > 1  || rainIntensity < 0)
			wetness = 0; // hackfix for weird values returned by weather system
		
		if (wetness == 0)
			wetness = -0.1 * CHECK_RAIN_INTERVAL;
		
		// At the moment we dont want to remove water from the slot when it is not raining so we check and manipulate water quantity only when we add to it
		if (rainIntensity > RAIN_INTENSITY_THRESHOLD)
		{	
			if (wetness != 0) // only change slot water quantity when wetness value can actually add/deduct something
			{
				int slotsCount = GetGardenSlotsCount();
				for (int i = 0; i < slotsCount; i++)
				{
					if (m_Slots)
					{
						Slot slot = m_Slots.Get(i);
						if (slot)
						{
							slot.GiveWater(wetness * Math.RandomFloat01());
						}
					}
				}
			}
		}
	}
	
	array<ref Slot> GetSlots()
	{
		return m_Slots;
	}
	
	Slot GetSlotByIndex( int index )
	{
		return m_Slots.Get(index);
	}
	
	override void SetActions()
	{
		AddAction(ActionHarvestCrops);
		AddAction(ActionRemovePlant);
		AddAction(ActionRemoveSeed);
	}
	
	void SetFertilizerBitmapByIndex(int index, int value)
	{
	    switch (index)
	    {
	        case 0: 
				m_SlotFertilizerBitmap0 = value; 
			break;
	        case 1: 
				m_SlotFertilizerBitmap1 = value; 
			break;
	        case 2: 
				m_SlotFertilizerBitmap2 = value; 
			break;
	    }
	}
	
	int GetFertilizerBitmapByIndex(int index)
	{
	    switch (index)
	    {
	        case 0: 
				return m_SlotFertilizerBitmap0;
	        case 1:
				return m_SlotFertilizerBitmap1;
	        case 2: 
				return m_SlotFertilizerBitmap2;
	    }
	    return 0;
	}
	
	void SetWaterBitmapByIndex(int index, int value)
	{
	    switch (index)
	    {
	        case 0: 
				m_SlotWaterBitmap0 = value; 
			break;
	        case 1: 
				m_SlotWaterBitmap1 = value; 
			break;
	        case 2: 
				m_SlotWaterBitmap2 = value; 
			break;
	    }
	}
	
	int GetWaterBitmapByIndex(int index)
	{
	    switch (index)
	    {
	        case 0: 
				return m_SlotWaterBitmap0;
	        case 1:
				return m_SlotWaterBitmap1;
	        case 2: 
				return m_SlotWaterBitmap2;
	    }
	    return 0;
	}
	
	void SetWaterQuantity(int slotIndex, int value)
	{
	    int bitsPerSlot = 8;
	    int slotsPerInt = 32 / bitsPerSlot;
	
	    int groupIndex = slotIndex / slotsPerInt;
	    int bitOffset = (slotIndex % slotsPerInt) * bitsPerSlot;
	
	    int mask = 255 << bitOffset;
	    value = Math.Clamp(value, 0, 200);
	
	    int temp = GetWaterBitmapByIndex(groupIndex);
	    temp &= ~mask;
	    temp |= (value << bitOffset);
	    SetWaterBitmapByIndex(groupIndex, temp);
	}
	
	int GetWaterQuantity(int slotIndex)
	{
	    int bitsPerSlot = 8;
	    int slotsPerInt = 32 / bitsPerSlot;
	
	    int groupIndex = slotIndex / slotsPerInt;
	    int bitOffset = (slotIndex % slotsPerInt) * bitsPerSlot;
	
	    int value = GetWaterBitmapByIndex(groupIndex);
	    return (value >> bitOffset) & 255;
	}
	
	void SetFertilizerQuantity(int slotIndex, int value)
	{
	    int bitsPerSlot = 8;
	    int slotsPerInt = 32 / bitsPerSlot;
	
	    int groupIndex = slotIndex / slotsPerInt;
	    int bitOffset = (slotIndex % slotsPerInt) * bitsPerSlot;
	
	    int mask = 255 << bitOffset;
	    value = Math.Clamp(value, 0, 200);
	
	    int temp = GetFertilizerBitmapByIndex(groupIndex);
	    temp &= ~mask;
	    temp |= (value << bitOffset);
	    SetFertilizerBitmapByIndex(groupIndex, temp);
	}
	
	int GetFertilizerQuantity(int slotIndex)
	{
	    int bitsPerSlot = 8;
	    int slotsPerInt = 32 / bitsPerSlot;
	
	    int groupIndex = slotIndex / slotsPerInt;
	    int bitOffset = (slotIndex % slotsPerInt) * bitsPerSlot;
	
	    int value = GetFertilizerBitmapByIndex(groupIndex);
	    return (value >> bitOffset) & 255;
	}
	
	//! DEPRECATED
	[Obsolete("No replacement")]	
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
	
	[Obsolete("No replacement")]
	void UpdateTexturesOnAllSlots()
	{
		int slots_count = GetGardenSlotsCount();
		for (int i = 0; i < slots_count; ++i)
		{
			UpdateSlotTexture(i);
		}
	}
	
	[Obsolete("No replacement")]
	bool NeedsFertilization(string selection_component)
	{
		Slot slot = GetSlotBySelection(selection_component);
		if (slot)
		{
			if (slot.GetFertilityState() == eFertlityState.NONE)
			{
				return true;
			}
		}
		
		return false;
	}

	[Obsolete("CAContinuousFertilizeGardenSlot::FertilizeSlot is used now instead.")]
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

	[Obsolete("No replacement")]
	void WaterAllSlots()
	{
		for (int i = 0; i < m_Slots.Count(); i++)
		{
			Slot slot = m_Slots[i];
			if (!slot)
				continue;

			slot.SetWateredState(eWateredState.WET);
			UpdateSlotTexture(slot.GetSlotIndex());
			m_SlotWateredState |= eWateredState.WET << slot.GetSlotIndex();
		}
	}
}