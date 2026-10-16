class NVGHeadstrap : Glasses_Base
{
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionToggleNVG);
	}

	override protected void InitGlobalExclusionValues()
	{
		super.InitGlobalExclusionValues();

		ClearSingleExclusionValueGlobal(EAttExclusions.EXCLUSION_GLASSES_REGULAR_0);
		AddSingleExclusionValueGlobal(EAttExclusions.EXCLUSION_HEADSTRAP_0);
	}
	
	override ECachedEquipmentItemCategory GetCachedEquipmentCategory()
	{
		return ECachedEquipmentItemCategory.NONE;
	}
	
	override void OnDebugSpawn()
	{
		EntityAI item = GetInventory().CreateInInventory("NVGoggles");
		item.GetInventory().CreateInInventory("Battery9V");
	}
}
