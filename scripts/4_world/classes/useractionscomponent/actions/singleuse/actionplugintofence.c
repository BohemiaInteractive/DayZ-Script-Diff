class ActionPlugIntoFence: ActionSingleUseBase
{
	void ActionPlugIntoFence()
	{
		m_Text = "#plug_in";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		Object targetObject = target.GetObject();
		
		if ( targetObject  &&  item  &&  targetObject.IsInherited(BaseBuildingBase))
		{
			BaseBuildingBase target_BBB = BaseBuildingBase.Cast( targetObject );
			
			BarbedWire bw = BarbedWire.Cast( target_BBB.GetAttachmentByType(BarbedWire) );
			
			if ( bw  &&  bw.GetCompEM().CanReceivePlugFrom(item) )
			{
				return true;
			}
		}
		
		return false;
	}

	override void OnExecuteServer( ActionData action_data )
	{	
		Object targetObject = action_data.m_Target.GetObject();
		ItemBase target_IB = ItemBase.Cast( targetObject );
		BarbedWire bw = BarbedWire.Cast( target_IB.GetAttachmentByType(BarbedWire) );
		action_data.m_MainItem.GetCompEM().PlugThisInto(bw);
		
		if ( !action_data.m_Player.IsPlacingServer() )
		{
			//action_data.m_Player.TogglePlacing( action_data.m_Target, action_data.m_MainItem );
		}
	}
};