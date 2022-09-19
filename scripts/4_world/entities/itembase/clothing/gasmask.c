class GasMask extends MaskBase
{
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if (!super.CanPutAsAttachment(parent)) {return false;}
		
		Clothing headgear = Clothing.Cast(parent.FindAttachmentBySlotName("Headgear"));
		if ( headgear && headgear.ConfigGetBool("noMask") )
		{
			return false;
		}
		
		Clothing eyewear = Clothing.Cast(parent.FindAttachmentBySlotName("Eyewear"));
		if ( eyewear && SportGlasses_ColorBase.Cast(eyewear) )
		{
			return false;
		}
		
		return true;
	}
	
	override bool IsObstructingVoice()
	{
		return true;
	}
	
	override int GetVoiceEffect()
	{
		return VoiceEffectObstruction;
	}
	
	override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		if (GetGame().IsServer())
		{
			if( newLevel == GameConstants.STATE_RUINED )
			{
				SetQuantity(0);
			}
		}
	}
}
