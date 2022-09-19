//!Rain - PostProcessEffectType.Rain
class PPERain: PPEClassBase
{
	static const int PARAM_DROPDISTANCE = 0;
	static const int PARAM_DROPSIZEX = 1;
	static const int PARAM_DROPSIZEY = 2;
	static const int PARAM_RAINFOGGINESS = 3;
	static const int PARAM_RAINDENSITY = 4;
	static const int PARAM_FOGCOLORMULT = 5;
	static const int PARAM_BCKGRNDCOLORMULT = 6;
	static const int PARAM_REFRACTIONSCALE = 7;
	static const int PARAM_SUNVISIBLE = 8;
	static const int PARAM_GODRAYINTNEAR = 9;
	static const int PARAM_GODRAYINTFAR = 10;
	static const int PARAM_RIPPLESBLEND = 11;
	static const int PARAM_RIPPLESDIST = 12;
	static const int PARAM_SPEED = 13;
	static const int PARAM_DISTANTSPEED = 14;
	static const int PARAM_WINDAFFECTSPEED = 15;
	static const int PARAM_MINDIVERGENCE = 16;
	static const int PARAM_MAXDIVERGENCE = 17;
	static const int PARAM_WINDSPEEDFORMAXDIV = 18;
	/*
	static const int PARAM_DROPREFRACTIONTEX = 19;
	static const int PARAM_DISTANTTEX = 20;
	static const int PARAM_ROTNOISEMAP = 21;
	static const int PARAM_RIPPLEMAP = 22;
	static const int PARAM_MOVMAP = 23;
	static const int PARAM_MOVNOISEMAP = 24;
	static const int PARAM_SUNMASKMAT = 25;
	*/
	
	override int GetPostProcessEffectID()
	{
		return PostProcessEffectType.Rain;
	}
	
	override string GetDefaultMaterialPath()
	{
		return "Graphics/Materials/postprocess/rain";
	}
	
	override void RegisterMaterialParameters()
	{
		RegisterParameterScalarFloat(PARAM_DROPDISTANCE,"DropDistance",1.45,0.0,100.0);
		RegisterParameterScalarFloat(PARAM_DROPSIZEX,"DropSizeX",0.003,0.0,100.0);
		RegisterParameterScalarFloat(PARAM_DROPSIZEY,"DropSizeY",0.075,0.0,100.0);
		RegisterParameterScalarFloat(PARAM_RAINFOGGINESS,"RainFogginess",0.5,0.0,5.0);
		RegisterParameterScalarFloat(PARAM_RAINDENSITY,"RainDensity",1.0,0.0,5.0);
		RegisterParameterScalarFloat(PARAM_FOGCOLORMULT,"FogColorMultiplier",0.5,0.0,5.0);
		RegisterParameterScalarFloat(PARAM_BCKGRNDCOLORMULT,"BackgroundColorMultiplier",1.1,0.0,5.0);
		RegisterParameterScalarFloat(PARAM_REFRACTIONSCALE,"RefractionScale",0.01,0.0,1.0);
		RegisterParameterScalarFloat(PARAM_SUNVISIBLE,"SunVisible",1.0,0.0,1.0);
		RegisterParameterScalarFloat(PARAM_GODRAYINTNEAR,"GodRayIntensityNear",1.5,0.0,50.0);
		RegisterParameterScalarFloat(PARAM_GODRAYINTFAR,"GodRayIntensityFar",1.2,0.0,50.0);
		RegisterParameterScalarFloat(PARAM_RIPPLESBLEND,"RipplesBlend",50.0,0.0,250.0);
		RegisterParameterScalarFloat(PARAM_RIPPLESDIST,"RipplesDist",100.0,0.0,250.0);
		RegisterParameterScalarFloat(PARAM_SPEED,"Speed",10.0,0.0,50.0);
		RegisterParameterScalarFloat(PARAM_DISTANTSPEED,"DistantSpeed",16.0,0.0,100.0);
		RegisterParameterScalarFloat(PARAM_WINDAFFECTSPEED,"WindAffectsSpeed",0.02,0.0,10.0);
		RegisterParameterScalarFloat(PARAM_MINDIVERGENCE,"MinDivergence",0.005,0.0,1.0);
		RegisterParameterScalarFloat(PARAM_MAXDIVERGENCE,"MaxDivergence",0.03,0.0,1.0);
		RegisterParameterScalarFloat(PARAM_WINDSPEEDFORMAXDIV,"WindSpeedForMaxDivergence",10.0,0.0,100.0);
		
		/*RegisterParameterTexture(PARAM_DROPREFRACTIONTEX,"DropRefractionTex","{3D95A4E49ACDDE5}graphics/textures/postprocess/drop_refraction.edds");
		RegisterParameterTexture(PARAM_DISTANTTEX,"DistantTex","{3435F57626D5A6DB}graphics/textures/postprocess/raindistantmask.edds");
		RegisterParameterTexture(PARAM_ROTNOISEMAP,"RotationNoiseMap","{7B4F4B4EE2566E84}graphics/textures/postprocess/noise.edds");
		RegisterParameterTexture(PARAM_RIPPLEMAP,"RipplesMap","{59E25B3D2EB94B}graphics/textures/postprocess/ripple.edds");
		RegisterParameterTexture(PARAM_MOVMAP,"MovableMap","{EBF63DBBED7D8}graphics/textures/postprocess/rainripplesmovable.edds");
		RegisterParameterTexture(PARAM_MOVNOISEMAP,"MovableNoiseMap","{2E275BD75961FA54}graphics/textures/postprocess/rainripplesmovablenoise.edds");*/
		
		//RegisterParameterResource(PARAM_SUNMASKMAT,"SunMaskMat","{FAF56DD689E895BE}Graphics/Materials/postprocess/godrayssunmask.emat");
	}
}