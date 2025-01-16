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
}

class ActionArmExplosive : ActionContinuousBase
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
		m_ConditionItem 	= new CCINotPresent();
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
			return false;

		ItemBase targetItem = ItemBase.Cast(target.GetObject());
		if (targetItem && targetItem.IsBeingPlaced())
			return false;

		ExplosivesBase explosive = ExplosivesBase.Cast(target.GetObject());

		return explosive && !explosive.GetArmed();
	}
	
	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		
		ExplosivesBase item = ExplosivesBase.Cast(action_data.m_Target.GetObject());
		if (item.GetArmSoundset() != string.Empty)
			item.StartItemSoundServer(SoundConstants.ITEM_EXPLOSIVE_ARM);
	}
	
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		
		ExplosivesBase item = ExplosivesBase.Cast(action_data.m_Target.GetObject());
		if (item.GetArmSoundset() != string.Empty)
			item.StopItemSoundServer(SoundConstants.ITEM_EXPLOSIVE_ARM);
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);

		ExplosivesBase explosive = ExplosivesBase.Cast(action_data.m_Target.GetObject());
		if (explosive)
			explosive.OnPlacementComplete(action_data.m_Player, explosive.GetPosition(), action_data.m_Player.GetOrientation());
	}
}
