//!DepthOfField - PostProcessEffectType.DepthOfField
//TODO - may be just a dummy, since CGame.OverrideDOF function handles script overrides?
class PPEDepthOfField: PPEClassBase
{
	static const int PARAM_DOFLQ = 0;
	static const int PARAM_FOCALDISTANCE = 1;
	static const int PARAM_HYPERFOCAL = 2;
	static const int PARAM_FOCALOFFSET = 3;
	static const int PARAM_BLURFACTOR = 4;
	static const int PARAM_SIMPLEDOF = 5;
	static const int PARAM_SIMPLEHFNEAR = 6;
	static const int PARAM_SIMPLEDOFSIZE = 7;
	static const int PARAM_SIMPLEDOFGAUSS = 8;
	
	override int GetPostProcessEffectID()
	{
		return PostProcessEffectType.DepthOfField;
	}
	
	override string GetDefaultMaterialPath()
	{
		string ret = "Graphics/Materials/postprocess/depthoffieldTest2";
		/*
		GameOptions m_Options = new GameOptions();
		ListOptionsAccess loa = ListOptionsAccess.Cast( m_Options.GetOptionByType( AT_POSTPROCESS_EFFECTS ) );
		
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
		*/
		return ret;
	}
	
	override void RegisterMaterialParameters()
	{
		RegisterParameterScalarBool(PARAM_DOFLQ,"DOFLowQuality",false);
		RegisterParameterScalarFloat(PARAM_FOCALDISTANCE,"FocalDistance",0.1,0.0,1.0);
		RegisterParameterScalarFloat(PARAM_HYPERFOCAL,"HyperFocal",0.85,0.1,100.0);
		RegisterParameterScalarFloat(PARAM_FOCALOFFSET,"FocalOffset",0.0,0.0,1.0);
		RegisterParameterScalarFloat(PARAM_BLURFACTOR,"BlurFactor",4.0,0.0,10.0);
		RegisterParameterScalarBool(PARAM_SIMPLEDOF,"SimpleDOF",false);
		RegisterParameterScalarFloat(PARAM_SIMPLEHFNEAR,"SimpleHyperFocalNear",0.7,0.1,100.0);
		RegisterParameterScalarInt(PARAM_SIMPLEDOFSIZE,"SimpleDOFSize",2.0,1.0,4.0);
		RegisterParameterScalarInt(PARAM_SIMPLEDOFGAUSS,"SimpleDOFGauss",1.0,0.0,4.0);
	}
};