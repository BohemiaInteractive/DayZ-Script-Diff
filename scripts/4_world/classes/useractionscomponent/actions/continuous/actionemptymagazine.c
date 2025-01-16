class EmptyMagazineActionData : ActionData
{
	Magazine m_magazinePile;
}

class ActionEmptyMagazineCB : ActionContinuousBaseCB
{	
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(UATimeSpent.MAG_EMPTY);
	}
};

class ActionEmptyMagazine: ActionContinuousBase
{
	void ActionEmptyMagazine()
	{
		m_CallbackClass = ActionEmptyMagazineCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_EMPTYMAG;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_EMPTYMAG;
		m_Text = "#empty_magazine";
	}
	
	override bool SetupAction(PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = NULL)
	{
		if ( super.SetupAction( player, target, item, action_data, extra_data))
		{
			EmptyMagazineActionData emActionData = EmptyMagazineActionData.Cast(action_data);
			emActionData.m_magazinePile = NULL;
			return true;
		}
		return false;
	}
	
	override ActionData CreateActionData()
	{
		EmptyMagazineActionData action_data = new EmptyMagazineActionData;
		return action_data;
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}
	
	override bool HasProneException()
	{
		return true;
	}
	
	bool CanEmpty(ItemBase item)
	{
		Magazine mag;
		return ( item && Class.CastTo(mag, item) && mag.GetAmmoCount() > 0 );
	}
	
	override bool ActionConditionContinue( ActionData action_data )
	{	
		return CanEmpty(action_data.m_MainItem);
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		Magazine itm;
		return CanEmpty(item);
	}

	override bool HasTarget()
	{
		return false;
	}
	
	override void OnExecuteServer( ActionData action_data )
	{
		EmptyMagazineActionData emActionData = EmptyMagazineActionData.Cast(action_data);
		
		Magazine mag;
		if (!Class.CastTo(mag, action_data.m_MainItem))
			return;
		
		float dmg;
		string ammoType;
		
		if (!mag.ServerAcquireCartridge(dmg,ammoType))
			return;
					
		mag.ApplyManipulationDamage();
					
		string magazineTypeName;
		GetGame().ConfigGetText( "CfgAmmo " + ammoType + " spawnPileType" , magazineTypeName);
					
		if (!DayZPlayerUtils.HandleStoreCartridge(action_data.m_Player, NULL, -1, dmg, ammoType, magazineTypeName, false))
		{
			array< Magazine > piles;
			vector posHead;
			MiscGameplayFunctions.GetHeadBonePos(action_data.m_Player,posHead);
			vector posTarget = action_data.m_Player.GetPosition() + (action_data.m_Player.GetDirection() * DEFAULT_SPAWN_DISTANCE);
			
			if (!emActionData.m_magazinePile)
			{
				piles = MiscGameplayFunctions.CreateMagazinePilesDispersed(magazineTypeName,posHead,posTarget,UAItemsSpreadRadius.NARROW,1,dmg,action_data.m_Player);
				if (piles.Count() > 0)
					emActionData.m_magazinePile = piles[0];
			}
			else
			{				
				if (emActionData.m_magazinePile.GetAmmoMax() > emActionData.m_magazinePile.GetAmmoCount() && emActionData.m_magazinePile.GetType() == magazineTypeName && (vector.DistanceSq(action_data.m_Player.GetPosition(), emActionData.m_magazinePile.GetPosition()) < (UAMaxDistances.DEFAULT * UAMaxDistances.DEFAULT)))
				{
					emActionData.m_magazinePile.ServerStoreCartridge(dmg,ammoType);
				}
				else
				{
					piles = MiscGameplayFunctions.CreateMagazinePilesFromBulletDispersed(ammoType,posHead,posTarget,UAItemsSpreadRadius.VERY_NARROW,1,dmg,action_data.m_Player);
					if (piles.Count() > 0)
						emActionData.m_magazinePile = piles[0];
				}	
			}
		}
	}
};