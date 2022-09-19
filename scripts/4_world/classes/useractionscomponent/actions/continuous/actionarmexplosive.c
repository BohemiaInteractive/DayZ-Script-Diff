class ActionArmExplosiveCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.EXPLOSIVE_ARM);
	}
	
	override void InitActionComponent()
	{
		super.InitActionComponent();
		
		RegisterAnimationEvent("CraftingAction", UA_IN_CRAFTING);
	}
};

class ActionArmExplosive: ActionContinuousBase
{	
	void ActionArmExplosive()
	{
		m_CallbackClass		= ActionArmExplosiveCB;
		m_CommandUID		= DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody			= true;
		m_SpecialtyWeight	= UASoftSkillsWeight.PRECISE_LOW;
		m_Text				= "#STR_ArmExplosive";
		m_Sound				= "craft_universal_0";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionTarget 	= new CCTNonRuined(UAMaxDistances.DEFAULT);
		m_ConditionItem 	= new CCINotPresent;
	}
	
	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override bool HasProgress()
	{
		return true;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (target.GetParent())
		{
			return false;
		}

		ItemBase tgt_item = ItemBase.Cast(target.GetObject());
		if (tgt_item && tgt_item.IsBeingPlaced())
		{
			return false;
		}

		ExplosivesBase explosive = ExplosivesBase.Cast(target.GetObject());

		return explosive && !explosive.GetArmed();
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{
		ExplosivesBase explosive = ExplosivesBase.Cast(action_data.m_Target.GetObject());
		if (explosive)
		{
			vector orientation	= action_data.m_Player.GetOrientation();
			vector position		= action_data.m_Player.GetPosition();
			position 			= position + (action_data.m_Player.GetDirection() * 0.5);

			explosive.OnPlacementComplete(action_data.m_Player, position, orientation);
		}
	}
}
