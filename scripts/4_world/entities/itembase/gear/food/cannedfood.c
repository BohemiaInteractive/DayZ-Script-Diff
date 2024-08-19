//-------------Animal food-----------------------------------------------
class DogFoodCan : Edible_Base
{
	override void Open()
	{
		//super.Open();
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
}

class DogFoodCan_Opened: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !( GetAgents() & eAgents.FOOD_POISON );
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
		//super.Open();
		ReplaceEdibleWithNew("CatFoodCan_Opened");
	}
	
	override bool IsOpen()
	{
		return false;
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
		//super.Open();
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
}

class PorkCan_Opened: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !( GetAgents() & eAgents.FOOD_POISON );
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
		//super.Open();
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
}

class Lunchmeat_Opened: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !( GetAgents() & eAgents.FOOD_POISON );
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
		//super.Open();
		ReplaceEdibleWithNew("UnknownFoodCan_Opened");
	}
	
	override bool IsOpen()
	{
		return false;
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
		return !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmallCan);
	}
	
	override bool IsOpen()
	{
		return false;
	}
};

//-------------Pajka-----------------------------------------------
class Pajka : Edible_Base
{
	override void Open()
	{
		//super.Open();
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
}

class Pajka_Opened: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmallCan);
	}
	
	override bool IsOpen()
	{
		return false;
	}
};

//-------------Pate-----------------------------------------------
class Pate : Edible_Base
{
	override void Open()
	{
		//super.Open();
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
}

class Pate_Opened: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmallCan);
	}
	
	override bool IsOpen()
	{
		return false;
	}
};

//-------------BrisketSpread-----------------------------------------------
class BrisketSpread : Edible_Base
{
	override void Open()
	{
		//super.Open();
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
}

class BrisketSpread_Opened: Edible_Base
{
	override bool CanDecay()
	{
		return true;
	}
	
	override bool CanProcessDecay()
	{
		return !( GetAgents() & eAgents.FOOD_POISON );
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeedSmall);
		AddAction(ActionEatSmallCan);
	}
};