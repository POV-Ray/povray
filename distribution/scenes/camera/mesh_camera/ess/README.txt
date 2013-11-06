These files are the extreme supersampling demo.
Contributed by Adam Crume, Feb. 2011.

The orthographic and perspective camera versions that illustrate the problem:

	ortho-camera.pov
	persp-camera.pov

and the mesh_camera versions showing how forcing extreme supersampling can
overcome the problem.

	ess-ortho-camera.pov
	ess-persp-camera.pov

The reference examples produce images showing how default anti-aliasing totally
elimainates the grid of tiny spheres that should appear to the right of the
test object, they should be rendered as:

	povray +Iortho-camera.pov +Oortho-camera.png +FN +W800 +H600 +A
	povray +Ipersp-camera.pov +Opersp-camera.png +FN +W800 +H600 +A

whereas their mesh_camera versions are able to produce anti-aliasied images that
are a more accurate representation of the scene, they should be rendered as:

	povray +Iess-ortho-camera.pov +Oess-ortho-camera.png +FN +W800 +H600
	povray +Iess-persp-camera.pov +Oess-persp-camera.png +FN +W800 +H600

For additional information see:
http://www.adamcrume.com/blog/archive/2011/01/19/forcing-extreme-supersampling-with-pov-ray
