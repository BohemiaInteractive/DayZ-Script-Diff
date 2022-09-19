/**@class	WeaponStateJammed
 * @brief	handle jamming state set jam/unjam state for weapon 
 **/
class WeaponStateJammed extends WeaponStableState
{
	/**@fn		OnEntry
	 * @brief	called upon entry to state
	 * @NOTE	if state has (non-running) sub-machine, it's started on entry
	 * @param[in] e	the event that triggered transition to this state
	 **/
	override void OnEntry (WeaponEventBase e)
	{
		super.OnEntry(e);
		m_weapon.SetJammed(true);
	}


	/**@fn		OnExit
	 * @brief called on exit from state
	 * @param[in] e	the event that triggered transition from this state
	 **/
	override void OnExit (WeaponEventBase e)
	{
		super.OnExit(e);
		//m_weapon.SetJammed(false);
	}
};


