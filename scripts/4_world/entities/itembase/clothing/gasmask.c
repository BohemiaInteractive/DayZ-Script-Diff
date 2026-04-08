class GasMask extends MaskBase
{
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
		if (g_Game.IsServer())
		{
			if( newLevel == GameConstants.STATE_RUINED )
			{
				SetQuantity(0);
			}
		}
	}
}
