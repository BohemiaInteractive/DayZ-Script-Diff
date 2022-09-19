//!FilmGrain - PostProcessEffectType.FilmGrain
/*!
	Be advised, for this to take any effect, 'g_Game.NightVissionLightParams' needs to have non-zero 'noiseIntensity' parameter.
	/note set by 'PPELightIntensityParamsNative.PARAM_NOISE_MULT' currently
*/
class PPEFilmGrain: PPEClassBase
{
	static const int PARAM_INTENSITY = 0;
	static const int PARAM_SHARPNESS = 1;
	static const int PARAM_GRAINSIZE = 2;
	static const int PARAM_INTENSITYX0 = 3;
	static const int PARAM_INTENSITYX1 = 4;
	static const int PARAM_MONOCHROMATIC = 5;
	static const int PARAM_SIMPLE = 6;
	static const int PARAM_DISTORT = 7;
	static const int PARAM_FREQUENCY = 8;
	//static const int PARAM_NOISEMAP = 9;
	
	static const int L_1_NVG = 100;
	static const int L_1_TOXIC_TINT = 200;
	static const int L_2_NVG = 100;
	static const int L_2_TOXIC_TINT = 200;
	
	override int GetPostProcessEffectID()
	{
		return PostProcessEffectType.FilmGrain;
	}
	
	override string GetDefaultMaterialPath()
	{
		return "Graphics/Materials/postprocess/filmgrainNV"; //TODO - differentiate between filmgrainNV?
	}
	
	override void RegisterMaterialParameters()
	{
		RegisterParameterScalarFloat(PARAM_INTENSITY,"Intensity",0.0,0.0,1.0);
		RegisterParameterScalarFloat(PARAM_SHARPNESS,"Sharpness",2.35,0.0,20.0);
		RegisterParameterScalarFloat(PARAM_GRAINSIZE,"GrainSize",2.75,1.0,9.0);
		RegisterParameterScalarFloat(PARAM_INTENSITYX0,"IntensityX0",0.0,0.0,1.0);
		RegisterParameterScalarFloat(PARAM_INTENSITYX1,"IntensityX1",0.0,0.0,1.0);
		RegisterParameterScalarBool(PARAM_MONOCHROMATIC,"Monochromatic",true);
		RegisterParameterScalarBool(PARAM_SIMPLE,"Simple",false);
		RegisterParameterScalarBool(PARAM_DISTORT,"Distort",true);
		RegisterParameterScalarFloat(PARAM_FREQUENCY,"Frequency",20.0,1.0,1000.0);
		//RegisterParameterTexture(PARAM_NOISEMAP,"NoiseMap","{0B1C7AEDC4645C8A}System/textures/noise.edds");
	}
}