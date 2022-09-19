
class HandShiversSymptom extends SymptomBase
{
	int m_ShakeLevel;
	//this is just for the Symptom parameters set-up and is called even if the Symptom doesn't execute, don't put any gameplay code in here
	override void OnInit()
	{
		m_SymptomType = SymptomTypes.SECONDARY;
		m_Priority = 0;
		m_ID = SymptomIDs.SYMPTOM_HAND_SHIVER;
		m_SyncToClient = false;

	}
	
	override void SetParam(Param p)
	{
		Param1<int> p1 = Param1<int>.Cast(p);
		if ( p1 )
		{
			m_ShakeLevel = p1.param1;
			
			if ( m_ShakeLevel > PlayerBase.SHAKE_LEVEL_MAX )
			{
				m_ShakeLevel = PlayerBase.SHAKE_LEVEL_MAX;
			}
		}
	}

	
	override void OnUpdateClient(PlayerBase player, float deltatime)
	{
	}

	
	override void OnGetActivatedServer(PlayerBase player)
	{
		player.SetShakesForced(m_ShakeLevel);
	}
	
	override void OnGetDeactivatedServer(PlayerBase player)
	{
		player.SetShakesForced(0);
	}

	override void OnGetActivatedClient(PlayerBase player)
	{

		
	}

	//!only gets called once on an active Symptom that is being deactivated
	override void OnGetDeactivatedClient(PlayerBase player)
	{

	}
	
}
