class FirearmActionLoadMultiBullet : FirearmActionBase
{	
	//-----------------------------------------------------
	// 	Action events and methods
	//-----------------------------------------------------
	void FirearmActionLoadMultiBullet() 
	{
		m_Text = "#load_bullets";
	}	
	
	override int GetActionCategory()
	{
		return AC_CONTINUOUS;
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
		
		Weapon_Base wpn = Weapon_Base.Cast(item);
		Magazine mag = Magazine.Cast(target.GetObject());		
		return mag && player.GetWeaponManager().CanLoadBullet(wpn,mag);
	}
	
	override void Start( ActionData action_data )
	{
		super.Start( action_data );
		Magazine mag = Magazine.Cast(action_data.m_Target.GetObject());	

		action_data.m_Player.GetWeaponManager().LoadMultiBullet(mag, this);

	}
	
	override bool CanBePerformedFromInventory()
	{
		return false;
	}
	
	override bool CanBeSetFromInventory()
	{
		return false;
	}

	override void OnEndInput( ActionData action_data )
	{
		action_data.m_Player.GetWeaponManager().LoadMultiBulletStop();
	}

	override bool CanBePerformedFromQuickbar()
	{
		return true;
	}
	
	override bool HasProgress()
	{
		return false;
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
	
	override typename GetInputType()
	{
		return ContinuousDefaultActionInput;
	}
};


class FirearmActionLoadMultiBulletQuick : FirearmActionBase
{	
	//-----------------------------------------------------
	// 	Action events and methods
	//-----------------------------------------------------
	void FirearmActionLoadMultiBulletQuick() 
	{
	}	
	
	override bool HasTarget()
	{
		return false;
	}
	
	override bool HasProgress()
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
	
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item ) //condition for action
	{
		if (!super.ActionCondition( player, target, item ))
			return false;
		
		Weapon_Base weapon = Weapon_Base.Cast( item );
		return player.GetWeaponManager().CanLoadBullet(weapon ,player.GetWeaponManager().GetPreparedMagazine());
	}
	
	override void Start( ActionData action_data )
	{
		super.Start( action_data );
		
		WeaponManager weaponManager = action_data.m_Player.GetWeaponManager();
		int idx = 0;
		Magazine mag = weaponManager.GetNextPreparedMagazine(idx);
		Weapon weapon = Weapon.Cast(action_data.m_Player.GetItemInHands());
		
		int internalCount = weapon.GetInternalMagazineCartridgeCount(0);
		int maxCount = weapon.GetInternalMagazineMaxCartridgeCount(0) + 1;
		int total = mag.GetAmmoCount() + internalCount;
		
		if (total < maxCount)
		{	
			//Increment index for the first additional mag find, since GetNextPreparedMagazine does not do that
			//Normally the mag at found index is combined right after, removing it from the suitable magazine array	
			++idx;
			Magazine additionalMag = weaponManager.GetNextPreparedMagazine(idx);

			while ((additionalMag != null) && (total < maxCount))
			{
				total += additionalMag.GetAmmoCount();
				mag.CombineItems(additionalMag);
			 	additionalMag = weaponManager.GetNextPreparedMagazine(idx);
			}
		}

		action_data.m_Player.GetWeaponManager().LoadMultiBullet(mag, this);
	}
	
	override void OnEndInput( ActionData action_data )
	{
		action_data.m_Player.GetWeaponManager().LoadMultiBulletStop();
	}
};

class FirearmActionLoadMultiBulletRadial : FirearmActionBase
{	
	//-----------------------------------------------------
	// 	Action events and methods
	//-----------------------------------------------------
	void FirearmActionLoadMultiBulletRadial() 
	{
	}
	
	override bool HasProgress()
	{
		return false;
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined();
		m_ConditionTarget = new CCTSelf;
	}
	
	override bool CanContinue( ActionData action_data )
	{
		if (!super.CanContinue( action_data ))
			return false;
		
		return ActionCondition(action_data.m_Player, action_data.m_Target, action_data.m_MainItem);
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item ) //condition for action
	{
		if (!super.ActionCondition( player, target, item ))
			return false;
		
		Weapon_Base wpn = Weapon_Base.Cast(item);
		Magazine mag = Magazine.Cast(target.GetObject());		
		return mag && player.GetWeaponManager().CanLoadBullet(wpn,mag);
	}
	
	override void OnStart( ActionData action_data )
	{
		super.OnStart(action_data);
		
		Magazine mag = Magazine.Cast(action_data.m_Target.GetObject());	
		action_data.m_Player.GetWeaponManager().LoadMultiBullet(mag, this);
	}
	
	override void OnEnd( ActionData action_data )
	{
		action_data.m_Player.GetWeaponManager().LoadMultiBulletStop();
	}
};

