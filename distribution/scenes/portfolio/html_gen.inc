// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

// Persistence of Vision Ray Tracer Scene Include File
// File: html_gen.inc
// Vers: 3.5
// Desc: Generates a html-file for the Portfolio.
// Date: 2001/07/28
// Auth: Ingo Janssen
//
// HTMLgen(FileName, OutName, Keyword, Data, NrH_Images, NrV_Images, IW, IH, Comment)
// FileName   = the name of the include file the data came from.
//              it is used for the <h3>Title</h3>.
// OutName    = the OutName should match with Output_File_Name in the ini-file!!!!
//              it is the base for the html file names.
// Keyword    = the stuff represented in the array: texture, pigment, material, color etc.
//              it is used in the <h3>Title<h/h3>
// HrH_Images = the amount of images per row in the table
// NrV_Images = the amount of images per collumn in the table
// IW         = The width of the image, this should be set in the ini-file!
// IH         = The hight of the image, this should be set in the ini-file!
// Comment    = A string containing comment, as formatted HTML. The comment will be inserted
//              at the end of the first generated page. If the comment is "" it will be seen as 
//              empty.


#declare Header1="<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">\n <html>\n  <head>\n   <title>\n    "
#declare Header2="\n   </title>\n  </head>\n  <body bgcolor=\"#7F7F7F\" link=\"#000000\" alink=\"#000000\" vlink=\"#000000\">\n   <div align=\"center\">\n"

#macro HTMLgen(FileName, OutName, Keyword, Data, NrH_Images, NrV_Images, IW, IH, Comment)

   #if (NrH_Images=0)
      #local NrH_Images=1;
      #debug "\nNrH_Images can't be '0': it is set to '1'\n"
   #end
   #if (NrV_Images=0)
      #local NrV_Images=1;
      #debug "\nNrV_Images can't be '0': it is set to '1'\n"   
   #end

   #local Len=dimension_size(Data,1);
   #local Padding=-1*strlen(str(Len-1,0,0));
   #local Nr_Images=NrH_Images*NrV_Images;
   #local Nr_Pages=ceil(Len/Nr_Images);
   
   #local htmlPrevious=""
   #local Image=1;
   #local Page=0;
   #while (Page<Nr_Pages)
      #local htmlName=concat(OutName,str(Page+1,0,0),".html")
      #local htmlNext=concat(OutName,str(Page+2,0,0),".html")

      #local Title=concat ("    <h3>",FileName,", ",Keyword," (page ",str(Page+1,0,0),"/",str(Nr_Pages,0,0),")</h3>\n")
      #local NavSingle=concat("    <p>\n    [ <font color=#4F4F4F>previous</font> ] [ <a href=\"index.html\">index</a> ] [ <font color=#4F4F4F>next</font> ]\n    </p>\n")
      #local NavFirst=concat("    <p>\n    [ <font color=#4F4F4F>previous</font> ] [ <a href=\"index.html\">index</a> ] [ <a href=\"",htmlNext,"\">next</a> ]\n    </p>\n")
      #local NavLast=concat("    <p>\n    [ <a href=\"",htmlPrevious,"\">previous</a> ] [ <a href=\"index.html\">index</a> ] [ <font color=#4F4F4F>next</font> ]\n    </p>\n")
      #local Nav=concat("    <p>\n    [ <a href=\"",htmlPrevious,"\">previous</a> ] [ <a href=\"index.html\">index</a> ] [ <a href=\"",htmlNext,"\">next</a> ]\n    </p>\n")
      
      #if (Page=0)
         #local Nav=NavFirst
      #end
      #if (Page=Nr_Pages-1)
         #local Nav=NavLast
      #end
      #if (Nr_Pages=1)
         #local Nav=NavSingle
      #end
      
      #fopen HTMLFile htmlName write
      #write(HTMLFile Header1)
      #write(HTMLFile htmlName)
      #write(HTMLFile Header2)
      #write(HTMLFile Title)
      #write(HTMLFile Nav)
      #write(HTMLFile "    <table>\n")
                                       
      #local V=0;
      #while (V<NrV_Images)
         #local INames=array[NrH_Images]
         #write(HTMLFile "     <tr>\n")
         
         #local H=0;
         #while (H<NrH_Images)           
            #local INames[H]=Data[Image-1]
            
            #local ImageName=concat(OutName,str(Image,Padding,0))
            #local ImageCell=concat("      <td><img src=\"",ImageName,".png\" alt=\"",Data[Image-1],"\" width=\"",str(IW,0,0),"\" height=\"",str(IH,0,0),"\"></td>\n")
            #write (HTMLFile ImageCell)
            
            #local Image=Image+1;
            #if(Image>Len)
               #local H=NrH_Images;
               #local V=NrV_Images;
            #else
               #local H=H+1;
            #end
         #end//H
         
         #write(HTMLFile "     </tr>\n     <tr>\n")
         #local I=0;
         #while (I<NrH_Images)
            #ifdef(INames[I])
               #local NameCell=concat("      <td>",INames[I],"</td>\n")
               #write(HTMLFile NameCell)
            #end
            #local I=I+1;
         #end
         #undef INames
         #write(HTMLFile "     </tr>\n")
         
         #local V=V+1;
      #end//V
      
      #write(HTMLFile "    </table>\n")
      #if (Page=0 & strlen(Comment) != 0)
         #write(HTMLFile concat("   </div>\n",Comment,"   <div align=\"center\">\n"))
      #end
      #write(HTMLFile Nav)
      #write(HTMLFile "   </div>\n")
      #write(HTMLFile "  </body>\n </html>")
      #fclose HTMLFile
      #debug concat("\nGenerated the file: ", htmlName)
      #local htmlPrevious=htmlName
      #local Page=Page+1;
   #end//Page
#end
