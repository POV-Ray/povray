/* ---------------------------------------------------------------------------
*  DONNEES2.C
*
*
*  from POVLAB 3D Modeller
*  Copyright 1994-1999 POVLAB Authors.
*  ---------------------------------------------------------------------------
*  NOTICE: This source code file is provided so that users may experiment
*  with enhancements to POVLAB and to port the software to platforms other
*  than those supported by the POVLAB authors. There are strict rules under
*  which you are permitted to use this file. The rules are in the file
*  named LEGAL.TXT which should be distributed with this file.
*  If LEGAL.TXT is not available or for more info please contact the POVLAB
*  primary author by leaving a message on http://www.povlab.org
*  The latest and official version of POVLAB may be found at this site.
*
*  POVLAB was originally written by Denis Olivier.
*
*  ---------------------------------------------------------------------------*/
#include <FLOAT.H>
#include <MATH.H>
#include <STDIO.H>
#include "GLIB.H"
#include "LIB.H"
#include "GLOBAL.H"

// ----------------------------------------------------------------------- */
// --------- LECTURE DES DONNEES D'UN PLAN ------------------------------- */
// ----------------------------------------------------------------------- */
int data_plan(byte Type,int NumeroObjet,byte Vue) {
  register int NbPoint;

  if (Type==CUBE_R) {              // cube affichage rapide
    charge_xyz( 0,-1, 1, 1,1,-1);
    charge_xyz( 1, 1, 1, 1,0,-1);
    charge_xyz( 2, 1, 1,-1,0,-1);
    charge_xyz( 3,-1, 1,-1,0,-1);
    charge_xyz( 4,-1, 1, 1,0,-1);
    charge_xyz( 5,-1,-1, 1,0,-1);
    charge_xyz( 6,-1,-1,-1,0,-1);
    charge_xyz( 7, 1,-1,-1,0,-1);
    charge_xyz( 8, 1,-1, 1,0,-1);
    charge_xyz( 9,-1,-1, 1,0,-1);
    charge_xyz(10,-1, 1,-1,1,-1);
    charge_xyz(11,-1,-1,-1,0,-1);
    charge_xyz(12, 1, 1,-1,1,-1);
    charge_xyz(13, 1,-1,-1,0,-1);
    charge_xyz(14, 1, 1, 1,1,-1);
    charge_xyz(15, 1,-1, 1,0,-1);
    return 15;
  }

  if (Type==PRISME) {               // PRISME
    charge_xyz( 0,-1,-1, 0,1,-1);
    charge_xyz( 1,-1,+1, 1,0,-1);
    charge_xyz( 2,-1,+1,-1,0,-1);
    charge_xyz( 3,-1,-1, 0,0,-1);
    charge_xyz( 4, 1,-1, 0,1,-1);
    charge_xyz( 5, 1,+1, 1,0,-1);
    charge_xyz( 6, 1,+1,-1,0,-1);
    charge_xyz( 7, 1,-1, 0,0,-1);
    charge_xyz( 8,-1,-1, 0,1,-1);
    charge_xyz( 9, 1,-1, 0,0,-1);
    charge_xyz(10,-1,+1,-1,1,-1);
    charge_xyz(11, 1,+1,-1,0,-1);
    charge_xyz(12,-1,+1, 1,1,-1);
    charge_xyz(13, 1,+1, 1,0,-1);
    return 13;
  }

  if (Type==PYRAMIDE) {               // PYRAMIDE
    charge_xyz( 0,-1,+1,-1,1,-1);
    charge_xyz( 1, 1,+1,-1,0,-1);
    charge_xyz( 2, 1,+1, 1,0,-1);
    charge_xyz( 3,-1,+1, 1,0,-1);
    charge_xyz( 4,-1,+1,-1,0,-1);
    charge_xyz( 5, 0,-1, 0,1,-1);
    charge_xyz( 6,-1,+1,-1,0,-1);
    charge_xyz( 7, 0,-1, 0,1,-1);
    charge_xyz( 8, 1,+1,-1,0,-1);
    charge_xyz( 9, 0,-1, 0,1,-1);
    charge_xyz(10, 1,+1, 1,0,-1);
    charge_xyz(11, 0,-1, 0,1,-1);
    charge_xyz(12,-1,+1, 1,0,-1);
    return 12;
  }

  if (Type==CARREZ) { // CARREZ
    charge_xyz(0,-1, 1,0,1,-1);
    charge_xyz(1, 1, 1,0,0,-1);
    charge_xyz(2, 1,-1,0,0,-1);
    charge_xyz(3,-1,-1,0,0,-1);
    charge_xyz(4,-1, 1,0,0,-1);
    charge_xyz(5,-0.2, 0  ,0,1,-1); // croix 2D centr‚e
    charge_xyz(6, 0.2, 0  ,0,0,-1);
    charge_xyz(7, 0  ,-0.2,0,1,-1);
    charge_xyz(8, 0  , 0.2,0,0,-1);
    return 8;
  }

  if (Type==CARREY) { // CARREY
    charge_xyz(0,-1,0, 1,1,-1);
    charge_xyz(1, 1,0, 1,0,-1);
    charge_xyz(2, 1,0,-1,0,-1);
    charge_xyz(3,-1,0,-1,0,-1);
    charge_xyz(4,-1,0, 1,0,-1);
    charge_xyz(5,-0.2,0, 0  ,1,-1); // croix 2D centr‚e
    charge_xyz(6, 0.2,0, 0  ,0,-1);
    charge_xyz(7, 0  ,0,-0.2,1,-1);
    charge_xyz(8, 0  ,0, 0.2,0,-1);
    return 8;
  }

  if (Type==CARREX) { // CARREX
    charge_xyz(0,0,-1, 1,1,-1);
    charge_xyz(1,0, 1, 1,0,-1);
    charge_xyz(2,0, 1,-1,0,-1);
    charge_xyz(3,0,-1,-1,0,-1);
    charge_xyz(4,0,-1, 1,0,-1);
    charge_xyz(5, 0  ,-0.2, 0  ,1,-1); // croix 2D centr‚e
    charge_xyz(6, 0  , 0.2, 0  ,0,-1);
    charge_xyz(7, 0  , 0  ,-0.2,1,-1);
    charge_xyz(8, 0  , 0  , 0.2,0,-1);
    return 8;
  }

  if (Type==PLANY) { // PLAN sur Y
    charge_xyz( 0,-1.00,0,-1,1,-1); charge_xyz( 1,-1.00,0,1,0,-1);
    charge_xyz( 2,-0.90,0,-1,1,-1); charge_xyz( 3,-0.90,0,1,0,-1);
    charge_xyz( 4,-0.80,0,-1,1,-1); charge_xyz( 5,-0.80,0,1,0,-1);
    charge_xyz( 6,-0.70,0,-1,1,-1); charge_xyz( 7,-0.70,0,1,0,-1);
    charge_xyz( 8,-0.60,0,-1,1,-1); charge_xyz( 9,-0.60,0,1,0,-1);
    charge_xyz(10,-0.50,0,-1,1,-1); charge_xyz(11,-0.50,0,1,0,-1);
    charge_xyz(12,-0.40,0,-1,1,-1); charge_xyz(13,-0.40,0,1,0,-1);
    charge_xyz(14,-0.30,0,-1,1,-1); charge_xyz(15,-0.30,0,1,0,-1);
    charge_xyz(16,-0.20,0,-1,1,-1); charge_xyz(17,-0.20,0,1,0,-1);
    charge_xyz(18,-0.10,0,-1,1,-1); charge_xyz(19,-0.10,0,1,0,-1);
    charge_xyz(20, 0.00,0,-1,1,-1); charge_xyz(21, 0.00,0,1,0,-1);
    charge_xyz(22, 0.10,0,-1,1,-1); charge_xyz(23, 0.10,0,1,0,-1);
    charge_xyz(24, 0.20,0,-1,1,-1); charge_xyz(25, 0.20,0,1,0,-1);
    charge_xyz(26, 0.30,0,-1,1,-1); charge_xyz(27, 0.30,0,1,0,-1);
    charge_xyz(28, 0.40,0,-1,1,-1); charge_xyz(29, 0.40,0,1,0,-1);
    charge_xyz(30, 0.50,0,-1,1,-1); charge_xyz(31, 0.50,0,1,0,-1);
    charge_xyz(32, 0.60,0,-1,1,-1); charge_xyz(33, 0.60,0,1,0,-1);
    charge_xyz(34, 0.70,0,-1,1,-1); charge_xyz(35, 0.70,0,1,0,-1);
    charge_xyz(36, 0.80,0,-1,1,-1); charge_xyz(37, 0.80,0,1,0,-1);
    charge_xyz(38, 0.90,0,-1,1,-1); charge_xyz(39, 0.90,0,1,0,-1);
    charge_xyz(40, 1.00,0,-1,1,-1); charge_xyz(41, 1.00,0,1,0,-1);

    charge_xyz(42,-1,0,-1.00,1,-1); charge_xyz(43,1,0,-1.00,0,-1);
    charge_xyz(44,-1,0,-0.90,1,-1); charge_xyz(45,1,0,-0.90,0,-1);
    charge_xyz(46,-1,0,-0.80,1,-1); charge_xyz(47,1,0,-0.80,0,-1);
    charge_xyz(48,-1,0,-0.70,1,-1); charge_xyz(49,1,0,-0.70,0,-1);
    charge_xyz(50,-1,0,-0.60,1,-1); charge_xyz(51,1,0,-0.60,0,-1);
    charge_xyz(52,-1,0,-0.50,1,-1); charge_xyz(53,1,0,-0.50,0,-1);
    charge_xyz(54,-1,0,-0.40,1,-1); charge_xyz(55,1,0,-0.40,0,-1);
    charge_xyz(56,-1,0,-0.30,1,-1); charge_xyz(57,1,0,-0.30,0,-1);
    charge_xyz(58,-1,0,-0.20,1,-1); charge_xyz(59,1,0,-0.20,0,-1);
    charge_xyz(60,-1,0,-0.10,1,-1); charge_xyz(61,1,0,-0.10,0,-1);
    charge_xyz(62,-1,0, 0.00,1,-1); charge_xyz(63,1,0, 0.00,0,-1);
    charge_xyz(64,-1,0, 0.10,1,-1); charge_xyz(65,1,0, 0.10,0,-1);
    charge_xyz(66,-1,0, 0.20,1,-1); charge_xyz(67,1,0, 0.20,0,-1);
    charge_xyz(68,-1,0, 0.30,1,-1); charge_xyz(69,1,0, 0.30,0,-1);
    charge_xyz(70,-1,0, 0.40,1,-1); charge_xyz(71,1,0, 0.40,0,-1);
    charge_xyz(72,-1,0, 0.50,1,-1); charge_xyz(73,1,0, 0.50,0,-1);
    charge_xyz(74,-1,0, 0.60,1,-1); charge_xyz(75,1,0, 0.60,0,-1);
    charge_xyz(76,-1,0, 0.70,1,-1); charge_xyz(77,1,0, 0.70,0,-1);
    charge_xyz(78,-1,0, 0.80,1,-1); charge_xyz(79,1,0, 0.80,0,-1);
    charge_xyz(80,-1,0, 0.90,1,-1); charge_xyz(81,1,0, 0.90,0,-1);
    charge_xyz(82,-1,0, 1.00,1,-1); charge_xyz(83,1,0, 1.00,0,-1);
    return 83;
  }

  if (Type==PLANX) { // PLAN s   ur X
    charge_xyz( 0,0,-1.00,-1,1,-1); charge_xyz( 1,0,-1.00,1,0,-1);
    charge_xyz( 2,0,-0.90,-1,1,-1); charge_xyz( 3,0,-0.90,1,0,-1);
    charge_xyz( 4,0,-0.80,-1,1,-1); charge_xyz( 5,0,-0.80,1,0,-1);
    charge_xyz( 6,0,-0.70,-1,1,-1); charge_xyz( 7,0,-0.70,1,0,-1);
    charge_xyz( 8,0,-0.60,-1,1,-1); charge_xyz( 9,0,-0.60,1,0,-1);
    charge_xyz(10,0,-0.50,-1,1,-1); charge_xyz(11,0,-0.50,1,0,-1);
    charge_xyz(12,0,-0.40,-1,1,-1); charge_xyz(13,0,-0.40,1,0,-1);
    charge_xyz(14,0,-0.30,-1,1,-1); charge_xyz(15,0,-0.30,1,0,-1);
    charge_xyz(16,0,-0.20,-1,1,-1); charge_xyz(17,0,-0.20,1,0,-1);
    charge_xyz(18,0,-0.10,-1,1,-1); charge_xyz(19,0,-0.10,1,0,-1);
    charge_xyz(20,0, 0.00,-1,1,-1); charge_xyz(21,0, 0.00,1,0,-1);
    charge_xyz(22,0, 0.10,-1,1,-1); charge_xyz(23,0, 0.10,1,0,-1);
    charge_xyz(24,0, 0.20,-1,1,-1); charge_xyz(25,0, 0.20,1,0,-1);
    charge_xyz(26,0, 0.30,-1,1,-1); charge_xyz(27,0, 0.30,1,0,-1);
    charge_xyz(28,0, 0.40,-1,1,-1); charge_xyz(29,0, 0.40,1,0,-1);
    charge_xyz(30,0, 0.50,-1,1,-1); charge_xyz(31,0, 0.50,1,0,-1);
    charge_xyz(32,0, 0.60,-1,1,-1); charge_xyz(33,0, 0.60,1,0,-1);
    charge_xyz(34,0, 0.70,-1,1,-1); charge_xyz(35,0, 0.70,1,0,-1);
    charge_xyz(36,0, 0.80,-1,1,-1); charge_xyz(37,0, 0.80,1,0,-1);
    charge_xyz(38,0, 0.90,-1,1,-1); charge_xyz(39,0, 0.90,1,0,-1);
    charge_xyz(40,0, 1.00,-1,1,-1); charge_xyz(41,0, 1.00,1,0,-1);

    charge_xyz(42,0,-1,-1.00,1,-1); charge_xyz(43,0,1,-1.00,0,-1);
    charge_xyz(44,0,-1,-0.90,1,-1); charge_xyz(45,0,1,-0.90,0,-1);
    charge_xyz(46,0,-1,-0.80,1,-1); charge_xyz(47,0,1,-0.80,0,-1);
    charge_xyz(48,0,-1,-0.70,1,-1); charge_xyz(49,0,1,-0.70,0,-1);
    charge_xyz(50,0,-1,-0.60,1,-1); charge_xyz(51,0,1,-0.60,0,-1);
    charge_xyz(52,0,-1,-0.50,1,-1); charge_xyz(53,0,1,-0.50,0,-1);
    charge_xyz(54,0,-1,-0.40,1,-1); charge_xyz(55,0,1,-0.40,0,-1);
    charge_xyz(56,0,-1,-0.30,1,-1); charge_xyz(57,0,1,-0.30,0,-1);
    charge_xyz(58,0,-1,-0.20,1,-1); charge_xyz(59,0,1,-0.20,0,-1);
    charge_xyz(60,0,-1,-0.10,1,-1); charge_xyz(61,0,1,-0.10,0,-1);
    charge_xyz(62,0,-1, 0.00,1,-1); charge_xyz(63,0,1, 0.00,0,-1);
    charge_xyz(64,0,-1, 0.10,1,-1); charge_xyz(65,0,1, 0.10,0,-1);
    charge_xyz(66,0,-1, 0.20,1,-1); charge_xyz(67,0,1, 0.20,0,-1);
    charge_xyz(68,0,-1, 0.30,1,-1); charge_xyz(69,0,1, 0.30,0,-1);
    charge_xyz(70,0,-1, 0.40,1,-1); charge_xyz(71,0,1, 0.40,0,-1);
    charge_xyz(72,0,-1, 0.50,1,-1); charge_xyz(73,0,1, 0.50,0,-1);
    charge_xyz(74,0,-1, 0.60,1,-1); charge_xyz(75,0,1, 0.60,0,-1);
    charge_xyz(76,0,-1, 0.70,1,-1); charge_xyz(77,0,1, 0.70,0,-1);
    charge_xyz(78,0,-1, 0.80,1,-1); charge_xyz(79,0,1, 0.80,0,-1);
    charge_xyz(80,0,-1, 0.90,1,-1); charge_xyz(81,0,1, 0.90,0,-1);
    charge_xyz(82,0,-1, 1.00,1,-1); charge_xyz(83,0,1, 1.00,0,-1);
    return 83;
  }

  if (Type==PLANZ) { // PLAN sur Z
    charge_xyz( 0,-1.00,-1,0,1,-1); charge_xyz( 1,-1.00,1,0,0,-1);
    charge_xyz( 2,-0.90,-1,0,1,-1); charge_xyz( 3,-0.90,1,0,0,-1);
    charge_xyz( 4,-0.80,-1,0,1,-1); charge_xyz( 5,-0.80,1,0,0,-1);
    charge_xyz( 6,-0.70,-1,0,1,-1); charge_xyz( 7,-0.70,1,0,0,-1);
    charge_xyz( 8,-0.60,-1,0,1,-1); charge_xyz( 9,-0.60,1,0,0,-1);
    charge_xyz(10,-0.50,-1,0,1,-1); charge_xyz(11,-0.50,1,0,0,-1);
    charge_xyz(12,-0.40,-1,0,1,-1); charge_xyz(13,-0.40,1,0,0,-1);
    charge_xyz(14,-0.30,-1,0,1,-1); charge_xyz(15,-0.30,1,0,0,-1);
    charge_xyz(16,-0.20,-1,0,1,-1); charge_xyz(17,-0.20,1,0,0,-1);
    charge_xyz(18,-0.10,-1,0,1,-1); charge_xyz(19,-0.10,1,0,0,-1);
    charge_xyz(20, 0.00,-1,0,1,-1); charge_xyz(21, 0.00,1,0,0,-1);
    charge_xyz(22, 0.10,-1,0,1,-1); charge_xyz(23, 0.10,1,0,0,-1);
    charge_xyz(24, 0.20,-1,0,1,-1); charge_xyz(25, 0.20,1,0,0,-1);
    charge_xyz(26, 0.30,-1,0,1,-1); charge_xyz(27, 0.30,1,0,0,-1);
    charge_xyz(28, 0.40,-1,0,1,-1); charge_xyz(29, 0.40,1,0,0,-1);
    charge_xyz(30, 0.50,-1,0,1,-1); charge_xyz(31, 0.50,1,0,0,-1);
    charge_xyz(32, 0.60,-1,0,1,-1); charge_xyz(33, 0.60,1,0,0,-1);
    charge_xyz(34, 0.70,-1,0,1,-1); charge_xyz(35, 0.70,1,0,0,-1);
    charge_xyz(36, 0.80,-1,0,1,-1); charge_xyz(37, 0.80,1,0,0,-1);
    charge_xyz(38, 0.90,-1,0,1,-1); charge_xyz(39, 0.90,1,0,0,-1);
    charge_xyz(40, 1.00,-1,0,1,-1); charge_xyz(41, 1.00,1,0,0,-1);

    charge_xyz(42,-1,-1.00,0,1,-1); charge_xyz(43,1,-1.00,0,0,-1);
    charge_xyz(44,-1,-0.90,0,1,-1); charge_xyz(45,1,-0.90,0,0,-1);
    charge_xyz(46,-1,-0.80,0,1,-1); charge_xyz(47,1,-0.80,0,0,-1);
    charge_xyz(48,-1,-0.70,0,1,-1); charge_xyz(49,1,-0.70,0,0,-1);
    charge_xyz(50,-1,-0.60,0,1,-1); charge_xyz(51,1,-0.60,0,0,-1);
    charge_xyz(52,-1,-0.50,0,1,-1); charge_xyz(53,1,-0.50,0,0,-1);
    charge_xyz(54,-1,-0.40,0,1,-1); charge_xyz(55,1,-0.40,0,0,-1);
    charge_xyz(56,-1,-0.30,0,1,-1); charge_xyz(57,1,-0.30,0,0,-1);
    charge_xyz(58,-1,-0.20,0,1,-1); charge_xyz(59,1,-0.20,0,0,-1);
    charge_xyz(60,-1,-0.10,0,1,-1); charge_xyz(61,1,-0.10,0,0,-1);
    charge_xyz(62,-1, 0.00,0,1,-1); charge_xyz(63,1, 0.00,0,0,-1);
    charge_xyz(64,-1, 0.10,0,1,-1); charge_xyz(65,1, 0.10,0,0,-1);
    charge_xyz(66,-1, 0.20,0,1,-1); charge_xyz(67,1, 0.20,0,0,-1);
    charge_xyz(68,-1, 0.30,0,1,-1); charge_xyz(69,1, 0.30,0,0,-1);
    charge_xyz(70,-1, 0.40,0,1,-1); charge_xyz(71,1, 0.40,0,0,-1);
    charge_xyz(72,-1, 0.50,0,1,-1); charge_xyz(73,1, 0.50,0,0,-1);
    charge_xyz(74,-1, 0.60,0,1,-1); charge_xyz(75,1, 0.60,0,0,-1);
    charge_xyz(76,-1, 0.70,0,1,-1); charge_xyz(77,1, 0.70,0,0,-1);
    charge_xyz(78,-1, 0.80,0,1,-1); charge_xyz(79,1, 0.80,0,0,-1);
    charge_xyz(80,-1, 0.90,0,1,-1); charge_xyz(81,1, 0.90,0,0,-1);
    charge_xyz(82,-1, 1.00,0,1,-1); charge_xyz(83,1, 1.00,0,0,-1);
    return 83;
  }

  if (Type==CUBE || Type==SUPEREL) {               // cube
    charge_xyz( 0,-1, 1, 1,1,-1);
    charge_xyz( 1, 1, 1, 1,0,-1);
    charge_xyz( 2, 1, 1,-1,0,-1);
    charge_xyz( 3,-1, 1,-1,0,-1);
    charge_xyz( 4,-1, 1, 1,0,-1);
    charge_xyz( 5,-1,-1, 1,0,-1);
    charge_xyz( 6,-1,-1,-1,0,-1);
    charge_xyz( 7, 1,-1,-1,0,-1);
    charge_xyz( 8, 1,-1, 1,0,-1);
    charge_xyz( 9,-1,-1, 1,0,-1);
    charge_xyz(10,-1, 1,-1,1,-1);
    charge_xyz(11,-1,-1,-1,0,-1);
    charge_xyz(12, 1, 1,-1,1,-1);
    charge_xyz(13, 1,-1,-1,0,-1);
    charge_xyz(14, 1, 1, 1,1,-1);
    charge_xyz(15, 1,-1, 1,0,-1);
    charge_xyz(16,-1,-1,-1,1,-1); // diagonales 3D
    charge_xyz(17, 1, 1, 1,0,-1);
    charge_xyz(18, 1,-1,-1,1,-1);
    charge_xyz(19,-1, 1, 1,0,-1);
    charge_xyz(20, 1,-1, 1,1,-1);
    charge_xyz(21,-1, 1,-1,0,-1);
    charge_xyz(22,-1,-1, 1,1,-1);
    charge_xyz(23, 1, 1,-1,0,-1);
    return 23;
  }

  if (Type==CUBE_C) {   // Modif objets
    charge_xyz( 0, 0, 0, 0,1,-1); // croix 3D centr‚e
    charge_xyz( 1,.5, 0, 0,0,-1);
    charge_xyz( 2, 0, 0, 0,1,-1);
    charge_xyz( 3, 0,.5, 0,0,-1);
    charge_xyz( 4, 0, 0, 0,1,-1);
    charge_xyz( 5, 0, 0,.5,0,-1);
    charge_xyz( 6,-0.5,1  ,-1  ,1,-1); // --
    charge_xyz( 7,-1  ,1  ,-1  ,0,-1);
    charge_xyz( 8,-1  ,0.5,-1  ,0,-1);
    charge_xyz( 9,-1  ,1  ,-1  ,1,-1);
    charge_xyz(10,-1  ,1  ,-0.5,0,-1);
    charge_xyz(11, 0.5,1  ,-1  ,1,-1); // --
    charge_xyz(12, 1  ,1  ,-1  ,0,-1);
    charge_xyz(13, 1  ,0.5,-1  ,0,-1);
    charge_xyz(14, 1  ,1  ,-1  ,1,-1);
    charge_xyz(15, 1  ,1  ,-0.5,0,-1);
    charge_xyz(16,-0.5,1  , 1  ,1,-1); // --
    charge_xyz(17,-1  ,1  , 1  ,0,-1);
    charge_xyz(18,-1  ,0.5, 1  ,0,-1);
    charge_xyz(19,-1  ,1  , 1  ,1,-1);
    charge_xyz(20,-1  ,1  , 0.5,0,-1);
    charge_xyz(21, 0.5,1  , 1  ,1,-1); // --
    charge_xyz(22, 1  ,1  , 1  ,0,-1);
    charge_xyz(23, 1  ,0.5, 1  ,0,-1);
    charge_xyz(24, 1  ,1  , 1  ,1,-1);
    charge_xyz(25, 1  ,1  , 0.5,0,-1);
    charge_xyz(26,-0.5,-1  ,-1  ,1,-1); // --
    charge_xyz(27,-1  ,-1  ,-1  ,0,-1);
    charge_xyz(28,-1  ,-0.5,-1  ,0,-1);
    charge_xyz(29,-1  ,-1  ,-1  ,1,-1);
    charge_xyz(30,-1  ,-1  ,-0.5,0,-1);
    charge_xyz(31, 0.5,-1  ,-1  ,1,-1); // --
    charge_xyz(32, 1  ,-1  ,-1  ,0,-1);
    charge_xyz(33, 1  ,-0.5,-1  ,0,-1);
    charge_xyz(34, 1  ,-1  ,-1  ,1,-1);
    charge_xyz(35, 1  ,-1  ,-0.5,0,-1);
    charge_xyz(36,-0.5,-1  , 1  ,1,-1); // --
    charge_xyz(37,-1  ,-1  , 1  ,0,-1);
    charge_xyz(38,-1  ,-0.5, 1  ,0,-1);
    charge_xyz(39,-1  ,-1  , 1  ,1,-1);
    charge_xyz(40,-1  ,-1  , 0.5,0,-1);
    charge_xyz(41, 0.5,-1  , 1  ,1,-1); // --
    charge_xyz(42, 1  ,-1  , 1  ,0,-1);
    charge_xyz(43, 1  ,-0.5, 1  ,0,-1);
    charge_xyz(44, 1  ,-1  , 1  ,1,-1);
    charge_xyz(45, 1  ,-1  , 0.5,0,-1);
    return 45;
  }

  if (Type==CUBE_C) {            // cube avec croix au milieu (manipulations
    charge_xyz( 0,-1, 1, 1,1,-1);   // d'objets et s‚lections
    charge_xyz( 1, 1, 1, 1,0,-1);
    charge_xyz( 2, 1, 1,-1,0,-1);
    charge_xyz( 3,-1, 1,-1,0,-1);
    charge_xyz( 4,-1, 1, 1,0,-1);
    charge_xyz( 5,-1,-1, 1,0,-1);
    charge_xyz( 6,-1,-1,-1,0,-1);
    charge_xyz( 7, 1,-1,-1,0,-1);
    charge_xyz( 8, 1,-1, 1,0,-1);
    charge_xyz( 9,-1,-1, 1,0,-1);
    charge_xyz(10,-1, 1,-1,1,-1);
    charge_xyz(11,-1,-1,-1,0,-1);
    charge_xyz(12, 1, 1,-1,1,-1);
    charge_xyz(13, 1,-1,-1,0,-1);
    charge_xyz(14, 1, 1, 1,1,-1);
    charge_xyz(15, 1,-1, 1,0,-1);
    charge_xyz(16,-0.2,0,0,1,-1); // croix 3D centr‚e
    /*charge_xyz(17, 0.2,0,0,0,-1);
    charge_xyz(18, 0,-0.2,0,1,-1);
    charge_xyz(19, 0, 0.2,0,0,-1);
    charge_xyz(20, 0,0,-0.2,1,-1);
    charge_xyz(21, 0,0, 0.2,0,-1);*/
    charge_xyz(16, 0, 0, 0,1,-1);
    charge_xyz(17,.5, 0, 0,0,-1);
    charge_xyz(18, 0, 0, 0,1,-1);
    charge_xyz(19, 0,.5, 0,0,-1);
    charge_xyz(20, 0, 0, 0,1,-1);
    charge_xyz(21, 0, 0,.5,0,-1);
    return 21;
  }

  if (Type==MAPPING) { // cube pour mapping
    charge_xyz( 0,-1, 1, 1,1,-1);
    charge_xyz( 1, 1, 1, 1,0,-1);
    charge_xyz( 2, 1, 1,-1,0,-1);
    charge_xyz( 3,-1, 1,-1,0,-1);
    charge_xyz( 4,-1, 1, 1,0,-1);
    charge_xyz( 5,-1,-1, 1,0,-1);
    charge_xyz( 6,-1,-1,-1,0,-1);
    charge_xyz( 7, 1,-1,-1,0,-1);
    charge_xyz( 8, 1,-1, 1,0,-1);
    charge_xyz( 9,-1,-1, 1,0,-1);
    charge_xyz(10,-1, 1,-1,1,-1);
    charge_xyz(11,-1,-1,-1,0,-1);
    charge_xyz(12, 1, 1,-1,1,-1);
    charge_xyz(13, 1,-1,-1,0,-1);
    charge_xyz(14, 1, 1, 1,1,-1);
    charge_xyz(15, 1,-1, 1,0,-1);
    charge_xyz(16,-0.6,-1  , 1,1,-1);  // onglets
    charge_xyz(17,-1  ,-0.6, 1,0,-1);
    charge_xyz(18,-0.6,-1  ,-1,1,-1);
    charge_xyz(19,-1  ,-0.6,-1,0,-1);
    return 19;
  }

  // CERCLE 12 segments sur X/Y
  if (Type==CERCLE12 ||
      Type==ANNEAU ||
      Type==QTORE ||
      Type==TORE ||
      Type==CONET ||
      Type==DISQUE) {
    charge_xyz( 0, 0.9659, 0.2588,0,1,-1);
    charge_xyz( 1, 0.7071, 0.7071,0,0,-1);
    charge_xyz( 2, 0.2588, 0.9659,0,0,-1);
    charge_xyz( 3,-0.2588, 0.9659,0,0,-1);
    charge_xyz( 4,-0.7071, 0.7071,0,0,-1);
    charge_xyz( 5,-0.9659, 0.2588,0,0,-1);
    charge_xyz( 6,-0.9659,-0.2588,0,0,-1);
    charge_xyz( 7,-0.7071,-0.7071,0,0,-1);
    charge_xyz( 8,-0.2588,-0.9659,0,0,-1);
    charge_xyz( 9, 0.2588,-0.9659,0,0,-1);
    charge_xyz(10, 0.7071,-0.7071,0,0,-1);
    charge_xyz(11, 0.9659,-0.2588,0,0,-1);
    charge_xyz(12, 0.9659, 0.2588,0,0,-1);
    NbPoint=12;
  }

  if (Type==TORE) { // trou sur plan X/Z
     return (NbPoint=calcule_tore(NumeroObjet));
  }

  if (Type==QTORE) { // trou sur plan X/Z
    return (NbPoint=calcule_quart_tore(NumeroObjet));
  }

  if (Type==CERCLEX24) { // CERCLE 24 segments sur Y/Z
    charge_xyz( 0,0, 0.99, 0.13,1,-1);
    charge_xyz( 1,0, 0.92, 0.38,0,-1);
    charge_xyz( 2,0, 0.79, 0.60,0,-1);
    charge_xyz( 3,0, 0.60, 0.79,0,-1);
    charge_xyz( 4,0, 0.38, 0.92,0,-1);
    charge_xyz( 5,0, 0.13, 0.99,0,-1);
    charge_xyz( 6,0,-0.13, 0.99,0,-1);
    charge_xyz( 7,0,-0.38, 0.92,0,-1);
    charge_xyz( 8,0,-0.60, 0.79,0,-1);
    charge_xyz( 9,0,-0.79, 0.60,0,-1);
    charge_xyz(10,0,-0.92, 0.38,0,-1);
    charge_xyz(11,0,-0.99, 0.13,0,-1);
    charge_xyz(12,0,-0.99,-0.13,0,-1);
    charge_xyz(13,0,-0.92,-0.38,0,-1);
    charge_xyz(14,0,-0.79,-0.60,0,-1);
    charge_xyz(15,0,-0.60,-0.79,0,-1);
    charge_xyz(16,0,-0.38,-0.92,0,-1);
    charge_xyz(17,0,-0.13,-0.99,0,-1);
    charge_xyz(18,0, 0.13,-0.99,0,-1);
    charge_xyz(19,0, 0.38,-0.92,0,-1);
    charge_xyz(20,0, 0.60,-0.79,0,-1);
    charge_xyz(21,0, 0.79,-0.60,0,-1);
    charge_xyz(22,0, 0.92,-0.38,0,-1);
    charge_xyz(23,0, 0.99,-0.13,0,-1);
    charge_xyz(24,0, 0.99, 0.13,0,-1);
    return 24;
  }

  if (Type==CERCLEY24) { // CERCLE 24 segments sur X/Z
    charge_xyz( 0, 0.99,0, 0.13,1,-1);
    charge_xyz( 1, 0.92,0, 0.38,0,-1);
    charge_xyz( 2, 0.79,0, 0.60,0,-1);
    charge_xyz( 3, 0.60,0, 0.79,0,-1);
    charge_xyz( 4, 0.38,0, 0.92,0,-1);
    charge_xyz( 5, 0.13,0, 0.99,0,-1);
    charge_xyz( 6,-0.13,0, 0.99,0,-1);
    charge_xyz( 7,-0.38,0, 0.92,0,-1);
    charge_xyz( 8,-0.60,0, 0.79,0,-1);
    charge_xyz( 9,-0.79,0, 0.60,0,-1);
    charge_xyz(10,-0.92,0, 0.38,0,-1);
    charge_xyz(11,-0.99,0, 0.13,0,-1);
    charge_xyz(12,-0.99,0,-0.13,0,-1);
    charge_xyz(13,-0.92,0,-0.38,0,-1);
    charge_xyz(14,-0.79,0,-0.60,0,-1);
    charge_xyz(15,-0.60,0,-0.79,0,-1);
    charge_xyz(16,-0.38,0,-0.92,0,-1);
    charge_xyz(17,-0.13,0,-0.99,0,-1);
    charge_xyz(18, 0.13,0,-0.99,0,-1);
    charge_xyz(19, 0.38,0,-0.92,0,-1);
    charge_xyz(20, 0.60,0,-0.79,0,-1);
    charge_xyz(21, 0.79,0,-0.60,0,-1);
    charge_xyz(22, 0.92,0,-0.38,0,-1);
    charge_xyz(23, 0.99,0,-0.13,0,-1);
    charge_xyz(24, 0.99,0, 0.13,0,-1);
    NbPoint=24;
  }

  if (Type==CERCLEZ24 || Type==TUBE) { // CERCLE 24 segments sur X/Y
    charge_xyz( 0, 0.99, 0.13,0,1,-1);
    charge_xyz( 1, 0.92, 0.38,0,0,-1);
    charge_xyz( 2, 0.79, 0.60,0,0,-1);
    charge_xyz( 3, 0.60, 0.79,0,0,-1);
    charge_xyz( 4, 0.38, 0.92,0,0,-1);
    charge_xyz( 5, 0.13, 0.99,0,0,-1);
    charge_xyz( 6,-0.13, 0.99,0,0,-1);
    charge_xyz( 7,-0.38, 0.92,0,0,-1);
    charge_xyz( 8,-0.60, 0.79,0,0,-1);
    charge_xyz( 9,-0.79, 0.60,0,0,-1);
    charge_xyz(10,-0.92, 0.38,0,0,-1);
    charge_xyz(11,-0.99, 0.13,0,0,-1);
    charge_xyz(12,-0.99,-0.13,0,0,-1);
    charge_xyz(13,-0.92,-0.38,0,0,-1);
    charge_xyz(14,-0.79,-0.60,0,0,-1);
    charge_xyz(15,-0.60,-0.79,0,0,-1);
    charge_xyz(16,-0.38,-0.92,0,0,-1);
    charge_xyz(17,-0.13,-0.99,0,0,-1);
    charge_xyz(18, 0.13,-0.99,0,0,-1);
    charge_xyz(19, 0.38,-0.92,0,0,-1);
    charge_xyz(20, 0.60,-0.79,0,0,-1);
    charge_xyz(21, 0.79,-0.60,0,0,-1);
    charge_xyz(22, 0.92,-0.38,0,0,-1);
    charge_xyz(23, 0.99,-0.13,0,0,-1);
    charge_xyz(24, 0.99, 0.13,0,0,-1);
    if (Type==TUBE) {
      NbPoint=24;
    } else {
      return 24;
    }
  }

  if (Type==DISQUE) {            // Sur plan X/Y
    charge_xyz(13, 0,      0,     0,1,-1);
    charge_xyz(14, 0.9659, 0.2588,0,0,-1);
    charge_xyz(15, 0,      0,     0,1,-1);
    charge_xyz(16, 0.7071, 0.7071,0,0,-1);
    charge_xyz(17, 0,      0,     0,1,-1);
    charge_xyz(18, 0.2588, 0.9659,0,0,-1);
    charge_xyz(19, 0,      0,     0,1,-1);
    charge_xyz(20,-0.2588, 0.9659,0,0,-1);
    charge_xyz(21, 0,      0,     0,1,-1);
    charge_xyz(22,-0.7071, 0.7071,0,0,-1);
    charge_xyz(23, 0,      0,     0,1,-1);
    charge_xyz(24,-0.9659, 0.2588,0,0,-1);
    charge_xyz(25, 0,      0,     0,1,-1);
    charge_xyz(26,-0.9659,-0.2588,0,0,-1);
    charge_xyz(27, 0,      0,     0,1,-1);
    charge_xyz(28,-0.7071,-0.7071,0,0,-1);
    charge_xyz(29, 0,      0,     0,1,-1);
    charge_xyz(30,-0.2588,-0.9659,0,0,-1);
    charge_xyz(31, 0,      0,     0,1,-1);
    charge_xyz(32, 0.2588,-0.9659,0,0,-1);
    charge_xyz(33, 0,      0,     0,1,-1);
    charge_xyz(34, 0.7071,-0.7071,0,0,-1);
    charge_xyz(35, 0,      0,     0,1,-1);
    charge_xyz(36, 0.9659,-0.2588,0,0,-1);
    //charge_xyz(37, 0,      0,     0,1,-1);
    //charge_xyz(38, 0.9659, 0.2588,0,0,-1);
    return 36;
  }

  if (Type==DSPHERE) {
    charge_xyz( 0 ,   1.0000,   1.0000,   0.0000, 1 ,-1);
    charge_xyz( 1 ,   0.9239,   0.2346,   0.0000, 0 ,-1);
    charge_xyz( 2 ,   0.7071,  -0.4142,   0.0000, 0 ,-1);
    charge_xyz( 3 ,   0.3827,  -0.8478,   0.0000, 0 ,-1);
    charge_xyz( 4 ,  -0.0000,  -1.0000,   0.0000, 0 ,-1);
    charge_xyz( 5 ,  -0.3827,  -0.8478,   0.0000, 0 ,-1);
    charge_xyz( 6 ,  -0.7071,  -0.4142,   0.0000, 0 ,-1);
    charge_xyz( 7 ,  -0.9239,   0.2346,   0.0000, 0 ,-1);
    charge_xyz( 8 ,  -1.0000,   1.0000,   0.0000, 0 ,-1);
    charge_xyz( 9 ,   0.9239,   1.0000,  -0.3827, 1 ,-1);
    charge_xyz( 10 ,   0.8536,   0.2346,  -0.3536, 0 ,-1);
    charge_xyz( 11 ,   0.6533,  -0.4142,  -0.2706, 0 ,-1);
    charge_xyz( 12 ,   0.3536,  -0.8478,  -0.1464, 0 ,-1);
    charge_xyz( 13 ,  -0.0000,  -1.0000,   0.0000, 0 ,-1);
    charge_xyz( 14 ,  -0.3536,  -0.8478,   0.1464, 0 ,-1);
    charge_xyz( 15 ,  -0.6533,  -0.4142,   0.2706, 0 ,-1);
    charge_xyz( 16 ,  -0.8536,   0.2346,   0.3536, 0 ,-1);
    charge_xyz( 17 ,  -0.9239,   1.0000,   0.3827, 0 ,-1);
    charge_xyz( 18 ,   0.7071,   1.0000,  -0.7071, 1 ,-1);
    charge_xyz( 19 ,   0.6533,   0.2346,  -0.6533, 0 ,-1);
    charge_xyz( 20 ,   0.5000,  -0.4142,  -0.5000, 0 ,-1);
    charge_xyz( 21 ,   0.2706,  -0.8478,  -0.2706, 0 ,-1);
    charge_xyz( 22 ,  -0.0000,  -1.0000,   0.0000, 0 ,-1);
    charge_xyz( 23 ,  -0.2706,  -0.8478,   0.2706, 0 ,-1);
    charge_xyz( 24 ,  -0.5000,  -0.4142,   0.5000, 0 ,-1);
    charge_xyz( 25 ,  -0.6533,   0.2346,   0.6533, 0 ,-1);
    charge_xyz( 26 ,  -0.7071,   1.0000,   0.7071, 0 ,-1);
    charge_xyz( 27 ,   0.3827,   1.0000,  -0.9239, 1 ,-1);
    charge_xyz( 28 ,   0.3536,   0.2346,  -0.8536, 0 ,-1);
    charge_xyz( 29 ,   0.2706,  -0.4142,  -0.6533, 0 ,-1);
    charge_xyz( 30 ,   0.1464,  -0.8478,  -0.3536, 0 ,-1);
    charge_xyz( 31 ,  -0.0000,  -1.0000,   0.0000, 0 ,-1);
    charge_xyz( 32 ,  -0.1464,  -0.8478,   0.3536, 0 ,-1);
    charge_xyz( 33 ,  -0.2706,  -0.4142,   0.6533, 0 ,-1);
    charge_xyz( 34 ,  -0.3536,   0.2346,   0.8536, 0 ,-1);
    charge_xyz( 35 ,  -0.3827,   1.0000,   0.9239, 0 ,-1);
    charge_xyz( 36 ,  -0.0000,   1.0000,  -1.0000, 1 ,-1);
    charge_xyz( 37 ,  -0.0000,   0.2346,  -0.9239, 0 ,-1);
    charge_xyz( 38 ,  -0.0000,  -0.4142,  -0.7071, 0 ,-1);
    charge_xyz( 39 ,  -0.0000,  -0.8478,  -0.3827, 0 ,-1);
    charge_xyz( 40 ,   0.0000,  -1.0000,   0.0000, 0 ,-1);
    charge_xyz( 41 ,   0.0000,  -0.8478,   0.3827, 0 ,-1);
    charge_xyz( 42 ,   0.0000,  -0.4142,   0.7071, 0 ,-1);
    charge_xyz( 43 ,   0.0000,   0.2346,   0.9239, 0 ,-1);
    charge_xyz( 44 ,   0.0000,   1.0000,   1.0000, 0 ,-1);
    charge_xyz( 45 ,  -0.3827,   1.0000,  -0.9239, 1 ,-1);
    charge_xyz( 46 ,  -0.3536,   0.2346,  -0.8536, 0 ,-1);
    charge_xyz( 47 ,  -0.2706,  -0.4142,  -0.6533, 0 ,-1);
    charge_xyz( 48 ,  -0.1464,  -0.8478,  -0.3536, 0 ,-1);
    charge_xyz( 49 ,   0.0000,  -1.0000,   0.0000, 0 ,-1);
    charge_xyz( 50 ,   0.1464,  -0.8478,   0.3536, 0 ,-1);
    charge_xyz( 51 ,   0.2706,  -0.4142,   0.6533, 0 ,-1);
    charge_xyz( 52 ,   0.3536,   0.2346,   0.8536, 0 ,-1);
    charge_xyz( 53 ,   0.3827,   1.0000,   0.9239, 0 ,-1);
    charge_xyz( 54 ,  -0.7071,   1.0000,  -0.7071, 1 ,-1);
    charge_xyz( 55 ,  -0.6533,   0.2346,  -0.6533, 0 ,-1);
    charge_xyz( 56 ,  -0.5000,  -0.4142,  -0.5000, 0 ,-1);
    charge_xyz( 57 ,  -0.2706,  -0.8478,  -0.2706, 0 ,-1);
    charge_xyz( 58 ,   0.0000,  -1.0000,   0.0000, 0 ,-1);
    charge_xyz( 59 ,   0.2706,  -0.8478,   0.2706, 0 ,-1);
    charge_xyz( 60 ,   0.5000,  -0.4142,   0.5000, 0 ,-1);
    charge_xyz( 61 ,   0.6533,   0.2346,   0.6533, 0 ,-1);
    charge_xyz( 62 ,   0.7071,   1.0000,   0.7071, 0 ,-1);
    charge_xyz( 63 ,  -0.9239,   1.0000,  -0.3827, 1 ,-1);
    charge_xyz( 64 ,  -0.8536,   0.2346,  -0.3536, 0 ,-1);
    charge_xyz( 65 ,  -0.6533,  -0.4142,  -0.2706, 0 ,-1);
    charge_xyz( 66 ,  -0.3536,  -0.8478,  -0.1464, 0 ,-1);
    charge_xyz( 67 ,   0.0000,  -1.0000,   0.0000, 0 ,-1);
    charge_xyz( 68 ,   0.3536,  -0.8478,   0.1464, 0 ,-1);
    charge_xyz( 69 ,   0.6533,  -0.4142,   0.2706, 0 ,-1);
    charge_xyz( 70 ,   0.8536,   0.2346,   0.3536, 0 ,-1);
    charge_xyz( 71 ,   0.9239,   1.0000,   0.3827, 0 ,-1);
    charge_xyz( 72 ,  -1.0000,   1.0000,   0.0000, 1 ,-1);
    charge_xyz( 73 ,  -0.9239,   0.2346,   0.0000, 0 ,-1);
    charge_xyz( 74 ,  -0.7071,  -0.4142,   0.0000, 0 ,-1);
    charge_xyz( 75 ,  -0.3827,  -0.8478,   0.0000, 0 ,-1);
    charge_xyz( 76 ,   0.0000,  -1.0000,  -0.0000, 0 ,-1);
    charge_xyz( 77 ,   0.3827,  -0.8478,  -0.0000, 0 ,-1);
    charge_xyz( 78 ,   0.7071,  -0.4142,  -0.0000, 0 ,-1);
    charge_xyz( 79 ,   0.9239,   0.2346,  -0.0000, 0 ,-1);
    charge_xyz( 80 ,   1.0000,   1.0000,  -0.0000, 0 ,-1);
    charge_xyz( 81 ,   0.0000,   1.0000,   1.0000, 1 ,-1);
    charge_xyz( 82 ,   0.3827,   1.0000,   0.9239, 0 ,-1);
    charge_xyz( 83 ,   0.7071,   1.0000,   0.7071, 0 ,-1);
    charge_xyz( 84 ,   0.9239,   1.0000,   0.3827, 0 ,-1);
    charge_xyz( 85 ,   1.0000,   1.0000,  -0.0000, 0 ,-1);
    charge_xyz( 86 ,   0.9239,   1.0000,  -0.3827, 0 ,-1);
    charge_xyz( 87 ,   0.7071,   1.0000,  -0.7071, 0 ,-1);
    charge_xyz( 88 ,   0.3827,   1.0000,  -0.9239, 0 ,-1);
    charge_xyz( 89 ,  -0.0000,   1.0000,  -1.0000, 0 ,-1);
    charge_xyz( 90 ,  -0.3827,   1.0000,  -0.9239, 0 ,-1);
    charge_xyz( 91 ,  -0.7071,   1.0000,  -0.7071, 0 ,-1);
    charge_xyz( 92 ,  -0.9239,   1.0000,  -0.3827, 0 ,-1);
    charge_xyz( 93 ,  -1.0000,   1.0000,   0.0000, 0 ,-1);
    charge_xyz( 94 ,  -0.9239,   1.0000,   0.3827, 0 ,-1);
    charge_xyz( 95 ,  -0.7071,   1.0000,   0.7071, 0 ,-1);
    charge_xyz( 96 ,  -0.3827,   1.0000,   0.9239, 0 ,-1);
    charge_xyz( 97 ,   0.0000,   1.0000,   1.0000, 0 ,-1);
    charge_xyz( 98 ,   0.0000,   0.2346,   0.9239, 1 ,-1);
    charge_xyz( 99 ,   0.3536,   0.2346,   0.8536, 0 ,-1);
    charge_xyz( 100 ,   0.6533,   0.2346,   0.6533, 0 ,-1);
    charge_xyz( 101 ,   0.8536,   0.2346,   0.3536, 0 ,-1);
    charge_xyz( 102 ,   0.9239,   0.2346,  -0.0000, 0 ,-1);
    charge_xyz( 103 ,   0.8536,   0.2346,  -0.3536, 0 ,-1);
    charge_xyz( 104 ,   0.6533,   0.2346,  -0.6533, 0 ,-1);
    charge_xyz( 105 ,   0.3536,   0.2346,  -0.8536, 0 ,-1);
    charge_xyz( 106 ,  -0.0000,   0.2346,  -0.9239, 0 ,-1);
    charge_xyz( 107 ,  -0.3536,   0.2346,  -0.8536, 0 ,-1);
    charge_xyz( 108 ,  -0.6533,   0.2346,  -0.6533, 0 ,-1);
    charge_xyz( 109 ,  -0.8536,   0.2346,  -0.3536, 0 ,-1);
    charge_xyz( 110 ,  -0.9239,   0.2346,   0.0000, 0 ,-1);
    charge_xyz( 111 ,  -0.8536,   0.2346,   0.3536, 0 ,-1);
    charge_xyz( 112 ,  -0.6533,   0.2346,   0.6533, 0 ,-1);
    charge_xyz( 113 ,  -0.3536,   0.2346,   0.8536, 0 ,-1);
    charge_xyz( 114 ,   0.0000,   0.2346,   0.9239, 0 ,-1);
    charge_xyz( 115 ,   0.0000,  -0.4142,   0.7071, 1 ,-1);
    charge_xyz( 116 ,   0.2706,  -0.4142,   0.6533, 0 ,-1);
    charge_xyz( 117 ,   0.5000,  -0.4142,   0.5000, 0 ,-1);
    charge_xyz( 118 ,   0.6533,  -0.4142,   0.2706, 0 ,-1);
    charge_xyz( 119 ,   0.7071,  -0.4142,  -0.0000, 0 ,-1);
    charge_xyz( 120 ,   0.6533,  -0.4142,  -0.2706, 0 ,-1);
    charge_xyz( 121 ,   0.5000,  -0.4142,  -0.5000, 0 ,-1);
    charge_xyz( 122 ,   0.2706,  -0.4142,  -0.6533, 0 ,-1);
    charge_xyz( 123 ,  -0.0000,  -0.4142,  -0.7071, 0 ,-1);
    charge_xyz( 124 ,  -0.2706,  -0.4142,  -0.6533, 0 ,-1);
    charge_xyz( 125 ,  -0.5000,  -0.4142,  -0.5000, 0 ,-1);
    charge_xyz( 126 ,  -0.6533,  -0.4142,  -0.2706, 0 ,-1);
    charge_xyz( 127 ,  -0.7071,  -0.4142,   0.0000, 0 ,-1);
    charge_xyz( 128 ,  -0.6533,  -0.4142,   0.2706, 0 ,-1);
    charge_xyz( 129 ,  -0.5000,  -0.4142,   0.5000, 0 ,-1);
    charge_xyz( 130 ,  -0.2706,  -0.4142,   0.6533, 0 ,-1);
    charge_xyz( 131 ,   0.0000,  -0.4142,   0.7071, 0 ,-1);
    charge_xyz( 132 ,   0.0000,  -0.8478,   0.3827, 1 ,-1);
    charge_xyz( 133 ,   0.1464,  -0.8478,   0.3536, 0 ,-1);
    charge_xyz( 134 ,   0.2706,  -0.8478,   0.2706, 0 ,-1);
    charge_xyz( 135 ,   0.3536,  -0.8478,   0.1464, 0 ,-1);
    charge_xyz( 136 ,   0.3827,  -0.8478,  -0.0000, 0 ,-1);
    charge_xyz( 137 ,   0.3536,  -0.8478,  -0.1464, 0 ,-1);
    charge_xyz( 138 ,   0.2706,  -0.8478,  -0.2706, 0 ,-1);
    charge_xyz( 139 ,   0.1464,  -0.8478,  -0.3536, 0 ,-1);
    charge_xyz( 140 ,  -0.0000,  -0.8478,  -0.3827, 0 ,-1);
    charge_xyz( 141 ,  -0.1464,  -0.8478,  -0.3536, 0 ,-1);
    charge_xyz( 142 ,  -0.2706,  -0.8478,  -0.2706, 0 ,-1);
    charge_xyz( 143 ,  -0.3536,  -0.8478,  -0.1464, 0 ,-1);
    charge_xyz( 144 ,  -0.3827,  -0.8478,   0.0000, 0 ,-1);
    charge_xyz( 145 ,  -0.3536,  -0.8478,   0.1464, 0 ,-1);
    charge_xyz( 146 ,  -0.2706,  -0.8478,   0.2706, 0 ,-1);
    charge_xyz( 147 ,  -0.1464,  -0.8478,   0.3536, 0 ,-1);
    charge_xyz( 148 ,   0.0000,  -0.8478,   0.3827, 0 ,-1);
    charge_xyz( 149 ,   0.0000,  -1.0000,  -0.0000, 1 ,-1);
    charge_xyz( 150 ,  -0.0000,  -1.0000,  -0.0000, 0 ,-1);
    charge_xyz( 151 ,  -0.0000,  -1.0000,  -0.0000, 0 ,-1);
    charge_xyz( 152 ,  -0.0000,  -1.0000,  -0.0000, 0 ,-1);
    charge_xyz( 153 ,  -0.0000,  -1.0000,   0.0000, 0 ,-1);
    charge_xyz( 154 ,  -0.0000,  -1.0000,   0.0000, 0 ,-1);
    charge_xyz( 155 ,  -0.0000,  -1.0000,   0.0000, 0 ,-1);
    charge_xyz( 156 ,  -0.0000,  -1.0000,   0.0000, 0 ,-1);
    charge_xyz( 157 ,   0.0000,  -1.0000,   0.0000, 0 ,-1);
    charge_xyz( 158 ,   0.0000,  -1.0000,   0.0000, 0 ,-1);
    charge_xyz( 159 ,   0.0000,  -1.0000,   0.0000, 0 ,-1);
    charge_xyz( 160 ,   0.0000,  -1.0000,   0.0000, 0 ,-1);
    charge_xyz( 161 ,   0.0000,  -1.0000,  -0.0000, 0 ,-1);
    charge_xyz( 162 ,   0.0000,  -1.0000,  -0.0000, 0 ,-1);
    charge_xyz( 163 ,   0.0000,  -1.0000,  -0.0000, 0 ,-1);
    charge_xyz( 164 ,   0.0000,  -1.0000,  -0.0000, 0 ,-1);
    charge_xyz( 165 ,  -0.0000,  -1.0000,  -0.0000, 0 ,-1);
    charge_xyz( 166 ,   0.0000,  -0.8478,  -0.3827, 1 ,-1);
    charge_xyz( 167 ,  -0.1464,  -0.8478,  -0.3536, 0 ,-1);
    charge_xyz( 168 ,  -0.2706,  -0.8478,  -0.2706, 0 ,-1);
    charge_xyz( 169 ,  -0.3536,  -0.8478,  -0.1464, 0 ,-1);
    charge_xyz( 170 ,  -0.3827,  -0.8478,   0.0000, 0 ,-1);
    charge_xyz( 171 ,  -0.3536,  -0.8478,   0.1464, 0 ,-1);
    charge_xyz( 172 ,  -0.2706,  -0.8478,   0.2706, 0 ,-1);
    charge_xyz( 173 ,  -0.1464,  -0.8478,   0.3536, 0 ,-1);
    charge_xyz( 174 ,   0.0000,  -0.8478,   0.3827, 0 ,-1);
    charge_xyz( 175 ,   0.1464,  -0.8478,   0.3536, 0 ,-1);
    charge_xyz( 176 ,   0.2706,  -0.8478,   0.2706, 0 ,-1);
    charge_xyz( 177 ,   0.3536,  -0.8478,   0.1464, 0 ,-1);
    charge_xyz( 178 ,   0.3827,  -0.8478,  -0.0000, 0 ,-1);
    charge_xyz( 179 ,   0.3536,  -0.8478,  -0.1464, 0 ,-1);
    charge_xyz( 180 ,   0.2706,  -0.8478,  -0.2706, 0 ,-1);
    charge_xyz( 181 ,   0.1464,  -0.8478,  -0.3536, 0 ,-1);
    charge_xyz( 182 ,  -0.0000,  -0.8478,  -0.3827, 0 ,-1);
    charge_xyz( 183 ,   0.0000,  -0.4142,  -0.7071, 1 ,-1);
    charge_xyz( 184 ,  -0.2706,  -0.4142,  -0.6533, 0 ,-1);
    charge_xyz( 185 ,  -0.5000,  -0.4142,  -0.5000, 0 ,-1);
    charge_xyz( 186 ,  -0.6533,  -0.4142,  -0.2706, 0 ,-1);
    charge_xyz( 187 ,  -0.7071,  -0.4142,   0.0000, 0 ,-1);
    charge_xyz( 188 ,  -0.6533,  -0.4142,   0.2706, 0 ,-1);
    charge_xyz( 189 ,  -0.5000,  -0.4142,   0.5000, 0 ,-1);
    charge_xyz( 190 ,  -0.2706,  -0.4142,   0.6533, 0 ,-1);
    charge_xyz( 191 ,   0.0000,  -0.4142,   0.7071, 0 ,-1);
    charge_xyz( 192 ,   0.2706,  -0.4142,   0.6533, 0 ,-1);
    charge_xyz( 193 ,   0.5000,  -0.4142,   0.5000, 0 ,-1);
    charge_xyz( 194 ,   0.6533,  -0.4142,   0.2706, 0 ,-1);
    charge_xyz( 195 ,   0.7071,  -0.4142,  -0.0000, 0 ,-1);
    charge_xyz( 196 ,   0.6533,  -0.4142,  -0.2706, 0 ,-1);
    charge_xyz( 197 ,   0.5000,  -0.4142,  -0.5000, 0 ,-1);
    charge_xyz( 198 ,   0.2706,  -0.4142,  -0.6533, 0 ,-1);
    charge_xyz( 199 ,  -0.0000,  -0.4142,  -0.7071, 0 ,-1);
    charge_xyz( 200 ,   0.0000,   0.2346,  -0.9239, 1 ,-1);
    charge_xyz( 201 ,  -0.3536,   0.2346,  -0.8536, 0 ,-1);
    charge_xyz( 202 ,  -0.6533,   0.2346,  -0.6533, 0 ,-1);
    charge_xyz( 203 ,  -0.8536,   0.2346,  -0.3536, 0 ,-1);
    charge_xyz( 204 ,  -0.9239,   0.2346,   0.0000, 0 ,-1);
    charge_xyz( 205 ,  -0.8536,   0.2346,   0.3536, 0 ,-1);
    charge_xyz( 206 ,  -0.6533,   0.2346,   0.6533, 0 ,-1);
    charge_xyz( 207 ,  -0.3536,   0.2346,   0.8536, 0 ,-1);
    charge_xyz( 208 ,   0.0000,   0.2346,   0.9239, 0 ,-1);
    charge_xyz( 209 ,   0.3536,   0.2346,   0.8536, 0 ,-1);
    charge_xyz( 210 ,   0.6533,   0.2346,   0.6533, 0 ,-1);
    charge_xyz( 211 ,   0.8536,   0.2346,   0.3536, 0 ,-1);
    charge_xyz( 212 ,   0.9239,   0.2346,  -0.0000, 0 ,-1);
    charge_xyz( 213 ,   0.8536,   0.2346,  -0.3536, 0 ,-1);
    charge_xyz( 214 ,   0.6533,   0.2346,  -0.6533, 0 ,-1);
    charge_xyz( 215 ,   0.3536,   0.2346,  -0.8536, 0 ,-1);
    charge_xyz( 216 ,  -0.0000,   0.2346,  -0.9239, 0 ,-1);
    charge_xyz( 217 ,   0.0000,   1.0000,  -1.0000, 1 ,-1);
    charge_xyz( 218 ,  -0.3827,   1.0000,  -0.9239, 0 ,-1);
    charge_xyz( 219 ,  -0.7071,   1.0000,  -0.7071, 0 ,-1);
    charge_xyz( 220 ,  -0.9239,   1.0000,  -0.3827, 0 ,-1);
    charge_xyz( 221 ,  -1.0000,   1.0000,   0.0000, 0 ,-1);
    charge_xyz( 222 ,  -0.9239,   1.0000,   0.3827, 0 ,-1);
    charge_xyz( 223 ,  -0.7071,   1.0000,   0.7071, 0 ,-1);
    charge_xyz( 224 ,  -0.3827,   1.0000,   0.9239, 0 ,-1);
    charge_xyz( 225 ,   0.0000,   1.0000,   1.0000, 0 ,-1);
    charge_xyz( 226 ,   0.3827,   1.0000,   0.9239, 0 ,-1);
    charge_xyz( 227 ,   0.7071,   1.0000,   0.7071, 0 ,-1);
    charge_xyz( 228 ,   0.9239,   1.0000,   0.3827, 0 ,-1);
    charge_xyz( 229 ,   1.0000,   1.0000,  -0.0000, 0 ,-1);
    charge_xyz( 230 ,   0.9239,   1.0000,  -0.3827, 0 ,-1);
    charge_xyz( 231 ,   0.7071,   1.0000,  -0.7071, 0 ,-1);
    charge_xyz( 232 ,   0.3827,   1.0000,  -0.9239, 0 ,-1);
    charge_xyz( 233 ,  -0.0000,   1.0000,  -1.0000, 0 ,-1);
    return 233;
  }

  if (Type==OMNI) {            // Source de lumiŠre OMNI
    charge_xyz( 0,-1,      0,     0,1,-1);
    charge_xyz( 1, 1,      0,     0,0,-1);
    charge_xyz( 2, 0,      1,     0,1,-1);
    charge_xyz( 3, 0,     -1,     0,0,-1);
    charge_xyz( 4, 0,      0,     1,1,-1);
    charge_xyz( 5, 0,      0,    -1,0,-1);
    charge_xyz( 6,-1,-1,-1,1,-1); // diagonales 3D
    charge_xyz( 7, 1, 1, 1,0,-1);
    charge_xyz( 8, 1,-1,-1,1,-1);
    charge_xyz( 9,-1, 1, 1,0,-1);
    charge_xyz(10, 1,-1, 1,1,-1);
    charge_xyz(11,-1, 1,-1,0,-1);
    charge_xyz(12,-1,-1, 1,1,-1);
    charge_xyz(13, 1, 1,-1,0,-1);
    return 13;
  }

  if (Type==TUBE) {              // Trou sur plan X/Y
     return (NbPoint=calcule_tube(NumeroObjet));
  }

  if (Type==CONET) {              // Axe sur plan X/Y (Z)
     return (NbPoint=calcule_cone_tronque(NumeroObjet));
  }

  if (Type==QTUBE) {              // Trou sur plan X/Y
     return (NbPoint=calcule_quart_tube(NumeroObjet));
  }

  if (Type==ANNEAU) {            // Trou sur plan X/Y
     return (NbPoint=calcule_anneau(NumeroObjet));
  }

  if (Type==SPLINE) {            // Affiche une B-Spline
     return (NbPoint=calcule_spline(NumeroObjet,Vue));
  }

  if (Type==AXES) { // AXES X/Y/Z
    charge_xyz( 0,-1,0,0,1,-1);
    charge_xyz( 1, 1,0,0,0,-1);
    charge_xyz( 2,0,-1,0,1,-1);
    charge_xyz( 3,0, 1,0,0,-1);
    charge_xyz( 4,0,0,-1,1,-1);
    charge_xyz( 5,0,0, 1,0,-1);
    return 5;
  }

  if (Type==GRILLE) { // grille pour placement objets
    if (!GrilleType) {
      return (1250-1);
    } else {
      return (800-1);
    }
  }

  if (Type==TRIANGLE || Type==HFIELD) {            // Objets form‚s de polygone
    return (NbPoint=data_triangle(NumeroObjet));
  }

  if (Type==SOR || Type==LATHE) {
    return (NbPoint=calcule_special(NumeroObjet));
  }

  if (Type==EXTRUDE) {
    return (NbPoint=calcule_extrude(NumeroObjet));
  }

  if (Type==SPOT) {
    return (NbPoint=calcule_spot(NumeroObjet));
  }

  if (Type==BEZIER) {
    return (NbPoint=calcule_patch_bezier(NumeroObjet));
  }

  return NbPoint;
}

