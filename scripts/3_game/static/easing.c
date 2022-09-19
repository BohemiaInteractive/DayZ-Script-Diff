//! Input value between 0 and 1, returns value adjusted by easing, no automatic clamping of input(do your own !!)
class Easing
{
	static float EaseInSine( float t ) 
	{
		return -1 * Math.Cos( t * ( Math.PI / 2 ) ) + 1;
	}

	static float EaseOutSine( float t ) 
	{
		return Math.Sin( t * ( Math.PI / 2 ) );
	}

	static float EaseInOutSine( float t ) 
	{
		return -0.5 * ( Math.Cos( Math.PI * t ) - 1 );
	}

	static float EaseInQuad( float t ) 
	{
		return t * t;
	}

	static float EaseOutQuad( float t ) 
	{
		return t * ( 2 - t );
	}

	static float EaseInOutQuad( float t ) 
	{
		if(t < 0.5)
			return 2 * t * t;
		else 
			return -1 + ( 4 - 2 * t ) * t;
	}

	static float EaseInCubic( float t ) 
	{
		return t * t * t;
	}

	static float EaseOutCubic( float t ) 
	{
		float t1 = t - 1;
		return t1 * t1 * t1 + 1;
	}

	static float EaseInOutCubic( float t ) 
	{
		if( t < 0.5 ) 
			return 4 * t * t * t;
		else 
			return ( t - 1 ) * ( 2 * t - 2 ) * ( 2 * t - 2 ) + 1;
	}

	static float EaseInQuart( float t ) 
	{
		return t * t * t * t;
	}

	static float EaseOutQuart( float t ) 
	{
		float t1 = t - 1;
		return 1 - t1 * t1 * t1 * t1;
	}


	static float EaseInOutQuart( float t ) 
	{
		float t1 = t - 1;
		
		if(t < 0.5)
			return 8 * t * t * t * t;
		else
			return 1 - 8 * t1 * t1 * t1 * t1;
	}

	static float EaseInQuint( float t ) 
	{
		return t * t * t * t * t;
	}

	static float EaseOutQuint( float t ) 
	{
		float t1 = t - 1;
		return 1 + t1 * t1 * t1 * t1 * t1;
	}

	static float EaseInOutQuint( float t ) 
	{
		float t1 = t - 1;
		
		if( t < 0.5 )
		{
			return 16 * t * t * t * t * t;
		}
		else
		{
			return  1 + 16 * t1 * t1 * t1 * t1 * t1;
		}
	}

	static float EaseInExpo( float t ) 
	{
		if( t == 0 ) 
		{
			return 0;
		}
		
		return 	Math.Pow( 2, 10 * ( t - 1 ) );
	}

	static float EaseOutExpo( float t ) 
	{
		if( t == 1 ) {
			return 1;
		}

		return ( -Math.Pow( 2, -10 * t ) + 1 );
	}

	static float EaseInOutExpo( float t ) 
	{
    
		if( t == 0 || t == 1 ) 
		{
			return t;
    	}

	    float scaledTime = t * 2;
	    float scaledTime1 = scaledTime - 1;
			
	    if( scaledTime < 1 ) 
		{
	       return 0.5 * Math.Pow( 2, 10 *  scaledTime1 );
	   	}
	
		return 0.5 * ( -Math.Pow( 2, -10 * scaledTime1 ) + 2 );

	}

	static float EaseInCirc( float t ) 
	{
		float scaledTime = t / 1;
		return -1 * ( Math.Sqrt( 1 - scaledTime * t ) - 1 );
	}

	static float EaseOutCirc( float t ) 
	{
		float t1 = t - 1;
		return Math.Sqrt( 1 - t1 * t1 );
	}
	
	static float EaseInOutCirc( float t ) 
	{

		float scaledTime = t * 2;
		float scaledTime1 = scaledTime - 2;

		if( scaledTime < 1 ) 
		{
			return -0.5 * ( Math.Sqrt( 1 - scaledTime * scaledTime ) - 1 );
		}

		return 0.5 * ( Math.Sqrt( 1 - scaledTime1 * scaledTime1 ) + 1 );
	}

	static float EaseInBack( float t, float magnitude = 1.70158 ) 
	{
		return t * t * ( ( magnitude + 1 ) * t - magnitude );
	}

	static float EaseOutBack( float t, float magnitude = 1.70158 ) 
	{
		float scaledTime = ( t / 1 ) - 1;
		return (scaledTime * scaledTime * ( ( magnitude + 1 ) * scaledTime + magnitude )) + 1;
	}

	static float EaseInOutBack( float t, float magnitude = 1.70158 ) 
	{

		float scaledTime = t * 2;
		float scaledTime2 = scaledTime - 2;

		float s = magnitude * 1.525;

		if( scaledTime < 1) 
		{
			return 0.5 * scaledTime * scaledTime * (( ( s + 1 ) * scaledTime ) - s);
		}

		return 0.5 * (scaledTime2 * scaledTime2 * ( ( s + 1 ) * scaledTime2 + s ) + 2);
	}

	static float EaseInElastic( float t, float magnitude = 0.7 ) 
	{
		if( t == 0 || t == 1 ) 
			return t;

		float scaledTime = t / 1;
		float scaledTime1 = scaledTime - 1;

		float p = 1 - magnitude;
		float s = p / ( 2 * Math.PI ) * Math.Asin( 1 );

		return -(Math.Pow( 2, 10 * scaledTime1 ) * Math.Sin( ( scaledTime1 - s ) * ( 2 * Math.PI ) / p ));
	}

	static float EaseOutElastic( float t, float magnitude = 0.7 ) 
	{
		float p = 1 - magnitude;
		float scaledTime = t * 2;

		if( t == 0 || t == 1 ) {
			return t;
		}

		float s = p / ( 2 * Math.PI ) * Math.Asin( 1 );
		return (Math.Pow( 2, -10 * scaledTime ) * Math.Sin( ( scaledTime - s ) * ( 2 * Math.PI ) / p )) + 1;
	}

	static float EaseInOutElastic( float t, float magnitude = 0.65 ) 
	{
		float p = 1 - magnitude;

		if( t == 0 || t == 1 ) 
		{
			return t;
		}

		float scaledTime = t * 2;
		float scaledTime1 = scaledTime - 1;
		
		float s = p / ( 2 * Math.PI ) * Math.Asin( 1 );

		if( scaledTime < 1 ) 
		{
			return -0.5 * (Math.Pow( 2, 10 * scaledTime1 ) * Math.Sin( ( scaledTime1 - s ) * ( 2 * Math.PI ) / p ));
		}

		return (Math.Pow( 2, -10 * scaledTime1 ) *        Math.Sin( ( scaledTime1 - s ) * ( 2 * Math.PI ) / p ) * 0.5) + 1;
	}

	static float EaseOutBounce( float t ) 
	{

		float scaledTime = t / 1;

		if( scaledTime < ( 1 / 2.75 ) ) {

			return 7.5625 * scaledTime * scaledTime;

		} else if( scaledTime < ( 2 / 2.75 ) ) {

			float scaledTime2 = scaledTime - ( 1.5 / 2.75 );
			return ( 7.5625 * scaledTime2 * scaledTime2 ) + 0.75;

		} else if( scaledTime < ( 2.5 / 2.75 ) ) {

			scaledTime2 = scaledTime - ( 2.25 / 2.75 );
			return ( 7.5625 * scaledTime2 * scaledTime2 ) + 0.9375;

		} else {

			scaledTime2 = scaledTime - ( 2.625 / 2.75 );
			return ( 7.5625 * scaledTime2 * scaledTime2 ) + 0.984375;

		}
	}

	static float EaseInBounce( float t ) 
	{
		return 1 - EaseOutBounce( 1 - t );
	}

	static float EaseInOutBounce( float t ) 
	{
		if( t < 0.5 ) 
		{
			return EaseInBounce( t * 2 ) * 0.5;
		}
		return ( EaseOutBounce( ( t * 2 ) - 1 ) * 0.5 ) + 0.5;
	}
}