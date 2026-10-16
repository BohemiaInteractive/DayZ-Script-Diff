class Mag_FNX45_15Rnd: MagazineStorage {};
class Mag_357Speedloader_6Rnd: MagazineStorage {};
class Mag_Deagle_9rnd: MagazineStorage {};
class Mag_1911_7Rnd: MagazineStorage {};
class Mag_CZ75_15Rnd: MagazineStorage {};
class Mag_Glock_15Rnd: MagazineStorage {};
class Mag_P1_8Rnd: MagazineStorage {};
class Mag_IJ70_8Rnd: MagazineStorage {};
class Mag_MP5_15Rnd: MagazineStorage {};
class Mag_MP5_30Rnd: MagazineStorage {};
class Mag_PM73_15Rnd: MagazineStorage {};
class Mag_PM73_25Rnd: MagazineStorage {};
class Mag_CZ61_20Rnd: MagazineStorage {};
class Mag_MKII_10Rnd: MagazineStorage {};
class Mag_ShockCartridge: MagazineStorage {};
class Mag_Ruger1022_10Rnd: MagazineStorage {};
class Mag_Ruger1022_15Rnd: MagazineStorage {};
class Mag_Ruger1022_30Rnd: MagazineStorage {};
class Mag_CLIP762x54_5Rnd: MagazineStorage {};
class Mag_762x54Snaploader_2Rnd: MagazineStorage {};
class Mag_308WinSnaploader_2Rnd: MagazineStorage {};
class Mag_CLIP762x39_10Rnd: MagazineStorage {};
class Mag_CLIP9x19_10Rnd: MagazineStorage {};
class Mag_AKM_30Rnd: MagazineStorage {};
class Mag_AKM_Drum75Rnd: MagazineStorage {};
class Mag_AKM_Palm30Rnd: MagazineStorage {};
class Mag_CZ527_5rnd: MagazineStorage {};
class Mag_STANAG_30Rnd: MagazineStorage {};
class Mag_STANAGCoupled_30Rnd: MagazineStorage {};
class Mag_CMAG_10Rnd: MagazineStorage {};
class Mag_CMAG_20Rnd: MagazineStorage {};
class Mag_CMAG_30Rnd: MagazineStorage {};
class Mag_CMAG_40Rnd: MagazineStorage {};
class Mag_AK101_30Rnd: MagazineStorage {};
class Mag_AK74_30Rnd: MagazineStorage {};
class Mag_SVD_10Rnd: MagazineStorage {};
class Mag_FAL_20Rnd: MagazineStorage {};
class Mag_Scout_5Rnd: MagazineStorage {};
class Mag_UMP_25Rnd: MagazineStorage {};
class Mag_VSS_10Rnd: MagazineStorage {};
class Mag_M249_Box200Rnd: MagazineStorage {};
class Mag_12gaSnaploader_2Rnd: MagazineStorage {};
class Mag_Saiga_5Rnd: MagazineStorage {};
class Mag_Saiga_8Rnd: MagazineStorage {};
class Mag_Saiga_Drum20Rnd: MagazineStorage {};
class Mag_Arrows_Quiver: MagazineStorage {};
class Mag_Bolts_Quiver: MagazineStorage {};
class Mag_Dartgun_CO2 : MagazineStorage {};
class Mag_Groza_20Rnd: MagazineStorage {};
class Mag_MakarovPB_8Rnd: MagazineStorage {};
class Mag_VAL_20Rnd: MagazineStorage {};
class Mag_STANAG_60Rnd: MagazineStorage {};
class Mag_AK74_45Rnd: MagazineStorage {};
class Mag_FAMAS_25Rnd: MagazineStorage {};
class Mag_Aug_30Rnd: MagazineStorage {};
class Mag_PP19_64Rnd: MagazineStorage {};
class Mag_CZ550_4rnd: MagazineStorage {};
class Mag_CZ550_10rnd: MagazineStorage {};
class Mag_SSG82_5rnd: MagazineStorage {};
class Mag_M14_10Rnd: MagazineStorage {};
class Mag_M14_20Rnd: MagazineStorage {};
class Mag_Vikhr_30Rnd: MagazineStorage {};
class Mag_SV98_10Rnd: MagazineStorage {};
class Mag_SCARH_20Rnd: MagazineStorage {};
class Mag_Luger_8Rnd: MagazineStorage {};
class Mag_LeeEnfield_10Rnd: MagazineStorage {};
class Mag_MP18_Drum32Rnd : MagazineStorage
{
	
	//! at 1.0 the magazine is at max bullets
	protected float m_AnimStart = 1.0;
		 
	//! at 0.0 the magazine is at 12 bullets and anim stops there as requested by designers
	protected float m_AnimEnd = 0.0; 
	
	protected float m_LastAnimPhase = 0.0;
	
	//! the number of bullets at which the animation reaches m_AnimEnd (e.g. animation finishes at 12 bullets)
	protected float m_AmmoOffset = 12;
	protected float m_MaxAmmoPercentage = 1.0 /(GetAmmoMax() - m_AmmoOffset);
	protected string m_AnimationSource = "drum";
	protected string m_DrumLeverSoundset = "wpn_mp18_drum_mag_SoundSet";
	
	override void UpdateAnimation()
	{
		int ammoCount = GetAmmoCount();
		float animPhase = Math.Lerp(m_AnimStart, m_AnimEnd, (ammoCount - m_AmmoOffset) * m_MaxAmmoPercentage);
		SetAnimationPhase(m_AnimationSource, animPhase);
		m_LastAnimPhase = animPhase;
		if(g_Game && !g_Game.IsDedicatedServer())
		{
			if(ammoCount >= m_AmmoOffset)
			{
				EffectSound sound;
				PlaySoundSetAtMemoryPoint(sound, m_DrumLeverSoundset, "drum_sound");
				sound.SetAutodestroy(true);
			}
		}
	}
	
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		ctx.Write(m_LastAnimPhase);
	}
	
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if(!super.OnStoreLoad(ctx, version))
			return false;
		
		if(version >= 144)
		{
			if(!ctx.Read(m_LastAnimPhase))
			{
				return false;
			}
		}
		SetAnimationPhase(m_AnimationSource, m_LastAnimPhase);
		return true;
	}
};
