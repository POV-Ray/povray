//******************************************************************************
///
/// @file windows/cmedit/povlangdef.h
///
/// This file is part of the CodeMax editor support code.
///
/// @author Christopher J. Cason
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2017 Persistence of Vision Raytracer Pty. Ltd.
///
/// POV-Ray is free software: you can redistribute it and/or modify
/// it under the terms of the GNU Affero General Public License as
/// published by the Free Software Foundation, either version 3 of the
/// License, or (at your option) any later version.
///
/// POV-Ray is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU Affero General Public License for more details.
///
/// You should have received a copy of the GNU Affero General Public License
/// along with this program.  If not, see <http://www.gnu.org/licenses/>.
///
/// ----------------------------------------------------------------------------
///
/// POV-Ray is based on the popular DKB raytracer version 2.12.
/// DKBTrace was originally written by David K. Buck.
/// DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
///
/// @endparblock
///
//******************************************************************************

CodemaxLanguage LangPOVRay =
{
  0,
  true,                 // case-sensitive flag
  NULL,                 // keywords; these are passed in from PVENGINE
  "+\n-\n*\n/\n=\n"     // operators
  "<\n>\n<=\n>=\n"
  "!=\n&\n|\n!\n"
  "?\n:",
  "//",                 // single line comment
  "/*",                 // multiline comment 1
  "*/",                 // multiline comment 2
  "{",                  // scope keywords 1
  "}",                  // scope keywords 2
  "\"",                 // string delimiters
  '\\',                 // escape character
  ';',                  // statement terminator
  NULL,
  NULL,
  NULL
} ;

CodemaxLanguage LangINI =
{
  0,      // not really appropriate but works better than CMLS_SGML
  true,                 // case-sensitive flag
  "All_Console\n"       // TODO: get INI keywords from core
  "All_File\n"
  "Antialias_Depth\n"
  "Antialias\n"
  "Antialias_Threshold\n"
  "Bits_Per_Color\n"
  "Bits_Per_Colour\n"
  "Bounding\n"
  "Bounding_Method\n"
  "Bounding_Threshold\n"
  "BSP_BaseAccessCost\n"
  "BSP_ChildAccessCost\n"
  "BSP_ISectCost\n"
  "BSP_MaxDepth\n"
  "BSP_MissChance\n"
  "Buffer_Output\n"
  "Buffer_Size\n"
  "Clock\n"
  "Clockless_Animation\n"
  "Compression\n"
  "Continue_Trace\n"
  "Create_Histogram\n"
  "Create_Ini\n"
  "Cyclic_Animation\n"
  "Debug_Console\n"
  "Debug_File\n"
  "Declare\n"
  "Display\n"
  "Display_Gamma\n"
  "Draw_Vistas\n"
  "End_Column\n"
  "End_Row\n"
  "Fatal_Console\n"
  "Fatal_Error_Command\n"
  "Fatal_Error_Return\n"
  "Fatal_File\n"
  "Field_Render\n"
  "File_Gamma\n"
  "Final_Clock\n"
  "Final_Frame\n"
  "Height\n"
  "Histogram_Name\n"
  "Histogram_Grid_Size\n"
  "Histogram_Type\n"
  "Initial_Clock\n"
  "Initial_Frame\n"
  "Input_File_Name\n"
  "Include_Header\n"
  "Include_Ini\n"
  "Jitter_Amount\n"
  "Jitter\n"
  "Library_Path\n"
  "Light_Buffer\n"
  "Odd_Field\n"
  "Output_Alpha\n"
  "Output_File_Name\n"
  "Output_File_Type\n"
  "Output_To_File\n"
  "Palette\n"
  "Pause_When_Done\n"
  "Post_Frame_Command\n"
  "Post_Frame_Return\n"
  "Post_Scene_Command\n"
  "Post_Scene_Return\n"
  "Preview_End_Size\n"
  "Preview_Start_Size\n"
  "Pre_Frame_Command\n"
  "Pre_Frame_Return\n"
  "Pre_Scene_Command\n"
  "Pre_Scene_Return\n"
  "Quality\n"
  "Radiosity_File_Name\n"
  "Radiosity_From_File\n"
  "Radiosity_To_File\n"
  "Real_Time_Raytracing\n"
  "Remove_Bounds\n"
  "Render_Block_Size\n"
  "Render_Console\n"
  "Render_File\n"
  "Sampling_Method\n"
  "Split_Unions\n"
  "Start_Column\n"
  "Start_Row\n"
  "Statistic_Console\n"
  "Statistic_File\n"
  "Subset_End_Frame\n"
  "Subset_Start_Frame\n"
  "Test_Abort_Count\n"
  "Test_Abort\n"
  "User_Abort_Command\n"
  "User_Abort_Return\n"
  "Verbose\n"
  "Version\n"
  "Video_Mode\n"
  "Vista_Buffer\n"
  "Warning_Console\n"
  "Warning_File\n"
  "Warning_Level\n"
  "Width\n"
  "Work_Threads\n",
  "=\n",                // operators
  ";",                  // single line comment
  NULL,                 // multiline comment 1
  NULL,                 // multiline comment 2
  NULL,                 // scope keywords 1
  NULL,                 // scope keywords 2
  "\"",                 // string delimiters
  '\\',                 // escape character
  NULL,                 // statement terminator
  NULL,
  NULL,
  NULL
} ;

