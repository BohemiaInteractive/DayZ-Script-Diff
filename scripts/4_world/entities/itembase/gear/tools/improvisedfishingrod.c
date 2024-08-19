class ImprovisedFishingRod: FishingRod_Base_New
{
	override void AnimateFishingRod(bool state)
	{
		SetAnimationPhase("AnimateRod",state);
	}
};
