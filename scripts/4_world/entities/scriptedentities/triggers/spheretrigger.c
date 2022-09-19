//! Trigger with spherical shape
class SphereTrigger : Trigger
{
	override void EOnInit(IEntity other, int extra)
	{
		SetCollisionSphere(3);
	}
	
#ifdef DEVELOPER
	override protected Shape DrawDebugShape(vector pos, vector min, vector max, float radius, int color)
	{
		Shape dbgShape = Debug.DrawSphere(pos, radius, color, ShapeFlags.TRANSP|ShapeFlags.NOZWRITE|ShapeFlags.NOOUTLINE|ShapeFlags.DOUBLESIDE);
		dbgTargets.Insert( dbgShape );
		return dbgShape;
	}
#endif
};