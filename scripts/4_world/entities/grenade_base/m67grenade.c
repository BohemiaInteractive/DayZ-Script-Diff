class M67Grenade extends Grenade_Base
{
	void M67Grenade()
	{
		SetAmmoType("M67Grenade_Ammo");
		SetFuseDelay(4);
		SetParticleExplosion(ParticleList.M67);
	}

	void ~M67Grenade() {}
}