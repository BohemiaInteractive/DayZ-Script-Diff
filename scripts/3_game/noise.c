class NoiseSystem
{
	private void NoiseSystem() {}
	private void ~NoiseSystem() {}
	
	proto void AddNoise(EntityAI source_entity, NoiseParams noise_params, float external_strenght_multiplier = 1.0);
	proto void AddNoisePos(EntityAI source_entity, vector pos, NoiseParams noise_params, float external_strenght_multiplier = 1.0);
	
	//! Will make a noise at that position which the AI will "see" for the duration of 'lifetime'
	proto void AddNoiseTarget(vector pos, float lifetime, NoiseParams noise_params, float external_strength_multiplier = 1.0);
}
	
class NoiseParams
{	
	void NoiseParams()
	{
		
	}
	
	proto native void Load(string noise_name);
	
	proto native void LoadFromPath(string noise_path);
}