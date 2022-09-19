class ActionFillGeneratorTankCB : ActionContinuousBaseCB
{
	private const float QUANTITY_FILLED_PER_SEC = 400;
	
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousFillPowerGenerator(QUANTITY_FILLED_PER_SEC, LIQUID_GASOLINE);
	}
};

class ActionFillGeneratorTank: ActionContinuousBase
{	
	void ActionFillGeneratorTank()
	{
		m_CallbackClass = ActionFillGeneratorTankCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_EMPTY_VESSEL;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_FullBody = true;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_Text = "#refuel";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		PowerGenerator pg;
		
		if ( Class.CastTo(pg,target.GetObject()) )
		{
			if ( pg.CanAddFuel( item ) )
			{
				return true;
			}
		}
		
		return false;
	}
};