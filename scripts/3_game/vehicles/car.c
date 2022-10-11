
//!	Car's sound controller list. (native, do not change or extend)
enum CarSoundCtrl
{
	// simulation
	ENGINE, //!< indicates if engine is ON
	RPM,    //!< engine's RPM
	SPEED,  //!< speed of the car in km/h

	// miscellaneous
	DOORS, //!< indicates if doors are open
	PLAYER //!< indicates if driver is controlled by player
};



//!	Type of vehicle's fluid. (native, do not change or extend)
enum CarFluid
{
	FUEL,
	OIL,
	BRAKE,
	COOLANT,

	USER1, //!< reserved for user / modding support
	USER2, //!< reserved for user / modding support
	USER3, //!< reserved for user / modding support
	USER4  //!< reserved for user / modding support
};



//!	Enumerated gearbox types. (native, do not change or extend)
enum CarGearboxType
{
	MANUAL,   //!< classic manual transmission with friction plates between engine and gearbox
	AUTOMATIC //!< automatic transmission with torque converter between engine and gearbox
}



//!	Enumerated vehicle's gears. (native, do not change or extend)
enum CarGear
{
	REVERSE,
	NEUTRAL,
	FIRST,
	SECOND,
	THIRD,
	FOURTH,
	FIFTH,
	SIXTH,
	SEVENTH,
	EIGTH,
	NINTH,
	TENTH,
	ELEVENTH,
	TWELFTH,
	THIRTEENTH,
	FOURTEENTH,
	FIFTEENTH,
	SIXTEENTH
};



//!	Enumerated automatic gearbox modes. (native, do not change or extend)
enum CarAutomaticGearboxMode
{
	P, //!< park
	R, //!< reverse
	N, //!< neutral
	D  //!< drive
};



//!	Base native class for all motorized wheeled vehicles.
class Car extends Transport
{
	//!	DEPRECATED, left for backwards compatibility, the methods of this class are now directly accessible on Car itself
	proto native CarController GetController();
	
	//!	Returns the current speed of the vehicle in km/h.
	proto native float GetSpeedometer();
	
	//! Returns the current speed of the vehicle in km/h. Value is absolute
	float GetSpeedometerAbsolute()
	{
		return Math.AbsFloat(GetSpeedometer());
	}

	override bool IsAreaAtDoorFree( int currentSeat, float maxAllowedObjHeight = 0.5, float horizontalExtents = 0.5, float playerHeight = 1.7 )
	{
		float speed = GetSpeedometerAbsolute();
		vector direction = GetDirection();
		vector crewPos;
		vector crewDir;
		CrewEntryWS( currentSeat, crewPos, crewDir );
		crewPos[1] = crewPos[1] + maxAllowedObjHeight + playerHeight * 0.5;
		float vExtents = horizontalExtents;

		if (speed > 8)
			vExtents *= 6;
		if (speed > 8)
			horizontalExtents = 2;

		array<Object> excluded = new array<Object>;
		array<Object> collided = new array<Object>;
		excluded.Insert(this);
		excluded.Insert(GetGame().GetPlayer());
		GetGame().IsBoxColliding(crewPos, crewDir, Vector(horizontalExtents, playerHeight, vExtents), excluded, collided); 
		foreach (Object o : collided)
		{
			EntityAI e = EntityAI.Cast(o);			
			if (IsIgnoredObject(o))
				continue;
			
			vector minmax[2];
			if (o.GetCollisionBox(minmax))
				return false;
		}

		return true;
	}
	
	override Shape DebugFreeAreaAtDoor( int currentSeat, float maxAllowedObjHeight = 0.5, float horizontalExtents = 0.5, float playerHeight = 1.7 )
	{
		float speed = GetSpeedometerAbsolute();
		vector direction = GetDirection();
		vector crewPos;
		vector crewDir;
		CrewEntryWS( currentSeat, crewPos, crewDir );
		crewPos[1] = crewPos[1] + maxAllowedObjHeight + playerHeight * 0.5;
		float vExtents = horizontalExtents;

		if (speed > 8)
			vExtents *= 6;
		if (speed > 8)
			horizontalExtents = 2;

		array<Object> excluded = new array<Object>;
		array<Object> collided = new array<Object>;
		excluded.Insert(this);
		excluded.Insert(GetGame().GetPlayer());
		GetGame().IsBoxColliding(crewPos, crewDir, Vector(horizontalExtents, playerHeight, vExtents), excluded, collided); 
		int color = ARGB(20, 0, 255, 0);
		foreach (Object o : collided)
		{
			vector minmax[2];
			if (o.GetCollisionBox(minmax))
			{
				color = ARGB(20, 255, 0, 0);
			}
		}

		return Debug.DrawCylinder(crewPos, horizontalExtents, playerHeight, color);
	}
	
	override int GetHideIconMask()
	{
		return EInventoryIconVisibility.HIDE_VICINITY;
	}

//-----------------------------------------------------------------------------
// controls

	//!	Returns the current steering value in range <-1, 1>.
	proto native float GetSteering();
	/*!
		Sets the steering value.

		\param in     should be in range <-1, 1>
		\param analog indicates if the input value was taken from analog controller
	*/
	proto native void SetSteering( float in, bool analog = false );

	//!	Returns the current thrust turbo modifier value in range <0, 1>.
	proto native float GetThrustTurbo();
	//!	Returns the current thrust gentle modifier value in range <0, 1>.
	proto native float GetThrustGentle();
	//!	Returns the current thrust value in range <0, 1>.
	proto native float GetThrust();
	/*!
		Sets the thrust value.

		\param in     should be in range <0, 1>
		\param gentle should be in range <0, 1>, thrust modifier
		\param turbo  should be in range <0, 1>, thrust modifier
	*/
	proto native void SetThrust( float in, float gentle = 0, float turbo = 0 );

	//! Returns the current brake value in range <0, 1>.
	proto native float GetBrake();
	/*!
		Sets the brake value.

		\param in should be in range <0, 1>
		\param panic should be in range <0, 1>
	*/
	proto native void SetBrake( float in, float panic = 0 );
	
	//! Returns the current handbrake value in range <0, 1>.
	proto native float GetHandbrake();
	/*!
		Sets the handbrake value.

		\param in should be in range <0, 1>
	*/
	proto native void SetHandbrake( float in );

	//!	Returns index of the current gear.
	proto native int GetGear();

	proto native void ShiftUp();
	proto native void ShiftTo( CarGear gear );
	proto native void ShiftDown();

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// fluids

	/*!
		Returns tank capacity for the specified vehicle's fluid.

		\param fluid the specified fluid type
	*/
	proto native float GetFluidCapacity( CarFluid fluid );

	/*!
		Returns fraction value (in range <0, 1>)
		of the current state of the specified vehicle's fluid.

		\param[in] fluid the specified fluid type
	*/
	proto native float GetFluidFraction( CarFluid fluid );

	//! Removes from the specified fluid the specified amount.
	proto native void Leak( CarFluid fluid, float amount );

	//! Removes all the specified fluid from vehicle.
	proto native void LeakAll( CarFluid fluid );

	//! Adds to the specified fluid the specified amount.
	proto native void Fill( CarFluid fluid, float amount );

	/*!
		Is called every time when the specified vehicle's fluid level
		changes eg. when car is consuming fuel.

		\param[in] fluid fluid identifier, \see CarFluid
		\param[in] newValue new fluid level
		\param[in] oldValue previous fluid level before change
	*/
	void OnFluidChanged( CarFluid fluid, float newValue, float oldValue ) {}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// engine

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

	/*!
		Is called every time the game wants to start the engine.

		\return true if the engine can start, false otherwise.
	*/
	bool OnBeforeEngineStart()
	{
		// engine can start by default
		return true;
	}

	//! Is called every time the engine starts.
	void OnEngineStart() {}

	//! Stops the engine.
	proto native void EngineStop();

	//! Is called every time the engine stops.
	void OnEngineStop() {}

	//! Get actual position of engine (model space)
	proto native vector GetEnginePos();
	//! Override the position of engine (model space)
	proto native void SetEnginePos(vector pos);

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// gearbox

	//! Returns total number of gears.
	proto native int GetGearsCount();

	//! Returns gearbox type. See CarGearboxType enum for more info.
	proto native CarGearboxType GearboxGetType();

	//! Returns gearbox mode. This is useful when car has automatic gearbox.
	proto native CarAutomaticGearboxMode GearboxGetMode();

	/*!
		Is called every time when the simulation changed gear.

		\param[in] newGear new gear level
		\param[in] oldGear previous gear level before gear shift
	*/
	void OnGearChanged( int newGear, int oldGear )
	{
	}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// wheels

	//! Returns true if any of the wheels are locked in terms of its movement.
	proto native bool WheelIsAnyLocked();
	/*!
		Returns true if given wheel is locked in terms of its movement.

		\param[in] wheelIdx index of the wheel, they are counted from left-front to rear-right
	*/
	proto native bool WheelIsLocked( int wheelIdx );

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// events

	/*!
		Is called every time when vehicle collides with other object.

		\param[in] zoneName configured vehicle's zone that was hit
		\param[in] localPos position where the vehicle was hit in vehicle's space
		\param[in] other object with which the vehicle is colliding
		\param[in] data contact properties
	*/
	void OnContact( string zoneName, vector localPos, IEntity other, Contact data ) {}

	/*!
		Is called every sound simulation step.
		In this callback, user can modify behaviour of sound controllers.

		\param[in] ctrl sound controller identifier, \see CarSoundCtrl
		\param[in] oldValue already computed value by the game code
		\return new value of the specified sound controller.
	*/
	float OnSound( CarSoundCtrl ctrl, float oldValue )
	{
		// just use the computed value by the game code
		return oldValue;
	}

	/*!
		Is called after every input simulation step.

		Note that the player character and other systems can always change the internal state.
		It is highly recommended to store state of custom inputs elsewhere and call Setters here.

		\param[in] dt frame time in seconds
	*/
	void OnInput( float dt ) {}

	/*!
		Is called every game frame.
		\param[in] dt frame time in seconds
	*/
	void OnUpdate( float dt ) {}
//-----------------------------------------------------------------------------


	// implemented only in internal configuration
	proto native void ForcePosition( vector pos );
	// implemented only in internal configuration
	proto native void ForceDirection( vector dir );
};



//! DEPRECATED class left for backwards compatibility, methods are available on car itself now
class CarController
{
	private void CarController() {}
	private void ~CarController() {}
	
	//!	Returns the current steering value in range <-1, 1>.
	proto float GetSteering();
	/*!
		Sets the steering value.

		\param in     should be in range <-1, 1>
		\param analog indicates if the input value was taken from analog controller
	*/
	proto void SetSteering( float in, bool analog = false );

	//!	Returns the current thrust turbo modifier value in range <0, 1>.
	proto float GetThrustTurbo();
	//!	Returns the current thrust gentle modifier value in range <0, 1>.
	proto float GetThrustGentle();
	//!	Returns the current thrust value in range <0, 1>.
	proto float GetThrust();
	/*!
		Sets the thrust value.

		\param in     should be in range <0, 1>
		\param gentle should be in range <0, 1>, thrust modifier
		\param turbo  should be in range <0, 1>, thrust modifier
	*/
	proto void SetThrust( float in, float gentle = 0, float turbo = 0 );

	//! Returns the current brake value in range <0, 1>.
	proto float GetBrake();
	/*!
		Sets the brake value.

		\param in should be in range <0, 1>
		\param panic should be in range <0, 1>
	*/
	proto void SetBrake( float in, float panic = 0 );

	//!	Returns index of the current gear.
	proto int GetGear();

	proto void ShiftUp();
	proto void ShiftTo( CarGear gear );
	proto void ShiftDown();
};