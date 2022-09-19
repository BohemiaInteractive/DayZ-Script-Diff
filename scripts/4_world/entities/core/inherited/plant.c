/*enum PlantType
{
	TREE_HARD		= 0,
	TREE_SOFT		= 1,
	BUSH_HARD		= 2,	
	BUSH_SOFT		= 3,
}*/
//-----------------------------------------------------------------------------
/** \brief Specific tree class can be declared as: class TreeHard + _ + p3d filename (without extension) 
@code
//exapmle for tree DZ\plants2\tree\t_acer2s.p3d
class TreeHard_t_acer2s: TreeHard 
{
};
@endcode
*/
class TreeHard extends PlantSuper
{
	override void OnTreeCutDown( EntityAI cutting_entity )
	{
		//int tree_hard = 14;
		
		//ToolBase cut_tree_tool = ToolBase.Cast( cutting_tool );		
		GetGame().RPCSingleParam( cutting_entity, PlantType.TREE_HARD, NULL, true );
	}
	
	override bool IsTree()
	{
		return true;
	}
	/*
	override void InitMiningValues()
	{
		super.InitMiningValues();
		
		//m_BarkType = "Bark_Oak";
	};
	*/
	/*override int GetAmountOfDrops(ItemBase item)
	{
		if ( item && item.KindOf("Knife") )
		{
			return 1000; //HOTFIX "infinite" bark
		}
		else if ( item && item.KindOf("Axe") )
		{
			return 4;
		}
		else
		{
			return 100; 
		}
	}
	
	override void GetMaterialAndQuantityMap(ItemBase item, out map<string,int> output_map)
	{
		if ( item && item.KindOf("Knife") )
		{
			output_map.Insert("Bark_Oak",1);
		}
		else if ( item && item.KindOf("Axe") )
		{
			output_map.Insert("FireWood",1);
			//output_map.Insert("WoodenStick",1);
		}
	}
	
	override float GetDamageToMiningItemEachDrop(ItemBase item)
	{
		if ( item && item.KindOf("Knife") )
		{
			return 3;
		}
		else if ( item && item.KindOf("Axe") )
		{
			return 2;
		}
		else
		{
			return 0; 
		}
	}*/
};

//-----------------------------------------------------------------------------
//! For specific tree declaration see description of \ref TreeHard
class TreeSoft extends PlantSuper
{
	override void OnTreeCutDown( EntityAI cutting_entity )
	{
		//ToolBase cut_tree_tool = ToolBase.Cast( cutting_tool );
		GetGame().RPCSingleParam( cutting_entity, PlantType.TREE_SOFT, NULL, true );
	}
	
	override bool IsTree()
	{
		return true;
	}
	/*
	override void InitMiningValues()
	{
		super.InitMiningValues();
		
		m_BarkType = "Bark_Oak";
	};
	*/
	/*override int GetAmountOfDrops(ItemBase item)
	{
		if ( item && item.KindOf("Knife") )
		{
			return 1000; //HOTFIX "infinite" bark
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
	
	override void GetMaterialAndQuantityMap(ItemBase item, out map<string,int> output_map)
	{
		if ( item && item.KindOf("Knife") )
		{
			output_map.Insert("Bark_Oak",1);
		}
		else if ( item && item.KindOf("Axe") )
		{
			//output_map.Insert("FireWood",1);
			output_map.Insert("WoodenStick",1);
		}
	}
	
	override float GetDamageToMiningItemEachDrop(ItemBase item)
	{
		if ( item && item.KindOf("Knife") )
		{
			return 2;
		}
		else if ( item && item.KindOf("Axe") )
		{
			return 2;
		}
		else
		{
			return 0; 
		}
	}*/
};

//-----------------------------------------------------------------------------
//! For specific tree declaration see description of \ref TreeHard
//! default values for hard bushes
class BushHard extends PlantSuper
{
	override void OnTreeCutDown( EntityAI cutting_entity )
	{
		//ToolBase cut_tree_tool = ToolBase.Cast( cutting_tool );		
		GetGame().RPCSingleParam( cutting_entity, PlantType.BUSH_HARD, NULL, true );
	}
	
	override bool IsBush()
	{
		return true;
	}
	
	/*override int GetAmountOfDrops(ItemBase item)
	{
		if ( item && item.KindOf("Knife") )
		{
			return 1;
		}
		else if ( item && item.KindOf("Axe") )
		{
			return 1;
		}
		else
		{
			return 3;
		}
	}
	
	override void GetMaterialAndQuantityMap(ItemBase item, out map<string,int> output_map)
	{
		if ( item && item.KindOf("Knife") )
		{
			output_map.Insert("LongWoodenStick",1);
		}
		else if ( item && item.KindOf("Axe") )
		{
			output_map.Insert("LongWoodenStick",1);
			//output_map.Insert("WoodenStick",1);
		}
	}
	
	override float GetDamageToMiningItemEachDrop(ItemBase item)
	{
		if ( item && item.KindOf("Knife") )
		{
			return 2;
		}
		else if ( item && item.KindOf("Axe") )
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}*/
};

//-----------------------------------------------------------------------------
//! For specific tree declaration see description of \ref TreeHard
class BushSoft extends PlantSuper
{
	override void OnTreeCutDown( EntityAI cutting_entity )
	{
		//ToolBase cut_tree_tool = ToolBase.Cast( cutting_tool );		
		GetGame().RPCSingleParam( cutting_entity, PlantType.BUSH_SOFT, NULL, true );
	}
	
	override bool IsBush()
	{
		return true;
	}
	
	/*override int GetAmountOfDrops(ItemBase item)
	{
		if ( item && item.KindOf("Knife") )
		{
			return 1;
		}
		else if ( item && item.KindOf("Axe") )
		{
			return 1;
		}
		else
		{
			return 3;
		}
	}
	
	override void GetMaterialAndQuantityMap(ItemBase item, out map<string,int> output_map)
	{
		if ( item && item.KindOf("Knife") )
		{
			output_map.Insert("LongWoodenStick",1);
		}
		else if ( item && item.KindOf("Axe") )
		{
			output_map.Insert("LongWoodenStick",1);
			//output_map.Insert("WoodenStick",1);
		}
	}
	
	override float GetDamageToMiningItemEachDrop(ItemBase item)
	{
		if ( item && item.KindOf("Knife") )
		{
			return 1;
		}
		else if ( item && item.KindOf("Axe") )
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}*/
};
