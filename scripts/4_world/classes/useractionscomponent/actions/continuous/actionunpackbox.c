class ActionUnpackBoxCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.UNPACK);
	}
};

class ActionUnpackBox: ActionContinuousBase
{	
	void ActionUnpackBox()
	{
		m_CallbackClass = ActionUnpackBoxCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_OPENITEM;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_OPENITEM;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_Text = "#unbox";
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

	override bool HasTarget()
	{
		return false;
	}

	override void OnFinishProgressServer( ActionData action_data )
	{
		if ( action_data.m_MainItem && action_data.m_MainItem.GetHierarchyRootPlayer() == action_data.m_Player )
		{
			string path = "CfgVehicles " + action_data.m_MainItem.GetType();
			string child_name = "";
			int count;
			array<string> resources = new array<string>;
			
			if( GetGame().ConfigIsExisting( path ) && GetGame().ConfigIsExisting( path + " Resources") )
			{
				path = path + " Resources";
				count = GetGame().ConfigGetChildrenCount ( path );
				for (int i = 0; i < count; i++)
				{
					GetGame().ConfigGetChildName ( path, i, child_name );
					
					if ( GetGame().ConfigGetInt( path + " " + child_name + " value" ) )
					{
						resources.Insert( child_name );
					}
				}

				//TODO modify to allow for multiple ammo types spawning (if needed??)
				string itemType = resources.Get(0);
				int itemCount = GetGame().ConfigGetInt( path + " " + itemType + " value" );
				
				UnboxLambda lambda = new UnboxLambda(action_data.m_MainItem, itemType, action_data.m_Player, itemCount);
				action_data.m_Player.ServerReplaceItemInHandsWithNew(lambda);
				
				//spawns wrapping Paper
				ItemBase paper = ItemBase.Cast( GetGame().CreateObjectEx("Paper", action_data.m_Player.GetPosition(), ECE_PLACE_ON_SURFACE) );
				
				action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
			}
		}
	}
};

class UnboxLambda : ReplaceItemWithNewLambdaBase
{
	int m_ItemCount;
	void UnboxLambda (EntityAI old_item, string new_item_type, PlayerBase player, int count) { m_ItemCount = count; }

	override void CopyOldPropertiesToNew (notnull EntityAI old_item, EntityAI new_item)
	{
		super.CopyOldPropertiesToNew(old_item, new_item);

		if ( GetGame().ConfigIsExisting( "CfgMagazines " + m_NewItemType ) )
		{
			Magazine pile;
			Class.CastTo(pile, new_item);
			pile.ServerSetAmmoCount(m_ItemCount);
			Print("UnboxLambda::SetupNewMagazine new_item=" + new_item + " count=" + m_ItemCount);
		}
		else
		{
			ItemBase unboxed;
			Class.CastTo(unboxed, new_item);
			unboxed.SetQuantity(m_ItemCount);
			Print("UnboxLambda::SetupNewItem new_item=" + new_item + " count=" + m_ItemCount);
		}
	}
};

