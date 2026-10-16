/*
	This class serves as the interface to the Sandstorm weather phenomenom.
	Control of the phenomenon is driven by the server and replicated to all clients.
	The controller instance can be obtained via `Weather::GetSandstorm`.
*/
class SandstormController
{
	/*!
		\brief Creation of `SandstormController` is disallowed in scripts.
		       Retrieve current instance using `Weather::GetSandstorm()` instead.
	*/
	private void SandstormController()  { /*Disallowed.*/ }
	private void ~SandstormController() { /*Disallowed.*/ }
	/*
		\brief Starts the sandstorm, gathering its full magnitude over `duration` seconds.
		       The sandstorm must be inactive prior to this call and this call is Server-only!
		\param duration The duration in seconds over which the sandstorm gradually fades
		                in towards its target intensity.
	*/
	proto native bool Start(float duration, bool calledByWeather);
	/*!
		\brief Stops the sandstorm, leaving it to fade out over `duration` seconds.
		       The sandstorm must be active prior to this call and this call is Server-only.
		\param duration The duration in seconds over which the sandstorm gradually fades
		                out towards 0 intensity.
	*/
	proto native void Stop(float duration, bool calledByWeather);
	/*!
		\brief Returns whether sandstorm is currently active.
		\return True for as long as there is sandstorm active somewhere.
	*/
	proto native bool IsActive();
	/*!
		\brief Returns the direction from the start position to the end position of the sandstorm.
		\return Directional vector of the sandstorm facing.
	*/
	proto native vector GetDirection();
	/*!
		\brief Returns whether the sandstorm has advanced and reached the end (final) position.
		       The sandstorm must be active prior to this call!
	*/
	proto native bool IsPositionAtEnd();
	/*!
		\brief Returns the current position of the sandstorm in world space.
		       The sandstorm must be active prior to this call!
	*/
	proto native vector GetPosition();
	/*!
		\brief Returns the speed in m/s at which the sandstorm advances.
			   The sandstorm must be active prior to this call!
	*/
	proto native float GetSpeed();
	/*!
		\brief Returns the remaining movement duration until the sandstorm reached its end (final) position at its current speed in seconds.
			The sandstorm must be active prior to this call!
	*/
	proto native float GetRemainingMovementDuration();
	/*!
		\brief Projects the provided `worldPosition` to the edge of the sandstorm.
		       The sandstorm must be active prior to this call!
		\param worldPosition Input position to project, in world space.
		\param outPosition Projection of the input position on the sandstorm edge in world space.
		\return Returns the distance to the edge (Negative for points beyond the edge, i.e. inside the sandstorm)
	*/
	proto native float GetPointOnEdge(vector worldPosition, out vector outPosition);
	/*!
		\brief Returns the intensity of the sandstorm at a given position in world space.
			   The sandstorm must be active prior to this call!
		\param worldPosition Input position to project, in world space.
	*/
	proto native float GetIntensity(vector worldPosition);
	/*!
		\brief Returns the intensity of the sandstorm at a players head position in world space.
			   Value is influenced by external factors like interrior/underground conditions.
			   The sandstorm must be active prior to this call!
		\param Man object.
	*/
	float GetIntensityForPlayer(Man player);
	/*!
		\brief Returns the magnitude of the sandstorm - the intensity independent of its position.
		\return Returns the peak intensity of the active storm.
	*/
	proto native float GetMagnitude();
	/*!
		\brief Callback raised when the sandstorm becomes active.
		       Raised for both the server and the client.
		\note For clients joining a in progress this event will be raised even if
		      the sandstorm has already been in progress on the server prior.
	*/
	protected event void OnStart(bool continuedSession, bool calledByWeather = false)
	{
		// Forward the callback to WorldData
		if (g_Game.IsServer())
		{
			GetGame().GetMission().GetWorldData().WeatherSandstormStart(continuedSession);
		}
	}
	/*!
		\brief Callback raised every tick for as long as the sandstorm is active.
		       Raised for both the server and the client.
		\param deltaTime Time step since last update.
	*/
	protected event void OnUpdate(float deltaTime)
	{
	}
	/*!
		\brief Callback raised when the sandstorm becomes inactive.
		       Raised for both the server and the client.
	*/
	protected event void OnStop(bool calledByWeather = false)
	{
		// Forward the callback to WorldData
		if (g_Game.IsServer())
		{
			GetGame().GetMission().GetWorldData().WeatherSandstormStop(calledByWeather);
		}
	}
	/*!
		\brief This method may be implemented to override the default sandstorm controller class
		       instantiated by the Weather controller. Provided class must be inherited from the
		       SandstormController class and must exist in the World module or lower.
		\note This method is optional. It may be left empty for default native behavior.
	*/
	private static event typename GetScriptedType()
	{
		//
		// The following class is present in module 4_World and
		// hence its typename cannot be used directly in 3_Game.
		//
		string className = "ScriptedSandstormController";
		return className.ToType();
	}
};

typedef SandstormController Sandstorm;