class GardenPlot extends GardenBase
{
	Object 	m_ClutterCutter;
	private const int GARDEN_SLOT_COUNT = 9;
	
	void GardenPlot()
	{
		SetBaseFertility(0.5);
	}
	
	override void EEInit()
	{	
		super.EEInit();
	}	
	
	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{				
		if ( !super.OnStoreLoad(ctx, version) )
			return false;

		if ( !m_ClutterCutter )
		{		
			m_ClutterCutter = GetGame().CreateObjectEx( "ClutterCutter6x6", GetPosition(), ECE_PLACE_ON_SURFACE );
			m_ClutterCutter.SetOrientation( GetOrientation() );
		}

		return true;
	}

	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		
		if (m_ClutterCutter  &&  GetGame())
		{
			GetGame().ObjectDelete(m_ClutterCutter);
			m_ClutterCutter = NULL;
		}
	}
	
	override bool IsInventoryVisible()
	{
		return true;
	}

	override int GetGardenSlotsCount()
	{
		return GARDEN_SLOT_COUNT;
	}

	void RefreshSlots()
	{
		HideSelection("SeedBase_1");
		HideSelection("SeedBase_2");
		HideSelection("SeedBase_3");
		HideSelection("SeedBase_4");
		HideSelection("SeedBase_5");
		HideSelection("SeedBase_6");
		HideSelection("SeedBase_7");
		HideSelection("SeedBase_8");
		HideSelection("SeedBase_9");
		HideSelection("slotCovered_01");
		HideSelection("slotCovered_02");
		HideSelection("slotCovered_03");
		HideSelection("slotCovered_04");
		HideSelection("slotCovered_05");
		HideSelection("slotCovered_06");
		HideSelection("slotCovered_07");
		HideSelection("slotCovered_08");
		HideSelection("slotCovered_09");
	}
	
	//================================================================
	// ADVANCED PLACEMENT
	//================================================================
	
	override void OnPlacementStarted( Man player )
	{
		RefreshSlots();
	}
	
	override void OnHologramBeingPlaced( Man player )
	{
		RefreshSlots();
	}
	
	override void OnPlacementComplete( Man player, vector position = "0 0 0", vector orientation = "0 0 0" )
	{
		super.OnPlacementComplete( player, position, orientation );
				
		PlayerBase player_base = PlayerBase.Cast( player );
		//vector pos = player_base.GetLocalProjectionPosition();
		//vector ori = player_base.GetLocalProjectionOrientation();
			
		if ( GetGame().IsServer() )
		{
			// To properly move the clutter cutter from spawn position, it must be deleted and created again.
			if (m_ClutterCutter)
			{
				GetGame().ObjectDelete(m_ClutterCutter);
				m_ClutterCutter = NULL;
			}
			
			if (!m_ClutterCutter)
			{		
				m_ClutterCutter = GetGame().CreateObjectEx( "ClutterCutter6x6", GetPosition(), ECE_PLACE_ON_SURFACE );
				m_ClutterCutter.SetOrientation( orientation );
			}
		}
	}
	
	override bool CanBePlaced( Man player, vector position )
	{
		string surface_type;
		GetGame().SurfaceGetType3D( position[0], position[1], position[2], surface_type );
		
		return GetGame().IsSurfaceFertile(surface_type);
	}
}

//class GardenPlotOutside : GardenPlot {}
class GardenPlotPolytunnel : GardenPlot
{
	private const int POLYTUNNEL_SLOT_COUNT = 13;
	override int GetGardenSlotsCount()
	{
		return POLYTUNNEL_SLOT_COUNT;
	}
	
	override void RefreshSlots()
	{
		HideSelection("SeedBase_1");
		HideSelection("SeedBase_2");
		HideSelection("SeedBase_3");
		HideSelection("SeedBase_4");
		HideSelection("SeedBase_5");
		HideSelection("SeedBase_6");
		HideSelection("SeedBase_7");
		HideSelection("SeedBase_8");
		HideSelection("SeedBase_9");
		HideSelection("SeedBase_10");
		HideSelection("SeedBase_11");
		HideSelection("SeedBase_12");
		HideSelection("SeedBase_13");
	}
}
class GardenPlotGreenhouse : GardenPlot {}

class GardenPlotPlacing extends GardenPlot
{
	override void EOnInit(IEntity other, int extra) {};
	override void RefreshSlots() {};
	override void SyncSlots() {};
}