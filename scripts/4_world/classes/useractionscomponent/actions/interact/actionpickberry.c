class ActionPickBerryCB : ActionInteractLoopBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAInteractLoop(UATimeSpent.DEFAULT_PICK);
	}
	/*override void OnAnimationEvent(int pEventID)	
	{
		Print("ActionInteractLoopBase.c | OnAnimationEvent | OnAnimationEvent called");
		if ( !m_Interrupted && pEventID == UA_ANIM_EVENT ) 
		{
			SetCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
			m_State = UA_FINISHED;
			m_ActionData.Do(this,m_State,m_ActionComponent,m_Player,m_Target,m_Item);
		}	
	}*/
};

class ActionPickBerry: ActionInteractLoopBase
{
	void ActionPickBerry()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_MEDIUM;
		
		m_CallbackClass = ActionPickBerryCB;
		m_Text = "#harvest";
	}

	override void CreateConditionComponents()  
	{
		m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
		m_ConditionItem = new CCINone;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		Object targetObject = target.GetObject();
		if ( player && targetObject && targetObject.IsWoodBase() ) 
		{
			return true;
		}
		return false;
	}

	//deprecated
	////override void OnCompleteServer( ActionData action_data )
	//{
		/*Object targetObject = action_data.m_Target.GetObject();
		WoodBase ntarget = WoodBase.Cast( targetObject );
		string drop_name = ntarget.GetMaterial(NULL);
		float drop_quantity = ntarget.GetAmountOfMaterialPerDrop(action_data.m_MainItem);
		
		targetObject.DecreaseHealth( "", "", (1 / Math.Max(1,ntarget.GetAmountOfDrops(action_data.m_MainItem)))*100, true );
				
		ItemBase drop = ItemBase.Cast( GetGame().CreateObject(drop_name, action_data.m_Player.GetPosition(), false) );
		drop.SetQuantity(drop_quantity, false);

		action_data.m_Player.GetSoftSkillManager().AddSpecialty( m_SpecialtyWeight );*/
	//}
};