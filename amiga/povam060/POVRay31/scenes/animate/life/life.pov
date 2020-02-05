/*
Life.Pov Celular autotomata life animation for POV-Ray 3.0
   by JIm WIlliamson 100041,2563
   using a character string to simulate arrays.
Adapted for POV-Ray 3.1 using arrays by Chris Young
*/

camera {location 100*y  direction z*2.1  look_at <0,0,0>}
light_source { <10,100,5> color rgb 1}

#declare LifeObject=sphere{<0,0,0>,0.49 no_shadow}
#declare LifeTex=texture { pigment {rgb 1} }

#if (file_exists("lifedata.inc"))
   #include "lifedata.inc"
#else
   #declare NumCols=60;
   #declare NumRows=40;
   #declare Gen    =0;
   #declare Ratio=0.75;
   #declare LifeGrid=array[NumRows][NumCols]
   #declare LR=seed(554);

   // Initialize grid with random values
   #declare Row     = 0;
   #while(Row<NumRows)
      #declare Column=0;
      #while (Column<NumCols)
         #if(rand(LR)>Ratio)
            #declare LifeGrid[Row][Column]=1;
         #else
            #declare LifeGrid[Row][Column]=0;
         #end
         #declare Column=Column+1;
      #end
      #declare Row=Row+1;
   #end
#end

#declare NC2=-NumCols/2;
#declare NR2=-NumRows/2;

// Display results
#declare Row = 0;
#while (Row < NumRows)
   #declare Column = 0;
   #while (Column < NumCols)
         object {
           LifeObject
      #if (LifeGrid[Row][Column])
           texture {LifeTex}
      #else
           pigment{rgb<1,0,0>}
           scale .5
      #end
           translate<Column,0,Row>
           translate<NC2,0,NR2>
        }
      #declare Column = Column + 1;
   #end
   #declare Row = Row + 1;
#end

#debug concat("Generation ",str(Gen,3,0),"\n")

#declare Gen=Gen+1;

// Compute next generation
#fopen LifeNext "lifedata.inc" write

#write (LifeNext, "#declare Gen=",str(Gen,4,0),";\n")
#write (LifeNext, "#declare NumCols=",str(NumCols,4,0),";\n")
#write (LifeNext, "#declare NumRows=",str(NumRows,4,0),";\n")
#write (LifeNext, "#declare LifeGrid=array[NumRows][NumCols]\n")

#write (LifeNext, "{")

#declare Row = 0;
#while (Row < NumRows)
   #if (Row)
      #write (LifeNext, ",\n")
   #end
   #write (LifeNext, "{")

   #declare Column = 0;
   #while (Column < NumCols)
      #if (Column)
         #write (LifeNext, ",")
      #end

      #declare Near = 0;
      #declare YY = Row - 1;
      #while (YY <= Row + 1)
         #declare Y3=mod(YY+NumRows,NumRows);
         #declare XX = Column - 1;
         #while (XX <= Column + 1)
            #declare X3=mod(XX+NumCols,NumCols);
            #if ((XX=Column)&(YY=Row))
            #else
               #declare Near=Near+LifeGrid[Y3][X3];
            #end
            #declare XX = XX + 1;
         #end
         #declare YY = YY + 1;
      #end
      #if (LifeGrid[Row][Column])
         #if((Near=2) | (Near=3))
            #write (LifeNext, "1")
         #else
            #write (LifeNext, "0")
         #end
      #else
         #if (Near=3)
            #write (LifeNext, "1")
         #else
            #write (LifeNext, "0")
         #end
      #end
      #declare Column = Column + 1;
   #end
   #declare Row = Row + 1;
   #write (LifeNext, "}")
#end

#write (LifeNext, "}\n")

#fclose LifeNext


