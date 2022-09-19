//!WetDistort - PostProcessEffectType.WetDistort
class PPEWetDistort: PPEClassBase
{
	static const int PARAM_BLURPOWER = 0;
	static const int PARAM_LOCALBLURPOWER = 1;
	static const int PARAM_EFFPOWERTOP = 2;
	static const int PARAM_EFFPOWERBOTTOM = 3;
	static const int PARAM_BLURDOWNSIZE = 4;
	static const int PARAM_BLURGAUSS = 5;
	static const int PARAM_WAVSPDX1 = 6;
	static const int PARAM_WAVSPDX2 = 7;
	static const int PARAM_WAVSPDY1 = 8;
	static const int PARAM_WAVSPDY2 = 9;
	static const int PARAM_WAVEAMPX1 = 10;
	static const int PARAM_WAVEAMPX2 = 11;
	static const int PARAM_WAVEAMPY1 = 12;
	static const int PARAM_WAVEAMPY2 = 13;
	static const int PARAM_PHASERANDX = 14;
	static const int PARAM_PHASERANDY = 15;
	static const int PARAM_PHASEPOSX = 16;
	static const int PARAM_PHASEPOSY = 17;
	
	override int GetPostProcessEffectID()
	{
		return PostProcessEffectType.WetDistort;
	}
	
	override string GetDefaultMaterialPath()
	{
		return "Graphics/Materials/postprocess/wetdistort";
	}
	
	override void RegisterMaterialParameters()
	{
		RegisterParameterScalarFloat(PARAM_BLURPOWER,"BlurPower",0.5,0.0,1.0);
		RegisterParameterScalarFloat(PARAM_LOCALBLURPOWER,"LocalBlurPower",0.4,0.0,1.0);
		RegisterParameterScalarFloat(PARAM_EFFPOWERTOP,"EffectPowerTop",0.0,0.0,1.0);
		RegisterParameterScalarFloat(PARAM_EFFPOWERBOTTOM,"EffectPowerBottom",0.0,0.0,1.0);
		RegisterParameterScalarInt(PARAM_BLURDOWNSIZE,"BlurDownSize",2,0,3);
		RegisterParameterScalarInt(PARAM_BLURGAUSS,"BlurGauss",1,1,4);
		RegisterParameterScalarFloat(PARAM_WAVSPDX1,"WaveSpeedX1",4.1,0.5,10.0);
		RegisterParameterScalarFloat(PARAM_WAVSPDX2,"WaveSpeedX2",3.7,0.5,10.0);
		RegisterParameterScalarFloat(PARAM_WAVSPDY1,"WaveSpeedY1",2.5,0.5,10.0);
		RegisterParameterScalarFloat(PARAM_WAVSPDY2,"WaveSpeedY2",1.85,0.5,10.0);
		RegisterParameterScalarFloat(PARAM_WAVEAMPX1,"WaveAmpX1",0.005,0.001,0.02);
		RegisterParameterScalarFloat(PARAM_WAVEAMPX2,"WaveAmpX2",0.004,0.001,0.02);
		RegisterParameterScalarFloat(PARAM_WAVEAMPY1,"WaveAmpY1",0.009,0.001,0.02);
		RegisterParameterScalarFloat(PARAM_WAVEAMPY2,"WaveAmpY2",0.007,0.001,0.02);
		RegisterParameterScalarFloat(PARAM_PHASERANDX,"PhaseRandX",0.5,0.0,1.0);
		RegisterParameterScalarFloat(PARAM_PHASERANDY,"PhaseRandY",0.3,0.0,1.0);
		RegisterParameterScalarFloat(PARAM_PHASEPOSX,"PhasePosX",10.0,0.0,20.0);
		RegisterParameterScalarFloat(PARAM_PHASEPOSY,"PhasePosY",6.0,0.0,20.0);
	}
}