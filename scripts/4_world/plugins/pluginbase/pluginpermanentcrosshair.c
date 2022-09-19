class PluginPermanentCrossHair extends PluginBase
{
	private Hud m_Hud;

	void SwitchPermanentCrossHair(bool state)
	{
		if (!m_Hud)
			m_Hud = GetGame().GetMission().GetHud();
		m_Hud.SetPermanentCrossHair(state);
	}
}