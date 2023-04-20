class TrackSuitPants_ColorBase extends Pants_Base
{
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionWringClothes);
	}
};
class TrackSuitPants_Black extends TrackSuitPants_ColorBase {};
class TrackSuitPants_Blue extends TrackSuitPants_ColorBase {};
class TrackSuitPants_Green extends TrackSuitPants_ColorBase {};
class TrackSuitPants_Red extends TrackSuitPants_ColorBase {};
class TrackSuitPants_LightBlue extends TrackSuitPants_ColorBase {};