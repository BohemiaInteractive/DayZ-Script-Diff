//!Median - PostProcessEffectType.Median
//unused/not modifiable from script?
class PPEMedian: PPEClassBase
{
	static const int PARAM_KERNEL = 0;
	static const int PARAM_METHOD = 1;
	
	override int GetPostProcessEffectID()
	{
		return PostProcessEffectType.Median;
	}
	
	override string GetDefaultMaterialPath()
	{
		return "Graphics/Materials/postprocess/median";
	}
	
	override void RegisterMaterialParameters()
	{
		RegisterParameterScalarInt(PARAM_KERNEL,"Kernel",3,3,5);
		RegisterParameterScalarInt(PARAM_METHOD,"Method",2,0,2);
	}
}