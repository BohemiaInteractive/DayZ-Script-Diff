class FracturedLegNotfr: NotifierBase
{
	void FracturedLegNotfr(NotifiersManager manager)
	{
		m_Active = false;
	}

	override int GetNotifierType()
	{
		return eNotifiers.NTF_FRACTURE;
	}

	override void SetActive( bool state )
	{
		if (state == true)
		{
			DisplayBadge();
		}
		else
		{
			HideBadge();
		}
			
	}
	
	override void DisplayBadge()
	{
		DisplayElementBadge dis_elm = DisplayElementBadge.Cast(GetVirtualHud().GetElement(eDisplayElements.DELM_BADGE_FRACTURE));
		if( dis_elm )
		{
			dis_elm.SetLevel(eBadgeLevel.FIRST);
		}
		
	}

	override void HideBadge()
	{
		DisplayElementBadge dis_elm = DisplayElementBadge.Cast(GetVirtualHud().GetElement(eDisplayElements.DELM_BADGE_FRACTURE));
		if( dis_elm )
		{
			dis_elm.SetLevel(eBadgeLevel.NONE);
		}
	}
};