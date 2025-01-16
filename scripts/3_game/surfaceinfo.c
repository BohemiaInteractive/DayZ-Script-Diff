typedef int[] SurfaceInfo;

/*!
	Unmanaged surface info handle. Provides API to surfaces that are defined as part of 'CfgSurfaces' or exist in an objects .bisurf file.
	Lifetime is managed in code so don't store handles of this type yourself.
	Any created 'SurfaceInfo' is destroyed during 'Game' ScriptModule destruction.
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

enum UseObjectsMode
{
	//! wait for the data to be ready
	Wait,
	
	//! do not wait for the data, but refresh caches
	NoWait,
	
	//! only return the data we have, do not touch any caches (MT safe mode)
	NoLock
};

enum SurfaceDetectionType
{
	Scenery,
	Roadway
};

/*!
	Parameters for detecting the surface
*/
/*sealed*/ class SurfaceDetectionParameters
{
	//! Type of surface to detect
	SurfaceDetectionType type = SurfaceDetectionType.Scenery;
	
	//! 3D position to trace the surface from
	vector position;
	
	//! Include water in the surface detection, will return the water if it is higher than the surface
	bool includeWater = false;
	
	//! See UseObjectsMode, SurfaceTraceType.Roadway only
	UseObjectsMode syncMode = UseObjectsMode.Wait;
	
	//! Object to ignore tracing against, SurfaceTraceType.Roadway only
	Object ignore = null;
	
	//! See RoadSurfaceDetection, SurfaceTraceType.Roadway only
	RoadSurfaceDetection rsd = RoadSurfaceDetection.ABOVE;
	
};

/*!
	Output of surface detection
*/
/*sealed*/ class SurfaceDetectionResult
{
	//! Height position
	float height = 0;
	
	//! Right normal 
	float normalX = 0;
	
	//! Up normal - always set to 1, not necessary to expose
	//float normalY = 1;
	
	//! Forward normal
	float normalZ = 0;
	
	//! Returned SurfaceInfo, plain pointer
	SurfaceInfo surface = null;
	
	//! If water was the returned surface
	bool aboveWater = false;
	
	//! SurfaceTraceType.Roadway only
	Object object = null;
	
};
