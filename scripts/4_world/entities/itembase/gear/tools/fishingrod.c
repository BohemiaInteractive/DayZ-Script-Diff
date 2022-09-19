class FishingRod: FishingRod_Base_New
{
	void FishingRod()
	{
	}
	
	override float GetFishingEffectivityBonus()
	{
		return 0.1;
	}
	
	override void AnimateFishingRod(bool state)
	{
		SetAnimationPhase("AnimateRod",state);
	}
	
	override void SetActions()
	{
		super.SetActions();
	}
};
