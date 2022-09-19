class EmoteBase
{
	protected PlayerBase m_Player; //useful for various conditions in child classes
	protected int m_ID = -1;
	protected string m_InputActionName = "";
	protected int m_StanceMaskAdditive = 0;
	protected int m_StanceMaskFullbody = 0;
	protected int m_AdditiveCallbackUID = 0;
	protected int m_FullbodyCallbackUID = 0;
	protected bool m_HideItemInHands = false;
	
	bool EmoteCondition(int stancemask)
	{
		return true;
	}
	
	//! Checks for valid stance mask
	bool EmoteFBStanceCheck(int stancemask)
	{
		int stanceIdx = DayZPlayerUtils.ConvertStanceMaskToStanceIdx(stancemask);
		
		if (!m_Player)
		{
			ErrorEx("No player for 'PlayerCanChangeStance'");
			return false;
		}
		
		if (stanceIdx != -1 && !DayZPlayerUtils.PlayerCanChangeStance(m_Player, stanceIdx))
			return false;
		
		return true;
	}
	
	bool DetermineOverride(out int callback_ID, out int stancemask, out bool is_fullbody)
	{
		return false;
	}
	
	void OnBeforeStandardCallbackCreated(int callback_ID, int stancemask, bool is_fullbody)
	{
	}
	
	bool EmoteStartOverride(typename callbacktype, int id, int mask, bool fullbody)
	{
		return false;
	}
	
	void SetOwnerPlayer(PlayerBase player)
	{
		m_Player = player;
	}
	
///////////
//Getters//
///////////
	PlayerBase GetOwnerPlayer()
	{
		return m_Player;
	}
	
	int GetID()
	{
		return m_ID;
	}
	
	string GetInputActionName()
	{
		return m_InputActionName;
	}
	
	int GetStanceMaskAdditive()
	{
		return m_StanceMaskAdditive;
	}
	
	int GetStanceMaskFullbody()
	{
		return m_StanceMaskFullbody;
	}
	
	int GetAdditiveCallbackUID()
	{
		return m_AdditiveCallbackUID;
	}
	
	int GetFullbodyCallbackUID()
	{
		return m_FullbodyCallbackUID;
	}
	
	bool GetHideItemInHands()
	{
		return m_HideItemInHands;
	}
	/*
	protected int m_StanceMaskAdditive = 0;
	protected int m_StanceMaskFullbody = 0;
	*/
}
