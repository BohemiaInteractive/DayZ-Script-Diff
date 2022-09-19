//!Dummy class - PostProcessEffectType.None
class PPENone: PPEClassBase
{
	override int GetPostProcessEffectID()
	{
		return PostProcessEffectType.None;
	}
	
	override string GetDefaultMaterialPath()
	{
		return "";
	}
};