class ActionGiveBloodTargetCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousQuantityBloodTransfer(UAQuantityConsumed.BLOOD, UATimeSpent.BLOOD);
	}
};

class ActionGiveBloodTarget: ActionContinuousBase
{
	void ActionGiveBloodTarget()
	{
		m_CallbackClass = ActionGiveBloodTargetCB;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_HIGH;
		
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_SALINEBLOODBAGTARGET;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#give_blood_person";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionTarget = new CCTMan(UAMaxDistances.DEFAULT);
		m_ConditionItem = new CCINonRuined;
	}
	
	override ActionData CreateActionData()
	{
		ActionGiveBloodData action_data = new ActionGiveBloodData;
		return action_data;
	}
	
	override bool SetupAction(PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = NULL )
	{
		if ( super.SetupAction(player, target, item, action_data, extra_data ) )
		{
			ActionGiveBloodData action_data_b = ActionGiveBloodData.Cast( action_data );
			action_data_b.m_ItemBloodType = action_data.m_MainItem.GetLiquidType();
			action_data_b.m_BloodAmount = action_data.m_MainItem.GetQuantity();
			action_data_b.m_Agents = action_data.m_MainItem.GetAgents();
			return true;
		}
		return false;
	}

	override void OnEndAnimationLoopServer( ActionData action_data )
	{
		//OnFinishProgressServer(action_data);

		if ( action_data.m_MainItem.IsKindOf("BloodSyringe") )
		{
			SyringeLambda lambda = new SyringeLambda(action_data.m_MainItem, "Syringe", action_data.m_Player);
			lambda.SetTransferParams(true, true, true);
			MiscGameplayFunctions.TurnItemIntoItemEx(action_data.m_Player, lambda);
		}
		
		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
	}
	
	override void OnEndServer(ActionData action_data)
	{
		ActionGiveBloodData action_data_b = ActionGiveBloodData.Cast( action_data );
		float blood_obtained = action_data_b.m_BloodAmount - action_data_b.m_MainItem.GetQuantity();
		PlayerBase player_target = PlayerBase.Cast(action_data_b.m_Target.GetObject());
		
		
		PluginTransmissionAgents plugin = PluginTransmissionAgents.Cast(GetPlugin(PluginTransmissionAgents));
		plugin.TransmitAgents(action_data.m_MainItem, player_target, AGT_UACTION_TO_PLAYER,blood_obtained );
		
		
		int bloodtypetarget = player_target.GetStatBloodType().Get();
		bool bloodmatch = BloodTypes.MatchBloodCompatibility(action_data_b.m_ItemBloodType, bloodtypetarget);
		
		if ( !bloodmatch )
		{
			
			
			if (blood_obtained > PlayerConstants.HEMOLYTIC_RISK_SHOCK_THRESHOLD)
			{
				player_target.m_UnconsciousEndTime = -60;
				player_target.SetHealth("","Shock",0);

				if (blood_obtained > PlayerConstants.HEMOLYTIC_REACTION_THRESHOLD)
				{
					player_target.m_ModifiersManager.ActivateModifier(eModifiers.MDF_HEMOLYTIC_REACTION);
				}
			}
		}
		
		if ( action_data_b.m_MainItem && action_data_b.m_MainItem.GetQuantity() <= 0.01 )
		{
			action_data_b.m_MainItem.SetQuantity(0);
		}
		
		if (!(action_data_b.m_Agents & eAgents.CHEMICAL_POISON))//does bloodbag NOT contain nerve agent ?
		{
			float remove_count_agents = blood_obtained * ActionGiveBloodSelf.CHEM_AGENT_BLOOD_REMOVAL_MODIFIER;
			player_target.InsertAgent(eAgents.CHEMICAL_POISON, -remove_count_agents);
			
		}
	}
};