#declare Random_1 = seed (10053); // Use: "rand(Random_1)" 
#declare Random_2 = seed ( 4953); // Use: "rand(Random_2)"


union{
 // outer loop
 #local NrX = -5;    // start x
 #local EndNrX = 5; // end   x
 #while (NrX< EndNrX) 
       // inner loop
       #declare NrZ = 0;    // start z
       #declare EndNrZ = 10; // end   z
       #while (NrZ< EndNrZ) 
     
       sphere{ <0,0,0>,0.15
               texture { pigment{ color rgb< 1.0, 0.5, 0.0> }  
                         finish {  diffuse 0.9 phong 1}
                       } // end of texture 
        
        //    translate<NrX*0.5 0.15 NrZ*0.5>} 
              translate<  NrX*0.5 + 0.35*(-0.5+rand(Random_1)) , 
                          0.15 , 
                          NrZ*0.5 + 0.35*(-0.5+rand(Random_2)) 
                        >
            } // end of sphere  

       #declare NrZ = NrZ + 1;  // next Nr z
       #end // --------------- end of loop z
       // end inner loop
 #local NrX = NrX + 1;  // next Nr x
 #end // --------------- end of loop x
 // end of outer loop
rotate<0,0,0> 
translate<0,0.0,-0.5>} // end of union
