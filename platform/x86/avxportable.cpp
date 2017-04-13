
#include "syspovconfigbase.h"
#include "avxportable.h"

#include "core/material/pattern.h"
#include "core/material/texture.h"

#ifdef TRY_OPTIMIZED_NOISE_AVX_PORTABLE

namespace pov
{

extern DBL RTable[];

#define AVXSCURVE(a) ((a)*(a)*(3.0-2.0*(a)))

#define AVXINCRSUM(m,s,x,y,z)  \
    ((s)*(RTable[m+1] + RTable[m+2]*(x) + RTable[m+4]*(y) + RTable[m+6]*(z)))

#define AVXINCRSUMP(mp,s,x,y,z) \
    ((s)*((mp[1]) + (mp[2])*(x) + (mp[4])*(y) + (mp[6])*(z)))

DBL AVXPortableNoise(const Vector3d& EPoint, int noise_generator)
{
	DBL x, y, z;
	DBL *mp;
	int tmp;
	int ix, iy, iz;
	int ixiy_hash, ixjy_hash, jxiy_hash, jxjy_hash;

	DBL sx, sy, sz, tx, ty, tz;
	DBL sum = 0.0;

	DBL x_ix, x_jx, y_iy, y_jy, z_iz, z_jz, txty, sxty, txsy, sxsy;

	// TODO FIXME - global statistics reference
	// Stats[Calls_To_Noise]++;

	if (noise_generator == kNoiseGen_Perlin)
	{
		// The 1.59 and 0.985 are to correct for some biasing problems with
		// the random # generator used to create the noise tables.  Final
		// range of values is about 5.0e-4 below 0.0 and above 1.0.  Mean
		// value is 0.49 (ideally it would be 0.5).
		sum = 0.5 * (1.59 * SolidNoise(EPoint) + 0.985);

		// Clamp final value to 0-1 range
		if (sum < 0.0) sum = 0.0;
		if (sum > 1.0) sum = 1.0;

		return sum;
	}

	x = EPoint[X];
	y = EPoint[Y];
	z = EPoint[Z];

	/* its equivalent integer lattice point. */
	/* ix = (int)x; iy = (int)y; iz = (long)z; */
	/* JB fix for the range problem */
	tmp = (x >= 0) ? (int)x : (int)(x - (1 - EPSILON));
	ix = (int)((tmp - NOISE_MINX) & 0xFFF);
	x_ix = x - tmp;

	tmp = (y >= 0) ? (int)y : (int)(y - (1 - EPSILON));
	iy = (int)((tmp - NOISE_MINY) & 0xFFF);
	y_iy = y - tmp;

	tmp = (z >= 0) ? (int)z : (int)(z - (1 - EPSILON));
	iz = (int)((tmp - NOISE_MINZ) & 0xFFF);
	z_iz = z - tmp;

	x_jx = x_ix - 1; y_jy = y_iy - 1; z_jz = z_iz - 1;

	sx = AVXSCURVE(x_ix); sy = AVXSCURVE(y_iy); sz = AVXSCURVE(z_iz);

	/* the complement values of sx,sy,sz */
	tx = 1 - sx; ty = 1 - sy; tz = 1 - sz;

	/*
	*  interpolate!
	*/
	txty = tx * ty;
	sxty = sx * ty;
	txsy = tx * sy;
	sxsy = sx * sy;
	ixiy_hash = Hash2d(ix, iy);
	jxiy_hash = Hash2d(ix + 1, iy);
	ixjy_hash = Hash2d(ix, iy + 1);
	jxjy_hash = Hash2d(ix + 1, iy + 1);

	mp = &RTable[Hash1dRTableIndex(ixiy_hash, iz)];
	sum = AVXINCRSUMP(mp, (txty*tz), x_ix, y_iy, z_iz);

	mp = &RTable[Hash1dRTableIndex(jxiy_hash, iz)];
	sum += AVXINCRSUMP(mp, (sxty*tz), x_jx, y_iy, z_iz);

	mp = &RTable[Hash1dRTableIndex(ixjy_hash, iz)];
	sum += AVXINCRSUMP(mp, (txsy*tz), x_ix, y_jy, z_iz);

	mp = &RTable[Hash1dRTableIndex(jxjy_hash, iz)];
	sum += AVXINCRSUMP(mp, (sxsy*tz), x_jx, y_jy, z_iz);

	mp = &RTable[Hash1dRTableIndex(ixiy_hash, iz + 1)];
	sum += AVXINCRSUMP(mp, (txty*sz), x_ix, y_iy, z_jz);

	mp = &RTable[Hash1dRTableIndex(jxiy_hash, iz + 1)];
	sum += AVXINCRSUMP(mp, (sxty*sz), x_jx, y_iy, z_jz);

	mp = &RTable[Hash1dRTableIndex(ixjy_hash, iz + 1)];
	sum += AVXINCRSUMP(mp, (txsy*sz), x_ix, y_jy, z_jz);

	mp = &RTable[Hash1dRTableIndex(jxjy_hash, iz + 1)];
	sum += AVXINCRSUMP(mp, (sxsy*sz), x_jx, y_jy, z_jz);

	if (noise_generator == kNoiseGen_RangeCorrected)
	{
		/* details of range here:
		Min, max: -1.05242, 0.988997
		Mean: -0.0191481, Median: -0.535493, Std Dev: 0.256828

		We want to change it to as close to [0,1] as possible.
		*/
		sum += 1.05242;
		sum *= 0.48985582;
		/*sum *= 0.5;
		sum += 0.5;*/

		if (sum < 0.0)
			sum = 0.0;
		if (sum > 1.0)
			sum = 1.0;
	}
	else
	{
		sum = sum + 0.5;                     /* range at this point -0.5 - 0.5... */

		if (sum < 0.0)
			sum = 0.0;
		if (sum > 1.0)
			sum = 1.0;
	}

	return (sum);
}


void AVXPortableDNoise(Vector3d& result, const Vector3d& EPoint)
{
	DBL x, y, z;
	DBL *mp;
	int tmp;
	int ix, iy, iz;
	int ixiy_hash, ixjy_hash, jxiy_hash, jxjy_hash;
	DBL x_ix, x_jx, y_iy, y_jy, z_iz, z_jz;
	DBL s;
	DBL sx, sy, sz, tx, ty, tz;
	DBL txty, sxty, txsy, sxsy;

	// TODO FIXME - global statistics reference
	// Stats[Calls_To_DNoise]++;

	x = EPoint[X];
	y = EPoint[Y];
	z = EPoint[Z];

	/* its equivalent integer lattice point. */
	/*ix = (int)x; iy = (int)y; iz = (int)z;
	x_ix = x - ix; y_iy = y - iy; z_iz = z - iz;*/
	/* JB fix for the range problem */
	tmp = (x >= 0) ? (int)x : (int)(x - (1 - EPSILON));
	ix = (int)((tmp - NOISE_MINX) & 0xFFF);
	x_ix = x - tmp;

	tmp = (y >= 0) ? (int)y : (int)(y - (1 - EPSILON));
	iy = (int)((tmp - NOISE_MINY) & 0xFFF);
	y_iy = y - tmp;

	tmp = (z >= 0) ? (int)z : (int)(z - (1 - EPSILON));
	iz = (int)((tmp - NOISE_MINZ) & 0xFFF);
	z_iz = z - tmp;

	x_jx = x_ix - 1; y_jy = y_iy - 1; z_jz = z_iz - 1;

	sx = AVXSCURVE(x_ix); sy = AVXSCURVE(y_iy); sz = AVXSCURVE(z_iz);

	/* the complement values of sx,sy,sz */
	tx = 1 - sx; ty = 1 - sy; tz = 1 - sz;

	/*
	*  interpolate!
	*/
	txty = tx * ty;
	sxty = sx * ty;
	txsy = tx * sy;
	sxsy = sx * sy;
	ixiy_hash = Hash2d(ix, iy);
	jxiy_hash = Hash2d(ix + 1, iy);
	ixjy_hash = Hash2d(ix, iy + 1);
	jxjy_hash = Hash2d(ix + 1, iy + 1);

	mp = &RTable[Hash1dRTableIndex(ixiy_hash, iz)];
	s = txty*tz;
	result[X] = AVXINCRSUMP(mp, s, x_ix, y_iy, z_iz);
	mp += 8;
	result[Y] = AVXINCRSUMP(mp, s, x_ix, y_iy, z_iz);
	mp += 8;
	result[Z] = AVXINCRSUMP(mp, s, x_ix, y_iy, z_iz);

	mp = &RTable[Hash1dRTableIndex(jxiy_hash, iz)];
	s = sxty*tz;
	result[X] += AVXINCRSUMP(mp, s, x_jx, y_iy, z_iz);
	mp += 8;
	result[Y] += AVXINCRSUMP(mp, s, x_jx, y_iy, z_iz);
	mp += 8;
	result[Z] += AVXINCRSUMP(mp, s, x_jx, y_iy, z_iz);

	mp = &RTable[Hash1dRTableIndex(jxjy_hash, iz)];
	s = sxsy*tz;
	result[X] += AVXINCRSUMP(mp, s, x_jx, y_jy, z_iz);
	mp += 8;
	result[Y] += AVXINCRSUMP(mp, s, x_jx, y_jy, z_iz);
	mp += 8;
	result[Z] += AVXINCRSUMP(mp, s, x_jx, y_jy, z_iz);

	mp = &RTable[Hash1dRTableIndex(ixjy_hash, iz)];
	s = txsy*tz;
	result[X] += AVXINCRSUMP(mp, s, x_ix, y_jy, z_iz);
	mp += 8;
	result[Y] += AVXINCRSUMP(mp, s, x_ix, y_jy, z_iz);
	mp += 8;
	result[Z] += AVXINCRSUMP(mp, s, x_ix, y_jy, z_iz);

	mp = &RTable[Hash1dRTableIndex(ixjy_hash, iz + 1)];
	s = txsy*sz;
	result[X] += AVXINCRSUMP(mp, s, x_ix, y_jy, z_jz);
	mp += 8;
	result[Y] += AVXINCRSUMP(mp, s, x_ix, y_jy, z_jz);
	mp += 8;
	result[Z] += AVXINCRSUMP(mp, s, x_ix, y_jy, z_jz);

	mp = &RTable[Hash1dRTableIndex(jxjy_hash, iz + 1)];
	s = sxsy*sz;
	result[X] += AVXINCRSUMP(mp, s, x_jx, y_jy, z_jz);
	mp += 8;
	result[Y] += AVXINCRSUMP(mp, s, x_jx, y_jy, z_jz);
	mp += 8;
	result[Z] += AVXINCRSUMP(mp, s, x_jx, y_jy, z_jz);

	mp = &RTable[Hash1dRTableIndex(jxiy_hash, iz + 1)];
	s = sxty*sz;
	result[X] += AVXINCRSUMP(mp, s, x_jx, y_iy, z_jz);
	mp += 8;
	result[Y] += AVXINCRSUMP(mp, s, x_jx, y_iy, z_jz);
	mp += 8;
	result[Z] += AVXINCRSUMP(mp, s, x_jx, y_iy, z_jz);

	mp = &RTable[Hash1dRTableIndex(ixiy_hash, iz + 1)];
	s = txty*sz;
	result[X] += AVXINCRSUMP(mp, s, x_ix, y_iy, z_jz);
	mp += 8;
	result[Y] += AVXINCRSUMP(mp, s, x_ix, y_iy, z_jz);
	mp += 8;
	result[Z] += AVXINCRSUMP(mp, s, x_ix, y_iy, z_jz);
}

}

#endif // TRY_OPTIMIZED_NOISE_AVX_PORTABLE

