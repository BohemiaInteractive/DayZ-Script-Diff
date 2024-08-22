#ifdef FEATURE_NETWORK_RECONCILIATION

/**
 * The replay system only replays outputs from animation. It does not replay animations and does not replay actual inputs. 
 * This means very often when a desync is detected, multiple corrections have to be sent before the simulation is stable. 
 * This is why we only ever actually use this system for when a player is in HumanCommandMove and HumanCommandFall.
 * 
 * For some situations, such as swimming, it is not possible to ever correct as the water alignment heavily depends on re-simulaton of 
 * animations, which is not happening. With this,  we use MINIMAL correction mode. This minimal correction mode is something similiar
 * to how the old correction system used to work and could lead to situations such as being stuck behind doors. 
 * This is not a problem for swimming.
 */

enum AnimPhysCorrectionType
{
	//! Additively apply the correction based on the delta and sends very minimal data, uses 'NetworkCompareResult.FORCE_CORRECT'
	MINIMAL,
	
	//! No correction is performed
	DROP,

	//! Rewind the player physics and replay the moves
	FULL
};

class AnimPhysOwnerState : PawnOwnerState
{
	proto native void	SetCorrectionType(int value);
	proto native int	GetCorrectionType();

	proto native void	SetPosition(vector value);
	proto native void	GetPosition(out vector value);

	proto native void	SetRotation(float value[4]);
	proto native void	GetRotation(out float value[4]);

	//! If parent entity doesn't exist for the owner then the correction is dropped
	proto native void	SetParent(Object value);
	proto native Object	GetParent();

	//! If linked entity doesn't exist for the owner then the correction is dropped
	proto native void	SetLinked(Object value);
	proto native Object	GetLinked();

	proto native bool	IsLinked();
	proto native bool	IsParented();

	proto native void	SetSpeed(vector value);
	proto native void	GetSpeed(out vector value);

	proto native void	SetCollisionLayer(int value);
	proto native int	GetCollisionLayer();

	proto native void	SetFlags(int value);
	proto native int	GetFlags();

	proto native void	SetCollisionOffset(vector value);
	proto native int	GetCollisionOffset(out vector value);

	proto native void	SetStance(int value);
	proto native int	GetStance();

#ifdef DIAG_DEVELOPER
	override event void	GetTransform(inout vector transform[4])
	{
		float rotation[4];
		GetRotation(rotation);
		Math3D.QuatToMatrix(rotation, transform);

		vector position;
		GetPosition(position);
		transform[3] = position;

		Object parent = GetParent();
		if (parent)
		{
			vector parentTransform[4];
			parent.GetTransform(parentTransform);
			Math3D.MatrixMultiply4(parentTransform, transform, transform);
		}
	}
#endif

};

class AnimPhysMove : PawnMove
{
	proto native void	SetCorrectionType(int value);
	proto native int	GetCorrectionType();

	proto native void	SetPosition(vector value);
	proto native void	GetPosition(out vector value);

	proto native void	SetRotation(float value[4]);
	proto native void	GetRotation(out float value[4]);

	proto native void	SetParent(Object value);
	proto native void	SetLinked(Object value);
	
	proto native Object	GetParentOrLinked();
	proto native Object	GetParent();

	proto native bool	HasParent();

	proto native bool	IsParented();
	proto native bool	IsLinked();

	//! Parent (or linked) entity could've been deleted, store the necessary values so we have something to work with
	proto native void	SetParentTransform(vector transform[4]);
	proto native void	GetParentTransform(out vector transform[4]);

#ifdef DIAG_DEVELOPER
	override event void	GetTransform(inout vector transform[4])
	{
		float rotation[4];
		GetRotation(rotation);
		Math3D.QuatToMatrix(rotation, transform);

		vector position;
		GetPosition(position);
		transform[3] = position;

		if (HasParent())
		{
			vector parentTransform[4];
			GetParentTransform(parentTransform);
			Math3D.MatrixMultiply4(parentTransform, transform, transform);
		}
	}
#endif

};

#endif
