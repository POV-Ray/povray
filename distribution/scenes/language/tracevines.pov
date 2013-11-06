// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

//    Persistence of Vision Raytracer Scene Description File
//    File: tracevines.pov
//    Author: Ron Parker
//    Description: Algorithm shamelessly lifted from
//     "Modeling Plants With Environment-Sensitive Automata"
//     Proceedings of Ausgraph '88, pages 27-33
//     by James Arvo and David Kirk
//
// -w320 -h240
// -w800 -h600 +a0.3
//
//*******************************************

#version 3.6;
global_settings {assumed_gamma 1.0}

#macro Interfere( A, B, Object )
  #local N=<0,0,0>;
  #local I=trace( Object, A, B-A, N );
  (vlength(N) & (vlength(I-A)<vlength(B-A)))
#end // macro

#macro FindTmp( CurPt, Normal, Object, RandSeed )
  #local More = 1;
  #local Safety = C3;
  #local Q = <0,0,0>;
  #while ( More & Safety )
    // select random unit T orthogonal to Normal
    #local T = <rand(RandSeed)-.5,rand(RandSeed)-.5, rand(RandSeed)-.5>;
    #local T = T-vdot(T,Normal)*Normal;
    #if ( vlength(T))
      #local T = T/vlength(T);
      #local Safety = Safety-1;
      #local Q = CurPt + C2 * T;
      #local More = Interfere( CurPt, Q, Object );
    #end // if
  #end // while
  #if (Safety)
    Q;
  #else
    <0,0,0>;
  #end // if
#end // macro

#macro Draw( CurPt, NewPt, Normal, NewNormal )
  union {
    sphere {CurPt, R1}
    sphere {NewPt, R1}
    cylinder {CurPt, NewPt, R1}
    #local Pr = vcross(NewPt-CurPt, NewNormal );
    #local Pl = vnormalize(NewPt-CurPt);
    #local Or = vnormalize(.3*NewNormal+.7*Pl);
    triangle {NewPt, NewPt+LL*Or, NewPt+.5*LL*Or+.5*LW*Pr
              translate R1*NewNormal}
    triangle {NewPt, NewPt+LL*Or, NewPt+.5*LL*Or-.5*LW*Pr
              translate R1*NewNormal}
    texture {
      pigment {color green 1}
    }
  }
#end // macro

#macro Grow( Start, Normal, Object, RandSeed )

  #local Continue = 1;
  #while ( Continue )
    #ifndef (Watchdog)
      #local Watchdog = C8;
    #else
      #declare Watchdog = Watchdog - 1;
    #end // ifndef

    #ifndef (Gen)
      #local Gen = 0;
    #else
      #local Gen2 = Gen+1;
      #local Gen = Gen2;
    #end // if

    #local Continue = 0;
    #local Branch = 0;

    #if ( Watchdog )

      #local CurPt = Start + C1 * Normal;
      #local NewTmp = FindTmp( CurPt, Normal, Object, RandSeed )
      #if (vlength( NewTmp ))

        #local Dist = 9999;
        #local NewRoot = Start;
        #local NewNormal = Normal;
        #local NewPt = CurPt;
        #local Iter = C3;
        #while (Iter)
          #local R = <rand(RandSeed)-.5,rand(RandSeed)-.5,
                      rand(RandSeed)-.5>+Bias;
          #local N = <0,0,0>;
          #local Int = trace( Object, NewTmp, R, N );
          #if ( vlength(N) )
            #local CurDist = vlength( Int-Start );
            #local TestPt = Int + C1 * N;
            #if ( (CurDist < Dist) & (CurDist < C4) &
                   !Interfere( CurPt, TestPt, Object ))
              #local Dist = CurDist;
              #local NewRoot = Int;
              #local NewNormal = vnormalize(N);
              #local NewPt = Int+ C1 * N;
            #end // if shorter dist
          #end // if N
          #local Iter = Iter-1;
        #end // while Iter
        #if ( vlength(CurPt-NewPt))
          Draw( CurPt, NewPt, Normal, NewNormal )
          #if (rand(RandSeed) > C5 & Gen < C7)
            #local Continue = 1;
            #if (rand(RandSeed)<C6)
              #local Branch = 1;
            #end // if branch
          #end // if continue
        #end // if new point
      #end // if NewTmp found
    #end // if watchdog

    #if (Branch)
      Grow( NewRoot, NewNormal, Object, RandSeed )
    #end // if branch
  #local Start = NewRoot;
  #local Normal = NewNormal;
  #end // while continue
#end // macro

#declare Fence = union {
  cylinder {-2.2*x, <-2.2,2,0>, .2}
  cylinder {0, <0,2,0>, .2}
  cylinder {2.2*x, <2.2,2,0>, .2}
  cylinder {<2.2,1.7,0> <-2.2,1.7,0> .1}
  cylinder {<2.2,1,0> <-2.2,1,0> .1}
  plane {y 0 pigment {bozo color_map {[0 rgb <1,.8,.5>][1 rgb <.8,.5,.1>]}}}
  translate -.02*y
  texture {
    pigment {
      color rgb <.5,.25,.1>
    }
  }
}

#declare C1 =  .02;  // distance of the vine from the object it grows on
#declare C2 =  .05;  // Approximate step distance
#declare C3 =  60;  // number of attempts to find a surface to take root
#declare C4 =  .1;  // Maximum jump between roots
#declare C5 = .05;  // probability of quitting after each generation
#declare C6 = .3;  // probability of branching after each generation
#declare C7 =  150;  // absolute maximum generations
#declare C8 = 10000; // absolute maximum generations along all branches

#declare R1 = .01;  // radius of vine

#declare LL = .1;   // length of leaf
#declare LW = .1;   // width of leaf

#declare Bias = <-.2,.4,0>;
#declare RandSeed = seed(347);

#declare Sa=array[12] {
  <-2,0,0>,<-2.4,0,0>,<-2.2,0,.2>,<-2.2,0,-.2>, // left post
  <2,0,0>,<2.4,0,0>,<2.2,0,.2>,<2.2,0,-.2>, // right post
  <-.2,0,0>,<.2,0,0>,<0,0,.2>,<0,0,-.2> // center post
}

#declare Na=array[12] {x,-x,z,-z,-x,x,z,-z,-x,x,z,-z}

#declare Count=0;

#while (Count<12)
  #debug concat("plant ",str(Count+1,0,0), "\n")
  Grow( Sa[Count], Na[Count], Fence, RandSeed )
  #declare Count=Count+1;
#end

object {Fence}
light_source {<-20,20,-20> rgb 1}
camera { location <-2,3,-4>
         right     x*image_width/image_height
         look_at <-0.4,1,0>
         angle 56
       }
