class ActionDestroyCombinationLockCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(6.0);
	}
};

class ActionDestroyCombinationLock: ActionContinuousBase
{
	static int CYCLES = 5;
	void ActionDestroyCombinationLock()
	{
		m_CallbackClass = ActionDestroyCombinationLockCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_DISASSEMBLE;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_MEDIUM;
		m_Text = "#destroy_combination_lock";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
		m_ConditionItem = new CCINonRuined;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		Object target_object = target.GetObject();
		string selection = target_object.GetActionComponentName( target.GetComponentIndex() );
		Fence fence = Fence.Cast( target_object );
		
		if ( fence && fence.IsLocked() && selection == "wall_interact" )
		{
			return true;
		}
		
		return false;
	}

	override void OnFinishProgressServer( ActionData action_data )
	{	
		Fence fence = Fence.Cast( action_data.m_Target.GetObject() );
		if ( fence )
		{
			CombinationLock combination_lock = fence.GetCombinationLock();
			if ( combination_lock )
			{
				combination_lock.AddHealth("","",-(combination_lock.GetMaxHealth("","")/CYCLES));
				
				if ( combination_lock.IsDamageDestroyed() )
				{
					combination_lock.UnlockServer( action_data.m_Player, fence );
					GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( combination_lock.DestroyLock, 200, false );
				}
			}
		}
		
		action_data.m_MainItem.DecreaseHealth( UADamageApplied.SAW_LOCK, false );
		
		//soft skills
		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
	}
	
	override string GetAdminLogMessage(ActionData action_data)
	{
		return " destroyed combination lock with " + action_data.m_MainItem.GetDisplayName();
	}
};