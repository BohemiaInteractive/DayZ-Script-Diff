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
class TreeHard : PlantSuper
{
	override void OnTreeCutDown(EntityAI cutting_entity)
	{
		GetGame().RPCSingleParam(cutting_entity, PlantType.TREE_HARD, null, true);
	}
	
	override bool IsTree()
	{
		return true;
	}

	override bool CanBeAutoDeleted()
	{
		return false;
	}
	
	override bool HasPlayerCollisionSound()
	{
		return false;
	}
};

//-----------------------------------------------------------------------------
//! For specific tree declaration see description of \ref TreeHard
class TreeSoft : PlantSuper
{
	override void OnTreeCutDown(EntityAI cutting_entity)
	{
		GetGame().RPCSingleParam(cutting_entity, PlantType.TREE_SOFT, null, true);
	}
	
	override bool IsTree()
	{
		return true;
	}

	override bool CanBeAutoDeleted()
	{
		return false;
	}

	override bool HasPlayerCollisionSound()
	{
		return false;
	}
};

//-----------------------------------------------------------------------------
//! For specific tree declaration see description of \ref TreeHard
//! default values for hard bushes
class BushHard : PlantSuper
{
	override void OnTreeCutDown(EntityAI cutting_entity)
	{
	
		GetGame().RPCSingleParam(cutting_entity, PlantType.BUSH_HARD, null, true);
	}
	
	override bool IsBush()
	{
		return true;
	}
	
	override bool CanBeAutoDeleted()
	{
		return false;
	}
};

//-----------------------------------------------------------------------------
//! For specific tree declaration see description of \ref TreeHard
class BushSoft : PlantSuper
{
	override void OnTreeCutDown(EntityAI cutting_entity)
	{
		GetGame().RPCSingleParam(cutting_entity, PlantType.BUSH_SOFT, null, true);
	}
	
	override bool IsBush()
	{
		return true;
	}

	override bool CanBeAutoDeleted()
	{
		return false;
	}
};
