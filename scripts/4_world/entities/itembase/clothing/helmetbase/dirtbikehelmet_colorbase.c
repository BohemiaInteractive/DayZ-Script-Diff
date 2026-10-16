class DirtBikeHelmet_ColorBase : HelmetBase
{
	override ECachedEquipmentItemCategory GetCachedEquipmentCategory()
	{
		return ECachedEquipmentItemCategory.MASK;
	}

	override float GetProtectionLevel(int type, bool consider_filter = false, int system = 0)
	{
		if (IsDamageDestroyed())
			return 0.0;

		if (GetInventory().FindAttachment(DirtBikeHelmet_Mouthguard.SLOT_ID))
		{	
			if (type == DEF_DUST_PARTICLE_BREATH)
				return 0.65;
		}
		
		return super.GetProtectionLevel(type, consider_filter, system);
	}

	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		GetInventory().CreateInInventory("DirtBikeHelmet_Visor");
		GetInventory().CreateInInventory("DirtBikeHelmet_Mouthguard");
	}
}

class DirtBikeHelmet_Green : DirtBikeHelmet_ColorBase {};
class DirtBikeHelmet_Chernarus : DirtBikeHelmet_ColorBase {};
class DirtBikeHelmet_Khaki : DirtBikeHelmet_ColorBase {};
class DirtBikeHelmet_Police : DirtBikeHelmet_ColorBase {};
class DirtBikeHelmet_Red : DirtBikeHelmet_ColorBase {};
class DirtBikeHelmet_Black : DirtBikeHelmet_ColorBase {};
class DirtBikeHelmet_Blue : DirtBikeHelmet_ColorBase {};

class DirtBikeHelmet_Visor : ClothingBase {};
class DirtBikeHelmet_Mouthguard : ClothingBase
{
	static int SLOT_ID = InventorySlots.GetSlotIdFromString("DBHelmetMouth");

	override protected void InitGlobalExclusionValues()
	{
		super.InitGlobalExclusionValues();
		
		AddSingleExclusionValueGlobal(EAttExclusions.EXCLUSION_MASK_2);
		
		AddSingleExclusionValueGlobal(EAttExclusions.SHAVING_HEADGEAR_ATT_0);
	}
};
