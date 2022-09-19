class ActionDrainLiquidCB : ActionContinuousBaseCB
{
	private const float TIME_TO_REPEAT = 0.25;
	
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousQuantityLiquidTransfer(UAQuantityConsumed.DRAIN_LIQUID, TIME_TO_REPEAT, true);
	}
};

class ActionDrainLiquid: ActionContinuousBase
{
	void ActionDrainLiquid()
	{
		m_CallbackClass = ActionDrainLiquidCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_EMPTY_VESSEL;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_EMPTY_VESSEL;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_Text = "#drain_liquid";
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
			return Liquid.CanTransfer(target_item,item);
		}
		return false;
	}

	override void OnFinishProgressServer( ActionData action_data )
	{
		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
	}
	
	
	override void OnStartAnimationLoop( ActionData action_data )
	{
		if ( !GetGame().IsMultiplayer() || GetGame().IsServer() )
		{
			Bottle_Base vessel_in_hands = Bottle_Base.Cast( action_data.m_Target.GetObject() );
			Param1<bool> play = new Param1<bool>( true );

			GetGame().RPCSingleParam( vessel_in_hands, SoundTypeBottle.EMPTYING, play, true );
		}
	}
	
	override void OnEndAnimationLoop( ActionData action_data )
	{
		if ( !GetGame().IsMultiplayer() || GetGame().IsServer() )
		{
			Bottle_Base target_vessel = Bottle_Base.Cast( action_data.m_Target.GetObject());
			Param1<bool> play = new Param1<bool>( false );
			GetGame().RPCSingleParam( target_vessel, SoundTypeBottle.EMPTYING, play, true );
		}
	}
};