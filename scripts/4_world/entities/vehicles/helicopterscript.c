/*!
	Base script class for helicopters.
*/
class HelicopterScript extends HelicopterAuto
{
	void HelicopterScript()
	{
		SetEventMask(EntityEvent.POSTSIMULATE);
	}

	override void EOnPostSimulate(IEntity other, float timeSlice)
	{
	}

	/*!
		Gets called everytime the game wants to start the engine.
		This callback is called on server only.

		\return true if the engine can start, false otherwise.
	*/
	bool OnBeforeEngineStart()
	{
		return true;
	}

	//! Gets called everytime the engine starts.
	void OnEngineStart()
	{
	}

	//! Gets called everytime the engine stops.
	void OnEngineStop()
	{
	}
};
