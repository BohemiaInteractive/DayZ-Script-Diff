class GorkaHelmet : HelmetBase
{
	override ECachedEquipmentItemCategory GetCachedEquipmentCategory()
	{
		return ECachedEquipmentItemCategory.MASK;
	}

	override float GetProtectionLevel(int type, bool consider_filter = false, int system = 0)
	{
		if (IsDamageDestroyed())
			return 0.0;

		if (GetInventory().FindAttachment(GorkaHelmetVisor.SLOT_ID))
		{	
			if (type == DEF_DUST_PARTICLE_EYES)
				return 1.0;
		}
		
		return super.GetProtectionLevel(type, consider_filter, system);
	}

	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		GetInventory().CreateInInventory("GorkaHelmetVisor");
	}
}