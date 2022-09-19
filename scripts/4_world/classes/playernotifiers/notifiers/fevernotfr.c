class FeverNotfr: NotifierBase
{
	void FeverNotfr(NotifiersManager manager)
	{
		m_Active = false; 
	}

	override int GetNotifierType()
	{
		return eNotifiers.NTF_FEVERISH;
	}

	override void DisplayBadge()
	{
	//	Print("FEVER");
		////GetVirtualHud().SetStatus(eDisplayElements.DELM_NTFR_FEVER,DELM_LVL_1);
	}

	override void HideBadge()
	{
		
		////GetVirtualHud().SetStatus(eDisplayElements.DELM_NTFR_FEVER,DELM_LVL_0);
	}

};