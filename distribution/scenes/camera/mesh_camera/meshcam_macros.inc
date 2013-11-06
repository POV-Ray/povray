// This work is licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/ or send a
// letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.

/* Persistence Of Vision Raytracer sample file.

   Mesh Camera macros for use with the new mesh_camera.
  
   These macros help to create and place the grid-like meshes needed to simulate
   a camera with the new mesh_camera feature:
  
   + meshcam_pinhole() creates a convex mesh simulating a perspective camera, for use with distrib.#0,1,2
   + meshcam_lens() creates a convex mesh like above, but adding simulated lens distortion, for use with distrib.#0,1,2
   + meshcam_pinhole_uv() is a uv mapped version of meshcam_pinhole(), for use with distrib.#3
   + meshcam_placement() helps to place the mesh cameras created by the other macros.
  
   --
   Jaime Vives Piqueres, Jan. 2011  <jaime@ignorancia.org> */

/***************************************************************************************
 * $File: //depot/povray/smp/distribution/scenes/camera/mesh_camera/meshcam_macros.inc $
 * $Revision: #2 $
 * $Change: 5377 $
 * $DateTime: 2011/01/09 19:56:00 $
 * $Author: jholsenback $
 **************************************************************************************/
#include "math.inc"
#include "transforms.inc"

/*
  Convex grid-like mesh for pinhole perspective projection
*/  
#macro meshcam_pinhole(res_x, res_y, c_angle, c_file)

#local c_up=1*y;
#local c_right=(res_x/res_y)*x;
#local c_direction=0.5*vlength(c_right)/tan(radians(c_angle)/2);

#local sph_lens=sphere{0,c_direction}

#local px_inc=(res_x/res_y)/res_x;
#local centering=<-res_x*.5*px_inc+px_inc*.5,res_y*.5*px_inc-px_inc-px_inc*2/3,0>;

#local tr_base_1=<        0,2*px_inc/3,0>;
#local tr_base_2=< px_inc/2, -px_inc/3,0>;
#local tr_base_3=<-px_inc/2, -px_inc/3,0>;

#if (c_file!="")
 #fopen fmesh c_file write
 #write (fmesh, "mesh{", "\n")
#end
mesh{
#local row_count=0;
#while (row_count<res_y)
  #local col_count=0;
  #local d_y=row_count*px_inc;
  #while (col_count<res_x)
     #local d_x=col_count*px_inc;
     #local Norm=<0,0,0>;
     #local tr_vertex_1=trace(sph_lens,0,tr_base_1+<d_x,-d_y,c_direction>+centering, Norm);
     #local tr_vertex_2=trace(sph_lens,0,tr_base_2+<d_x,-d_y,c_direction>+centering, Norm);
     #local tr_vertex_3=trace(sph_lens,0,tr_base_3+<d_x,-d_y,c_direction>+centering, Norm);
     triangle{tr_vertex_1,tr_vertex_2,tr_vertex_3}
     #if (c_file!="")
      #write (fmesh, "triangle{", 
              "<",str(tr_vertex_1.x,0,16),",",str(tr_vertex_1.y,0,16),",",str(tr_vertex_1.z,0,16),">", ",", 
              "<",str(tr_vertex_2.x,0,16),",",str(tr_vertex_2.y,0,16),",",str(tr_vertex_2.z,0,16),">", ",", 
              "<",str(tr_vertex_3.x,0,16),",",str(tr_vertex_3.y,0,16),",",str(tr_vertex_3.z,0,16),">", "}", "\n")
     #end
     #local col_count=col_count+1;
  #end
  #local row_count=row_count+1;
#end
}
#if (c_file!="")
 #write (fmesh, "}", "\n")
 #fclose fmesh
#end

#end


/*
  Convex grid-like mesh for pinhole perspective projection
  --
  Version simulating lens distortion: use positive values for barrel, negative for pincushion
*/  
#macro meshcam_lens(res_x, res_y, c_angle, c_distort, c_file)

#local c_up=1*y;
#local c_right=(res_x/res_y)*x;
#local c_direction=0.5*vlength(c_right)/tan(radians(c_angle)/2);

#local sph_lens=sphere{0,c_direction}

#local px_inc=(res_x/res_y)/res_x;
#local centering=<-res_x*.5*px_inc+px_inc*.5,res_y*.5*px_inc-px_inc-px_inc*2/3,0>;

#local tr_base_1=<        0,2*px_inc/3,0>;
#local tr_base_2=< px_inc/2, -px_inc/3,0>;
#local tr_base_3=<-px_inc/2, -px_inc/3,0>;

#if (c_file!="")
 #fopen fmesh c_file write
 #write (fmesh, "mesh{", "\n")
#end
mesh{
#local row_count=0;
#while (row_count<res_y)
  #local col_count=0;
  #local d_y=row_count*px_inc;
  #while (col_count<res_x)
     #local d_x=col_count*px_inc;
     #local Norm=<0,0,0>;
     #local tr_vertex_1=trace(sph_lens,0,tr_base_1+<d_x,-d_y,c_direction>+centering, Norm);
     #local tr_vertex_2=trace(sph_lens,0,tr_base_2+<d_x,-d_y,c_direction>+centering, Norm);
     #local tr_vertex_3=trace(sph_lens,0,tr_base_3+<d_x,-d_y,c_direction>+centering, Norm);
     #if (c_distort!=0)
      #local tr_vertex_1=tr_vertex_1-c_distort*pow(abs(tr_vertex_1.x)*abs(tr_vertex_1.y),2)*z;
      #local tr_vertex_2=tr_vertex_2-c_distort*pow(abs(tr_vertex_2.x)*abs(tr_vertex_2.y),2)*z;
      #local tr_vertex_3=tr_vertex_3-c_distort*pow(abs(tr_vertex_3.x)*abs(tr_vertex_3.y),2)*z;
     #end
     triangle{tr_vertex_1,tr_vertex_2,tr_vertex_3}
     #if (c_file!="")
      #write (fmesh, "triangle{", 
              "<",str(tr_vertex_1.x,0,16),",",str(tr_vertex_1.y,0,16),",",str(tr_vertex_1.z,0,16),">", ",", 
              "<",str(tr_vertex_2.x,0,16),",",str(tr_vertex_2.y,0,16),",",str(tr_vertex_2.z,0,16),">", ",", 
              "<",str(tr_vertex_3.x,0,16),",",str(tr_vertex_3.y,0,16),",",str(tr_vertex_3.z,0,16),">", "}", "\n")
     #end
     #local col_count=col_count+1;
  #end
  #local row_count=row_count+1;
#end
}
#if (c_file!="")
 #write (fmesh, "}", "\n")
 #fclose fmesh
#end

#end


/*
  Flat grid-like mesh for pure orthographic projection
*/  
#macro meshcam_orthographic(res_x, res_y, c_up_length, c_file)

#local c_direction=1;
#local px_inc=c_up_length*(res_x/res_y)/res_x;
#local centering=<-res_x*.5*px_inc+px_inc*.5,res_y*.5*px_inc-px_inc-px_inc*2/3,0>;

#local tr_base_1=<        0,px_inc*2/3,0>;
#local tr_base_2=< px_inc/2, -px_inc/3,0>;
#local tr_base_3=<-px_inc/2, -px_inc/3,0>;

#if (c_file!="")
 #fopen fmesh c_file write
 #write (fmesh, "mesh{", "\n")
#end
mesh{
#local row_count=0;
#while (row_count<res_y)
  #local col_count=0;
  #local d_y=row_count*px_inc;
  #while (col_count<res_x)
     #local d_x=col_count*px_inc;
     #local tr_vertex_1=tr_base_1+<d_x,-d_y,c_direction>+centering;
     #local tr_vertex_2=tr_base_2+<d_x,-d_y,c_direction>+centering;
     #local tr_vertex_3=tr_base_3+<d_x,-d_y,c_direction>+centering;
     triangle{tr_vertex_1,tr_vertex_2,tr_vertex_3}
     #if (c_file!="")
      #write (fmesh, "triangle{", 
              "<",str(tr_vertex_1.x,0,16),",",str(tr_vertex_1.y,0,16),",",str(tr_vertex_1.z,0,16),">", ",", 
              "<",str(tr_vertex_2.x,0,16),",",str(tr_vertex_2.y,0,16),",",str(tr_vertex_2.z,0,16),">", ",", 
              "<",str(tr_vertex_3.x,0,16),",",str(tr_vertex_3.y,0,16),",",str(tr_vertex_3.z,0,16),">", "}", "\n")
     #end
     #local col_count=col_count+1;
  #end
  #local row_count=row_count+1;
#end
}
#if (c_file!="")
 #write (fmesh, "}", "\n")
 #fclose fmesh
#end

#end


/*
  Convex grid-like mesh for pinhole perspective projection
  --
  Version with UV mapping for use with distrib.#3 and the "smooth" modifier.  
*/  
#macro meshcam_pinhole_uv(res_x, res_y, c_angle, c_file)

#local c_up=1*y;
#local c_right=(res_x/res_y)*x;
#local c_direction=0.5*vlength(c_right)/tan(radians(c_angle)/2);

#local sph_lens=sphere{0,c_direction}

#local px_inc=(res_x/res_y)/res_x;
#local centering=<-res_x*.5*px_inc,res_y*.5*px_inc,0>;

#local tr_base_1=<px_inc/2,-px_inc/2,0>;
#local tr_base_2=<0,0,0>;
#local tr_base_3=<px_inc,0,0>;
#local tr_base_4=<px_inc,-px_inc,0>;
#local tr_base_5=<0,-px_inc,0>;

#if (c_file!="")
 #fopen fmesh c_file write
 #write (fmesh, "mesh{", "\n")
#end
mesh{
#local row_count=0;
#while (row_count<res_y)
  #local col_count=0;
  #local d_y=row_count*px_inc;
  #while (col_count<res_x)
     #local d_x=col_count*px_inc;
     #local Norm=<0,0,0>;
     #local tr_vertex_1=trace(sph_lens,0,tr_base_1+<d_x,-d_y,c_direction>+centering, Norm);
     #local tr_normal_1=Norm;
     #local tr_vertex_2=trace(sph_lens,0,tr_base_2+<d_x,-d_y,c_direction>+centering, Norm);
     #local tr_normal_2=Norm;
     #local tr_vertex_3=trace(sph_lens,0,tr_base_3+<d_x,-d_y,c_direction>+centering, Norm);
     #local tr_normal_3=Norm;
     #local tr_vertex_4=trace(sph_lens,0,tr_base_4+<d_x,-d_y,c_direction>+centering, Norm);
     #local tr_normal_4=Norm;
     #local tr_vertex_5=trace(sph_lens,0,tr_base_5+<d_x,-d_y,c_direction>+centering, Norm);
     #local tr_normal_5=Norm;
     smooth_triangle{tr_vertex_1, tr_normal_1, tr_vertex_2, tr_normal_2, tr_vertex_3, tr_normal_3
      uv_vectors 
        <(tr_base_1.x+d_x)/(res_x*px_inc),1-abs(tr_base_1.y-d_y)/(res_y*px_inc)>,
        <(tr_base_2.x+d_x)/(res_x*px_inc),1-abs(tr_base_2.y-d_y)/(res_y*px_inc)>,
        <(tr_base_3.x+d_x)/(res_x*px_inc),1-abs(tr_base_3.y-d_y)/(res_y*px_inc)>
     }
     smooth_triangle{tr_vertex_1, tr_normal_1, tr_vertex_3, tr_normal_3, tr_vertex_4, tr_normal_4
      uv_vectors 
        <(tr_base_1.x+d_x)/(res_x*px_inc),1-abs(tr_base_1.y-d_y)/(res_y*px_inc)>,
        <(tr_base_3.x+d_x)/(res_x*px_inc),1-abs(tr_base_3.y-d_y)/(res_y*px_inc)>,
        <(tr_base_4.x+d_x)/(res_x*px_inc),1-abs(tr_base_4.y-d_y)/(res_y*px_inc)>
     }
     smooth_triangle{tr_vertex_1, tr_normal_1, tr_vertex_4, tr_normal_4, tr_vertex_5, tr_normal_5
      uv_vectors 
        <(tr_base_1.x+d_x)/(res_x*px_inc),1-abs(tr_base_1.y-d_y)/(res_y*px_inc)>,
        <(tr_base_4.x+d_x)/(res_x*px_inc),1-abs(tr_base_4.y-d_y)/(res_y*px_inc)>,
        <(tr_base_5.x+d_x)/(res_x*px_inc),1-abs(tr_base_5.y-d_y)/(res_y*px_inc)>
     }
     smooth_triangle{tr_vertex_1, tr_normal_1, tr_vertex_5, tr_normal_5, tr_vertex_2, tr_normal_2
      uv_vectors 
        <(tr_base_1.x+d_x)/(res_x*px_inc),1-abs(tr_base_1.y-d_y)/(res_y*px_inc)>,
        <(tr_base_5.x+d_x)/(res_x*px_inc),1-abs(tr_base_5.y-d_y)/(res_y*px_inc)>,
        <(tr_base_2.x+d_x)/(res_x*px_inc),1-abs(tr_base_2.y-d_y)/(res_y*px_inc)>
     }
     #if (c_file!="")
      #write (fmesh, "smooth_triangle{", 
              "<",str(tr_vertex_1.x,0,16),",",str(tr_vertex_1.y,0,16),",",str(tr_vertex_1.z,0,16),">", ",", 
              "<",str(tr_normal_1.x,0,16),",",str(tr_normal_1.y,0,16),",",str(tr_normal_1.z,0,16),">", ",", 
              "<",str(tr_vertex_2.x,0,16),",",str(tr_vertex_2.y,0,16),",",str(tr_vertex_2.z,0,16),">", ",", 
              "<",str(tr_normal_2.x,0,16),",",str(tr_normal_2.y,0,16),",",str(tr_normal_2.z,0,16),">", ",", 
              "<",str(tr_vertex_3.x,0,16),",",str(tr_vertex_3.y,0,16),",",str(tr_vertex_3.z,0,16),">", ",",
              "<",str(tr_normal_3.x,0,16),",",str(tr_normal_3.y,0,16),",",str(tr_normal_3.z,0,16),">", 
              "uv_vectors ",
              "<", str((tr_base_1.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_1.y-d_y)/(res_y*px_inc),0,16), ">,",
              "<", str((tr_base_2.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_2.y-d_y)/(res_y*px_inc),0,16), ">,",
              "<", str((tr_base_3.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_3.y-d_y)/(res_y*px_inc),0,16), ">}",
              "\n")
      #write (fmesh, "smooth_triangle{", 
              "<",str(tr_vertex_1.x,0,16),",",str(tr_vertex_1.y,0,16),",",str(tr_vertex_1.z,0,16),">", ",", 
              "<",str(tr_normal_1.x,0,16),",",str(tr_normal_1.y,0,16),",",str(tr_normal_1.z,0,16),">", ",", 
              "<",str(tr_vertex_3.x,0,16),",",str(tr_vertex_3.y,0,16),",",str(tr_vertex_3.z,0,16),">", ",", 
              "<",str(tr_normal_3.x,0,16),",",str(tr_normal_3.y,0,16),",",str(tr_normal_3.z,0,16),">",
              "<",str(tr_vertex_4.x,0,16),",",str(tr_vertex_4.y,0,16),",",str(tr_vertex_4.z,0,16),">", ",",  
              "<",str(tr_normal_4.x,0,16),",",str(tr_normal_4.y,0,16),",",str(tr_normal_4.z,0,16),">",
              "uv_vectors ",
              "<", str((tr_base_1.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_1.y-d_y)/(res_y*px_inc),0,16), ">,",
              "<", str((tr_base_3.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_3.y-d_y)/(res_y*px_inc),0,16), ">,",
              "<", str((tr_base_4.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_4.y-d_y)/(res_y*px_inc),0,16), ">}",
              "\n")
      #write (fmesh, "smooth_triangle{", 
              "<",str(tr_vertex_1.x,0,16),",",str(tr_vertex_1.y,0,16),",",str(tr_vertex_1.z,0,16),">", ",", 
              "<",str(tr_normal_1.x,0,16),",",str(tr_normal_1.y,0,16),",",str(tr_normal_1.z,0,16),">", ",", 
              "<",str(tr_vertex_4.x,0,16),",",str(tr_vertex_4.y,0,16),",",str(tr_vertex_4.z,0,16),">", ",", 
              "<",str(tr_normal_4.x,0,16),",",str(tr_normal_4.y,0,16),",",str(tr_normal_4.z,0,16),">", ",", 
              "<",str(tr_vertex_5.x,0,16),",",str(tr_vertex_5.y,0,16),",",str(tr_vertex_5.z,0,16),">", ",",  
              "<",str(tr_normal_5.x,0,16),",",str(tr_normal_5.y,0,16),",",str(tr_normal_5.z,0,16),">", 
              "uv_vectors ",
              "<", str((tr_base_1.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_1.y-d_y)/(res_y*px_inc),0,16), ">,",
              "<", str((tr_base_4.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_4.y-d_y)/(res_y*px_inc),0,16), ">,",
              "<", str((tr_base_5.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_5.y-d_y)/(res_y*px_inc),0,16), ">}",
              "\n")
      #write (fmesh, "smooth_triangle{", 
              "<",str(tr_vertex_1.x,0,16),",",str(tr_vertex_1.y,0,16),",",str(tr_vertex_1.z,0,16),">", ",", 
              "<",str(tr_normal_1.x,0,16),",",str(tr_normal_1.y,0,16),",",str(tr_normal_1.z,0,16),">", ",", 
              "<",str(tr_vertex_5.x,0,16),",",str(tr_vertex_5.y,0,16),",",str(tr_vertex_5.z,0,16),">", ",", 
              "<",str(tr_normal_5.x,0,16),",",str(tr_normal_5.y,0,16),",",str(tr_normal_5.z,0,16),">", ",", 
              "<",str(tr_vertex_2.x,0,16),",",str(tr_vertex_2.y,0,16),",",str(tr_vertex_2.z,0,16),">", ",",  
              "<",str(tr_normal_2.x,0,16),",",str(tr_normal_2.y,0,16),",",str(tr_normal_2.z,0,16),">",
              "uv_vectors ",
              "<", str((tr_base_1.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_1.y-d_y)/(res_y*px_inc),0,16), ">,",
              "<", str((tr_base_5.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_5.y-d_y)/(res_y*px_inc),0,16), ">,",
              "<", str((tr_base_2.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_2.y-d_y)/(res_y*px_inc),0,16), ">}",
              "\n")
     #end
     #local col_count=col_count+1;
  #end
  #local row_count=row_count+1;
#end
}
#if (c_file!="")
 #write (fmesh, "}", "\n")
 #fclose fmesh
#end

#end


/*
  Convex grid-like mesh for pinhole perspective projection
  --
  Version simulating lens distortion: use positive values for barrel, negative for pincushion
  Version with UV mapping for use with distrib.#3 and the "smooth" modifier.  
*/  
#macro meshcam_lens_uv(res_x, res_y, c_angle, c_distort, c_file)

#local c_up=1*y;
#local c_right=(res_x/res_y)*x;
#local c_direction=0.5*vlength(c_right)/tan(radians(c_angle)/2);

#local sph_lens=sphere{0,c_direction}

#local px_inc=(res_x/res_y)/res_x;
#local centering=<-res_x*.5*px_inc,res_y*.5*px_inc,0>;

#local tr_base_1=<px_inc/2,-px_inc/2,0>;
#local tr_base_2=<0,0,0>;
#local tr_base_3=<px_inc,0,0>;
#local tr_base_4=<px_inc,-px_inc,0>;
#local tr_base_5=<0,-px_inc,0>;

#if (c_file!="")
 #fopen fmesh c_file write
 #write (fmesh, "mesh{", "\n")
#end
mesh{
#local row_count=0;
#while (row_count<res_y)
  #local col_count=0;
  #local d_y=row_count*px_inc;
  #while (col_count<res_x)
     #local d_x=col_count*px_inc;
     #local Norm=<0,0,0>;
     #local tr_tmp_1=tr_base_1+<d_x,-d_y,c_direction>+centering;
     #local tr_tmp_2=tr_base_2+<d_x,-d_y,c_direction>+centering;
     #local tr_tmp_3=tr_base_3+<d_x,-d_y,c_direction>+centering;
     #local tr_tmp_4=tr_base_4+<d_x,-d_y,c_direction>+centering;
     #local tr_tmp_5=tr_base_5+<d_x,-d_y,c_direction>+centering;
     #if (c_distort!=0) // had to multiply it by 10 to obtain the same result as the non-uvmapped version?!?!
      #local tr_tmp_1=tr_tmp_1-c_distort*10*pow(abs(tr_tmp_1.x)*abs(tr_tmp_1.y),2)*z;
      #local tr_tmp_2=tr_tmp_2-c_distort*10*pow(abs(tr_tmp_2.x)*abs(tr_tmp_2.y),2)*z;
      #local tr_tmp_3=tr_tmp_3-c_distort*10*pow(abs(tr_tmp_3.x)*abs(tr_tmp_3.y),2)*z;
      #local tr_tmp_4=tr_tmp_4-c_distort*10*pow(abs(tr_tmp_4.x)*abs(tr_tmp_4.y),2)*z;
      #local tr_tmp_5=tr_tmp_5-c_distort*10*pow(abs(tr_tmp_5.x)*abs(tr_tmp_5.y),2)*z;
     #end    
     #local tr_vertex_1=trace(sph_lens,0,tr_tmp_1, Norm);
     #local tr_normal_1=Norm;
     #local tr_vertex_2=trace(sph_lens,0,tr_tmp_2, Norm);
     #local tr_normal_2=Norm;
     #local tr_vertex_3=trace(sph_lens,0,tr_tmp_3, Norm);
     #local tr_normal_3=Norm;
     #local tr_vertex_4=trace(sph_lens,0,tr_tmp_4, Norm);
     #local tr_normal_4=Norm;
     #local tr_vertex_5=trace(sph_lens,0,tr_tmp_5, Norm);
     #local tr_normal_5=Norm;
     smooth_triangle{tr_vertex_1, tr_normal_1, tr_vertex_2, tr_normal_2, tr_vertex_3, tr_normal_3
      uv_vectors 
        <(tr_base_1.x+d_x)/(res_x*px_inc),1-abs(tr_base_1.y-d_y)/(res_y*px_inc)>,
        <(tr_base_2.x+d_x)/(res_x*px_inc),1-abs(tr_base_2.y-d_y)/(res_y*px_inc)>,
        <(tr_base_3.x+d_x)/(res_x*px_inc),1-abs(tr_base_3.y-d_y)/(res_y*px_inc)>
     }
     smooth_triangle{tr_vertex_1, tr_normal_1, tr_vertex_3, tr_normal_3, tr_vertex_4, tr_normal_4
      uv_vectors 
        <(tr_base_1.x+d_x)/(res_x*px_inc),1-abs(tr_base_1.y-d_y)/(res_y*px_inc)>,
        <(tr_base_3.x+d_x)/(res_x*px_inc),1-abs(tr_base_3.y-d_y)/(res_y*px_inc)>,
        <(tr_base_4.x+d_x)/(res_x*px_inc),1-abs(tr_base_4.y-d_y)/(res_y*px_inc)>
     }
     smooth_triangle{tr_vertex_1, tr_normal_1, tr_vertex_4, tr_normal_4, tr_vertex_5, tr_normal_5
      uv_vectors 
        <(tr_base_1.x+d_x)/(res_x*px_inc),1-abs(tr_base_1.y-d_y)/(res_y*px_inc)>,
        <(tr_base_4.x+d_x)/(res_x*px_inc),1-abs(tr_base_4.y-d_y)/(res_y*px_inc)>,
        <(tr_base_5.x+d_x)/(res_x*px_inc),1-abs(tr_base_5.y-d_y)/(res_y*px_inc)>
     }
     smooth_triangle{tr_vertex_1, tr_normal_1, tr_vertex_5, tr_normal_5, tr_vertex_2, tr_normal_2
      uv_vectors 
        <(tr_base_1.x+d_x)/(res_x*px_inc),1-abs(tr_base_1.y-d_y)/(res_y*px_inc)>,
        <(tr_base_5.x+d_x)/(res_x*px_inc),1-abs(tr_base_5.y-d_y)/(res_y*px_inc)>,
        <(tr_base_2.x+d_x)/(res_x*px_inc),1-abs(tr_base_2.y-d_y)/(res_y*px_inc)>
     }
     #if (c_file!="")
      #write (fmesh, "smooth_triangle{", 
              "<",str(tr_vertex_1.x,0,16),",",str(tr_vertex_1.y,0,16),",",str(tr_vertex_1.z,0,16),">", ",", 
              "<",str(tr_normal_1.x,0,16),",",str(tr_normal_1.y,0,16),",",str(tr_normal_1.z,0,16),">", ",", 
              "<",str(tr_vertex_2.x,0,16),",",str(tr_vertex_2.y,0,16),",",str(tr_vertex_2.z,0,16),">", ",", 
              "<",str(tr_normal_2.x,0,16),",",str(tr_normal_2.y,0,16),",",str(tr_normal_2.z,0,16),">", ",", 
              "<",str(tr_vertex_3.x,0,16),",",str(tr_vertex_3.y,0,16),",",str(tr_vertex_3.z,0,16),">", ",",
              "<",str(tr_normal_3.x,0,16),",",str(tr_normal_3.y,0,16),",",str(tr_normal_3.z,0,16),">", 
              "uv_vectors ",
              "<", str((tr_base_1.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_1.y-d_y)/(res_y*px_inc),0,16), ">,",
              "<", str((tr_base_2.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_2.y-d_y)/(res_y*px_inc),0,16), ">,",
              "<", str((tr_base_3.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_3.y-d_y)/(res_y*px_inc),0,16), ">}",
              "\n")
      #write (fmesh, "smooth_triangle{", 
              "<",str(tr_vertex_1.x,0,16),",",str(tr_vertex_1.y,0,16),",",str(tr_vertex_1.z,0,16),">", ",", 
              "<",str(tr_normal_1.x,0,16),",",str(tr_normal_1.y,0,16),",",str(tr_normal_1.z,0,16),">", ",", 
              "<",str(tr_vertex_3.x,0,16),",",str(tr_vertex_3.y,0,16),",",str(tr_vertex_3.z,0,16),">", ",", 
              "<",str(tr_normal_3.x,0,16),",",str(tr_normal_3.y,0,16),",",str(tr_normal_3.z,0,16),">",
              "<",str(tr_vertex_4.x,0,16),",",str(tr_vertex_4.y,0,16),",",str(tr_vertex_4.z,0,16),">", ",",  
              "<",str(tr_normal_4.x,0,16),",",str(tr_normal_4.y,0,16),",",str(tr_normal_4.z,0,16),">",
              "uv_vectors ",
              "<", str((tr_base_1.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_1.y-d_y)/(res_y*px_inc),0,16), ">,",
              "<", str((tr_base_3.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_3.y-d_y)/(res_y*px_inc),0,16), ">,",
              "<", str((tr_base_4.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_4.y-d_y)/(res_y*px_inc),0,16), ">}",
              "\n")
      #write (fmesh, "smooth_triangle{", 
              "<",str(tr_vertex_1.x,0,16),",",str(tr_vertex_1.y,0,16),",",str(tr_vertex_1.z,0,16),">", ",", 
              "<",str(tr_normal_1.x,0,16),",",str(tr_normal_1.y,0,16),",",str(tr_normal_1.z,0,16),">", ",", 
              "<",str(tr_vertex_4.x,0,16),",",str(tr_vertex_4.y,0,16),",",str(tr_vertex_4.z,0,16),">", ",", 
              "<",str(tr_normal_4.x,0,16),",",str(tr_normal_4.y,0,16),",",str(tr_normal_4.z,0,16),">", ",", 
              "<",str(tr_vertex_5.x,0,16),",",str(tr_vertex_5.y,0,16),",",str(tr_vertex_5.z,0,16),">", ",",  
              "<",str(tr_normal_5.x,0,16),",",str(tr_normal_5.y,0,16),",",str(tr_normal_5.z,0,16),">", 
              "uv_vectors ",
              "<", str((tr_base_1.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_1.y-d_y)/(res_y*px_inc),0,16), ">,",
              "<", str((tr_base_4.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_4.y-d_y)/(res_y*px_inc),0,16), ">,",
              "<", str((tr_base_5.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_5.y-d_y)/(res_y*px_inc),0,16), ">}",
              "\n")
      #write (fmesh, "smooth_triangle{", 
              "<",str(tr_vertex_1.x,0,16),",",str(tr_vertex_1.y,0,16),",",str(tr_vertex_1.z,0,16),">", ",", 
              "<",str(tr_normal_1.x,0,16),",",str(tr_normal_1.y,0,16),",",str(tr_normal_1.z,0,16),">", ",", 
              "<",str(tr_vertex_5.x,0,16),",",str(tr_vertex_5.y,0,16),",",str(tr_vertex_5.z,0,16),">", ",", 
              "<",str(tr_normal_5.x,0,16),",",str(tr_normal_5.y,0,16),",",str(tr_normal_5.z,0,16),">", ",", 
              "<",str(tr_vertex_2.x,0,16),",",str(tr_vertex_2.y,0,16),",",str(tr_vertex_2.z,0,16),">", ",",  
              "<",str(tr_normal_2.x,0,16),",",str(tr_normal_2.y,0,16),",",str(tr_normal_2.z,0,16),">",
              "uv_vectors ",
              "<", str((tr_base_1.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_1.y-d_y)/(res_y*px_inc),0,16), ">,",
              "<", str((tr_base_5.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_5.y-d_y)/(res_y*px_inc),0,16), ">,",
              "<", str((tr_base_2.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_2.y-d_y)/(res_y*px_inc),0,16), ">}",
              "\n")
     #end
     #local col_count=col_count+1;
  #end
  #local row_count=row_count+1;
#end
}
#if (c_file!="")
 #write (fmesh, "}", "\n")
 #fclose fmesh
#end

#end


/*
  Flat grid-like mesh for pure orthographic projection
  --
  Version with UV mapping for use with distrib.#3 
*/  
#macro meshcam_orthographic_uv(res_x, res_y, c_up_length, c_file)

#local c_direction=1;
#local px_inc=c_up_length*(res_x/res_y)/res_x;
#local centering=<-res_x*.5*px_inc,res_y*.5*px_inc,0>;

#local tr_base_1=<px_inc/2,-px_inc/2,0>;
#local tr_base_2=<0,0,0>;
#local tr_base_3=<px_inc,0,0>;
#local tr_base_4=<px_inc,-px_inc,0>;
#local tr_base_5=<0,-px_inc,0>;

#if (c_file!="")
 #fopen fmesh c_file write
 #write (fmesh, "mesh{", "\n")
#end
mesh{
#local row_count=0;
#while (row_count<res_y)
  #local col_count=0;
  #local d_y=row_count*px_inc;
  #while (col_count<res_x)
     #local d_x=col_count*px_inc;
     #local tr_vertex_1=tr_base_1+<d_x,-d_y,c_direction>+centering;
     #local tr_vertex_2=tr_base_2+<d_x,-d_y,c_direction>+centering;
     #local tr_vertex_3=tr_base_3+<d_x,-d_y,c_direction>+centering;
     #local tr_vertex_4=tr_base_4+<d_x,-d_y,c_direction>+centering;
     #local tr_vertex_5=tr_base_5+<d_x,-d_y,c_direction>+centering;
     #local Norm=z;
     smooth_triangle{tr_vertex_1, Norm, tr_vertex_2, Norm, tr_vertex_3, Norm
      uv_vectors 
        <(tr_base_1.x+d_x)/(res_x*px_inc),1-abs(tr_base_1.y-d_y)/(res_y*px_inc)>,
        <(tr_base_2.x+d_x)/(res_x*px_inc),1-abs(tr_base_2.y-d_y)/(res_y*px_inc)>,
        <(tr_base_3.x+d_x)/(res_x*px_inc),1-abs(tr_base_3.y-d_y)/(res_y*px_inc)>
     }
     smooth_triangle{tr_vertex_1, Norm, tr_vertex_3, Norm, tr_vertex_4, Norm
      uv_vectors 
        <(tr_base_1.x+d_x)/(res_x*px_inc),1-abs(tr_base_1.y-d_y)/(res_y*px_inc)>,
        <(tr_base_3.x+d_x)/(res_x*px_inc),1-abs(tr_base_3.y-d_y)/(res_y*px_inc)>,
        <(tr_base_4.x+d_x)/(res_x*px_inc),1-abs(tr_base_4.y-d_y)/(res_y*px_inc)>
     }
     smooth_triangle{tr_vertex_1, Norm, tr_vertex_4, Norm, tr_vertex_5, Norm
      uv_vectors 
        <(tr_base_1.x+d_x)/(res_x*px_inc),1-abs(tr_base_1.y-d_y)/(res_y*px_inc)>,
        <(tr_base_4.x+d_x)/(res_x*px_inc),1-abs(tr_base_4.y-d_y)/(res_y*px_inc)>,
        <(tr_base_5.x+d_x)/(res_x*px_inc),1-abs(tr_base_5.y-d_y)/(res_y*px_inc)>
     }
     smooth_triangle{tr_vertex_1, Norm, tr_vertex_5, Norm, tr_vertex_2, Norm
      uv_vectors 
        <(tr_base_1.x+d_x)/(res_x*px_inc),1-abs(tr_base_1.y-d_y)/(res_y*px_inc)>,
        <(tr_base_5.x+d_x)/(res_x*px_inc),1-abs(tr_base_5.y-d_y)/(res_y*px_inc)>,
        <(tr_base_2.x+d_x)/(res_x*px_inc),1-abs(tr_base_2.y-d_y)/(res_y*px_inc)>
     }
     #if (c_file!="")
      #write (fmesh, "smooth_triangle{", 
              "<",str(tr_vertex_1.x,0,16),",",str(tr_vertex_1.y,0,16),",",str(tr_vertex_1.z,0,16),">", ",", 
              "<",str(Norm.x,0,16),",",str(Norm.y,0,16),",",str(Norm.z,0,16),">", ",", 
              "<",str(tr_vertex_2.x,0,16),",",str(tr_vertex_2.y,0,16),",",str(tr_vertex_2.z,0,16),">", ",", 
              "<",str(Norm.x,0,16),",",str(Norm.y,0,16),",",str(Norm.z,0,16),">", ",", 
              "<",str(tr_vertex_3.x,0,16),",",str(tr_vertex_3.y,0,16),",",str(tr_vertex_3.z,0,16),">", ",",
              "<",str(Norm.x,0,16),",",str(Norm.y,0,16),",",str(Norm.z,0,16),">", 
              "uv_vectors ",
              "<", str((tr_base_1.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_1.y-d_y)/(res_y*px_inc),0,16), ">,",
              "<", str((tr_base_2.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_2.y-d_y)/(res_y*px_inc),0,16), ">,",
              "<", str((tr_base_3.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_3.y-d_y)/(res_y*px_inc),0,16), ">}",
              "\n")
      #write (fmesh, "smooth_triangle{", 
              "<",str(tr_vertex_1.x,0,16),",",str(tr_vertex_1.y,0,16),",",str(tr_vertex_1.z,0,16),">", ",", 
              "<",str(Norm.x,0,16),",",str(Norm.y,0,16),",",str(Norm.z,0,16),">", ",", 
              "<",str(tr_vertex_3.x,0,16),",",str(tr_vertex_3.y,0,16),",",str(tr_vertex_3.z,0,16),">", ",", 
              "<",str(Norm.x,0,16),",",str(Norm.y,0,16),",",str(Norm.z,0,16),">",
              "<",str(tr_vertex_4.x,0,16),",",str(tr_vertex_4.y,0,16),",",str(tr_vertex_4.z,0,16),">", ",",  
              "<",str(Norm.x,0,16),",",str(Norm.y,0,16),",",str(Norm.z,0,16),">",
              "uv_vectors ",
              "<", str((tr_base_1.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_1.y-d_y)/(res_y*px_inc),0,16), ">,",
              "<", str((tr_base_3.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_3.y-d_y)/(res_y*px_inc),0,16), ">,",
              "<", str((tr_base_4.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_4.y-d_y)/(res_y*px_inc),0,16), ">}",
              "\n")
      #write (fmesh, "smooth_triangle{", 
              "<",str(tr_vertex_1.x,0,16),",",str(tr_vertex_1.y,0,16),",",str(tr_vertex_1.z,0,16),">", ",", 
              "<",str(Norm.x,0,16),",",str(Norm.y,0,16),",",str(Norm.z,0,16),">", ",", 
              "<",str(tr_vertex_4.x,0,16),",",str(tr_vertex_4.y,0,16),",",str(tr_vertex_4.z,0,16),">", ",", 
              "<",str(Norm.x,0,16),",",str(Norm.y,0,16),",",str(Norm.z,0,16),">", ",", 
              "<",str(tr_vertex_5.x,0,16),",",str(tr_vertex_5.y,0,16),",",str(tr_vertex_5.z,0,16),">", ",",  
              "<",str(Norm.x,0,16),",",str(Norm.y,0,16),",",str(Norm.z,0,16),">", 
              "uv_vectors ",
              "<", str((tr_base_1.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_1.y-d_y)/(res_y*px_inc),0,16), ">,",
              "<", str((tr_base_4.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_4.y-d_y)/(res_y*px_inc),0,16), ">,",
              "<", str((tr_base_5.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_5.y-d_y)/(res_y*px_inc),0,16), ">}",
              "\n")
      #write (fmesh, "smooth_triangle{", 
              "<",str(tr_vertex_1.x,0,16),",",str(tr_vertex_1.y,0,16),",",str(tr_vertex_1.z,0,16),">", ",", 
              "<",str(Norm.x,0,16),",",str(Norm.y,0,16),",",str(Norm.z,0,16),">", ",", 
              "<",str(tr_vertex_5.x,0,16),",",str(tr_vertex_5.y,0,16),",",str(tr_vertex_5.z,0,16),">", ",", 
              "<",str(Norm.x,0,16),",",str(Norm.y,0,16),",",str(Norm.z,0,16),">", ",", 
              "<",str(tr_vertex_2.x,0,16),",",str(tr_vertex_2.y,0,16),",",str(tr_vertex_2.z,0,16),">", ",",  
              "<",str(Norm.x,0,16),",",str(Norm.y,0,16),",",str(Norm.z,0,16),">",
              "uv_vectors ",
              "<", str((tr_base_1.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_1.y-d_y)/(res_y*px_inc),0,16), ">,",
              "<", str((tr_base_5.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_5.y-d_y)/(res_y*px_inc),0,16), ">,",
              "<", str((tr_base_2.x+d_x)/(res_x*px_inc),0,16), ",", str(1-abs(tr_base_2.y-d_y)/(res_y*px_inc),0,16), ">}",
              "\n")
     #end
     #local col_count=col_count+1;
  #end
  #local row_count=row_count+1;
#end
}
#if (c_file!="")
 #write (fmesh, "}", "\n")
 #fclose fmesh
#end

#end


/*
  Creates a transform to place the mesh camera at the "location", looking at the "look_at ".
*/
#macro meshcam_placement(c_lo, c_la)

transform{
  transform{Reorient_Trans(z,<c_la.x,0,c_la.z>-<c_lo.x,0,c_lo.z>)}
  transform{Reorient_Trans(<c_la.x,0,c_la.z>-<c_lo.x,0,c_lo.z>,c_la-c_lo)}
  translate c_lo
}

#end

