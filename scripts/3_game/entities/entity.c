class Entity extends ObjectTyped
{
	proto native void DisableSimulation(bool disable);

	//! Returns whether simulation is disabled
	proto native bool GetIsSimulationDisabled();
	
	//! Returns simulation timestamp
	proto native int GetSimulationTimeStamp();

	//! Return animation phase of animation on object.
	proto native float GetAnimationPhase(string animation);
	
	//! Process animation on object. Animation is defined in config file. Wanted animation phase is set to phase.
	proto native void SetAnimationPhase(string animation, float phase);
	
	//! Same as SetAnimationPhase, only ignores any animation and sets the phase immediately
	void SetAnimationPhaseNow(string animation, float phase)
	{
		ResetAnimationPhase(animation, phase);
		SetAnimationPhase(animation, phase);
	}
		
	proto native void ResetAnimationPhase(string animation, float phase);
	
	/**
	\brief callback called from C++ when AnimationPhase has started
	*/
	void OnAnimationPhaseStarted(string animSource, float phase);

	//! Returns skeleton's bone index of named proxy selection.
	proto native int GetBoneIndex( string proxySelectionName );

	//! Returns proxy object that corresponds given bone inside skeleton.
	proto native Object GetBoneObject( int boneIndex );
	
	//! Turns on/off invisibility
	proto native void SetInvisible(bool invisible);

	/**
	\brief On server, entity's transformation is directly changed to targetTransform, on client entity's transformation is interpolated to targetTransform in time deltaT
	@param targetTransform
	@param deltaT, interpolation time between current transformation and target transformation 
	\note it's not recommended to call this on client as it can break interpolation process if called on server previously
	*/	
	proto        void MoveInTime(vector targetTransform[4], float deltaT);
	
	/**
	\brief callback called when entity is moved to world and creating its physics representation
	*/
	void OnCreatePhysics();
	
	/**
	\brief Client event on transformation update from network
	@param pos, world space position
	@param ypr, world space orientation in radians in form of Yaw/Pitch/Roll
	@return true if visual cut (won't do any interpolation) should be done after calling this callback
	*/
	bool OnNetworkTransformUpdate(out vector pos, out vector ypr);
};
