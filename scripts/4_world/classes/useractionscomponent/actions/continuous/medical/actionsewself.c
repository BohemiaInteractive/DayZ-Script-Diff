class ActionSewSelfCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.SEW_CUTS);
	}
};

class ActionSewSelf: ActionBandageBase
{
	void ActionSewSelf()
	{
		m_CallbackClass = ActionSewSelfCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_STITCHUPSELF;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_MEDIUM;
		m_Text = "#sew_cuts";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTSelf;
	}

	override bool HasTarget()
	{
		return false;
	}
	/*
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		return player.IsBleeding();
	}
*/
	override void OnFinishProgressServer( ActionData action_data )
	{
		PlayerBase target = PlayerBase.Cast(action_data.m_Player);
		if(action_data.m_MainItem && target)
		{
			ApplyBandage( action_data.m_MainItem, target );
			action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
		}
	}
	
	override void ApplyBandage( ItemBase item, PlayerBase player )
	{	
		if (player.GetBleedingManagerServer() )
		{
			player.GetBleedingManagerServer().RemoveMostSignificantBleedingSourceEx(item);	
		}
		
		PluginTransmissionAgents m_mta = PluginTransmissionAgents.Cast(GetPlugin(PluginTransmissionAgents));
		m_mta.TransmitAgents(item, player, AGT_ITEM_TO_FLESH);
		
		if (item.HasQuantity())
		{
			item.AddQuantity(-20,true);
		}
		else
		{
			item.Delete();
		}
	}
};