class ActionFoldBaseBuildingObjectCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.DEFAULT_DEPLOY);
	}
}

class ActionFoldBaseBuildingObject : ActionContinuousBase
{
	void ActionFoldBaseBuildingObject()
	{
		m_CallbackClass 	= ActionFoldBaseBuildingObjectCB;
		m_CommandUID 		= DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody 			= true;
		m_StanceMask 		= DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight 	= UASoftSkillsWeight.ROUGH_HIGH;
		
		m_Text = "#fold";
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

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		Object targetObject = target.GetObject();
		if (targetObject.CanUseConstruction())
		{
			BaseBuildingBase baseBuilding = BaseBuildingBase.Cast(targetObject);
			
			if (baseBuilding.CanFoldBaseBuildingObject())
				return true;
		}
		
		return false;
	}
	
	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);

		ItemBase item = ItemBase.Cast(action_data.m_Target.GetObject());
		if (item.GetLoopFoldSoundset() != string.Empty)
			item.StartItemSoundServer(SoundConstants.ITEM_FOLD_LOOP);
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);

		BaseBuildingBase baseBuilding = BaseBuildingBase.Cast(action_data.m_Target.GetObject());
		baseBuilding.CreateConstructionKitInHands(action_data.m_Player);
		baseBuilding.DestroyConstruction();

		if (baseBuilding.GetFoldSoundset() != string.Empty)
			baseBuilding.StartItemSoundServer(SoundConstants.ITEM_FOLD);
		if (baseBuilding)
			baseBuilding.StopItemSoundServer(SoundConstants.ITEM_FOLD_LOOP);
	}
	
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		
		//ItemBase item = ItemBase.Cast(action_data.m_Player.GetItemInHands());
		ItemBase item = ItemBase.Cast(action_data.m_Target.GetObject());
		if (item)
			item.StopItemSoundServer(SoundConstants.ITEM_FOLD_LOOP);
	}
	
	override string GetAdminLogMessage(ActionData action_data)
	{
		return " folded " + action_data.m_Target.GetObject().GetDisplayName();
	}
}