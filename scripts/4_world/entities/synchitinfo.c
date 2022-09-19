//! extendable type to allow for smarter hit data syncing
class SyncHitInfo extends Managed
{
	int m_AnimType;
	float m_HitDir;
	float m_HealthDamage;
	bool m_Fullbody;
	//ref TotalDamageResult m_DamageResult;
	bool m_HasSource;
};