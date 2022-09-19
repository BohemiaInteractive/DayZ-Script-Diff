class DetachMagActionReciveData : ActionReciveData
{
	ref InventoryLocation  m_ilMagazine;
}
class DetachMagActionData : ActionData
{
	ref InventoryLocation  m_ilMagazine;
}

class FirearmActionDetachMagazine_Old : FirearmActionBase
{	
	void FirearmActionDetachMagazine_Old() 
	{
	}	
	
	override bool HasTarget()
	{
		return true;
	}
	
	override typename GetInputType()
	{
		return QuickaBarActionInput;
	}
	
	override bool CanBePerformedFromQuickbar()
	{
		return true;
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
	
	override bool InventoryReservation( ActionData action_data)
	{
		Magazine mag = Magazine.Cast(action_data.m_Target.GetObject());
		InventoryLocation il = new InventoryLocation();
		if( !action_data.m_Player.GetInventory().FindFreeLocationFor(mag, FindInventoryLocationType.ANY_CARGO, il) )
			return false;
		
		if( !super.InventoryReservation( action_data) )
			return false;

		DetachMagActionData action_data_dm = DetachMagActionData.Cast(action_data);
		if( !action_data.m_Player.GetInventory().AddInventoryReservationEx(mag,il,10000) )
			return false;
			
		action_data_dm.m_ReservedInventoryLocations.Insert(il);
		action_data_dm.m_ilMagazine = il;

		return true;
	}
	
	override ActionData CreateActionData()
	{
		ActionData action_data = new DetachMagActionData;
		return action_data;
	}
	
	override void WriteToContext (ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		
		DetachMagActionData action_data_dm;
		DetachMagActionData.CastTo(action_data_dm, action_data);

		action_data_dm.m_ilMagazine.WriteToContext(ctx);
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data )
	{
		if(!action_recive_data)
		{
			action_recive_data = new DetachMagActionReciveData;
		}
		
		if(!super.ReadFromContext( ctx, action_recive_data ))
			return false;
		
		DetachMagActionReciveData recive_data_dm;
		recive_data_dm = DetachMagActionReciveData.Cast( action_recive_data );
		
		recive_data_dm.m_ilMagazine = new InventoryLocation;

		if(!recive_data_dm.m_ilMagazine.ReadFromContext(ctx))
			return false;
		
		return true;
	}
	
	override void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		super.HandleReciveData(action_recive_data, action_data);
		
		DetachMagActionReciveData recive_data_dm = DetachMagActionReciveData.Cast(action_recive_data);
		DetachMagActionData action_data_dm = DetachMagActionData.Cast(action_data);
		
		action_data_dm.m_ilMagazine = recive_data_dm.m_ilMagazine;
	}
	
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item ) //condition for action
	{
		if (!super.ActionCondition( player, target, item ))
			return false;
		
		Magazine mag = Magazine.Cast(target.GetObject());
		Weapon_Base wpn = Weapon_Base.Cast(item);
		return mag && player.GetWeaponManager().CanDetachMagazine(wpn,mag);
	}
	
	override bool ActionConditionContinue( ActionData action_data)
	{
		return true;
	}
	
	override void Start( ActionData action_data )
	{
		super.Start( action_data );
		DetachMagActionData action_data_dm = DetachMagActionData.Cast(action_data);
		
		Weapon_Base wpn = Weapon_Base.Cast( action_data.m_MainItem );
		
		action_data.m_Player.GetWeaponManager().DetachMagazine(action_data_dm.m_ilMagazine,this);
	}
};

class AdvDetachMagActionReciveData : ActionReciveData
{
	ref InventoryLocation  m_ilWeapon;
	ref InventoryLocation  m_ilMagazine;
}
class AdvDetachMagActionData : SequentialActionData
{
	ref InventoryLocation  m_ilWeapon = new InventoryLocation;
	ref InventoryLocation  m_ilMagazine = new InventoryLocation;
}

class FirearmActionDetachMagazine : ActionSequentialBase
{
	void FirearmActionDetachMagazine()
	{
	}
	
	override ActionData CreateActionData()
	{
		ActionData action_data = new AdvDetachMagActionData;
		return action_data;
	}
	

	override bool InventoryReservation( ActionData action_data)
	{
		Weapon_Base wpn = Weapon_Base.Cast(action_data.m_MainItem);
		Magazine mag = Magazine.Cast(action_data.m_Target.GetObject());
		
		InventoryLocation ilWpn = new InventoryLocation();
		InventoryLocation ilMag = new InventoryLocation();
		
		
		int ur_index = action_data.m_Player.GetHumanInventory().FindUserReservedLocationIndex(wpn);
			
		if(ur_index > -1)
			action_data.m_Player.GetHumanInventory().GetUserReservedLocation(ur_index,ilWpn);
		else
		{
			action_data.m_Player.GetInventory().FindFreeLocationFor(wpn, FindInventoryLocationType.ANY_CARGO | FindInventoryLocationType.ATTACHMENT, ilWpn);
			
			
				//return false;
		}
		
		if( !action_data.m_Player.GetInventory().FindFreeLocationFor(mag, FindInventoryLocationType.ANY_CARGO, ilMag) )
			return false;
		
		if( !super.InventoryReservation( action_data ) )
			return false;

		AdvDetachMagActionData action_data_dm = AdvDetachMagActionData.Cast(action_data);
		if( !action_data.m_Player.GetInventory().AddInventoryReservationEx(wpn,ilWpn,10000) )
			return false;
		
		if( !action_data.m_Player.GetInventory().AddInventoryReservationEx(mag,ilMag,10000) )
			return false;
			
		action_data_dm.m_ReservedInventoryLocations.Insert(ilWpn);
		action_data_dm.m_ReservedInventoryLocations.Insert(ilMag);
		action_data_dm.m_ilWeapon = ilWpn;
		action_data_dm.m_ilMagazine = ilMag;

		return true;
	}
	
	override void WriteToContext (ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		
		AdvDetachMagActionData action_data_dm;
		AdvDetachMagActionData.CastTo(action_data_dm, action_data);

		action_data_dm.m_ilWeapon.WriteToContext(ctx);
		action_data_dm.m_ilMagazine.WriteToContext(ctx);
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data )
	{
		if(!action_recive_data)
		{
			action_recive_data = new AdvDetachMagActionReciveData;
		}
		
		if(!super.ReadFromContext( ctx, action_recive_data ))
			return false;
		
		AdvDetachMagActionReciveData recive_data_dm;
		recive_data_dm = AdvDetachMagActionReciveData.Cast( action_recive_data );
		
		recive_data_dm.m_ilWeapon = new InventoryLocation;
		recive_data_dm.m_ilMagazine = new InventoryLocation;
		
		if(!recive_data_dm.m_ilWeapon.ReadFromContext(ctx))
			return false;

		if(!recive_data_dm.m_ilMagazine.ReadFromContext(ctx))
			return false;
		
		return true;
	}
	
	override void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		super.HandleReciveData(action_recive_data, action_data);
		
		AdvDetachMagActionReciveData recive_data_dm = AdvDetachMagActionReciveData.Cast(action_recive_data);
		AdvDetachMagActionData action_data_dm = AdvDetachMagActionData.Cast(action_data);
		
		action_data_dm.m_ilWeapon.Copy(recive_data_dm.m_ilWeapon);
		action_data_dm.m_ilMagazine.Copy(recive_data_dm.m_ilMagazine);
	}	
	
	
	override typename GetInputType()
	{
		return QuickaBarActionInput;
	}
	
	override void CreateConditionComponents() //Instantiates components, called once from ActionManager on actions construction
	{
		m_ConditionItem = new CCIDummy;
		m_ConditionTarget = new CCTNone;
	}
	
	override bool CanBePerformedFromQuickbar()
	{
		return true;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!super.ActionCondition( player, target, item ))
			return false;
		
		Weapon_Base wpn = Weapon_Base.Cast( item );
		int mi = wpn.GetCurrentMuzzle();
		return wpn && wpn.GetMagazine(mi) == target.GetObject();
	}
	
	override bool ActionConditionContinue( ActionData action_data )
	{
		return true;
	}
	
	override void OnStageStart(SequentialActionData action_data)
	{
		if( action_data.m_ReciveEndInput )
		{
			End( action_data );
			return;
		}
		super.OnStageStart(action_data);
		Weapon_Base wpn;
		Magazine mag;
		//InventoryLocation il = new InventoryLocation();
		AdvDetachMagActionData action_data_dm = AdvDetachMagActionData.Cast(action_data);

		switch (action_data.m_Stage)
		{
			case 0:
				action_data.m_Player.GetWeaponManager().DetachMagazine(action_data_dm.m_ilMagazine,this);			
				break;
			
			case 1:
				if ( !action_data_dm.m_ilWeapon.IsValid() )
				{
					End( action_data );
					break;
				}
				
				wpn = Weapon_Base.Cast(action_data.m_MainItem);
				mag = Magazine.Cast(action_data.m_Target.GetObject());
			
				action_data.m_Player.GetHumanInventory().ClearUserReservedLocation(wpn);
				if ( ( !GetGame().IsDedicatedServer() ) && !mag.GetHierarchyParent() )
				{
					InventoryMode invMode = InventoryMode.PREDICTIVE;
					
					if ( action_data.m_Player.NeedInventoryJunctureFromServer(mag, action_data.m_Player, action_data.m_Player) || action_data.m_Player.NeedInventoryJunctureFromServer(wpn, action_data.m_Player, action_data.m_Player) )
						invMode = InventoryMode.JUNCTURE;
				
					action_data.m_Player.GetInventory().ForceSwapEntities(invMode, mag, wpn, action_data_dm.m_ilWeapon);
				}

				End( action_data );
				break;
			default:
				End( action_data );
				break;
		}
	}
	
	override void OnUpdate( ActionData action_data )
	{
		super.OnUpdate( action_data );
		
		SequentialActionData ad = SequentialActionData.Cast(action_data);
		if(ad)
		{
			switch (ad.m_Stage)
			{
				case 0:
					Weapon_Base wpn = Weapon_Base.Cast(action_data.m_MainItem);
		
					if ( (!wpn || ad.m_Player.GetItemInHands()!=wpn || !ad.m_Player.GetWeaponManager().IsRunning()) && !ad.m_Player.IsJumpInProgress() )
					{
						SetStage(1, ad);
					}
					break;
				case 1:
					//action_data.m_Player.GetHandFs
					
					break;
				default:
					break;
			}
		}
	}
	
	override void OnStartServer( ActionData action_data )
	{
		AdvDetachMagActionData action_data_dm = AdvDetachMagActionData.Cast(action_data);
		GetGame().AddInventoryJunctureEx(action_data.m_Player, EntityAI.Cast(action_data.m_Target.GetObject()),action_data_dm.m_ilMagazine, true, 10000);
	}
	
	override void OnEndServer( ActionData action_data )
	{
		GetGame().ClearJuncture(action_data.m_Player, EntityAI.Cast(action_data.m_Target.GetObject()));
	}
}