class ActionSortAmmoPileCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.DEFAULT_SORT);
	}
};

class ActionSortAmmoPile: ActionContinuousBase
{
	void ActionSortAmmoPile()
	{
		m_CallbackClass = ActionSortAmmoPileCB;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_Text = "#sort_ammunition";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTSelf;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		return false;
	}

	override void OnEndAnimationLoopServer( ActionData action_data )
	{	
		SortAmmo(action_data.m_MainItem);
	}

	void SortAmmo(ItemBase item)
	{
		Magazine magazine;
		Magazine piles[5];
		//string pile_classname = magazine.ConfigGetString("spawnPileType");
		if( Class.CastTo(magazine, item) )
		{
			float health;
			string ammo_type;
			while( magazine.GetAmmoCount() > 0)
			{
				int count = magazine.GetAmmoCount();
				if(magazine.ServerAcquireCartridge(health, ammo_type))
				{
					int health_label = MiscGameplayFunctions.GetHealthLevelForAmmo(ammo_type,health);
					if( piles[health_label] == null )
					{
						piles[health_label] = Magazine.Cast( GetGame().CreateObjectEx( item.ClassName(), item.GetPosition(), ECE_PLACE_ON_SURFACE ) );		
					}
					piles[health_label].ServerStoreCartridge(health,ammo_type);
				}
			}
		}
	}
};