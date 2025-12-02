	.section ".text"
_trap:
	pushq   %rax
	pushq   %rdi
	pushq   %rsi
	pushq   %rdx
	pushq   %rcx
	pushq   %r8
	pushq   %r9
	pushq   %r10
	pushq   %r11
	pushq   %r12
	pushq   %r13
	pushq   %r14
	pushq   %r15
	pushq   %rbx
	pushq   %rbp

	movq    %rsp, %rdi
	call    trap

	popq    %rbp
	popq    %rbx
	popq    %r15
	popq    %r14
	popq    %r13
	popq    %r12
	popq    %r11
	popq    %r10
	popq    %r9
	popq    %r8
	popq    %rcx
	popq    %rdx
	popq    %rsi
	popq    %rdi
	popq    %rax

	addq    $16, %rsp
	iretq


__trap_vector_0:
	pushq   $0
	pushq   $0
	jmp     _trap
__trap_vector_1:
	pushq   $0
	pushq   $1
	jmp     _trap
__trap_vector_2:
	pushq   $0
	pushq   $2
	jmp     _trap
__trap_vector_3:
	pushq   $0
	pushq   $3
	jmp     _trap
__trap_vector_4:
	pushq   $0
	pushq   $4
	jmp     _trap
__trap_vector_5:
	pushq   $0
	pushq   $5
	jmp     _trap
__trap_vector_6:
	pushq   $0
	pushq   $6
	jmp     _trap
__trap_vector_7:
	pushq   $0
	pushq   $7
	jmp     _trap
__trap_vector_8:
	pushq   $8
	jmp     _trap
__trap_vector_9:
	pushq   $0
	pushq   $9
	jmp     _trap
__trap_vector_10:
	pushq   $10
	jmp     _trap
__trap_vector_11:
	pushq   $11
	jmp     _trap
__trap_vector_12:
	pushq   $12
	jmp     _trap
__trap_vector_13:
	pushq   $13
	jmp     _trap
__trap_vector_14:
	pushq   $14
	jmp     _trap
__trap_vector_15:
	pushq   $0
	pushq   $15
	jmp     _trap
__trap_vector_16:
	pushq   $0
	pushq   $16
	jmp     _trap
__trap_vector_17:
	pushq   $17
	jmp     _trap
__trap_vector_18:
	pushq   $0
	pushq   $18
	jmp     _trap
__trap_vector_19:
	pushq   $0
	pushq   $19
	jmp     _trap
__trap_vector_20:
	pushq   $0
	pushq   $20
	jmp     _trap
__trap_vector_21:
	pushq   $0
	pushq   $21
	jmp     _trap
__trap_vector_22:
	pushq   $0
	pushq   $22
	jmp     _trap
__trap_vector_23:
	pushq   $0
	pushq   $23
	jmp     _trap
__trap_vector_24:
	pushq   $0
	pushq   $24
	jmp     _trap
__trap_vector_25:
	pushq   $0
	pushq   $25
	jmp     _trap
__trap_vector_26:
	pushq   $0
	pushq   $26
	jmp     _trap
__trap_vector_27:
	pushq   $0
	pushq   $27
	jmp     _trap
__trap_vector_28:
	pushq   $0
	pushq   $28
	jmp     _trap
__trap_vector_29:
	pushq   $0
	pushq   $29
	jmp     _trap
__trap_vector_30:
	pushq   $0
	pushq   $30
	jmp     _trap
__trap_vector_31:
	pushq   $0
	pushq   $31
	jmp     _trap
__trap_vector_32:
	pushq   $0
	pushq   $32
	jmp     _trap
__trap_vector_33:
	pushq   $0
	pushq   $33
	jmp     _trap
__trap_vector_34:
	pushq   $0
	pushq   $34
	jmp     _trap
__trap_vector_35:
	pushq   $0
	pushq   $35
	jmp     _trap
__trap_vector_36:
	pushq   $0
	pushq   $36
	jmp     _trap
__trap_vector_37:
	pushq   $0
	pushq   $37
	jmp     _trap
__trap_vector_38:
	pushq   $0
	pushq   $38
	jmp     _trap
__trap_vector_39:
	pushq   $0
	pushq   $39
	jmp     _trap
__trap_vector_40:
	pushq   $0
	pushq   $40
	jmp     _trap
__trap_vector_41:
	pushq   $0
	pushq   $41
	jmp     _trap
__trap_vector_42:
	pushq   $0
	pushq   $42
	jmp     _trap
__trap_vector_43:
	pushq   $0
	pushq   $43
	jmp     _trap
__trap_vector_44:
	pushq   $0
	pushq   $44
	jmp     _trap
__trap_vector_45:
	pushq   $0
	pushq   $45
	jmp     _trap
__trap_vector_46:
	pushq   $0
	pushq   $46
	jmp     _trap
__trap_vector_47:
	pushq   $0
	pushq   $47
	jmp     _trap
__trap_vector_48:
	pushq   $0
	pushq   $48
	jmp     _trap
__trap_vector_49:
	pushq   $0
	pushq   $49
	jmp     _trap
__trap_vector_50:
	pushq   $0
	pushq   $50
	jmp     _trap
__trap_vector_51:
	pushq   $0
	pushq   $51
	jmp     _trap
__trap_vector_52:
	pushq   $0
	pushq   $52
	jmp     _trap
__trap_vector_53:
	pushq   $0
	pushq   $53
	jmp     _trap
__trap_vector_54:
	pushq   $0
	pushq   $54
	jmp     _trap
__trap_vector_55:
	pushq   $0
	pushq   $55
	jmp     _trap
__trap_vector_56:
	pushq   $0
	pushq   $56
	jmp     _trap
__trap_vector_57:
	pushq   $0
	pushq   $57
	jmp     _trap
__trap_vector_58:
	pushq   $0
	pushq   $58
	jmp     _trap
__trap_vector_59:
	pushq   $0
	pushq   $59
	jmp     _trap
__trap_vector_60:
	pushq   $0
	pushq   $60
	jmp     _trap
__trap_vector_61:
	pushq   $0
	pushq   $61
	jmp     _trap
__trap_vector_62:
	pushq   $0
	pushq   $62
	jmp     _trap
__trap_vector_63:
	pushq   $0
	pushq   $63
	jmp     _trap
__trap_vector_64:
	pushq   $0
	pushq   $64
	jmp     _trap
__trap_vector_65:
	pushq   $0
	pushq   $65
	jmp     _trap
__trap_vector_66:
	pushq   $0
	pushq   $66
	jmp     _trap
__trap_vector_67:
	pushq   $0
	pushq   $67
	jmp     _trap
__trap_vector_68:
	pushq   $0
	pushq   $68
	jmp     _trap
__trap_vector_69:
	pushq   $0
	pushq   $69
	jmp     _trap
__trap_vector_70:
	pushq   $0
	pushq   $70
	jmp     _trap
__trap_vector_71:
	pushq   $0
	pushq   $71
	jmp     _trap
__trap_vector_72:
	pushq   $0
	pushq   $72
	jmp     _trap
__trap_vector_73:
	pushq   $0
	pushq   $73
	jmp     _trap
__trap_vector_74:
	pushq   $0
	pushq   $74
	jmp     _trap
__trap_vector_75:
	pushq   $0
	pushq   $75
	jmp     _trap
__trap_vector_76:
	pushq   $0
	pushq   $76
	jmp     _trap
__trap_vector_77:
	pushq   $0
	pushq   $77
	jmp     _trap
__trap_vector_78:
	pushq   $0
	pushq   $78
	jmp     _trap
__trap_vector_79:
	pushq   $0
	pushq   $79
	jmp     _trap
__trap_vector_80:
	pushq   $0
	pushq   $80
	jmp     _trap
__trap_vector_81:
	pushq   $0
	pushq   $81
	jmp     _trap
__trap_vector_82:
	pushq   $0
	pushq   $82
	jmp     _trap
__trap_vector_83:
	pushq   $0
	pushq   $83
	jmp     _trap
__trap_vector_84:
	pushq   $0
	pushq   $84
	jmp     _trap
__trap_vector_85:
	pushq   $0
	pushq   $85
	jmp     _trap
__trap_vector_86:
	pushq   $0
	pushq   $86
	jmp     _trap
__trap_vector_87:
	pushq   $0
	pushq   $87
	jmp     _trap
__trap_vector_88:
	pushq   $0
	pushq   $88
	jmp     _trap
__trap_vector_89:
	pushq   $0
	pushq   $89
	jmp     _trap
__trap_vector_90:
	pushq   $0
	pushq   $90
	jmp     _trap
__trap_vector_91:
	pushq   $0
	pushq   $91
	jmp     _trap
__trap_vector_92:
	pushq   $0
	pushq   $92
	jmp     _trap
__trap_vector_93:
	pushq   $0
	pushq   $93
	jmp     _trap
__trap_vector_94:
	pushq   $0
	pushq   $94
	jmp     _trap
__trap_vector_95:
	pushq   $0
	pushq   $95
	jmp     _trap
__trap_vector_96:
	pushq   $0
	pushq   $96
	jmp     _trap
__trap_vector_97:
	pushq   $0
	pushq   $97
	jmp     _trap
__trap_vector_98:
	pushq   $0
	pushq   $98
	jmp     _trap
__trap_vector_99:
	pushq   $0
	pushq   $99
	jmp     _trap
__trap_vector_100:
	pushq   $0
	pushq   $100
	jmp     _trap
__trap_vector_101:
	pushq   $0
	pushq   $101
	jmp     _trap
__trap_vector_102:
	pushq   $0
	pushq   $102
	jmp     _trap
__trap_vector_103:
	pushq   $0
	pushq   $103
	jmp     _trap
__trap_vector_104:
	pushq   $0
	pushq   $104
	jmp     _trap
__trap_vector_105:
	pushq   $0
	pushq   $105
	jmp     _trap
__trap_vector_106:
	pushq   $0
	pushq   $106
	jmp     _trap
__trap_vector_107:
	pushq   $0
	pushq   $107
	jmp     _trap
__trap_vector_108:
	pushq   $0
	pushq   $108
	jmp     _trap
__trap_vector_109:
	pushq   $0
	pushq   $109
	jmp     _trap
__trap_vector_110:
	pushq   $0
	pushq   $110
	jmp     _trap
__trap_vector_111:
	pushq   $0
	pushq   $111
	jmp     _trap
__trap_vector_112:
	pushq   $0
	pushq   $112
	jmp     _trap
__trap_vector_113:
	pushq   $0
	pushq   $113
	jmp     _trap
__trap_vector_114:
	pushq   $0
	pushq   $114
	jmp     _trap
__trap_vector_115:
	pushq   $0
	pushq   $115
	jmp     _trap
__trap_vector_116:
	pushq   $0
	pushq   $116
	jmp     _trap
__trap_vector_117:
	pushq   $0
	pushq   $117
	jmp     _trap
__trap_vector_118:
	pushq   $0
	pushq   $118
	jmp     _trap
__trap_vector_119:
	pushq   $0
	pushq   $119
	jmp     _trap
__trap_vector_120:
	pushq   $0
	pushq   $120
	jmp     _trap
__trap_vector_121:
	pushq   $0
	pushq   $121
	jmp     _trap
__trap_vector_122:
	pushq   $0
	pushq   $122
	jmp     _trap
__trap_vector_123:
	pushq   $0
	pushq   $123
	jmp     _trap
__trap_vector_124:
	pushq   $0
	pushq   $124
	jmp     _trap
__trap_vector_125:
	pushq   $0
	pushq   $125
	jmp     _trap
__trap_vector_126:
	pushq   $0
	pushq   $126
	jmp     _trap
__trap_vector_127:
	pushq   $0
	pushq   $127
	jmp     _trap
__trap_vector_128:
	pushq   $0
	pushq   $128
	jmp     _trap
__trap_vector_129:
	pushq   $0
	pushq   $129
	jmp     _trap
__trap_vector_130:
	pushq   $0
	pushq   $130
	jmp     _trap
__trap_vector_131:
	pushq   $0
	pushq   $131
	jmp     _trap
__trap_vector_132:
	pushq   $0
	pushq   $132
	jmp     _trap
__trap_vector_133:
	pushq   $0
	pushq   $133
	jmp     _trap
__trap_vector_134:
	pushq   $0
	pushq   $134
	jmp     _trap
__trap_vector_135:
	pushq   $0
	pushq   $135
	jmp     _trap
__trap_vector_136:
	pushq   $0
	pushq   $136
	jmp     _trap
__trap_vector_137:
	pushq   $0
	pushq   $137
	jmp     _trap
__trap_vector_138:
	pushq   $0
	pushq   $138
	jmp     _trap
__trap_vector_139:
	pushq   $0
	pushq   $139
	jmp     _trap
__trap_vector_140:
	pushq   $0
	pushq   $140
	jmp     _trap
__trap_vector_141:
	pushq   $0
	pushq   $141
	jmp     _trap
__trap_vector_142:
	pushq   $0
	pushq   $142
	jmp     _trap
__trap_vector_143:
	pushq   $0
	pushq   $143
	jmp     _trap
__trap_vector_144:
	pushq   $0
	pushq   $144
	jmp     _trap
__trap_vector_145:
	pushq   $0
	pushq   $145
	jmp     _trap
__trap_vector_146:
	pushq   $0
	pushq   $146
	jmp     _trap
__trap_vector_147:
	pushq   $0
	pushq   $147
	jmp     _trap
__trap_vector_148:
	pushq   $0
	pushq   $148
	jmp     _trap
__trap_vector_149:
	pushq   $0
	pushq   $149
	jmp     _trap
__trap_vector_150:
	pushq   $0
	pushq   $150
	jmp     _trap
__trap_vector_151:
	pushq   $0
	pushq   $151
	jmp     _trap
__trap_vector_152:
	pushq   $0
	pushq   $152
	jmp     _trap
__trap_vector_153:
	pushq   $0
	pushq   $153
	jmp     _trap
__trap_vector_154:
	pushq   $0
	pushq   $154
	jmp     _trap
__trap_vector_155:
	pushq   $0
	pushq   $155
	jmp     _trap
__trap_vector_156:
	pushq   $0
	pushq   $156
	jmp     _trap
__trap_vector_157:
	pushq   $0
	pushq   $157
	jmp     _trap
__trap_vector_158:
	pushq   $0
	pushq   $158
	jmp     _trap
__trap_vector_159:
	pushq   $0
	pushq   $159
	jmp     _trap
__trap_vector_160:
	pushq   $0
	pushq   $160
	jmp     _trap
__trap_vector_161:
	pushq   $0
	pushq   $161
	jmp     _trap
__trap_vector_162:
	pushq   $0
	pushq   $162
	jmp     _trap
__trap_vector_163:
	pushq   $0
	pushq   $163
	jmp     _trap
__trap_vector_164:
	pushq   $0
	pushq   $164
	jmp     _trap
__trap_vector_165:
	pushq   $0
	pushq   $165
	jmp     _trap
__trap_vector_166:
	pushq   $0
	pushq   $166
	jmp     _trap
__trap_vector_167:
	pushq   $0
	pushq   $167
	jmp     _trap
__trap_vector_168:
	pushq   $0
	pushq   $168
	jmp     _trap
__trap_vector_169:
	pushq   $0
	pushq   $169
	jmp     _trap
__trap_vector_170:
	pushq   $0
	pushq   $170
	jmp     _trap
__trap_vector_171:
	pushq   $0
	pushq   $171
	jmp     _trap
__trap_vector_172:
	pushq   $0
	pushq   $172
	jmp     _trap
__trap_vector_173:
	pushq   $0
	pushq   $173
	jmp     _trap
__trap_vector_174:
	pushq   $0
	pushq   $174
	jmp     _trap
__trap_vector_175:
	pushq   $0
	pushq   $175
	jmp     _trap
__trap_vector_176:
	pushq   $0
	pushq   $176
	jmp     _trap
__trap_vector_177:
	pushq   $0
	pushq   $177
	jmp     _trap
__trap_vector_178:
	pushq   $0
	pushq   $178
	jmp     _trap
__trap_vector_179:
	pushq   $0
	pushq   $179
	jmp     _trap
__trap_vector_180:
	pushq   $0
	pushq   $180
	jmp     _trap
__trap_vector_181:
	pushq   $0
	pushq   $181
	jmp     _trap
__trap_vector_182:
	pushq   $0
	pushq   $182
	jmp     _trap
__trap_vector_183:
	pushq   $0
	pushq   $183
	jmp     _trap
__trap_vector_184:
	pushq   $0
	pushq   $184
	jmp     _trap
__trap_vector_185:
	pushq   $0
	pushq   $185
	jmp     _trap
__trap_vector_186:
	pushq   $0
	pushq   $186
	jmp     _trap
__trap_vector_187:
	pushq   $0
	pushq   $187
	jmp     _trap
__trap_vector_188:
	pushq   $0
	pushq   $188
	jmp     _trap
__trap_vector_189:
	pushq   $0
	pushq   $189
	jmp     _trap
__trap_vector_190:
	pushq   $0
	pushq   $190
	jmp     _trap
__trap_vector_191:
	pushq   $0
	pushq   $191
	jmp     _trap
__trap_vector_192:
	pushq   $0
	pushq   $192
	jmp     _trap
__trap_vector_193:
	pushq   $0
	pushq   $193
	jmp     _trap
__trap_vector_194:
	pushq   $0
	pushq   $194
	jmp     _trap
__trap_vector_195:
	pushq   $0
	pushq   $195
	jmp     _trap
__trap_vector_196:
	pushq   $0
	pushq   $196
	jmp     _trap
__trap_vector_197:
	pushq   $0
	pushq   $197
	jmp     _trap
__trap_vector_198:
	pushq   $0
	pushq   $198
	jmp     _trap
__trap_vector_199:
	pushq   $0
	pushq   $199
	jmp     _trap
__trap_vector_200:
	pushq   $0
	pushq   $200
	jmp     _trap
__trap_vector_201:
	pushq   $0
	pushq   $201
	jmp     _trap
__trap_vector_202:
	pushq   $0
	pushq   $202
	jmp     _trap
__trap_vector_203:
	pushq   $0
	pushq   $203
	jmp     _trap
__trap_vector_204:
	pushq   $0
	pushq   $204
	jmp     _trap
__trap_vector_205:
	pushq   $0
	pushq   $205
	jmp     _trap
__trap_vector_206:
	pushq   $0
	pushq   $206
	jmp     _trap
__trap_vector_207:
	pushq   $0
	pushq   $207
	jmp     _trap
__trap_vector_208:
	pushq   $0
	pushq   $208
	jmp     _trap
__trap_vector_209:
	pushq   $0
	pushq   $209
	jmp     _trap
__trap_vector_210:
	pushq   $0
	pushq   $210
	jmp     _trap
__trap_vector_211:
	pushq   $0
	pushq   $211
	jmp     _trap
__trap_vector_212:
	pushq   $0
	pushq   $212
	jmp     _trap
__trap_vector_213:
	pushq   $0
	pushq   $213
	jmp     _trap
__trap_vector_214:
	pushq   $0
	pushq   $214
	jmp     _trap
__trap_vector_215:
	pushq   $0
	pushq   $215
	jmp     _trap
__trap_vector_216:
	pushq   $0
	pushq   $216
	jmp     _trap
__trap_vector_217:
	pushq   $0
	pushq   $217
	jmp     _trap
__trap_vector_218:
	pushq   $0
	pushq   $218
	jmp     _trap
__trap_vector_219:
	pushq   $0
	pushq   $219
	jmp     _trap
__trap_vector_220:
	pushq   $0
	pushq   $220
	jmp     _trap
__trap_vector_221:
	pushq   $0
	pushq   $221
	jmp     _trap
__trap_vector_222:
	pushq   $0
	pushq   $222
	jmp     _trap
__trap_vector_223:
	pushq   $0
	pushq   $223
	jmp     _trap
__trap_vector_224:
	pushq   $0
	pushq   $224
	jmp     _trap
__trap_vector_225:
	pushq   $0
	pushq   $225
	jmp     _trap
__trap_vector_226:
	pushq   $0
	pushq   $226
	jmp     _trap
__trap_vector_227:
	pushq   $0
	pushq   $227
	jmp     _trap
__trap_vector_228:
	pushq   $0
	pushq   $228
	jmp     _trap
__trap_vector_229:
	pushq   $0
	pushq   $229
	jmp     _trap
__trap_vector_230:
	pushq   $0
	pushq   $230
	jmp     _trap
__trap_vector_231:
	pushq   $0
	pushq   $231
	jmp     _trap
__trap_vector_232:
	pushq   $0
	pushq   $232
	jmp     _trap
__trap_vector_233:
	pushq   $0
	pushq   $233
	jmp     _trap
__trap_vector_234:
	pushq   $0
	pushq   $234
	jmp     _trap
__trap_vector_235:
	pushq   $0
	pushq   $235
	jmp     _trap
__trap_vector_236:
	pushq   $0
	pushq   $236
	jmp     _trap
__trap_vector_237:
	pushq   $0
	pushq   $237
	jmp     _trap
__trap_vector_238:
	pushq   $0
	pushq   $238
	jmp     _trap
__trap_vector_239:
	pushq   $0
	pushq   $239
	jmp     _trap
__trap_vector_240:
	pushq   $0
	pushq   $240
	jmp     _trap
__trap_vector_241:
	pushq   $0
	pushq   $241
	jmp     _trap
__trap_vector_242:
	pushq   $0
	pushq   $242
	jmp     _trap
__trap_vector_243:
	pushq   $0
	pushq   $243
	jmp     _trap
__trap_vector_244:
	pushq   $0
	pushq   $244
	jmp     _trap
__trap_vector_245:
	pushq   $0
	pushq   $245
	jmp     _trap
__trap_vector_246:
	pushq   $0
	pushq   $246
	jmp     _trap
__trap_vector_247:
	pushq   $0
	pushq   $247
	jmp     _trap
__trap_vector_248:
	pushq   $0
	pushq   $248
	jmp     _trap
__trap_vector_249:
	pushq   $0
	pushq   $249
	jmp     _trap
__trap_vector_250:
	pushq   $0
	pushq   $250
	jmp     _trap
__trap_vector_251:
	pushq   $0
	pushq   $251
	jmp     _trap
__trap_vector_252:
	pushq   $0
	pushq   $252
	jmp     _trap
__trap_vector_253:
	pushq   $0
	pushq   $253
	jmp     _trap
__trap_vector_254:
	pushq   $0
	pushq   $254
	jmp     _trap
__trap_vector_255:
	pushq   $0
	pushq   $255
	jmp     _trap


	.section ".data"
	.globl  trap_vector
trap_vector:
	.quad   __trap_vector_0
	.quad   __trap_vector_1
	.quad   __trap_vector_2
	.quad   __trap_vector_3
	.quad   __trap_vector_4
	.quad   __trap_vector_5
	.quad   __trap_vector_6
	.quad   __trap_vector_7
	.quad   __trap_vector_8
	.quad   __trap_vector_9
	.quad   __trap_vector_10
	.quad   __trap_vector_11
	.quad   __trap_vector_12
	.quad   __trap_vector_13
	.quad   __trap_vector_14
	.quad   __trap_vector_15
	.quad   __trap_vector_16
	.quad   __trap_vector_17
	.quad   __trap_vector_18
	.quad   __trap_vector_19
	.quad   __trap_vector_20
	.quad   __trap_vector_21
	.quad   __trap_vector_22
	.quad   __trap_vector_23
	.quad   __trap_vector_24
	.quad   __trap_vector_25
	.quad   __trap_vector_26
	.quad   __trap_vector_27
	.quad   __trap_vector_28
	.quad   __trap_vector_29
	.quad   __trap_vector_30
	.quad   __trap_vector_31
	.quad   __trap_vector_32
	.quad   __trap_vector_33
	.quad   __trap_vector_34
	.quad   __trap_vector_35
	.quad   __trap_vector_36
	.quad   __trap_vector_37
	.quad   __trap_vector_38
	.quad   __trap_vector_39
	.quad   __trap_vector_40
	.quad   __trap_vector_41
	.quad   __trap_vector_42
	.quad   __trap_vector_43
	.quad   __trap_vector_44
	.quad   __trap_vector_45
	.quad   __trap_vector_46
	.quad   __trap_vector_47
	.quad   __trap_vector_48
	.quad   __trap_vector_49
	.quad   __trap_vector_50
	.quad   __trap_vector_51
	.quad   __trap_vector_52
	.quad   __trap_vector_53
	.quad   __trap_vector_54
	.quad   __trap_vector_55
	.quad   __trap_vector_56
	.quad   __trap_vector_57
	.quad   __trap_vector_58
	.quad   __trap_vector_59
	.quad   __trap_vector_60
	.quad   __trap_vector_61
	.quad   __trap_vector_62
	.quad   __trap_vector_63
	.quad   __trap_vector_64
	.quad   __trap_vector_65
	.quad   __trap_vector_66
	.quad   __trap_vector_67
	.quad   __trap_vector_68
	.quad   __trap_vector_69
	.quad   __trap_vector_70
	.quad   __trap_vector_71
	.quad   __trap_vector_72
	.quad   __trap_vector_73
	.quad   __trap_vector_74
	.quad   __trap_vector_75
	.quad   __trap_vector_76
	.quad   __trap_vector_77
	.quad   __trap_vector_78
	.quad   __trap_vector_79
	.quad   __trap_vector_80
	.quad   __trap_vector_81
	.quad   __trap_vector_82
	.quad   __trap_vector_83
	.quad   __trap_vector_84
	.quad   __trap_vector_85
	.quad   __trap_vector_86
	.quad   __trap_vector_87
	.quad   __trap_vector_88
	.quad   __trap_vector_89
	.quad   __trap_vector_90
	.quad   __trap_vector_91
	.quad   __trap_vector_92
	.quad   __trap_vector_93
	.quad   __trap_vector_94
	.quad   __trap_vector_95
	.quad   __trap_vector_96
	.quad   __trap_vector_97
	.quad   __trap_vector_98
	.quad   __trap_vector_99
	.quad   __trap_vector_100
	.quad   __trap_vector_101
	.quad   __trap_vector_102
	.quad   __trap_vector_103
	.quad   __trap_vector_104
	.quad   __trap_vector_105
	.quad   __trap_vector_106
	.quad   __trap_vector_107
	.quad   __trap_vector_108
	.quad   __trap_vector_109
	.quad   __trap_vector_110
	.quad   __trap_vector_111
	.quad   __trap_vector_112
	.quad   __trap_vector_113
	.quad   __trap_vector_114
	.quad   __trap_vector_115
	.quad   __trap_vector_116
	.quad   __trap_vector_117
	.quad   __trap_vector_118
	.quad   __trap_vector_119
	.quad   __trap_vector_120
	.quad   __trap_vector_121
	.quad   __trap_vector_122
	.quad   __trap_vector_123
	.quad   __trap_vector_124
	.quad   __trap_vector_125
	.quad   __trap_vector_126
	.quad   __trap_vector_127
	.quad   __trap_vector_128
	.quad   __trap_vector_129
	.quad   __trap_vector_130
	.quad   __trap_vector_131
	.quad   __trap_vector_132
	.quad   __trap_vector_133
	.quad   __trap_vector_134
	.quad   __trap_vector_135
	.quad   __trap_vector_136
	.quad   __trap_vector_137
	.quad   __trap_vector_138
	.quad   __trap_vector_139
	.quad   __trap_vector_140
	.quad   __trap_vector_141
	.quad   __trap_vector_142
	.quad   __trap_vector_143
	.quad   __trap_vector_144
	.quad   __trap_vector_145
	.quad   __trap_vector_146
	.quad   __trap_vector_147
	.quad   __trap_vector_148
	.quad   __trap_vector_149
	.quad   __trap_vector_150
	.quad   __trap_vector_151
	.quad   __trap_vector_152
	.quad   __trap_vector_153
	.quad   __trap_vector_154
	.quad   __trap_vector_155
	.quad   __trap_vector_156
	.quad   __trap_vector_157
	.quad   __trap_vector_158
	.quad   __trap_vector_159
	.quad   __trap_vector_160
	.quad   __trap_vector_161
	.quad   __trap_vector_162
	.quad   __trap_vector_163
	.quad   __trap_vector_164
	.quad   __trap_vector_165
	.quad   __trap_vector_166
	.quad   __trap_vector_167
	.quad   __trap_vector_168
	.quad   __trap_vector_169
	.quad   __trap_vector_170
	.quad   __trap_vector_171
	.quad   __trap_vector_172
	.quad   __trap_vector_173
	.quad   __trap_vector_174
	.quad   __trap_vector_175
	.quad   __trap_vector_176
	.quad   __trap_vector_177
	.quad   __trap_vector_178
	.quad   __trap_vector_179
	.quad   __trap_vector_180
	.quad   __trap_vector_181
	.quad   __trap_vector_182
	.quad   __trap_vector_183
	.quad   __trap_vector_184
	.quad   __trap_vector_185
	.quad   __trap_vector_186
	.quad   __trap_vector_187
	.quad   __trap_vector_188
	.quad   __trap_vector_189
	.quad   __trap_vector_190
	.quad   __trap_vector_191
	.quad   __trap_vector_192
	.quad   __trap_vector_193
	.quad   __trap_vector_194
	.quad   __trap_vector_195
	.quad   __trap_vector_196
	.quad   __trap_vector_197
	.quad   __trap_vector_198
	.quad   __trap_vector_199
	.quad   __trap_vector_200
	.quad   __trap_vector_201
	.quad   __trap_vector_202
	.quad   __trap_vector_203
	.quad   __trap_vector_204
	.quad   __trap_vector_205
	.quad   __trap_vector_206
	.quad   __trap_vector_207
	.quad   __trap_vector_208
	.quad   __trap_vector_209
	.quad   __trap_vector_210
	.quad   __trap_vector_211
	.quad   __trap_vector_212
	.quad   __trap_vector_213
	.quad   __trap_vector_214
	.quad   __trap_vector_215
	.quad   __trap_vector_216
	.quad   __trap_vector_217
	.quad   __trap_vector_218
	.quad   __trap_vector_219
	.quad   __trap_vector_220
	.quad   __trap_vector_221
	.quad   __trap_vector_222
	.quad   __trap_vector_223
	.quad   __trap_vector_224
	.quad   __trap_vector_225
	.quad   __trap_vector_226
	.quad   __trap_vector_227
	.quad   __trap_vector_228
	.quad   __trap_vector_229
	.quad   __trap_vector_230
	.quad   __trap_vector_231
	.quad   __trap_vector_232
	.quad   __trap_vector_233
	.quad   __trap_vector_234
	.quad   __trap_vector_235
	.quad   __trap_vector_236
	.quad   __trap_vector_237
	.quad   __trap_vector_238
	.quad   __trap_vector_239
	.quad   __trap_vector_240
	.quad   __trap_vector_241
	.quad   __trap_vector_242
	.quad   __trap_vector_243
	.quad   __trap_vector_244
	.quad   __trap_vector_245
	.quad   __trap_vector_246
	.quad   __trap_vector_247
	.quad   __trap_vector_248
	.quad   __trap_vector_249
	.quad   __trap_vector_250
	.quad   __trap_vector_251
	.quad   __trap_vector_252
	.quad   __trap_vector_253
	.quad   __trap_vector_254
	.quad   __trap_vector_255
