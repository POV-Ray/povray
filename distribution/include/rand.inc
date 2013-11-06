// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

//    Persistence of Vision Ray Tracer version 3.5 Include File
//    File: rand.inc
//    Last updated: 2001.8.9
//    Description: some predefined random number generators, and
//    macros for working with random numbers.
//    Random number distribution macros contributed by Ingo Janssen.

#ifndef(RAND_INC_TEMP)
#declare RAND_INC_TEMP = version;
#version 3.5;

#ifdef(View_POV_Include_Stack)
   #debug "including rand.inc\n"
#end

#include "consts.inc"

//--------------------
//Random number generators:
//--------------------
#declare RdmA = seed(574647);// Random stream A
#declare RdmB = seed(324879);// Random stream B
#declare RdmC = seed(296735);// Random stream C
#declare RdmD = seed(978452);// Random stream D

//--------------------
//Random number macros:
//--------------------

//Probability, returns true or false.
//P is probability of returning true, RS is a random number stream.
#macro Prob(P, RS) (rand(RS) < P) #end


/////////////////////////////////////////
// Continuous Symmetric Distributions //
////////////////////////////////////////
#declare Gauss_Next = false;

// Cauchy distribution
// Input:  Mu=mean, Sigma= standard deviation and a random stream
#macro Rand_Cauchy(Mu, Sigma, Stream)
   (Sigma*tan(pi*(rand(Stream)-0.5))+Mu)
#end

// Student's-t distribution
// Input: N= degrees of freedom and a random stream.
#macro Rand_Student(N, Stream)
   (Rand_Gauss(0,1,Stream)/sqrt(Rand_Chi_Square(N,Stream)/N))
#end

// Normal distribution
// Input:  Mu=mean, Sigma= standard deviation and a random stream
// Output: a random value in the range of the normal distribution
//         defined by the standard deviation, around the mean.
#macro Rand_Normal(Mu, Sigma, Stream)
   #local Cn=4*exp(-0.5)/sqrt(2);
   #local Loop=true;
   #while (Loop)
      #local R=rand(Stream);
      #local V=Cn*(rand(Stream)-0.5)/R;
      #local VV=V*V/4;
      #if (VV<=-ln(R))
         #local Loop=false;
      #end
   #end
   (Mu+(V*Sigma))
#end

// Gaussian distribution
// like Rand_Normal, but a bit faster
#macro Rand_Gauss(Mu, Sigma, Stream)
   #local Zgauss=Gauss_Next;
   #declare Gauss_Next=false;
   #if (!Zgauss)
      #local R1=rand(Stream)*2*pi;
      #local R2=sqrt(-2*ln(1-rand(Stream)));
      #local Zgauss=cos(R1)*R2;
      #declare Gauss_Next=sin(R1)*R2;
   #end
   (Mu+(Zgauss*Sigma))
#end


/////////////////////////////////////
// Continuous Skewed Distributions //
/////////////////////////////////////

// Input:  spline and a random stream.
// Output: a random value in the range 0 - 1.
// The probability of the value is controled
// by the spline. The splines clock_value is
// the output value and the .y value its chanche.
#macro Rand_Spline(Spl, Stream)
   #local I=1;
   #while (I)
      #declare cVal=rand(Stream);
      #if (Spl(cVal).y>=rand(Stream))
         #local I=0;
         (cVal)
      #end
   #end
#end

// Gamma distribution
// Input: Alpha= shape parameter >0, Beta= scale parameter >0 and a random stream.
#macro Rand_Gamma(Alpha, Beta, Stream)
   #if(Alpha<=0 | Beta<=0)
       #error "Alpha and Beta should be bigger than 0"
   #end
   #local Ainv=sqrt(2*Alpha-1);
   #local BBB=Alpha-ln(4);
   #local CCC=Alpha+Ainv;
   #if (Alpha>1)
      #local Loop = true;
      #while (Loop)
         #local R1=rand(Stream);
         #local R2=rand(Stream);
         #local V=ln(R1/(1-R1))/Ainv;
         #local X=Alpha*exp(V);
         #local Z=R1*R1*R2;
         #local R=BBB+CCC*V-X;
         #local RZ=R+(1+ln(4.5))-4.5*Z;
         #if (RZ>=0 | R>=ln(Z))
            #local Loop=false;
            #local RETURN=X;
         #end
      #end
   #end
   #if (Alpha=1)
      #local R=rand(Stream);
      #while (R<=1e-7)
         #local R=rand(Stream);
      #end
      #local RETURN=-ln(R);
   #end
   #if (Alpha>0 & Alpha<1)
      #local Loop=true;
      #while (Loop)
         #local R=rand(Stream);
         #local B=(e+Alpha)/e;
         #local P=B*R;
         #if (P<=1)
            #local X=pow(P, (1/Alpha));
         #else
            #local X=-ln((B-P)/Alpha);
         #end
         #local R1=rand(Stream); 
         #if(!( ((P<=1) & (R1>exp(-X))) | ((P>1) & (R1>pow(X,Alpha-1))) ))
             #local RETURN=X;
             #local Loop=false;
         #end
      #end
   #end
   #local Return=Beta*RETURN;
   Return
#end

// Beta variate
// Input: Alpha= shape Gamma1, Beta= shape Gamma2 and a random stream.
#macro Rand_Beta(Alpha, Beta, Stream)
   #if(Alpha<=0 | Beta<=0)
      #error "Alpha and Beta should be bigger than 0"
   #end 
   #local Gamma1=Rand_Gamma(Alpha,1,Stream);
   #if (Gamma1=0)
      #local Return=0;
   #else
      #local Return=(Gamma1/(Gamma1+Rand_Gamma(Beta,1,Stream)));
   #end
   (Return)
#end

// Chi Square random variate
// Input: N= degrees of freedom int() and a random stream
#macro Rand_Chi_Square(N, Stream)
   (Rand_Gamma(2,0.5*int(N),Stream))
#end

// F-Distribution
// Input: N, M degrees of freedom and a random stream.
#macro Rand_F_Dist(N, M, Stream)
   #local C1=Rand_Chi_Square(M,Stream);
   #local C2=Rand_Chi_Square(N,Stream);
   #local Return=(M*C1)/(N*C2);
   (Return)
#end

//Triangular distribution
//Input: Min, Max, Mode (Min < Mode < Max) and a random stream
#macro Rand_Triangle(Min, Max, Mode, Stream)
   #local Right=Max-Mode;
   #local Left=Mode-Min;
   #local Range=Max-Min;
   #local R=rand(Stream);
   #if(R<=Left/Range)
      #local Return= Min+sqrt(Left*Range*R);
   #else
      #local Return= Max-sqrt(Right*Range*(1-R));
   #end
   (Return)
#end

// Erlang variate
// Input: Mu= mean >=0, K= number of exponential samples and a random stream.
#macro Rand_Erlang(Mu, K, Stream)
   #local Prod=1;
   #local I=0;
   #while(I<K)
      #local Prod=Prod*rand(Stream);
      #local I=I+1;
   #end
   (-Mu*ln(Prod))
#end

// Exponential distribution
// Input: Lambda = rate = 1/mean
#macro Rand_Exp(Lambda, Stream)
   (-ln(rand(Stream))/Lambda)
#end

// Lognormal distribution
// Input:  Mu=mean, Sigma= standard deviation and a random stream
#macro Rand_Lognormal(Mu, Sigma, Stream)
   (exp(Rand_Gauss(Mu,Sigma,Stream)))
#end

// Pareto distribution
#macro Rand_Pareto(Alpha, Stream)
   (1/pow(rand(Stream),(1/Alpha)))
#end

// Weibull distribution
#macro Rand_Weibull(Alpha, Beta, Stream)
   (Alpha*pow(-ln(rand(Stream)),(1/Beta)))
#end

////////////////////////////////////
//    Discrete Distribution       //
////////////////////////////////////

// Bernoulli distribution
// Input: P = probability range: 0 - 1. And a random stream.
// Output: the BOOLEAN value TRUE with a probability equal 
//         to the value of P and FALSE with a probability of 1 - P.
#macro Rand_Bernoulli(P,Stream)
   (P>=rand(Stream)?true:false)
#end

// Binomial distribution
// Input: N= number of trials, P= probability [0-1] and a random stream.
#macro Rand_Binomial(N, P, Stream)
   #local Count=0;
   #local N=int(N);
   #local I=0;
   #while (I<N)
      #if (rand(Stream)<=P)
         #local Count=Count+1;
      #end
      #local I=I+1;
   #end
   (Count)
#end

//Geometric distribution
//Input: P=probability [0-1] and a random stream.
#macro Rand_Geo(P, Stream)
   (floor(ln(rand(Stream))/ln(1-P)))
#end

// Poisson distribution
// Input: Mu= mean and a random stream.
#macro Rand_Poisson(Mu, Stream)
   #local Maxtimes = 100000;  //just to be sure
   #local Cut=exp(-Mu);
   #local N=0;
   #local R=1;
   #while (R>Cut)
      #local R=R*rand(Stream);
      #local N=N+1;
      #if(N>Maxtimes)
         #local R=Cut;
      #end
   #end
   (N)
#end



//signed random number, range [-1, 1]
#macro SRand(RS) (rand(RS)*2 - 1) #end

//random number in specified range [Min, Max]
#macro RRand(Min, Max, RS) (rand(RS)*(Max-Min) + Min) #end

//a random point in a box from < 0, 0, 0> to < 1, 1, 1>
#macro VRand(RS) < rand(RS), rand(RS), rand(RS)> #end

//a random point in a box from Mn to Mx
#macro VRand_In_Box(Mn, Mx, RS) (< rand(RS), rand(RS), rand(RS)>*(Mx-Mn) + Mn) #end

//a random point in a unit-radius sphere centered on the origin
//Thanks to Ingo for this macro, which is faster than the original VRand3()
#macro VRand_In_Sphere(Stream)
   #local R = pow(rand(Stream),1/3);
   #local Theta = 2*pi*rand(Stream);
   #local Phi = acos(2*rand(Stream)-1);
   (R*<cos(Theta)*sin(Phi),
       sin(Theta)*sin(Phi),
       cos(Phi)>)
#end

//a random point on a unit-radius sphere centered on the origin
//Author: Ingo
#macro VRand_On_Sphere(Stream)
   #local Theta = 2*pi*rand(Stream);
   #local Phi = acos(2*rand(Stream)-1);
   (<cos(Theta)*sin(Phi),
     sin(Theta)*sin(Phi),
     cos(Phi)>)
#end

//a random point inside an arbitrary object
//Warning: can be quite slow if the object occupies a small
//portion of the volume of it's bounding box!
//Also, will not work on objects without a definite "inside".
#macro VRand_In_Obj(Obj, RS)
    #local Mn = min_extent(Obj);
    #local Mx = max_extent(Obj);
    #local Pt = VRand_In_Box(Mn, Mx, RS);
    #local J = 0;
    #while(inside(Obj, Pt) = 0 & J < 1000)
        #local Pt = VRand_In_Box(Mn, Mx, RS);
        #local J = J + 1;
    #end
    (Pt)
#end


#version RAND_INC_TEMP;
#end//rand.inc

