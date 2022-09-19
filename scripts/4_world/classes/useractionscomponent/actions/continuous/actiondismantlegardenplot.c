class ActionDismantleGardenPlotCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime( UATimeSpent.DIG_GARDEN );
	}
};

class ActionDismantleGardenPlot: ActionContinuousBase
{	
	//GardenPlot m_GardenPlot;
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;//new CCINone;
		m_ConditionTarget = new CCTCursorNoRuinCheck;
	}
	
	void ActionDismantleGardenPlot()
	{
		m_CallbackClass	= ActionDismantleGardenPlotCB;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_LOW;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_DIGMANIPULATE;
		
		m_Text = "#remove_garden_plot";
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		//Action not allowed if player has broken legs
		if ( player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS )
			return false;
		
		GardenPlot targetPlot = GardenPlot.Cast( target.GetObject() );
		
		if ( targetPlot && !player.IsPlacingLocal() )
			return true;
		else
			return false;
	}

	void SetupAnimation( ItemBase item )
	{
		if ( item )
		{
			m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_DIG;
		}
	}
	
	override void OnFinishProgressServer( ActionData action_data )
	{	
		GardenPlot targetPlot = GardenPlot.Cast( action_data.m_Target.GetObject() );
		targetPlot.Delete();
		
		if ( GetGame().IsServer() )
			MiscGameplayFunctions.DealAbsoluteDmg( action_data.m_MainItem, 10 );
	}
};
