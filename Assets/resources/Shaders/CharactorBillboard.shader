Shader "Unlit/Character Billboard"
{
	Properties
	{
		_MainTex ("Texture", 2D) = "white" {}
		_BillboardWidth("Billboard Width", Float) = 1.0
		_BillboardHeight("Billboard Height", Float) = 1.0
	}
	SubShader
	{
		Tags 
		{ 
			"Queue" = "AlphaTest" 
			"RenderType"="Transparent"
			"IgnoreProjector" = "True"
			"DisableBatching" = "True"
		}

		Pass
		{ 
			Blend SrcAlpha OneMinusSrcAlpha

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
			float _BillboardWidth;
			float _BillboardHeight;
			
			v2f vert (appdata v)
			{
				v2f o;
				const float kQuadSize = 0.5;
				float3 BillboardOffset = float3(v.vertex.x * _BillboardWidth, (v.vertex.y + kQuadSize) * _BillboardHeight, 0.0);
				float3 ViewPos = UnityObjectToViewPos(float3(0.0, 0.0, 0.0));
				o.vertex = mul(UNITY_MATRIX_P, float4(ViewPos + BillboardOffset, 1.0));
				o.texcoord = TRANSFORM_TEX(v.texcoord, _MainTex);
				return o;
			}
			
			fixed4 frag (v2f i) : SV_Target
			{
				fixed4 col = tex2D(_MainTex, i.texcoord);
				clip(col.a - 0.05);
				return col;
			}
			ENDCG
		}
	}
}
