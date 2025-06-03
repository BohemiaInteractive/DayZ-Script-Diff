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
	\brief Sets collision cylinder for object, representing cylinder from origin(center) up to defined positive height
		\param radius \p float Radius of cylinder
		\param height \p float Height of cylinder
		\note Automatically sets TriggerShape.CYLINDER
		\n usage :
		@code
			SetCollisionCylinder(3, 6);
		@endcode
	*/
	proto native void SetCollisionCylinder(float radius, float height);
	
	/**
	\brief Sets collision cylinder for object, representing cylinder from origin(center), height can be defined in both directions
		\param radius \p float Radius of cylinder
		\param negativeHeigh \p float Negative height of cylinder
		\param positiveHeight \p float Positive height of cylinder
		\note Automatically sets TriggerShape.CYLINDER
		\n usage :
		@code
			SetCollisionCylinderTwoWay(3, -3, 3);
		@endcode
	*/	
	private proto native void SetCollisionCylinderTwoWayNative(float radius, float negativeHeight, float positiveHeight);
	
	/**
	\brief Input value validated version of SetCollisionCylinderTwoWay
		\param radius \p float Radius of cylinder
		\param negativeHeigh \p float Negative height of cylinder
		\param positiveHeight \p float Positive height of cylinder
		\note Automatically sets TriggerShape.CYLINDER
		\n usage :
		@code
			SetCollisionCylinderTwoWayValidated(3, -3, 3);
		@endcode
	*/	
	void SetCollisionCylinderTwoWay(float radius, float negativeHeight, float positiveHeight)
	{
		if (radius <=0)
		{
			ErrorEx("Radius has to >= 0");
			return;
		}

		if (negativeHeight > positiveHeight)
		{
			ErrorEx("Negative height cannot be higher than positive height");
			return;
		}
		
		SetCollisionCylinderTwoWayNative(radius, negativeHeight, positiveHeight);
	}
		
	//! Set the TriggerShape to be used, default is TriggerShape.BOX
	proto native void SetTriggerShape(TriggerShape shape);
	
	//! Get the current TriggerShape
	proto native TriggerShape GetTriggerShape();
	
	override bool IsInventoryVisible()
	{
		return false;
	}
}
