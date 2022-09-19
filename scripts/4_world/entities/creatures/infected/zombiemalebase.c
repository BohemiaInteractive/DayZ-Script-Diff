class ZombieMaleBase extends ZombieBase
{
	override string CaptureSound()
	{
		return "ZmbM_Normal_HeavyHit_Soundset";
	}
	
	override string ReleaseSound()
	{
		return "ZmbM_Normal_CallToArmsShort_Soundset";
	}
	
};

//! Base class for eatch Infected type
class ZmbM_CitizenASkinny_Base extends ZombieMaleBase
{
};

class ZmbM_CitizenASkinny_LT_Base extends ZombieMaleBase
{
};

class ZmbM_CitizenBFat_Base extends ZombieMaleBase
{
};

class ZmbM_ClerkFat_Base extends ZombieMaleBase
{
};

class ZmbM_ClerkFat_LT_Base extends ZombieMaleBase
{
};

class ZmbM_CommercialPilotOld_Base extends ZombieMaleBase
{
};

class ZmbM_CommercialPilotOld_LT_Base extends ZombieMaleBase
{
};

class ZmbM_ConstrWorkerNormal_Base extends ZombieMaleBase
{
};

class ZmbM_DoctorFat_Base extends ZombieMaleBase
{
};

class ZmbM_FarmerFat_Base extends ZombieMaleBase
{
};

class ZmbM_FarmerFat_LT_Base extends ZombieMaleBase
{
};

class ZmbM_FirefighterNormal_Base extends ZombieMaleBase
{
};

class ZmbM_FishermanOld_Base extends ZombieMaleBase
{
};

class ZmbM_HandymanNormal_Base extends ZombieMaleBase
{
};

class ZmbM_HeavyIndustryWorker_Base extends ZombieMaleBase
{
};

class ZmbM_HermitSkinny_Base extends ZombieMaleBase
{
};

class ZmbM_HikerSkinny_Base extends ZombieMaleBase
{
};

class ZmbM_HunterOld_Base extends ZombieMaleBase
{
};

class ZmbM_Jacket_Base extends ZombieMaleBase
{
};

class ZmbM_Jacket_LT_Base extends ZombieMaleBase
{
};

class ZmbM_JoggerSkinny_Base extends ZombieMaleBase
{
};

class ZmbM__Runner_Base extends ZombieMaleBase
{
};

class ZmbM_JournalistSkinny_Base extends ZombieMaleBase
{
};

class ZmbM_MechanicSkinny_Base extends ZombieMaleBase
{
};

class ZmbM_MotobikerFat_Base extends ZombieMaleBase
{
};

class ZmbM_OffshoreWorker_Base extends ZombieMaleBase
{
};

class ZmbM_ParamedicNormal_Base extends ZombieMaleBase
{
};

class ZmbM_PatientSkinny_Base extends ZombieMaleBase
{
};

class ZmbM_PatrolNormal_Base extends ZombieMaleBase
{
	override bool IsZombieMilitary()
	{
		return true;
	}
};

class ZmbM_PolicemanFat_Base extends ZombieMaleBase
{
};

class ZmbM_PolicemanSpecForce_Base extends ZombieMaleBase
{
};

class ZmbM_priestPopSkinny_Base extends ZombieMaleBase
{
};

class ZmbM_PrisonerSkinny_Base extends ZombieMaleBase
{
};

class ZmbM_SkaterYoung_Base extends ZombieMaleBase
{
};

class ZmbM_SkaterYoung_LT_Base extends ZombieMaleBase
{
};

class ZmbM_Soldier_Base extends ZombieMaleBase
{
	override bool IsZombieMilitary()
	{
		return true;
	}
};

class ZmbM_SoldierNormal_Base extends ZombieMaleBase
{
	override bool IsZombieMilitary()
	{
		return true;
	}
};

class ZmbM_SurvivorDean_Base extends ZombieMaleBase
{
};

class ZmbM_usSoldier_normal_Base extends ZombieMaleBase
{
	override bool IsZombieMilitary()
	{
		return true;
	}
};

class ZmbM_VillagerOld_Base extends ZombieMaleBase
{
};

class ZmbM_VillagerOld_LT_Base extends ZombieMaleBase
{
};

class ZmbM_NBC_Yellow extends ZombieMaleBase
{
	override bool ResistContaminatedEffect()
	{
		return true;
	}
};

class ZmbM_NBC_Grey extends ZombieMaleBase
{
	override bool ResistContaminatedEffect()
	{
		return true;
	}
	
	override bool IsZombieMilitary()
	{
		return true;
	}
};



