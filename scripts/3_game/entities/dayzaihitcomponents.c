//! holds hit components and its weights for attack from AI (used on each type - DayZPlayerType, DayZAnimalType, etc.)
class DayZAIHitComponent
{
	string m_Name; 			//! "Head", "Torso", etc.
	int m_Weight;			//! [0..1]
};

class DayZAIHitComponentHelpers
{
	/**
	 * @fn		RegisterHitComponent
	 * @brief	Register Hit Component for AI targeting
	 *
	 * Register Hit Component for AI targeting to array defined on entity's Type (DayZPlayerType, DayZInfectedType, etc.)
	 * Uses component name from damage system (zone) and its weight.
	 *
	 * @param[in]	pHitComponents	\p		array that keeps registered compoenents
	 * @param[in]	pName			\p		component name (zone in Damage System)
	 * @param[in]	pWeight			\p		weight of the component (probability)
	 **/
	static void RegisterHitComponent(array<ref DayZAIHitComponent> pHitComponents, string pName, float pWeight)
	{
		DayZAIHitComponent newComponent = new DayZAIHitComponent();
		
		newComponent.m_Name = pName;
		newComponent.m_Weight = pWeight;

		pHitComponents.Insert(newComponent);
	}

	/**
	 * @fn		ComputeHitComponentProbability
	 * @brief	Computes hit component probability based on weights
	 *
	 * @param[in]	pHitComponents	\p		array that keeps registered compoenents
	 * @param[out]	pHitComponent	\p		selected component name (can be passed to Damage System)
	 *
	 * @return	true if found, false otherwise
	 **/
	static bool SelectMostProbableHitComponent(array<ref DayZAIHitComponent> pHitComponents, out string pHitComponent)
	{
		int weights = SumOfWeights(pHitComponents);
		float rnd = Math.RandomInt(0, weights);

		for ( int i = 0; i < pHitComponents.Count(); ++i )
		{
			DayZAIHitComponent hitComp = pHitComponents.Get(i);
			rnd -= hitComp.m_Weight;
			
			if (rnd <= 0)
			{
				pHitComponent = hitComp.m_Name;
				return true;
			}
		}
		
		return false;
	}

	/**
	 * @fn		SumOfWeights
	 * @brief	Calculates the sum of all entries in DayZAIHitComponent array
	 *
	 * @param[in]	pHitComponents	\p		array that keeps registered compoenents
	 *
	 * @return	Sum of weights of all entries in array
	 **/
	static int SumOfWeights(array<ref DayZAIHitComponent> pHitComponents)
	{
		int sum = 0;

		for( int i = 0; i < pHitComponents.Count(); ++i )
		{
			DayZAIHitComponent hitComp = pHitComponents.Get(i);
			sum += hitComp.m_Weight;
		}
		
		return sum;
	}
};