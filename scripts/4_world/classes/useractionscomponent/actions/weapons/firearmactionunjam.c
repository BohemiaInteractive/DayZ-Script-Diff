class FirearmActionUnjam : FirearmActionBase
{	
	void FirearmActionUnjam() 
	{
	}	
	
	override bool HasTarget()
	{
		return false;
	}
	
	override typename GetInputType()
	{
		return ContinuousWeaponManipulationActionInput;
	} 
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined();
		m_ConditionTarget = new CCTSelf;
	}
	
	override bool HasProgress()
	{
		return false;
	}
	
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item ) //condition for action
	{
		if (!super.ActionCondition( player, target, item ))
			return false;
		
		Weapon_Base wpn = Weapon_Base.Cast(item);
		return player.GetWeaponManager().CanUnjam(wpn);
	}
	
	override void Start( ActionData action_data )
	{
		super.Start( action_data );
		
		action_data.m_Player.GetWeaponManager().Unjam( this );
	}
};