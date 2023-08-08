/**
 \brief ScriptCallQueue Class provide "lazy" calls - when we don't want to execute function immediately but later during frame update (used mainly in UI)
 \n usage:
 @code
	class Arkanoid extends Game
	{
		ref ScriptCallQueue m_CallQueue = new ScriptCallQueue();
	
		ScriptCallQueue GetCallQueue() { 
			return m_CallQueue;
		}
	
		override void OnUpdate(float timeslice)
		{
			m_CallQueue.Tick(timeslice);
			...
		}
		...
	}

	class MyObject
	{
		int m_cnt = 0;
	
		void Hello(int p1, string p2)
		{
			Print("Hello( " + p1 + " , " + p2 + ")");
		}
	
		void Test()
		{
			Print(m_cnt);
			m_cnt++;
			
			if (m_cnt > 10)
			{
				ScriptCallQueue queue = GetGame().GetCallQueue();
				queue.Remove(Test);
			}
		}
	}

	void Test(MyObject obj)
	{
		ScriptCallQueue queue = GetGame().GetCallQueue(); 
		queue.CallLater(obj.Hello, 5000, false, 65, "world"); // adds call 'obj.Hello(65, "world")' into queue, and it will be executed once after 5s
		queue.CallLater(obj.Test, 3000, true); // adds call 'obj.Test()' into queue, and it will be executed each 3s
		queue.Call(obj.Hello, 72, "world 2"); // adds call 'obj.Hello(72, "world 2")' into queue, and it will be executed next frame (on next call of ScriptCallQueue.Tick)
	}
 @endcode
 */
class ScriptCallQueue
{
	//! executes calls on queue if their time is already elapsed, if 'repeat = false' call is removed from queue
	proto native void Tick(float timeslice);
	//! adds call into the queue with given parameters and arguments (arguments are held in memory until the call is executed/removed or ScriptCallQueue is destroyed)
	proto void Call(func fn, void param1 = NULL, void param2 = NULL, void param3 = NULL, void param4 = NULL, void param5 = NULL, void param6 = NULL, void param7 = NULL, void param8 = NULL, void param9 = NULL);
	//! adds call into the queue with given parameters and arguments (arguments are held in memory until the call is executed/removed or ScriptCallQueue is destroyed)
	proto void CallByName(Class obj, string fnName , Param params = NULL);
	//! adds call into the queue with given parameters and arguments (arguments are held in memory until the call is executed/removed or ScriptCallQueue is destroyed)
	proto void CallLater(func fn, int delay = 0, bool repeat = false, void param1 = NULL, void param2 = NULL, void param3 = NULL, void param4 = NULL, void param5 = NULL, void param6 = NULL, void param7 = NULL, void param8 = NULL, void param9 = NULL);	
	//! adds call into the queue with given parameters and arguments (arguments are held in memory until the call is executed/removed or ScriptCallQueue is destroyed)
	proto void CallLaterByName(Class obj, string fnName, int delay = 0, bool repeat = false, Param params = NULL);	
	//! remove specific call from queue	
	proto void Remove(func fn);
	//! return Remaining time to the call execution (in miliseconds)
	proto int GetRemainingTime(func fn);
	//! remove specific call from queue	
	proto void RemoveByName(Class obj, string fnName);
	//! return Remaining time to the call execution (in miliseconds)
	proto int GetRemainingTimeByName(Class obj, string fnName);
	//! remove all calls from queue
	proto native void Clear();
};

/**
 \brief ScriptInvoker Class provide list of callbacks
 \n usage:
 @code
	class Player
	{
		ref ScriptInvoker m_DeathInvoker	= new ScriptInvoker();
	
		void OnKilled()
		{
			m_DeathInvoker.Invoke(this);		
		}
	}

	void LogPlayerDeath(p)
	{
		Print("RIP " + p);
	}		

	class Game
	{
		void RemovePlayer(Player p)
		{
		}

		void GameOver()
		{
		}
	}

	void OnPlayerSpaned(Player p)
	{
		Game game = GetGame();
		p.m_DeathInvoker.Insert(LogPlayerDeath);
		p.m_DeathInvoker.Insert(game.RemovePlayer);
		p.m_DeathInvoker.Insert(game.GameOver);
	}
 @endcode
 */
class ScriptInvoker
{
	//! invoke call on all inserted methods with given arguments
	proto void Invoke(void param1 = NULL, void param2 = NULL, void param3 = NULL, void param4 = NULL, void param5 = NULL, void param6 = NULL, void param7 = NULL, void param8 = NULL, void param9 = NULL);
	//! insert method to list
	proto bool Insert(func fn, int flags = EScriptInvokerInsertFlags.IMMEDIATE);	
	//! remove specific call from list
	proto bool Remove(func fn, int flags = EScriptInvokerRemoveFlags.ALL);
	//! count how many times this fn is actively present in the Invoker
	proto int Count(func fn);
	//! remove all calls from list
	proto native void Clear();
};

enum EScriptInvokerInsertFlags
{
	NONE,
	/**
	\brief It gets added in immediately, which means that when called while an invoker is running, it will call this newly added call in the same run
		\note Default flag, as that is the original behaviour, although it might cause endless Insert chain now... (still better than undefined behaviour)
		\note In case of "Possible endless Insert detected" VME, either create an exit, remove the IMMEDIATE flag or make the insert UNIQUE
		\note The Endless Insert is detected by seeing if "amount of calls > initial size + 128"
	*/
	IMMEDIATE,
	/**
	\brief Only one call to this instance+method is ever expected
		\note Will throw a VME when a second one is attempted to be added
		\note If it was already added without the flag, it will also throw a VME and keep the first of all previously inserted
	*/
	UNIQUE,
}

enum EScriptInvokerRemoveFlags
{
	NONE,
	/**
	\brief Default flag
		\note Don't use this if you want it to remove only the last insert instead of all of them
	*/
	ALL,
}

/**
 \brief Designed to hold 1 valid call
*/
class ScriptCaller
{
	//! ScriptCaller is meant to be created through Create
	private void ScriptCaller();
	//! Creates a ScriptCaller
	static proto ScriptCaller Create(func fn);
	//! Replaces the current registered func with the new one, throws errors if unsuccessful
	proto void Init(func fn);	
	//! Invoke call on the registered func, throws errors if unsuccessful
	proto void Invoke(void param1 = null, void param2 = null, void param3 = null, void param4 = null, void param5 = null, void param6 = null, void param7 = null, void param8 = null, void param9 = null);

	//! Checks if the ScriptCaller is valid
	proto bool IsValid();

	/**
	\brief Compares this script caller against another script caller
		\note May return true even if either one is invalid

	@code
		class SomeClass
		{
			void SomeMethod()
			{
			}
		}

		void Test()
		{
			SomeClass instanceA = new SomeClass();
			SomeClass instanceB = new SomeClass();

			ScriptCaller callerA;
			ScriptCaller callerB;

			//! Two methods that are to the same instance
			callerA = ScriptCaller.Create(instanceA.SomeMethod);
			callerB = ScriptCaller.Create(instanceA.SomeMethod);

			Print(callerA.Equals(callerB)); //! "1"
			Print(callerA == callerB); //! "0"
			Print(callerA); //! "ScriptCaller callerA = ScriptCaller<87bc2d40>"
			Print(callerB); //! "ScriptCaller callerB = ScriptCaller<87bc3600>"

			//! Two methods belonging to different instances
			callerA = ScriptCaller.Create(instanceA.SomeMethod);
			callerB = ScriptCaller.Create(instanceB.SomeMethod);

			Print(callerA.Equals(callerB)); //! "0"
			Print(callerA == callerB); //! "0"
			Print(callerA); //! "ScriptCaller callerA = ScriptCaller<87bc3c40>"
			Print(callerB); //! "ScriptCaller callerB = ScriptCaller<87bc2d40>"
		}
	@endcode
	*/
	proto bool Equals(notnull ScriptCaller other);

};