class ActionPourLiquidCB : ActionContinuousBaseCB
{
	private const float TIME_TO_REPEAT = 0.25;
	
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousQuantityLiquidTransfer(UAQuantityConsumed.DRAIN_LIQUID, TIME_TO_REPEAT, false);
	}
};

class ActionPourLiquid: ActionContinuousBase
{
	void ActionPourLiquid()
	{
		m_CallbackClass = ActionPourLiquidCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_EMPTY_VESSEL;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_EMPTY_VESSEL;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_Text = "#pour_liquid";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
	}
	
	override bool HasProneException()
	{
		return true;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		ItemBase target_item = ItemBase.Cast(target.GetObject());
		
		if ( target_item && item )
		{
				return Liquid.CanTransfer(item,target_item);;
		}
		return false;
	}
	
	override void OnStartAnimationLoop( ActionData action_data )
	{
		if ( !g_Game.IsMultiplayer() || g_Game.IsServer() )
		{
			Bottle_Base vessel_in_hands = Bottle_Base.Cast( action_data.m_Target.GetObject() );
			Param1<bool> play = new Param1<bool>( true );

			g_Game.RPCSingleParam( vessel_in_hands, SoundTypeBottle.POURING, play, true );
		}
	}
	
	override void OnEndAnimationLoop( ActionData action_data )
	{
		if ( !g_Game.IsMultiplayer() || g_Game.IsServer() )
		{
			Bottle_Base target_vessel = Bottle_Base.Cast( action_data.m_Target.GetObject());
			Param1<bool> play = new Param1<bool>( false );

			g_Game.RPCSingleParam( target_vessel, SoundTypeBottle.POURING, play, true );
		}
	}
	
	override void OnEnd( ActionData action_data )
	{
		if ( !g_Game.IsMultiplayer() || g_Game.IsServer() )
		{
			Bottle_Base target_vessel = Bottle_Base.Cast( action_data.m_Target.GetObject());
			Param1<bool> play = new Param1<bool>( false );

			g_Game.RPCSingleParam( target_vessel, SoundTypeBottle.POURING, play, true );
		}
	}
};