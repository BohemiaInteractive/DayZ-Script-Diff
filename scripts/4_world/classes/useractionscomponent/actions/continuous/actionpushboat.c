class CAContinuousRepeatPushBoat : CAContinuousRepeatPushObject
{
	override float GetProgressWidgetMultiplier()
	{
		return 4;
	}
}

class ActionPushBoatCB : ActionPushObjectCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeatPushBoat(UATimeSpent.PUSH_BOAT);
	}
}

class ActionPushBoat : ActionPushObject
{	
	void ActionPushBoat()
	{
		m_CallbackClass	= ActionPushBoatCB;
		m_CommandUID 	= DayZPlayerConstants.CMD_ACTIONFB_VEHICLE_PUSH;
		m_FullBody 		= true;
		m_StanceMask 	= DayZPlayerConstants.STANCEMASK_ERECT;

		m_Text = "#STR_push_boat";
	}
		
	override void OnStartServer(ActionData action_data)
	{
		super.OnStartServer(action_data);
		
		BoatScript boat = BoatScript.Cast(action_data.m_Target.GetObject());
		if (boat)
			boat.SyncSoundPushBoat(true);
	}
	
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		
		BoatScript boat = BoatScript.Cast(action_data.m_Target.GetObject());
		if (boat)
			boat.SyncSoundPushBoat(false);
	}
}
