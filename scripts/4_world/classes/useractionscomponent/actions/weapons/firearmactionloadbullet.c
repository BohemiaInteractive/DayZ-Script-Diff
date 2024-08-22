class FirearmActionLoadBullet : FirearmActionBase
{	
	void FirearmActionLoadBullet() 
	{
		m_Text = "#load_bullet";
	}	
	
	override int GetActionCategory()
	{
		return AC_SINGLE_USE;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!super.ActionCondition(player, target, item))
			return false;
		
		HumanCommandWeapons hcw = player.GetCommandModifier_Weapons();
		if (!target.GetObject())
			return false;
		
		Magazine mag = null;
		EntityAI entity = EntityAI.Cast(target.GetObject());
		if (entity && entity.IsManagingArrows() && entity.IsDamageDestroyed())
		{
			ArrowManagerBase arrowManager = ArrowManagerBase.Cast(entity.GetArrowManager());
			if (arrowManager)
				mag = Magazine.Cast(arrowManager.GetFirstArrow());
		}
		else
			mag = Magazine.Cast(target.GetObject());
		
		Weapon_Base weapon = Weapon_Base.Cast(item);
		return mag && player.GetWeaponManager().CanLoadBullet(weapon, mag) && hcw && hcw.GetRunningAction() != WeaponActions.CHAMBERING;
	}
	
	override void Start(ActionData action_data)
	{
		super.Start(action_data);

		Magazine mag;
		EntityAI entity = EntityAI.Cast(action_data.m_Target.GetObject());
		if (entity && entity.IsManagingArrows() && entity.IsDamageDestroyed())
		{
			ArrowManagerBase arrowManager = ArrowManagerBase.Cast(entity.GetArrowManager());
			if (arrowManager)
			{
				mag = Magazine.Cast(arrowManager.AcquireFirstArrow(false));
				mag.SetPosition(action_data.m_Player.GetPosition());
			}
		}
		else
			mag = Magazine.Cast(action_data.m_Target.GetObject());

		action_data.m_Player.GetWeaponManager().LoadBullet(mag, this);
	}
	
	override Object GetDisplayInteractObject(PlayerBase player, ActionTarget target)
	{
		EntityAI entity = EntityAI.Cast(target.GetObject());
		if (!entity || (entity && entity.IsManagingArrows() && !entity.IsDamageDestroyed()))
			return null;
		
		ArrowManagerBase arrowManager = entity.GetArrowManager();
		if (arrowManager)
			return arrowManager.GetFirstArrow();
		
		return null;
	}
	
	override bool CanBePerformedFromInventory()
	{
		return true;
	}
	
	override bool CanBePerformedFromQuickbar()
	{
		return true;
	}
}

class FirearmActionLoadBulletQuick : FirearmActionBase
{	
	void FirearmActionLoadBulletQuick();
	
	override typename GetInputType()
	{
		return ContinuousWeaponManipulationActionInput;
	} 
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTSelf();
	}
	
	override bool HasProgress()
	{
		return false;
	}
		
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!super.ActionCondition(player, target, item))
			return false;
		
		HumanCommandWeapons hcw = player.GetCommandModifier_Weapons();		
		Weapon_Base weapon = Weapon_Base.Cast(item);

		return player.GetWeaponManager().CanLoadBullet(weapon, player.GetWeaponManager().GetPreparedMagazine(), true) && hcw && hcw.GetRunningAction() != WeaponActions.CHAMBERING;
	}
	
	override bool SetupAction(PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = NULL)
	{
		if (super.SetupAction(player, target, item, action_data, extra_data))
		{
			#ifndef SERVER
			ActionTarget newTarget = new ActionTarget(player.GetWeaponManager().GetPreparedMagazine(), null, -1, vector.Zero, -1);
			action_data.m_Target = newTarget;
			#endif

			return true;
		}

		return false;
	}
	
	override void Start(ActionData action_data)
	{
		super.Start(action_data);
		
		Magazine mag = Magazine.Cast(action_data.m_Target.GetObject());
		action_data.m_Player.GetWeaponManager().LoadBullet(mag, this);
	}
}
