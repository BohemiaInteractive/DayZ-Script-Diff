class FirearmActionMechanicManipulate : FirearmActionBase
{	
	void FirearmActionLoadBulletQuick() 
	{
	}	
	
	override bool HasTarget()
	{
		return false;
	}
	
	override typename GetInputType()
	{
		return WeaponManipulationActionInput;
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
		
		bool result = false;
		Weapon_Base wpn = Weapon_Base.Cast(item);
		if ( player.GetWeaponManager().CanEjectBullet(wpn))
		{
			if( GetGame().IsServer() && GetGame().IsMultiplayer() )
			{
				result = true;
			}
			else
			{ 
				if( player.GetWeaponManager().CanEjectBulletVerified() )
				{
					result = true;
				}
				player.GetWeaponManager().SetEjectBulletTryTimestamp();
			}
		}
		return result;
	}
	
	override void Start( ActionData action_data )
	{
		super.Start( action_data );
		
		action_data.m_Player.GetWeaponManager().EjectBulletVerified( this );
	}
};