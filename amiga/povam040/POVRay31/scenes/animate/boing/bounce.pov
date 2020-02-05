#declare Gravity=-2*y;
#declare Elastic=0.7;
#declare Time_Delta=0.25;

camera {location <0,10.5,-100>  look_at <0,11,0> }

light_source {<30, 120, -100> rgb 1}

#macro VReplace(V,A,N)
  #local V=(V-V*A)+N*A;
#end

#macro V2F(V,A)
 #local T=V*A;
 (T.x+T.y+T.z)
#end

#macro Check_Bounce(Pos,Vel,Axis,InOut,Dist)
  #local PosA=V2F(Pos,Axis);            // float position along axis
  #if (((InOut < 0.0 ) & (PosA > Dist)) // Right of right barrier?
       |                                //  or... 
       ((InOut > 0.0 ) & (PosA < Dist)) // left of left barrier?
      ) 
    #local PosA = Dist - (PosA-Dist);   // New position is outside by amount inside
    VReplace(Pos,Axis,PosA)             // Replace new position
    VReplace(Vel,Axis,-Vel*Elastic)     //Reverse velosity along axis
  #end
#end

union{
  plane { x, -60 } // left wall
  plane {-x, -60 } // right wall
  plane {-z, -40 } // back wall
  plane { y, -10 } // floor
  pigment {rgb 0.85}
}

#if (file_exists("bounce.txt"))
  #fopen Previous "bounce.txt" read
  #read (Previous,Position,Velocity)
  #fclose Previous 
#else
  #declare Position=<-45,40,5>;  // Initial position
  #declare Velocity=<5,1,5>;     // Initial velocity
#end

sphere { Position, 10
        pigment { rgb 1}
}

#declare NewPos=Position + Velocity*Time_Delta + Gravity/2*Time_Delta*Time_Delta;
#declare NewVel=Velocity + Gravity*Time_Delta;

Check_Bounce (NewPos,NewVel,x,+1,-50)
Check_Bounce (NewPos,NewVel,x,-1, 50)
Check_Bounce (NewPos,NewVel,y,+1,  0)
Check_Bounce (NewPos,NewVel,z,-1, 30)
Check_Bounce (NewPos,NewVel,z,+1,-20)

#fopen Previous "bounce.txt" write

#write (Previous,NewPos,",",NewVel,"\n")

#fclose Previous 

