class ActionWringClothesCB : ActionContinuousBaseCB
{
	protected const float QUANTITY_WRINGLED_PER_SECOND = 0.02;
	
	override void CreateActionComponent()
	{
		//m_ActionData.m_ActionComponent = new CAContinuousWringClothes(QUANTITY_WRINGLED_PER_SECOND, UATimeSpent.WASH_HANDS);
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(UATimeSpent.WASH_HANDS);
	}
};

class ActionWringClothes: ActionContinuousBase
{
	void ActionWringClothes()
	{
		m_CallbackClass = ActionWringClothesCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_WRING;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_LOW;
		m_Text = "#wring_clothes";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTSelf;
	}

	override bool HasTarget()
	{
		return false;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if (player.IsInWater()) return false;
		
		//! wet+ items (so they will stay damp after wringing)
		if ( item && item.GetWet() >= GameConstants.STATE_WET )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	/*override void OnExecuteServer( ActionData action_data )
	{
		Param1<float> nacdata = Param1<float>.Cast( action_data.m_ActionComponent.GetACData() );
		float delta = nacdata.param1;
		action_data.m_MainItem.AddWet( -delta );
	}*/

	override void OnFinishProgressServer( ActionData action_data )
	{
		/*Param1<float> nacdata = Param1<float>.Cast( action_data.m_ActionComponent.GetACData() );
		float delta = nacdata.param1;
		action_data.m_MainItem.AddWet( -delta );*/
		float wetness = action_data.m_MainItem.GetWet();
		float wet_change;
		
		if (wetness >= GameConstants.STATE_DRENCHED)
		{
			wet_change = Math.RandomFloat(GameConstants.STATE_SOAKING_WET,GameConstants.STATE_DRENCHED);
		}
		else if (wetness >= GameConstants.STATE_SOAKING_WET)
		{
			wet_change = Math.RandomFloat(GameConstants.STATE_WET,GameConstants.STATE_SOAKING_WET);
		}
		else if (wetness >= GameConstants.STATE_WET)
		{
			wet_change = Math.RandomFloat(GameConstants.STATE_DAMP,GameConstants.STATE_WET);
		}
		
		//Print(wet_change);
		action_data.m_MainItem.SetWet(wet_change);
		
		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
	}

	/*override void OnEndServer( ActionData action_data )
	{
		if (action_data.m_MainItem && action_data.m_MainItem.GetWet() < GameConstants.STATE_DAMP)
		{
			action_data.m_MainItem.SetWet(Math.RandomFloat(GameConstants.STATE_DAMP,GameConstants.STATE_WET));
		}
	}*/
};