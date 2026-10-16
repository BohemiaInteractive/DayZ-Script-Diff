class ActionWetClothingFromContainerCB : ActionContinuousBaseCB
{
	bool  m_RPCStopAlreadySent;
	float m_WetGainPerStep;

	override void CreateActionComponent()
	{
		float quantityPerSecond = 1.0;
		Bottle_Base bottle = Bottle_Base.Cast(m_ActionData.m_MainItem);
		if (bottle)
			quantityPerSecond = bottle.GetLiquidEmptyRate() * bottle.GetLiquidThroughputCoef();

		float wetMax = 1.0;
		ItemBase clothing = ItemBase.Cast(m_ActionData.m_Target.GetObject());
		if (clothing)
			wetMax = clothing.GetWetMax();

		float stepSize = 0.2 * wetMax / ActionWetClothingFromContainer.WET_PER_QUANTITY;
		m_WetGainPerStep = 0.2 * wetMax;
		float stepTime = stepSize / quantityPerSecond;
		m_ActionData.m_ActionComponent = new CAContinuousQuantityRepeat(quantityPerSecond, stepTime);
	}

	override void EndActionComponent()
	{
		ActionContinuousBase action = ActionContinuousBase.Cast(m_ActionData.m_Action);
		if (action.UseAlternativeInterrupt(m_ActionData))
			SetCommand(DayZPlayerConstants.CMD_ACTIONINT_FINISH);
		else
			SetCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
	}
};

class ActionWetClothingFromContainer : ActionContinuousBase
{
	static const float WET_PER_QUANTITY = 0.001;

	void ActionWetClothingFromContainer()
	{
		m_CallbackClass   = ActionWetClothingFromContainerCB;
		m_CommandUID      = DayZPlayerConstants.CMD_ACTIONMOD_EMPTY_VESSEL;
		m_StanceMask      = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_LOW;
		m_Text            = "#STR_Wet_Clothing";
		m_LockTargetOnUse = false;
	}

	override void CreateConditionComponents()
	{
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
		m_ConditionItem   = new CCINonRuined;
	}

	override bool HasAlternativeInterrupt() { return true; }

	override bool UseAlternativeInterrupt(ActionData action_data)
	{
		return action_data.m_ActionComponent.GetProgress() >= 1;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (g_Game.IsServer() && g_Game.IsMultiplayer())
			return true;

		Object targetObject = target.GetObject();
		if (!targetObject || !item || item.IsDamageDestroyed())
			return false;

		ItemBase targetItem = ItemBase.Cast(targetObject);
		if (!targetItem || !targetItem.IsClothing())
			return false;

		if (targetItem.GetHierarchyParent())
			return false;

		if (targetItem.GetWet() >= targetItem.GetWetMax())
			return false;

		if (!item.IsLiquidPresent() || item.GetQuantity() <= item.GetQuantityMin() || item.GetIsFrozen())
			return false;

		int lt = item.GetLiquidType();
		if (lt == LIQUID_GASOLINE || lt == LIQUID_DIESEL)
			return false;

		return true;
	}

	override bool ActionConditionContinue(ActionData action_data)
	{
		ItemBase container = action_data.m_MainItem;
		ItemBase clothing  = ItemBase.Cast(action_data.m_Target.GetObject());
		if (!container || !clothing)
			return false;

		if (container.GetQuantity() <= container.GetQuantityMin())
			return false;

		if (clothing.GetWet() >= clothing.GetWetMax())
			return false;

		return true;
	}

	override void OnFinishProgressServer(ActionData action_data)
	{
		ActionWetClothingFromContainerCB cb = ActionWetClothingFromContainerCB.Cast(action_data.m_Callback);
		if (!cb) return;

		ItemBase clothing = ItemBase.Cast(action_data.m_Target.GetObject());
		if (!clothing) return;

		clothing.SetWet(Math.Clamp(clothing.GetWet() + cb.m_WetGainPerStep, 0.0, clothing.GetWetMax()));
	}

	override void OnEndServer(ActionData action_data)
	{
		SendRPC(action_data, false);

		ActionWetClothingFromContainerCB cb = ActionWetClothingFromContainerCB.Cast(action_data.m_Callback);
		if (cb)
		{
			float wetGain = cb.m_WetGainPerStep * action_data.m_ActionComponent.GetProgress();
			if (wetGain > 0)
			{
				ItemBase clothing = ItemBase.Cast(action_data.m_Target.GetObject());
				if (clothing)
					clothing.SetWet(Math.Clamp(clothing.GetWet() + wetGain, 0.0, clothing.GetWetMax()));
			}
		}

		super.OnEndServer(action_data);
	}

	override void OnStartAnimationLoop(ActionData action_data) { SendRPC(action_data, true);  }
	override void OnEndAnimationLoop(ActionData action_data)   { SendRPC(action_data, false); }

	protected void SendRPC(ActionData actionData, bool enable)
	{
		if (!g_Game.IsMultiplayer() || g_Game.IsServer())
		{
			ActionWetClothingFromContainerCB comp = ActionWetClothingFromContainerCB.Cast(actionData.m_Callback);
			if (comp.m_RPCStopAlreadySent)
				return;

			Bottle_Base target_vessel = Bottle_Base.Cast(actionData.m_MainItem);
			Param1<bool> play = new Param1<bool>(enable);
			g_Game.RPCSingleParam(target_vessel, SoundTypeBottle.EMPTYING, play, true);

			if (!enable)
				comp.m_RPCStopAlreadySent = true;
		}
	}
};
