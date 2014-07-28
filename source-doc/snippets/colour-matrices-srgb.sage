# Compute Transformation matrices for conversion between sRGB and XYZ

# Whitepoint (D65) xyY as per sRGB spec
(x_w,y_w,Y_w) = (0.3127, 0.3290, 1.0000)

# Red,Green,Blue Primaries xy as per sRGB spec
(x_r,y_r) = (0.6400, 0.3300)
(x_g,y_g) = (0.3000, 0.6000)
(x_b,y_b) = (0.1500, 0.0600)

# Compute Whitepoint,Red,Green,Blue z
z_w = 1 - x_w - y_w
z_r = 1 - x_r - y_r
z_g = 1 - x_g - y_g
z_b = 1 - x_b - y_b

# Compute Whitepoint XYZ (note that Y is already known)
X_w = Y_w * x_w/y_w
Z_w = Y_w * z_w/y_w

# Solve Red,Green,Blue Y
var('Y_r,Y_g,Y_b')
eq1 = X_w == x_r * Y_r/y_r + x_g * Y_g/y_g + x_b * Y_b/y_b
eq2 = Y_w ==       Y_r     +       Y_g     +       Y_b
eq3 = Z_w == z_r * Y_r/y_r + z_g * Y_g/y_g + z_b * Y_b/y_b
solns = solve([eq1,eq2,eq3],Y_r,Y_g,Y_b,solution_dict=True)
Y_r = solns[0][Y_r].n(100)
Y_g = solns[0][Y_g].n(100)
Y_b = solns[0][Y_b].n(100)

# Compute Red,Green,Blue XYZ (note that Y is already known)
X_r = x_r * Y_r/y_r
Z_r = z_r * Y_r/y_r

X_g = x_g * Y_g/y_g
Z_g = z_g * Y_g/y_g

X_b = x_b * Y_b/y_b
Z_b = z_b * Y_b/y_b

# Assemble transformation matrix from sRGB to XYZ
M_rgb_to_XYZ = matrix([[X_r,X_g,X_b],
                       [Y_r,Y_g,Y_b],
                       [Z_r,Z_g,Z_b]])

# Compute transformation matrix from XYZ to sRGB
M_XYZ_to_rgb = M_rgb_to_XYZ.inverse()

# Output
print
print "sRGB to XYZ:"
print M_rgb_to_XYZ
print
print "XYZ to sRGB:"
print M_XYZ_to_rgb
print
print "Illuminant E in sRGB:"
print M_XYZ_to_rgb * vector([1,1,1])
