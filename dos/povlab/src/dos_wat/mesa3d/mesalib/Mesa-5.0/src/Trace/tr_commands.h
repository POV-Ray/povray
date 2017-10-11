/* This may look like C code, but it is really -*- C++ -*-  */
/* $Id: tr_commands.h,v 1.1 2000/11/11 01:42:30 brianp Exp $ */

/*
 * DebugGL
 * Version:  1.0
 * 
 * Copyright (C) 1999-2000  Bernd Kreimeier, Loki Entertainment
 * All Rights Reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#ifndef TR_COMMANDS_H
#define TR_COMMANDS_H


/**
 * Enumeration of GL commands, for metafile format 
 *  and networking protocol. Has to be re-indexed
 *  if (ever) used as GLX stream. This uses the
 *  Mesa internal values.
 */

#define CMD_NEWLIST                                            0
#define CMD_ENDLIST                                            1
#define CMD_CALLLIST                                           2
#define CMD_CALLLISTS                                          3
#define CMD_DELETELISTS                                        4
#define CMD_GENLISTS                                           5
#define CMD_LISTBASE                                           6
#define CMD_BEGIN                                              7
#define CMD_BITMAP                                             8 
#define CMD_COLOR3B                                            9
#define CMD_COLOR3BV                                          10
#define CMD_COLOR3D                                           11
#define CMD_COLOR3DV                                          12
#define CMD_COLOR3F                                           13 
#define CMD_COLOR3FV                                          14 
#define CMD_COLOR3I                                           15 
#define CMD_COLOR3IV                                          16 
#define CMD_COLOR3S                                           17 
#define CMD_COLOR3SV                                          18 
#define CMD_COLOR3UB                                          19 
#define CMD_COLOR3UBV                                         20 
#define CMD_COLOR3UI                                          21 
#define CMD_COLOR3UIV                                         22 
#define CMD_COLOR3US                                          23 
#define CMD_COLOR3USV                                         24 
#define CMD_COLOR4B                                           25 
#define CMD_COLOR4BV                                          26 
#define CMD_COLOR4D                                           27 
#define CMD_COLOR4DV                                          28 
#define CMD_COLOR4F                                           29 
#define CMD_COLOR4FV                                          30 
#define CMD_COLOR4I                                           31 
#define CMD_COLOR4IV                                          32 
#define CMD_COLOR4S                                           33 
#define CMD_COLOR4SV                                          34 
#define CMD_COLOR4UB                                          35 
#define CMD_COLOR4UBV                                         36 
#define CMD_COLOR4UI                                          37 
#define CMD_COLOR4UIV                                         38 
#define CMD_COLOR4US                                          39 
#define CMD_COLOR4USV                                         40 
#define CMD_EDGEFLAG                                          41 
#define CMD_EDGEFLAGV                                         42 
#define CMD_END                                               43 
#define CMD_INDEXD                                            44 
#define CMD_INDEXDV                                           45 
#define CMD_INDEXF                                            46 
#define CMD_INDEXFV                                           47 
#define CMD_INDEXI                                            48 
#define CMD_INDEXIV                                           49 
#define CMD_INDEXS                                            50 
#define CMD_INDEXSV                                           51 
#define CMD_NORMAL3B                                          52 
#define CMD_NORMAL3BV                                         53 
#define CMD_NORMAL3D                                          54 
#define CMD_NORMAL3DV                                         55 
#define CMD_NORMAL3F                                          56 
#define CMD_NORMAL3FV                                         57 
#define CMD_NORMAL3I                                          58 
#define CMD_NORMAL3IV                                         59 
#define CMD_NORMAL3S                                          60 
#define CMD_NORMAL3SV                                         61 
#define CMD_RASTERPOS2D                                       62 
#define CMD_RASTERPOS2DV                                      63 
#define CMD_RASTERPOS2F                                       64 
#define CMD_RASTERPOS2FV                                      65 
#define CMD_RASTERPOS2I                                       66 
#define CMD_RASTERPOS2IV                                      67 
#define CMD_RASTERPOS2S                                       68 
#define CMD_RASTERPOS2SV                                      69 
#define CMD_RASTERPOS3D                                       70 
#define CMD_RASTERPOS3DV                                      71 
#define CMD_RASTERPOS3F                                       72 
#define CMD_RASTERPOS3FV                                      73 
#define CMD_RASTERPOS3I                                       74 
#define CMD_RASTERPOS3IV                                      75 
#define CMD_RASTERPOS3S                                       76 
#define CMD_RASTERPOS3SV                                      77 
#define CMD_RASTERPOS4D                                       78 
#define CMD_RASTERPOS4DV                                      79 
#define CMD_RASTERPOS4F                                       80 
#define CMD_RASTERPOS4FV                                      81 
#define CMD_RASTERPOS4I                                       82 
#define CMD_RASTERPOS4IV                                      83 
#define CMD_RASTERPOS4S                                       84 
#define CMD_RASTERPOS4SV                                      85 
#define CMD_RECTD                                             86 
#define CMD_RECTDV                                            87 
#define CMD_RECTF                                             88 
#define CMD_RECTFV                                            89 
#define CMD_RECTI                                             90 
#define CMD_RECTIV                                            91 
#define CMD_RECTS                                             92 
#define CMD_RECTSV                                            93 
#define CMD_TEXCOORD1D                                        94 
#define CMD_TEXCOORD1DV                                       95 
#define CMD_TEXCOORD1F                                        96 
#define CMD_TEXCOORD1FV                                       97 
#define CMD_TEXCOORD1I                                        98 
#define CMD_TEXCOORD1IV                                       99 
#define CMD_TEXCOORD1S                                       100 
#define CMD_TEXCOORD1SV                                      101 
#define CMD_TEXCOORD2D                                       102 
#define CMD_TEXCOORD2DV                                      103 
#define CMD_TEXCOORD2F                                       104 
#define CMD_TEXCOORD2FV                                      105 
#define CMD_TEXCOORD2I                                       106 
#define CMD_TEXCOORD2IV                                      107 
#define CMD_TEXCOORD2S                                       108 
#define CMD_TEXCOORD2SV                                      109 
#define CMD_TEXCOORD3D                                       110 
#define CMD_TEXCOORD3DV                                      111 
#define CMD_TEXCOORD3F                                       112 
#define CMD_TEXCOORD3FV                                      113 
#define CMD_TEXCOORD3I                                       114 
#define CMD_TEXCOORD3IV                                      115 
#define CMD_TEXCOORD3S                                       116 
#define CMD_TEXCOORD3SV                                      117 
#define CMD_TEXCOORD4D                                       118 
#define CMD_TEXCOORD4DV                                      119 
#define CMD_TEXCOORD4F                                       120 
#define CMD_TEXCOORD4FV                                      121 
#define CMD_TEXCOORD4I                                       122 
#define CMD_TEXCOORD4IV                                      123 
#define CMD_TEXCOORD4S                                       124 
#define CMD_TEXCOORD4SV                                      125 
#define CMD_VERTEX2D                                         126 
#define CMD_VERTEX2DV                                        127 
#define CMD_VERTEX2F                                         128 
#define CMD_VERTEX2FV                                        129 
#define CMD_VERTEX2I                                         130 
#define CMD_VERTEX2IV                                        131 
#define CMD_VERTEX2S                                         132 
#define CMD_VERTEX2SV                                        133 
#define CMD_VERTEX3D                                         134 
#define CMD_VERTEX3DV                                        135 
#define CMD_VERTEX3F                                         136 
#define CMD_VERTEX3FV                                        137 
#define CMD_VERTEX3I                                         138 
#define CMD_VERTEX3IV                                        139 
#define CMD_VERTEX3S                                         140 
#define CMD_VERTEX3SV                                        141 
#define CMD_VERTEX4D                                         142 
#define CMD_VERTEX4DV                                        143 
#define CMD_VERTEX4F                                         144 
#define CMD_VERTEX4FV                                        145 
#define CMD_VERTEX4I                                         146 
#define CMD_VERTEX4IV                                        147 
#define CMD_VERTEX4S                                         148 
#define CMD_VERTEX4SV                                        149 
#define CMD_CLIPPLANE                                        150 
#define CMD_COLORMATERIAL                                    151 
#define CMD_CULLFACE                                         152 
#define CMD_FOGF                                             153 
#define CMD_FOGFV                                            154 
#define CMD_FOGI                                             155 
#define CMD_FOGIV                                            156 
#define CMD_FRONTFACE                                        157 
#define CMD_HINT                                             158 
#define CMD_LIGHTF                                           159 
#define CMD_LIGHTFV                                          160 
#define CMD_LIGHTI                                           161 
#define CMD_LIGHTIV                                          162 
#define CMD_LIGHTMODELF                                      163 
#define CMD_LIGHTMODELFV                                     164 
#define CMD_LIGHTMODELI                                      165 
#define CMD_LIGHTMODELIV                                     166 
#define CMD_LINESTIPPLE                                      167 
#define CMD_LINEWIDTH                                        168 
#define CMD_MATERIALF                                        169 
#define CMD_MATERIALFV                                       170 
#define CMD_MATERIALI                                        171 
#define CMD_MATERIALIV                                       172 
#define CMD_POINTSIZE                                        173 
#define CMD_POLYGONMODE                                      174 
#define CMD_POLYGONSTIPPLE                                   175 
#define CMD_SCISSOR                                          176 
#define CMD_SHADEMODEL                                       177 
#define CMD_TEXPARAMETERF                                    178 
#define CMD_TEXPARAMETERFV                                   179 
#define CMD_TEXPARAMETERI                                    180 
#define CMD_TEXPARAMETERIV                                   181 
#define CMD_TEXIMAGE1D                                       182 
#define CMD_TEXIMAGE2D                                       183 
#define CMD_TEXENVF                                          184 
#define CMD_TEXENVFV                                         185 
#define CMD_TEXENVI                                          186 
#define CMD_TEXENVIV                                         187 
#define CMD_TEXGEND                                          188 
#define CMD_TEXGENDV                                         189 
#define CMD_TEXGENF                                          190 
#define CMD_TEXGENFV                                         191 
#define CMD_TEXGENI                                          192 
#define CMD_TEXGENIV                                         193 
#define CMD_FEEDBACKBUFFER                                   194 
#define CMD_SELECTBUFFER                                     195 
#define CMD_RENDERMODE                                       196 
#define CMD_INITNAMES                                        197 
#define CMD_LOADNAME                                         198 
#define CMD_PASSTHROUGH                                      199 
#define CMD_POPNAME                                          200 
#define CMD_PUSHNAME                                         201 
#define CMD_DRAWBUFFER                                       202 
#define CMD_CLEAR                                            203 
#define CMD_CLEARACCUM                                       204 
#define CMD_CLEARINDEX                                       205 
#define CMD_CLEARCOLOR                                       206 
#define CMD_CLEARSTENCIL                                     207 
#define CMD_CLEARDEPTH                                       208 
#define CMD_STENCILMASK                                      209 
#define CMD_COLORMASK                                        210 
#define CMD_DEPTHMASK                                        211 
#define CMD_INDEXMASK                                        212 
#define CMD_ACCUM                                            213 
#define CMD_DISABLE                                          214 
#define CMD_ENABLE                                           215 
#define CMD_FINISH                                           216 
#define CMD_FLUSH                                            217 
#define CMD_POPATTRIB                                        218 
#define CMD_PUSHATTRIB                                       219 
#define CMD_MAP1D                                            220 
#define CMD_MAP1F                                            221 
#define CMD_MAP2D                                            222 
#define CMD_MAP2F                                            223 
#define CMD_MAPGRID1D                                        224 
#define CMD_MAPGRID1F                                        225 
#define CMD_MAPGRID2D                                        226 
#define CMD_MAPGRID2F                                        227 
#define CMD_EVALCOORD1D                                      228 
#define CMD_EVALCOORD1DV                                     229 
#define CMD_EVALCOORD1F                                      230 
#define CMD_EVALCOORD1FV                                     231 
#define CMD_EVALCOORD2D                                      232 
#define CMD_EVALCOORD2DV                                     233 
#define CMD_EVALCOORD2F                                      234 
#define CMD_EVALCOORD2FV                                     235 
#define CMD_EVALMESH1                                        236 
#define CMD_EVALPOINT1                                       237 
#define CMD_EVALMESH2                                        238 
#define CMD_EVALPOINT2                                       239 
#define CMD_ALPHAFUNC                                        240 
#define CMD_BLENDFUNC                                        241 
#define CMD_LOGICOP                                          242 
#define CMD_STENCILFUNC                                      243 
#define CMD_STENCILOP                                        244 
#define CMD_DEPTHFUNC                                        245 
#define CMD_PIXELZOOM                                        246 
#define CMD_PIXELTRANSFERF                                   247 
#define CMD_PIXELTRANSFERI                                   248 
#define CMD_PIXELSTOREF                                      249 
#define CMD_PIXELSTOREI                                      250 
#define CMD_PIXELMAPFV                                       251 
#define CMD_PIXELMAPUIV                                      252 
#define CMD_PIXELMAPUSV                                      253 
#define CMD_READBUFFER                                       254 
#define CMD_COPYPIXELS                                       255 
#define CMD_READPIXELS                                       256 
#define CMD_DRAWPIXELS                                       257 
#define CMD_GETBOOLEANV                                      258 
#define CMD_GETCLIPPLANE                                     259 
#define CMD_GETDOUBLEV                                       260 
#define CMD_GETERROR                                         261 
#define CMD_GETFLOATV                                        262 
#define CMD_GETINTEGERV                                      263 
#define CMD_GETLIGHTFV                                       264 
#define CMD_GETLIGHTIV                                       265 
#define CMD_GETMAPDV                                         266 
#define CMD_GETMAPFV                                         267 
#define CMD_GETMAPIV                                         268 
#define CMD_GETMATERIALFV                                    269 
#define CMD_GETMATERIALIV                                    270 
#define CMD_GETPIXELMAPFV                                    271 
#define CMD_GETPIXELMAPUIV                                   272 
#define CMD_GETPIXELMAPUSV                                   273 
#define CMD_GETPOLYGONSTIPPLE                                274 
#define CMD_GETSTRING                                        275 
#define CMD_GETTEXENVFV                                      276 
#define CMD_GETTEXENVIV                                      277 
#define CMD_GETTEXGENDV                                      278 
#define CMD_GETTEXGENFV                                      279 
#define CMD_GETTEXGENIV                                      280 
#define CMD_GETTEXIMAGE                                      281 
#define CMD_GETTEXPARAMETERFV                                282 
#define CMD_GETTEXPARAMETERIV                                283 
#define CMD_GETTEXLEVELPARAMETERFV                           284 
#define CMD_GETTEXLEVELPARAMETERIV                           285 
#define CMD_ISENABLED                                        286 
#define CMD_ISLIST                                           287 
#define CMD_DEPTHRANGE                                       288 
#define CMD_FRUSTUM                                          289 
#define CMD_LOADIDENTITY                                     290 
#define CMD_LOADMATRIXF                                      291 
#define CMD_LOADMATRIXD                                      292 
#define CMD_MATRIXMODE                                       293 
#define CMD_MULTMATRIXF                                      294 
#define CMD_MULTMATRIXD                                      295 
#define CMD_ORTHO                                            296 
#define CMD_POPMATRIX                                        297 
#define CMD_PUSHMATRIX                                       298 
#define CMD_ROTATED                                          299 
#define CMD_ROTATEF                                          300 
#define CMD_SCALED                                           301 
#define CMD_SCALEF                                           302 
#define CMD_TRANSLATED                                       303 
#define CMD_TRANSLATEF                                       304 
#define CMD_VIEWPORT                                         305 
#define CMD_ARRAYELEMENT                                     306 
#define CMD_BINDTEXTURE                                      307 
#define CMD_COLORPOINTER                                     308 
#define CMD_DISABLECLIENTSTATE                               309 
#define CMD_DRAWARRAYS                                       310 
#define CMD_DRAWELEMENTS                                     311 
#define CMD_EDGEFLAGPOINTER                                  312 
#define CMD_ENABLECLIENTSTATE                                313 
#define CMD_INDEXPOINTER                                     314 
#define CMD_INDEXUB                                          315 
#define CMD_INDEXUBV                                         316 
#define CMD_INTERLEAVEDARRAYS                                317 
#define CMD_NORMALPOINTER                                    318 
#define CMD_POLYGONOFFSET                                    319 
#define CMD_TEXCOORDPOINTER                                  320 
#define CMD_VERTEXPOINTER                                    321 
#define CMD_ARETEXTURESRESIDENT                              322 
#define CMD_COPYTEXIMAGE1D                                   323 
#define CMD_COPYTEXIMAGE2D                                   324 
#define CMD_COPYTEXSUBIMAGE1D                                325 
#define CMD_COPYTEXSUBIMAGE2D                                326 
#define CMD_DELETETEXTURES                                   327 
#define CMD_GENTEXTURES                                      328 
#define CMD_GETPOINTERV                                      329 
#define CMD_ISTEXTURE                                        330 
#define CMD_PRIORITIZETEXTURES                               331 
#define CMD_TEXSUBIMAGE1D                                    332 
#define CMD_TEXSUBIMAGE2D                                    333 
#define CMD_POPCLIENTATTRIB                                  334 
#define CMD_PUSHCLIENTATTRIB                                 335 
#define CMD_BLENDCOLOR                                       336 
#define CMD_BLENDEQUATION                                    337 
#define CMD_DRAWRANGEELEMENTS                                338 
#define CMD_COLORTABLE                                       339 
#define CMD_COLORTABLEPARAMETERFV                            340 
#define CMD_COLORTABLEPARAMETERIV                            341 
#define CMD_COPYCOLORTABLE                                   342 
#define CMD_GETCOLORTABLE                                    343 
#define CMD_GETCOLORTABLEPARAMETERFV                         344 
#define CMD_GETCOLORTABLEPARAMETERIV                         345 
#define CMD_COLORSUBTABLE                                    346 
#define CMD_COPYCOLORSUBTABLE                                347 
#define CMD_CONVOLUTIONFILTER1D                              348 
#define CMD_CONVOLUTIONFILTER2D                              349 
#define CMD_CONVOLUTIONPARAMETERF                            350 
#define CMD_CONVOLUTIONPARAMETERFV                           351 
#define CMD_CONVOLUTIONPARAMETERI                            352 
#define CMD_CONVOLUTIONPARAMETERIV                           353 
#define CMD_COPYCONVOLUTIONFILTER1D                          354 
#define CMD_COPYCONVOLUTIONFILTER2D                          355 
#define CMD_GETCONVOLUTIONFILTER                             356 
#define CMD_GETCONVOLUTIONPARAMETERFV                        357 
#define CMD_GETCONVOLUTIONPARAMETERIV                        358 
#define CMD_GETSEPARABLEFILTER                               359 
#define CMD_SEPARABLEFILTER2D                                360 
#define CMD_GETHISTOGRAM                                     361 
#define CMD_GETHISTOGRAMPARAMETERFV                          362 
#define CMD_GETHISTOGRAMPARAMETERIV                          363 
#define CMD_GETMINMAX                                        364 
#define CMD_GETMINMAXPARAMETERFV                             365 
#define CMD_GETMINMAXPARAMETERIV                             366 
#define CMD_HISTOGRAM                                        367 
#define CMD_MINMAX                                           368 
#define CMD_RESETHISTOGRAM                                   369 
#define CMD_RESETMINMAX                                      370 
#define CMD_TEXIMAGE3D                                       371 
#define CMD_TEXSUBIMAGE3D                                    372 
#define CMD_COPYTEXSUBIMAGE3D                                373 
#define CMD_ACTIVETEXTUREARB                                 374 
#define CMD_CLIENTACTIVETEXTUREARB                           375 
#define CMD_MULTITEXCOORD1DARB                               376 
#define CMD_MULTITEXCOORD1DVARB                              377 
#define CMD_MULTITEXCOORD1FARB                               378 
#define CMD_MULTITEXCOORD1FVARB                              379 
#define CMD_MULTITEXCOORD1IARB                               380 
#define CMD_MULTITEXCOORD1IVARB                              381 
#define CMD_MULTITEXCOORD1SARB                               382 
#define CMD_MULTITEXCOORD1SVARB                              383 
#define CMD_MULTITEXCOORD2DARB                               384 
#define CMD_MULTITEXCOORD2DVARB                              385 
#define CMD_MULTITEXCOORD2FARB                               386 
#define CMD_MULTITEXCOORD2FVARB                              387 
#define CMD_MULTITEXCOORD2IARB                               388 
#define CMD_MULTITEXCOORD2IVARB                              389 
#define CMD_MULTITEXCOORD2SARB                               390 
#define CMD_MULTITEXCOORD2SVARB                              391 
#define CMD_MULTITEXCOORD3DARB                               392 
#define CMD_MULTITEXCOORD3DVARB                              393 
#define CMD_MULTITEXCOORD3FARB                               394 
#define CMD_MULTITEXCOORD3FVARB                              395 
#define CMD_MULTITEXCOORD3IARB                               396 
#define CMD_MULTITEXCOORD3IVARB                              397 
#define CMD_MULTITEXCOORD3SARB                               398 
#define CMD_MULTITEXCOORD3SVARB                              399 
#define CMD_MULTITEXCOORD4DARB                               400 
#define CMD_MULTITEXCOORD4DVARB                              401 
#define CMD_MULTITEXCOORD4FARB                               402 
#define CMD_MULTITEXCOORD4FVARB                              403 
#define CMD_MULTITEXCOORD4IARB                               404 
#define CMD_MULTITEXCOORD4IVARB                              405 
#define CMD_MULTITEXCOORD4SARB                               406 
#define CMD_MULTITEXCOORD4SVARB                              407 
#define CMD_LOADTRANSPOSEMATRIXFARB                          408 
#define CMD_LOADTRANSPOSEMATRIXDARB                          409 
#define CMD_MULTTRANSPOSEMATRIXFARB                          410 
#define CMD_MULTTRANSPOSEMATRIXDARB                          411 
#define CMD_SAMPLECOVERAGEARB                                412 
#define CMD_SAMPLEPASSARB                                    413 
#define CMD_POLYGONOFFSETEXT                                 414 
#define CMD_GETTEXFILTERFUNCSGIS                             415 
#define CMD_TEXFILTERFUNCSGIS                                416 
#define CMD_GETHISTOGRAMEXT                                  417 
#define CMD_GETHISTOGRAMPARAMETERFVEXT                       418 
#define CMD_GETHISTOGRAMPARAMETERIVEXT                       419 
#define CMD_GETMINMAXEXT                                     420 
#define CMD_GETMINMAXPARAMETERFVEXT                          421 
#define CMD_GETMINMAXPARAMETERIVEXT                          422 
#define CMD_GETCONVOLUTIONFILTEREXT                          423 
#define CMD_GETCONVOLUTIONPARAMETERFVEXT                     424 
#define CMD_GETCONVOLUTIONPARAMETERIVEXT                     425 
#define CMD_GETSEPARABLEFILTEREXT                            426 
#define CMD_GETCOLORTABLESGI                                 427 
#define CMD_GETCOLORTABLEPARAMETERFVSGI                      428 
#define CMD_GETCOLORTABLEPARAMETERIVSGI                      429 
#define CMD_PIXELTEXGENSGIX                                  430 
#define CMD_PIXELTEXGENPARAMETERISGIS                        431 
#define CMD_PIXELTEXGENPARAMETERIVSGIS                       432 
#define CMD_PIXELTEXGENPARAMETERFSGIS                        433 
#define CMD_PIXELTEXGENPARAMETERFVSGIS                       434 
#define CMD_GETPIXELTexGenPARAMETERIVSGIS                    435 
#define CMD_GETPIXELTexGenPARAMETERFVSGIS                    436 
#define CMD_TEXIMAGE4DSGIS                                   437 
#define CMD_TEXSUBIMAGE4DSGIS                                438 
#define CMD_ARETEXTURESRESIDENTEXT                           439 
#define CMD_GENTEXTURESEXT                                   440 
#define CMD_ISTEXTUREEXT                                     441 
#define CMD_DETAILTEXFUNCSGIS                                442 
#define CMD_GETDETAILTEXFUNCSGIS                             443 
#define CMD_SHARPENTEXFUNCSGIS                               444 
#define CMD_GETSHARPENTEXFUNCSGIS                            445 
#define CMD_SAMPLEMASKSGIS                                   446 
#define CMD_SAMPLEPATTERNSGIS                                447 
#define CMD_COLORPOINTEREXT                                  448 
#define CMD_EDGEFLAGPOINTEREXT                               449 
#define CMD_INDEXPOINTEREXT                                  450 
#define CMD_NORMALPOINTEREXT                                 451 
#define CMD_TEXCOORDPOINTEREXT                               452 
#define CMD_VERTEXPOINTEREXT                                 453 
#define CMD_SPRITEPARAMETERFSGIX                             454 
#define CMD_SPRITEPARAMETERFVSGIX                            455 
#define CMD_SPRITEPARAMETERISGIX                             456 
#define CMD_SPRITEPARAMETERIVSGIX                            457 
#define CMD_POINTPARAMETERFEXT                               458 
#define CMD_POINTPARAMETERFVEXT                              459 
#define CMD_GETINSTRUMENTSSGIX                               460 
#define CMD_INSTRUMENTSBUFFERSGIX                            461 
#define CMD_POLLINSTRUMENTSSGIX                              462 
#define CMD_READINSTRUMENTSSGIX                              463 
#define CMD_STARTINSTRUMENTSSGIX                             464 
#define CMD_STOPINSTRUMENTSSGIX                              465 
#define CMD_FRAMEZOOMSGIX                                    466 
#define CMD_TAGSAMPLEBUFFERSGIX                              467 
#define CMD_REFERENCEPLANESGIX                               468 
#define CMD_FLUSHRASTERSGIX                                  469 
#define CMD_GETLISTPARAMETERFVSGIX                           470 
#define CMD_GETLISTPARAMETERIVSGIX                           471 
#define CMD_LISTPARAMETERFSGIX                               472 
#define CMD_LISTPARAMETERFVSGIX                              473 
#define CMD_LISTPARAMETERISGIX                               474 
#define CMD_LISTPARAMETERIVSGIX                              475 
#define CMD_FRAGMENTCOLORMATERIALSGIX                        476 
#define CMD_FRAGMENTLIGHTFSGIX                               477 
#define CMD_FRAGMENTLIGHTFVSGIX                              478 
#define CMD_FRAGMENTLIGHTISGIX                               479 
#define CMD_FRAGMENTLIGHTIVSGIX                              480 
#define CMD_FRAGMENTLIGHTMODELFSGIX                          481 
#define CMD_FRAGMENTLIGHTMODELFVSGIX                         482 
#define CMD_FRAGMENTLIGHTMODELISGIX                          483 
#define CMD_FRAGMENTLIGHTMODELIVSGIX                         484 
#define CMD_FRAGMENTMATERIALFSGIX                            485 
#define CMD_FRAGMENTMATERIALFVSGIX                           486 
#define CMD_FRAGMENTMATERIALISGIX                            487 
#define CMD_FRAGMENTMATERIALIVSGIX                           488 
#define CMD_GETFRAGMENTLIGHTFVSGIX                           489 
#define CMD_GETFRAGMENTLIGHTIVSGIX                           490 
#define CMD_GETFRAGMENTMATERIALFVSGIX                        491 
#define CMD_GETFRAGMENTMATERIALIVSGIX                        492 
#define CMD_LIGHTENVISGIX                                    493 
#define CMD_VERTEXWEIGHTFEXT                                 494 
#define CMD_VERTEXWEIGHTFVEXT                                495 
#define CMD_VERTEXWEIGHTPOINTEREXT                           496 
#define CMD_FLUSHVERTEXARRAYRANGENV                          497 
#define CMD_VERTEXARRAYRANGENV                               498 
#define CMD_COMBINERPARAMETERFVNV                            499 
#define CMD_COMBINERPARAMETERFNV                             500 
#define CMD_COMBINERPARAMETERIVNV                            501 
#define CMD_COMBINERPARAMETERINV                             502 
#define CMD_COMBINERINPUTNV                                  503 
#define CMD_COMBINEROUTPUTNV                                 504 
#define CMD_FINALCOMBINERINPUTNV                             505 
#define CMD_GETCOMBINERINPUTPARAMETERFVNV                    506 
#define CMD_GETCOMBINERINPUTPARAMETERIVNV                    507 
#define CMD_GETCOMBINEROUTPUTPARAMETERFVNV                   508 
#define CMD_GETCOMBINEROUTPUTPARAMETERIVNV                   509 
#define CMD_GETFINALCOMBINERINPUTPARAMETERFVNV               510 
#define CMD_GETFINALCOMBINERINPUTPARAMETERIVNV               511 
#define CMD_RESIZEBUFFERSMESA                                512 
#define CMD_WINDOWPOS2DMESA                                  513 
#define CMD_WINDOWPOS2DVMESA                                 514 
#define CMD_WINDOWPOS2FMESA                                  515 
#define CMD_WINDOWPOS2FVMESA                                 516 
#define CMD_WINDOWPOS2IMESA                                  517 
#define CMD_WINDOWPOS2IVMESA                                 518 
#define CMD_WINDOWPOS2SMESA                                  519 
#define CMD_WINDOWPOS2SVMESA                                 520 
#define CMD_WINDOWPOS3DMESA                                  521 
#define CMD_WINDOWPOS3DVMESA                                 522 
#define CMD_WINDOWPOS3FMESA                                  523 
#define CMD_WINDOWPOS3FVMESA                                 524 
#define CMD_WINDOWPOS3IMESA                                  525 
#define CMD_WINDOWPOS3IVMESA                                 526 
#define CMD_WINDOWPOS3SMESA                                  527 
#define CMD_WINDOWPOS3SVMESA                                 528 
#define CMD_WINDOWPOS4DMESA                                  529 
#define CMD_WINDOWPOS4DVMESA                                 530 
#define CMD_WINDOWPOS4FMESA                                  531 
#define CMD_WINDOWPOS4FVMESA                                 532 
#define CMD_WINDOWPOS4IMESA                                  533 
#define CMD_WINDOWPOS4IVMESA                                 534 
#define CMD_WINDOWPOS4SMESA                                  535 
#define CMD_WINDOWPOS4SVMESA                                 536 
#define CMD_BLENDFUNCSEPARATEEXT                             537 
#define CMD_INDEXMATERIALEXT                                 538 
#define CMD_INDEXFUNCEXT                                     539 
#define CMD_LOCKARRAYSEXT                                    540 
#define CMD_UNLOCKARRAYSEXT                                  541 
#define CMD_CULLPARAMETERDVEXT                               542 
#define CMD_CULLPARAMETERFVEXT                               543 
#define CMD_HINTPGI                                          544 
#define CMD_FOGCOORDFEXT                                     545 
#define CMD_FOGCOORDFVEXT                                    546 
#define CMD_FOGCOORDDEXT                                     547 
#define CMD_FOGCOORDDVEXT                                    548 
#define CMD_FOGCOORDPOINTEREXT                               549 
#define CMD_GETCOLORTABLEEXT                                 550 
#define CMD_GETCOLORTABLEPARAMETERIVEXT                      551 
#define CMD_GETCOLORTABLEPARAMETERFVEXT                      552         


/* FIXME: Trace Extension itself - not yet defined. */
#define CMD_NEWTRACEMESA                                     666
#define CMD_ENDTRACEMESA                                     667
#define CMD_TRACECOMMENTMESA                                 668



/**
 * Enumeration of non-GL entries in trace stream.
 * This is (static) variable and "various" data,
 *  some of which optional. All these values are
 *  negative.
 */
#define VAR_VERSION                                           -1 /* Metafile version. Mandatory. */
#define VAR_CYCLES                                            -2 /* Profiling, CPU cycles. Optional. */
#define VAR_CONTEXT                                           -3 /* Context ID. Optional. */    
#define VAR_THREAD                                            -4 /* Thread ID. Optional. */
#define VAR_STRING                                            -5 /* Internally generated. Optional. */

#define VAR_COLORELEMENT                                      -6
#define VAR_EDGEFLAGELEMENT                                   -7
#define VAR_INDEXELEMENT                                      -8
#define VAR_NORMALELEMENT                                     -9
#define VAR_TEXCOORDELEMENT                                   -10
#define VAR_VERTEXELEMENT                                     -11 /* Dereferenced Vertex Array Data. */


/* FIXME: more VAR_ as needed. */
/*
 * Mindbender - The following two vars are needed to surround the queries
 *              performed when the trace context is in a half initialized
 *              state.
 */
#define VAR_OOBBEGIN                                          -14
#define VAR_OOBEND                                            -15

/*
 * Mindbender - These are needed when the pointer changes through one of the
 *              XXXPointer calls.  Change these as needed.
 */
#define VAR_COLORPOINTER                                      -8
#define VAR_EDGEFLAGPOINTER                                   -9
#define VAR_INDEXPOINTER                                      -10
#define VAR_NORMALPOINTER                                     -11
#define VAR_TEXCOORDPOINTER                                   -12
#define VAR_VERTEXPOINTER                                     -13


#endif
