# Compute Transformation matrices for conversion between internal colour model sRGB and XYZ

# Colour bands to use
Bands = [[0,  0,  490,490, "Blue",   x, (0,0,0), 0],
         [490,490,550,550, "Green",  x, (0,0,0), 0],
         [550,550,590,590, "Yellow", x, (0,0,0), 0],
         [590,590,999,999, "Red",    x, (0,0,0), 0]]

# CIE 1931 (2 degree) standard colorimetric observer raw data
CIE_1931_raw = {
    380 : (0.001368, 0.000039, 0.006450),
    385 : (0.002236, 0.000064, 0.010550),
    390 : (0.004243, 0.000120, 0.020050),
    395 : (0.007650, 0.000217, 0.036210),
    400 : (0.014310, 0.000396, 0.067850),
    405 : (0.023190, 0.000640, 0.110200),
    410 : (0.043510, 0.001210, 0.207400),
    415 : (0.077630, 0.002180, 0.371300),
    420 : (0.134380, 0.004000, 0.645600),
    425 : (0.214770, 0.007300, 1.039050),
    430 : (0.283900, 0.011600, 1.385600),
    435 : (0.328500, 0.016840, 1.622960),
    440 : (0.348280, 0.023000, 1.747060),
    445 : (0.348060, 0.029800, 1.782600),
    450 : (0.336200, 0.038000, 1.772110),
    455 : (0.318700, 0.048000, 1.744100),
    460 : (0.290800, 0.060000, 1.669200),
    465 : (0.251100, 0.073900, 1.528100),
    470 : (0.195360, 0.090980, 1.287640),
    475 : (0.142100, 0.112600, 1.041900),
    480 : (0.095640, 0.139020, 0.812950),
    485 : (0.057950, 0.169300, 0.616200),
    490 : (0.032010, 0.208020, 0.465180),
    495 : (0.014700, 0.258600, 0.353300),
    500 : (0.004900, 0.323000, 0.272000),
    505 : (0.002400, 0.407300, 0.212300),
    510 : (0.009300, 0.503000, 0.158200),
    515 : (0.029100, 0.608200, 0.111700),
    520 : (0.063270, 0.710000, 0.078250),
    525 : (0.109600, 0.793200, 0.057250),
    530 : (0.165500, 0.862000, 0.042160),
    535 : (0.225750, 0.914850, 0.029840),
    540 : (0.290400, 0.954000, 0.020300),
    545 : (0.359700, 0.980300, 0.013400),
    550 : (0.433450, 0.994950, 0.008750),
    555 : (0.512050, 1.000000, 0.005750),
    560 : (0.594500, 0.995000, 0.003900),
    565 : (0.678400, 0.978600, 0.002750),
    570 : (0.762100, 0.952000, 0.002100),
    575 : (0.842500, 0.915400, 0.001800),
    580 : (0.916300, 0.870000, 0.001650),
    585 : (0.978600, 0.816300, 0.001400),
    590 : (1.026300, 0.757000, 0.001100),
    595 : (1.056700, 0.694900, 0.001000),
    600 : (1.062200, 0.631000, 0.000800),
    605 : (1.045600, 0.566800, 0.000600),
    610 : (1.002600, 0.503000, 0.000340),
    615 : (0.938400, 0.441200, 0.000240),
    620 : (0.854450, 0.381000, 0.000190),
    625 : (0.751400, 0.321000, 0.000100),
    630 : (0.642400, 0.265000, 0.000050),
    635 : (0.541900, 0.217000, 0.000030),
    640 : (0.447900, 0.175000, 0.000020),
    645 : (0.360800, 0.138200, 0.000010),
    650 : (0.283500, 0.107000, 0.000000),
    655 : (0.218700, 0.081600, 0.000000),
    660 : (0.164900, 0.061000, 0.000000),
    665 : (0.121200, 0.044580, 0.000000),
    670 : (0.087400, 0.032000, 0.000000),
    675 : (0.063600, 0.023200, 0.000000),
    680 : (0.046770, 0.017000, 0.000000),
    685 : (0.032900, 0.011920, 0.000000),
    690 : (0.022700, 0.008210, 0.000000),
    695 : (0.015840, 0.005723, 0.000000),
    700 : (0.011359, 0.004102, 0.000000),
    705 : (0.008111, 0.002929, 0.000000),
    710 : (0.005790, 0.002091, 0.000000),
    715 : (0.004109, 0.001484, 0.000000),
    720 : (0.002899, 0.001047, 0.000000),
    725 : (0.002049, 0.000740, 0.000000),
    730 : (0.001440, 0.000520, 0.000000),
    735 : (0.001000, 0.000361, 0.000000),
    740 : (0.000690, 0.000249, 0.000000),
    745 : (0.000476, 0.000172, 0.000000),
    750 : (0.000332, 0.000120, 0.000000),
    755 : (0.000235, 0.000085, 0.000000),
    760 : (0.000166, 0.000060, 0.000000),
    765 : (0.000117, 0.000042, 0.000000),
    770 : (0.000083, 0.000030, 0.000000),
    775 : (0.000059, 0.000021, 0.000000),
    780 : (0.000042, 0.000015, 0.000000)
}

# CIE D65 relative spectral power distribution raw data
CIE_D65_raw = {
    380 :  49.975500,
    385 :  52.311800,
    390 :  54.648200,
    395 :  68.701500,
    400 :  82.754900,
    405 :  87.120400,
    410 :  91.486000,
    415 :  92.458900,
    420 :  93.431800,
    425 :  90.057000,
    430 :  86.682300,
    435 :  95.773600,
    440 : 104.865000,
    445 : 110.936000,
    450 : 117.008000,
    455 : 117.410000,
    460 : 117.812000,
    465 : 116.336000,
    470 : 114.861000,
    475 : 115.392000,
    480 : 115.923000,
    485 : 112.367000,
    490 : 108.811000,
    495 : 109.082000,
    500 : 109.354000,
    505 : 108.578000,
    510 : 107.802000,
    515 : 106.296000,
    520 : 104.790000,
    525 : 106.239000,
    530 : 107.689000,
    535 : 106.047000,
    540 : 104.405000,
    545 : 104.225000,
    550 : 104.046000,
    555 : 102.023000,
    560 : 100.000000,
    565 :  98.167100,
    570 :  96.334200,
    575 :  96.061100,
    580 :  95.788000,
    585 :  92.236800,
    590 :  88.685600,
    595 :  89.345900,
    600 :  90.006200,
    605 :  89.802600,
    610 :  89.599100,
    615 :  88.648900,
    620 :  87.698700,
    625 :  85.493600,
    630 :  83.288600,
    635 :  83.493900,
    640 :  83.699200,
    645 :  81.863000,
    650 :  80.026800,
    655 :  80.120700,
    660 :  80.214600,
    665 :  81.246200,
    670 :  82.277800,
    675 :  80.281000,
    680 :  78.284200,
    685 :  74.002700,
    690 :  69.721300,
    695 :  70.665200,
    700 :  71.609100,
    705 :  72.979000,
    710 :  74.349000,
    715 :  67.976500,
    720 :  61.604000,
    725 :  65.744800,
    730 :  69.885600,
    735 :  72.486300,
    740 :  75.087000,
    745 :  69.339800,
    750 :  63.592700,
    755 :  55.005400,
    760 :  46.418200,
    765 :  56.611800,
    770 :  66.805400,
    775 :  65.094100,
    780 :  63.382800
}

# Compute CIE 1931 (2 degree) standard colorimetric observer data normalized to sum up to (1,1,1)
CIE_1931_begin = min(k for k in CIE_1931_raw)
CIE_1931_end   = max(k for k in CIE_1931_raw)
CIE_1931_sum   = max(sum(vector(v) for v in CIE_1931_raw.itervalues()))
CIE_1931_norm  = {k:vector(v)/CIE_1931_sum for k,v in CIE_1931_raw.iteritems()}

# Compute D65 relative spectral power distribution data normalized to a maximum of 1
CIE_D65_max  = max(v for v in CIE_D65_raw.itervalues())
CIE_D65_norm = {k:v/CIE_D65_max for k,v in CIE_D65_raw.iteritems()}

# Compute colour band secondary data:
#   [5] = spectral power distribution function
#   [6] = XYZ coordinate [5]
#   [7] = spectral power distributoin function integral
#
f0(x) = 0
f1(x) = 1
for b in Bands:
    f = Piecewise([[ (-999,      b[0]-1e-10), f0                                 ],
                   [ (b[0]-1e-10,b[1]+1e-10), (x-(b[0]-1e-10))/(b[1]-b[0]+2e-10) ],
                   [ (b[1]+1e-10,b[2]-1e-10), f1                                 ],
                   [ (b[2]-1e-10,b[3]+1e-10), (x-(b[3]+1e-10))/(b[2]-b[3]-2e-10) ],
                   [ (b[3]+1e-10,9999),       f0                                 ]])
    b[5] = f;
    b[6] = sum(CIE_1931_norm[lmbd] * f(lmbd) for lmbd in range(CIE_1931_begin,CIE_1931_end+1,5))
    b[7] = sum(f(lmbd) for lmbd in range(CIE_1931_begin,CIE_1931_end+1,5))

# Assemble transformation matrix to XYZ
M_to_XYZ = matrix([ b[6] for b in Bands ]).transpose()

# Compute transformation matrix from XYZ with Yellow == (Red+Green)/2
M_split_Yellow_to_XYZ = matrix([ Bands[0][6],
                                 Bands[1][6] + Bands[2][6]/2,
                                 Bands[3][6] + Bands[2][6]/2 ]).transpose()
M_split_Yellow_from_XYZ = M_split_Yellow_to_XYZ.inverse()
M_from_XYZ = matrix([ M_split_Yellow_from_XYZ[0],
                      M_split_Yellow_from_XYZ[1],
                     (M_split_Yellow_from_XYZ[1]+M_split_Yellow_from_XYZ[2])/2,
                      M_split_Yellow_from_XYZ[2]])

# Compute metameric adjustment of power distribution
M_no_Yellow_to_XYZ = matrix([ Bands[0][6],
                              Bands[1][6],
                              Bands[3][6] ]).transpose()
V_alternative_Yellow = M_no_Yellow_to_XYZ.inverse() * M_to_XYZ * vector([0,0,1,0])
V_metameric = vector([ V_alternative_Yellow[0], V_alternative_Yellow[1], -1.0, V_alternative_Yellow[2] ])

# Compute Illuminant D65 XYZ at maximum power
D65_XYZ = sum(vector(v)*CIE_D65_norm[k] for k,v in CIE_1931_norm.iteritems())

################################################################################
# sRGB Colour Space

# Whitepoint (D65) xyY
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

################################################################################

# Compute pigment mode whitepoint compensation
D65_internal = vector([
    sum([
        v*(b[5])(k) for k,v in CIE_D65_norm.iteritems()
    ])/b[7] for b in Bands
])
rgb_D65 = M_from_XYZ * M_rgb_to_XYZ * vector([1,1,1])
var('p,q')
f_D65 = rgb_D65 * p + q * V_metameric
f_D65_err = f_D65(p,q)-D65_internal
f_D65_errSqr = sum(f_D65_err(p,q).pairwise_product(f_D65_err(p,q)))
(D65_p,D65_q) = minimize(f_D65_errSqr, [0,0])
pigment_D65_decompensation = rgb_D65 + (D65_q/D65_p) * V_metameric
pigment_D65_compensation = vector([1/v for v in pigment_D65_decompensation])

# Compute transformation matrices from and to sRGB
M_from_rgb = M_from_XYZ * M_rgb_to_XYZ
M_light_to_rgb = M_XYZ_to_rgb * M_to_XYZ

# Output
for b in Bands :
    print
    print b[4], "band (" + str(b[0]) + ".." + str(b[3]) + " nm):"
    print "XYZ =", b[6]
    print "xyz =", b[6] / sum(b[6])
print
print "White:"
print "XYZ =", sum([b[6] for b in Bands])
print
print "XYZ to Internal:"
print M_from_XYZ
print
print "sRGB to Internal:"
print M_from_rgb
print
print "Internal to XYZ:"
print M_to_XYZ
print
print "Internal to sRGB:"
print M_light_to_rgb
print
print "Canonical D65:"
print pigment_D65_decompensation
print
print "Metameric adjustment:"
print V_metameric

# Demonstration that the entire sRGB colour space can be represented by our internal model
test_list = [[(0,0,0),0,0], [(1,0,0),0.24,0], [(0,1,0),0,-0.2], [(0,0,1),0,0], [(1,1,0),0,-0.04], [(1,0,1),0.2,0], [(0,1,1),0,-0.3], [(1,1,1),D65_q/D65_p,0]]
print
print "Fitness demonstration (light):"
print "(all coefficients shall be non-negative)"
for it in test_list:
    rgb = vector(it[0]) ; light_q = it[1] ; pigment_q = it[2]
    light = M_from_rgb * rgb + light_q * V_metameric
    print rgb, "->", light
print
print "Fitness demonstration (pigment):"
print "(all coefficients shall be in the range [0..1])"
for it in test_list:
    rgb = vector(it[0]) ; light_q = it[1] ; pigment_q = it[2]
    light = M_from_rgb * rgb + light_q * V_metameric
    pigment = (light + pigment_q * V_metameric).pairwise_product(pigment_D65_compensation)
    print rgb, "->", pigment

