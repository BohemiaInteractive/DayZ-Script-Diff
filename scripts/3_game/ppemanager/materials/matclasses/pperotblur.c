//!Rotation Blur
class PPERotBlur: PPEClassBase
{
	static const int PARAM_POWER = 0;
	static const int PARAM_MINANGLEPERSEC = 1;
	static const int PARAM_MAXANGLEPERSEC = 2;
	static const int PARAM_MINDEPTH = 3;
	static const int PARAM_MAXDEPTH = 4;
	
	override int GetPostProcessEffectID()
	{
		return PostProcessEffectType.RotBlur;
	}
	
	override string GetDefaultMaterialPath()
	{
		return "Graphics/Materials/postprocess/rotblur";
	}
	
	override void RegisterMaterialParameters()
	{
		RegisterParameterScalarFloat(PARAM_POWER,"Power",0.0,0.0,0.09);
		RegisterParameterScalarFloat(PARAM_MINANGLEPERSEC,"MinAnglePerSec",10.0,0.0,180.0);
		RegisterParameterScalarFloat(PARAM_MAXANGLEPERSEC,"MaxAnglePerSec",100.0,0.0,180.0);
		RegisterParameterScalarFloat(PARAM_MINDEPTH,"MinDepth",2.5,0.0,5.0);
		RegisterParameterScalarFloat(PARAM_MAXDEPTH,"MaxDepth",4.5,0.0,50.0);
	}
}