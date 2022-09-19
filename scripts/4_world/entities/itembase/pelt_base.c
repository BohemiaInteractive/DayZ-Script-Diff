class Pelt_Base extends ItemBase
{	
	override bool IsPeltBase()
	{
		return true;
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionAttach);
		AddAction(ActionDetach);
	}
}

class WildboarPelt: Pelt_Base {  };
class RabbitPelt: Pelt_Base {  };
class CowPelt: Pelt_Base {  };
class PigPelt: Pelt_Base {  };
class DeerPelt: Pelt_Base {  };
class GoatPelt: Pelt_Base {  };
class BearPelt: Pelt_Base {  };
class WolfPelt: Pelt_Base {  };
class SheepPelt: Pelt_Base {  };
class MouflonPelt: Pelt_Base {  };
class FoxPelt: Pelt_Base {  };