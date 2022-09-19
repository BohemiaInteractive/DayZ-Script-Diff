class FlashGrenade extends Grenade_Base
{
	const float FX_RANGE_MAX_MULT = 1.0;
	
	override void OnExplosionEffects(Object source, Object directHit, int componentIndex, string surface, vector pos, vector surfNormal, float energyFactor, float explosionFactor, bool isWater, string ammoType)
	{
		super.OnExplosionEffects(source, directHit, componentIndex, surface, pos, surfNormal, energyFactor, explosionFactor, isWater, ammoType);

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());

		if (player)
		{
			vector headPos 			= player.GetDamageZonePos("Head"); // animated position in the middle of the zone
			float ammoRangeKill	 	= GetGame().ConfigGetFloat(string.Format("CfgAmmo %1 indirectHitRange", ammoType));
			float ammoRangeMaxMult 	= 4.0;
			
			string indirectHitRangeMultiplier = string.Format("CfgAmmo %1 indirectHitRangeMultiplier", ammoType);
			if (GetGame().ConfigIsExisting(indirectHitRangeMultiplier))
			{
				//! values less than 1.0 make no sense
				ammoRangeMaxMult = Math.Clamp(GetGame().ConfigGetFloat(indirectHitRangeMultiplier), 1.0, float.MAX);
			}

			float ammoRangeMax = ammoRangeKill * ammoRangeMaxMult;
			ammoRangeMax = Math.Clamp(ammoRangeMax * FX_RANGE_MAX_MULT, ammoRangeKill, ammoRangeMax);
			
			float dist			= vector.Distance(headPos, pos);
			float distSq 		= vector.DistanceSq(headPos, pos);
			float radiusMaxSq = Math.SqrFloat(ammoRangeMax);
			
			if (distSq <= radiusMaxSq)
			{
				// ignore collisions with parent if fireplace
				InventoryItem invItem = InventoryItem.Cast(source);
				EntityAI parent = invItem.GetHierarchyParent();
				array<Object> excluded = new array<Object>;
				
				if (!parent || !parent.IsFireplace())
					parent = null;
				else if (parent)
					excluded.Insert(parent);
				
				array<ref RaycastRVResult> results = new array<ref RaycastRVResult>;
				excluded.Insert(this); //Ignore self for visibility check
				
				//There shouldn't be cases justifying we go further than first entry (if in fireplace, self does not impact)
				RaycastRVParams rayParams	= new RaycastRVParams(pos, headPos, excluded[0]);
				rayParams.flags				= CollisionFlags.ALLOBJECTS;
				DayZPhysics.RaycastRVProxy(rayParams, results, excluded);

				//! removes possible obstacles made by items around the grenade(or on the same position)
				array<Object> hitObjects = new array<Object>;
				for (int i = 0; i < results.Count(); i++)
				{
					if (results[i].obj && !results[i].obj.IsInherited(ItemBase))
					{
						hitObjects.Insert(results[i].obj);
					}
				}

				//If player is not first index, object is between player and grenade
				if (hitObjects.Count() && PlayerBase.Cast(hitObjects[0]))
				{
					float effectCoef;
					if (ammoRangeMax == ammoRangeKill)
					{
						effectCoef = 1.0; //edge case, landed right on the edge
					}
					effectCoef = 1 - ((dist - ammoRangeKill) / (ammoRangeMax - ammoRangeKill));
					effectCoef = Math.Clamp(effectCoef, 0.1, 100.0);

					player.OnPlayerReceiveFlashbangHitStart(MiscGameplayFunctions.IsPlayerOrientedTowardPos(player, pos, 60));
					player.GetFlashbangEffect().SetupFlashbangValues(effectCoef, effectCoef, effectCoef);
				}
			}
		}
	}

	void FlashGrenade()
	{
		SetAmmoType("FlashGrenade_Ammo");
		SetFuseDelay(2);
		SetGrenadeType(EGrenadeType.ILLUMINATING);
		SetParticleExplosion(ParticleList.GRENADE_M84);
	}
	
	protected override void CreateLight()
	{
		m_Light = ExplosiveLight.Cast(ScriptedLightBase.CreateLight(FlashGrenadeLight, GetPosition()));
	}

	void ~FlashGrenade() {}
}