typedef int[] SurfaceInfo;

/*!
	Unmanaged surface info handle. Provides API to surfaces that are defined as part of 'CfgSurfaces' or exist in an objects .bisurf file.
	Lifetime is managed in code so don't store handles of this type yourself.
*/
class SurfaceInfo
{
	private void SurfaceInfo() {};
	private void ~SurfaceInfo() {};
	
	//! Warning: O(n) time complexity where n is the total number of loaded surfaces
	//! Note: Will load the surface if not loaded
	//! Warning: If the surface name is invalid, it will still create a SurfaceInfo
	proto static SurfaceInfo GetByName(string name);
	
	//! Warning: O(n) time complexity where n is the total number of loaded surfaces
	//! Note: Will load the surface if not loaded
	//! Warning: If the surface name is invalid, it will still create a SurfaceInfo
	//! 'CfgSurfaces' can be pathed by having the name prefixed with '#', so 'GetByFile("#cp_grass")' will return same as 'GetByName("cp_grass")'
	proto static SurfaceInfo GetByFile(string name);

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
