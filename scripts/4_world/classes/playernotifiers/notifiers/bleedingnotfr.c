class BleedingNotfr: NotifierBase
{
	void BleedingNotfr(NotifiersManager manager)
	{
		m_TendencyBufferSize = 8;
		m_Active = false; 
	}

	override int GetNotifierType()
	{
		return eNotifiers.NTF_BLEEDISH;
	}

	override void DisplayBadge()
	{
		DisplayElementBadge dis_elm = DisplayElementBadge.Cast(GetVirtualHud().GetElement(eDisplayElements.DELM_BADGE_BLEEDING));
		
		if( dis_elm )
		{
			dis_elm.SetLevel(eBadgeLevel.FIRST);
		}
	}

	override void HideBadge()
	{
		
		DisplayElementBadge dis_elm = DisplayElementBadge.Cast(GetVirtualHud().GetElement(eDisplayElements.DELM_BADGE_BLEEDING));
		
		if( dis_elm )
		{
			dis_elm.SetLevel(eBadgeLevel.NONE);
		}
	}
};