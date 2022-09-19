enum EHarvestType
{
	NORMAL,
	BARK
}

class WoodBase extends Plant
{
	
	static bool 	m_IsCuttable;
	static int 		m_PrimaryDropsAmount = -1;
	static int 		m_SecondaryDropsAmount = -1;
	static float 	m_ToolDamage = -1.0;
	static float 	m_CycleTimeOverride = -1.0;
	static string 	m_PrimaryOutput = ""; //some nonsensical item for debugging purposes
	static string 	m_SecondaryOutput = ""; //some nonsensical item for debugging purposes
	static string 	m_BarkType = "";
	
	/*
	 bool 	m_IsCuttable;
	 int 	m_PrimaryDropsAmount = -1;
	 int 	m_SecondaryDropsAmount = -1;
	 float 	m_ToolDamage = -1.0;
	 float 	m_CycleTimeOverride = -1.0;
	 string 	m_PrimaryOutput = ""; //some nonsensical item for debugging purposes
	 string 	m_SecondaryOutput = ""; //some nonsensical item for debugging purposes
	 string 	m_BarkType = "";
	*/
	
	void WoodBase()
	{
		//InitMiningValues();
	}
	
	
	void InitMiningValues()
	{
		//m_IsCuttable = false;
	};
	
	override bool IsWoodBase()
	{
		return true;
	}

	override bool IsCuttable()
	{
		return ConfigGetBool("isCuttable");
	}
	
	int GetPrimaryDropsAmount()
	{
		return ConfigGetInt("primaryDropsAmount");
	}
	
	int GetSecondaryDropsAmount()
	{
		return ConfigGetInt("secondaryDropsAmount");
	}
	
	float GetToolDamage()
	{
		return ConfigGetFloat("toolDamage");
	}
	
	float GetCycleTimeOverride()
	{
		return ConfigGetFloat("cycleTimeOverride");
	}
	
	string GetPrimaryOutput()
	{
		return ConfigGetString("primaryOutput");
	}
	
	string GetSecondaryOutput()
	{
		return ConfigGetString("secondaryOutput");
	}
	
	string GetBarkType()
	{
		return ConfigGetString("barkType");
	}
	
	
	
	int GetAmountOfDrops(ItemBase item)
	{
		if ( GetPrimaryDropsAmount() > 0 )
		{
			if ( IsTree() && item && ( item.KindOf("Knife") || item.IsInherited(Screwdriver) ) )
			{
				return -1;
			}
			else
			{
				return GetPrimaryDropsAmount(); 
			}
		}
		else
		{
			if ( item && ( item.KindOf("Knife") || item.IsInherited(Screwdriver) ) )
			{
				return -1;
			}
			else if ( item && item.KindOf("Axe") )
			{
				return 3;
			}
			else
			{
				return 100;
			}
		}
	}
	
	int GetAmountOfDropsEx(ItemBase item, EHarvestType type)
	{
		if ( GetPrimaryDropsAmount() > 0 )
		{
			if ( IsTree() && item && type == EHarvestType.BARK )
			{
				return -1;
			}
			else
			{
				return GetPrimaryDropsAmount(); 
			}
		}
		else
		{
			if ( item && type == EHarvestType.BARK )
			{
				return -1;
			}
			else if ( item &&  type == EHarvestType.NORMAL )
			{
				return 3;
			}
			else
			{
				return 100;
			}
		}
	}
	
	void GetMaterialAndQuantityMap(ItemBase item, out map<string,int> output_map)
	{
		if ( IsTree() && item && ( item.KindOf("Knife") || item.IsInherited(Screwdriver) ) && GetBarkType() != "" )
		{
			output_map.Insert(GetBarkType(),1);
		}
		else
		{
			output_map.Insert(GetPrimaryOutput(),1);
		}
	}
	
	void GetMaterialAndQuantityMapEx(ItemBase item, out map<string,int> output_map, EHarvestType type)
	{
		if ( IsTree() && item && type == EHarvestType.BARK && GetBarkType() != "" )
		{
			output_map.Insert(GetBarkType(),1);
		}
		else
		{
			output_map.Insert(GetPrimaryOutput(),1);
		}
	}
	
	float GetDamageToMiningItemEachDrop(ItemBase item)
	{
		if (GetToolDamage() > -1)
			return GetToolDamage();
		
		if ( IsTree() )
		{
			if ( item && item.KindOf("Knife") )
			{
				return 20;
			}
			else if ( item && item.KindOf("Axe") )
			{
				return 20;
			}
			else
			{
				return 0; 
			}
		}
		else
		{
			if ( item && item.KindOf("Knife") )
			{
				return 30;
			}
			else if ( item && item.KindOf("Axe") )
			{
				return 30;
			}
			else
			{
				return 0;
			}
		}
	}
	
	float GetDamageToMiningItemEachDropEx(ItemBase item, EHarvestType type)
	{
		if (GetToolDamage() > -1)
			return GetToolDamage();
		
		if ( IsTree() )
		{
			if ( item && type == EHarvestType.BARK )
			{
				return 20;
			}
			else if ( item && type == EHarvestType.NORMAL )
			{
				return 20;
			}
			else
			{
				return 0; 
			}
		}
		else
		{
			if ( item && type == EHarvestType.BARK )
			{
				return 30;
			}
			else if ( item && type == EHarvestType.NORMAL )
			{
				return 30;
			}
			else
			{
				return 0;
			}
		}
	}
	

};