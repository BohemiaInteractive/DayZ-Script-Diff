class ActionDebugReciveData : ActionReciveData
{
	ItemBase 	m_Item;
	int			m_DebugActionID;
}

class ActionDebugData : ActionData
{
	int			m_DebugActionID;
}

class ActionDebug : ActionInstantBase
{	
	override bool HasTarget()
	{
		return false;
	}
	
	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTNone;
	}
	
	override ActionData CreateActionData()
	{
		ActionDebugData action_data = new ActionDebugData;
		return action_data;
	}
	
	override bool SetupAction(PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = NULL)
	{
		ActionDebugData action_data_d;
		Class.CastTo( action_data_d, CreateActionData());
		action_data_d.m_Action = this;
		action_data_d.m_Player = player;
		
		if (!GetGame().IsDedicatedServer() )
		{
			ref Param2<ItemBase,int> ndata = Param2<ItemBase,int>.Cast( extra_data );
			action_data_d.m_MainItem = ndata.param1;
			action_data_d.m_DebugActionID = ndata.param2;
		}
		
		ActionReciveData action_recive_data = player.GetActionManager().GetReciveData();
		if ( action_recive_data )
		{
			HandleReciveData(action_recive_data,action_data_d);
		}
		action_data = action_data_d;
		
		return true;
	}
	
	override void WriteToContext (ParamsWriteContext ctx, ActionData action_data)
	{
		ActionDebugData action_data_d;
		Class.CastTo( action_data_d, action_data );
		
		ctx.Write(action_data_d.m_MainItem);
		ctx.Write(action_data_d.m_DebugActionID);
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data )
	{
		if(!action_recive_data)
		{
			action_recive_data = new ActionDebugReciveData;
		}
		
		ActionDebugReciveData action_recive_data_d = ActionDebugReciveData.Cast(action_recive_data);
		
		ItemBase item;
		int debugActionID;
		
		if(!ctx.Read(item))
			return false;
		if(!ctx.Read(debugActionID))
			return false;
		
		action_recive_data_d.m_Item = item;
		action_recive_data_d.m_DebugActionID = debugActionID;
		return true;
	}
	
	override void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		ActionDebugReciveData action_recive_data_d = ActionDebugReciveData.Cast(action_recive_data);
		ActionDebugData action_data_d = ActionDebugData.Cast(action_data);
		
		action_data_d.m_MainItem = action_recive_data_d.m_Item;
		action_data_d.m_DebugActionID = action_recive_data_d.m_DebugActionID;
	}
	
	override void Start(ActionData action_data)
	{
		ActionDebugData action_data_d;
		Class.CastTo( action_data_d, action_data );
		
		action_data_d.m_MainItem.OnAction(action_data_d.m_DebugActionID,action_data_d.m_Player,NULL);
	}
	
	override bool UseAcknowledgment()
	{
		return false;
	}
	
};