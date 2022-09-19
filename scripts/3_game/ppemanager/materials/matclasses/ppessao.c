//!SSAO - PostProcessEffectType.SSAO
class PPESSAO: PPEClassBase
{
	static const int POSTPROCESS_OPTION_VALUE_LOW = 0;
	static const int POSTPROCESS_OPTION_VALUE_MEDIUM = 1;
	static const int POSTPROCESS_OPTION_VALUE_HIGH = 2;
	static const int POSTPROCESS_OPTION_VALUE_HIGHEST = 3;
	
	static const int PARAM_NUMPASSES = 0;
	static const int PARAM_INTENSITY = 1;
	static const int PARAM_RADIUSNEAR = 2;
	static const int PARAM_RADIUSFAR = 3;
	static const int PARAM_COSANGLE0 = 4;
	static const int PARAM_COSANGLE1 = 5;
	static const int PARAM_MAXDISTANCE = 6;
	static const int PARAM_MAXBLURDISTANCE = 7;
	static const int PARAM_NEARINTENSITY = 8;
	static const int PARAM_FARINTENSITY = 9;
	static const int PARAM_FARINTENSITYDIST = 10;
	static const int PARAM_NUMBLURPASSES = 11;
	static const int PARAM_BLURSIZE = 12;
	static const int PARAM_COLORAMOUNT = 13;
	static const int PARAM_COLORSSAOAMOUNT = 14;
	
	override int GetPostProcessEffectID()
	{
		return PostProcessEffectType.SSAO;
	}
	
	override string GetDefaultMaterialPath()
	{
		//return "Graphics/Materials/postprocess/hbao"; //"default-default", with no option taken into account (c++)
		GameOptions options = new GameOptions;
		
		ListOptionsAccess loa = ListOptionsAccess.Cast( options.GetOptionByType( OptionAccessType.AT_POSTPROCESS_EFFECTS ) );
		string ret = "";
		switch (loa.GetIndex())
		{
			case POSTPROCESS_OPTION_VALUE_LOW:
				ret = "Graphics/Materials/postprocess/hbao_low";
			break;
			
			case POSTPROCESS_OPTION_VALUE_MEDIUM:
				ret = "Graphics/Materials/postprocess/hbao_medium";
			break;
			
			case POSTPROCESS_OPTION_VALUE_HIGH:
				ret = "Graphics/Materials/postprocess/hbao_high";
			break;
			
			case POSTPROCESS_OPTION_VALUE_HIGHEST:
				ret = "Graphics/Materials/postprocess/hbao_highest";
			break;
		}
		return ret;
	}
	
	//TODO - defaults are taken from c++ directly, instead of material files! Add support for options switching
	override void RegisterMaterialParameters()
	{
		RegisterParameterScalarInt(PARAM_NUMPASSES,"NumPasses",2,2,8);
		RegisterParameterScalarFloat(PARAM_INTENSITY,"Intensity",15.0,0.0,1000.0);
		RegisterParameterScalarFloat(PARAM_RADIUSNEAR,"RadiusNear",60.0,0.0,200.0);
		RegisterParameterScalarFloat(PARAM_RADIUSFAR,"RadiusFar",5.0,0.0,200.0);
		RegisterParameterScalarFloat(PARAM_COSANGLE0,"CosAngle0",0.1,0.0,2.0);
		RegisterParameterScalarFloat(PARAM_COSANGLE1,"CosAngle1",1.0,0.0,2.0);
		RegisterParameterScalarFloat(PARAM_MAXDISTANCE,"MaxDistance",1.5,0.0,10000.0);
		RegisterParameterScalarFloat(PARAM_MAXBLURDISTANCE,"MaxBlurDistance",0.2,0.0,10.0);
		RegisterParameterScalarFloat(PARAM_NEARINTENSITY,"NearIntensity",0.2,0.0,1.0);
		RegisterParameterScalarFloat(PARAM_FARINTENSITY,"FarIntensity",5.0,0.0,20.0);
		RegisterParameterScalarFloat(PARAM_FARINTENSITYDIST,"FarIntensityDistance",20.0,0.0,1000.0);
		RegisterParameterScalarInt(PARAM_NUMBLURPASSES,"NumBlurPasses",1,0,8);
		RegisterParameterScalarInt(PARAM_BLURSIZE,"BlurSize",1.0,1.0,4.0);
		RegisterParameterScalarFloat(PARAM_COLORAMOUNT,"ColorAmount",0.5,0.0,1.0);
		RegisterParameterScalarFloat(PARAM_COLORSSAOAMOUNT,"ColorSsaoAmount",0.25,0.0,1.0);
	}
};