// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence Of Vision raytracer sample file.
//
// ------------------------------
// Features pseudo-Gaussian distribution and use of trace function
// Collision algorithm by Greg M. Johnson 2001
// ------------------------------
// ------------------------------
// visibility test
// code by Gilles Tran, derived from work by John Van Sickle and Christoph Hormann
// ------------------------------
#declare CamD=vnormalize(CamEye-CamLoc);         // direction of camera view
#declare CamR=vnormalize(vcross(CamSky,CamD));   // to the right
#declare CamU=vnormalize(vcross(CamD,CamR));     // camera up

#declare Screen= // pseudo screen to test the visibility of an object
  mesh{
    triangle{0,x,x+y}
    triangle{0,y,x+y}
    translate -0.5*(x+y)
    scale <AspectRatio,1,1>
    translate CamZoom*z
    matrix < CamR.x,  CamR.y,  CamR.z,
             CamU.x,  CamU.y,  CamU.z,
             CamD.x,  CamD.y,  CamD.z,
             CamLoc.x,CamLoc.y,CamLoc.z >
  }
#macro IsObjectVisible(PosObject)
// this macro tests the position of the object against the pseudo screen
// note: it also returns false when the position is higher than y=10
#local Norm1 = <0, 0, 0>;
#local PosTemp= trace (Screen,PosObject,-PosObject+CamLoc,Norm1);
    #if (vlength(Norm1)!=0 & PosObject.y <10)
        true;
    #else
        false;
    #end
#end
//object{Screen pigment{rgbf<1,0,0,0.9>}}

// ------------------------------
// Greg Johnson's algorithm
// ------------------------------
// cube random position
// ------------------------------
#macro randsq(RRR)
        #local A=360*rand(RRR);
        #local D=5*(1/rand(RRR)-0.75);
        #if (D*sin(radians(A))>9) // clipped to let the POVRAY letters visible
            #local A = A+180;
        #end
        vaxis_rotate(x*D,y,A)

#end

// ------------------------------
// cube placement routine
// ------------------------------

#declare R2R=seed(1923);
#declare S = seed(14400);
#declare Norm=<0,0,0>;

#declare ntests=10;

#declare stored=array[ntests+2][ntests+2]
#declare Posy=array[num]

#declare Posy[0]=randsq(S);

// pseudo box used for collision testing
#declare Pseudo=mesh{triangle{0,x,x+z}triangle{0,z,x+z} translate -x*0.5-z*0.5+y*0.5}

// ------------------------------
// generates the first set of cubes that read POVRAY
// ------------------------------
#declare R=array[6]
#declare T=array[6]
#declare i=0;
#while (i<6) // rotations and positions are put in arrays so that they're the same in the file and in the "ally" object
    #declare R[i]=<0,-10+25*rand(R2R),0>;
    #declare T[i]=<-0.5+rand(R2R),0,rand(R2R)-0.5>;
    #declare i=i+1;
#end
#declare ally=union{
    object{Pseudo rotate R[0] translate <-4,0.5,-10>+0.35*T[0]}
    object{Pseudo rotate R[1] translate <-2.50,0.5,-10>+0.35*T[1]}
    object{Pseudo rotate x*90 rotate R[2] translate  <-1,0.5,-10>+0.35*T[2]}
    object{Pseudo rotate x*90 rotate R[3] translate < 0.50,0.5,-10>+0.35*T[3]}
    object{Pseudo rotate R[4] translate < 2,0.5,-10>+0.35*T[4]}
    object{Pseudo rotate R[5] translate  < 4,0.5,-10>+0.35*T[5]}
}



#fopen StackFile "stacks.inc" write
#write (StackFile, "union{\n")
#write (StackFile, "object{unitbox() rotate -90*y rotate ",R[0]," translate <-4,0.5,-10>+0.35*",T[0],"}\n")
#write (StackFile, "object{unitbox() rotate ",R[1]," translate <-2.5,0.5,-10>+0.35*",T[1],"}\n")
#write (StackFile, "object{unitbox() rotate 90*y rotate ",R[2]," translate <-1,0.5,-10>+0.35*",T[2],"}\n")
#write (StackFile, "object{unitbox() rotate 180*y rotate ",R[3]," translate <0.5,0.5,-10>+0.35*",T[3],"}\n")
#write (StackFile, "object{unitbox() rotate -90*x rotate ",R[4]," translate <2,0.5,-10>+0.35*",T[4],"}\n")
#write (StackFile, "object{unitbox() rotate 90*x rotate ",R[5]," translate <4,0.5,-10>+0.35*",T[5],"}\n")



// ------------------------------
// main placement loop
// runs only if WriteFile is true
// ------------------------------
#declare n=1;
#while(n<num)

        #debug concat(str(n,4,0),":")
        #declare rotey=rand(R2R)*360;

        #declare okayness=no;
        #declare counter=0 ;
        #while(counter<50)

            #declare testcenter=randsq(S);
            #declare Visible=IsObjectVisible(testcenter)
            #if (Visible)
                #declare maxy=-1000;

                #declare nx=0;
                #while(nx<ntests)
                    #declare nz=0;
                    #while (nz<ntests)

                        #declare testpoint=<-0.5,-0.5,-0.5>+<nx/ntests,0,nz/ntests>;

                        #declare tracey=trace(ally,testcenter+y*1000+vaxis_rotate(testpoint,y,rotey),-y,Norm);

                        #if (maxy<tracey.y)
                                #declare maxy=tracey.y;
                        #end

                        #declare stored[nx][nz]=tracey.y;

                        #declare nz=nz+1;
                    #end
                    #declare nx=nx+1;
                #end

                #declare nnnn=int(ntests/2);

                #if(abs(stored[nnnn][nnnn]-maxy)<0.01)
                    #declare okayness=yes;
                    #debug "[y]"
                #else

                    #debug "[n]"
                    #declare netbalancevect=<0,0,0>;

                    #declare nx=0;
                    #while(nx<ntests)
                        #declare nz=0;
                        #while (nz<ntests)

                            #if (abs(stored[nx][nz]-maxy)<0.01)
                                #declare netbalancevect=netbalancevect+<nx-nnnn,0,nz-nnnn>;

                            #end

                            #declare nz=nz+1;

                        #end
                        #declare nx=nx+1;
                    #end

                    #if (vlength(netbalancevect)<0.5)
                        #declare okayness=yes;
                    #end

                #end

                #if(okayness=yes)
                    #declare counter=counter+100;
                #end

                #declare counter=counter+1;
            #else
                #declare counter=counter+100;
                #debug "[out]"
            #end
        #end
        #if(Visible)
            #declare Posy[n]=testcenter+<0,maxy+0.5,0>;
            #declare Visible=IsObjectVisible(Posy[n])
            #if (Visible)
                #declare R[0]=<int(rand(R2R)*4)*90,0,int(rand(R2R)*4)*90>;
                #write (StackFile, "object{unitbox() rotate ",R[0]," rotate y*",rotey," translate ", Posy[n],"}\n")
                #declare ally=union{
                    object{ally}
                    object{Pseudo rotate y*rotey translate Posy[n]}
                }
            #else
                #debug "[out]"
            #end
        #end
        #debug "\n"

#declare n=n+1;
#end

#write (StackFile, "}")
#fclose StackFile
