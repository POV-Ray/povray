Colour Model
============

POV-Ray operates with different colour models internally (that is, in the rendering core) and externally (colours
specified in the SDL or via input image files, and colours displayed in the render preview and written to output image
files).

@note   Implementation of this specification is still pending.


Input Colours
-------------

All input colour representations are based on the RGB colour model ubiquitous in computer graphics, more precisely a
linear RGB model based on the _sRGB_ primaries.

### Primaries

@note   While the colour model is _based_ on the sRGB primaries, the primaries we use are not always _identical_ with
        them. This is necessary to avoid user surprise.

Light sources use the standard sRGB primaries:

| Colour               | x      | y      | Y      |
|:---------------------|:-------|:-------|:-------|
| **Red**              | 0.6400 | 0.3300 | 0.2126 |
| **Green**            | 0.3000 | 0.6000 | 0.7152 |
| **Blue**             | 0.1500 | 0.0600 | 0.0722 |
|                      |        |        |        |
| **Whitepoint** (D65) | 0.3127 | 0.3290 | 1.0000 |

Note that the whitepoint does _not_ match equal energy (x=y=1/3); if we used these same primaries for pigments as well,
a user specifying both the light source and object colour as "white" (`<1,1,1>`) would find that the object would _not_
be rendered as sRGB white, but would be shifted towards blue instead; the effect would be even worse when using
radiosity. To prevent this, we define that an RGB pigment colour specification means that the pigment _appears_ to have
the specified colour _when illuminated with the sRGB standard illuminant (D65)_. This means that pigments effectively
use the following primaries:

| Colour               | x      | y      | Y      |
|:---------------------|:-------|:-------|:-------|
| **Red**              | 0.6532 | 0.3201 | 0.2126 |
| **Green**            | 0.3133 | 0.5956 | 0.7152 |
| **Blue**             | 0.1673 | 0.0636 | 0.0722 |
|                      |        |        |        |
| **Whitepoint** (E)   | 0.3333 | 0.3333 | 1.0000 |

@note   For the `emission` texture and media effects, sRGB primaries _are_ used, even though all the other texture and
        media properties use the modified primaries.

### Transparency

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


Output Colours
--------------

The interface between the render engine and the file output modules and GUI uses a linear high dynamic range RGB colour
model based on the sRGB primaries, complemented with an additional single transparency channel. The latter uses
_associated_ transparency mode.


Internal Colours
----------------

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

### Transparency

Internally, transparency is represented as a full-fledged colour, using the same four-band colour model described above,
in _associated_ mode. To convert this four-channel transparency to a single channel for output, a simple greyscale
conversion is applied.
