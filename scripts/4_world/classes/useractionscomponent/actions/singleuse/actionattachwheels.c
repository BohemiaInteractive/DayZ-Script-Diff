// TODO: REMOVE -> Attach action handle it
class ActionAttachWheels: ActionAttach
{
	int m_wheel_hub;
	
	void ActionAttachWheels()
	{
		m_wheel_hub = -1;
		m_Text = "#attach";
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		Object targetObject = target.GetObject();
		
		string test = player.GetDisplayName();
		string test2 = item.GetType();
		string test3 = targetObject.GetDisplayName();
		
		if ( player && targetObject && item )
		{
			int componentIndex = target.GetComponentIndex();
			if( targetObject.IsActionComponentPartOfSelection(componentIndex, "wheel_1_1") )
			{
				Print( "Predni leve" );
				m_wheel_hub = 0;
				return true;
			}

			if( targetObject.IsActionComponentPartOfSelection(componentIndex, "wheel_1_2") )
			{
				Print( "zadni leve" );
				m_wheel_hub = 1;
				return true;
			}

			if( targetObject.IsActionComponentPartOfSelection(componentIndex, "wheel_1_3") )
			{
				Print( "prostredni leve" );
				m_wheel_hub = 2;
				return true;
			}

			if( targetObject.IsActionComponentPartOfSelection(componentIndex, "wheel_1_4") )
			{
				Print( "druhe predni leve" );
				m_wheel_hub = 3;
				return true;
			}
			
			if( targetObject.IsActionComponentPartOfSelection(componentIndex, "wheel_2_1") )
			{
				Print( "Predni prave" );
				m_wheel_hub = 4;
				return true;
			}

			if( targetObject.IsActionComponentPartOfSelection(componentIndex, "wheel_2_2") )
			{
				Print( "zadni prave" );
				m_wheel_hub = 5;
				return true;
			}

			if( targetObject.IsActionComponentPartOfSelection(componentIndex, "wheel_2_3") )
			{
				Print( "prostredni prave" );
				m_wheel_hub = 6;
				return true;
			}

			if( targetObject.IsActionComponentPartOfSelection(componentIndex, "wheel_2_4") )
			{
				Print( "druhe predni prave" );
				m_wheel_hub = 7;
				return true;
			}		
		}
		
 		return false;
	}

	override void OnExecuteServer( ActionData action_data )
	{
		EntityAI target_entity;
		Class.CastTo(target_entity,  action_data.m_Target.GetObject() ); // cast to ItemBase
		int slot_index	= action_data.m_MainItem.GetInventory().GetSlotId(0);
		int test	= m_wheel_hub;
		if (!target_entity.LocalTakeEntityAsAttachmentEx( action_data.m_MainItem, slot_index ))
			target_entity.LocalTakeEntityAsAttachment( action_data.m_MainItem );
	}
	override void OnExecuteClient( ActionData action_data )
	{
		EntityAI target_entity;
		Class.CastTo(target_entity,  action_data.m_Target.GetObject() ); // cast to ItemBase
		int slot_index	= action_data.m_MainItem.GetInventory().GetSlotId(0);
		int test	= m_wheel_hub;
		if (!target_entity.LocalTakeEntityAsAttachmentEx( action_data.m_MainItem, slot_index ))
			target_entity.LocalTakeEntityAsAttachment( action_data.m_MainItem );
	}
};