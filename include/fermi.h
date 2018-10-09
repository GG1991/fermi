/*
 *  This source code is part of MicroPP: a finite element library
 *  to solve microstructural problems for composite materials.
 *
 *  Copyright (C) - 2018 - Guido Giuntoli <gagiuntoli@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef _FERMI_H_
#define _FERMI_H_

struct material {

	int ngroups;

	double *diff_coef;      // diffusion coeficient
	double *xs_abs;         // absortion XS
	double *nu_xs_fission;  // nu x fission XS
	double *energy_xs_f;    // energy x fission XS
	double *xs_scattering;  // scattering XS
	double *xs_remotion;    // remotion XS
	double *chi;            // fission spectrum

}

struct fermi_reactor {

	int mesh_type;
	int ngroups;
	int nmaterials;

	char *gmsh_file;
	struct *material;

}

#endif
