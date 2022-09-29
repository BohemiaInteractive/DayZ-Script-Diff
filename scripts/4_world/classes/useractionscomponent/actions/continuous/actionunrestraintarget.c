class ActionUnrestrainTargetCB : ActionContinuousBaseCB
{
	const float DEFAULT_UNRESTRAIN_TIME = 2;
	
	override void CreateActionComponent()
	{
		float time = ObtainUnrestrainTime();
		
		if( time <=0 )
		{
			time = DEFAULT_UNRESTRAIN_TIME;
		}
		
		if( m_ActionData.m_Player.IsQuickRestrain() )
		{
			time = DEBUG_QUICK_UNRESTRAIN_TIME;
		}

		m_ActionData.m_ActionComponent = new CAContinuousTime(time);
	}
	
	float ObtainUnrestrainTime()
	{
		PlayerBase target_player = PlayerBase.Cast(m_ActionData.m_Target.GetObject());
		PlayerBase source_player = m_ActionData.m_Player;
		
		EntityAI item_in_hands_source = source_player.GetItemInHands();
		
		ItemBase item_in_hands_target = target_player.GetItemInHands();
		
		CachedObjectsArrays.ARRAY_STRING.Clear();
		item_in_hands_target.ConfigGetTextArray( "CanBeUnrestrainedBy", CachedObjectsArrays.ARRAY_STRING );
		
		string item_in_hands_name = item_in_hands_source.GetType();
		
		for(int i = 0; i < CachedObjectsArrays.ARRAY_STRING.Count(); i++)
		{
			if((i % 2) == 0)
			{
				string class_name = CachedObjectsArrays.ARRAY_STRING.Get(i);
				if(	GetGame().IsKindOf(item_in_hands_name, class_name) )
				{
					float value = CachedObjectsArrays.ARRAY_STRING.Get(i+1).ToFloat();
					return value;
				}
			}
		}
		return -1;
	}
};

class ActionUnrestrainTarget: ActionContinuousBase
{
	void ActionUnrestrainTarget()
	{
		m_CallbackClass = ActionUnrestrainTargetCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_UNRESTRAINTARGET;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
		m_Text = "#unrestrain";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionTarget = new CCTMan(UAMaxDistances.DEFAULT, false);
		m_ConditionItem = new CCINonRuined;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		PlayerBase other_player = PlayerBase.Cast(target.GetObject());
		EntityAI item_in_hands_source = player.GetItemInHands();
		
		if( other_player.IsRestrained() )
		{
			//Print("is restrained");
			EntityAI item_in_hands_target = other_player.GetItemInHands();
			
			CachedObjectsArrays.ARRAY_STRING.Clear();
			
			if( item_in_hands_target )
			{
				item_in_hands_target.ConfigGetTextArray( "CanBeUnrestrainedBy", CachedObjectsArrays.ARRAY_STRING );
				
				string item_in_hands_name = item_in_hands_source.GetType();
				
				for(int i = 0; i < CachedObjectsArrays.ARRAY_STRING.Count(); i++)
				{
					if((i % 2) == 0)
					{
						string class_name = CachedObjectsArrays.ARRAY_STRING.Get(i);
						if(	GetGame().IsKindOf(item_in_hands_name, class_name) )
						{
							return true;
						}
					}
				}
			}
		}
		return false;
	}

	override void OnFinishProgressServer( ActionData action_data )
	{	
		PlayerBase player_target = PlayerBase.Cast(action_data.m_Target.GetObject());
		PlayerBase player_source = PlayerBase.Cast(action_data.m_Player);
		
		EntityAI unrestraining_tool = action_data.m_MainItem;
		EntityAI restraining_item = player_target.GetItemInHands();

		player_target.SetRestrained(false);
		
		
		//Damage applied to tool
		CachedObjectsArrays.ARRAY_STRING.Clear();
		
		restraining_item.ConfigGetTextArray( "CanBeUnrestrainedBy", CachedObjectsArrays.ARRAY_STRING );
		restraining_item.ConfigGetFloatArray( "CanBeUnrestrainedByDMG", CachedObjectsArrays.ARRAY_FLOAT );
		
		string item_in_hands_name = unrestraining_tool.GetType();
		float damageToTool = 0;
		
		for(int i = 0; i < CachedObjectsArrays.ARRAY_STRING.Count(); i++)
		{
			string class_name = CachedObjectsArrays.ARRAY_STRING.Get(i);
			if(	GetGame().IsKindOf(item_in_hands_name, class_name) )
			{
				damageToTool = CachedObjectsArrays.ARRAY_FLOAT.Get(i/2);
				break;
			}
		}
		
		MiscGameplayFunctions.DealAbsoluteDmg(action_data.m_MainItem, damageToTool);
		//---------------------------
		
		MiscGameplayFunctions.TransformRestrainItem(restraining_item, unrestraining_tool, player_source, player_target);
		
		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
	}
	
};

class ReplaceAndDestroyLambda : TurnItemIntoItemLambdaAnimSysNotifyLambda
{
	PlayerBase m_TargetPlayer;
	bool	m_Destroy;
	bool	m_Drop;
	
	void ReplaceAndDestroyLambda(EntityAI old_item, string new_item_type, PlayerBase player, bool destroy = false)
	{
		m_TargetPlayer = player;
		m_Destroy = destroy;
		m_OldItem = old_item;
	}
	
	override void OnSuccess(EntityAI new_item)
	{
		super.OnSuccess(new_item);
		
		if ( m_Destroy )
		{
			new_item.SetHealth("","",0);
		}
	}
};

class ReplaceAndDestroyLambdaEx : ReplaceAndDestroyLambda
{
	void ReplaceAndDestroyLambdaEx(EntityAI old_item, string new_item_type, PlayerBase player, bool destroy = false, bool enableDrop = true)
	{
		m_Drop = enableDrop;
	}
	
	override protected EntityAI CreateNewEntity()
	{
		EntityAI newItem = super.CreateNewEntity();
		
		if (!newItem && m_Drop)
		{
			newItem = EntityAI.Cast(GetGame().CreateObjectEx(m_NewItemType, m_TargetPlayer.GetPosition(), ECE_PLACE_ON_SURFACE|ECE_LOCAL));
		}
		
		return newItem;
	}
};