class AirborneMask extends MaskBase
{
	override ECachedEquipmentItemCategory GetCachedEquipmentCategory()
	{
		return ECachedEquipmentItemCategory.MASK|ECachedEquipmentItemCategory.EYEWEAR;
	}

	override bool IsObstructingVoice()
	{
		return true;
	}
	
	override int GetVoiceEffect()
	{
		return VoiceEffectObstruction;
	}
	
	override float GetProtectionLevel(int type, bool consider_filter = false, int system = 0)
	{
		if (IsDamageDestroyed())
			return 0.0;

		ItemBase filter = ItemBase.Cast(FindAttachmentBySlotName("GasMaskFilter"));
		if (filter)
		{
			if (type == DEF_DUST_PARTICLE_EYES)
				return 1.0;

			return filter.GetProtectionLevel(type, false, system);
		}

		return super.GetProtectionLevel(type, consider_filter, system);
	}
}
