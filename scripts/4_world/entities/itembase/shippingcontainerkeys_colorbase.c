class ShippingContainerKeys_ColorBase : ToolBase
{
	override void InitItemSounds()
	{
		super.InitItemSounds();
		
		ItemSoundHandler handler = GetItemSoundHandler();
		handler.AddSound(SoundConstants.ITEM_KEY_BREAK, "AlarmClock_Hit_SoundSet");
	}
	
	void DestroyKeyServer()
	{
		SetHealth("","",0);
		StartItemSoundServer(SoundConstants.ITEM_KEY_BREAK);
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionUnlockShippingContainer);
	}
};

class ShippingContainerKeys_Blue : ShippingContainerKeys_ColorBase
{
	override int GetKeyCompatibilityType()
	{
		return 1 << EBuildingLockType.SHIP_CONTAINER_0;
	}
};

class ShippingContainerKeys_Yellow : ShippingContainerKeys_ColorBase
{
	override int GetKeyCompatibilityType()
	{
		return 1 << EBuildingLockType.SHIP_CONTAINER_1;
	}
};

class ShippingContainerKeys_Orange : ShippingContainerKeys_ColorBase
{
	override int GetKeyCompatibilityType()
	{
		return 1 << EBuildingLockType.SHIP_CONTAINER_2;
	}
};

class ShippingContainerKeys_Red : ShippingContainerKeys_ColorBase
{
	override int GetKeyCompatibilityType()
	{
		return 1 << EBuildingLockType.SHIP_CONTAINER_3;
	}
};
