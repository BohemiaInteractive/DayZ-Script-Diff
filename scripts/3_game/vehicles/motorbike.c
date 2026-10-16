//!	Motorbike's sound controller list. (native, do not change or extend)
enum MotorbikeSoundCtrl
{
	// simulation
	ENGINE, //!< indicates if engine is ON
	RPM,
	SPEED,  //!< speed of the bike in km/h

	// miscellaneous
	PLAYER //!< indicates if driver is controlled by player
};

//!	Type of vehicle's fluid. (native, do not change or extend)
enum MotorbikeFluid : ETransportFluid {}

class MotorbikeType : TransportType
{
};

class MotorbikeOwnerState : TransportOwnerState
{

}

class MotorbikeMove : TransportMove
{
}

//!	Native class for motorbikes - handles physics simulation
class Motorbike extends Transport
{
	//!
	protected override event typename GetOwnerStateType()
	{
		return MotorbikeOwnerState;
	}

	//!
	protected override event typename GetMoveType()
	{
		return MotorbikeMove;
	}
	
	//!	Returns the current speed of the vehicle in km/h.
	float GetSpeedometer()
	{
		vector transform[4];
		GetTransform(transform);

		float velocity = GetVelocity(this).InvMultiply3(transform)[2];
		return velocity * 3.6;
	}
	
	//! Returns the current speed of the vehicle in km/h. Value is absolute
	float GetSpeedometerAbsolute()
	{
		return Math.AbsFloat(GetSpeedometer());
	}

	override bool IsAreaAtDoorFree( int currentSeat, float maxAllowedObjHeight, inout vector extents, out vector transform[4] )
	{
		return true;
	}

	override bool IsAreaAtDoorFree( int currentSeat, float maxAllowedObjHeight = 0.5, float horizontalExtents = 0.5, float playerHeight = 1.7 )
	{
		return true;
	}

	override Shape DebugFreeAreaAtDoor( int currentSeat, float maxAllowedObjHeight = 0.5, float horizontalExtents = 0.5, float playerHeight = 1.7 )
	{
		return null;
	}
	
	protected bool DetectFlippedUsingWheels(VehicleFlippedContext ctx, bool disallowSide)
	{
		if (disallowSide && (vector.Dot(GetDirectionUp(), vector.Up) < 0.7))
		{
			// return as "flipped", vehicle isn't pointing enough up to be reasonably certain
			return true;
		}
		
		int wheelCount = WheelCount();
		
		for (int wheelIdx = 0; wheelIdx < wheelCount; wheelIdx++)
		{
			if (!WheelHasContact(wheelIdx))
			{
				// wheel not in contact, then we could be flipped, we assume there exist other predicates
				return true;
			}
		}
		
		// all wheels in contact (or zero registered wheels), then we are in contact
		return false;
	}
	

	//!	Returns the current steering value in range <-1, 1>.
	proto native float GetSteering();

	//! Sets the future steering value.
	proto native void SetSteering(float value);

	//! Query if the motorbike can be picked back up
	proto native bool CanPickUp();
	
	//! Picks the bike back up when it is fallen over
	proto native void PickUp();

	//! Returns if the bike is fallen over
	proto native bool IsFallen();
	
	//! Forcibly topple over the motorbike, ejects the player
	proto native bool FallOver();

	//!	Returns the actual throttle value in range <0, 1>.
	proto native float GetThrottle();
	
	//! Sets the future throttle value.
	proto native void SetThrottle(float value);

	//! Returns the value of how much the clutch is disengaged.
	proto native int GetClutch();

	//! Sets the future clutch value.
	proto native void SetClutch(float value);

	//! Returns the current rear brake value in range <0, 1>.
	proto native float GetBrake();

	//! Sets the future rear brake value
	proto native void SetBrake(float value);

	//! Returns the current front brake value in range <0, 1>.
	proto native float GetHandbrake();

	//! Sets the future front brake value
	proto native void SetHandbrake(float value);
	
	//! Returns the current kickstand value in range <0, 1>.
	proto native float GetKickstand();

	//! Sets the future kickstand value
	proto native void SetKickstand(float value);
	
	//! Returns engine's min operating rpm
	proto native float EngineGetRPMMin();
	
	//! Returns engine's idle rpm before engine stalls.
	proto native float EngineGetRPMIdle();
	
	//! Returns engine's max rpm before engine blows up.
	proto native float EngineGetRPMMax();

	//! Returns engine's maximal working rpm without damaging the engine.
	proto native float EngineGetRPMRedline();

	//! Returns engine's rpm value.
	proto native float EngineGetRPM();

	//! Returns true when engine is running, false otherwise.
	proto native bool EngineIsOn();

	//! Starts the engine.
	proto native void EngineStart();

	//! Stops the engine.
	proto native void EngineStop();

	//! Get actual position of engine (model space)
	proto native vector GetEnginePos();
	
	//! Override the position of engine (model space)
	proto native void SetEnginePos(vector pos);

	//! Returns the index of the current gear, -1 if there is no engine.
	proto native int GetCurrentGear();

	//! Returns the index of the future gear, -1 if there is no engine.
	proto native int GetGear();

	//! Returns the index of the neutral gear.
	proto native int GetNeutralGear();

	//! Returns the number of gears.
	proto native int GetGearCount();

	//! Shifts the future gear up, triggering gearbox simulation.
	proto native void ShiftUp();

	//! Shifts the future gear to selected gear, triggering gearbox simulation.
	proto native void ShiftTo(int gear);

	//! Shifts the future gear down, triggering gearbox simulation.
	proto native void ShiftDown();

	//! Returns gearbox type. See CarGearboxType enum for more info.
	proto native CarGearboxType GearboxGetType();

	//! Returns gearbox mode. This is useful when motorbike has automatic gearbox.
	proto native CarAutomaticGearboxMode GearboxGetMode();

	//! Returns true if any of the wheels are locked in terms of its movement.
	proto native bool WheelIsAnyLocked();

	/*!
		Returns the raw angular velocity of the wheel, unstable value

		\param[in] wheelIdx index of the wheel, they are counted from left-front to rear-right
	*/
	proto native float WheelGetAngularVelocity( int wheelIdx );

	/*!
		Returns true if given wheel is making any contact

		\param[in] wheelIdx index of the wheel, they are counted from left-front to rear-right
	*/
	proto native bool WheelHasContact( int wheelIdx );

	/*!
		Returns the position of contact in world space, only valid if there was an actual contact

		\param[in] wheelIdx index of the wheel, they are counted from left-front to rear-right
	*/
	proto native vector WheelGetContactPosition( int wheelIdx );
	
	/*!
		Returns the position of wheel in local space

		\param[in] wheelIdx index of the wheel, they are counted from left-front to rear-right
	*/	
	proto native vector WheelGetPositionLS( int wheelIdx );

	/*!
		Returns the normal of contact in world space, only valid if there was an actual contact

		\param[in] wheelIdx index of the wheel, they are counted from left-front to rear-right
	*/
	proto native vector WheelGetContactNormal( int wheelIdx );

	/*!
		Returns the direction pointing forwards that the wheel is facing

		\param[in] wheelIdx index of the wheel, they are counted from left-front to rear-right
	*/
	proto native vector WheelGetDirection( int wheelIdx );

	/*!
		Returns the surface that the wheel is nearby

		\param[in] wheelIdx index of the wheel, they are counted from left-front to rear-right
	*/
	proto native SurfaceInfo WheelGetSurface( int wheelIdx );

	/*!
		Returns the state that the wheel is in with water

		\param[in] wheelIdx index of the wheel, they are counted from left-front to rear-right
	*/
	proto native CarWheelWaterState WheelGetWaterState( int wheelIdx );

	/*!
		Returns the entity attached that represents the wheel

		\param[in] wheelIdx index of the wheel, they are counted from left-front to rear-right
	*/
	proto native EntityAI WheelGetEntity( int wheelIdx );

	/*!
		Returns true if given wheel is locked in terms of its movement.

		\param[in] wheelIdx index of the wheel, they are counted from left-front to rear-right
	*/
	proto native bool WheelIsLocked( int wheelIdx );
	
	//! How many wheel can be attached to a motorbike (hubs only)
	proto native int WheelCount();
	
	//! Number of actually attached wheels (hubs only)
	proto native int WheelCountPresent();

	/*!
		Returns tank capacity for the specified vehicle's fluid.

		\param fluid the specified fluid type
	*/
	proto native float GetFluidCapacity(MotorbikeFluid fluid);

	/*!
		Returns fraction value (in range <0, 1>)
		of the current state of the specified vehicle's fluid.

		\param[in] fluid the specified fluid type
	*/
	proto native float GetFluidFraction(MotorbikeFluid fluid);

	//! Removes from the specified fluid the specified amount.
	proto native void Leak(MotorbikeFluid fluid, float amount);

	//! Removes all the specified fluid from vehicle.
	proto native void LeakAll(MotorbikeFluid fluid);

	//! Adds to the specified fluid the specified amount.
	proto native void Fill(MotorbikeFluid fluid, float amount);

	/*!
		Is called every time the game wants to start the engine.

		\return true if the engine can start, false otherwise.
	*/
	bool OnBeforeEngineStart()
	{
		// engine can start by default
		return true;
	}

	/*!
		Is called every time the engine starts.
	*/
	void OnEngineStart() {}

	/*!
		Is called every time the engine stops.
	*/
	void OnEngineStop() {}

	/*!
		Is called every time when the simulation changed gear.

		\param[in] newGear new gear level
		\param[in] oldGear previous gear level before gear shift
	*/
	void OnGearChanged(int newGear, int oldGear) {}

	/*!
		Is called every time when the specified vehicle's fluid level changes. 
		This callback is called on owner only.

		\param[in] fluid fluid identifier, \see MotorbikeFluid
		\param[in] newValue new fluid level
		\param[in] oldValue previous fluid level before change
	*/
	void OnFluidChanged(MotorbikeFluid fluid, float newValue, float oldValue) {}

	/*!
		Is called every sound simulation step.
		In this callback, user can modify behaviour of sound controllers.

		\param[in] ctrl sound controller identifier, \see MotorbikeSoundCtrl
		\param[in] oldValue already computed value by the game code
		\return new value of the specified sound controller.
	*/
	float OnSound(MotorbikeSoundCtrl ctrl, float oldValue)
	{
		// just use the computed value by the game code
		return oldValue;
	}

	/*!
		Called when the motorbike is upright and rideable
	*/
	void OnStabilityStart() {}

	/*!
		Called when the motorbike topples over
	*/
	void OnStabilityStop() {}
	
}
