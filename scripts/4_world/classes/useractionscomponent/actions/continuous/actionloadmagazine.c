class ActionLoadMagazineCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(UATimeSpent.MAG_LOAD);
	}
};

class ActionLoadMagazine: ActionContinuousBase
{
	void ActionLoadMagazine()
	{
		m_CallbackClass = ActionLoadMagazineCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_FILLMAG;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_FILLMAG;
		m_Text = "#load_magazine";
	}
	
	override bool HasProneException()
	{
		return true;
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined();
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
	}
	
	bool CanLoadMagazine(ActionTarget target, ItemBase item)
	{
		Magazine trg;
		Magazine itm;
		return ( Class.CastTo(trg, target.GetObject()) && Class.CastTo(itm, item) && itm.GetAmmoCount() < itm.GetAmmoMax() && trg.GetAmmoCount() != 0 && itm.IsCompatiableAmmo( trg ) );
	}
	
	override bool ActionConditionContinue( ActionData action_data )
	{
		return CanLoadMagazine(action_data.m_Target, action_data.m_MainItem);
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		return CanLoadMagazine(target, item);
	}
	
	override bool CanBePerformedFromQuickbar()
	{
		return true;
	}
	
	override void OnExecuteServer( ActionData action_data )
	{
		//Param1<float> timeSpendParam;
		//if( !Class.CastTo(timeSpendParam, action_data.m_ActionComponent.GetACData()) ) return;
		
		Magazine trg;
		Magazine itm;
		if ( Class.CastTo(trg, action_data.m_Target.GetObject()) && Class.CastTo(itm, action_data.m_MainItem) )
		{
			float dmg;
			string ammoType;
						
			if ( trg.ServerAcquireCartridge(dmg, ammoType) )
			{
				if ( !itm.ServerStoreCartridge(dmg, ammoType) )			
					trg.ServerStoreCartridge(dmg, ammoType);
				else
					itm.ApplyManipulationDamage(); //damages magazine
			}

			trg.SetSynchDirty();
			itm.SetSynchDirty();
			//manipulationDamage
			//if(trg.GetAmmoCount() > 0 )
				//trg.SetSynchDirty();
			//itm.SetSynchDirty();
		}
	}
};

class ActionLoadMagazineQuickCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(UATimeSpent.MAG_LOAD);
	}
};

class ActionLoadMagazineQuick: ActionContinuousBase
{
	void ActionLoadMagazineQuick()
	{
		m_CallbackClass = ActionLoadMagazineQuickCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_FILLMAG;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_FILLMAG;
	}
	
	override bool HasProneException()
	{
		return true;
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
	
	override bool HasTarget()
	{
		return false;
	}
	
	bool CanLoadMagazine( PlayerBase player, ItemBase item )
	{
		Magazine mag = Magazine.Cast( item );

		return mag && player.GetWeaponManager().GetPreparedMagazine() != null && mag.GetAmmoCount() < mag.GetAmmoMax();
	}
	
	override bool ActionConditionContinue( ActionData action_data )
	{
		return CanLoadMagazine(action_data.m_Player, action_data.m_MainItem);
	}	
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		return CanLoadMagazine(player, item);
	}
	
	override void OnExecuteServer( ActionData action_data )
	{
		//Param1<float> timeSpendParam;
		//if( !Class.CastTo(timeSpendParam, action_data.m_ActionComponent.GetACData()) ) return;
					
		Magazine itm = Magazine.Cast(action_data.m_MainItem);
		Magazine trg = action_data.m_Player.GetWeaponManager().GetPreparedMagazine();
					
		if ( !itm || !trg )
			return;
					
		float dmg;
		string ammoType;
						
		if ( trg.ServerAcquireCartridge(dmg, ammoType) )
		{
			if ( !itm.ServerStoreCartridge(dmg, ammoType) )			
				trg.ServerStoreCartridge(dmg, ammoType);
			else
				itm.ApplyManipulationDamage(); //damages magazine
		}
					
		trg.SetSynchDirty();
		itm.SetSynchDirty();
		//manipulationDamage
		//if(trg.GetAmmoCount() > 0 )
			//trg.SetSynchDirty();
		//itm.SetSynchDirty();
	}
};