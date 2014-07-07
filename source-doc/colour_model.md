# Colour Model

POV-Ray operates with different colour models internally (that is, in the rendering core) and externally (colours
specified in the SDL or via input image files, and colours displayed in the render preview and written to output image
files).

@todo   Implementation of this specification is still pending.


@section in         Input Colours

All input colour representations are based on the RGB colour model ubiquitous in computer graphics, more precisely a
linear RGB model based on the _sRGB_ primaries and white point.

@subsection in_l        Light Sources

Light sources use the standard sRGB primaries and white point:

| Colour               | x      | y      | Y      |
|:---------------------|:-------|:-------|:-------|
| **Red**              | 0.6400 | 0.3300 | 0.2126 |
| **Green**            | 0.3000 | 0.6000 | 0.7152 |
| **Blue**             | 0.1500 | 0.0600 | 0.0722 |
|                      |        |        |        |
| **Whitepoint** (D65) | 0.3127 | 0.3290 | 1.0000 |

This also applies to anything else that brings light into the scene, such as the texture and media `emission` effects.

@subsection in_p        Pigments

As the sRGB standard uses illuminant D65 as its whitepoint, but our internal colour model's whitepoint is necessarily
based on illuminant E (equal energy, x=y=Y/3), care must be taken in handling of pigments. Naively converting the
pigment colours in the same way as the light source colours would make a scene with straight "white" (`<1,1,1>`) light
sources and object colours render in an off-white blueish hue, especially when radiosity would be involved. To prevent
this, we interpret an RGB pigment colour specification in the sense that the pigment _appears_ to have the specified
colour _when illuminated with the sRGB standard illuminant (D65)_.

Technically, this means that for any effects that modulate a light's colour -- such as `diffuse`, metallic reflections
and highlights, or filtered transparency -- we're applying a simple scaling after conversion to the internal colour
space.

@subsection in_t        Transparency

To represent transparency, the three primary colour channels are complemented with a pair of additional parameters
called `filter` and `transmit` (abbreviated "F" and "T"). The effective transmitted colour can be computed as follows:

@f[
\left(\begin{array}{c}R_t\\G_t\\B_t\end{array}\right) =
\left(\begin{array}{c}R\\G\\B\end{array}\right) F + \left(\begin{array}{c}1\\1\\1\end{array}\right) T
@f]

Input colours use _unassociated_ transparency mode, i.e. the _effective_ opaque component of a transparent colour is
computed according to the following formula:

@f[
\left(\begin{array}{c}R_o\\G_o\\B_o\end{array}\right) =
\left(\begin{array}{c}R\\G\\B\end{array}\right) \left(1-F+T\right)
@f]


@section out            Output Colours

The interface between the render engine and the file output modules and GUI uses a linear high dynamic range RGB colour
model based on the sRGB primaries, complemented with an additional single transparency channel. The latter uses
_associated_ transparency mode.


@section int            Internal Colours

Internally, colours are described in terms of _four_ spectral bands with the following parameters:

| Colour               | Wavelength      | x      | y      | Y      |
|:---------------------|:----------------|:-------|:-------|:-------|
| **Red**              | 595 nm - 780 nm | 0.6784 | 0.3214 | 0.2056 |
| **Yellow**           | 550 nm - 595 nm | 0.4639 | 0.5345 | 0.3804 |
| **Green**            | 490 nm - 550 nm | 0.1369 | 0.7190 | 0.3704 |
| **Blue**             | 380 nm - 490 nm | 0.1459 | 0.0385 | 0.0437 |

For each of these four bands, energy is presumed to be distributed equally across the entire band; this is required to
keep colour math simple yet self-consistent and physically accurate within the model.

The blue minimum and red maximum wavelengths are rather theoretical in nature, and were chosen for the sole reason that
the available colorimetric data tables end there. The other boundaries were chosen by trial and error to make sure the
internal colour space entirely contains both the sRGB colour space and the pigment colour space.

@note   An initial attempt was made to design a three-band model with similar properties; however, it appears to be
        technically impossible for such a model to encompass both sRGB and the derived pigment colour space at the same
        time. While the fourth channel does add some computational overhead, this is deemed acceptable, as modern
        compilers should ideally employ SIMD instructions (Single Instruction Multiple Data) for the colour math, and
        these instructions are optimized for four channels anyway.

@note   The internal colour model was primarily designed to be clearly defined (in contrast to the old naive model)
        without a notable sacrifice in performance. It was explicitly _not_ designed for more realistically looking
        colours; while the current architecture should be sufficiently easy to extend to a much higher number of
        channels, we leave it up to 3rd party patches to fill this role for now.

To convert from the internal model to RGB colour space for output, straightforward linear transformation is employed.
No clipping is performed unless mandated by the output file format.

To convert from an external colour model to the internal one, the (linear) colour is first converted using linear
transformation such that the yellow coefficient is the average of the red and green coefficient. If the colour
represents a pigment, coefficients outside the range [0..1] will be corrected by clipping the channel and adjusting the
other ones accordingly (if at all possible); if the colour represents light intensity, only negative coefficients will
be corrected in this manner.

@subsection int_t           Transparency

Internally, transparency is represented as a full-fledged colour, using the same four-band colour model described above,
in _associated_ mode. To convert this four-channel transparency to a single channel for output, a simple greyscale
conversion is applied.
