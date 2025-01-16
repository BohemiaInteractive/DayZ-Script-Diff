class ActionDigWormsCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		float time_spent;
		time_spent = UATimeSpent.DIG_WORMS;
		if (m_ActionData.m_MainItem.KindOf("Knife"))
			time_spent = time_spent * 1.2;
		
		if (m_ActionData.m_Player.GetInColdArea())
			time_spent *= GameConstants.COLD_AREA_DIG_WORMS_MODIF;
		
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(time_spent);
	}
};

class ActionDigWorms: ActionContinuousBase
{
	void ActionDigWorms()
	{
		m_CallbackClass = ActionDigWormsCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_DIGGIN_WORMS;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_MEDIUM;
		m_Text = "#dig_up_worms";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTSurface(UAMaxDistances.DEFAULT);
	}
	
	override bool Can(PlayerBase player, ActionTarget target, ItemBase item, int condition_mask)
	{
		if (!super.Can(player, target, item, condition_mask))
			return false;
		
		return player.CheckFreeSpace(vector.Forward, 0.8, false);
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.IsPlacingLocal())
			return false;
		
		return IsTargetFertile(target) && IsPlayerOnGround(player);
	}
	
	override bool ActionConditionContinue(ActionData action_data)
	{
		return IsPlayerOnGround(action_data.m_Player);
	}
	
	override bool HasTarget()
	{
		return true;
	}

	override void OnFinishProgressServer(ActionData action_data)
	{	
		int count = action_data.m_MainItem.GetOnDigWormsAmount();
		for (int i = 0; i < count; i++)
		{
			/*
			vector posHead;
			MiscGameplayFunctions.GetHeadBonePos(action_data.m_Player,posHead);
			vector posRandom = MiscGameplayFunctions.GetRandomizedPositionVerified(posHead,action_data.m_Target.GetCursorHitPos(),UAItemsSpreadRadius.NARROW,action_data.m_Player);
			
			GetGame().CreateObjectEx("Worm", posRandom, ECE_PLACE_ON_SURFACE);
			*/
			GetGame().CreateObjectEx("Worm", action_data.m_Target.GetCursorHitPos(), ECE_PLACE_ON_SURFACE);
		}
		
		MiscGameplayFunctions.DealEvinronmentAdjustedDmg(action_data.m_MainItem, action_data.m_Player, 4);
	}
		
	bool IsTargetFertile(ActionTarget target)
	{
		if (target)
		{
			string surface_type;
			vector position;
			position = target.GetCursorHitPos();
			
			GetGame().SurfaceGetType(position[0], position[2], surface_type);
			
			if (GetGame().IsSurfaceFertile(surface_type))
			{
				return true;
			}
		}
		
		return false;
	}
	
	bool IsPlayerOnGround(PlayerBase player)
	{
		vector position = player.GetPosition();
		float heightDiff = GetGame().SurfaceY(position[0], position[2]);
		heightDiff = position[1] - heightDiff;
		
		return heightDiff <= 0.4; // Player is considered on ground
	}
	
	//! DEPRECATED - See ItemBase.OverrideActionAnimation() to override commands for items
	void SetDiggignAnimation(ItemBase item);
	
};