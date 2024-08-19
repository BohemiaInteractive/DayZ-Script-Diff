

class PCOHandlerStats
{
	ref map<int, ref PlayerStatsPCO_Base> m_PCOs = new map<int, ref PlayerStatsPCO_Base>;
	int m_HighestVersion;
	
	void PCOHandlerStats()
	{
		RegisterPCO(new PlayerStatsPCO_v100);
		RegisterPCO(new PlayerStatsPCO_v101);
		RegisterPCO(new PlayerStatsPCO_current);
	}
	
	void RegisterPCO(PlayerStatsPCO_Base pco)
	{
		int version = pco.GetVersion();
		
		if( version > m_HighestVersion )
		{
			m_HighestVersion = version;
		}
		
		m_PCOs.Insert(version, pco);
	}
	
	PlayerStatsPCO_Base GetPCO(int version = -1)
	{
		//PlayerStatsPCO_Base pco;

		if(version == -1)//no version set - fetch the highest version
		{
			return m_PCOs.Get(m_HighestVersion);
		}
		else if( !m_PCOs.Contains(version))//version set - version not present, fetch the closest lower version
		{
			for(int i = version; i > 100; i--)
			{
				if(m_PCOs.Contains(i))
				{
					//Print("fetching PCO version:"+ i);
					return m_PCOs.Get(i);	
				}
			}
			return null;
		}
		else//version set - version present, fetch it
		{
			//Print("fetching PCO version:"+ version);
			return m_PCOs.Get(version);
		}
	}
}


class PlayerStatsPCO_Base
{
	void PlayerStatsPCO_Base()
	{
		Init();
	}
	
	void Init();
	
	int GetVersion()
	{
		return -1;
	}
	
	ref array<ref PlayerStatBase> m_PlayerStats = new array<ref PlayerStatBase>;
	
	PlayerStatBase GetStatObject(int id)
	{
		return m_PlayerStats.Get(id);
	}
	
	void RegisterStat(int id, PlayerStatBase stat)
	{
		m_PlayerStats.InsertAt(stat, id);
		stat.Init(id/*, this*/);
	}
	
	void OnStoreSave ( ParamsWriteContext ctx )
	{
		for ( int i = 0; i < m_PlayerStats.Count(); i++ )
		{
			m_PlayerStats.Get(i).OnStoreSave(ctx);
		}
	}
	
	bool OnStoreLoad ( ParamsReadContext ctx )
	{
		for ( int i = 0; i < m_PlayerStats.Count(); i++ )
		{
			if(!m_PlayerStats.Get(i).OnStoreLoad(ctx))
			{
				return false;
			}
		}
		return true;
	}
	
	array<ref PlayerStatBase> Get()
	{
		return m_PlayerStats;
	}
	
	void ResetAllStats()
	{
		m_PlayerStats.Clear();
		Init();
	}
}

//---------------------------------------------------------------
//------------------------- version 100 -------------------------
//---------------------------------------------------------------
enum EPlayerStats_v100
{
	HEATCOMFORT,
	TREMOR,
	WET,
	ENERGY,
	WATER,
	STOMACH_SOLID,
	STOMACH_ENERGY,
	STOMACH_WATER,
	DIET,
	STAMINA,
	SPECIALTY,
	BLOODTYPE,
};


class PlayerStatsPCO_v100 extends PlayerStatsPCO_Base
{

	override int GetVersion()
	{
		return 100;
	}
	
	override void Init()
	{
		RegisterStat(EPlayerStats_v100.HEATCOMFORT,  	new PlayerStat<float>	(-1,					1,						0,						"HeatComfort",		EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v100.TREMOR,		  	new PlayerStat<float>	(0,						1,						0,						"Tremor",			EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v100.WET,  			new PlayerStat<int>		(0,						1,						0,						"Wet",				EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v100.ENERGY,  			new PlayerStat<float>	(0,						20000,					600,					"Energy",			EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v100.WATER,  			new PlayerStat<float>	(0,						5000,					500,					"Water",			EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v100.STOMACH_SOLID,  	new PlayerStat<float>	(0,						5000,					0,						"StomachSolid",			EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v100.STOMACH_ENERGY,  	new PlayerStat<float>	(0,						40000,					0,						"StomachEnergy",	EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v100.STOMACH_WATER,  	new PlayerStat<float>	(0,						5000,					0,						"StomachWater",		EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v100.DIET,  			new PlayerStat<float>	(0,						5000,					2500,					"Diet",				EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v100.STAMINA,  		new PlayerStat<float>	(0,						GameConstants.STAMINA_MAX,			100,					"Stamina",			EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v100.SPECIALTY,  		new PlayerStat<float>	(-1,					1,						0,						"Specialty",		EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v100.BLOODTYPE,  		new PlayerStat<int>		(0,						128,					BloodTypes.GenerateBloodType(),	"BloodType",		EPSstatsFlags.EMPTY) );
	}
	
	void PlayerStatsPCO_v100()
	{
		
	}

};
//---------------------------------------------------------------
//------------------------- version 101 -------------------------
//---------------------------------------------------------------

enum EPlayerStats_v101
{
	HEATCOMFORT,
	TREMOR,
	WET,
	ENERGY,
	WATER,
	STOMACH_SOLID,
	STOMACH_ENERGY,
	STOMACH_WATER,
	DIET,
	STAMINA,
	SPECIALTY,
	BLOODTYPE,
	TOXICITY,
};


class PlayerStatsPCO_v101 extends PlayerStatsPCO_Base
{
	override int GetVersion()
	{
		return 101;
	}
	
	override void Init()
	{
		RegisterStat(EPlayerStats_v101.HEATCOMFORT,  	new PlayerStat<float>	(-1,					1,						0,						"HeatComfort",		EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v101.TREMOR,		  	new PlayerStat<float>	(0,						1,						0,						"Tremor",			EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v101.WET,  			new PlayerStat<int>		(0,						1,						0,						"Wet",				EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v101.ENERGY,  			new PlayerStat<float>	(0,						20000,					600,					"Energy",			EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v101.WATER,  			new PlayerStat<float>	(0,						5000,					500,					"Water",			EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v101.STOMACH_SOLID,  	new PlayerStat<float>	(0,						5000,					0,						"StomachSolid",			EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v101.STOMACH_ENERGY,  	new PlayerStat<float>	(0,						40000,					0,						"StomachEnergy",	EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v101.STOMACH_WATER,  	new PlayerStat<float>	(0,						5000,					0,						"StomachWater",		EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v101.DIET,  			new PlayerStat<float>	(0,						5000,					2500,					"Diet",				EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v101.STAMINA,  		new PlayerStat<float>	(0,						GameConstants.STAMINA_MAX,			100,					"Stamina",			EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v101.SPECIALTY,  		new PlayerStat<float>	(-1,					1,						0,						"Specialty",		EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v101.BLOODTYPE,  		new PlayerStat<int>		(0,						128,					BloodTypes.GenerateBloodType(),	"BloodType",		EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v101.TOXICITY,  		new PlayerStat<float>	(0,						100,					0,						"Toxicity",			EPSstatsFlags.EMPTY) );
	}
};


enum EPlayerStats_v106
{
	HEATCOMFORT,
	TREMOR,
	WET,
	ENERGY,
	WATER,
	DIET,
	STAMINA,
	SPECIALTY,
	BLOODTYPE,
	TOXICITY,
};

class PlayerStatsPCO_v106 extends PlayerStatsPCO_Base
{
	override int GetVersion()
	{
		return 106;
	}
	
	override void Init()
	{
		RegisterStat(EPlayerStats_v106.HEATCOMFORT,  	new PlayerStat<float>	(-1,	1,							0,								"HeatComfort",		EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v106.TREMOR,		  	new PlayerStat<float>	(0,		1,							0,								"Tremor",			EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v106.WET,  			new PlayerStat<int>		(0,		1,							0,								"Wet",				EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v106.ENERGY,  		new PlayerStat<float>	(0,		20000,						600,							"Energy",			EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v106.WATER,  			new PlayerStat<float>	(0,		5000,						650,							"Water",			EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v106.DIET,  			new PlayerStat<float>	(0,		5000,						2500,							"Diet",				EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v106.STAMINA,  		new PlayerStat<float>	(0,		GameConstants.STAMINA_MAX,	100,							"Stamina",			EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v106.SPECIALTY,  		new PlayerStat<float>	(-1,	1,							0,								"Specialty",		EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v106.BLOODTYPE,  		new PlayerStat<int>		(0,		128,						BloodTypes.GenerateBloodType(),	"BloodType",		EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v106.TOXICITY,  		new PlayerStat<float>	(0,		100,						0,								"Toxicity",			EPSstatsFlags.EMPTY) );
	}
};


enum EPlayerStats_v115
{
	HEATCOMFORT,
	TREMOR,
	WET,
	ENERGY,
	WATER,
	DIET,
	STAMINA,
	SPECIALTY,
	BLOODTYPE,
	TOXICITY,
	HEATBUFFER,
};

class PlayerStatsPCO_v115 extends PlayerStatsPCO_Base
{
	override int GetVersion()
	{
		return 115;
	}
	
	override void Init()
	{
		RegisterStat(EPlayerStats_v115.HEATCOMFORT,  	new PlayerStat<float>	(-1,	1,								0,								"HeatComfort",		EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v115.TREMOR,		  	new PlayerStat<float>	(0,		1,								0,								"Tremor",			EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v115.WET,  			new PlayerStat<int>		(0,		1,								0,								"Wet",				EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v115.ENERGY,  		new PlayerStat<float>	(0,		PlayerConstants.SL_ENERGY_MAX,	600,							"Energy",			EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v115.WATER,  			new PlayerStat<float>	(0,		PlayerConstants.SL_WATER_MAX,	600,							"Water",			EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v115.DIET,  			new PlayerStat<float>	(0,		5000,							2500,							"Diet",				EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v115.STAMINA,  		new PlayerStat<float>	(0,		CfgGameplayHandler.GetStaminaMax(),		100,					"Stamina",			EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v115.SPECIALTY,  		new PlayerStat<float>	(-1,	1,								0,								"Specialty",		EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v115.BLOODTYPE,  		new PlayerStat<int>		(0,		128,							BloodTypes.GenerateBloodType(),	"BloodType",		EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v115.TOXICITY,  		new PlayerStat<float>	(0,		100,							0,								"Toxicity",			EPSstatsFlags.EMPTY) );
		RegisterStat(EPlayerStats_v115.HEATBUFFER,  	new PlayerStat<float>	(-30,	30,								0,								"HeatBuffer",		EPSstatsFlags.EMPTY) );
	}
};


//---------------------------------------------------------------
//------------------------- version current -------------------------
//---------------------------------------------------------------

enum EPlayerStats_current: EPlayerStats_v115
{
	
};

//mod this class, it will always point to the latest version
class PlayerStatsPCO_current extends PlayerStatsPCO_v115
{

};