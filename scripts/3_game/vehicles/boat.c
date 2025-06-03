//!	Boat's sound controller list. (native, do not change or extend)
enum BoatSoundCtrl
{
	// simulation
	ENGINE, //!< indicates if engine is ON
	SPEED,  //!< speed of the boat in km/h

	// miscellaneous
	PLAYER //!< indicates if driver is controlled by player
};

//!	Type of vehicle's fluid. (native, do not change or extend)
enum BoatFluid
{
	FUEL
};

class BoatOwnerState : TransportOwnerState
{
};

class BoatMove : TransportMove
{
};

//!	Native class for boats - handles physics simulation
class Boat extends Transport
{
	//!
	protected override event typename GetOwnerStateType()
	{
		return BoatOwnerState;
	}

	//!
	protected override event typename GetMoveType()
	{
		return BoatMove;
	}

	//!	Returns the actual steering value in range <-1, 1>.
	proto native float GetSteering();

	//! Sets the future steering value.
	proto native void SetSteering(float value);

	//!	Returns the actual throttle value in range <0, 1>.
	proto native float GetThrottle();
	
	//! Sets the future throttle value.
	proto native void SetThrottle(float value);

	//! Returns the value of how much the clutch is disengaged.
	proto native int GetClutch();

	//! Sets the future clutch value.
	proto native void SetClutch(float value);

	//! Returns if there is an engine.
	proto native bool HasEngine();

	//! Returns engine's min operating rpm.
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

	//! Returns the propeller position in local space
	proto native vector PropellerGetPosition();

	//! Returns the angular velocity of the propeller
	proto native float PropellerGetAngularVelocity();
	
	/*!
		Returns tank capacity for the specified vehicle's fluid.

		\param fluid the specified fluid type
	*/
	proto native float GetFluidCapacity(BoatFluid fluid);

	/*!
		Returns fraction value (in range <0, 1>)
		of the current state of the specified vehicle's fluid.

		\param[in] fluid the specified fluid type
	*/
	proto native float GetFluidFraction(BoatFluid fluid);

	//! Removes from the specified fluid the specified amount.
	proto native void Leak(BoatFluid fluid, float amount);

	//! Removes all the specified fluid from vehicle.
	proto native void LeakAll(BoatFluid fluid);

	//! Adds to the specified fluid the specified amount.
	proto native void Fill(BoatFluid fluid, float amount);

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

		\param[in] fluid fluid identifier, \see BoatFluid
		\param[in] newValue new fluid level
		\param[in] oldValue previous fluid level before change
	*/
	void OnFluidChanged(BoatFluid fluid, float newValue, float oldValue) {}

	/*!
		Is called every sound simulation step.
		In this callback, user can modify behaviour of sound controllers.

		\param[in] ctrl sound controller identifier, \see BoatSoundCtrl
		\param[in] oldValue already computed value by the game code
		\return new value of the specified sound controller.
	*/
	float OnSound(BoatSoundCtrl ctrl, float oldValue)
	{
		// just use the computed value by the game code
		return oldValue;
	}
	
};
