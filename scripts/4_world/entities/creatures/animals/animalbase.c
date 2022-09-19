class AnimalBase extends DayZAnimal
{
	override bool IsRefresherSignalingViable()
	{
		return false;
	}
}

class Animal_BosTaurus extends AnimalBase
{
	override void RegisterHitComponentsForAI()
	{
		m_DefaultHitComponent = "Zone_Chest";
		m_DefaultHitPositionComponent = "Pelvis";
		
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Head", 5);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Belly", 25);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Neck", 65);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Chest", 50);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Legs", 70);
	}
	
	override string CaptureSound()
	{
		return "CattleBellow_SoundSet";
	}
	
	override string ReleaseSound()
	{
		return "CattleMooA_SoundSet";
	}
}
class Animal_BosTaurusF extends Animal_BosTaurus {}

class Animal_CanisLupus extends AnimalBase
{
	override void RegisterHitComponentsForAI()
	{
		m_DefaultHitComponent = "Zone_Chest";
		m_DefaultHitPositionComponent = "Pelvis";
		
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Head", 2);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Neck", 65);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Chest", 50);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Legs_Back", 75);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Legs_Front", 70);
	}
	
	override bool IsDanger()
	{
		return true;
	}
	
	override string CaptureSound()
	{
		return "WolfWhimper_SoundSet";
	}
	
	override string ReleaseSound()
	{
		return "WolfBark_SoundSet";
	}
}

class Animal_CapraHircus extends AnimalBase
{
	override void RegisterHitComponentsForAI()
	{
		m_DefaultHitComponent = "Zone_Chest";
		m_DefaultHitPositionComponent = "Pelvis";
		
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Head", 4);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Neck", 65);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Chest", 50);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Legs", 70);
	}
	
	override string CaptureSound()
	{
		return "GoatBleat_B_SoundSet";
	}
	
	override string ReleaseSound()
	{
		return "GoatBleat_A_SoundSet";
	}
}
class Animal_CapraHircusF extends Animal_CapraHircus {}

class Animal_CapreolusCapreolus extends AnimalBase
{
	override void RegisterHitComponentsForAI()
	{
		m_DefaultHitComponent = "Zone_Chest";
		m_DefaultHitPositionComponent = "Pelvis";
		
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Head", 4);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Neck", 65);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Chest", 50);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Legs", 70);
	}
	
	override string CaptureSound()
	{
		return "HareSquawk_SoundSet";
	}
	
	override string ReleaseSound()
	{
		return "HareChirp_SoundSet";
	}
}
class Animal_CapreolusCapreolusF extends Animal_CapreolusCapreolus {}

class Animal_CervusElaphus extends AnimalBase
{
	override void RegisterHitComponentsForAI()
	{
		m_DefaultHitComponent = "Zone_Chest";
		m_DefaultHitPositionComponent = "Pelvis";
		
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Head", 2);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Neck", 65);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Chest", 50);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Legs", 70);
	}
	
	override string CaptureSound()
	{
		return "DeerBleat_SoundSet";
	}
	
	override string ReleaseSound()
	{
		return "DeerRoar_SoundSet";
	}
}
class Animal_CervusElaphusF extends Animal_CervusElaphus {}

class Animal_GallusGallusDomesticus extends AnimalBase
{
	override void RegisterHitComponentsForAI()
	{
		m_DefaultHitComponent = "Spine";
		m_DefaultHitPositionComponent = "Spine";
		
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Head", 1);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Head", 5);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Head", 20);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "LArm1", 33);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "RArm1", 33);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Spine", 70);
	}
	
	override string CaptureSound()
	{
		return "HenScream_SoundSet";
	}
	
	override string ReleaseSound()
	{
		return "HenCluck_X_SoundSet";
	}
	
	override bool ReplaceOnDeath()
	{
		return true;
	}
	
	override bool CanBeSkinned()
	{
		return false;
	}
	
	override string GetDeadItemName()
	{
		return "DeadRooster";
	}
	
	override bool KeepHealthOnReplace()
	{
		return false;
	}
}

class Animal_GallusGallusDomesticusF extends Animal_GallusGallusDomesticus 
{
	override string GetDeadItemName()
	{
		return "DeadChicken_Brown";
	}
}

class Animal_GallusGallusDomesticusF_Brown extends Animal_GallusGallusDomesticusF
{
	override string GetDeadItemName()
	{
		return "DeadChicken_Brown";
	}
}

class Animal_GallusGallusDomesticusF_Spotted extends Animal_GallusGallusDomesticusF
{
	override string GetDeadItemName()
	{
		return "DeadChicken_Spotted";
	}
}

class Animal_GallusGallusDomesticusF_White extends Animal_GallusGallusDomesticusF
{
	override string GetDeadItemName()
	{
		return "DeadChicken_White";
	}
}


class Animal_OvisAries extends AnimalBase
{
	override void RegisterHitComponentsForAI()
	{
		m_DefaultHitComponent = "Zone_Chest";
		m_DefaultHitPositionComponent = "Pelvis";
		
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Head", 4);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Chest", 15);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Neck", 55);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Chest", 50);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Legs", 70);
	}
	
	override string CaptureSound()
	{
		return "SheepBleat_E_SoundSet";
	}
	
	override string ReleaseSound()
	{
		return "SheepBleat_G_SoundSet";
	}
}
class Animal_OvisAriesF extends Animal_OvisAries {}

class Animal_SusDomesticus extends AnimalBase
{
	override void RegisterHitComponentsForAI()
	{
		m_DefaultHitComponent = "Zone_Spine";
		m_DefaultHitPositionComponent = "Spine1";
		
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Head", 5);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Belly", 25);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Neck", 65);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Chest", 50);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Legs", 70);
	}
	
	override string CaptureSound()
	{
		return "HogSqueal_SoundSet";
	}
	
	override string ReleaseSound()
	{
		return "HogGrunt_G_SoundSet";
	}
}

class Animal_SusScrofa extends AnimalBase
{
	override void RegisterHitComponentsForAI()
	{
		m_DefaultHitComponent = "Zone_Spine";
		m_DefaultHitPositionComponent = "Pelvis";
		
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Head", 3);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Neck", 55);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Belly", 15);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Legs", 70);
	}
	
	override string CaptureSound()
	{
		return "HogSqueal_SoundSet";
	}
	
	override string ReleaseSound()
	{
		return "HogGrunt_G_SoundSet";
	}
}

class Animal_UrsusArctos extends AnimalBase
{
	override void RegisterHitComponentsForAI()
	{
		m_DefaultHitComponent = "Zone_Head";
		m_DefaultHitPositionComponent = "Pelvis";
		
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Head", 25);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Belly", 50);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Neck", 65);
		DayZAIHitComponentHelpers.RegisterHitComponent(m_HitComponentsForAI, "Zone_Legs", 70);
	}
	
	override bool IsDanger()
	{
		return true;
	}
	
	override string CaptureSound()
	{
		return "BearSnarl_SoundSet";
	}
	
	override string ReleaseSound()
	{
		return "BearRoarShort_SoundSet";
	}
}