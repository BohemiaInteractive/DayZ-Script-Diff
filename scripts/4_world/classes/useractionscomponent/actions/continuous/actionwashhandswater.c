class ActionWashHandsWaterCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(UATimeSpent.WASH_HANDS);
	}
};

class ActionWashHandsWater: ActionContinuousBase
{
	void ActionWashHandsWater()
	{
		m_CallbackClass 	= ActionWashHandsWaterCB;
		m_CommandUID		= DayZPlayerConstants.CMD_ACTIONFB_WASHHANDSPOND;
		m_FullBody			= true;
		m_StanceMask		= DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#wash_hands";
	}

	override void CreateConditionComponents()  
	{		
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTWaterSurface(UAMaxDistances.DEFAULT, UAWaterType.ALL);
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		// Other conditions are in CCTWaterSurface
		return ( GetGame().IsMultiplayer() && GetGame().IsServer() ) || ( player.HasBloodyHands() && !player.GetItemInHands() );
	}
	
	override void OnFinishProgressServer( ActionData action_data )
	{
		PluginLifespan module_lifespan = PluginLifespan.Cast( GetPlugin( PluginLifespan ) );
		module_lifespan.UpdateBloodyHandsVisibility( action_data.m_Player, false );
	}
};