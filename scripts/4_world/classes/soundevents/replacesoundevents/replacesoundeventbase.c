
class ReplaceSoundEventBase
{
	protected ESoundEventType		m_EventType;
	protected EReplaceSoundEventID 	m_ID;
	protected int					m_SoundAnimEventClassID;
	protected int					m_ReplacedSoundAnimID;
	protected PlayerBase 			m_Player;
	
	ESoundEventType GetSoundEventType()
	{
		return m_EventType;
	}
	
	EReplaceSoundEventID GetSoundEventID()
	{
		return m_ID;
	}
					
	int GetSoundAnimEventClassID()
	{
		return m_SoundAnimEventClassID;
	}
				
	void Init(PlayerBase player)
	{
		m_Player = player;
	}
	
	bool Play()
	{	
		SelectSoundID();
		
		switch (m_EventType)
		{
			case ESoundEventType.SOUND_COMMON:
				m_Player.ProcessSoundEvent("","", m_ReplacedSoundAnimID);	
				break;
			
			case ESoundEventType.SOUND_WEAPON:
				m_Player.ProcessWeaponEvent("","", m_ReplacedSoundAnimID);	
				break;
			
			default:
				break;
		}
		
		return true;
	}
	
	protected void SelectSoundID();
}