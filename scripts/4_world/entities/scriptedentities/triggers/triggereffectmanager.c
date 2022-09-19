/*
DISCLAIMER: may undergo some changes in the course of 1.14 experimental stage.
*/

// Used for Effect based triggers ( such as Contaminated areas )
// Allows us to unify these triggers and treat Enter / Exit events as if all triggers were one
class TriggerEffectManager
{
	static ref TriggerEffectManager m_This;
	
	// Player map will only be handled client side as it is most relevant
	ref map<string, ref map<PlayerBase, int>> m_PlayerInsiderMap; // Used to keep track of which players are in which effects

	void TriggerEffectManager()
	{
		m_PlayerInsiderMap = new map<string, ref map<PlayerBase, int>>;
	}
	
	static void DestroyInstance()
	{
		m_This = null;
	}
	
	// This is a SINGLETON so, if we can't get the instance, we create it
	static TriggerEffectManager GetInstance()
	{
		if ( !m_This )
			m_This = new TriggerEffectManager();

		return m_This;
	}
	
	void RegisterTriggerType( EffectTrigger effectTrigger )
	{
		if ( !m_PlayerInsiderMap.Contains( effectTrigger.GetType() ) )
		{
			map<PlayerBase, int> playerMap = new map<PlayerBase, int>;
			m_PlayerInsiderMap.Insert( effectTrigger.GetType(), playerMap );
			
			//Debug.Log( "We have : " + m_PlayerInsiderMap.Count() + " triggers");
		}
	}

	// When a player enters a trigger using this class, we add player to map or increase its trigger count
	void OnPlayerEnter( notnull PlayerBase player, notnull EffectTrigger trigger )
	{
		string trigger_type = trigger.GetType();
		
		map<PlayerBase, int> playerMap;
		if ( m_PlayerInsiderMap.Find( trigger_type, playerMap) )
		{
			if ( playerMap )
			{ 
				if ( playerMap.Contains( player ) )
				{
					int newTriggerCount = playerMap.Get( player ) + 1;
					playerMap.Set( player, newTriggerCount );
					//Debug.Log("We increase the amount of triggers a player is in");
				}
				else
				{
					// First time this player enters this type of trigger, we set the trigger count to 1
					playerMap.Insert( player, 1 );
					//Debug.Log("We insert a player");
				}
				
				//m_PlayerInsiderMap.Set( trigger_type, playerMap );
			}
			else
			{
				// We didn't have the map yet, we create it and register this player in it
				playerMap = new map<PlayerBase, int>;
				playerMap.Insert( player, 1 );
				m_PlayerInsiderMap.Insert( trigger_type, playerMap );
			}
		}
	}
	
	// Same as OnPlayerEnter, but we decrease trigger count on each trigger leave and remove player from map when count is 0
	void OnPlayerExit( notnull PlayerBase player, notnull EffectTrigger trigger )
	{
		string trigger_type = trigger.GetType();
		
		map<PlayerBase, int> playerMap;
		if ( m_PlayerInsiderMap.Find( trigger_type, playerMap ) && playerMap )
		{
			if ( playerMap.Contains( player ))
			{
				if( playerMap.Get( player ) > 1 )
				{
					//Debug.Log("We reduced the amount of triggers the player is in");
					playerMap.Set( player, playerMap.Get( player ) - 1 );
				}
				else
				{
					// The player left the last trigger of this type they were in
					playerMap.Remove( player );
					//Debug.Log("We removed this player from this trigger type");
				}
			}
			//m_PlayerInsiderMap.Set( trigger_type, playerMap );

		}
	}
	
	bool IsPlayerInTriggerType( notnull PlayerBase player, notnull EffectTrigger trigger )
	{
		map<PlayerBase, int> playerMap;
		if ( m_PlayerInsiderMap.Find( trigger.GetType(), playerMap ) )
		{
			return playerMap != null && playerMap.Contains( player );
		}
		return false;
	}
}