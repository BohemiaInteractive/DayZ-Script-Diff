enum TriggerShape
{
	BOX,
	SPHERE,
	CYLINDER,
}

class ScriptedEntity extends EntityAI
{
	/**
	\brief Sets collision properties for object
		\param mins \p vector Min values of box
		\param maxs \p vector Max values of box
		\param radius \p float Radius of bounding sphere
		\note This function is obsolete, use rather SetCollisionBox()
	*/
	proto native void SetClippingInfo(vector mins, vector maxs, float radius);
	
	/**
	\brief Sets collision box for object
		\param mins \p vector Min values of box
		\param maxs \p vector Max values of box
		\note Automatically sets TriggerShape.BOX
		\n usage :
		@code
			vector mins = "-1 -1 -1";
			vector maxs = "1 1 1";
			SetCollisionBox(mins, maxs);
		@endcode
	*/
	proto native void SetCollisionBox(vector mins, vector maxs);
		
	/**
	\brief Sets collision sphere for object
		\param radius \p float Radius of cylinder
		\note Automatically sets TriggerShape.SPHERE
		\n usage :
		@code
			SetCollisionSphere(3);
		@endcode
	*/
	proto native void SetCollisionSphere(float radius);
	
	/**
	\brief Sets collision cylinder for object
		\param radius \p float Radius of cylinder
		\param height \p float Height of cylinder
		\note Automatically sets TriggerShape.CYLINDER
		\n usage :
		@code
			SetCollisionCylinder(3, 6);
		@endcode
	*/
	proto native void SetCollisionCylinder(float radius, float height);	
		
	//! Set the TriggerShape to be used, default is TriggerShape.BOX
	proto native void SetTriggerShape(TriggerShape shape);
	
	//! Get the current TriggerShape
	proto native TriggerShape GetTriggerShape();
	
	override bool IsInventoryVisible()
	{
		return false;
	}
};