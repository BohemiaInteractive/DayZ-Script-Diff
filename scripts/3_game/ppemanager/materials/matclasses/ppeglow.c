/*enum PPEGlow
{
	PARAM_VISIBLEPERCENT = 0,
}*/

//!Glow - PostProcessEffectType.Glow
class PPEGlow: PPEClassBase
{
	static const int PARAM_VISIBLEPERCENT = 0;
	static const int PARAM_TARGETBRIGHTNESS = 1;
	static const int PARAM_SPEEDDARKTOBRIGHT = 2;
	static const int PARAM_SPEEDBRIGHTTODARK = 3;
	static const int PARAM_TONEMAPPING = 4;
	static const int PARAM_HDR = 5;
	static const int PARAM_FILMICSHOULDERSTRENGTH = 6;
	static const int PARAM_FILMICLINEARSTRENGTH = 7;
	static const int PARAM_FILMICLINEARANGLE = 8;
	static const int PARAM_FILMICTOESTRENGTH = 9;
	static const int PARAM_FILMICTOENUMERATOR = 10;
	static const int PARAM_FILMICTOEDENUMERATOR = 11;
	static const int PARAM_FILMICEXPOSUREBIAS = 12;
	static const int PARAM_FILMICWHITEPOINT = 13;
	static const int PARAM_BLOOMTHRESHOLD = 14;
	static const int PARAM_BLOOMSTEEPNESS = 15;
	static const int PARAM_BLOOMINTENSITY = 16;
	static const int PARAM_BRIGHTNESS = 17;
	static const int PARAM_CONTRAST = 18;
	static const int PARAM_OFFSET = 19;
	static const int PARAM_OVERLAYFACTOR = 20;
	static const int PARAM_OVERLAYCOLOR = 21;
	static const int PARAM_SATURATION = 22;
	static const int PARAM_COLORIZATIONCOLOR = 23;
	static const int PARAM_DESATURATIONWEIGHTS = 24;
	static const int PARAM_VIGNETTE = 25;
	static const int PARAM_VIGNETTECOLOR = 26;
	static const int PARAM_LENSDISTORT = 27;
	static const int PARAM_MAXCHROMABBERATION = 28;
	static const int PARAM_LENSCENTERX = 29;
	static const int PARAM_LENSCENTERY = 30;
	
	//layer info
	static const int L_20_HIT = 100;
	static const int L_20_FLASHBANG = 300;
	static const int L_20_SHOCK = 500;
	
	static const int L_21_SHOCK = 100;
	static const int L_21_HIT = 300;
	static const int L_21_FLASHBANG = 500;
	
	static const int L_22_BLOODLOSS = 100;
	
	static const int L_23_GLASSES = 100;
	static const int L_23_TOXIC_TINT = 200;
	static const int L_23_NVG = 600;
	
	static const int L_25_MENU = 100;
	static const int L_25_TUNNEL = 300;
	static const int L_25_SHOCK = 500;
	static const int L_25_UNCON = 700;
	static const int L_25_BURLAP = 800;
	
	static const int L_26_MENU = 100;
	static const int L_26_TUNNEL = 300;
	static const int L_26_SHOCK = 500;
	static const int L_26_UNCON = 700;
	static const int L_26_BURLAP = 800;
	
	static const int L_27_ADS = 100;
	
	static const int L_28_ADS = 100;
	
	static const int L_29_ADS = 100;
	
	static const int L_30_ADS = 100;
	
	override int GetPostProcessEffectID()
	{
		return PostProcessEffectType.Glow;
	}
	
	override string GetDefaultMaterialPath()
	{
		return "Graphics/Materials/postprocess/glow";
	}
	
	override void RegisterMaterialParameters()
	{
		//HDR
		RegisterParameterScalarFloat(PARAM_VISIBLEPERCENT,"VisiblePercent",95,1,100);
		RegisterParameterScalarFloat(PARAM_TARGETBRIGHTNESS,"TargetBrightness",0.3,0.01,1);
		RegisterParameterScalarFloat(PARAM_SPEEDDARKTOBRIGHT,"SpeedDarkToBright",0.3,0.01,1);
		RegisterParameterScalarFloat(PARAM_SPEEDBRIGHTTODARK,"SpeedBrightToDark",0.3,0.01,1);
		RegisterParameterScalarInt(PARAM_TONEMAPPING,"ToneMapping",2,0,2);
		RegisterParameterScalarBool(PARAM_HDR,"HDR",1);
		//Filmic tonemapping
		RegisterParameterScalarFloat(PARAM_FILMICSHOULDERSTRENGTH,"FilmicShoulderStrength",0.153,0,1);
		RegisterParameterScalarFloat(PARAM_FILMICLINEARSTRENGTH,"FilmicLinearStrength",0.357,-1,1);
		RegisterParameterScalarFloat(PARAM_FILMICLINEARANGLE,"FilmicLinearAngle",0.231,-1,1);
		RegisterParameterScalarFloat(PARAM_FILMICTOESTRENGTH,"FilmicToeStrength",0.1573,0.001,3);
		RegisterParameterScalarFloat(PARAM_FILMICTOENUMERATOR,"FilmicToeNumerator",0.011,-1,1);
		RegisterParameterScalarFloat(PARAM_FILMICTOEDENUMERATOR,"FilmicToeDenumerator",3.75,0.1,20);
		RegisterParameterScalarFloat(PARAM_FILMICEXPOSUREBIAS,"FilmicExposureBias",4.0,0.1,20);
		RegisterParameterScalarFloat(PARAM_FILMICWHITEPOINT,"FilmicWhitePoint",1.5,0.1,20);
		//Bloom
		RegisterParameterScalarFloat(PARAM_BLOOMTHRESHOLD,"BloomThreshold",0.5,0,1);
		RegisterParameterScalarFloat(PARAM_BLOOMSTEEPNESS,"BloomSteepness",1,0,10);
		RegisterParameterScalarFloat(PARAM_BLOOMINTENSITY,"BloomIntensity",1,0,4);
		//Color modificators
			//color defines
			RegisterParameterScalarFloat(PARAM_BRIGHTNESS,"Brightness",1,0,2);
			RegisterParameterScalarFloat(PARAM_CONTRAST,"Contrast",1,0,2);
			RegisterParameterScalarFloat(PARAM_OFFSET,"Offset",0,-1,1);
			RegisterParameterScalarFloat(PARAM_OVERLAYFACTOR,"OverlayFactor",0,0,1);
			RegisterParameterColor(PARAM_OVERLAYCOLOR,"OverlayColor",1,1,1,0);
			RegisterParameterScalarFloatEx(PARAM_SATURATION,"Saturation",1,0,1,PPEMatClassParameterFloatSaturation);
			RegisterParameterColorEx(PARAM_COLORIZATIONCOLOR,"ColorizationColor",1.0,1.0,1.0,0.0,PPEMatClassParameterColorColorization);
			RegisterParameterVector(PARAM_DESATURATIONWEIGHTS,"DesaturationWeights",{0.299,0.587,0.114,0});
		RegisterParameterScalarFloat(PARAM_VIGNETTE,"Vignette",0,0,2); //C++ maximum is set to 1.0 (Workbench), but the parameter can accept more. 2.0 is a reasonable value
		RegisterParameterColor(PARAM_VIGNETTECOLOR,"VignetteColor",0,0,0,0);
		//Distort
		RegisterParameterScalarFloat(PARAM_LENSDISTORT,"LensDistort",0,-5,5);
		RegisterParameterScalarFloat(PARAM_MAXCHROMABBERATION,"MaxChromAbberation",0,0,1);
		RegisterParameterScalarFloat(PARAM_LENSCENTERX,"LensCenterX",0,-1,1);
		RegisterParameterScalarFloat(PARAM_LENSCENTERY,"LensCenterY",0,-1,1);
	}
}