class GasMask_Filter : ItemBase
{
	/*
	override void OnQuantityChanged(float delta)
	{
		super.OnQuantityChanged(delta);
		
		if (GetQuantity() == 0)
		{
			SetHealth("","",0);
		}
	}
	*/
	
	override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		if (GetGame().IsServer())
		{
			if( newLevel == GameConstants.STATE_RUINED )
			{
				SetQuantity(0);
			}
		}
	}
	
	override float GetFilterDamageRatio() 
	{
		return 0.09; //Health lost per quantity consumed
	}
}

class GasMask_Filter_Improvised: GasMask_Filter
{
	override float GetFilterDamageRatio() 
	{
		return 0.25; //Health lost per quantity consumed
	}
}