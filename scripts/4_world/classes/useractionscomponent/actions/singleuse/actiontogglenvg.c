class ActionToggleNVG: ActionBase
{
	void ActionToggleNVG()
	{
	}
	
	override bool IsInstant()
	{
		return true;
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
	}
	
	override typename GetInputType()
	{
		return ToggleNVGActionInput;
	}
	
	override bool HasTarget()
	{
		return true;
	}
	
	override bool UseMainItem()
	{
		return false;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		NVGoggles goggles;
		Clothing NVmount;
		NVmount = Clothing.Cast(target.GetObject());
		if ( !NVmount )
			return false;
		goggles = NVGoggles.Cast(NVmount.FindAttachmentBySlotName("NVG"));
		if ( goggles )
			return true;
		
		return false;
	}

	override void Start( ActionData action_data )
	{
		super.Start( action_data );
		
		NVGoggles goggles;
		Clothing NVmount;

		NVmount = Clothing.Cast(action_data.m_Target.GetObject());
		goggles = NVGoggles.Cast(NVmount.FindAttachmentBySlotName("NVG"));
		
		goggles.RotateGoggles(goggles.m_IsLowered);
	}
};
