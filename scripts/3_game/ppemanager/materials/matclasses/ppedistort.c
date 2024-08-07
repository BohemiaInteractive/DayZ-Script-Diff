//!Distort - PostProcessEffectType.Distort
class PPEDistort: PPEClassBase
{	
	static const int PARAM_DISTORT = 0;				//! Lens distortion magnitude, x > 0 = barrel distortion, x < 0 = pincushion distortion	
	static const int PARAM_CENTERX = 1;				//! Lens center X direction (-1 = left, 0 = center, 1 = right)
	static const int PARAM_CENTERY = 2; 			//! Lens center Y direction (-1 = top, 0 = center, 1 = bottom)
	static const int PARAM_CHROM_ABB = 3; 			//! Maximum value of chromatic abberation, 0 = disable - note that chrom. aberr depends on amount of distortion
	static const int PARAM_CHROM_ABB_SHIFTR = 4; 	//! Chromatic abberation red channel shift magnitude. 0 = none
	static const int PARAM_CHROM_ABB_SHIFTG = 5; 	//! Chromatic abberation green channel shift magnitude. 0 = none, 1 = default
	static const int PARAM_CHROM_ABB_SHIFTB = 6; 	//! Chromatic abberation blue channel shift magnitude. 0 = none, 2 = default
	static const int PARAM_CHROM_COLOR_MOD = 7; 	//! Color modulation effect. (red, green, blue, scale) (0,0,0,0) = default
	static const int PARAM_DISTORT_WEIGHT = 8;		//! Distortion applied to imagine. 1 = all, 0 = none, applied only to ChromAbber
	
	//layering info
	static const int L_0_HMP = 100;
	static const int L_1_HMP = 100;
	static const int L_2_HMP = 100;
	static const int L_3_HMP = 100;
	static const int L_4_HMP = 100;
	static const int L_5_HMP = 100;
	static const int L_6_HMP = 100;
	static const int L_7_HMP = 100;
	
	override int GetPostProcessEffectID()
	{
		return PostProcessEffectType.Distort;
	}
	
	override string GetDefaultMaterialPath()
	{
		return "Graphics/Materials/postprocess/distort";
	}
	
	override void RegisterMaterialParameters()
	{
		RegisterParameterScalarFloat(PARAM_DISTORT, "Distort", 0, -5, 5);
		RegisterParameterScalarFloat(PARAM_CENTERX, "CenterX", 0, -1, 1);
		RegisterParameterScalarFloat(PARAM_CENTERY, "CenterY", 0, -1, 1);
		RegisterParameterScalarFloat(PARAM_CHROM_ABB, "MaxChromAbberation", 0, 0, 1);
		RegisterParameterScalarFloat(PARAM_CHROM_ABB_SHIFTR, "ChromAbberationShiftR", 0, -2, 2);
		RegisterParameterScalarFloat(PARAM_CHROM_ABB_SHIFTG, "ChromAbberationShiftG", 1, -2, 2);
		RegisterParameterScalarFloat(PARAM_CHROM_ABB_SHIFTB, "ChromAbberationShiftB", 2, -2, 2);
		RegisterParameterColor(PARAM_CHROM_COLOR_MOD, "ModulateColor", 0, 0, 0, 0);
		RegisterParameterScalarFloat(PARAM_DISTORT_WEIGHT, "DistortWeight", 1.0, 0.0, 1.0);
	}
}