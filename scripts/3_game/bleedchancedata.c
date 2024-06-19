typedef map<int,float> BleedChanceMaxMap; //<bloodDamageReceived,chanceMax>

//! Static data of bleeding chance probabilities; currently used for melee only
class BleedChanceData : Managed
{
	private static const float BLOOD_HITPOINTS_UNIVERSAL = 100.0;
	
	private static ref map<string, ref BleedChanceMaxMap> m_DamageTypeMap;
	
	static void InitBleedChanceData()
	{
		m_DamageTypeMap = new map<string,ref BleedChanceMaxMap>();
		
		InitMeleeChanceMap();
		InitInfectedChanceMap();
	}
	
	static void InitMeleeChanceMap()
	{
		if (m_DamageTypeMap.Contains("Melee"))
			ErrorEx("'Melee' damage type bleed chances already initialized!");
		
		BleedChanceMaxMap bleedChanceMaxMap = new BleedChanceMaxMap();
		
		//keys have to be integers, recalculated later
		bleedChanceMaxMap.Set(0,0.0);
		bleedChanceMaxMap.Set(10,5.0);
		bleedChanceMaxMap.Set(20,15.0);
		bleedChanceMaxMap.Set(30,23.4);
		bleedChanceMaxMap.Set(40,31.2);
		bleedChanceMaxMap.Set(50,39.0);
		bleedChanceMaxMap.Set(60,46.8);
		bleedChanceMaxMap.Set(70,54.6);
		bleedChanceMaxMap.Set(80,62.4);
		bleedChanceMaxMap.Set(90,70.2);
		bleedChanceMaxMap.Set(BLOOD_HITPOINTS_UNIVERSAL,100.0);
		
		m_DamageTypeMap.Set("Melee",bleedChanceMaxMap);
	}
	
	static void InitInfectedChanceMap()
	{
		if (m_DamageTypeMap.Contains("Infected"))
			ErrorEx("'Infected' damage type bleed chances already initialized!");
		
		BleedChanceMaxMap bleedChanceMaxMap = new BleedChanceMaxMap();
		
		//keys have to be integers, recalculated later
		bleedChanceMaxMap.Set(0,0.0);
		bleedChanceMaxMap.Set(10,5.0);
		bleedChanceMaxMap.Set(20,15.0);
		bleedChanceMaxMap.Set(30,27.5);
		bleedChanceMaxMap.Set(40,40.0);
		bleedChanceMaxMap.Set(50,55.0);
		bleedChanceMaxMap.Set(60,60.0);
		bleedChanceMaxMap.Set(70,70.0);
		bleedChanceMaxMap.Set(80,75.0);
		bleedChanceMaxMap.Set(90,85.0);
		bleedChanceMaxMap.Set(BLOOD_HITPOINTS_UNIVERSAL,100.0);
		
		m_DamageTypeMap.Set("Infected",bleedChanceMaxMap);
	}
	
	static void Cleanup()
	{
		delete m_DamageTypeMap;
	}
	
	//! returns 'false' when damageType is unhandled
	static bool CalculateBleedChance(string damageType, float bloodDamage, float bleedThreshold, out float bleedChance)
	{
		map<int,float> bleedChanceMap = m_DamageTypeMap.Get(damageType);
		if (!bleedChanceMap)
			return false;
		
		float armor = bloodDamage / BLOOD_HITPOINTS_UNIVERSAL;
		float valueHigher = Math.Max(bleedThreshold,armor) * BLOOD_HITPOINTS_UNIVERSAL;
		float valueLower = Math.Min(bleedThreshold,armor) * BLOOD_HITPOINTS_UNIVERSAL;
		
		#ifdef ENABLE_LOGGING
		if (LogManager.IsBleedingChancesLogEnable())
		{	
			Debug.BleedingChancesLog(armor.ToString(), "BleedChanceData" , "n/a", "armor:");
			Debug.BleedingChancesLog(bleedThreshold.ToString(), "BleedChanceData" , "n/a", "bleedThreshold:");
		}
		#endif
		
		//unexpected values, unhandled
		if (valueLower > BLOOD_HITPOINTS_UNIVERSAL)
		{
			bleedChance = 1.0;
			
			#ifdef ENABLE_LOGGING
			if (LogManager.IsBleedingChancesLogEnable())
			{	
				Debug.BleedingChancesLog(bleedChance.ToString(), "BleedChanceData" , "n/a", "Unhandleed values, default bleeding chance used:");
			}
			#endif
			
			return true;
		}
		
		if (bleedChanceMap.Contains(valueLower))
		{
			bleedChance = bleedChanceMap.Get(valueLower) * valueHigher / 10000;
		}
		else
		{
			float chanceMaxActual;
			
			float floor = Math.Floor(valueLower / 10) * 10;
			float ceil = Math.Ceil(valueLower / 10) * 10;
			float pos = Math.InverseLerp(floor,ceil,valueLower);
			
			float chanceMin = bleedChanceMap.Get(floor);
			float chanceMax = bleedChanceMap.Get(ceil);
			
			chanceMaxActual = Math.Lerp(chanceMin,chanceMax,pos);
			bleedChance = valueHigher * chanceMaxActual / 10000;
		}
		
		#ifdef ENABLE_LOGGING
		if (LogManager.IsBleedingChancesLogEnable())
		{	
			Debug.BleedingChancesLog(bleedChance.ToString(), "BleedChanceData" , "n/a", "bleeding chance:");
		}
		#endif
		
		return true;
	}
}