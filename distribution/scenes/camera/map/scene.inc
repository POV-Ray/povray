#include "colors.inc"
#default { finish { ambient 1.0 } }

//background { White /2 }

#declare av_ra=800;
#declare mar=av_ra/600;
union {
sphere { 0,av_ra 
//	texture { pigment { White } } }
texture { 
checker texture { pigment { color rgb <1,1,0> } },texture { pigment {color rgb 0.5} }
 scale 1/4 warp { spherical orientation y } } }
#local  colo=array[4];
#local colo[0]=rgb<0,0,1/2>;
#local colo[1]=rgb<1/2,0,0>;
#local colo[2]=rgb<0,1/2,0>;
#local colo[3]=rgb<0,0,0>;

	cylinder { y*(av_ra*1.05),-y*(av_ra*1.05), 3 texture { pigment { color rgb<1,1,0> } } }
	cylinder { y*(av_ra*1.1),-y*(av_ra*1.1), 2 texture { pigment { color rgb<1,1,1> } } }
	cylinder { y*(av_ra*1.15),-y*(av_ra*1.15), 1 texture { pigment { color rgb<1,0,1> } } }
	cylinder { y*(av_ra*1.2),-y*(av_ra*1.2), 0.5 texture { pigment { color rgb<0,1,1> } } }
#for(i,0,180,5)
#if (mod(i,2))
#local rr=0;
#else
#local rr=1;
#end
#if (mod(i,3))
#local gg=0;
#else
#local gg=1;
#end
#local tt = texture { pigment { color colo[rr+2*gg]  } };
torus { av_ra,mar *(1+rr*gg)  sturm texture { tt } rotate 90*z rotate i*y }
#local aa=av_ra*sin(radians(i));
#local bb=av_ra*cos(radians(i));
torus { aa,mar*(1+rr*gg) sturm texture { tt } translate (bb)*y }
#end
}
