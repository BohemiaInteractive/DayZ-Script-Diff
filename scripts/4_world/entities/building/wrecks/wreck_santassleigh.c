//Christmas Event: Santa's Sleigh
class Wreck_SantasSleigh extends CrashBase
{
	XmasSleighLight 		m_SleighLight;
	
	int m_MaxDeersAmount = 4;
	int m_MinDeersAmount = 2;
	int m_MaxDeersSpawnRange = 25;
	int m_MinDeersSpawnRange = 5;

	void Wreck_SantasSleigh()
	{
		if ( !GetGame().IsDedicatedServer() )
		{
			//particles - Aurora trail
			m_ParticleEfx = ParticleManager.GetInstance().PlayOnObject(ParticleList.AURORA_SANTA_WRECK, this, Vector(0, 0, 0));
			
			//lights - green light
			m_SleighLight = XmasSleighLight.Cast( ScriptedLightBase.CreateLight( XmasSleighLight, Vector(0, 0, 0) ) );
			m_SleighLight.AttachOnMemoryPoint( this, "light" );
		}
	}
	
	// needs to have the soundset registered in CrashBase.Init()
	override string GetSoundSet()
	{
		return "SledgeCrash_Distant_SoundSet";
	}
	
	override void EEOnCECreate()
	{
		super.EEOnCECreate();
		SpawnRandomDeerLater();
	}
	
	override void EEDelete(EntityAI parent)
	{
		super.EEDelete(parent);
		
		if ( !GetGame().IsDedicatedServer() )
		{
			if ( m_SleighLight )
				m_SleighLight.Destroy();
		}	
	}
	
	void SpawnRandomDeerLater()
	{
		//SpawnRandomDeers();
		GetGame().GetCallQueue( CALL_CATEGORY_GAMEPLAY ).CallLater( SpawnRandomDeers, 0);
	}
	
	//Spawn a random amount of (dead) deers around the sleigh
	void SpawnRandomDeers()
	{
		EntityAI deer;
		vector crash_pos = GetPosition();
		
		int deersAmount = Math.RandomIntInclusive(m_MinDeersAmount,m_MaxDeersAmount);
		
		for (int i = 0; i < m_MaxDeersAmount; i++)
		{
			vector deer_pos = RandomizePosition(crash_pos);
			deer = EntityAI.Cast(GetGame().CreateObject("Animal_CervusElaphus", deer_pos,false, true));
			deer.SetHealth01("","", 0);
			vector orientation = deer.GetOrientation();
			deer.SetOrientation(Vector(Math.RandomIntInclusive(0,360),orientation[1],orientation[2]));
		}		
	}
	
	//Return a new vector scattered around origin.
	vector RandomizePosition(vector origin)
	{		
		int randX;
		int randZ;
		
		randX = Math.RandomIntInclusive(m_MinDeersSpawnRange, m_MaxDeersSpawnRange);
		if (Math.RandomIntInclusive(0,1) < 1)
			randX = -randX;
			
		randZ = Math.RandomIntInclusive(m_MinDeersSpawnRange, m_MaxDeersSpawnRange);
		if (Math.RandomIntInclusive(0,1) < 1)
			randZ = -randZ; 
			
		origin[0] = origin[0] + randX;
		origin[2] = origin[2] + randZ;
		
		return origin;
	
	}
}