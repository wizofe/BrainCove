#include "ColorMapping.h"

ColorMapping *ColorMapping::s_instance = 0;

ColorMapping::ColorMapping() : lut(0)
{
}

ColorMapping::~ColorMapping()
{
	if(lut)
		lut->Delete();
}

vtkLookupTable *ColorMapping::BlueToYellowColormap()
{
	if(!lut)
	{
		lut = vtkLookupTable::New();
		lut->SetRange(-1,1);       
		lut->SetNumberOfColors(256);
		lut->SetTableValue(	0	, 	0.027	,	0.027	,	0.996	,1.0);
		lut->SetTableValue(	1	, 	0.090	,	0.090	,	0.988	,1.0);
		lut->SetTableValue(	2	, 	0.118	,	0.118	,	0.980	,1.0);
		lut->SetTableValue(	3	, 	0.141	,	0.141	,	0.973	,1.0);
		lut->SetTableValue(	4	, 	0.157	,	0.157	,	0.969	,1.0);
		lut->SetTableValue(	5	, 	0.173	,	0.173	,	0.961	,1.0);
		lut->SetTableValue(	6	, 	0.184	,	0.184	,	0.953	,1.0);
		lut->SetTableValue(	7	, 	0.196	,	0.196	,	0.949	,1.0);
		lut->SetTableValue(	8	, 	0.204	,	0.204	,	0.941	,1.0);
		lut->SetTableValue(	9	, 	0.216	,	0.216	,	0.937	,1.0);
		lut->SetTableValue(	10	, 	0.224	,	0.224	,	0.933	,1.0);
		lut->SetTableValue(	11	, 	0.231	,	0.231	,	0.925	,1.0);
		lut->SetTableValue(	12	, 	0.239	,	0.239	,	0.922	,1.0);
		lut->SetTableValue(	13	, 	0.247	,	0.247	,	0.918	,1.0);
		lut->SetTableValue(	14	, 	0.255	,	0.255	,	0.914	,1.0);
		lut->SetTableValue(	15	, 	0.259	,	0.259	,	0.906	,1.0);
		lut->SetTableValue(	16	, 	0.267	,	0.267	,	0.902	,1.0);
		lut->SetTableValue(	17	, 	0.271	,	0.271	,	0.898	,1.0);
		lut->SetTableValue(	18	, 	0.278	,	0.278	,	0.894	,1.0);
		lut->SetTableValue(	19	, 	0.282	,	0.282	,	0.890	,1.0);
		lut->SetTableValue(	20	, 	0.290	,	0.290	,	0.886	,1.0);
		lut->SetTableValue(	21	, 	0.294	,	0.294	,	0.882	,1.0);
		lut->SetTableValue(	22	, 	0.298	,	0.298	,	0.882	,1.0);
		lut->SetTableValue(	23	, 	0.306	,	0.306	,	0.878	,1.0);
		lut->SetTableValue(	24	, 	0.310	,	0.310	,	0.875	,1.0);
		lut->SetTableValue(	25	, 	0.314	,	0.314	,	0.871	,1.0);
		lut->SetTableValue(	26	, 	0.318	,	0.318	,	0.867	,1.0);
		lut->SetTableValue(	27	, 	0.322	,	0.322	,	0.867	,1.0);
		lut->SetTableValue(	28	, 	0.329	,	0.329	,	0.863	,1.0);
		lut->SetTableValue(	29	, 	0.333	,	0.333	,	0.859	,1.0);
		lut->SetTableValue(	30	, 	0.337	,	0.337	,	0.855	,1.0);
		lut->SetTableValue(	31	, 	0.341	,	0.341	,	0.855	,1.0);
		lut->SetTableValue(	32	, 	0.345	,	0.345	,	0.851	,1.0);
		lut->SetTableValue(	33	, 	0.349	,	0.349	,	0.847	,1.0);
		lut->SetTableValue(	34	, 	0.353	,	0.353	,	0.847	,1.0);
		lut->SetTableValue(	35	, 	0.357	,	0.357	,	0.843	,1.0);
		lut->SetTableValue(	36	, 	0.361	,	0.361	,	0.839	,1.0);
		lut->SetTableValue(	37	, 	0.365	,	0.365	,	0.839	,1.0);
		lut->SetTableValue(	38	, 	0.369	,	0.369	,	0.835	,1.0);
		lut->SetTableValue(	39	, 	0.373	,	0.373	,	0.835	,1.0);
		lut->SetTableValue(	40	, 	0.376	,	0.376	,	0.831	,1.0);
		lut->SetTableValue(	41	, 	0.380	,	0.380	,	0.831	,1.0);
		lut->SetTableValue(	42	, 	0.384	,	0.384	,	0.827	,1.0);
		lut->SetTableValue(	43	, 	0.384	,	0.384	,	0.824	,1.0);
		lut->SetTableValue(	44	, 	0.388	,	0.388	,	0.824	,1.0);
		lut->SetTableValue(	45	, 	0.392	,	0.392	,	0.820	,1.0);
		lut->SetTableValue(	46	, 	0.396	,	0.396	,	0.820	,1.0);
		lut->SetTableValue(	47	, 	0.400	,	0.400	,	0.816	,1.0);
		lut->SetTableValue(	48	, 	0.404	,	0.404	,	0.816	,1.0);
		lut->SetTableValue(	49	, 	0.408	,	0.408	,	0.816	,1.0);
		lut->SetTableValue(	50	, 	0.412	,	0.412	,	0.812	,1.0);
		lut->SetTableValue(	51	, 	0.412	,	0.412	,	0.812	,1.0);
		lut->SetTableValue(	52	, 	0.416	,	0.416	,	0.808	,1.0);
		lut->SetTableValue(	53	, 	0.420	,	0.420	,	0.808	,1.0);
		lut->SetTableValue(	54	, 	0.424	,	0.424	,	0.804	,1.0);
		lut->SetTableValue(	55	, 	0.427	,	0.427	,	0.804	,1.0);
		lut->SetTableValue(	56	, 	0.431	,	0.431	,	0.800	,1.0);
		lut->SetTableValue(	57	, 	0.431	,	0.431	,	0.800	,1.0);
		lut->SetTableValue(	58	, 	0.435	,	0.435	,	0.800	,1.0);
		lut->SetTableValue(	59	, 	0.439	,	0.439	,	0.796	,1.0);
		lut->SetTableValue(	60	, 	0.443	,	0.443	,	0.796	,1.0);
		lut->SetTableValue(	61	, 	0.447	,	0.447	,	0.792	,1.0);
		lut->SetTableValue(	62	, 	0.447	,	0.447	,	0.792	,1.0);
		lut->SetTableValue(	63	, 	0.451	,	0.451	,	0.792	,1.0);
		lut->SetTableValue(	64	, 	0.455	,	0.455	,	0.788	,1.0);
		lut->SetTableValue(	65	, 	0.459	,	0.459	,	0.788	,1.0);
		lut->SetTableValue(	66	, 	0.463	,	0.463	,	0.784	,1.0);
		lut->SetTableValue(	67	, 	0.463	,	0.463	,	0.784	,1.0);
		lut->SetTableValue(	68	, 	0.467	,	0.467	,	0.784	,1.0);
		lut->SetTableValue(	69	, 	0.471	,	0.471	,	0.780	,1.0);
		lut->SetTableValue(	70	, 	0.475	,	0.475	,	0.780	,1.0);
		lut->SetTableValue(	71	, 	0.475	,	0.475	,	0.780	,1.0);
		lut->SetTableValue(	72	, 	0.478	,	0.478	,	0.776	,1.0);
		lut->SetTableValue(	73	, 	0.482	,	0.482	,	0.776	,1.0);
		lut->SetTableValue(	74	, 	0.486	,	0.486	,	0.776	,1.0);
		lut->SetTableValue(	75	, 	0.486	,	0.486	,	0.773	,1.0);
		lut->SetTableValue(	76	, 	0.490	,	0.490	,	0.773	,1.0);
		lut->SetTableValue(	77	, 	0.494	,	0.494	,	0.773	,1.0);
		lut->SetTableValue(	78	, 	0.498	,	0.498	,	0.769	,1.0);
		lut->SetTableValue(	79	, 	0.502	,	0.502	,	0.769	,1.0);
		lut->SetTableValue(	80	, 	0.502	,	0.502	,	0.765	,1.0);
		lut->SetTableValue(	81	, 	0.506	,	0.506	,	0.765	,1.0);
		lut->SetTableValue(	82	, 	0.510	,	0.510	,	0.765	,1.0);
		lut->SetTableValue(	83	, 	0.510	,	0.510	,	0.761	,1.0);
		lut->SetTableValue(	84	, 	0.514	,	0.514	,	0.761	,1.0);
		lut->SetTableValue(	85	, 	0.518	,	0.518	,	0.761	,1.0);
		lut->SetTableValue(	86	, 	0.522	,	0.522	,	0.757	,1.0);
		lut->SetTableValue(	87	, 	0.522	,	0.522	,	0.757	,1.0);
		lut->SetTableValue(	88	, 	0.525	,	0.525	,	0.757	,1.0);
		lut->SetTableValue(	89	, 	0.529	,	0.529	,	0.753	,1.0);
		lut->SetTableValue(	90	, 	0.533	,	0.533	,	0.753	,1.0);
		lut->SetTableValue(	91	, 	0.533	,	0.533	,	0.753	,1.0);
		lut->SetTableValue(	92	, 	0.537	,	0.537	,	0.749	,1.0);
		lut->SetTableValue(	93	, 	0.541	,	0.541	,	0.749	,1.0);
		lut->SetTableValue(	94	, 	0.545	,	0.545	,	0.749	,1.0);
		lut->SetTableValue(	95	, 	0.545	,	0.545	,	0.745	,1.0);
		lut->SetTableValue(	96	, 	0.549	,	0.549	,	0.745	,1.0);
		lut->SetTableValue(	97	, 	0.553	,	0.553	,	0.745	,1.0);
		lut->SetTableValue(	98	, 	0.557	,	0.557	,	0.741	,1.0);
		lut->SetTableValue(	99	, 	0.557	,	0.557	,	0.741	,1.0);
		lut->SetTableValue(	100	, 	0.561	,	0.561	,	0.741	,1.0);
		lut->SetTableValue(	101	, 	0.565	,	0.565	,	0.737	,1.0);
		lut->SetTableValue(	102	, 	0.565	,	0.565	,	0.737	,1.0);
		lut->SetTableValue(	103	, 	0.569	,	0.569	,	0.737	,1.0);
		lut->SetTableValue(	104	, 	0.573	,	0.573	,	0.733	,1.0);
		lut->SetTableValue(	105	, 	0.576	,	0.576	,	0.733	,1.0);
		lut->SetTableValue(	106	, 	0.576	,	0.576	,	0.733	,1.0);
		lut->SetTableValue(	107	, 	0.580	,	0.580	,	0.729	,1.0);
		lut->SetTableValue(	108	, 	0.584	,	0.584	,	0.729	,1.0);
		lut->SetTableValue(	109	, 	0.584	,	0.584	,	0.729	,1.0);
		lut->SetTableValue(	110	, 	0.588	,	0.588	,	0.725	,1.0);
		lut->SetTableValue(	111	, 	0.592	,	0.592	,	0.725	,1.0);
		lut->SetTableValue(	112	, 	0.596	,	0.596	,	0.725	,1.0);
		lut->SetTableValue(	113	, 	0.596	,	0.596	,	0.722	,1.0);
		lut->SetTableValue(	114	, 	0.600	,	0.600	,	0.722	,1.0);
		lut->SetTableValue(	115	, 	0.604	,	0.604	,	0.722	,1.0);
		lut->SetTableValue(	116	, 	0.604	,	0.604	,	0.718	,1.0);
		lut->SetTableValue(	117	, 	0.608	,	0.608	,	0.718	,1.0);
		lut->SetTableValue(	118	, 	0.612	,	0.612	,	0.714	,1.0);
		lut->SetTableValue(	119	, 	0.616	,	0.616	,	0.714	,1.0);
		lut->SetTableValue(	120	, 	0.616	,	0.616	,	0.714	,1.0);
		lut->SetTableValue(	121	, 	0.620	,	0.620	,	0.710	,1.0);
		lut->SetTableValue(	122	, 	0.624	,	0.624	,	0.710	,1.0);
		lut->SetTableValue(	123	, 	0.624	,	0.624	,	0.710	,1.0);
		lut->SetTableValue(	124	, 	0.627	,	0.627	,	0.706	,1.0);
		lut->SetTableValue(	125	, 	0.631	,	0.631	,	0.706	,1.0);
		lut->SetTableValue(	126	, 	0.635	,	0.635	,	0.706	,1.0);
		lut->SetTableValue(	127	, 	0.635	,	0.635	,	0.702	,1.0);
		lut->SetTableValue(	128	, 	0.639	,	0.639	,	0.702	,1.0);
		lut->SetTableValue(	129	, 	0.643	,	0.643	,	0.698	,1.0);
		lut->SetTableValue(	130	, 	0.643	,	0.643	,	0.698	,1.0);
		lut->SetTableValue(	131	, 	0.647	,	0.647	,	0.698	,1.0);
		lut->SetTableValue(	132	, 	0.651	,	0.651	,	0.694	,1.0);
		lut->SetTableValue(	133	, 	0.655	,	0.655	,	0.694	,1.0);
		lut->SetTableValue(	134	, 	0.655	,	0.655	,	0.690	,1.0);
		lut->SetTableValue(	135	, 	0.659	,	0.659	,	0.690	,1.0);
		lut->SetTableValue(	136	, 	0.663	,	0.663	,	0.690	,1.0);
		lut->SetTableValue(	137	, 	0.663	,	0.663	,	0.686	,1.0);
		lut->SetTableValue(	138	, 	0.667	,	0.667	,	0.686	,1.0);
		lut->SetTableValue(	139	, 	0.671	,	0.671	,	0.682	,1.0);
		lut->SetTableValue(	140	, 	0.675	,	0.675	,	0.682	,1.0);
		lut->SetTableValue(	141	, 	0.675	,	0.675	,	0.678	,1.0);
		lut->SetTableValue(	142	, 	0.678	,	0.678	,	0.678	,1.0);
		lut->SetTableValue(	143	, 	0.682	,	0.682	,	0.678	,1.0);
		lut->SetTableValue(	144	, 	0.682	,	0.682	,	0.675	,1.0);
		lut->SetTableValue(	145	, 	0.686	,	0.686	,	0.675	,1.0);
		lut->SetTableValue(	146	, 	0.690	,	0.690	,	0.671	,1.0);
		lut->SetTableValue(	147	, 	0.694	,	0.694	,	0.671	,1.0);
		lut->SetTableValue(	148	, 	0.694	,	0.694	,	0.667	,1.0);
		lut->SetTableValue(	149	, 	0.698	,	0.698	,	0.667	,1.0);
		lut->SetTableValue(	150	, 	0.702	,	0.702	,	0.663	,1.0);
		lut->SetTableValue(	151	, 	0.702	,	0.702	,	0.663	,1.0);
		lut->SetTableValue(	152	, 	0.706	,	0.706	,	0.659	,1.0);
		lut->SetTableValue(	153	, 	0.710	,	0.710	,	0.659	,1.0);
		lut->SetTableValue(	154	, 	0.710	,	0.710	,	0.655	,1.0);
		lut->SetTableValue(	155	, 	0.714	,	0.714	,	0.655	,1.0);
		lut->SetTableValue(	156	, 	0.718	,	0.718	,	0.651	,1.0);
		lut->SetTableValue(	157	, 	0.722	,	0.722	,	0.651	,1.0);
		lut->SetTableValue(	158	, 	0.722	,	0.722	,	0.647	,1.0);
		lut->SetTableValue(	159	, 	0.725	,	0.725	,	0.647	,1.0);
		lut->SetTableValue(	160	, 	0.729	,	0.729	,	0.643	,1.0);
		lut->SetTableValue(	161	, 	0.729	,	0.729	,	0.643	,1.0);
		lut->SetTableValue(	162	, 	0.733	,	0.733	,	0.639	,1.0);
		lut->SetTableValue(	163	, 	0.737	,	0.737	,	0.639	,1.0);
		lut->SetTableValue(	164	, 	0.741	,	0.741	,	0.635	,1.0);
		lut->SetTableValue(	165	, 	0.741	,	0.741	,	0.635	,1.0);
		lut->SetTableValue(	166	, 	0.745	,	0.745	,	0.631	,1.0);
		lut->SetTableValue(	167	, 	0.749	,	0.749	,	0.631	,1.0);
		lut->SetTableValue(	168	, 	0.749	,	0.749	,	0.627	,1.0);
		lut->SetTableValue(	169	, 	0.753	,	0.753	,	0.624	,1.0);
		lut->SetTableValue(	170	, 	0.757	,	0.757	,	0.624	,1.0);
		lut->SetTableValue(	171	, 	0.761	,	0.761	,	0.620	,1.0);
		lut->SetTableValue(	172	, 	0.761	,	0.761	,	0.620	,1.0);
		lut->SetTableValue(	173	, 	0.765	,	0.765	,	0.616	,1.0);
		lut->SetTableValue(	174	, 	0.769	,	0.769	,	0.616	,1.0);
		lut->SetTableValue(	175	, 	0.769	,	0.769	,	0.612	,1.0);
		lut->SetTableValue(	176	, 	0.773	,	0.773	,	0.608	,1.0);
		lut->SetTableValue(	177	, 	0.776	,	0.776	,	0.608	,1.0);
		lut->SetTableValue(	178	, 	0.780	,	0.780	,	0.604	,1.0);
		lut->SetTableValue(	179	, 	0.780	,	0.780	,	0.600	,1.0);
		lut->SetTableValue(	180	, 	0.784	,	0.784	,	0.600	,1.0);
		lut->SetTableValue(	181	, 	0.788	,	0.788	,	0.596	,1.0);
		lut->SetTableValue(	182	, 	0.788	,	0.788	,	0.592	,1.0);
		lut->SetTableValue(	183	, 	0.792	,	0.792	,	0.592	,1.0);
		lut->SetTableValue(	184	, 	0.796	,	0.796	,	0.588	,1.0);
		lut->SetTableValue(	185	, 	0.800	,	0.800	,	0.584	,1.0);
		lut->SetTableValue(	186	, 	0.800	,	0.800	,	0.584	,1.0);
		lut->SetTableValue(	187	, 	0.804	,	0.804	,	0.580	,1.0);
		lut->SetTableValue(	188	, 	0.808	,	0.808	,	0.576	,1.0);
		lut->SetTableValue(	189	, 	0.808	,	0.808	,	0.573	,1.0);
		lut->SetTableValue(	190	, 	0.812	,	0.812	,	0.573	,1.0);
		lut->SetTableValue(	191	, 	0.816	,	0.816	,	0.569	,1.0);
		lut->SetTableValue(	192	, 	0.820	,	0.820	,	0.565	,1.0);
		lut->SetTableValue(	193	, 	0.820	,	0.820	,	0.561	,1.0);
		lut->SetTableValue(	194	, 	0.824	,	0.824	,	0.561	,1.0);
		lut->SetTableValue(	195	, 	0.827	,	0.827	,	0.557	,1.0);
		lut->SetTableValue(	196	, 	0.827	,	0.827	,	0.553	,1.0);
		lut->SetTableValue(	197	, 	0.831	,	0.831	,	0.549	,1.0);
		lut->SetTableValue(	198	, 	0.835	,	0.835	,	0.545	,1.0);
		lut->SetTableValue(	199	, 	0.839	,	0.839	,	0.541	,1.0);
		lut->SetTableValue(	200	, 	0.839	,	0.839	,	0.541	,1.0);
		lut->SetTableValue(	201	, 	0.843	,	0.843	,	0.537	,1.0);
		lut->SetTableValue(	202	, 	0.847	,	0.847	,	0.533	,1.0);
		lut->SetTableValue(	203	, 	0.847	,	0.847	,	0.529	,1.0);
		lut->SetTableValue(	204	, 	0.851	,	0.851	,	0.525	,1.0);
		lut->SetTableValue(	205	, 	0.855	,	0.855	,	0.522	,1.0);
		lut->SetTableValue(	206	, 	0.859	,	0.859	,	0.518	,1.0);
		lut->SetTableValue(	207	, 	0.859	,	0.859	,	0.514	,1.0);
		lut->SetTableValue(	208	, 	0.863	,	0.863	,	0.510	,1.0);
		lut->SetTableValue(	209	, 	0.867	,	0.867	,	0.506	,1.0);
		lut->SetTableValue(	210	, 	0.867	,	0.867	,	0.502	,1.0);
		lut->SetTableValue(	211	, 	0.871	,	0.871	,	0.498	,1.0);
		lut->SetTableValue(	212	, 	0.875	,	0.875	,	0.494	,1.0);
		lut->SetTableValue(	213	, 	0.878	,	0.878	,	0.490	,1.0);
		lut->SetTableValue(	214	, 	0.878	,	0.878	,	0.486	,1.0);
		lut->SetTableValue(	215	, 	0.882	,	0.882	,	0.482	,1.0);
		lut->SetTableValue(	216	, 	0.886	,	0.886	,	0.478	,1.0);
		lut->SetTableValue(	217	, 	0.886	,	0.886	,	0.475	,1.0);
		lut->SetTableValue(	218	, 	0.890	,	0.890	,	0.467	,1.0);
		lut->SetTableValue(	219	, 	0.894	,	0.894	,	0.463	,1.0);
		lut->SetTableValue(	220	, 	0.898	,	0.898	,	0.459	,1.0);
		lut->SetTableValue(	221	, 	0.898	,	0.898	,	0.455	,1.0);
		lut->SetTableValue(	222	, 	0.902	,	0.902	,	0.447	,1.0);
		lut->SetTableValue(	223	, 	0.906	,	0.906	,	0.443	,1.0);
		lut->SetTableValue(	224	, 	0.910	,	0.910	,	0.439	,1.0);
		lut->SetTableValue(	225	, 	0.910	,	0.910	,	0.431	,1.0);
		lut->SetTableValue(	226	, 	0.914	,	0.914	,	0.427	,1.0);
		lut->SetTableValue(	227	, 	0.918	,	0.918	,	0.420	,1.0);
		lut->SetTableValue(	228	, 	0.918	,	0.918	,	0.416	,1.0);
		lut->SetTableValue(	229	, 	0.922	,	0.922	,	0.408	,1.0);
		lut->SetTableValue(	230	, 	0.925	,	0.925	,	0.404	,1.0);
		lut->SetTableValue(	231	, 	0.929	,	0.929	,	0.396	,1.0);
		lut->SetTableValue(	232	, 	0.929	,	0.929	,	0.392	,1.0);
		lut->SetTableValue(	233	, 	0.933	,	0.933	,	0.384	,1.0);
		lut->SetTableValue(	234	, 	0.937	,	0.937	,	0.376	,1.0);
		lut->SetTableValue(	235	, 	0.937	,	0.937	,	0.369	,1.0);
		lut->SetTableValue(	236	, 	0.941	,	0.941	,	0.361	,1.0);
		lut->SetTableValue(	237	, 	0.945	,	0.945	,	0.357	,1.0);
		lut->SetTableValue(	238	, 	0.949	,	0.949	,	0.349	,1.0);
		lut->SetTableValue(	239	, 	0.949	,	0.949	,	0.337	,1.0);
		lut->SetTableValue(	240	, 	0.953	,	0.953	,	0.329	,1.0);
		lut->SetTableValue(	241	, 	0.957	,	0.957	,	0.322	,1.0);
		lut->SetTableValue(	242	, 	0.961	,	0.961	,	0.314	,1.0);
		lut->SetTableValue(	243	, 	0.961	,	0.961	,	0.302	,1.0);
		lut->SetTableValue(	244	, 	0.965	,	0.965	,	0.290	,1.0);
		lut->SetTableValue(	245	, 	0.969	,	0.969	,	0.282	,1.0);
		lut->SetTableValue(	246	, 	0.969	,	0.969	,	0.271	,1.0);
		lut->SetTableValue(	247	, 	0.973	,	0.973	,	0.255	,1.0);
		lut->SetTableValue(	248	, 	0.976	,	0.976	,	0.243	,1.0);
		lut->SetTableValue(	249	, 	0.980	,	0.980	,	0.227	,1.0);
		lut->SetTableValue(	250	, 	0.980	,	0.980	,	0.212	,1.0);
		lut->SetTableValue(	251	, 	0.984	,	0.984	,	0.192	,1.0);
		lut->SetTableValue(	252	, 	0.988	,	0.988	,	0.173	,1.0);
		lut->SetTableValue(	253	, 	0.992	,	0.992	,	0.145	,1.0);
		lut->SetTableValue(	254	, 	0.992	,	0.992	,	0.110	,1.0);
		lut->SetTableValue(	255	, 	0.996	,	0.996	,	0.051	,1.0);
		lut->Build();			
	}
	return lut;
}

void ColorMapping::makeAlpha(vtkLookupTable *table)
{
        int nval = table->GetNumberOfTableValues() / 2;
        for(int i = 0;i<table->GetNumberOfTableValues();i++)
        {
                double rgba[4];
                table->GetTableValue(i,rgba);
				float normalized = ((float)(i-nval))/128.0f;
				float d = normalized*3.0f;
				d = 1.0 / (1 + exp(-1*d));
				//d-=0.5;
				//d*=2;
				if(d<-1.0f) d = -1.0f;
				if(d>1.0f) d = 1.0f;
                rgba[3] = d;  //normalized*normalized*normalized;
                table->SetTableValue(i,rgba);
        }
}

vtkLookupTable *ColorMapping::ImportFromDevideTF(std::string filename, int size, vtkColorTransferFunction *ext_ctf, vtkPiecewiseFunction *ext_otf)
{
	vtkColorTransferFunction *ctf = vtkColorTransferFunction::New();
	vtkPiecewiseFunction *otf = vtkPiecewiseFunction::New();
	
	char buff[16*1024];
	std::vector<double> numbers;

	std::ifstream stream(filename.c_str(),ios::in);
	double num = -1;

	while(!stream.eof())
	{
		stream.getline(buff,16*1024);
		if(buff[0] == '#')
			continue;
		char *tokens = strtok(buff,"()[], ");
		while(tokens)
		{
			num = atof(tokens);
			numbers.push_back(num);
			tokens = strtok(NULL,"()[], ");
		}
	}
	int numtuples = numbers.size() / 5;
	for(int i = 0;i < numtuples; i++)
	{
		int idx = i*5;
		double x = numbers.at(idx+4);
		ctf->AddRGBPoint(numbers.at(idx),numbers.at(idx+1)/256.0,numbers.at(idx+2)/256.0,numbers.at(idx+3)/256.0);
		otf->AddPoint(numbers.at(idx),numbers.at(idx+4));
	}

	vtkLookupTable *lut = vtkLookupTable::New();
	lut->SetNumberOfTableValues(size);
	double range[2];
	ctf->GetRange(range);
	lut->SetRange(range);
	for(int i = 0; i < size; i++)
	{
		double ctfIdx = range[0] + ((double)i/(double)size)*(range[1]-range[0]);
		lut->SetTableValue(i,ctf->GetRedValue(ctfIdx),ctf->GetGreenValue(ctfIdx),ctf->GetBlueValue(ctfIdx),otf->GetValue(ctfIdx));
	}
	lut->Build();

	if(ext_ctf)
		ext_ctf->DeepCopy(ctf);
	if(ext_otf)
		ext_otf->DeepCopy(otf);

	ctf->Delete();
	otf->Delete();
	return lut;
}