//!Ghost - PostProcessEffectType.Ghost
class PPEGhost: PPEClassBase
{	
	//static const int PARAM_NOISE_TEX = 0;	// changing texture not supported. Default '0xff000000'
	static const int PARAM_NOISE_CHANNEL_WEIGHT = 1;	//!
	static const int PARAM_NOISE_OFFSET_X = 2;			//! 
	static const int PARAM_NOISE_OFFSET_Y = 3;			//! 
	static const int PARAM_NOISE_SCALE_X = 4;			//! 
	static const int PARAM_NOISE_SCALE_Y = 5;			//!
	static const int PARAM_NOISE_REMAP_LO = 6;			//!
	static const int PARAM_NOISE_REMAP_HI = 7;			//!
	
	static const int PARAM_GHOST_OFFSET_X = 8;			//! 
	static const int PARAM_GHOST_OFFSET_Y = 9;			//! 
	static const int PARAM_GHOST_SCALE_X = 10;			//! 
	static const int PARAM_GHOST_SCALE_Y = 11;			//! 
	static const int PARAM_GHOST_COLOR_MOD = 12;		//! RGB = per channel multiplication A = overall color curve (0=linear,1=exponential)
	
	//layering info
	static const int L_1_HMP = 100;
	static const int L_2_HMP = 100;
	static const int L_3_HMP = 100;
	static const int L_4_HMP = 100;
	static const int L_5_HMP = 100;
	static const int L_6_HMP = 100;
	static const int L_7_HMP = 100;
	static const int L_8_HMP = 100;
	static const int L_9_HMP = 100;
	
	override int GetPostProcessEffectID()
	{
		return PostProcessEffectType.Ghost;
	}
	
	override string GetDefaultMaterialPath()
	{
		return "Graphics/Materials/postprocess/ghost";
	}
	
	override void RegisterMaterialParameters()
	{
		//RegisterParameterTexture(PARAM_NOISE_TEX,"NoiseMap","");
		RegisterParameterColor(PARAM_NOISE_CHANNEL_WEIGHT,"NoiseChannelWeight",0,0,0,0);
		
		RegisterParameterScalarFloat(PARAM_NOISE_OFFSET_X, "NoiseOffsetX", 0, 0, 1);
		RegisterParameterScalarFloat(PARAM_NOISE_OFFSET_Y, "NoiseOffsetY", 0, 0, 1);
		RegisterParameterScalarFloat(PARAM_NOISE_SCALE_X, "NoiseScaleX", 1, 0.0001, 100);
		RegisterParameterScalarFloat(PARAM_NOISE_SCALE_Y, "NoiseScaleY", 1, 0.0001, 100);
		RegisterParameterColor(PARAM_NOISE_REMAP_LO,"NoiseRemapLo",0,0,0,0);
		RegisterParameterColor(PARAM_NOISE_REMAP_HI,"NoiseRemapHi",1,1,1,1);
		
		RegisterParameterScalarFloat(PARAM_GHOST_OFFSET_X, "GhostOffsetX", 0, -200, 200);
		RegisterParameterScalarFloat(PARAM_GHOST_OFFSET_Y, "GhostOffsetY", 0, -200, 200);
		RegisterParameterScalarFloat(PARAM_GHOST_SCALE_X, "GhostScaleX", 1, 0.0001, 1);
		RegisterParameterScalarFloat(PARAM_GHOST_SCALE_Y, "GhostScaleY", 1, 0.0001, 1);
		RegisterParameterColor(PARAM_GHOST_COLOR_MOD,"GhostColorMod",1,1,1,0.5);
		
	}
}