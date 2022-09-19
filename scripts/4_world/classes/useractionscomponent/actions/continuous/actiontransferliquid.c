class TransferLiquidActionReciveData : ActionReciveData
{
	bool m_TendencyRecived;
}

class TransferLiquidActionData : ActionData
{
	bool m_Tendency;
};

class ActionTransferLiquidCB : ActionContinuousBaseCB
{
	private const float TIME_TO_REPEAT = 0.25;
	
	override void CreateActionComponent()
	{
		if (TransferLiquidActionData.Cast(m_ActionData))
			m_ActionData.m_ActionComponent = new CAContinuousQuantityLiquidTransfer(UAQuantityConsumed.DRAIN_LIQUID, TIME_TO_REPEAT, TransferLiquidActionData.Cast(m_ActionData).m_Tendency);
		else
			m_ActionData.m_ActionComponent = new CAContinuousQuantityLiquidTransfer(UAQuantityConsumed.DRAIN_LIQUID, TIME_TO_REPEAT);
	}
};

class ActionTransferLiquid: ActionContinuousBase
{
	void ActionTransferLiquid()
	{
		m_CallbackClass = ActionTransferLiquidCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_EMPTY_VESSEL;
		m_CommandUIDProne = DayZPlayerConstants.CMD_ACTIONFB_EMPTY_VESSEL;
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_LOW;
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
	}
	
	override bool HasProneException()
	{
		return true;
	}
	
	override string GetText()
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		if ( player.GetLiquidTendencyDrain() )
			return "#drain_liquid";
		else
			return "#pour_liquid";
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		ItemBase target_item = ItemBase.Cast(target.GetObject());
		
		if ( target_item && item )
		{
			bool can_drain = Liquid.CanTransfer(target_item,item);
			bool can_pour = Liquid.CanTransfer(item,target_item);
			
			if (can_drain && can_pour)
			{
				return true;
			}
			else if (!can_pour && can_drain)
			{
				if ( (!GetGame().IsDedicatedServer()) && !player.GetLiquidTendencyDrain() )
				{
					player.SetLiquidTendencyDrain(true);
					return false;
				}
				else
				{
					return true;
				}
			}
			else if (!can_drain && can_pour)
			{
				if ( (!GetGame().IsDedicatedServer()) && player.GetLiquidTendencyDrain() )
				{
					player.SetLiquidTendencyDrain(false);
					return false;
				}
				else
				{
					return true;
				}
			}
		}
		return false;
	}
	
	override void OnStartClient( ActionData action_data )
	{
		TransferLiquidActionData.Cast(action_data).m_Tendency = action_data.m_Player.GetLiquidTendencyDrain();
	}
	
	override void OnStartServer( ActionData action_data )
	{
		if (!GetGame().IsMultiplayer())
			TransferLiquidActionData.Cast(action_data).m_Tendency = action_data.m_Player.GetLiquidTendencyDrain();
	}
	
	override void OnFinishProgressServer( ActionData action_data )
	{
		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
	}
	
	override ActionData CreateActionData()
	{
		TransferLiquidActionData action_data = new TransferLiquidActionData;
		return action_data;
	}
	
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		
		ctx.Write(action_data.m_Player.GetLiquidTendencyDrain());
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data )
	{
		if(!action_recive_data)
		{
			action_recive_data = new TransferLiquidActionReciveData;
		}
		TransferLiquidActionReciveData recive_data_transfer = TransferLiquidActionReciveData.Cast(action_recive_data);
		
		super.ReadFromContext(ctx, action_recive_data);
		
		bool tendency_drain;
		if ( !ctx.Read(tendency_drain) )
			return false;
		recive_data_transfer.m_TendencyRecived = tendency_drain;
		return true;
	}
	
	override void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		super.HandleReciveData(action_recive_data, action_data);
		
		TransferLiquidActionReciveData recive_data_transfer = TransferLiquidActionReciveData.Cast(action_recive_data);
		TransferLiquidActionData.Cast(action_data).m_Tendency = recive_data_transfer.m_TendencyRecived;
	}
	
	override void OnStartAnimationLoop( ActionData action_data )
	{
		if ( !GetGame().IsMultiplayer() || GetGame().IsServer() )
		{
			Bottle_Base vessel_in_hands = Bottle_Base.Cast( action_data.m_Target.GetObject() );
			Param1<bool> play = new Param1<bool>( true );
			if (TransferLiquidActionData.Cast(action_data))
			{
				if (TransferLiquidActionData.Cast(action_data).m_Tendency == true)
					GetGame().RPCSingleParam( vessel_in_hands, SoundTypeBottle.EMPTYING, play, true );
				else if (!TransferLiquidActionData.Cast(action_data).m_Tendency == false)
					GetGame().RPCSingleParam( vessel_in_hands, SoundTypeBottle.POURING, play, true );
			}
		}
	}
	
	override void OnEndAnimationLoop( ActionData action_data )
	{
		if ( !GetGame().IsMultiplayer() || GetGame().IsServer() )
		{
			Bottle_Base target_vessel = Bottle_Base.Cast( action_data.m_Target.GetObject());
			Param1<bool> play = new Param1<bool>( false );
			if (TransferLiquidActionData.Cast(action_data).m_Tendency)
				GetGame().RPCSingleParam( target_vessel, SoundTypeBottle.EMPTYING, play, true );
			else
				GetGame().RPCSingleParam( target_vessel, SoundTypeBottle.POURING, play, true );
		}
	}
};