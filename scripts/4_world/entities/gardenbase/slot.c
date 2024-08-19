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

class Slot
{
	static const int 		STATE_DIGGED 		= 1;
	static const int 		STATE_PLANTED 		= 2;
	
	private int 			m_WaterQuantity;
	static private int 		m_WaterNeeded 		= 190; // How much water is needed to water a plant from a bottle. Value is in mililitres
	static private int 		m_WaterMax 			= 200; 
	
	float m_Fertility;
	float m_FertilizerUsage;
	float m_FertilizerQuantity;
	int m_slotIndex;
	int m_slotId;
	
	string m_FertilizerType;
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
		m_WaterQuantity = 0.0;
		Init( base_fertility );
	}

	void ~Slot()
	{
		if (m_Plant  &&  GetGame()) // GetGame() returns NULL when the game is being quit!
		{
			GetGame().ObjectDelete( GetPlant() );
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
		bool needed_water = NeedsWater();
		m_WaterQuantity += consumed_quantity;
		
		if (m_WaterQuantity >= GetWaterMax())
			m_WaterQuantity = GetWaterMax();
		
		if (m_WaterQuantity < 0)
			m_WaterQuantity = 0;
		
		if ( !NeedsWater() )
		{
			if ( !GetPlant() )
			{
				if ( GetSeed() )
				{
					GetGarden().CreatePlant(this);
				}
				
				// if there is no seed then do not create plant. Plant will be created when the seed is inserted into watered slot.
			}
		}
		
		if ( needed_water != NeedsWater() )
		{
			SetWateredState( eWateredState.WET );
			GetGarden().UpdateSlotTexture( GetSlotIndex() );
			if ( m_Garden.GetSlotWateredState() != m_Garden.GetMaxWaterStateVal() )
				m_Garden.SlotWaterStateUpdate( this );
		}
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
	
	float GetFertilizerQuantityMax()
	{
		return m_FertilizerUsage;
	}
	
	void SetFertilizerQuantity(float fertility)
	{
		m_FertilizerQuantity = fertility;
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
	
	void SetFertilityState( int newState )
	{
		m_FertilityState = newState;
	}
	
	int GetWateredState()
	{
		return m_WateredState;
	}
	
	void SetWateredState( int newState )
	{
		m_WateredState = newState;
		if ( m_WateredState == eWateredState.WET )
			SetWater( GetWaterMax() );
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
	}
	
	bool IsDigged()
	{
		if (m_State == STATE_DIGGED)
		{
			return true;
		}
		
		return false;
	}
	bool IsPlanted()
	{
		if (m_State == STATE_PLANTED)
		{
			return true;
		}
		
		return false;
	}
	
	void Init( float base_fertility )
	{
		m_Fertility = base_fertility;
		m_FertilizerUsage = 200;
		m_FertilizerQuantity = 0.0;
		m_FertilizerType = "";
		m_FertilityState = eFertlityState.NONE;
		
		m_WaterQuantity = 0;
		m_WateredState = eWateredState.DRY;
		
		m_HarvestingEfficiency = 1.0;
		//m_DiggedSlotComponent = "";
		m_State = STATE_DIGGED;
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
			ctx.Read( m_Fertility );       
			ctx.Read( m_FertilizerUsage );
			ctx.Read( m_FertilizerQuantity );       
			ctx.Read( m_HarvestingEfficiency );
			ctx.Read( m_State );
			
			if ( !ctx.Read( m_FertilizerType ) )
			{
				m_FertilizerType = "";
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
	}
}