//! Trigger with cylinder shape
class CylinderTrigger : Trigger
{
	override void EOnInit(IEntity other, int extra)
	{
		SetCollisionCylinderTwoWay(1, -0.25, 0.25);
	}

#ifdef DIAG_DEVELOPER
	override void DebugSendDmgTrigger()
	{		
		vector minmax[2];
		GetCollisionBox(minmax);
			
		DebugTriggerInfo data = new DebugTriggerInfo(vector.Zero, vector.Zero, vector.Zero, vector.Zero, 0, "", null);
		data.param1 = GetPosition();
		data.param2 = GetOrientation();		
		data.param3 = minmax[0];
		data.param4 = minmax[1];
		data.param5 = GetCollisionRadius();		
		data.param6 = m_DebugAreaType;
		data.param7 = m_insiders;
		
		if (GetGame().IsMultiplayer() && GetGame().IsServer())
			PluginDiagMenuServer.SendDataToSubscribersServer(this, ESubscriberSystems.TRIGGERS, ERPCs.DIAG_TRIGGER_DEBUG, data, false);
		else if (!GetGame().IsMultiplayer() || m_Local)
			DebugDmgTrigger(data.param1, data.param2, data.param3, data.param4, data.param5, data.param6, data.param7);
	}	

	override protected Shape DrawDebugShape(vector pos, vector min, vector max, float radius, int color)
	{
		//! min/max taken from bbox (which goes from center to top of the cylinder only; min is always negative)
		float triggerHeight = (max[1] - min[1]);

		Shape dbgShape = Debug.DrawCylinder(pos, radius, triggerHeight, color, ShapeFlags.TRANSP|ShapeFlags.NOZWRITE|ShapeFlags.DOUBLESIDE);		
		dbgShape.SetPosition(pos);

		dbgTargets.Insert(dbgShape);

		return dbgShape;
	}
#endif
}
