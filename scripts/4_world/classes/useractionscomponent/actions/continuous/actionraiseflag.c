class ActionManipulateFlagCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(1);
	}
};

class ActionRaiseFlag: ActionContinuousBase
{
	void ActionRaiseFlag()
	{
		m_CallbackClass = ActionManipulateFlagCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_RAISE_FLAG;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "#raise_flag";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionTarget = new CCTCursor;
		m_ConditionItem = new CCINone;
	}
	
	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}
	
	override bool HasTarget()
	{
		return true;
	}
	
	override bool HasProgress()
	{
		return true;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		TerritoryFlag totem = TerritoryFlag.Cast( target.GetObject() );
		if (!totem)
			return false;
		
		float state = totem.GetAnimationPhase("flag_mast");
		if ( totem.FindAttachmentBySlotName("Material_FPole_Flag") && state > 0 )
		{
			return true;
		}
		
		return false;
	}
	
	override void OnFinishProgressServer( ActionData action_data )
	{
		TerritoryFlag totem = TerritoryFlag.Cast( action_data.m_Target.GetObject() );
		if ( totem )
		{
			totem.AnimateFlag(totem.GetAnimationPhase("flag_mast") - UAMisc.FLAG_STEP_INCREMENT);
			totem.AddRefresherTime01(UAMisc.FLAG_STEP_INCREMENT);
		}
	}
};