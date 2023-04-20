class EyePatch_Improvised extends Glasses_Base
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

