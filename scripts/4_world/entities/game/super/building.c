class House : BuildingBase
{
	void House()
	{
	}
}

class BuildingBase : Building
{
	static ref map<typename, ref TInputActionMap> m_BuildingTypeActionsMap = new map<typename, ref TInputActionMap>;
	TInputActionMap m_InputActionMap;
	bool	m_ActionsInitialize;
	
	void BuildingBase()
	{
		m_ActionsInitialize = false;
	}
	
	void InitializeActions()
	{
		m_InputActionMap = m_BuildingTypeActionsMap.Get( this.Type() );
		if (!m_InputActionMap)
		{
			TInputActionMap iam = new TInputActionMap;
			m_InputActionMap = iam;
			SetActions();
			m_BuildingTypeActionsMap.Insert(this.Type(), m_InputActionMap);
		}
	}
	
	override void GetActions(typename action_input_type, out array<ActionBase_Basic> actions)
	{
		if (!m_ActionsInitialize)
		{
			m_ActionsInitialize = true;
			InitializeActions();
		}
		
		actions = m_InputActionMap.Get(action_input_type);
	}
	
	void SetActions()
	{
		//AddAction();
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
		ref array<ActionBase_Basic> action_array = m_InputActionMap.Get( ai );
		
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
		ref array<ActionBase_Basic> action_array = m_InputActionMap.Get( ai );
		
		if (action_array)
		{
			action_array.RemoveItem(action);
		}
	}
};

typedef House BuildingSuper;