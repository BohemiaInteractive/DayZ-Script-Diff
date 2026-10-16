class ActionWetClothingInHandsWaterCB : ActionWetClothingInHandsBaseCB {};

class ActionWetClothingInHandsWater : ActionWetClothingInHandsBase
{
	private const float WATER_DEPTH = 0.5;

	void ActionWetClothingInHandsWater()
	{
		m_CallbackClass = ActionWetClothingInHandsWaterCB;
		m_StanceMask    = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_CommandUID	= DayZPlayerConstants.CMD_ACTIONFB_WETCLOTHWELL;
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem   = new CCINonRuined;
		m_ConditionTarget = new CCTLiquid(LIQUID_GROUP_WATER - LIQUID_SNOW, UAMaxDistances.DEFAULT, UAMaxDistances.DEFAULT);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!item || !item.IsClothing())
			return false;

		return item.GetWet() < item.GetWetMax();
	}

	override protected int GetStanceMask(PlayerBase player)
	{
		vector waterInfo = HumanCommandSwim.WaterLevelCheck(player, player.GetPosition());
		if (waterInfo[1] > WATER_DEPTH)
			return DayZPlayerConstants.STANCEMASK_ERECT;

		return DayZPlayerConstants.STANCEMASK_CROUCH;
	}
	
	override void OnStart(ActionData action_data)
	{
		super.OnStart(action_data);
		
		action_data.m_Player.TryHideItemInHands(true);
	}
	
	override void OnEnd(ActionData action_data)
	{
		action_data.m_Player.TryHideItemInHands(false);
	}
}
