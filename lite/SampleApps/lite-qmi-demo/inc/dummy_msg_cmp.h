#ifndef _DUMMY_MSG_CMP_H_
#define _DUMMY_MSG_CMP_H_
#pragma once


/****
MACRO to run unpack, dump and compare function.
****/
#define CMP_PTR_TYPE   1
#define CMP_DATA_TYPE  0

#define SWI_UINT256_BIT_OFFSET_VALUE(x) \
    1 << SWI_UINT256_BIT_OFFSET(x)

#define SWI_UINT256_SET_BIT_N(y,x) \
    (SWI_UINT256_WORD_OFFSET(x) == y ? \
    SWI_UINT256_BIT_OFFSET_VALUE(x) : \
    0) 

#define SWI_UINT256_BIT_VALUE(FN, ...) \
[15] = FN(SWI_UINT256_SET_BIT_N,15,__VA_ARGS__) , \
[14] = FN(SWI_UINT256_SET_BIT_N,14,__VA_ARGS__) , \
[13] = FN(SWI_UINT256_SET_BIT_N,13,__VA_ARGS__) , \
[12] = FN(SWI_UINT256_SET_BIT_N,12,__VA_ARGS__) , \
[11] = FN(SWI_UINT256_SET_BIT_N,11,__VA_ARGS__) , \
[10] = FN(SWI_UINT256_SET_BIT_N,10,__VA_ARGS__) , \
[9] = FN(SWI_UINT256_SET_BIT_N,9,__VA_ARGS__) , \
[8] = FN(SWI_UINT256_SET_BIT_N,8,__VA_ARGS__) , \
[7] = FN(SWI_UINT256_SET_BIT_N,7,__VA_ARGS__) , \
[6] = FN(SWI_UINT256_SET_BIT_N,6,__VA_ARGS__) , \
[5] = FN(SWI_UINT256_SET_BIT_N,5,__VA_ARGS__) , \
[4] = FN(SWI_UINT256_SET_BIT_N,4,__VA_ARGS__) , \
[3] = FN(SWI_UINT256_SET_BIT_N,3,__VA_ARGS__) , \
[2] = FN(SWI_UINT256_SET_BIT_N,2,__VA_ARGS__) , \
[1] = FN(SWI_UINT256_SET_BIT_N,1,__VA_ARGS__) , \
[0] = FN(SWI_UINT256_SET_BIT_N,0,__VA_ARGS__) 

#define SET_1_BITS(FN, X, Y, ...) FN(X, Y) 
#define SET_2_BITS(FN, X, Y, ...) FN(X, Y) | SET_1_BITS(FN, X , __VA_ARGS__)
#define SET_3_BITS(FN, X, Y, ...) FN(X, Y) | SET_2_BITS(FN, X , __VA_ARGS__)
#define SET_4_BITS(FN, X, Y, ...) FN(X, Y) | SET_3_BITS(FN, X , __VA_ARGS__)
#define SET_5_BITS(FN, X, Y, ...) FN(X, Y) | SET_4_BITS(FN, X , __VA_ARGS__)
#define SET_6_BITS(FN, X, Y, ...) FN(X, Y) | SET_5_BITS(FN, X , __VA_ARGS__)
#define SET_7_BITS(FN, X, Y, ...) FN(X, Y) | SET_6_BITS(FN, X , __VA_ARGS__)
#define SET_8_BITS(FN, X, Y, ...) FN(X, Y) | SET_7_BITS(FN, X , __VA_ARGS__)
#define SET_9_BITS(FN, X, Y, ...) FN(X, Y) | SET_8_BITS(FN, X , __VA_ARGS__)
#define SET_10_BITS(FN, X, Y, ...) FN(X, Y) | SET_9_BITS(FN, X , __VA_ARGS__)
#define SET_11_BITS(FN, X, Y, ...) FN(X, Y) | SET_10_BITS(FN, X , __VA_ARGS__)
#define SET_12_BITS(FN, X, Y, ...) FN(X, Y) | SET_11_BITS(FN, X , __VA_ARGS__)
#define SET_13_BITS(FN, X, Y, ...) FN(X, Y) | SET_12_BITS(FN, X , __VA_ARGS__)
#define SET_14_BITS(FN, X, Y, ...) FN(X, Y) | SET_13_BITS(FN, X , __VA_ARGS__)
#define SET_15_BITS(FN, X, Y, ...) FN(X, Y) | SET_14_BITS(FN, X , __VA_ARGS__)
#define SET_16_BITS(FN, X, Y, ...) FN(X, Y) | SET_15_BITS(FN, X , __VA_ARGS__)
#define SET_17_BITS(FN, X, Y, ...) FN(X, Y) | SET_16_BITS(FN, X , __VA_ARGS__)
#define SET_18_BITS(FN, X, Y, ...) FN(X, Y) | SET_17_BITS(FN, X , __VA_ARGS__)
#define SET_19_BITS(FN, X, Y, ...) FN(X, Y) | SET_18_BITS(FN, X , __VA_ARGS__)
#define SET_20_BITS(FN, X, Y, ...) FN(X, Y) | SET_19_BITS(FN, X , __VA_ARGS__)
#define SET_21_BITS(FN, X, Y, ...) FN(X, Y) | SET_20_BITS(FN, X , __VA_ARGS__)
#define SET_22_BITS(FN, X, Y, ...) FN(X, Y) | SET_21_BITS(FN, X , __VA_ARGS__)
#define SET_23_BITS(FN, X, Y, ...) FN(X, Y) | SET_22_BITS(FN, X , __VA_ARGS__)
#define SET_24_BITS(FN, X, Y, ...) FN(X, Y) | SET_23_BITS(FN, X , __VA_ARGS__)
#define SET_25_BITS(FN, X, Y, ...) FN(X, Y) | SET_24_BITS(FN, X , __VA_ARGS__)
#define SET_26_BITS(FN, X, Y, ...) FN(X, Y) | SET_25_BITS(FN, X , __VA_ARGS__)
#define SET_27_BITS(FN, X, Y, ...) FN(X, Y) | SET_26_BITS(FN, X , __VA_ARGS__)
#define SET_28_BITS(FN, X, Y, ...) FN(X, Y) | SET_27_BITS(FN, X , __VA_ARGS__)
#define SET_29_BITS(FN, X, Y, ...) FN(X, Y) | SET_28_BITS(FN, X , __VA_ARGS__)
#define SET_30_BITS(FN, X, Y, ...) FN(X, Y) | SET_29_BITS(FN, X , __VA_ARGS__)
#define SET_31_BITS(FN, X, Y, ...) FN(X, Y) | SET_30_BITS(FN, X , __VA_ARGS__)
#define SET_32_BITS(FN, X, Y, ...) FN(X, Y) | SET_31_BITS(FN, X , __VA_ARGS__)
#define SET_33_BITS(FN, X, Y, ...) FN(X, Y) | SET_32_BITS(FN, X , __VA_ARGS__)
#define SET_34_BITS(FN, X, Y, ...) FN(X, Y) | SET_33_BITS(FN, X , __VA_ARGS__)
#define SET_35_BITS(FN, X, Y, ...) FN(X, Y) | SET_34_BITS(FN, X , __VA_ARGS__)
#define SET_36_BITS(FN, X, Y, ...) FN(X, Y) | SET_35_BITS(FN, X , __VA_ARGS__)
#define SET_37_BITS(FN, X, Y, ...) FN(X, Y) | SET_36_BITS(FN, X , __VA_ARGS__)
#define SET_38_BITS(FN, X, Y, ...) FN(X, Y) | SET_37_BITS(FN, X , __VA_ARGS__)
#define SET_39_BITS(FN, X, Y, ...) FN(X, Y) | SET_38_BITS(FN, X , __VA_ARGS__)
#define SET_40_BITS(FN, X, Y, ...) FN(X, Y) | SET_39_BITS(FN, X , __VA_ARGS__)
#define SET_41_BITS(FN, X, Y, ...) FN(X, Y) | SET_40_BITS(FN, X , __VA_ARGS__)
#define SET_42_BITS(FN, X, Y, ...) FN(X, Y) | SET_41_BITS(FN, X , __VA_ARGS__)
#define SET_43_BITS(FN, X, Y, ...) FN(X, Y) | SET_42_BITS(FN, X , __VA_ARGS__)
#define SET_44_BITS(FN, X, Y, ...) FN(X, Y) | SET_43_BITS(FN, X , __VA_ARGS__)
#define SET_45_BITS(FN, X, Y, ...) FN(X, Y) | SET_44_BITS(FN, X , __VA_ARGS__)
#define SET_46_BITS(FN, X, Y, ...) FN(X, Y) | SET_45_BITS(FN, X , __VA_ARGS__)
#define SET_47_BITS(FN, X, Y, ...) FN(X, Y) | SET_46_BITS(FN, X , __VA_ARGS__)
#define SET_48_BITS(FN, X, Y, ...) FN(X, Y) | SET_47_BITS(FN, X , __VA_ARGS__)
#define SET_49_BITS(FN, X, Y, ...) FN(X, Y) | SET_48_BITS(FN, X , __VA_ARGS__)
#define SET_50_BITS(FN, X, Y, ...) FN(X, Y) | SET_49_BITS(FN, X , __VA_ARGS__)
#define SET_51_BITS(FN, X, Y, ...) FN(X, Y) | SET_50_BITS(FN, X , __VA_ARGS__)
#define SET_52_BITS(FN, X, Y, ...) FN(X, Y) | SET_51_BITS(FN, X , __VA_ARGS__)
#define SET_53_BITS(FN, X, Y, ...) FN(X, Y) | SET_52_BITS(FN, X , __VA_ARGS__)
#define SET_54_BITS(FN, X, Y, ...) FN(X, Y) | SET_53_BITS(FN, X , __VA_ARGS__)
#define SET_55_BITS(FN, X, Y, ...) FN(X, Y) | SET_54_BITS(FN, X , __VA_ARGS__)
#define SET_56_BITS(FN, X, Y, ...) FN(X, Y) | SET_55_BITS(FN, X , __VA_ARGS__)
#define SET_57_BITS(FN, X, Y, ...) FN(X, Y) | SET_56_BITS(FN, X , __VA_ARGS__)
#define SET_58_BITS(FN, X, Y, ...) FN(X, Y) | SET_57_BITS(FN, X , __VA_ARGS__)
#define SET_59_BITS(FN, X, Y, ...) FN(X, Y) | SET_58_BITS(FN, X , __VA_ARGS__)
#define SET_60_BITS(FN, X, Y, ...) FN(X, Y) | SET_59_BITS(FN, X , __VA_ARGS__)
#define SET_61_BITS(FN, X, Y, ...) FN(X, Y) | SET_60_BITS(FN, X , __VA_ARGS__)
#define SET_62_BITS(FN, X, Y, ...) FN(X, Y) | SET_61_BITS(FN, X , __VA_ARGS__)
#define SET_63_BITS(FN, X, Y, ...) FN(X, Y) | SET_62_BITS(FN, X , __VA_ARGS__)
#define SET_64_BITS(FN, X, Y, ...) FN(X, Y) | SET_63_BITS(FN, X , __VA_ARGS__)
#define SET_65_BITS(FN, X, Y, ...) FN(X, Y) | SET_64_BITS(FN, X , __VA_ARGS__)
#define SET_66_BITS(FN, X, Y, ...) FN(X, Y) | SET_65_BITS(FN, X , __VA_ARGS__)
#define SET_67_BITS(FN, X, Y, ...) FN(X, Y) | SET_66_BITS(FN, X , __VA_ARGS__)
#define SET_68_BITS(FN, X, Y, ...) FN(X, Y) | SET_67_BITS(FN, X , __VA_ARGS__)
#define SET_69_BITS(FN, X, Y, ...) FN(X, Y) | SET_68_BITS(FN, X , __VA_ARGS__)
#define SET_70_BITS(FN, X, Y, ...) FN(X, Y) | SET_69_BITS(FN, X , __VA_ARGS__)
#define SET_71_BITS(FN, X, Y, ...) FN(X, Y) | SET_70_BITS(FN, X , __VA_ARGS__)
#define SET_72_BITS(FN, X, Y, ...) FN(X, Y) | SET_71_BITS(FN, X , __VA_ARGS__)
#define SET_73_BITS(FN, X, Y, ...) FN(X, Y) | SET_72_BITS(FN, X , __VA_ARGS__)
#define SET_74_BITS(FN, X, Y, ...) FN(X, Y) | SET_73_BITS(FN, X , __VA_ARGS__)
#define SET_75_BITS(FN, X, Y, ...) FN(X, Y) | SET_74_BITS(FN, X , __VA_ARGS__)
#define SET_76_BITS(FN, X, Y, ...) FN(X, Y) | SET_75_BITS(FN, X , __VA_ARGS__)
#define SET_77_BITS(FN, X, Y, ...) FN(X, Y) | SET_76_BITS(FN, X , __VA_ARGS__)
#define SET_78_BITS(FN, X, Y, ...) FN(X, Y) | SET_77_BITS(FN, X , __VA_ARGS__)
#define SET_79_BITS(FN, X, Y, ...) FN(X, Y) | SET_78_BITS(FN, X , __VA_ARGS__)
#define SET_80_BITS(FN, X, Y, ...) FN(X, Y) | SET_79_BITS(FN, X , __VA_ARGS__)
#define SET_81_BITS(FN, X, Y, ...) FN(X, Y) | SET_80_BITS(FN, X , __VA_ARGS__)
#define SET_82_BITS(FN, X, Y, ...) FN(X, Y) | SET_81_BITS(FN, X , __VA_ARGS__)
#define SET_83_BITS(FN, X, Y, ...) FN(X, Y) | SET_82_BITS(FN, X , __VA_ARGS__)
#define SET_84_BITS(FN, X, Y, ...) FN(X, Y) | SET_83_BITS(FN, X , __VA_ARGS__)
#define SET_85_BITS(FN, X, Y, ...) FN(X, Y) | SET_84_BITS(FN, X , __VA_ARGS__)
#define SET_86_BITS(FN, X, Y, ...) FN(X, Y) | SET_85_BITS(FN, X , __VA_ARGS__)
#define SET_87_BITS(FN, X, Y, ...) FN(X, Y) | SET_86_BITS(FN, X , __VA_ARGS__)
#define SET_88_BITS(FN, X, Y, ...) FN(X, Y) | SET_87_BITS(FN, X , __VA_ARGS__)
#define SET_89_BITS(FN, X, Y, ...) FN(X, Y) | SET_88_BITS(FN, X , __VA_ARGS__)
#define SET_90_BITS(FN, X, Y, ...) FN(X, Y) | SET_89_BITS(FN, X , __VA_ARGS__)
#define SET_91_BITS(FN, X, Y, ...) FN(X, Y) | SET_90_BITS(FN, X , __VA_ARGS__)
#define SET_92_BITS(FN, X, Y, ...) FN(X, Y) | SET_91_BITS(FN, X , __VA_ARGS__)
#define SET_93_BITS(FN, X, Y, ...) FN(X, Y) | SET_92_BITS(FN, X , __VA_ARGS__)
#define SET_94_BITS(FN, X, Y, ...) FN(X, Y) | SET_93_BITS(FN, X , __VA_ARGS__)
#define SET_95_BITS(FN, X, Y, ...) FN(X, Y) | SET_94_BITS(FN, X , __VA_ARGS__)
#define SET_96_BITS(FN, X, Y, ...) FN(X, Y) | SET_95_BITS(FN, X , __VA_ARGS__)
#define SET_97_BITS(FN, X, Y, ...) FN(X, Y) | SET_96_BITS(FN, X , __VA_ARGS__)
#define SET_98_BITS(FN, X, Y, ...) FN(X, Y) | SET_97_BITS(FN, X , __VA_ARGS__)
#define SET_99_BITS(FN, X, Y, ...) FN(X, Y) | SET_98_BITS(FN, X , __VA_ARGS__)
#define SET_100_BITS(FN, X, Y, ...) FN(X, Y) | SET_99_BITS(FN, X , __VA_ARGS__)
#define SET_101_BITS(FN, X, Y, ...) FN(X, Y) | SET_100_BITS(FN, X , __VA_ARGS__)
#define SET_102_BITS(FN, X, Y, ...) FN(X, Y) | SET_101_BITS(FN, X , __VA_ARGS__)
#define SET_103_BITS(FN, X, Y, ...) FN(X, Y) | SET_102_BITS(FN, X , __VA_ARGS__)
#define SET_104_BITS(FN, X, Y, ...) FN(X, Y) | SET_103_BITS(FN, X , __VA_ARGS__)
#define SET_105_BITS(FN, X, Y, ...) FN(X, Y) | SET_104_BITS(FN, X , __VA_ARGS__)
#define SET_106_BITS(FN, X, Y, ...) FN(X, Y) | SET_105_BITS(FN, X , __VA_ARGS__)
#define SET_107_BITS(FN, X, Y, ...) FN(X, Y) | SET_106_BITS(FN, X , __VA_ARGS__)
#define SET_108_BITS(FN, X, Y, ...) FN(X, Y) | SET_107_BITS(FN, X , __VA_ARGS__)
#define SET_109_BITS(FN, X, Y, ...) FN(X, Y) | SET_108_BITS(FN, X , __VA_ARGS__)
#define SET_110_BITS(FN, X, Y, ...) FN(X, Y) | SET_109_BITS(FN, X , __VA_ARGS__)
#define SET_111_BITS(FN, X, Y, ...) FN(X, Y) | SET_110_BITS(FN, X , __VA_ARGS__)
#define SET_112_BITS(FN, X, Y, ...) FN(X, Y) | SET_111_BITS(FN, X , __VA_ARGS__)
#define SET_113_BITS(FN, X, Y, ...) FN(X, Y) | SET_112_BITS(FN, X , __VA_ARGS__)
#define SET_114_BITS(FN, X, Y, ...) FN(X, Y) | SET_113_BITS(FN, X , __VA_ARGS__)
#define SET_115_BITS(FN, X, Y, ...) FN(X, Y) | SET_114_BITS(FN, X , __VA_ARGS__)
#define SET_116_BITS(FN, X, Y, ...) FN(X, Y) | SET_115_BITS(FN, X , __VA_ARGS__)
#define SET_117_BITS(FN, X, Y, ...) FN(X, Y) | SET_116_BITS(FN, X , __VA_ARGS__)
#define SET_118_BITS(FN, X, Y, ...) FN(X, Y) | SET_117_BITS(FN, X , __VA_ARGS__)
#define SET_119_BITS(FN, X, Y, ...) FN(X, Y) | SET_118_BITS(FN, X , __VA_ARGS__)
#define SET_120_BITS(FN, X, Y, ...) FN(X, Y) | SET_119_BITS(FN, X , __VA_ARGS__)
#define SET_121_BITS(FN, X, Y, ...) FN(X, Y) | SET_120_BITS(FN, X , __VA_ARGS__)
#define SET_122_BITS(FN, X, Y, ...) FN(X, Y) | SET_121_BITS(FN, X , __VA_ARGS__)
#define SET_123_BITS(FN, X, Y, ...) FN(X, Y) | SET_122_BITS(FN, X , __VA_ARGS__)
#define SET_124_BITS(FN, X, Y, ...) FN(X, Y) | SET_123_BITS(FN, X , __VA_ARGS__)
#define SET_125_BITS(FN, X, Y, ...) FN(X, Y) | SET_124_BITS(FN, X , __VA_ARGS__)
#define SET_126_BITS(FN, X, Y, ...) FN(X, Y) | SET_125_BITS(FN, X , __VA_ARGS__)
#define SET_127_BITS(FN, X, Y, ...) FN(X, Y) | SET_126_BITS(FN, X , __VA_ARGS__)
#define SET_128_BITS(FN, X, Y, ...) FN(X, Y) | SET_127_BITS(FN, X , __VA_ARGS__)
#define SET_129_BITS(FN, X, Y, ...) FN(X, Y) | SET_128_BITS(FN, X , __VA_ARGS__)
#define SET_130_BITS(FN, X, Y, ...) FN(X, Y) | SET_129_BITS(FN, X , __VA_ARGS__)
#define SET_131_BITS(FN, X, Y, ...) FN(X, Y) | SET_130_BITS(FN, X , __VA_ARGS__)
#define SET_132_BITS(FN, X, Y, ...) FN(X, Y) | SET_131_BITS(FN, X , __VA_ARGS__)
#define SET_133_BITS(FN, X, Y, ...) FN(X, Y) | SET_132_BITS(FN, X , __VA_ARGS__)
#define SET_134_BITS(FN, X, Y, ...) FN(X, Y) | SET_133_BITS(FN, X , __VA_ARGS__)
#define SET_135_BITS(FN, X, Y, ...) FN(X, Y) | SET_134_BITS(FN, X , __VA_ARGS__)
#define SET_136_BITS(FN, X, Y, ...) FN(X, Y) | SET_135_BITS(FN, X , __VA_ARGS__)
#define SET_137_BITS(FN, X, Y, ...) FN(X, Y) | SET_136_BITS(FN, X , __VA_ARGS__)
#define SET_138_BITS(FN, X, Y, ...) FN(X, Y) | SET_137_BITS(FN, X , __VA_ARGS__)
#define SET_139_BITS(FN, X, Y, ...) FN(X, Y) | SET_138_BITS(FN, X , __VA_ARGS__)
#define SET_140_BITS(FN, X, Y, ...) FN(X, Y) | SET_139_BITS(FN, X , __VA_ARGS__)
#define SET_141_BITS(FN, X, Y, ...) FN(X, Y) | SET_140_BITS(FN, X , __VA_ARGS__)
#define SET_142_BITS(FN, X, Y, ...) FN(X, Y) | SET_141_BITS(FN, X , __VA_ARGS__)
#define SET_143_BITS(FN, X, Y, ...) FN(X, Y) | SET_142_BITS(FN, X , __VA_ARGS__)
#define SET_144_BITS(FN, X, Y, ...) FN(X, Y) | SET_143_BITS(FN, X , __VA_ARGS__)
#define SET_145_BITS(FN, X, Y, ...) FN(X, Y) | SET_144_BITS(FN, X , __VA_ARGS__)
#define SET_146_BITS(FN, X, Y, ...) FN(X, Y) | SET_145_BITS(FN, X , __VA_ARGS__)
#define SET_147_BITS(FN, X, Y, ...) FN(X, Y) | SET_146_BITS(FN, X , __VA_ARGS__)
#define SET_148_BITS(FN, X, Y, ...) FN(X, Y) | SET_147_BITS(FN, X , __VA_ARGS__)
#define SET_149_BITS(FN, X, Y, ...) FN(X, Y) | SET_148_BITS(FN, X , __VA_ARGS__)
#define SET_150_BITS(FN, X, Y, ...) FN(X, Y) | SET_149_BITS(FN, X , __VA_ARGS__)
#define SET_151_BITS(FN, X, Y, ...) FN(X, Y) | SET_150_BITS(FN, X , __VA_ARGS__)
#define SET_152_BITS(FN, X, Y, ...) FN(X, Y) | SET_151_BITS(FN, X , __VA_ARGS__)
#define SET_153_BITS(FN, X, Y, ...) FN(X, Y) | SET_152_BITS(FN, X , __VA_ARGS__)
#define SET_154_BITS(FN, X, Y, ...) FN(X, Y) | SET_153_BITS(FN, X , __VA_ARGS__)
#define SET_155_BITS(FN, X, Y, ...) FN(X, Y) | SET_154_BITS(FN, X , __VA_ARGS__)
#define SET_156_BITS(FN, X, Y, ...) FN(X, Y) | SET_155_BITS(FN, X , __VA_ARGS__)
#define SET_157_BITS(FN, X, Y, ...) FN(X, Y) | SET_156_BITS(FN, X , __VA_ARGS__)
#define SET_158_BITS(FN, X, Y, ...) FN(X, Y) | SET_157_BITS(FN, X , __VA_ARGS__)
#define SET_159_BITS(FN, X, Y, ...) FN(X, Y) | SET_158_BITS(FN, X , __VA_ARGS__)
#define SET_160_BITS(FN, X, Y, ...) FN(X, Y) | SET_159_BITS(FN, X , __VA_ARGS__)
#define SET_161_BITS(FN, X, Y, ...) FN(X, Y) | SET_160_BITS(FN, X , __VA_ARGS__)
#define SET_162_BITS(FN, X, Y, ...) FN(X, Y) | SET_161_BITS(FN, X , __VA_ARGS__)
#define SET_163_BITS(FN, X, Y, ...) FN(X, Y) | SET_162_BITS(FN, X , __VA_ARGS__)
#define SET_164_BITS(FN, X, Y, ...) FN(X, Y) | SET_163_BITS(FN, X , __VA_ARGS__)
#define SET_165_BITS(FN, X, Y, ...) FN(X, Y) | SET_164_BITS(FN, X , __VA_ARGS__)
#define SET_166_BITS(FN, X, Y, ...) FN(X, Y) | SET_165_BITS(FN, X , __VA_ARGS__)
#define SET_167_BITS(FN, X, Y, ...) FN(X, Y) | SET_166_BITS(FN, X , __VA_ARGS__)
#define SET_168_BITS(FN, X, Y, ...) FN(X, Y) | SET_167_BITS(FN, X , __VA_ARGS__)
#define SET_169_BITS(FN, X, Y, ...) FN(X, Y) | SET_168_BITS(FN, X , __VA_ARGS__)
#define SET_170_BITS(FN, X, Y, ...) FN(X, Y) | SET_169_BITS(FN, X , __VA_ARGS__)
#define SET_171_BITS(FN, X, Y, ...) FN(X, Y) | SET_170_BITS(FN, X , __VA_ARGS__)
#define SET_172_BITS(FN, X, Y, ...) FN(X, Y) | SET_171_BITS(FN, X , __VA_ARGS__)
#define SET_173_BITS(FN, X, Y, ...) FN(X, Y) | SET_172_BITS(FN, X , __VA_ARGS__)
#define SET_174_BITS(FN, X, Y, ...) FN(X, Y) | SET_173_BITS(FN, X , __VA_ARGS__)
#define SET_175_BITS(FN, X, Y, ...) FN(X, Y) | SET_174_BITS(FN, X , __VA_ARGS__)
#define SET_176_BITS(FN, X, Y, ...) FN(X, Y) | SET_175_BITS(FN, X , __VA_ARGS__)
#define SET_177_BITS(FN, X, Y, ...) FN(X, Y) | SET_176_BITS(FN, X , __VA_ARGS__)
#define SET_178_BITS(FN, X, Y, ...) FN(X, Y) | SET_177_BITS(FN, X , __VA_ARGS__)
#define SET_179_BITS(FN, X, Y, ...) FN(X, Y) | SET_178_BITS(FN, X , __VA_ARGS__)
#define SET_180_BITS(FN, X, Y, ...) FN(X, Y) | SET_179_BITS(FN, X , __VA_ARGS__)
#define SET_181_BITS(FN, X, Y, ...) FN(X, Y) | SET_180_BITS(FN, X , __VA_ARGS__)
#define SET_182_BITS(FN, X, Y, ...) FN(X, Y) | SET_181_BITS(FN, X , __VA_ARGS__)
#define SET_183_BITS(FN, X, Y, ...) FN(X, Y) | SET_182_BITS(FN, X , __VA_ARGS__)
#define SET_184_BITS(FN, X, Y, ...) FN(X, Y) | SET_183_BITS(FN, X , __VA_ARGS__)
#define SET_185_BITS(FN, X, Y, ...) FN(X, Y) | SET_184_BITS(FN, X , __VA_ARGS__)
#define SET_186_BITS(FN, X, Y, ...) FN(X, Y) | SET_185_BITS(FN, X , __VA_ARGS__)
#define SET_187_BITS(FN, X, Y, ...) FN(X, Y) | SET_186_BITS(FN, X , __VA_ARGS__)
#define SET_188_BITS(FN, X, Y, ...) FN(X, Y) | SET_187_BITS(FN, X , __VA_ARGS__)
#define SET_189_BITS(FN, X, Y, ...) FN(X, Y) | SET_188_BITS(FN, X , __VA_ARGS__)
#define SET_190_BITS(FN, X, Y, ...) FN(X, Y) | SET_189_BITS(FN, X , __VA_ARGS__)
#define SET_191_BITS(FN, X, Y, ...) FN(X, Y) | SET_190_BITS(FN, X , __VA_ARGS__)
#define SET_192_BITS(FN, X, Y, ...) FN(X, Y) | SET_191_BITS(FN, X , __VA_ARGS__)
#define SET_193_BITS(FN, X, Y, ...) FN(X, Y) | SET_192_BITS(FN, X , __VA_ARGS__)
#define SET_194_BITS(FN, X, Y, ...) FN(X, Y) | SET_193_BITS(FN, X , __VA_ARGS__)
#define SET_195_BITS(FN, X, Y, ...) FN(X, Y) | SET_194_BITS(FN, X , __VA_ARGS__)
#define SET_196_BITS(FN, X, Y, ...) FN(X, Y) | SET_195_BITS(FN, X , __VA_ARGS__)
#define SET_197_BITS(FN, X, Y, ...) FN(X, Y) | SET_196_BITS(FN, X , __VA_ARGS__)
#define SET_198_BITS(FN, X, Y, ...) FN(X, Y) | SET_197_BITS(FN, X , __VA_ARGS__)
#define SET_199_BITS(FN, X, Y, ...) FN(X, Y) | SET_198_BITS(FN, X , __VA_ARGS__)
#define SET_200_BITS(FN, X, Y, ...) FN(X, Y) | SET_199_BITS(FN, X , __VA_ARGS__)
#define SET_201_BITS(FN, X, Y, ...) FN(X, Y) | SET_200_BITS(FN, X , __VA_ARGS__)
#define SET_202_BITS(FN, X, Y, ...) FN(X, Y) | SET_201_BITS(FN, X , __VA_ARGS__)
#define SET_203_BITS(FN, X, Y, ...) FN(X, Y) | SET_202_BITS(FN, X , __VA_ARGS__)
#define SET_204_BITS(FN, X, Y, ...) FN(X, Y) | SET_203_BITS(FN, X , __VA_ARGS__)
#define SET_205_BITS(FN, X, Y, ...) FN(X, Y) | SET_204_BITS(FN, X , __VA_ARGS__)
#define SET_206_BITS(FN, X, Y, ...) FN(X, Y) | SET_205_BITS(FN, X , __VA_ARGS__)
#define SET_207_BITS(FN, X, Y, ...) FN(X, Y) | SET_206_BITS(FN, X , __VA_ARGS__)
#define SET_208_BITS(FN, X, Y, ...) FN(X, Y) | SET_207_BITS(FN, X , __VA_ARGS__)
#define SET_209_BITS(FN, X, Y, ...) FN(X, Y) | SET_208_BITS(FN, X , __VA_ARGS__)
#define SET_210_BITS(FN, X, Y, ...) FN(X, Y) | SET_209_BITS(FN, X , __VA_ARGS__)
#define SET_211_BITS(FN, X, Y, ...) FN(X, Y) | SET_210_BITS(FN, X , __VA_ARGS__)
#define SET_212_BITS(FN, X, Y, ...) FN(X, Y) | SET_211_BITS(FN, X , __VA_ARGS__)
#define SET_213_BITS(FN, X, Y, ...) FN(X, Y) | SET_212_BITS(FN, X , __VA_ARGS__)
#define SET_214_BITS(FN, X, Y, ...) FN(X, Y) | SET_213_BITS(FN, X , __VA_ARGS__)
#define SET_215_BITS(FN, X, Y, ...) FN(X, Y) | SET_214_BITS(FN, X , __VA_ARGS__)
#define SET_216_BITS(FN, X, Y, ...) FN(X, Y) | SET_215_BITS(FN, X , __VA_ARGS__)
#define SET_217_BITS(FN, X, Y, ...) FN(X, Y) | SET_216_BITS(FN, X , __VA_ARGS__)
#define SET_218_BITS(FN, X, Y, ...) FN(X, Y) | SET_217_BITS(FN, X , __VA_ARGS__)
#define SET_219_BITS(FN, X, Y, ...) FN(X, Y) | SET_218_BITS(FN, X , __VA_ARGS__)
#define SET_220_BITS(FN, X, Y, ...) FN(X, Y) | SET_219_BITS(FN, X , __VA_ARGS__)
#define SET_221_BITS(FN, X, Y, ...) FN(X, Y) | SET_220_BITS(FN, X , __VA_ARGS__)
#define SET_222_BITS(FN, X, Y, ...) FN(X, Y) | SET_221_BITS(FN, X , __VA_ARGS__)
#define SET_223_BITS(FN, X, Y, ...) FN(X, Y) | SET_222_BITS(FN, X , __VA_ARGS__)
#define SET_224_BITS(FN, X, Y, ...) FN(X, Y) | SET_223_BITS(FN, X , __VA_ARGS__)
#define SET_225_BITS(FN, X, Y, ...) FN(X, Y) | SET_224_BITS(FN, X , __VA_ARGS__)
#define SET_226_BITS(FN, X, Y, ...) FN(X, Y) | SET_225_BITS(FN, X , __VA_ARGS__)
#define SET_227_BITS(FN, X, Y, ...) FN(X, Y) | SET_226_BITS(FN, X , __VA_ARGS__)
#define SET_228_BITS(FN, X, Y, ...) FN(X, Y) | SET_227_BITS(FN, X , __VA_ARGS__)
#define SET_229_BITS(FN, X, Y, ...) FN(X, Y) | SET_228_BITS(FN, X , __VA_ARGS__)
#define SET_230_BITS(FN, X, Y, ...) FN(X, Y) | SET_229_BITS(FN, X , __VA_ARGS__)
#define SET_231_BITS(FN, X, Y, ...) FN(X, Y) | SET_230_BITS(FN, X , __VA_ARGS__)
#define SET_232_BITS(FN, X, Y, ...) FN(X, Y) | SET_231_BITS(FN, X , __VA_ARGS__)
#define SET_233_BITS(FN, X, Y, ...) FN(X, Y) | SET_232_BITS(FN, X , __VA_ARGS__)
#define SET_234_BITS(FN, X, Y, ...) FN(X, Y) | SET_233_BITS(FN, X , __VA_ARGS__)
#define SET_235_BITS(FN, X, Y, ...) FN(X, Y) | SET_234_BITS(FN, X , __VA_ARGS__)
#define SET_236_BITS(FN, X, Y, ...) FN(X, Y) | SET_235_BITS(FN, X , __VA_ARGS__)
#define SET_237_BITS(FN, X, Y, ...) FN(X, Y) | SET_236_BITS(FN, X , __VA_ARGS__)
#define SET_238_BITS(FN, X, Y, ...) FN(X, Y) | SET_237_BITS(FN, X , __VA_ARGS__)
#define SET_239_BITS(FN, X, Y, ...) FN(X, Y) | SET_238_BITS(FN, X , __VA_ARGS__)
#define SET_240_BITS(FN, X, Y, ...) FN(X, Y) | SET_239_BITS(FN, X , __VA_ARGS__)
#define SET_241_BITS(FN, X, Y, ...) FN(X, Y) | SET_240_BITS(FN, X , __VA_ARGS__)
#define SET_242_BITS(FN, X, Y, ...) FN(X, Y) | SET_241_BITS(FN, X , __VA_ARGS__)
#define SET_243_BITS(FN, X, Y, ...) FN(X, Y) | SET_242_BITS(FN, X , __VA_ARGS__)
#define SET_244_BITS(FN, X, Y, ...) FN(X, Y) | SET_243_BITS(FN, X , __VA_ARGS__)
#define SET_245_BITS(FN, X, Y, ...) FN(X, Y) | SET_244_BITS(FN, X , __VA_ARGS__)
#define SET_246_BITS(FN, X, Y, ...) FN(X, Y) | SET_245_BITS(FN, X , __VA_ARGS__)
#define SET_247_BITS(FN, X, Y, ...) FN(X, Y) | SET_246_BITS(FN, X , __VA_ARGS__)
#define SET_248_BITS(FN, X, Y, ...) FN(X, Y) | SET_247_BITS(FN, X , __VA_ARGS__)
#define SET_249_BITS(FN, X, Y, ...) FN(X, Y) | SET_248_BITS(FN, X , __VA_ARGS__)
#define SET_250_BITS(FN, X, Y, ...) FN(X, Y) | SET_249_BITS(FN, X , __VA_ARGS__)
#define SET_251_BITS(FN, X, Y, ...) FN(X, Y) | SET_250_BITS(FN, X , __VA_ARGS__)
#define SET_252_BITS(FN, X, Y, ...) FN(X, Y) | SET_251_BITS(FN, X , __VA_ARGS__)
#define SET_253_BITS(FN, X, Y, ...) FN(X, Y) | SET_252_BITS(FN, X , __VA_ARGS__)
#define SET_254_BITS(FN, X, Y, ...) FN(X, Y) | SET_253_BITS(FN, X , __VA_ARGS__)
#define SET_255_BITS(FN, X, Y, ...) FN(X, Y) | SET_254_BITS(FN, X , __VA_ARGS__)
#define SET_256_BITS(FN, X, Y, ...) FN(X, Y) | SET_255_BITS(FN, X , __VA_ARGS__)

/*
 * MACRO to assign zeros to array.
 */
#define SET_1_ZERO 0
#define SET_2_ZERO 0,SET_1_ZERO
#define SET_3_ZERO 0,SET_2_ZERO
#define SET_4_ZERO 0,SET_3_ZERO
#define SET_5_ZERO 0,SET_4_ZERO
#define SET_6_ZERO 0,SET_5_ZERO
#define SET_7_ZERO 0,SET_6_ZERO
#define SET_8_ZERO 0,SET_7_ZERO
#define SET_9_ZERO 0,SET_8_ZERO
#define SET_10_ZERO 0,SET_9_ZERO
#define SET_11_ZERO 0,SET_10_ZERO
#define SET_12_ZERO 0,SET_11_ZERO
#define SET_13_ZERO 0,SET_12_ZERO
#define SET_14_ZERO 0,SET_13_ZERO
#define SET_15_ZERO 0,SET_14_ZERO
#define SET_16_ZERO 0,SET_15_ZERO
#define SET_17_ZERO 0,SET_16_ZERO
#define SET_18_ZERO 0,SET_17_ZERO
#define SET_19_ZERO 0,SET_18_ZERO
#define SET_20_ZERO 0,SET_19_ZERO
#define SET_21_ZERO 0,SET_20_ZERO
#define SET_22_ZERO 0,SET_21_ZERO
#define SET_23_ZERO 0,SET_22_ZERO
#define SET_24_ZERO 0,SET_23_ZERO
#define SET_25_ZERO 0,SET_24_ZERO
#define SET_26_ZERO 0,SET_25_ZERO
#define SET_27_ZERO 0,SET_26_ZERO
#define SET_28_ZERO 0,SET_27_ZERO
#define SET_29_ZERO 0,SET_28_ZERO
#define SET_30_ZERO 0,SET_29_ZERO
#define SET_31_ZERO 0,SET_30_ZERO
#define SET_32_ZERO 0,SET_31_ZERO
#define SET_33_ZERO 0,SET_32_ZERO
#define SET_34_ZERO 0,SET_33_ZERO
#define SET_35_ZERO 0,SET_34_ZERO
#define SET_36_ZERO 0,SET_35_ZERO
#define SET_37_ZERO 0,SET_36_ZERO
#define SET_38_ZERO 0,SET_37_ZERO
#define SET_39_ZERO 0,SET_38_ZERO
#define SET_40_ZERO 0,SET_39_ZERO
#define SET_41_ZERO 0,SET_40_ZERO
#define SET_42_ZERO 0,SET_41_ZERO
#define SET_43_ZERO 0,SET_42_ZERO
#define SET_44_ZERO 0,SET_43_ZERO
#define SET_45_ZERO 0,SET_44_ZERO
#define SET_46_ZERO 0,SET_45_ZERO
#define SET_47_ZERO 0,SET_46_ZERO
#define SET_48_ZERO 0,SET_47_ZERO
#define SET_49_ZERO 0,SET_48_ZERO
#define SET_50_ZERO 0,SET_49_ZERO
#define SET_51_ZERO 0,SET_50_ZERO
#define SET_52_ZERO 0,SET_51_ZERO
#define SET_53_ZERO 0,SET_52_ZERO
#define SET_54_ZERO 0,SET_53_ZERO
#define SET_55_ZERO 0,SET_54_ZERO
#define SET_56_ZERO 0,SET_55_ZERO
#define SET_57_ZERO 0,SET_56_ZERO
#define SET_58_ZERO 0,SET_57_ZERO
#define SET_59_ZERO 0,SET_58_ZERO
#define SET_60_ZERO 0,SET_59_ZERO
#define SET_61_ZERO 0,SET_60_ZERO
#define SET_62_ZERO 0,SET_61_ZERO
#define SET_63_ZERO 0,SET_62_ZERO
#define SET_64_ZERO 0,SET_63_ZERO
#define SET_65_ZERO 0,SET_64_ZERO
#define SET_66_ZERO 0,SET_65_ZERO
#define SET_67_ZERO 0,SET_66_ZERO
#define SET_68_ZERO 0,SET_67_ZERO
#define SET_69_ZERO 0,SET_68_ZERO
#define SET_70_ZERO 0,SET_69_ZERO
#define SET_71_ZERO 0,SET_70_ZERO
#define SET_72_ZERO 0,SET_71_ZERO
#define SET_73_ZERO 0,SET_72_ZERO
#define SET_74_ZERO 0,SET_73_ZERO
#define SET_75_ZERO 0,SET_74_ZERO
#define SET_76_ZERO 0,SET_75_ZERO
#define SET_77_ZERO 0,SET_76_ZERO
#define SET_78_ZERO 0,SET_77_ZERO
#define SET_79_ZERO 0,SET_78_ZERO
#define SET_80_ZERO 0,SET_79_ZERO
#define SET_81_ZERO 0,SET_80_ZERO
#define SET_82_ZERO 0,SET_81_ZERO
#define SET_83_ZERO 0,SET_82_ZERO
#define SET_84_ZERO 0,SET_83_ZERO
#define SET_85_ZERO 0,SET_84_ZERO
#define SET_86_ZERO 0,SET_85_ZERO
#define SET_87_ZERO 0,SET_86_ZERO
#define SET_88_ZERO 0,SET_87_ZERO
#define SET_89_ZERO 0,SET_88_ZERO
#define SET_90_ZERO 0,SET_89_ZERO
#define SET_91_ZERO 0,SET_90_ZERO
#define SET_92_ZERO 0,SET_91_ZERO
#define SET_93_ZERO 0,SET_92_ZERO
#define SET_94_ZERO 0,SET_93_ZERO
#define SET_95_ZERO 0,SET_94_ZERO
#define SET_96_ZERO 0,SET_95_ZERO
#define SET_97_ZERO 0,SET_96_ZERO
#define SET_98_ZERO 0,SET_97_ZERO
#define SET_99_ZERO 0,SET_98_ZERO
#define SET_100_ZERO 0,SET_99_ZERO
#define SET_101_ZERO 0,SET_100_ZERO
#define SET_102_ZERO 0,SET_101_ZERO
#define SET_103_ZERO 0,SET_102_ZERO
#define SET_104_ZERO 0,SET_103_ZERO
#define SET_105_ZERO 0,SET_104_ZERO
#define SET_106_ZERO 0,SET_105_ZERO
#define SET_107_ZERO 0,SET_106_ZERO
#define SET_108_ZERO 0,SET_107_ZERO
#define SET_109_ZERO 0,SET_108_ZERO
#define SET_110_ZERO 0,SET_109_ZERO
#define SET_111_ZERO 0,SET_110_ZERO
#define SET_112_ZERO 0,SET_111_ZERO
#define SET_113_ZERO 0,SET_112_ZERO
#define SET_114_ZERO 0,SET_113_ZERO
#define SET_115_ZERO 0,SET_114_ZERO
#define SET_116_ZERO 0,SET_115_ZERO
#define SET_117_ZERO 0,SET_116_ZERO
#define SET_118_ZERO 0,SET_117_ZERO
#define SET_119_ZERO 0,SET_118_ZERO
#define SET_120_ZERO 0,SET_119_ZERO
#define SET_121_ZERO 0,SET_120_ZERO
#define SET_122_ZERO 0,SET_121_ZERO
#define SET_123_ZERO 0,SET_122_ZERO
#define SET_124_ZERO 0,SET_123_ZERO
#define SET_125_ZERO 0,SET_124_ZERO
#define SET_126_ZERO 0,SET_125_ZERO
#define SET_127_ZERO 0,SET_126_ZERO
#define SET_128_ZERO 0,SET_127_ZERO
#define SET_129_ZERO 0,SET_128_ZERO
#define SET_130_ZERO 0,SET_129_ZERO
#define SET_131_ZERO 0,SET_130_ZERO
#define SET_132_ZERO 0,SET_131_ZERO
#define SET_133_ZERO 0,SET_132_ZERO
#define SET_134_ZERO 0,SET_133_ZERO
#define SET_135_ZERO 0,SET_134_ZERO
#define SET_136_ZERO 0,SET_135_ZERO
#define SET_137_ZERO 0,SET_136_ZERO
#define SET_138_ZERO 0,SET_137_ZERO
#define SET_139_ZERO 0,SET_138_ZERO
#define SET_140_ZERO 0,SET_139_ZERO
#define SET_141_ZERO 0,SET_140_ZERO
#define SET_142_ZERO 0,SET_141_ZERO
#define SET_143_ZERO 0,SET_142_ZERO
#define SET_144_ZERO 0,SET_143_ZERO
#define SET_145_ZERO 0,SET_144_ZERO
#define SET_146_ZERO 0,SET_145_ZERO
#define SET_147_ZERO 0,SET_146_ZERO
#define SET_148_ZERO 0,SET_147_ZERO
#define SET_149_ZERO 0,SET_148_ZERO
#define SET_150_ZERO 0,SET_149_ZERO
#define SET_151_ZERO 0,SET_150_ZERO
#define SET_152_ZERO 0,SET_151_ZERO
#define SET_153_ZERO 0,SET_152_ZERO
#define SET_154_ZERO 0,SET_153_ZERO
#define SET_155_ZERO 0,SET_154_ZERO
#define SET_156_ZERO 0,SET_155_ZERO
#define SET_157_ZERO 0,SET_156_ZERO
#define SET_158_ZERO 0,SET_157_ZERO
#define SET_159_ZERO 0,SET_158_ZERO
#define SET_160_ZERO 0,SET_159_ZERO
#define SET_161_ZERO 0,SET_160_ZERO
#define SET_162_ZERO 0,SET_161_ZERO
#define SET_163_ZERO 0,SET_162_ZERO
#define SET_164_ZERO 0,SET_163_ZERO
#define SET_165_ZERO 0,SET_164_ZERO
#define SET_166_ZERO 0,SET_165_ZERO
#define SET_167_ZERO 0,SET_166_ZERO
#define SET_168_ZERO 0,SET_167_ZERO
#define SET_169_ZERO 0,SET_168_ZERO
#define SET_170_ZERO 0,SET_169_ZERO
#define SET_171_ZERO 0,SET_170_ZERO
#define SET_172_ZERO 0,SET_171_ZERO
#define SET_173_ZERO 0,SET_172_ZERO
#define SET_174_ZERO 0,SET_173_ZERO
#define SET_175_ZERO 0,SET_174_ZERO
#define SET_176_ZERO 0,SET_175_ZERO
#define SET_177_ZERO 0,SET_176_ZERO
#define SET_178_ZERO 0,SET_177_ZERO
#define SET_179_ZERO 0,SET_178_ZERO
#define SET_180_ZERO 0,SET_179_ZERO
#define SET_181_ZERO 0,SET_180_ZERO
#define SET_182_ZERO 0,SET_181_ZERO
#define SET_183_ZERO 0,SET_182_ZERO
#define SET_184_ZERO 0,SET_183_ZERO
#define SET_185_ZERO 0,SET_184_ZERO
#define SET_186_ZERO 0,SET_185_ZERO
#define SET_187_ZERO 0,SET_186_ZERO
#define SET_188_ZERO 0,SET_187_ZERO
#define SET_189_ZERO 0,SET_188_ZERO
#define SET_190_ZERO 0,SET_189_ZERO
#define SET_191_ZERO 0,SET_190_ZERO
#define SET_192_ZERO 0,SET_191_ZERO
#define SET_193_ZERO 0,SET_192_ZERO
#define SET_194_ZERO 0,SET_193_ZERO
#define SET_195_ZERO 0,SET_194_ZERO
#define SET_196_ZERO 0,SET_195_ZERO
#define SET_197_ZERO 0,SET_196_ZERO
#define SET_198_ZERO 0,SET_197_ZERO
#define SET_199_ZERO 0,SET_198_ZERO
#define SET_200_ZERO 0,SET_199_ZERO
#define SET_201_ZERO 0,SET_200_ZERO
#define SET_202_ZERO 0,SET_201_ZERO
#define SET_203_ZERO 0,SET_202_ZERO
#define SET_204_ZERO 0,SET_203_ZERO
#define SET_205_ZERO 0,SET_204_ZERO
#define SET_206_ZERO 0,SET_205_ZERO
#define SET_207_ZERO 0,SET_206_ZERO
#define SET_208_ZERO 0,SET_207_ZERO
#define SET_209_ZERO 0,SET_208_ZERO
#define SET_210_ZERO 0,SET_209_ZERO
#define SET_211_ZERO 0,SET_210_ZERO
#define SET_212_ZERO 0,SET_211_ZERO
#define SET_213_ZERO 0,SET_212_ZERO
#define SET_214_ZERO 0,SET_213_ZERO
#define SET_215_ZERO 0,SET_214_ZERO
#define SET_216_ZERO 0,SET_215_ZERO
#define SET_217_ZERO 0,SET_216_ZERO
#define SET_218_ZERO 0,SET_217_ZERO
#define SET_219_ZERO 0,SET_218_ZERO
#define SET_220_ZERO 0,SET_219_ZERO
#define SET_221_ZERO 0,SET_220_ZERO
#define SET_222_ZERO 0,SET_221_ZERO
#define SET_223_ZERO 0,SET_222_ZERO
#define SET_224_ZERO 0,SET_223_ZERO
#define SET_225_ZERO 0,SET_224_ZERO
#define SET_226_ZERO 0,SET_225_ZERO
#define SET_227_ZERO 0,SET_226_ZERO
#define SET_228_ZERO 0,SET_227_ZERO
#define SET_229_ZERO 0,SET_228_ZERO
#define SET_230_ZERO 0,SET_229_ZERO
#define SET_231_ZERO 0,SET_230_ZERO
#define SET_232_ZERO 0,SET_231_ZERO
#define SET_233_ZERO 0,SET_232_ZERO
#define SET_234_ZERO 0,SET_233_ZERO
#define SET_235_ZERO 0,SET_234_ZERO
#define SET_236_ZERO 0,SET_235_ZERO
#define SET_237_ZERO 0,SET_236_ZERO
#define SET_238_ZERO 0,SET_237_ZERO
#define SET_239_ZERO 0,SET_238_ZERO
#define SET_240_ZERO 0,SET_239_ZERO
#define SET_241_ZERO 0,SET_240_ZERO
#define SET_242_ZERO 0,SET_241_ZERO
#define SET_243_ZERO 0,SET_242_ZERO
#define SET_244_ZERO 0,SET_243_ZERO
#define SET_245_ZERO 0,SET_244_ZERO
#define SET_246_ZERO 0,SET_245_ZERO
#define SET_247_ZERO 0,SET_246_ZERO
#define SET_248_ZERO 0,SET_247_ZERO
#define SET_249_ZERO 0,SET_248_ZERO
#define SET_250_ZERO 0,SET_249_ZERO
#define SET_251_ZERO 0,SET_250_ZERO
#define SET_252_ZERO 0,SET_251_ZERO
#define SET_253_ZERO 0,SET_252_ZERO
#define SET_254_ZERO 0,SET_253_ZERO
#define SET_255_ZERO 0,SET_254_ZERO

#define STRUCT_MEMBER_CMP(n, type, memb1, memb2) \
{\
    if (type == CMP_PTR_TYPE) \
    { \
        if (memcmp(memb1, memb2, sizeof(*memb1)) != 0) \
        { \
            fprintf(stderr,"memcmp not equal for field number(%d) %s:%d\n", n,__FUNCTION__,__LINE__); \
            fflush(stderr); \
        } \
    } \
    else if (type == CMP_DATA_TYPE) \
    {\
        if (memb1 != memb2) \
        { \
            fprintf(stderr,"data not equal for field number(%d) %s:%d\n",n,__FUNCTION__,__LINE__); \
            fflush(stderr); \
        } \
    } \
    else \
    { \
        fprintf(stderr,"invalid type to compare %s:%d\n",__FUNCTION__,__LINE__); fflush(stderr); \
    } \
}

#define CMP_STRUCT_MEMBERS_1(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n, type, memb1, memb2) \
}
#define CMP_STRUCT_MEMBERS_2(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-1, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_1(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_3(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-2, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_2(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_4(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-3, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_3(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_5(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-4, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_4(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_6(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-5, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_5(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_7(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-6, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_6(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_8(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-7, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_7(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_9(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-8, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_8(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_10(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-9, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_9(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_11(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-10, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_10(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_12(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-11, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_11(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_13(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-12, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_12(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_14(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-13, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_13(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_15(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-14, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_14(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_16(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-15, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_15(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_17(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-16, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_16(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_18(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-17, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_17(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_19(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-18, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_18(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_20(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-19, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_19(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_21(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-20, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_20(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_22(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-21, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_21(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_23(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-22, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_22(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_24(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-23, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_23(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_25(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-24, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_24(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_26(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-25, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_25(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_27(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-26, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_26(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_28(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-27, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_27(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_29(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-28, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_28(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_30(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-29, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_29(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_31(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-30, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_30(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_32(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-31, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_31(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_33(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-32, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_32(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_34(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-33, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_33(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_35(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-34, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_34(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_36(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-35, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_35(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_37(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-36, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_36(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_38(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-37, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_37(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_39(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-38, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_38(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_40(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-39, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_39(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_41(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-40, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_40(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_42(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-41, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_41(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_43(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-42, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_42(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_44(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-43, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_43(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_45(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-44, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_44(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_46(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-45, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_45(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_47(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-46, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_46(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_48(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-47, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_47(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_49(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-48, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_48(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_50(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-49, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_49(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_51(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-50, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_50(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_52(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-51, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_51(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_53(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-52, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_52(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_54(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-53, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_53(n, __VA_ARGS__) \
}
#define CMP_STRUCT_MEMBERS_55(n, type, memb1, memb2, ...) \
{ \
    STRUCT_MEMBER_CMP(n-54, type, memb1, memb2) \
    CMP_STRUCT_MEMBERS_54(n, __VA_ARGS__) \
}


#define RESP_CMP_STRUCT_MEMBERS(FN, n, ...) FN(n, __VA_ARGS__)
#define RESP_CMP_N_STRUCT_MEMBERS(nMemb, ...)  RESP_CMP_STRUCT_MEMBERS(CMP_STRUCT_MEMBERS_##nMemb, nMemb, __VA_ARGS__)

#define UNPACK_RESP_AND_DUMPCOMP(resultcode,unpackfn,dumpfn,respmsg,resplen,argpt,nfield, ...) \
{\
    if(eRSP==respmsg[0])\
    {\
        resultcode = unpackfn(respmsg, resplen, argpt); \
        if (eQCWWAN_ERR_NONE == resultcode) \
        {\
            dumpfn(argpt);\
            swi_uint256_print_mask((argpt)->ParamPresenceMask);\
            if (nfield > 0) \
            { \
                RESP_CMP_N_STRUCT_MEMBERS(nfield,__VA_ARGS__)\
            } \
            else \
            { \
                fprintf(stderr,"nfields not valid(%d) %s:%d\n",resultcode,__FUNCTION__,__LINE__); fflush(stderr); \
            } \
        }\
        else \
        { \
            fprintf(stderr,"unpack failed(%d) %s:%d\n",resultcode,__FUNCTION__,__LINE__); fflush(stderr); \
        } \
    } \
    else \
    {\
        fprintf(stderr,"Not resp %s:%d\n",__FUNCTION__,__LINE__); fflush(stderr); \
        resultcode = eQCWWAN_ERR_INVALID_QMI_RSP;\
    }\
}

/* unpack static response, compare response and run dump function */
#define UNPACK_STATIC_RESP_AND_DUMPCOMP(resultcode,unpackfn,dumpfn,respmsg,resplen,constunpack) \
{\
    if(eRSP==respmsg[0])\
    {\
        unpackfn##_t unpackfn##var; \
        memset(&unpackfn##var,0,sizeof(unpackfn##var));\
        resultcode = unpackfn(respmsg, resplen, &unpackfn##var); \
        if (eQCWWAN_ERR_NONE == resultcode) \
        {\
            dumpfn(&unpackfn##var);\
            swi_uint256_print_mask((&unpackfn##var)->ParamPresenceMask);\
            if( memcmp(&unpackfn##var,constunpack,sizeof(unpackfn##_t)) != 0) \
            { \
                fprintf(stderr,"memcmp cmp not equal(%d) %s:%d\n",resultcode,__FUNCTION__,__LINE__); \
                fflush(stderr); \
            } \
        } \
        else \
        { \
            fprintf(stderr,"unpack failed(%d) %s:%d\n",resultcode,__FUNCTION__,__LINE__); fflush(stderr); \
        } \
    }\
    else \
    {\
        fprintf(stderr,"Not resp %s:%d\n",__FUNCTION__,__LINE__); fflush(stderr); \
        resultcode = eQCWWAN_ERR_INVALID_QMI_RSP;\
    }\
}

#define UNPACK_IND_AND_DUMPCOMP(resultcode,unpackfn,dumpfn,respmsg,resplen,argpt,nfield, ...) \
{\
    if(eIND==respmsg[0])\
    {\
        resultcode = unpackfn(respmsg, resplen, argpt); \
        if ( (eQCWWAN_ERR_NONE == resultcode) || \
           ( (eQCWWAN_ERR_QMI_OFFSET == resultcode)) )\
        {\
            dumpfn(argpt);\
            swi_uint256_print_mask((argpt)->ParamPresenceMask);\
            if (nfield > 0) \
            { \
                RESP_CMP_N_STRUCT_MEMBERS(nfield,__VA_ARGS__)\
            } \
            else \
            { \
                fprintf(stderr,"nfields not valid(%d) %s:%d\n",resultcode,__FUNCTION__,__LINE__); fflush(stderr); \
            } \
        }\
        else \
        { \
            fprintf(stderr,"unpack failed(%d) %s:%d\n",resultcode,__FUNCTION__,__LINE__); fflush(stderr); \
        } \
    } \
    else \
    {\
        fprintf(stderr,"Not indication %s:%d\n",__FUNCTION__,__LINE__); fflush(stderr); \
        resultcode = eQCWWAN_ERR_INVALID_QMI_RSP;\
    }\
}

/* unpack static indication, compare response and run dump function */
#define UNPACK_STATIC_IND_AND_DUMPCOMP(resultcode,unpackfn,dumpfn,respmsg,resplen,constunpack) \
{\
    if(eIND==respmsg[0])\
    {\
        unpackfn##_t unpackfn##var; \
        memset(&unpackfn##var,0,sizeof(unpackfn##var));\
        resultcode = unpackfn(respmsg, resplen, &unpackfn##var); \
        if ( (eQCWWAN_ERR_NONE == resultcode) || \
           ( (eQCWWAN_ERR_QMI_OFFSET == resultcode)) )\
        {\
            dumpfn(&unpackfn##var);\
            swi_uint256_print_mask((&unpackfn##var)->ParamPresenceMask);\
            if( memcmp(&unpackfn##var,constunpack,sizeof(unpackfn##_t)) != 0) \
            { \
                fprintf(stderr,"memcmp cmp not equal(%d) %s:%d\n",resultcode,__FUNCTION__,__LINE__); \
                fflush(stderr); \
            } \
        } \
        else \
        { \
            fprintf(stderr,"unpack failed(%d) %s:%d\n",resultcode,__FUNCTION__,__LINE__); fflush(stderr); \
        } \
    }\
    else \
    {\
        fprintf(stderr,"Not indication %s:%d\n",__FUNCTION__,__LINE__); fflush(stderr); \
        resultcode = eQCWWAN_ERR_INVALID_QMI_RSP;\
    }\
}

#define UNPACK_RESP_AND_DUMPCOMP_NO_MASK(resultcode,unpackfn,dumpfn,respmsg,resplen,argpt,nfield, ...) \
{\
    if(eRSP==respmsg[0])\
    {\
        resultcode = unpackfn(respmsg, resplen, argpt); \
        if (eQCWWAN_ERR_NONE == resultcode) \
        {\
            dumpfn(argpt);\
            if (nfield > 0) \
            { \
                RESP_CMP_N_STRUCT_MEMBERS(nfield,__VA_ARGS__)\
            } \
            else \
            { \
                fprintf(stderr,"nfields not valid(%d) %s:%d\n",resultcode,__FUNCTION__,__LINE__); fflush(stderr); \
            } \
        }\
        else \
        { \
            fprintf(stderr,"unpack failed(%d) %s:%d\n",resultcode,__FUNCTION__,__LINE__); fflush(stderr); \
        } \
    } \
    else \
    {\
        fprintf(stderr,"Not resp %s:%d\n",__FUNCTION__,__LINE__); fflush(stderr); \
        resultcode = eQCWWAN_ERR_INVALID_QMI_RSP;\
    }\
}


#endif
