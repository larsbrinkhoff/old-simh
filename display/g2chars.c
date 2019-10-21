g2word chargen[128][17] = {
  /* space */
  { 0210000, 0626262, 0020000 },
  /* excla */
  { 0242000, 0010101, 0010160, 0407474, 0441315, 0171103, 0624200 },
  /* quote */
  { 0246000, 0010101, 0606000, 0540101, 0010254, 0030303, 0030303, 0440000 },
  /* number */
  { 0300000, 0201111, 0111111, 0506464, 0111111, 0111150, 0050566, 0467272, 0050505, 0056672, 0720303, 0030300 },
  /* dollar */
  { 0316000, 0010101, 0010101, 0010101, 0207636, 0151514, 0131372, 0131314, 0151576, 0560101, 0010101, 0602074, 0747414, 0016242 },
  /* percent */
  { 0274000, 0111111, 0111111, 0111111, 0113076, 0761674, 0727003, 0642472, 0747670, 0030303, 0030322 },
  /* amper */
  { 0316000, 0626222, 0171717, 0171710, 0171017, 0101110, 0121314, 0151415, 0151615, 0141314, 0131312, 0111211, 0111103, 0030304 },
  /* accr */
  { 0254000, 0010101, 0010101, 0016000, 0151536, 0030303, 0030303, 0030303, 0030000 },
  /* lparn */
  { 0244000, 0010101, 0010101, 0602015, 0157474, 0131313, 0016202 },
  /* rparn */
  { 0242000, 0010101, 0016060, 0131374, 0741515, 0150162, 0620200 },
  /* aster */
  { 0274000, 0016000, 0727212, 0070707, 0661313, 0131313, 0340760, 0201515, 0151515, 0340303, 0626202 },
  /* plus */
  { 0242000, 0016000, 0727212, 0070707, 0070674, 0741403, 0624200 },
  /* comma */
  { 0222000, 0620212, 0151511, 0106262 },
  /* minus */
  { 0224000, 0016000, 0727212, 0030303, 0440000 },
  /* period */
  { 0222000, 0014212, 0141610, 0036262 },
  /* sla */
  { 0244000, 0111111, 0111111, 0111111, 0113015, 0030303, 0036444 },
  /* c0 */
  { 0276000, 0010101, 0010101, 0010101, 0011717, 0763615, 0157474, 0131372, 0321111, 0707003, 0030364, 0440000 },
  /* c1 */
  { 0240000, 0010101, 0016020, 0111074, 0747426, 0721262, 0020000 },
  /* c2 */
  { 0252000, 0606020, 0111172, 0321313, 0341515, 0763615, 0157472, 0725222 },
  /* c3 */
  { 0272000, 0606060, 0727232, 0151515, 0151516, 0721313, 0541515, 0763607, 0071313, 0130162, 0620200 },
  /* c4 */
  { 0250000, 0624270, 0707015, 0151515, 0151515, 0141313, 0137272, 0030200 },
  /* c5 */
  { 0304000, 0010101, 0010101, 0010101, 0012076, 0763674, 0147272, 0131354, 0151576, 0360707, 0131313, 0016262, 0020000 },
  /* c6 */
  { 0314000, 0010101, 0010101, 0010101, 0011717, 0763615, 0157474, 0131372, 0321111, 0501717, 0763615, 0150303, 0030303, 0626200 },
  /* c7 */
  { 0250000, 0606060, 0727232, 0151515, 0151515, 0151515, 0740162, 0624200 },
  /* c8 */
  { 0306000, 0010160, 0171730, 0111172, 0321313, 0341515, 0763615, 0153413, 0137232, 0111130, 0171703, 0030303, 0030400 },
  /* c9 */
  { 0264000, 0201313, 0723211, 0117070, 0171776, 0361515, 0541313, 0723211, 0110303, 0036400 },
  /* colon */
  { 0250000, 0014212, 0141610, 0602012, 0141610, 0030303, 0030303, 0032200 },
  /* semcln */
  { 0252000, 0620212, 0151511, 0106040, 0121436, 0100303, 0030303, 0030322 },
  /* less */
  { 0256000, 0010101, 0010101, 0010101, 0001515, 0151576, 0131313, 0137203, 0032200 },
  /* equal */
  { 0246000, 0016040, 0727212, 0050505, 0056606, 0727212, 0030303, 0040000 },
  /* great */
  { 0246000, 0016060, 0001313, 0131372, 0151515, 0157603, 0036262, 0220000 },
  /* quest */
  { 0250000, 0016040, 0113011, 0321332, 0131314, 0151515, 0151674, 0626200 },
  /* at */
  { 0274000, 0707070, 0727232, 0747474, 0767636, 0010101, 0010101, 0010101, 0017656, 0747472, 0720303 },
  /* ca */
  { 0272000, 0111111, 0111170, 0501313, 0131313, 0137454, 0070707, 0070707, 0267212, 0030303, 0030300 },
  /* cb */
  { 0266000, 0602072, 0721313, 0341515, 0767670, 0707072, 0721313, 0341515, 0030303, 0030304 },
  /* cc */
  { 0256000, 0010162, 0621515, 0763617, 0177070, 0111172, 0321313, 0130303, 0644400 },
  /* cd */
  { 0242000, 0707070, 0727213, 0137474, 0151576, 0766262, 0620200 },
  /* ce */
  { 0264000, 0626222, 0767636, 0707070, 0727232, 0050505, 0447656, 0030303, 0030303, 0624200 },
  /* cf */
  { 0250000, 0707070, 0727232, 0050505, 0447656, 0030303, 0030303, 0624200 },
  /* cg */
  { 0274000, 0010101, 0010100, 0721274, 0544015, 0157636, 0171770, 0701111, 0723213, 0131303, 0036444 },
  /* ch */
  { 0252000, 0606060, 0747474, 0140160, 0207272, 0126020, 0747474, 0140122 },
  /* ci */
  { 0244000, 0427212, 0072670, 0705026, 0721203, 0030303, 0036444 },
  /* cj */
  { 0242000, 0201313, 0721111, 0707030, 0267212, 0030364, 0642400 },
  /* ck */
  { 0340000, 0606060, 0747474, 0140101, 0010101, 0010101, 0010140, 0151515, 0151515, 0151536, 0030303, 0036222, 0171717, 0171717, 0100303, 0030303, 0030322 },
  /* cl */
  { 0224000, 0606060, 0747474, 0727232, 0420000 },
  /* cm */
  { 0252000, 0707070, 0131313, 0131334, 0111111, 0111130, 0747474, 0140122 },
  /* cn */
  { 0302000, 0010101, 0010101, 0010101, 0012074, 0747417, 0171717, 0171717, 0171717, 0307474, 0741401, 0626262 },
  /* co */
  { 0310000, 0010101, 0010101, 0010160, 0151413, 0141111, 0171776, 0361515, 0747413, 0137232, 0111170, 0700303, 0036444 },
  /* cp */
  { 0250000, 0707070, 0727213, 0133415, 0157676, 0030303, 0030303, 0624200 },
  /* cq */
  { 0266000, 0221717, 0707011, 0117232, 0131374, 0741515, 0763601, 0010122, 0131313, 0130102 },
  /* cr */
  { 0300000, 0707070, 0727213, 0133415, 0157676, 0030303, 0030303, 0621717, 0171717, 0100303, 0030303, 0032200 },
  /* cs */
  { 0264000, 0201313, 0723211, 0113017, 0177636, 0171730, 0111172, 0321313, 0130303, 0644400 },
  /* ct */
  { 0264000, 0606060, 0727252, 0050505, 0050505, 0642470, 0707003, 0030303, 0030303, 0036400 },
  /* cu */
  { 0252000, 0606060, 0747434, 0131372, 0721217, 0106060, 0207474, 0540322 },
  /* cv */
  { 0264000, 0606060, 0131313, 0131374, 0540101, 0010101, 0604015, 0151515, 0157454, 0626200 },
  /* cw */
  { 0310000, 0606060, 0131374, 0743411, 0111170, 0701062, 0021515, 0747434, 0171717, 0707010, 0030303, 0030303, 0030364 },
  /* cx */
  { 0314000, 0111111, 0111110, 0010101, 0010100, 0151515, 0151514, 0070707, 0070700, 0131313, 0131313, 0131313, 0131334, 0010200 },
  /* cy */
  { 0264000, 0606060, 0131313, 0131354, 0010101, 0010140, 0151515, 0151554, 0741401, 0624200 },
  /* cz */
  { 0252000, 0606060, 0727232, 0151515, 0151515, 0151515, 0153472, 0725222 },
  /* lbk */
  { 0236000, 0010101, 0010101, 0010160, 0567474, 0747262 },
  /* bsla */
  { 0242000, 0606060, 0131313, 0131313, 0131313, 0131334, 0010200 },
  /* rbk */
  { 0232000, 0010160, 0602052, 0747474, 0766262, 0620000 },
  /* cmflx */
  { 0236000, 0010160, 0600011, 0321352, 0030303, 0036444 },
  /* under */
  { 0214000, 0047272, 0721200 },
  /* accl */
  { 0240000, 0010101, 0606000, 0131332, 0030303, 0030303, 0640000 },
  /* la */
  { 0300000, 0016020, 0111112, 0521313, 0127474, 0600017, 0171656, 0151516, 0341313, 0125211, 0111203, 0030200 },
  /* lb */
  { 0272000, 0606040, 0137474, 0546040, 0111112, 0521313, 0127415, 0151656, 0171716, 0030362, 0622200 },
  /* lc */
  { 0256000, 0010162, 0621515, 0165617, 0171670, 0111112, 0521313, 0131203, 0034400 },
  /* ld */
  { 0306000, 0010101, 0010101, 0010101, 0012074, 0747414, 0604017, 0171656, 0151516, 0741313, 0125211, 0111203, 0030200 },
  /* le */
  { 0270000, 0016262, 0021536, 0561717, 0167011, 0111252, 0131312, 0347676, 0160303, 0030362, 0620000 },
  /* lf */
  { 0252000, 0627070, 0301111, 0720505, 0050505, 0467232, 0030303, 0030324 },
  /* lg */
  { 0270000, 0031313, 0125211, 0111170, 0301717, 0165615, 0151654, 0131312, 0521111, 0120303, 0030000 },
  /* lh */
  { 0252000, 0016060, 0407474, 0741460, 0201111, 0125213, 0131274, 0340122 },
  /* li */
  { 0252000, 0010101, 0016020, 0323436, 0300505, 0245274, 0344672, 0326202 },
  /* lj */
  { 0262000, 0021313, 0321111, 0705076, 0012032, 0343630, 0030303, 0030303, 0030303, 0040000 },
  /* lk */
  { 0266000, 0016060, 0007474, 0546011, 0111111, 0117205, 0050566, 0131313, 0131313, 0320102 },
  /* ll */
  { 0244000, 0427232, 0467070, 0503603, 0030303, 0030303, 0030324 },
  /* lm */
  { 0264000, 0707044, 0111032, 0131311, 0105213, 0147434, 0070707, 0070700, 0743401, 0624200 },
  /* ln */
  { 0244000, 0016020, 0747460, 0201111, 0323213, 0131274, 0340122 },
  /* lo */
  { 0256000, 0010050, 0111112, 0521313, 0125415, 0151656, 0171716, 0030362, 0622200 },
  /* lp */
  { 0264000, 0032470, 0705044, 0111112, 0321313, 0325415, 0153636, 0171716, 0030362, 0622200 },
  /* lq */
  { 0304000, 0010101, 0010101, 0014274, 0743432, 0070760, 0201717, 0165615, 0151654, 0131312, 0521111, 0120303, 0020000 },
  /* lr */
  { 0242000, 0427030, 0070613, 0121112, 0321312, 0030303, 0032400 },
  /* ls */
  { 0254000, 0010013, 0137211, 0125017, 0161756, 0301172, 0131403, 0030303, 0440000 },
  /* lt */
  { 0246000, 0016060, 0727207, 0070726, 0747434, 0131432, 0111203, 0420000 },
  /* lu */
  { 0252000, 0016060, 0745413, 0135211, 0117070, 0030364, 0641717, 0030342 },
  /* lv */
  { 0252000, 0016060, 0131313, 0137414, 0111111, 0117010, 0030303, 0036404 },
  /* lw */
  { 0260000, 0606000, 0131313, 0743411, 0111150, 0131313, 0541111, 0117030, 0036464 },
  /* lx */
  { 0276000, 0016060, 0131313, 0131313, 0131313, 0130707, 0606015, 0151515, 0151515, 0151514, 0016262, 0620000 },
  /* ly */
  { 0270000, 0016060, 0131313, 0137414, 0010101, 0016000, 0151515, 0151574, 0345601, 0016262, 0220000 },
  /* lz */
  { 0242000, 0010160, 0407232, 0151515, 0151515, 0153472, 0726200 },
  /* lbr */
  { 0254000, 0010101, 0010101, 0010160, 0153674, 0151613, 0127413, 0133201, 0420000 },
  /* or */
  { 0234000, 0010101, 0010160, 0407474, 0741401, 0624200 },
  /* rbr */
  { 0250000, 0010160, 0602013, 0327413, 0121516, 0741515, 0360162, 0624200 },
  /* tild */
  { 0270000, 0010101, 0010101, 0010160, 0153417, 0171716, 0151514, 0030303, 0030303, 0030303, 0420000 },
  /* null */
  { 0200000, 0000000 },
};
