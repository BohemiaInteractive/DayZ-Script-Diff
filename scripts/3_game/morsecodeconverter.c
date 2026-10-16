class BunkerBroadcastManager
{
	private void BunkerBroadcastManager() {};
	private void ~BunkerBroadcastManager() {};
	
	// Takes in an array of strings, translates it to Morse, broadcasts it to all players (Server only)
	proto native void BroadcastTextAsMorse(notnull array<string> inputs, notnull array<string> outputs);
	
	// Sets the message speed - the duration of dots and dashes, the pause between single characters and the pause between words.
	proto native void SetMessageSpeed(float dotDuration, float dashDuration, float symbolGap, float wordGap);
	
	// How long the pause between bunkers should be
	proto native void SetPauseBetweenBunkers(float pause);
	
	// How many times one bunker should repeat before advancing to the next bunker
	proto native void SetBroadcastBunkerRepeatCount(int count);
	
	// How many times the whole message should repeat when all bunkers have been relayed
	proto native void SetBroadcastTotalRepeatCount(int count);
};
