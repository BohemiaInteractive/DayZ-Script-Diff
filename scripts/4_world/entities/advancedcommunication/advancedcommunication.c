class AdvancedCommunication extends EntityAI
{
	static ref map<typename, ref TInputActionMap> m_AdvComTypeActionsMap = new map<typename, ref TInputActionMap>;
	TInputActionMap m_InputActionMap;
	bool	m_ActionsInitialize;
	
	void AdvancedCommunication()
	{
		if (!GetGame().IsDedicatedServer())
		{
			if(GetGame().GetPlayer())
			{
				m_ActionsInitialize = false;
			}
		}	
	}

	//HUD
	/*
	protected Hud GetHud( PlayerBase player )
	{
		if ( !player ) 
		{
			return NULL;
		}
		
		return player.m_Hud;
	}

	void DisplayRadioInfo( string message, PlayerBase player )
	{
		Hud hud;
		if ( player )
		{
			hud = GetHud( player );
		}
		
		if ( hud ) 
		{	
			hud.SetWalkieTalkieText( message );
			hud.ShowWalkieTalkie( 3 );
		}
	}
	*/
	
	//control transmitter via user actions
	void TurnOnTransmitter()
	{
		GetCompEM().SwitchOn();
	}
	
	void TurnOffTransmitter()
	{
		GetCompEM().SwitchOff();
	}
	
	void InitializeActions()
	{
		m_InputActionMap = m_AdvComTypeActionsMap.Get( this.Type() );
		if(!m_InputActionMap)
		{
			TInputActionMap iam = new TInputActionMap;
			m_InputActionMap = iam;
			SetActions();
			m_AdvComTypeActionsMap.Insert(this.Type(), m_InputActionMap);
		}
	}
	
	override void GetActions(typename action_input_type, out array<ActionBase_Basic> actions)
	{
		if(!m_ActionsInitialize)
		{
			m_ActionsInitialize = true;
			InitializeActions();
		}
		
		actions = m_InputActionMap.Get(action_input_type);
	}
	
	void SetActions()
	{
		AddAction(ActionTurnOnTransmitterOnGround);
		AddAction(ActionTurnOffTransmitterOnGround);
		AddAction(ActionDetachPowerSourceFromPanel);
	}
	
	void AddAction(typename actionName)
	{
		ActionBase action = ActionManagerBase.GetAction(actionName);

		if (!action)
		{
			Debug.LogError("Action " + actionName + " dosn't exist!");
			return;
		}		
		
		typename ai = action.GetInputType();
		if (!ai)
		{
			m_ActionsInitialize = false;
			return;
		}
		
		array<ActionBase_Basic> action_array = m_InputActionMap.Get( ai );
		
		if (!action_array)
		{
			action_array = new array<ActionBase_Basic>;
			m_InputActionMap.Insert(ai, action_array);
		}
		
		if ( LogManager.IsActionLogEnable() )
		{
			Debug.ActionLog(action.ToString() + " -> " + ai, this.ToString() , "n/a", "Add action" );
		}
		action_array.Insert(action);
	}
	
	void RemoveAction(typename actionName)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		ActionBase action = player.GetActionManager().GetAction(actionName);
		typename ai = action.GetInputType();
		array<ActionBase_Basic> action_array = m_InputActionMap.Get( ai );
		
		if (action_array)
		{
			action_array.RemoveItem(action);
		}
	}
}

class PASReceiver extends AdvancedCommunication
{
	override bool IsInventoryVisible()
	{
		return false;
	}
};

class PASBroadcaster extends AdvancedCommunication
{
	proto native void SwitchOn(bool onOff);
	proto native bool IsOn();
};

class StaticTransmitter extends AdvancedCommunication
{
	proto native void SwitchOn(bool onOff);
	proto native bool IsOn();
	proto native void SetNextChannel();
	proto native void SetPrevChannel();
	proto native float GetTunedFrequency();
	proto native void EnableBroadcast(bool state);
	proto native void EnableReceive(bool state);
	proto native bool IsBroadcasting();
	proto native bool IsReceiving();
	proto native int GetTunedFrequencyIndex();
	proto native void SetFrequencyByIndex(int index);
};