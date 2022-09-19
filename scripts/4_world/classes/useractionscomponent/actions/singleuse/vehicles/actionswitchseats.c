
class ActionSwitchSeats: ActionBase
{
	void ActionSwitchSeats()
	{
		//m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_STARTENGINE;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ALL;
		m_SpecialtyWeight = 1.0;
		m_LockTargetOnUse = false;
		m_Text = "#change_seat";
	}

	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTObject;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		
		Transport trans = null;
		int nextSeatIdx = -1;
		
		HumanCommandVehicle vehCommand = player.GetCommand_Vehicle();

		if ( !vehCommand  )
			return false;
		
		int componentIndex = target.GetComponentIndex();

		if ( !target )
			return false;

		if ( !Class.CastTo(trans, target.GetObject()) )
			return false;

		nextSeatIdx = trans.CrewPositionIndex( componentIndex );

		if ( nextSeatIdx < 0 )
			return false;

		Human crew = trans.CrewMember( nextSeatIdx );
		if ( crew )
			return false;

		if ( !trans.CanReachSeatFromSeat( trans.CrewMemberIndex( player ), nextSeatIdx ) )
			return false;

		return true;
	}

	override void Start( ActionData action_data )
	{
		super.Start( action_data );
		HumanCommandVehicle vehCommand = action_data.m_Player.GetCommand_Vehicle();
		if ( vehCommand )
		{
			Transport trans;
			if ( Class.CastTo(trans, action_data.m_Target.GetObject()) )
			{
				int nextSeat = trans.CrewPositionIndex( action_data.m_Target.GetComponentIndex() );
				int seat = trans.GetSeatAnimationType( nextSeat );
				if ( seat >= 0 )
				{
					//pTransportPositionIndex, int pVehicleSeat
					vehCommand.SwitchSeat( nextSeat, seat );
					if ( !GetGame().IsDedicatedServer() )
					{

						action_data.m_Player.OnVehicleSwitchSeat( nextSeat );
					}
				}
			}
		}
	}
	
	override void OnUpdate(ActionData action_data)
	{

		if(action_data.m_State == UA_START)
		{
			HumanCommandVehicle hcv = action_data.m_Player.GetCommand_Vehicle();
			if( !hcv || !action_data.m_Player.GetCommand_Vehicle().IsSwitchSeat() )
			{
				End(action_data);
			}
		}
	}
	
	override bool CanBeUsedInVehicle()
	{
		return true;
	}
};