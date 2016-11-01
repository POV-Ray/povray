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

@remark The red, green and blue Y coordinates, as well as the whitepoint's xy coordinates, shown above are
        approximations. Internally, higher precision is used.

This also applies to anything else that brings light into the scene, such as the texture and media `emission` effects.

@subsection in_p        Pigments

As the sRGB standard uses illuminant D65 as its whitepoint, but our internal colour model's whitepoint is necessarily
based on illuminant E (equal energy, x=y=Y/3), care must be taken in handling of pigments. Naively converting the
pigment colours in the same way as the light source colours would make a scene with straight "white" (`<1,1,1>`) light
sources and object colours render in an off-white blueish hue, especially when radiosity would be involved. To prevent
this, we interpret an RGB pigment colour specification in the sense that the pigment _appears_ to have the specified
colour _when illuminated with the sRGB standard illuminant (D65)_.

Technically, this means that for any effects that modulate a light's colour -- such as `diffuse`, metallic reflections
and highlights, or filtered transparency -- we're applying a per-coefficient scaling by the inverse of the D65
whitepont's coefficients in internal colour space.

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


@section int3           Internal Colours -- 3-Channel Variant

Internally, colours are described in terms of three spectral bands with the following parameters:

| Colour               | Wavelength          | x      | y      | Y      |
|:---------------------|:--------------------|:-------|:-------|:-------|
| **Blue**             | 377.5 nm - 492.5 nm | 0.1445 | 0.0422 | 0.0485 |
| **Green**            | 492.5 nm - 587.5 nm | 0.3013 | 0.6393 | 0.6942 |
| **Red**              | 587.5 nm - 782.5 nm | 0.6631 | 0.3367 | 0.2572 |
|                      |                     |        |        |        |
| **Whitepoint** (E)   | 377.5 nm - 782.5 nm | 0.3333 | 0.3333 | 1.0000 |

@remark The xyY coordinates shown above are approximations. Internally, higher precision is used.

For each of these four bands, energy is presumed to be distributed equally across the entire band; this is required to
keep colour math simple yet self-consistent and physically accurate within the model.

The blue minimum and red maximum wavelengths are rather theoretical in nature, and were chosen for the sole reason that
the available colorimetric data tables end there. The other boundaries were chosen by trial and error to make sure the
model entirely contains the sRGB colour space.

To convert from the internal model to RGB colour space for output, straightforward linear transformation is employed.
No clipping is performed unless mandated by the output file format.

To convert from an external colour model to the internal one, the (linear) colour is converted as follows:
 -# Transform the colour into the intrnal model using linear transformation.
 -# When used for any pigment-type effects, perform whitepoint correction by applying a per-coefficient division by the
    representation of D65.

@subsection int3_t          Transparency

Internally, transparency is represented as a full-fledged colour, using the same three-band colour model described
above, in _associated_ mode. To convert this four-channel transparency to a single channel for output, a simple
greyscale conversion is applied.

@section int4           Internal Colours -- 4-Channel Variant

As an alternative, an internal colour model can be chosen at compile-time that describes colours in terms of _four_
spectral bands with the following parameters:

| Colour               | Wavelength      | x      | y      | Y      |
|:---------------------|:----------------|:-------|:-------|:-------|
| **Blue**             | 380 nm - 490 nm | 0.1459 | 0.0385 | 0.0437 |
| **Green**            | 490 nm - 550 nm | 0.1369 | 0.7190 | 0.3704 |
| **Yellow**           | 550 nm - 590 nm | 0.4474 | 0.5508 | 0.3464 |
| **Red**              | 590 nm - 780 nm | 0.6681 | 0.3316 | 0.2395 |
|                      |                 |        |        |        |
| **Whitepoint** (E)   | 380 nm - 780 nm | 0.3333 | 0.3333 | 1.0000 |

@remark The xyY coordinates shown above are approximations. Internally, higher precision is used.

For each of these four bands, energy is presumed to be distributed equally across the entire band; this is required to
keep colour math simple yet self-consistent and physically accurate within the model.

The blue minimum and red maximum wavelengths are rather theoretical in nature, and were chosen for the sole reason that
the available colorimetric data tables end there. The other boundaries were chosen by trial and error to make sure the
model entirely contains the sRGB colour space.

@note   The 4-channel internal colour model is primarily provided as a case study for implementing custom colour models
        in 3rd party patches. It was favored in this role over a more useful elaborate model because it allows for a
        relatively simple algorithm to choose a particular metameric representation for a given sRGB or XYZ colour in a
        somewhat useful manner. An added benefit is that the extra colour channel _per se_ is unlikely to noticeably
        impact render performance on modern CPUs, so it might turn out to be a worthwhile -- if minor -- improvement to
        render quality. Anything more sophisticated than that, however, we leave up to 3rd party patches for now,
        content to have paved the way there.

To convert from the internal model to RGB colour space for output, straightforward linear transformation is employed.
No clipping is performed unless mandated by the output file format.

To convert from an external colour model to the internal one, the (linear) colour is converted as follows:
 -# Transform the colour into the internal model using a canonical linear transformation.
 -# Apply metameric power redistribuion by adding a minimal (by magnitude) multiple of a metamerically neutral term to
    bring all channels into the range from 0 to the corresponding coefficient of a canonical D65 representation. If this
    is not possible, bring all channels into the non-negative domain while minimizing the sum-of-squares of the
    magnitude by which any channels exceed the canonical D65 representation. If even that is not possible, minimize the
    sum-of-squares of the negative channels.
 -# When used for any pigment-type effects, perform whitepoint correction by applying a per-coefficient division by the
    canonical D65 representation.

The canonical linear transformation is chosen in such a way that the yellow channel is the average of the red and green
channels.

The canonical D65 representation is chosen in such a way that it is metamerically equivalent to the real thing, while
having a relative power distribution among the channels as close as possible to the real thing as well.

The metamerically neutral term's relation of coefficients is fixed by mathematical principles; its magnitude was chosen
-- quite arbitrarily -- to have a yellow component of -1.

@subsection int4_t          Transparency

Internally, transparency is represented as a full-fledged colour, using the same four-band colour model described above,
in _associated_ mode. To convert this four-channel transparency to a single channel for output, a simple greyscale
conversion is applied.
