class ActionCollectBloodTargetCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.COLLECT_BLOOD);
	}
};

class ActionCollectBloodTarget: ActionContinuousBase
{
	void ActionCollectBloodTarget()
	{
		m_CallbackClass = ActionCollectBloodTargetCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_SALINEBLOODBAGTARGET;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_HIGH;
		m_Text = "#collect_persons_blood";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTMan( UAMaxDistances.DEFAULT );
	}
	
	override void OnEndServer( ActionData action_data )
	{
		PlayerBase ntarget = PlayerBase.Cast( action_data.m_Target.GetObject() );
		
		if ( !ntarget )
			return;
		
		Param1<float> nacdata;
		Class.CastTo( nacdata, action_data.m_ActionComponent.GetACData() );
		float delta = 0;
		if ( nacdata )
			delta = ( nacdata.param1 / UATimeSpent.COLLECT_BLOOD );
		
		if ( delta > 0 )
		{
			ActionCollectBloodTargetLambda lambda = new ActionCollectBloodTargetLambda(action_data.m_MainItem, "BloodBagFull", action_data.m_Player, m_SpecialtyWeight, ntarget, delta);
			action_data.m_Player.ServerReplaceItemInHandsWithNew(lambda);
		}
		//action_data.m_Player.GetItemInHands().SetQuantity();
	}
};

class ActionCollectBloodTargetLambda : ReplaceItemWithNewLambda
{
	float m_SpecialtyWeight;
	float m_percentFilled;
	PlayerBase m_Target;

	void ActionCollectBloodTargetLambda(EntityAI old_item, string new_item_type, PlayerBase player, float specialtyWeight, PlayerBase target, float delta) { m_SpecialtyWeight = specialtyWeight; m_Target = target; m_percentFilled = delta; }

	override void CopyOldPropertiesToNew(notnull EntityAI old_item, EntityAI new_item)
	{
		float maxQuantity;
		float quantitySet;
		super.CopyOldPropertiesToNew(old_item, new_item);

		if ( new_item )
		{
			ItemBase old_item_IB = ItemBase.Cast(old_item);
			ItemBase new_item_IB = ItemBase.Cast(new_item);
			
			ItemBase.Cast( old_item ).TransferModifiers( m_Target );
			MiscGameplayFunctions.TransferItemProperties( old_item_IB, new_item_IB, true, false, true );
			ItemBase.Cast( new_item ).SetLiquidType( m_Target.GetStatBloodType().Get() );
			m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
			
			maxQuantity = ItemBase.Cast( new_item ).GetQuantityMax();
			quantitySet = m_percentFilled * maxQuantity;
			ItemBase.Cast( new_item ).SetQuantity( quantitySet );
			m_Target.AddHealth( "", "Blood", -quantitySet );
			
			if ( m_Player.HasBloodTypeVisible() )
				BloodContainerBase.Cast( new_item ).SetBloodTypeVisible( true );
			PluginTransmissionAgents plugin = PluginTransmissionAgents.Cast(GetPlugin(PluginTransmissionAgents));
			plugin.TransmitAgents(m_Target, new_item, AGT_UACTION_TO_ITEM);
		}
		else
		{
			Debug.LogError("ActionCollectBloodTargetLambda: failed to create new item","static");
		}
	}
};
