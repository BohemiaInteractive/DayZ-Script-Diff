class ActionForceConsumeCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousQuantityEdible(UAQuantityConsumed.DEFAULT, UATimeSpent.DEFAULT);
	}
}

class ActionForceConsume : ActionContinuousBase
{
	protected const float SOUND_REPEAT_INTERVAL = 1.5;
	
	void ActionForceConsume()
	{
		m_CallbackClass = ActionForceConsumeCB;
		m_CommandUID 	= DayZPlayerConstants.CMD_ACTIONFB_FORCEFEED;
		m_StanceMask 	= DayZPlayerConstants.STANCEIDX_ERECT | DayZPlayerConstants.STANCEIDX_CROUCH;
		m_FullBody 		= true;
		m_Text 			= "#feed";
	}

	override void CreateConditionComponents()  
	{
		m_ConditionTarget 	= new CCTMan(UAMaxDistances.DEFAULT);
		m_ConditionItem 	= new CCINonRuined();
	}
		
	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		
		PlayerBase player = PlayerBase.Cast(action_data.m_Target.GetObject());
		if (player)
			g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(PlaySound, SOUND_REPEAT_INTERVAL * 1000, true, player); 
	}
			
	override void OnEndServer(ActionData action_data)
	{	
		super.OnEndServer(action_data);
		
		if (action_data.m_MainItem && (action_data.m_MainItem.GetQuantity() <= 0.01))
		{
			action_data.m_MainItem.SetQuantity(0);
		}
	
		g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(PlaySound);	
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!super.ActionCondition(player, target, item))
			return false;
		
		PlayerBase targetPlayer = PlayerBase.Cast(target.GetObject());
		ConsumeConditionData dta = new ConsumeConditionData(targetPlayer,item);
		if (!targetPlayer || !targetPlayer.CanEatAndDrink() || !targetPlayer.CanConsumeFood(dta) || !item.CanBeConsumed(dta))
			return false;
		
		float angleDiff = Math.AbsFloat(player.GetDirection().VectorToAngles()[0] - targetPlayer.GetDirection().VectorToAngles()[0]);
		if (angleDiff < 135 || angleDiff > 225)	// not in 90 deg cone angle facing the other player
			return false;
						
		if (targetPlayer.GetPerformedActionID() != -1 || targetPlayer.GetActivePrimarySymptomID() == SymptomIDs.SYMPTOM_VOMIT)
			return false;

		return item.GetQuantity() > item.GetQuantityMin();
	}
	
	protected void PlaySound(PlayerBase player)
	{
		if (player)
			player.RequestSoundEventEx(EPlayerSoundEventID.FORCE_FEED);
	}
}
