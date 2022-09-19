class HeartbeatNotfr: NotifierBase
{
	void HeartbeatNotfr(NotifiersManager manager)
	{
		m_Active = false; 
	}

	override int GetNotifierType()
	{
		return eNotifiers.NTF_HEARTBEAT;
	}

	override void DisplayBadge()
	{
		
		DisplayElementBadge dis_elm = DisplayElementBadge.Cast(GetVirtualHud().GetElement(eDisplayElements.DELM_BADGE_HEARTBEAT));
		
		if( dis_elm )
		{
			if (m_Player.GetPulseType() == EPulseType.REGULAR) 
			{
				dis_elm.SetLevel(eBadgeLevel.FIRST);
			}
			else
			{
				dis_elm.SetLevel(eBadgeLevel.SECOND);
			}
		}
	}

	override void HideBadge()
	{
		
		DisplayElementBadge dis_elm = DisplayElementBadge.Cast(GetVirtualHud().GetElement(eDisplayElements.DELM_BADGE_HEARTBEAT));
		
		if( dis_elm )
		{
			dis_elm.SetLevel(eBadgeLevel.NONE);
		}
	}
};