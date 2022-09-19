class WetnessNotfr: NotifierBase
{
	void WetnessNotfr(NotifiersManager manager)
	{
		m_Active = false;
	}

	override int GetNotifierType()
	{
		return eNotifiers.NTF_WETNESS;
	}

	override void DisplayBadge()
	{
		DisplayElementBadge dis_elm = DisplayElementBadge.Cast(GetVirtualHud().GetElement(eDisplayElements.DELM_BADGE_WET));
		
		if( dis_elm )
		{
			dis_elm.SetLevel(eBadgeLevel.FIRST);
		}
		
	}

	override void HideBadge()
	{
		DisplayElementBadge dis_elm = DisplayElementBadge.Cast(GetVirtualHud().GetElement(eDisplayElements.DELM_BADGE_WET));
		
		if( dis_elm )
		{
			dis_elm.SetLevel(eBadgeLevel.NONE);
		}
	}
};