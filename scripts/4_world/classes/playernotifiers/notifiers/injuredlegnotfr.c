class InjuredLegNotfr: NotifierBase
{
	void InjuredLegNotfr(NotifiersManager manager)
	{
		m_Active = true;
	}

	override int GetNotifierType()
	{
		return eNotifiers.NTF_LEGS;
	}

	
	override void DisplayBadge()
	{
		DisplayElementBadge dis_elm = DisplayElementBadge.Cast(GetVirtualHud().GetElement(eDisplayElements.DELM_BADGE_LEGS));
		
		
		if ( dis_elm )
		{
			float legHealthInverted = 1 - Math.Min(m_Player.GetHealth01("LeftLeg",""), m_Player.GetHealth01("RightLeg",""));
			int level = eBadgeLevel.NONE;
			if (m_Player.GetBrokenLegs() == eBrokenLegs.NO_BROKEN_LEGS)
				level =	DetermineBadgeLevel(legHealthInverted, 0.05, 0.35, 0.65);
			dis_elm.SetLevel(level);
		}
	}

	override void HideBadge()
	{
		DisplayElementBadge dis_elm = DisplayElementBadge.Cast(GetVirtualHud().GetElement(eDisplayElements.DELM_BADGE_LEGS));
		if ( dis_elm )
		{
			dis_elm.SetLevel(eBadgeLevel.NONE);
		}
	}
};