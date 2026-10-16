//! Identifiers of individual vehicle VFX effects.
enum EVehicleVFXEffect
{
	INVALID, //! unassigned for future modding compatibility
	DUST_BEHIND,
	ENGINE_SMOKE,
	COOLANT_STEAM,
	EXHAUST_SMOKE,
}

//! Identifiers of vehicle VFX effect groups - multiple instances of a single effect type.
enum EVehicleVFXEffectGroup
{
	INVALID, //! unassigned for future modding compatibility
	WHEEL_SMOKE,
	WHEEL_CONTACT,
}
