//-------------Animal food-----------------------------------------------
class DogFoodCan : Edible_Base
{
	override void Open()
	{
		ReplaceEdibleWithNew("DogFoodCan_Opened");
	}
		
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpen);
	}
	
	override bool IsOpen()
	{
		return false;
	}
	
	override float GetQuantityNormalizedScripted()
	{
		return 1.0;
	}
}

class DogFoodCan_Opened: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !GetIsFrozen() && !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmallCan);
	}
};

class CatFoodCan: DogFoodCan
{
	override void Open()
	{
		ReplaceEdibleWithNew("CatFoodCan_Opened");
	}
	
	override bool IsOpen()
	{
		return false;
	}
	
	override float GetQuantityNormalizedScripted()
	{
		return 1.0;
	}
}

class CatFoodCan_Opened: DogFoodCan_Opened
{

};

//-------------PorkCan-----------------------------------------------
class PorkCan : Edible_Base
{
	override void Open()
	{
		ReplaceEdibleWithNew("PorkCan_Opened");
	}
		
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpen);
	}
	
	override bool IsOpen()
	{
		return false;
	}

	override float GetQuantityNormalizedScripted()
	{
		return 1.0;
	}
}

class PorkCan_Opened: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !GetIsFrozen() && !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmallCan);
	}
};

//-------------Lunchmeat-----------------------------------------------
class Lunchmeat : Edible_Base
{
	override void Open()
	{
		ReplaceEdibleWithNew("Lunchmeat_Opened");
	}
		
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpen);
	}
	
	override bool IsOpen()
	{
		return false;
	}
	
	override float GetQuantityNormalizedScripted()
	{
		return 1.0;
	}
}

class Lunchmeat_Opened: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !GetIsFrozen() && !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmallCan);
	}
};



//-------------CrabCan-----------------------------------------------
class CrabCan : Edible_Base
{
	override void Open()
	{
		ReplaceEdibleWithNew("CrabCan_Opened");
	}
		
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpen);
	}
	
	override bool IsOpen()
	{
		return false;
	}
	
	override float GetQuantityNormalizedScripted()
	{
		return 1.0;
	}
}

class CrabCan_Opened: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !GetIsFrozen() && !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmallCan);
	}
};

//-------------Unknown Food-----------------------------------------------
class UnknownFoodCan : Edible_Base
{
	override void Open()
	{
		ReplaceEdibleWithNew("UnknownFoodCan_Opened");
	}
	
	override bool IsOpen()
	{
		return false;
	}
	
	override float GetQuantityNormalizedScripted()
	{
		return 1.0;
	}
}

class UnknownFoodCan_Opened: Edible_Base
{
	void UnknownFoodCan_Opened()
	{
		InsertAgent(eAgents.FOOD_POISON, 1);
	}
	
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !GetIsFrozen() && !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmallCan);
	}
	
	override bool IsOpen()
	{
		return true;
	}
};

//-------------Pajka-----------------------------------------------
class Pajka : Edible_Base
{
	override void Open()
	{
		ReplaceEdibleWithNew("Pajka_Opened");
	}
		
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpen);
	}
	
	override bool IsOpen()
	{
		return false;
	}
	
	override float GetQuantityNormalizedScripted()
	{
		return 1.0;
	}
}

class Pajka_Opened: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !GetIsFrozen() && !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmallCan);
	}
	
	override bool IsOpen()
	{
		return true;
	}
};

//-------------Pate-----------------------------------------------
class Pate : Edible_Base
{
	override void Open()
	{
		ReplaceEdibleWithNew("Pate_Opened");
	}
		
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpen);
	}
	
	override bool IsOpen()
	{
		return false;
	}
	
	override float GetQuantityNormalizedScripted()
	{
		return 1.0;
	}
}

class Pate_Opened: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !GetIsFrozen() && !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmallCan);
	}
	
	override bool IsOpen()
	{
		return true;
	}
};

//-------------BrisketSpread-----------------------------------------------
class BrisketSpread : Edible_Base
{
	override void Open()
	{
		ReplaceEdibleWithNew("BrisketSpread_Opened");
	}
		
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionOpen);
	}
	
	override bool IsOpen()
	{
		return false;
	}
	
	override float GetQuantityNormalizedScripted()
	{
		return 1.0;
	}
}

class BrisketSpread_Opened: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !GetIsFrozen() && !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmallCan);
	}
};