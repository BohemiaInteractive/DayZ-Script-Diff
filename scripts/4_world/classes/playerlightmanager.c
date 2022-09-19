class ActionTargetLighSource : ActionTarget
{
	bool 	m_Remove;
}

//WIP
class PlayerLightManager
{
	int 									m_SelectedLightSource;
	ref ActionTargetLighSource 				m_LightItemTarget;
	ref array<ref ActionTarget> 			m_ValidLightItems;
	PlayerBase 								m_Player;
	
	void PlayerLightManager(PlayerBase player)
	{
		m_LightItemTarget = null;
		m_ValidLightItems = new array<ref ActionTarget>;
		m_Player = player;
	}
	
	// can be anything, as long as it has appropriate actions for handling lights, see Mich2001Helmet
	void AddLightSource(Object object)
	{
		m_LightItemTarget = new ActionTargetLighSource(object, null, -1, vector.Zero, -1);
		UpdateLightSourceList();
	}
	
	void RemoveLightSource(Object object)
	{
		m_LightItemTarget = new ActionTargetLighSource(object, null, -1, vector.Zero, -1);
		m_LightItemTarget.m_Remove = true;
		UpdateLightSourceList();
	}
	
	void UpdateLightSourceList()
	{
		if ( m_LightItemTarget )
		{
			if ( !m_LightItemTarget.m_Remove )
			{
				m_ValidLightItems.Insert(m_LightItemTarget);
			}
			else
			{
				m_ValidLightItems.RemoveItem(m_LightItemTarget);
			}
		}
	}
	
	ref array<ref ActionTarget> GetLightSourceList()
	{
		return m_ValidLightItems;
	}
	
	void SetSelectedLightSourceIdx(int value)
	{
		m_SelectedLightSource = value;
	}
	
	int GetSelectedLightSourceIdx()
	{
		return m_SelectedLightSource;
	}
	
	void SelectLightSourceTarget(ActionTarget target) //should be ActionTarget?
	{
		int idx = m_ValidLightItems.Find(target);
		SetSelectedLightSourceIdx(idx);
	}
	
	ref ActionTarget GetSelectedLightSourceTarget()
	{
		return m_ValidLightItems.Get(m_SelectedLightSource);
	}
};
