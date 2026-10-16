// Full action class: Mix Mortar at Water Source (FillBottle-inspired)
// - Requires closed MortarMix in hands
// - Valid when aiming at a water source (well/pump) or water surface (fresh/still/sea)
// - Spawns MortarMix_Opened when finished

class ActionMixMortarCB : ActionContinuousBaseCB
{
    override void CreateActionComponent()
    {
        m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.MORTAR_MIX);
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

class ActionMixMortarAtWater : ActionContinuousBase
{
    private const float WATER_DEPTH = 0.5;

    // Which environmental liquids are allowed (fresh/still + salt)
    protected int m_AllowedLiquidMask;

    void ActionMixMortarAtWater()
    {
        m_CallbackClass   = ActionMixMortarCB;
        m_CommandUID      = DayZPlayerConstants.CMD_ACTIONFB_MIX_MORTAR;
        m_FullBody        = true;
        m_StanceMask      = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
        m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
        m_Text            = "#STR_CraftMortarMix0";

        // Allow ponds/rivers/wells and sea
        m_AllowedLiquidMask  = LIQUID_GROUP_DRINKWATER | LIQUID_SALTWATER;
    }

    // -------------------------------------------------------------------------
    // Conditions
    // -------------------------------------------------------------------------

	override void CreateConditionComponents()
	{
	    m_ConditionItem   = new CCINonRuined();
		m_ConditionTarget 	= new CCTLiquid(m_AllowedLiquidMask, UAMaxDistances.DEFAULT, UAMaxDistances.DEFAULT);
	}
	
	override bool ActionConditionContinue(ActionData action_data)
    {
        return true;
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