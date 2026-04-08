class ReplaceSoundEventBase
{
	protected ESoundEventType		m_EventType;
	protected EReplaceSoundEventID 	m_ID;
	protected int					m_SoundAnimEventClassID;
	protected int					m_ReplacedSoundAnimID;
	protected string				m_UserString;
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
		m_ReplacedSoundAnimID = -1; //invalidate ID before selecting
		SelectSoundID();
		
		//fail if no valid alternative found. Should return to regular flow.
		if (m_ReplacedSoundAnimID == -1)
			return false;
		
		switch (m_EventType)
		{
			case ESoundEventType.SOUND_COMMON:
				m_Player.ProcessSoundEvent("", m_UserString, m_ReplacedSoundAnimID);	
				break;
			
			case ESoundEventType.SOUND_WEAPON:
				m_Player.ProcessWeaponEvent("", m_UserString, m_ReplacedSoundAnimID);	
				break;
			
			case ESoundEventType.SOUND_VOICE:
				m_Player.ProcessVoiceEvent("", m_UserString, m_ReplacedSoundAnimID);	
				break;
			
			default:
				break;
		}
		
		return true;
	}
	
	protected void SelectSoundID();
}