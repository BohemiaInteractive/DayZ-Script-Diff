typedef int[] SurfaceInfo;

/*!
	Unmanaged surface info handle. Provides API to surfaces that are defined as part of 'CfgSurfaces' or exist in an objects .bisurf file.
	Lifetime is managed in code so don't store handles of this type yourself.
*/
class SurfaceInfo
{
	private void SurfaceInfo() {};
	private void ~SurfaceInfo() {};
	
	proto string GetName();
	proto string GetEntryName();
	proto string GetSurfaceType();
	
	proto float GetRoughness();
	proto float GetDustness();
	proto float GetBulletPenetrability();
	proto float GetThickness();
	proto float GetDeflection();
	proto float GetTransparency();
	proto float GetAudability();
	
	proto bool IsLiquid();
	proto bool IsStairs();
	proto bool IsPassthrough();
	proto bool IsSolid();
	
	proto string GetSoundEnv();
	proto string GetImpact();
	
	//! See 'LiquidTypes' in 'constants.c'
	proto int GetLiquidType();

	//! See 'ParticleList', if config entry not set, value is 'ParticleList.NONE', 
	//! if config entry is set but doesn't exist, value is 'ParticleList.INVALID'
	proto int GetStepParticleId();
	proto int GetWheelParticleId();
};
