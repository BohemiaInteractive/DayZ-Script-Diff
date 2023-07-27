class DirtBikeHelmet_ColorBase extends HelmetBase
{
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		EntityAI entity;
		if ( Class.CastTo(entity, this) )
		{
			entity.GetInventory().CreateInInventory( "DirtBikeHelmet_Visor" );
			entity.GetInventory().CreateInInventory( "DirtBikeHelmet_Mouthguard" );
		}
	}
}

class DirtBikeHelmet_Green extends DirtBikeHelmet_ColorBase {};
class DirtBikeHelmet_Chernarus extends DirtBikeHelmet_ColorBase {};
class DirtBikeHelmet_Khaki extends DirtBikeHelmet_ColorBase {};
class DirtBikeHelmet_Police extends DirtBikeHelmet_ColorBase {};
class DirtBikeHelmet_Red extends DirtBikeHelmet_ColorBase {};
class DirtBikeHelmet_Black extends DirtBikeHelmet_ColorBase {};
class DirtBikeHelmet_Blue extends DirtBikeHelmet_ColorBase {};

class DirtBikeHelmet_Visor : ClothingBase {};
class DirtBikeHelmet_Mouthguard : ClothingBase
{
	override protected void InitGlobalExclusionValues()
	{
		super.InitGlobalExclusionValues();
		
		AddSingleExclusionValueGlobal(EAttExclusions.EXCLUSION_MASK_2);
		
		AddSingleExclusionValueGlobal(EAttExclusions.SHAVING_HEADGEAR_ATT_0);
	}
};
