class SambucusBerry extends Edible_Base
{
	override bool CanBeCooked()
	{
		return true;
	}		
	
	override bool CanBeCookedOnStick()
	{
		return false;
	}
	
	override bool IsFruit()
	{
		return true;
	}

	override bool CanDecay()
	{
		return true;
	}
	
	bool ConditionAttach ( EntityAI parent )
	{
		//if Barrel_ColorBase
		if ( parent.IsInherited( Barrel_ColorBase ) )
		{
			Barrel_ColorBase barrel = Barrel_ColorBase.Cast( parent );
			
			if ( barrel.IsOpen() && !barrel.FindAttachmentBySlotName( "Nails" ) && !barrel.FindAttachmentBySlotName( "OakBark" ) && !barrel.FindAttachmentBySlotName( "BirchBark" ) && !barrel.FindAttachmentBySlotName( "Lime" ) && !barrel.FindAttachmentBySlotName( "Disinfectant" ) && !barrel.FindAttachmentBySlotName( "Guts" ) )
			{
				return true;
			}
		}
		
		return false;
	}
	
	bool ConditionDetach ( EntityAI parent )
	{
		//if Barrel_ColorBase
		if ( parent.IsInherited( Barrel_ColorBase ) )
		{
			Barrel_ColorBase barrel = Barrel_ColorBase.Cast( parent );
			
			if ( !barrel.IsOpen() )
			{
				return false;
			}
		}
		
		return true;
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionForceFeed);
		AddAction(ActionEat);
	}
}
