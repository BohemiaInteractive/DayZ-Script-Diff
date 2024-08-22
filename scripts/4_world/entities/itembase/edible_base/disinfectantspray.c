class DisinfectantSpray : Edible_Base
{
	override void InitItemVariables()
	{
		super.InitItemVariables();

		can_this_be_combined 	= true;
		m_VarLiquidType 		= GetLiquidTypeInit();
	}
	
	override bool CanPutAsAttachment(EntityAI parent)
	{
		if (!super.CanPutAsAttachment(parent))
			return false;

		const int SLOTS_ARRAY = 8;
		bool isBarrel;
		bool isBarrelOpened;
		bool slotsEmptyTest = true;
		string slotNames[SLOTS_ARRAY] = {"BerryR", "BerryB", "Plant", "Nails", "OakBark", "BirchBark", "Lime", "Guts"};

		if (parent.IsKindOf("Barrel_ColorBase"))
			isBarrel = true;

		if (isBarrel && parent.GetAnimationPhase("Lid") == 1)
			isBarrelOpened = true;

		for (int i = 0; i < SLOTS_ARRAY ; ++i)
		{
			if (parent.FindAttachmentBySlotName(slotNames[i]) != null)
			{
				slotsEmptyTest = false;
				break;
			}
		}
		
		if ((isBarrelOpened && slotsEmptyTest) || !isBarrel)
			return true;

		return false;
	}

	override bool CanDetachAttachment( EntityAI parent )
	{
		
		bool isBarrel;
		bool isBarrelOpened;
		
		if (parent.IsKindOf("Barrel_ColorBase"))
			isBarrel = true;

		if (isBarrel && parent.GetAnimationPhase("Lid") == 1)
			isBarrelOpened = true;
		
		if (isBarrelOpened || !isBarrel)
			return true;

		return false;
	}
	
	override float GetDisinfectQuantity(int system = 0, Param param1 = null)
	{
		return (GetQuantityMax() * 0.15);
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionDisinfectTarget);
		AddAction(ActionDisinfectSelf);
		AddAction(ActionDisinfectPlant);
		AddAction(ActionWashHandsItemContinuous);
	}
}