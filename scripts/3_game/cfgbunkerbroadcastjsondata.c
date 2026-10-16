class BunkerBroadcastJsonDataBase 
{
	bool IsValid()
	{
		return true;
	}
	
	protected void OutOfRangeException(string name, float value, float rangeMin, float rangeMax)
	{
		ErrorEx(string.Format("%1=%2 is out of range (<%3; %4>)", name, value, rangeMin, rangeMax));
	}
	
	protected void OutOfRangeException(string name, string value, float rangeMin, float rangeMax)
	{
		ErrorEx(string.Format("%1=%2 is out of range (<%3; %4>)", name, value, rangeMin, rangeMax));
	}
}

class BunkerBroadcastConfigInstance: BunkerBroadcastJsonDataBase
{
	ref array<ref BunkerBroadcastProxySpawn> proxies		= new array<ref BunkerBroadcastProxySpawn>();
	ref BunkerBroadcastSettings bunkerSettings 				= new BunkerBroadcastSettings();
	ref BunkerBroadcastSchedulerSettings schedulerSettings	= new BunkerBroadcastSchedulerSettings();
	ref BunkerBroadcastPlaybackSettings playbackSettings	= new BunkerBroadcastPlaybackSettings();
	
	override bool IsValid()
	{
		if (!super.IsValid())
			return false;

		bool hasErrors;
		foreach (BunkerBroadcastProxySpawn proxyData : proxies)
		{
			if (!proxyData.IsValid())
			{
				hasErrors = true;
				break;
			}
		}

		if (!bunkerSettings.IsValid())
			hasErrors = true;
		
		if (!schedulerSettings.IsValid())
			hasErrors = true;
		
		if (!playbackSettings.IsValid())
			hasErrors = true;
		
		return !hasErrors;
	}
}

class BunkerBroadcastProxySpawn : BunkerBroadcastJsonDataBase
{
	float position[3];
}

class BunkerBroadcastSettings : BunkerBroadcastJsonDataBase
{
	private const int MIN_ACTIVE = 1;
	private const int MAX_ACTIVE = 30;
	
	private const int MIN_LOCKCODE_LENGTH = 3;
	private const int MAX_LOCKCODE_LENGTH = 6;
	
	//! default values
	int numberOfActive 	= 2;
	int lockCodeLength	= 4;
	int lockCodeMode	= 0;

	override bool IsValid()
	{
		if (!super.IsValid())
			return false;

		bool hasErrors;
		if (numberOfActive < MIN_ACTIVE || numberOfActive > MAX_ACTIVE)
		{
			OutOfRangeException("numberOfActive", numberOfActive, MIN_ACTIVE, MAX_ACTIVE);
			hasErrors = true;
		}
		
		if (lockCodeLength < MIN_LOCKCODE_LENGTH || lockCodeLength > MAX_LOCKCODE_LENGTH)
		{
			OutOfRangeException("lockCodeLength", lockCodeLength, MIN_LOCKCODE_LENGTH, MAX_LOCKCODE_LENGTH);
			hasErrors = true;
		}
		
		if (lockCodeMode < 0 || lockCodeMode > 1)
		{
			OutOfRangeException("lockCodeMode", lockCodeMode, 0, 1);
			hasErrors = true;
		}
		
		return !hasErrors;
	}
}

class BunkerBroadcastSchedulerSettings : BunkerBroadcastJsonDataBase
{
	private const int BATCH_LIFETIME_MIN = 1; 	// [minute]
	private const int BATCH_LIFETIME_MAX = 1000; // [minute]
	
	private const int REPORT_INTERVAL_MIN = 1; 		// [minute]
	private const int REPORT_INTERVAL_MAX = 120; 	// [minute]

	//! default values
	int batchLifetimeLength	= 1;
	int reportInterval		= 1;
	
	ref array<ref BunkerBroadcastPeriod> periods = new array<ref BunkerBroadcastPeriod>();
	
	override bool IsValid()
	{
		if (!super.IsValid())
			return false;

		bool hasErrors;
		if (batchLifetimeLength < BATCH_LIFETIME_MIN || batchLifetimeLength > BATCH_LIFETIME_MAX)
		{
			OutOfRangeException("batchLifetimeLength", batchLifetimeLength, BATCH_LIFETIME_MIN, BATCH_LIFETIME_MAX);
			hasErrors = true;
		}
		
		if (reportInterval < REPORT_INTERVAL_MIN || reportInterval > REPORT_INTERVAL_MAX)
		{
			OutOfRangeException("reportInterval", reportInterval, REPORT_INTERVAL_MIN, REPORT_INTERVAL_MAX);
			hasErrors = true;
		}
		
		foreach (BunkerBroadcastPeriod period : periods)
		{
			if (!period.IsValid())
				hasErrors = true;
		}
		
		return !hasErrors;
	}
}

class BunkerBroadcastPeriod : BunkerBroadcastJsonDataBase
{
	private const int TIME_RANGE_MIN = 0;
	private const int TIME_RANGE_MAX = 2359;

	//! default values
	string startTime = "0000";
	string endTime = "2359";
	
	override bool IsValid()
	{
		if (!super.IsValid())
			return false;

		bool hasErrors;
		if (startTime.Length() != 4)
		{
			ErrorEx(string.Format("startTime=%1 is not 4 character long string (formated as '0700', '1310', etc.)", startTime, TIME_RANGE_MIN, TIME_RANGE_MAX));
			hasErrors = true;
		}

		if (endTime.Length() != 4)
		{
			ErrorEx(string.Format("endTime=%1 is not 4 character long string (formated as '0700', '1310', etc.)", endTime, TIME_RANGE_MIN, TIME_RANGE_MAX));
			hasErrors = true;
		}
		
		if (startTime.ToInt() < 0 || startTime.ToInt() > 2359)
		{
			OutOfRangeException("startTime", startTime, TIME_RANGE_MIN, TIME_RANGE_MAX);
			hasErrors = true;
		}

		if (endTime.ToInt() < 0 || endTime.ToInt() > 2359)
		{
			OutOfRangeException("endTime", endTime, TIME_RANGE_MIN, TIME_RANGE_MAX);
			hasErrors = true;
		}
		
		return !hasErrors;
	}
	
	int GetStartHour()
	{
		return startTime.ToInt() / 100;
	}
	
	int GetEndHour()
	{
		return endTime.ToInt() / 100;
	}

	int GetStartMinute()
	{
		return startTime.ToInt() % 100;
	}
	
	int GetEndMinute()
	{
		return endTime.ToInt() % 100;
	}
	
	override protected void OutOfRangeException(string name, string value, float rangeMin, float rangeMax)
	{
		ErrorEx(string.Format("%1=%2 is out of range (<%2; %3>, formated as '0700', '1310', etc.)", name, value, rangeMin, rangeMax));
	}
}

class BunkerBroadcastPlaybackSettings : BunkerBroadcastJsonDataBase
{
	private const float DURATION_MIN = 0.1;
	private const float DURATION_MAX = 5.0;

	private const float PAUSE_MIN = 0.5;
	private const float PAUSE_MAX = 10.0;

	private const int BUNKER_REPETITIONS_MIN = 0;
	private const int BUNKER_REPETITIONS_MAX = 3;
	
	private const int MESSAGE_REPETITIONS_MIN = 0;
	private const int MESSAGE_REPETITIONS_MAX = 5;

	//! default values
	float dotDuration			= 0.3;
	float dashDuration			= 0.4;
	float symbolGapDuration 	= 0.1;
	float wordGapDuration		= 0.3;
	float pauseBetweenBunkers	= 2.0;
	int bunkerRepetitions 		= 0;
	int messageRepetitions 		= 0;

	override bool IsValid()
	{
		if (!super.IsValid())
			return false;

		bool hasErrors;
		if (dotDuration < DURATION_MIN || dotDuration > DURATION_MAX)
		{
			OutOfRangeException("dotDuration", dotDuration, DURATION_MIN, DURATION_MAX);
			hasErrors = true;
		}
		
		if (dashDuration < DURATION_MIN || dashDuration > DURATION_MAX)
		{
			OutOfRangeException("dashDuration", dashDuration, DURATION_MIN, DURATION_MAX);
			hasErrors = true;
		}
		
		if (symbolGapDuration < DURATION_MIN || symbolGapDuration > DURATION_MAX)
		{
			OutOfRangeException("symbolGapDuration", symbolGapDuration, DURATION_MIN, DURATION_MAX);
			hasErrors = true;
		}

		if (wordGapDuration < DURATION_MIN || wordGapDuration > DURATION_MAX)
		{
			OutOfRangeException("wordGapDuration", wordGapDuration, DURATION_MIN, DURATION_MAX);
			hasErrors = true;
		}

		if (pauseBetweenBunkers < PAUSE_MIN || pauseBetweenBunkers > PAUSE_MAX)
		{
			OutOfRangeException("pauseBetweenBunkers", pauseBetweenBunkers, PAUSE_MIN, PAUSE_MAX);
			hasErrors = true;
		}
		
		if (bunkerRepetitions < BUNKER_REPETITIONS_MIN || bunkerRepetitions > BUNKER_REPETITIONS_MAX)
		{
			OutOfRangeException("bunkerRepetitions", bunkerRepetitions, BUNKER_REPETITIONS_MIN, BUNKER_REPETITIONS_MAX);
			hasErrors = true;
		}
		
		if (messageRepetitions < MESSAGE_REPETITIONS_MIN || messageRepetitions > MESSAGE_REPETITIONS_MAX)
		{
			OutOfRangeException("messageRepetitions", messageRepetitions, MESSAGE_REPETITIONS_MIN, MESSAGE_REPETITIONS_MAX);
			hasErrors = true;
		}
		
		return !hasErrors;
	}
}
