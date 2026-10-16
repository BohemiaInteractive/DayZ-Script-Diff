
class ActionMixMortarAtWell : ActionContinuousBase
{
	void ActionMixMortarAtWell ()
	{
		m_CallbackClass   = ActionMixMortarCB;
        m_CommandUID      = DayZPlayerConstants.CMD_ACTIONFB_MIX_MORTAR;
        m_FullBody        = true;
        m_StanceMask      = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
        m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
        m_Text            = "#STR_CraftMortarMix0";

	}

	override void CreateConditionComponents()
	{
	    m_ConditionItem   = new CCINonRuined();
	    m_ConditionTarget = new CCTObject();
	}
	
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		Object targetObject = target.GetObject();
 		if(targetObject && targetObject.IsWell())
		{
			return true;
		}
		return false;
	}
	
		override bool ActionConditionContinue(ActionData action_data)
    {
        return true;
    }
	
	override bool IsLockTargetOnUse()
	{
		return false;
	}
	
	override bool HasAlternativeInterrupt()
	{
		return true;
	}
	
	override bool UseAlternativeInterrupt(ActionData action_data)
	{
		return HasAlternativeInterrupt() && (action_data.m_ActionComponent.GetProgress() >= 1);
	}
	
	 // -------------------------------------------------------------------------
    // Result
    // -------------------------------------------------------------------------

    override void OnFinishProgressServer(ActionData action_data)
    {
        ItemBase mortarMixClosed = action_data.m_MainItem;
        if (!mortarMixClosed)
            return;

        string resultType = "MortarMix_Opened";

        // Try inventory first; fallback to ground at player pos
		// This is the result of the crafting (an open Mortar Mix)
        ItemBase mortarMixOpen = ItemBase.Cast(action_data.m_Player.GetInventory().CreateInInventory(resultType));

        if (!mortarMixOpen)
        {
            mortarMixOpen = ItemBase.Cast(GetGame().CreateObject(resultType, action_data.m_Player.GetPosition()));
            if (mortarMixOpen)
                mortarMixOpen.PlaceOnSurface();
        }

        if (mortarMixOpen)
        {
            mortarMixOpen.SetHealth("", "", mortarMixClosed.GetHealth());
            mortarMixClosed.Delete();
        }
    }
}