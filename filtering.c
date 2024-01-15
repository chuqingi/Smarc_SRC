/**
 * Smarc
 *
 * Copyright (c) 2009-2011 Institut T�l�com - T�l�com Paristech
 * T�l�com ParisTech / dept. TSI
 *
 * Authors : Benoit Mathieu, Jacques Prado
 *
 * This file is part of Smarc.
 *
 * Smarc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Smarc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "filtering.h"


double filter(const double* restrict filt, const double* restrict signal, const int K) {
	register double v = 0.0;
	for (int k=0;k<K;++k)
		v+=filt[k]*signal[k];
	return v;
}