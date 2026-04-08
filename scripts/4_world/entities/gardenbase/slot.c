enum eFertlityState
{
	NONE = 0,
	FERTILIZED = 1
	//This will be used to set bit values (DO NOT ADD MORE VALUES)
}

enum eWateredState
{
	DRY = 0,
	WET = 1
	//Used to improve readability of watered state changes
}

enum eGardenSlotState
{
	STATE_DIGGED = 1,
	STATE_PLANTED = 2
	//Used to set bit values depending on seed plant state
}

class Slot
{
	static const int		FERTILIZER_USAGE	= 200;
	
	private int 			m_WaterQuantity;
	static private int 		m_WaterNeeded 		= 190; // How much water is needed to water a plant from a bottle. Value is in mililitres
	static private int 		m_WaterMax 			= 200; 
	
	float m_Fertility;
	float m_FertilizerUsage;
	float m_FertilizerQuantity;
	float m_FertilizerQuantityMax;	// how much of a fertilizer can the slot contain
	int m_slotIndex;
	int m_slotId;
	
	string m_FertilizerType = "";
	int    m_FertilityState = eFertlityState.NONE;
	int    m_WateredState = eWateredState.DRY;
	string m_DiggedSlotComponent; // example: "Component02" for the 1st slot in GardenBase
	string m_PlantType;
	private ItemBase m_Seed;
	private GardenBase m_Garden;
	
	float m_HarvestingEfficiency;
	
	int m_State;
	
	private PlantBase m_Plant;
	
	void Slot( float base_fertility )
	{
		m_Seed = NULL;
		m_Plant = NULL;
		Init( base_fertility );
	}

	void ~Slot()
	{
		if (m_Plant  &&  g_Game) // g_Game returns NULL when the game is being quit!
		{
			g_Game.ObjectDelete( GetPlant() );
		}
	}
	
	int GetSlotIndex()
	{
		return m_slotIndex;
	}
	
	void SetSlotIndex(int index)
	{
		m_slotIndex = index;
	}
	
	int GetSlotId()
	{
		return m_slotId;
	}

	void SetSlotId(int id)
	{
		m_slotId = id;
	}
	
	void SetGarden(GardenBase garden)
	{
		m_Garden = garden;
	}
	
	GardenBase GetGarden()
	{
		return m_Garden;
	}
	
	void SetSeed(ItemBase seed)
	{
		m_Seed = seed;
	}
	
	PlantBase GetPlant()
	{
		return m_Plant;
	}
	
	void SetPlant(PlantBase plant)
	{
		m_Plant = plant;
		
		if (plant)
		{
			plant.SetSlot(this);
			plant.SetGarden(m_Garden);
		}
	}
	
	//Used to force water level an go around sync issues
	void SetWater( int val )
	{
		val = Math.Clamp( val, 0, m_WaterMax );
		m_WaterQuantity = val;
	}
	
	ItemBase GetSeed()
	{
		return m_Seed;
	}
	
	bool HasSeed()
	{
		if (m_Seed)
			return true;
		
		return false;
	}
	
	void GiveWater( float consumed_quantity )
	{
		m_WaterQuantity = Math.Clamp(m_WaterQuantity + consumed_quantity, 0.0, GetWaterMax());		
		m_Garden.SetWaterQuantity(GetSlotIndex(), m_WaterQuantity);
		
		bool needsWater = NeedsWater();
		if (m_WateredState == eWateredState.DRY && !needsWater)
		{
			SetWateredState(eWateredState.WET);
			m_Garden.SlotWaterStateUpdate(this);
		}
		//! This is here in case we want to support dry out the slot in the future
		else if (m_WateredState == eWateredState.WET && needsWater)
		{
			SetWateredState(eWateredState.DRY);
			m_Garden.SlotWaterStateUpdate(this);
		}

		if (!g_Game.IsServer())
			return;
		
		if (!GetPlant() && GetSeed() && !NeedsWater()) // if there is no seed then do not create plant. Plant will be created when the seed is inserted into watered slot.
			GetGarden().CreatePlant(this);
	}
	
	bool NeedsWater()
	{
		if ( m_WaterQuantity < GetWaterUsage() )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
	bool CanBeWatered()
	{
		if ( m_WaterQuantity < GetWaterMax() )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
	float GetWater()
	{
		return m_WaterQuantity;
	}
	
	float GetFertility()
	{
		return m_Fertility;
	}
	
	float GetFertilityMax()
	{
		return m_FertilizerUsage;
	}
	
	void SetFertility(float fertility)
	{
		m_Fertility = fertility;
	}
	
	float GetFertilizerQuantity()
	{
		return m_FertilizerQuantity;
	}

	void SetFertilizerQuantity(float fertility)
	{
		m_FertilizerQuantity = fertility;
		m_Garden.SetFertilizerQuantity(GetSlotIndex(), Math.Ceil(m_FertilizerQuantity));
		
		if (m_FertilizerQuantityMax != 0 && m_FertilizerQuantity >= m_FertilizerQuantityMax)
		{
			SetFertilityState(eFertlityState.FERTILIZED);
			m_Garden.SlotFertilityStateUpdate(this);
		}
	}
	
	float GetFertilizerQuantityMax()
	{
		return m_FertilizerQuantityMax;
	}
	
	void SetFertilizerQuantityMax(float quantMax)
	{
		m_FertilizerQuantityMax = quantMax;
	}
	
	string GetFertilityType()
	{
		return m_FertilizerType;
	}
	
	void SetFertilityType(string type)
	{
		m_FertilizerType = type;
	}
	
	int GetFertilityState()
	{
		return m_FertilityState;
	}
	
	void SetFertilityState(int newState)
	{
		if (m_FertilityState == newState)
			return;

		m_FertilityState = newState;		
		m_Garden.SlotFertilityStateUpdate(this);
	}
	
	int GetWateredState()
	{
		return m_WateredState;
	}
	
	void SetWateredState(int newState)
	{
		if (m_WateredState == newState)
			return;
		
		m_WateredState = newState;
		if (m_WateredState == eWateredState.WET)
		{
			SetWater(GetWaterMax());
		}
		else if (m_WateredState == eWateredState.DRY)
		{
			SetWater(0);
		}
		
		m_Garden.SlotWaterStateUpdate(this);
	}
	
	float GetWaterUsage()
	{
		return m_WaterNeeded;
	}
	
	float GetWaterMax()
	{
		return m_WaterMax;
	}
	
	int GetState()
	{
		return m_State;
	}
	
	void SetState(int new_state)
	{
		m_State = new_state;
		m_Garden.SetSlotState(GetSlotIndex(), new_state);
	}
	
	bool IsDigged()
	{
		if (m_State == eGardenSlotState.STATE_DIGGED)
		{
			return true;
		}
		
		return false;
	}
	
	bool IsPlanted()
	{
		if (m_State == eGardenSlotState.STATE_PLANTED)
		{
			return true;
		}
		
		return false;
	}
	
	void Init( float base_fertility )
	{
		m_Fertility = base_fertility;
		m_FertilizerUsage = FERTILIZER_USAGE;
		m_FertilizerQuantity = 0.0;
		m_FertilizerType = "";
		m_FertilityState = eFertlityState.NONE;
		m_WateredState = eWateredState.DRY;
		m_WaterQuantity = 0.0;
		m_HarvestingEfficiency = 1.0;
		m_State = eGardenSlotState.STATE_DIGGED;
		m_Plant = NULL;
	}

	void SetSlotComponent(string component)
	{
		m_DiggedSlotComponent = component;
	}

	string GetSlotComponent()
	{
		return m_DiggedSlotComponent;
	}
	
	bool OnStoreLoadCustom( ParamsReadContext ctx, int version )
	{
		if ( version < 102 )
		{
			ctx.Read( m_Fertility );
			ctx.Read( m_FertilizerUsage );
			ctx.Read( m_FertilizerQuantity );
			
			if ( !ctx.Read( m_FertilizerType ) )
				m_FertilizerType = "";
			
			ctx.Read( m_HarvestingEfficiency );
			ctx.Read( m_State );
		}
		
		if ( version >= 102 )
		{
			if(!ctx.Read( m_Fertility ))
				return false;

			if(!ctx.Read( m_FertilizerUsage ))
				return false;
			
			if(!ctx.Read( m_FertilizerQuantity ))
				return false;
			
			if(!ctx.Read( m_HarvestingEfficiency ))
				return false;
			
			if(!ctx.Read( m_State ))
				return false;
   			
			if (!ctx.Read( m_FertilizerType ))
			{
				m_FertilizerType = "";
			}
		}
		
		if (version >= 142)
		{
			int wateredState;
			if(!ctx.Read( wateredState ))
			{
				return false;
			}
			else
			{
				SetWateredState(wateredState);
			}
		}
		
		return true;
	}

	void OnStoreSaveCustom( ParamsWriteContext ctx )
	{
		ctx.Write( m_Fertility );
		ctx.Write( m_FertilizerUsage );
		ctx.Write( m_FertilizerQuantity );
		ctx.Write( m_HarvestingEfficiency );
		ctx.Write( m_State );
		ctx.Write( m_FertilizerType );
		ctx.Write( m_WateredState );
	}
}