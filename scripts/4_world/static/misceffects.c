class MiscEffects : Managed
{
	// Called by individual clients
	static void PlayVegetationCollideParticles(Object object)
	{
		int particleID;
		string particleName;
		string configPath = "CfgNonAIVehicles " + object.GetType() + " collisionParticle";
		GetGame().ConfigGetText(configPath, particleName);
		if (particleName == string.Empty)	// no ptc assigned
			return;
		
		particleID = ParticleList.GetParticleIDByName(particleName);
		ParticleManager.GetInstance().PlayInWorld(particleID, object.GetPosition());
	}

}