// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence of Vision Ray Tracer version 3.71 Include File
// File: arrays.inc
// Last updated: June 3 2016
// Description: Array manipulation macros...sorting, resizing, etc.

#ifndef(ARRAYS_INC_TEMP)
#declare ARRAYS_INC_TEMP = version;
#version 3.71;

#ifdef(View_POV_Include_Stack)
   #debug "including arrays.inc\n"
#end

// Input:  a one dimensional array and a random stream
// Output: a random item from the input array
// Source: variate.inc by Ingo Janssen
#macro Rand_Array_Item(Array, Stream)
   #if(dimensions(Array)=1)
      Array[floor(rand(Stream)*0.9999999*dimension_size(Array,1))]
   #else
      #error "The Rand_Array_Item() macro only works for 1D arrays."
   #end
#end


#macro Resize_Array(Array, NewSize)
   #if(dimensions(Array)=1)
      #local NewArray = array[NewSize]
      #local C = 0;
      #local Max = min(NewSize, dimension_size(Array, 1));
      #while (C < Max)
         #ifdef (Array[C])
            #local NewArray[C] = Array[C];
         #end
         #local C = C + 1;
      #end
      #declare Array = NewArray
   #else
      #error "The Resize_Array() macro only works for 1D arrays."
   #end
#end


#macro Reverse_Array(Array)
   #if(dimensions(Array)=1)
      #local J = 0;
      #local N = dimension_size(Array, 1) - 1;
      #while(J < N/2)
         #local Temp = Array[J];
         #local Array[J] = Array[N-J];
         #local Array[N-J] = Temp;
         #local J = J + 1;
      #end
   #else
      #error "The Reverse_Array() macro only works for 1D arrays."
   #end
#end


// #macro Insert_In_Array(Item, DestArray, Index)
// #macro Insert_Array_In_Array(ItemArray, DestArray, Index)

// #macro Append_To_Array(Item, DestArray, Index)
// #macro Append_Array_To_Array(ItemArray, DestArray, Index)

// This macro will compare float values and vector lengths.
// It is intended to be redefined by the user to suit their
// needs, but it should always return true if A is "less than"
// B, otherwise false.
// #macro SortCompare(A, B) (vlength(A) < vlength(B)) #end
// #macro SortCompare(Array, IdxA, IdxB) (vlength(Array[IdxA]) < vlength(Array[IdxB])) #end
#macro Sort_Compare(Array, IdxA, IdxB) (Array[IdxA] < Array[IdxB]) #end

// This macro swaps the data in two locations. The user can
// redefine it, for example to swap additional data along a
// second dimension, keeping columns of data together.
#macro Sort_Swap_Data(Array, IdxA, IdxB)
   #local Tmp = Array[IdxA];
   #declare Array[IdxA] = Array[IdxB];
   #declare Array[IdxB] = Tmp;
#end


// Sort macros slightly modified from QuickSort macros by Juha Nieminen
#macro Sort_Array(Array)
   Sort_Partial_Array(Array, 0, dimension_size(Array, 1)-1)
#end

#macro Sort_Partial_Array(Array, FirstInd, LastInd)
   ARRAYS_HybridQuickSortStep(Array, FirstInd, LastInd)
   ARRAYS_InsertionSort(Array, FirstInd, LastInd)
#end

#declare ARRAYS_QuickSortSeed = seed(0);

#macro ARRAYS_HybridQuickSortStep(Array, FirstInd, LastInd)
   #local FInd = FirstInd;
   #while(FInd < LastInd-10)
      #local RInd = FInd + rand(ARRAYS_QuickSortSeed)*(LastInd - FInd);
      Sort_Swap_Data(Array, FInd, RInd)
      #local I = FInd-1;
      #local J = LastInd+1;
      #local Mid = -1;
      #while(Mid < 0)
         #local J = J-1;
         #while(Sort_Compare(Array, FInd, J)) #local J = J-1; #end
         #local I = I+1;
         #while(Sort_Compare(Array, I, FInd)) #local I = I+1; #end

         #if(I < J)
            Sort_Swap_Data(Array, I, J)
         #else
            #local Mid = J;
         #end
      #end
      ARRAYS_HybridQuickSortStep(Array, FInd, Mid)
      #local FInd = Mid+1;
   #end
#end

#macro ARRAYS_InsertionSort(Array, FirstInd, LastInd)
   #local Ind1 = FirstInd+1;
   #while(Ind1 <= LastInd)
      #local Ind2 = Ind1;
      #while(Ind2 > FirstInd)
         #local NextInd2 = Ind2-1;
         #if(Sort_Compare(Array, Ind2, NextInd2))
            Sort_Swap_Data(Array, Ind2, NextInd2)
            #local Ind2 = NextInd2;
         #else
            #local Ind2 = 0;
         #end
      #end
      #local Ind1 = Ind1+1;
   #end
#end

#macro ARRAYS_WriteDF3(Array, FileName, BitDepth)

  #switch (BitDepth)
  #case (8)
    #macro ARRAYS_Temp_WriteDF3(ArrayItem)
      #ifdef (ArrayItem)
        #write(ARRAYS_Temp_File, uint8 ArrayItem * 255)
      #else
        #write(ARRAYS_Temp_File, uint8 0)
      #end
    #end
  #break
  #case (16)
    #macro ARRAYS_Temp_WriteDF3(ArrayItem)
      #ifdef (ArrayItem)
        #write(ARRAYS_Temp_File, uint16be ArrayItem * 65535)
      #else
        #write(ARRAYS_Temp_File, uint16be 0)
      #end
    #end
  #break
  #case (32)
    #macro ARRAYS_Temp_WriteDF3(ArrayItem)
      #ifdef (ArrayItem)
        #write(ARRAYS_Temp_File, uint32be ArrayItem * 4294967295)
      #else
        #write(ARRAYS_Temp_File, uint32be 0)
      #end
    #end
  #break
  #else
    #error "ARRAYS_WriteDF3: bit depth not supported\n"
  #end

  #switch (dimensions(Array))
  #case (1)
    #local SizeX = dimension_size(Array,1);
    #local SizeY = 1;
    #local SizeZ = 1;
    #macro ARRAYS_Temp(Arr,IX,IY,IZ) Arr[IX] #end
  #break
  #case (2)
    #local SizeX = dimension_size(Array,1);
    #local SizeY = dimension_size(Array,2);
    #local SizeZ = 1;
    #macro ARRAYS_Temp(Arr,IX,IY,IZ) Arr[IX][IY] #end
  #break
  #case (3)
    #local SizeX = dimension_size(Array,1);
    #local SizeY = dimension_size(Array,2);
    #local SizeZ = dimension_size(Array,3);
    #macro ARRAYS_Temp(Arr,IX,IY,IZ) Arr[IX][IY][IZ] #end
  #break
  #else
    #error "ARRAYS_WriteDF3: number of dimensions not supported\n"
  #end

  #if ((SizeX > 65535) | (SizeY > 65535) | (SizeZ > 65535))
    #error "ARRAYS_WriteDF3: dimension size not supported\n"
  #end
  #fopen ARRAYS_Temp_File FileName write
  #write (ARRAYS_Temp_File, uint16be <SizeX,SizeY,SizeZ>)
  #local IndZ = 0;
  #while (IndZ < SizeZ)
    #local IndY = 0;
    #while (IndY < SizeY)
      #local IndX = 0;
      #while (IndX < SizeX)
        ARRAYS_Temp_WriteDF3(ARRAYS_Temp(Array,IndX,IndY,IndZ))
        #local IndX = IndX + 1;
      #end
      #local IndY = IndY + 1;
    #end
    #local IndZ = IndZ + 1;
  #end
  #undef ARRAYS_Temp
  #undef ARRAYS_Temp_WriteDF3
  #fclose ARRAYS_Temp_File
#end

#version ARRAYS_INC_TEMP;
#end

