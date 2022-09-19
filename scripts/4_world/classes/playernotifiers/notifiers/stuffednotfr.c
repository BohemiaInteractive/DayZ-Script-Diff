class StuffedNotfr: NotifierBase
{
	void StuffedNotfr(NotifiersManager manager)
	{
		m_Active = true; 
	}

	override int GetNotifierType()
	{
		return eNotifiers.NTF_STUFFED;
	}

	override void DisplayBadge()
	{
		float volume = m_Player.m_PlayerStomach.GetStomachVolume();
		eBadgeLevel lvl = DetermineBadgeLevel(volume, PlayerConstants.BT_STOMACH_VOLUME_LVL3, PlayerConstants.BT_STOMACH_VOLUME_LVL3, PlayerConstants.BT_STOMACH_VOLUME_LVL3); 
		DisplayElementBadge dis_elm = DisplayElementBadge.Cast(GetVirtualHud().GetElement(eDisplayElements.DELM_BADGE_STUFFED));
		
		if( dis_elm )
		{
			dis_elm.SetLevel(lvl);
		}
	}

	override void HideBadge()
	{
		DisplayElementBadge dis_elm = DisplayElementBadge.Cast(GetVirtualHud().GetElement(eDisplayElements.DELM_BADGE_STUFFED));
		
		if( dis_elm )
		{
			dis_elm.SetLevel(eBadgeLevel.NONE);
		}
	}
};