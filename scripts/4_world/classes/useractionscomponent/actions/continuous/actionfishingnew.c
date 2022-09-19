class FishingActionData : ActionData
{
	const float FISHING_SUCCESS 		= 0.2;
	const float FISHING_BAIT_LOSS 		= 0.02;
	const float FISHING_HOOK_LOSS 		= 0.015;
	const float FISHING_DAMAGE 			= 1.5;
	const float FISHING_GARBAGE_CHANCE 	= 0.2;
	
	bool 		m_IsSurfaceSea;
	bool 		m_IsBaitAnEmptyHook;
	int 		m_FishingResult = -1;
	float 		m_RodQualityModifier = 0;
	ItemBase 	m_Bait;
	
	void InitBait(ItemBase item)
	{
		m_Bait = item;
		m_IsBaitAnEmptyHook = !m_Bait.ConfigIsExisting("hookType");
	}
	
	bool IsBaitEmptyHook()
	{
		return m_IsBaitAnEmptyHook;
	}
}

class ActionFishingNewCB : ActionContinuousBaseCB
{
	FishingActionData 	m_ActionDataFishing;
	ref array<string> 	m_JunkTypes = {"Wellies_Black","Wellies_Brown","Wellies_Green","Wellies_Grey","Pot"};
	
	override void CreateActionComponent()
	{
		EnableStateChangeCallback();
		m_ActionData.m_ActionComponent = new CAContinuousRepeatFishing(3.0);
	}
	
	override void EndActionComponent()
	{
		m_ActionDataFishing = FishingActionData.Cast(m_ActionData);
		
		if (!m_ActionDataFishing)
		{
			super.EndActionComponent();
			return;
		}
		
		if ( m_ActionDataFishing.m_State == UA_FINISHED )
		{
			if ( m_ActionDataFishing.m_FishingResult == 1 )
				SetCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
			else if ( m_ActionDataFishing.m_FishingResult == 0 )
				SetCommand(DayZPlayerConstants.CMD_ACTIONINT_FINISH);
		}
		else if (m_ActionDataFishing.m_State == UA_CANCEL )
		{
			ActionContinuousBase action = ActionContinuousBase.Cast(m_ActionDataFishing.m_Action);
			if(action.HasAlternativeInterrupt())
			{
				SetCommand(DayZPlayerConstants.CMD_ACTIONINT_FINISH);
			}
			else
			{
				SetCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
			}
			m_Canceled = true;
			return;
		}
		else
		{
			if ( m_ActionDataFishing.m_FishingResult == 1 )
				SetCommand(DayZPlayerConstants.CMD_ACTIONINT_END);
			else if ( m_ActionDataFishing.m_FishingResult == 0 )
				SetCommand(DayZPlayerConstants.CMD_ACTIONINT_FINISH);
		}
		m_ActionDataFishing.m_State = UA_FINISHED;
	}
	
	void HandleFishingResultSuccess()
	{
		if (!GetGame().IsMultiplayer() || GetGame().IsServer())
		{
			ItemBase fish;

			if (!m_ActionDataFishing.m_Bait)
				m_ActionDataFishing.InitBait(ItemBase.Cast(m_ActionDataFishing.m_MainItem.FindAttachmentBySlotName("Hook")));
			
			if (!m_ActionDataFishing.IsBaitEmptyHook())
			{
				m_ActionDataFishing.m_Bait.AddHealth(-m_ActionDataFishing.FISHING_DAMAGE);
				MiscGameplayFunctions.TurnItemIntoItem(m_ActionDataFishing.m_Bait,m_ActionDataFishing.m_Bait.ConfigGetString("hookType"),m_ActionDataFishing.m_Player);
			}
			else
			{
				m_ActionDataFishing.m_Bait.AddHealth(-m_ActionDataFishing.FISHING_DAMAGE);
			}
			
			float rnd = Math.RandomFloatInclusive(0.0,1.0);
			if (rnd > m_ActionDataFishing.FISHING_GARBAGE_CHANCE)
			{
				if (m_ActionDataFishing.m_IsSurfaceSea)
					fish = ItemBase.Cast(GetGame().CreateObjectEx("Mackerel",m_ActionDataFishing.m_Player.GetPosition(), ECE_PLACE_ON_SURFACE));
				else
					fish = ItemBase.Cast(GetGame().CreateObjectEx("Carp",m_ActionDataFishing.m_Player.GetPosition(), ECE_PLACE_ON_SURFACE));
			}
			else
			{
				if ( !m_ActionDataFishing.m_IsSurfaceSea )
				{
					string junk_type = m_JunkTypes.Get(Math.RandomInt(0,m_JunkTypes.Count()));
					fish = ItemBase.Cast(GetGame().CreateObjectEx(junk_type,m_ActionDataFishing.m_Player.GetPosition(), ECE_PLACE_ON_SURFACE));
					fish.SetHealth("","Health",fish.GetMaxHealth("","Health") * 0.1);
				}
			}
			
			if (fish)
			{
				//Print("---Caught something: " + fish + "---");
				fish.SetWet(0.3);
				if (fish.HasQuantity())
				{
					float coef = Math.RandomFloatInclusive(0.5, 1.0);
					float item_quantity = fish.GetQuantityMax() * coef;
					item_quantity = Math.Round(item_quantity);
					fish.SetQuantity( item_quantity );
				}
			}
			
			m_ActionDataFishing.m_MainItem.AddHealth(-m_ActionDataFishing.FISHING_DAMAGE);
		}
	}
	
	void HandleFishingResultFailure()
	{
		if (!GetGame().IsMultiplayer() || GetGame().IsServer())
		{
			if (!m_ActionDataFishing.m_Bait)
				m_ActionDataFishing.InitBait(ItemBase.Cast(m_ActionDataFishing.m_MainItem.FindAttachmentBySlotName("Hook")));
			
			if (Math.RandomFloatInclusive(0.0,1.0) > m_ActionDataFishing.FISHING_HOOK_LOSS) //loss of worm only
			{
				if (!m_ActionDataFishing.IsBaitEmptyHook())
				{
					m_ActionDataFishing.m_Bait.AddHealth(-m_ActionDataFishing.FISHING_DAMAGE);
					MiscGameplayFunctions.TurnItemIntoItem(m_ActionDataFishing.m_Bait,m_ActionDataFishing.m_Bait.ConfigGetString("hookType"),m_ActionDataFishing.m_Player);
				}
			}
			else //loss of the entire hook
			{
				m_ActionDataFishing.m_Bait.Delete();
			}
			
			m_ActionDataFishing.m_MainItem.AddHealth(-m_ActionDataFishing.FISHING_DAMAGE);
		}
	}
	
	override void OnStateChange(int pOldState, int pCurrentState)
	{
		if (pCurrentState == STATE_NONE && m_ActionDataFishing)
		{
			if (m_ActionDataFishing.m_FishingResult == 1 && pOldState == STATE_LOOP_END)
			{
				HandleFishingResultSuccess();
			}
			else if (m_ActionDataFishing.m_FishingResult == 0 && pOldState == STATE_LOOP_END2)
			{
				HandleFishingResultFailure();
			}
		}
	}
};

class ActionFishingNew: ActionContinuousBase
{
	void ActionFishingNew()
	{
		m_CallbackClass = ActionFishingNewCB;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_MEDIUM;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_FISHING;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#start_fishing";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTWaterSurface(UAMaxDistances.LARGE, UAWaterType.ALL);
	}
	
	override bool HasTarget()
	{
		return true;
	}
	
	override bool HasAlternativeInterrupt()
	{
		return true;
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		ItemBase bait;		
		FishingRod_Base_New rod = FishingRod_Base_New.Cast(item);
		
		if (rod)
			bait = ItemBase.Cast(rod.FindAttachmentBySlotName("Hook"));
		
		if (bait && !bait.IsRuined())
			return true;
		
		return false;
	}
	
	override ActionData CreateActionData()
	{
		FishingActionData action_data = new FishingActionData;
		return action_data;
	}
	
	override bool SetupAction(PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = NULL)
	{
		if ( super.SetupAction( player, target, item, action_data, extra_data))
		{
			vector pos_cursor = action_data.m_Target.GetCursorHitPos();
			if (GetGame().SurfaceIsSea(pos_cursor[0],pos_cursor[2]))
			{
				FishingActionData.Cast(action_data).m_IsSurfaceSea = true;
			}
			FishingRod_Base_New rod = FishingRod_Base_New.Cast(action_data.m_MainItem);
			if (rod)
			{
				FishingActionData.Cast(action_data).m_RodQualityModifier = rod.GetFishingEffectivityBonus();
				FishingActionData.Cast(action_data).InitBait(ItemBase.Cast(action_data.m_MainItem.FindAttachmentBySlotName("Hook")));
			}
			return true;
		}
		return false;
	}
	
	override void OnFinishProgressServer( ActionData action_data )
	{
		action_data.m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_ACTION);
		FishingActionData.Cast(action_data).m_FishingResult = EvaluateFishingResult(action_data);
	}
	
	override void OnFinishProgressClient( ActionData action_data )
	{
		action_data.m_Callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_ACTION);
		FishingActionData.Cast(action_data).m_FishingResult = EvaluateFishingResult(action_data);
	}
	
	override void OnStartClient( ActionData action_data )
	{
		FishingRod_Base_New rod = FishingRod_Base_New.Cast(action_data.m_MainItem);
		rod.AnimateFishingRod(true);
	}
	override void OnStartServer( ActionData action_data )
	{
		FishingRod_Base_New rod = FishingRod_Base_New.Cast(action_data.m_MainItem);
		rod.AnimateFishingRod(true);
	}
	
	override void OnEndClient( ActionData action_data )
	{
		FishingRod_Base_New rod = FishingRod_Base_New.Cast(action_data.m_MainItem);
		rod.AnimateFishingRod(false);
	}
	override void OnEndServer( ActionData action_data )
	{
		FishingRod_Base_New rod = FishingRod_Base_New.Cast(action_data.m_MainItem);
		rod.AnimateFishingRod(false);
	}
	
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		
		if( HasTarget() )
		{
			ctx.Write(action_data.m_Target.GetCursorHitPos());
		}
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data )
	{		
		super.ReadFromContext(ctx, action_recive_data);
		
		if( HasTarget() )
		{
			vector cursor_position;
			if ( !ctx.Read(cursor_position) )
				return false;
			action_recive_data.m_Target.SetCursorHitPos(cursor_position);
		}
		return true;
	}
	
	int EvaluateFishingResult(ActionData action_data)
	{
		if (action_data.m_Player.IsQuickFishing())
			return 1;
		
		FishingActionData fad = FishingActionData.Cast(action_data);
		float rnd = fad.m_Player.GetRandomGeneratorSyncManager().GetRandom01(RandomGeneratorSyncUsage.RGSGeneric);
		float daytime_modifier = 1;
		float hook_modifier = 1;
		float chance;
		
		daytime_modifier = GetGame().GetDayTime();
		if ( (daytime_modifier > 18 && daytime_modifier < 22) || (daytime_modifier > 5 && daytime_modifier < 9) )
		{
			daytime_modifier = 1;
		}
		else
		{
			daytime_modifier = 0.5;
		}
		
		//fishing with an empty hook
		if (fad.IsBaitEmptyHook())
		{
			hook_modifier = 0.05;
		}
		
		chance = 1 - (((fad.FISHING_SUCCESS * daytime_modifier) + fad.m_RodQualityModifier)) * hook_modifier;
		
		if (rnd > chance)
		{
			return 1;
		}
		else if (rnd < fad.FISHING_BAIT_LOSS && !fad.IsBaitEmptyHook()) // restricts the loss of an empty hook (low chance is enough)
		{
			return 0;
		}
		return -1;
	}
};