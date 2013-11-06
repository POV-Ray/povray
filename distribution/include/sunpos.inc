// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence of Vision Ray Tracer Scene Include File
// File: sunpos.inc
// Desc: This macro returns the suns position,          
//       on a given date, time, and location on earth.  
// Date: 15-09-1998
// Updated : 2001-07-27
// Auth: Ingo Janssen
//

#ifndef(SunPos_Inc_Temp)
#declare SunPos_Inc_Temp=version;
#version 3.5;

//
// ======================================================
//
// Assumption: in the scene north is in the +Z direction, south is -Z.
//
// Invoke in your scene:
//   light_source {
//      SunPos(Year, Month, Day, Hour, Minute, Lstm, LAT, LONG)
//      rgb 1
//   }
//
// Greenwich, noon on the longest day of 2000 (no daylight saving):
//   SunPos(2000, 6, 21, 12, 2, 0, 51.4667, 0.00) 
//
// Year:  in four digits
// Month: number
// Dat:   number
// Time:  in 24 hour format
// Lstm:  Meridian of your local timezone (+1 hour =  +15 deg)
//        in degrees (east = positive, west = negative)
// Location on earth: 
// LAT:   Lattitude in degrees.decimal, northern hemisphere = positive, southern = negative
// LONG:  Longitude in degrees.decimal, east = positive, west is negative
//
// The macro returns the position of the sun, but also declares it as the vector SolarPosition. 
// Also the found Azimuth (Az) and Altitude (Al) are declared, this can be usefull for 
// aligning an object with the sunlight: cylinder{
//                                          <-2,0,0>,<2,0,0>,0.1
//                                          rotate <0, Az-90, Al>
//                                          texture {.. LightRay ..}
//                                        }
//
// ======================================================
//
// Find your local position at http://gnpswww.nima.mil/geonames/GNS/ (Gazetteer search)
//
// Local time(zone) at http://www.hilink.com.au/times/
//
// Equations used here can be found at http://hotel04.ausys.se/pausch/english.htm (Computing rise/set times)
//  not only for the sun but also for the moon, planets and other stuff up there.
//
// ======================================================
//

#macro SunPos(Year, Month, Day, Hour, Minute, Lstm, LAT, LONG)

   #if (abs(LONG-Lstm)>30)
       #debug "\nREMARK: \nLongitude differs by more than 30 degrees from timezone meridian.\n"
       #debug concat("Local timezone meridian is:",str(Lstm,5,0),"\n")
       #debug concat("Longitude is:",str(LONG,5,0),"\n")
   #end
   
       // Calculate universal time (UT)
   #local T= Hour+(Minute/60);
   #local UT= T-Lstm/15;
   #if (0>UT)
       #local Day= Day-1;
       #local UT= 24+UT;
   #end
   #if (UT>24)
       #local Day= Day+1;
       #local UT= UT-24;
   #end
   
       // Amount of days to, or from, the year 2000
   #local d= 367*Year-int((7*int((Year+int((Month+9))/12)))/4)+int((275*Month)/9)+Day-730530+UT/24;
   
       // Longitude of perihelion (w), eccentricity (e)
   #local w= 282.9404+4.70935E-5*d;
   #local e= 0.016709-1.151E-9*d;
   
       // Mean anomaly (M), sun's mean longitude (L)
   #local M= 356.0470+0.9856002585*d;
   #if (0<M<360)
       #local M= M-floor(M/360)*360;
   #end
   #local L= w+M;
   #if (0<L<360)
       #local L= L-floor(L/360)*360;
   #end
   
       // Obliquity of the ecliptic, eccentric anomaly (E)
   #local oblecl= 23.4393-3.563E-7*d;
   #local E= M+(180/pi)*e*sin(radians(M))*(1+e*cos(radians(M)));
   
       // Sun's rectangular coordinates in the plane of ecliptic (A,B)
   #local A= cos(radians(E))-e;
   #local B= sin(radians(E))*sqrt(1-e*e);
   
       // Distance (r), true anomaly (V), longitude of the sun (lon)
   #local r= sqrt(A*A+B*B);
   #local V= degrees(atan2(radians(B),radians(A)));
   #local lon= V+w;
   #if (0<lon<360)
       #local lon= lon-floor(lon/360)*360;
   #end
   
       // Calculate declination (Decl) and right ascension (RA)
   #local Decl= degrees(asin(sin(radians(oblecl))*sin(radians(lon))));
   #local RA= degrees(atan2(sin(radians(lon))*cos(radians(oblecl)),cos(radians(lon))))/15;
   
       // Greenwich meridian siderial time at 00:00 (GMST0),siderial time (SIDTIME), hour angle (HA)
   #local GMST0= L/15+12;
   #local SIDTIME= GMST0+UT+LONG/15;
   #local HA= (SIDTIME-RA)*15;
   
       // This is what we're looking for: Altitude & Azimuth
   #declare Al= degrees(asin(sin(radians(LAT))*sin(radians(Decl))+cos(radians(LAT))*cos(radians(Decl))*cos(radians(HA))));
   #declare Az= degrees(atan2(sin(radians(HA)),cos(radians(HA))*sin(radians(LAT))-tan(radians(Decl))*cos(radians(LAT))))+180;
   
   #declare SolarPosition=vrotate(<0,0,1000000000>,<-Al,Az,0>);
   (SolarPosition)

#end

#version SunPos_Inc_Temp;
#end
