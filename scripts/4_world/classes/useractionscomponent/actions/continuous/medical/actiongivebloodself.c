class ActionGiveBloodData : ActionData
{
	int m_ItemBloodType;
	float m_BloodAmount;
	int m_Agents;
}

class ActionGiveBloodSelfCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousQuantityBloodTransfer(UAQuantityConsumed.BLOOD, UATimeSpent.BLOOD);
	}
};

class ActionGiveBloodSelf: ActionContinuousBase
{
	const float CHEM_AGENT_BLOOD_REMOVAL_MODIFIER = 0.25;
	void ActionGiveBloodSelf()
	{
		m_CallbackClass = ActionGiveBloodSelfCB;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_HIGH;
		
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_SALINEBLOODBAG;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#give_blood";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINotRuinedAndEmpty;
		m_ConditionTarget = new CCTSelf;
	}

	override bool HasTarget()
	{
		return false;
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
		
		PluginTransmissionAgents plugin = PluginTransmissionAgents.Cast(GetPlugin(PluginTransmissionAgents));
		plugin.TransmitAgentsEx(action_data.m_MainItem, action_data.m_Player, AGT_UACTION_TO_PLAYER,blood_obtained ,action_data_b.m_Agents);
		
		
		int bloodtypetarget = action_data_b.m_Player.GetStatBloodType().Get();
		bool bloodmatch = BloodTypes.MatchBloodCompatibility(action_data_b.m_ItemBloodType, bloodtypetarget);

		if ( !bloodmatch )
		{
			
			
			if (blood_obtained > PlayerConstants.HEMOLYTIC_RISK_SHOCK_THRESHOLD)
			{
				action_data_b.m_Player.m_UnconsciousEndTime = -60;
				action_data_b.m_Player.SetHealth("","Shock",0);

				if (blood_obtained > PlayerConstants.HEMOLYTIC_REACTION_THRESHOLD)
				{
					action_data_b.m_Player.m_ModifiersManager.ActivateModifier(eModifiers.MDF_HEMOLYTIC_REACTION);
				}
			}
		}
		
		if ( action_data_b.m_MainItem && action_data_b.m_MainItem.GetQuantity() <= 0.01 )
		{
			action_data_b.m_MainItem.SetQuantity(0);
		}
		
		if (!(action_data_b.m_Agents & eAgents.CHEMICAL_POISON))//does bloodbag NOT contain nerve agent ?
		{
			float remove_count_agents = blood_obtained * CHEM_AGENT_BLOOD_REMOVAL_MODIFIER;
			action_data.m_Player.InsertAgent(eAgents.CHEMICAL_POISON, -remove_count_agents);
			
		}
	}
};

class SyringeLambda : TurnItemIntoItemLambda
{
	void SyringeLambda (EntityAI old_item, string new_item_type, PlayerBase player) { }
};