class EyePatch_Improvised extends Clothing
{
	override array<int> GetEffectWidgetTypes()
	{
		return {EffectWidgetsTypes.EYEPATCH_OCCLUDER};
	}
	
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};

