Shader "Unlit/TerrainColor"
{
	Properties
	{
		_MainTex ("Texture", 2D) = "white" {}
		_Diffuse( "Light", Color) = (0.5, 0.5, 0.5, 0.5)
	}
	SubShader
	{
		Tags 
		{
			"RenderType"="Opaque" 
		}

		Pass
		{
			CGPROGRAM
			#pragma vertex vert
			#pragma fragment frag
			
			#include "UnityCG.cginc"

			struct appdata
			{
				float4 vertex : POSITION;
				float2 texcoord : TEXCOORD0;
			};

			struct v2f
			{
				float2 texcoord : TEXCOORD0;
				float4 vertex : SV_POSITION;
			};

			sampler2D _MainTex;
			float4 _MainTex_ST;
			float4 _Diffuse;
			
			v2f vert (appdata v)
			{
				v2f o;
				o.vertex = UnityObjectToClipPos(v.vertex);
				o.texcoord = TRANSFORM_TEX(v.texcoord, _MainTex);
				return o;
			}
			
			fixed4 frag (v2f i) : SV_Target
			{
				fixed4 col = tex2D(_MainTex, i.texcoord);
				return col * float4(_Diffuse.rgb, 1.0);
			}
			ENDCG
		}
	}
}
