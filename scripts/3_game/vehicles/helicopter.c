/*!
	Base native class for helicopter.
*/
class Helicopter extends Transport
{
};

//! Native implementatin with auto hover funcionality
class HelicopterAuto extends Helicopter
{
	//! Returns true when engine is running, false otherwise.
	proto native bool IsEngineOn();

	//! Starts the engine.
	proto native void EngineStart();

	//! Stops the engine.
	proto native void EngineStop();

	//! Returns true when auto-hover system is turned on, false otherwise.
	proto native bool IsAutohoverOn();

	//! Enables auto-hover system.
	proto native void AutohoverOn();

	//! Disables auto-hover system.
	proto native void AutohoverOff();
};
