//! Trigger with cylinder shape
class CylinderTrigger : Trigger
{
	override void EOnInit(IEntity other, int extra)
	{
		SetCollisionCylinder(1, 0.5);
	}
	
#ifdef DEVELOPER
	override protected Shape DrawDebugShape(vector pos, vector min, vector max, float radius, int color)
	{
		float height = max[1];
		
		Shape dbgShape = Debug.DrawCylinder(Vector(0, height * 0.5, 0), radius, height, color, ShapeFlags.TRANSP|ShapeFlags.NOZWRITE|ShapeFlags.DOUBLESIDE);
		
		vector mat[4];
		GetTransform( mat );
		dbgShape.CreateMatrix( mat );
		dbgShape.SetMatrix( mat );
		
		dbgTargets.Insert( dbgShape );
		return dbgShape;
	}
#endif
};