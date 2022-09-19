class ActionSewTargetCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.SEW_CUTS);
	}
};

class ActionSewTarget: ActionBandageBase
{
	void ActionSewTarget()
	{
		m_CallbackClass = ActionSewTargetCB;
		//m_Animation = "sew";
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_MEDIUM;
		m_Text = "#sew_targets_cuts";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTMan(UAMaxDistances.DEFAULT);
	}
	/*
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		PlayerBase other_player = PlayerBase.Cast(target.GetObject());
		return other_player.IsBleeding();
	}
*/
	override void OnFinishProgressServer( ActionData action_data )
	{
		PlayerBase target = PlayerBase.Cast(action_data.m_Target.GetObject());
		
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