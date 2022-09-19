class FirearmActionLoadBullet : FirearmActionBase
{	
	//-----------------------------------------------------
	// 	Action events and methods
	//-----------------------------------------------------
	void FirearmActionLoadBullet() 
	{
		m_Text = "#load_bullet";
	}	
	
	override int GetActionCategory()
	{
		return AC_SINGLE_USE;
	}
	
	/*string GetTargetDescription()
	{
		return "default target description";
	}*/
	
	/*protected bool ActionConditionContinue( ActionData action_data ) //condition for action
	{
		return true;
	}*/
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item ) //condition for action
	{
		if (!super.ActionCondition( player, target, item ))
			return false;
		
		HumanCommandWeapons hcw = player.GetCommandModifier_Weapons();		
		Magazine mag = Magazine.Cast(target.GetObject());	
		Weapon_Base wpn = Weapon_Base.Cast(item);
		return mag && player.GetWeaponManager().CanLoadBullet(wpn,mag) && hcw && hcw.GetRunningAction() != WeaponActions.CHAMBERING;
	}
	
	override void Start( ActionData action_data )
	{
		super.Start( action_data );
		Magazine mag = Magazine.Cast(action_data.m_Target.GetObject());	

		action_data.m_Player.GetWeaponManager().LoadBullet(mag, this);
	}
	
	override bool CanBePerformedFromInventory()
	{
		return true;
	}
	
	override bool CanBePerformedFromQuickbar()
	{
		return true;
	}
	
	// action need first have permission from server before can start
	/*bool UseAcknowledgment()
	{
		return true;
	}*/

	
	/*override int GetState( ActionData action_data )
	{
		return UA_PROCESSING;
	}*/
	
	/*override float GetProgress( ActionData action_data )
	{
		return -1;
	}*/
};

class FirearmActionLoadBulletQuick : FirearmActionBase
{	
	//-----------------------------------------------------
	// 	Action events and methods
	//-----------------------------------------------------
	void FirearmActionLoadBulletQuick() 
	{
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
		
		HumanCommandWeapons hcw = player.GetCommandModifier_Weapons();		
		Weapon_Base weapon = Weapon_Base.Cast( item );		
		return player.GetWeaponManager().CanLoadBullet(weapon, player.GetWeaponManager().GetPreparedMagazine(), true) && hcw && hcw.GetRunningAction() != WeaponActions.CHAMBERING;
		
		
		//return (weapon.IsChamberEmpty(0) || weapon.IsChamberFiredOut(0)) && player.GetWeaponManager().GetPreparedMagazine()!= null;
	}
	
	override bool SetupAction(PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = NULL)
	{
		if ( super.SetupAction( player, target, item, action_data, extra_data))
		{
			if ( !GetGame().IsDedicatedServer() )
			{
				ActionTarget newTarget = new ActionTarget(player.GetWeaponManager().GetPreparedMagazine(), null, -1, vector.Zero, -1);
				action_data.m_Target = newTarget;
			}
			return true;
		}
		return false;
	}
	
	override void Start( ActionData action_data )
	{
		super.Start( action_data );
		Magazine mag = Magazine.Cast(action_data.m_Target.GetObject());

		action_data.m_Player.GetWeaponManager().LoadBullet(mag, this);
	}
};