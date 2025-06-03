#ifdef FEATURE_NETWORK_RECONCILIATION

//! TODO(kumarjac): Create and link external documentation.

//!	Glossary:
//!		Authority: The server
//!		Owner: The client that is controlling the pawn
//!		Proxy/Remote: The client(s) that aren't controlling the pawn

//! NOTE: Do not depend on Serializer. 
//! 	Future updates may break compatibility if you diverge from only using Serializer.Write/Serializer.Read
//! 	Serializer is not optimized for network traffic. 

typedef Serializer PawnStateWriter;
typedef Serializer PawnStateReader;

typedef Serializer PawnMoveWriter;
typedef Serializer PawnMoveReader;

/**
PawnOwnerState
Contains the full state of the Pawn for purposes of full state synchronization. 
Purpose is to be infrequently sent to the owner when there is a desync detected in playing of moves, triggering a replay on the owner.
*/
class PawnOwnerState
{
	protected void PawnOwnerState() {}
	protected void ~PawnOwnerState() {}
	
	proto native void	SetMoveId(int value);
	proto native int	GetMoveId();

	proto native int	GetSimulationTimestamp();
	
	proto native void	SetPhysicsTimeStamp(int value);
	proto native int	GetPhysicsTimeStamp();
	
	proto native void	SetWaterTime(float value);
	proto native float	GetWaterTime();
	
	protected event void Write(PawnStateWriter ctx)
	{
	}
	
	protected event void Read(PawnStateReader ctx)
	{
	}
	
#ifdef DIAG_DEVELOPER
	protected event void DiagWrite(PawnStateWriter ctx)
	{
	}
	
	protected event void DiagRead(PawnStateReader ctx)
	{
	}
#endif
	
	/**
	\brief Estimate the additional maximum size for network buffer transmission on top of the native size
	*/
	protected event int EstimateMaximumSize()
	{
		return 0;
	}

#ifdef DIAG_DEVELOPER
	/**
	\brief Event used only for diagnostic purposes
	@param transform, World space transform of the move
	*/
	event void GetTransform(inout vector transform[4])
	{
	}
#endif
};

/**
PawnMove
Contains information about the move to be used on the owner and sent to the authority.
Authority will consume and play the same move. When desync is detected the moves that haven't been acknlowedged by the authority yet are replayed.
*/
class PawnMove
{
	protected void PawnMove() {}
	protected void ~PawnMove() {}
	
	proto native void	SetMoveId(int value);
	proto native int	GetMoveId();
	
	//! Not avaliable on server
	proto native void	SetSimulationTimestamp(int value);
	proto native int	GetSimulationTimestamp();
	
	//! Not avaliable on server
	proto native void	SetTimeSlice(float value);
	proto native float	GetTimeSlice();
	
	/**
	 * @ctx Stream to write the data to
	 * @prev Previous move for delta encoding
	 */
	protected event void Write(PawnMoveWriter ctx, PawnMove prev)
	{
	}
	
	/**
	 * @ctx Stream to read the data from
	 * @prev Previous move for delta encoding
	 */
	protected event void Read(PawnMoveReader ctx, PawnMove prev)
	{
	}
	
	/**
	\brief Estimate the additional maximum size for network buffer transmission on top of the native size
	*/
	protected event int EstimateMaximumSize()
	{
		return 0;
	}

#ifdef DIAG_DEVELOPER
	/**
	\brief Event used only for diagnostic purposes
	@param transform, World space transform of the move
	*/
	event void GetTransform(inout vector transform[4])
	{
	}
#endif
};

/**
NetworkMoveStrategy
The strategy with which the Pawn is reconciled on the owner
*/
enum NetworkMoveStrategy
{
	//! Has no support for NetworkMovement
	NONE,

	//! Places the move as the last input inside 'NetworkInput' and simulates with the input message
	LATEST,

	//! Sends over a fixed buffer of moves and re-simulates the physics steps on correction as a static scene
	PHYSICS,
};

/**
NetworkRewindType
The rewind type with which the Pawn is reconciled on the owner
*/
enum NetworkRewindType
{
	//! Rewind type hasn't been set yet by C++, meaning RewindState can process through any of the paths below. Internally treated as REPLAY
	//! NOTE: DROP may stil be set but it could cause destruction of state which would require additional states to be sent
	NOT_SET,
	
	//! Rewind can't be perfomed, we treat this rewind as just acknlowledgement
	DROP,

	//! Rewind requests moves to be replayed
	REPLAY,

	//! Rewind requests special behaviour where the moves just become additive, akin to how BacktrackingComponent works 
	ADDITIVE
};

/**
NetworkCompareResult
Result of comparison for how correction or approval should proceed
*/
enum NetworkCompareResult
{
	//! Approve client simulation as within bounds
	APPROVE,

	//! Requests correction if necessary
	CORRECT,

	//! Ignores RTT delay and forces a correction - should be used minimally as it is expensive on network traffic
	FORCE_CORRECT
};

/**
Pawn
An entity that can be possessed. Only one entity can be possesed by one PlayerIdentity at a single time. 
Meaning when something other than a Player is possessed, the Player can no longer be reconciled and desync can occur.
*/
class Pawn : EntityAI
{
	//! Returns true if the Pawn is being simulated by the owner
	proto native bool IsOwner();
	
	//! Returns true if the Pawn is being simulated on an authority
	proto native bool IsAuthority();
	
	//! Returns true if the Pawn is being simulated on an authority and has no owner
	proto native bool IsAuthorityOwner();
	
	//! Returns true if the Pawn is being processed on a proxy
	proto native bool IsProxy();

	//! Force a correction to the owner as a server only event was called (called on authority only)
	proto native void ForceCorrection();
	
	//! Returns the identity that is in possession of the Pawn
	proto native PlayerIdentity GetOwnerIdentity();
	
	//! Returns the strategy with which the pawn is reconciled on the Owner
	proto native NetworkMoveStrategy GetNetworkMoveStrategy();
	
	/**
	\brief Event that fires when the Pawn is possessed
	*/
	protected event void OnPossess()
	{
	}

	/**
	\brief Event that fires when the Pawn is no longer possessed
	*/
	protected event void OnUnPossess()
	{
	}

	/**
	\brief Initializes the NetworkMovement with the type of PawnOwnerState at construction
	\return The typename of the PawnOwnerState
	*/
	protected event typename GetOwnerStateType()
	{
		return PawnOwnerState;
	}
	
	/**
	\brief Initializes the NetworkMovement with the type of PawnMove at construction and for storing
	\return The typename of the PawnMove
	*/
	protected event typename GetMoveType()
	{
		return PawnMove;
	}

	/**
	\brief Fill the move of the pawn with the inputs for the pawn to be replayed
	@param move, Move which the variables are being written to
	*/
	protected event void ObtainMove(/*inout*/ PawnMove pMove)
	{
	}
	
	/**
	\brief Compares the incoming move from the owner on the authority
	@param move The incoming move
	@return result, also performed natively, both results are compared and higher one is used
	*/
	protected event NetworkCompareResult CompareMove(PawnMove pMove)
	{
		return NetworkCompareResult.APPROVE;
	}

	/**
	\brief Apply the moves inputs on the server
	@param move The next move to be consumed and applied on the authority
	*/
	protected event void ConsumeMove(PawnMove pMove)
	{
	}

	/**
	\brief Replay the move on the owner client
	@param move The incoming move
	@return false if further moves should not be replayed
	*/
	protected event bool ReplayMove(PawnMove pMove)
	{
		return true;
	}

	/**
	\brief Simulate the replayed move on the owner client
	@param move The incoming move
	*/
	protected event void SimulateMove(PawnMove pMove)
	{
	}

	/**
	\brief Fill the state of the pawn with it to be approriately rewound
	@param state The state which the variables are being written to
	*/
	protected event void ObtainState(/*inout*/ PawnOwnerState pState)
	{
	}

	/**
	\brief Replays the state, setting the internal variables of the entity back to the state. Decides the method for rewinding
	@param state The state recieved from the server to which the client is rewinding back to 
	@param move The move generated at this state, which is modified if NetworkRewindType.ADDITIVE is used and then becomes the delta move, (see ReplayAdditiveMove)
	@param rewindType The method which replay will execute in
	*/
	protected event void RewindState(PawnOwnerState pState, /*inout*/ PawnMove pMove, inout NetworkRewindType pRewindType)
	{
	}

	/**
	\brief Iterator function over moves ahead of the authority. Intended for the incoming move to be combined with the delta move.
	@param pMove The incoming move which is being updated
	@param pDeltaMove The delta of the move between the server and client state at the time of RewindState
	*/
	protected event void ReplayAdditiveMove(/*inout*/ PawnMove pMove, /*const*/ PawnMove pDeltaMove)
	{
	}

	/**
	\brief Applying the delta move to the pawns internal state. Instead of Rewind, this is effectively (T1 = T + D)
	@param pDeltaMove The delta of the move between the server and client state at the time of RewindState
	*/
	protected event void ApplyAdditiveMove(PawnMove pDeltaMove)
	{
	}
};

#endif
