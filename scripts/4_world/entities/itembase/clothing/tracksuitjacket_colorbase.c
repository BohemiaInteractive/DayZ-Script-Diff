class TrackSuitJacket_ColorBase extends Clothing
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class TrackSuitJacket_Black extends TrackSuitJacket_ColorBase {};
class TrackSuitJacket_Blue extends TrackSuitJacket_ColorBase {};
class TrackSuitJacket_Green extends TrackSuitJacket_ColorBase {};
class TrackSuitJacket_LightBlue extends TrackSuitJacket_ColorBase {};
class TrackSuitJacket_Red extends TrackSuitJacket_ColorBase {};