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
};