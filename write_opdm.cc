/*
 *@BEGIN LICENSE
 *
 * v2RDM-CASSCF, a plugin to:
 *
 * Psi4: an open-source quantum chemistry software package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Copyright (c) 2014, The Florida State University. All rights reserved.
 * 
 *@END LICENSE
 *
 */

#include <psi4/psi4-dec.h>
#include <psi4/psifiles.h>
#include <psi4/libiwl/iwl.h>
#include <psi4/libpsio/psio.hpp>
#include <psi4/libtrans/integraltransform.h>
#include <psi4/libmints/mintshelper.h>

#include "v2rdm_solver.h"

using namespace psi;

namespace psi{namespace v2rdm_casscf{

void v2RDMSolver::WriteOPDM(){

    double * x_p = x->pointer();

    std::shared_ptr<PSIO> psio (new PSIO());

    psio->open(PSIF_V2RDM_D1A,PSIO_OPEN_NEW);
    psio->open(PSIF_V2RDM_D1B,PSIO_OPEN_NEW);

    psio_address addr_a = PSIO_ZERO;
    psio_address addr_b = PSIO_ZERO;

    long int counta = 0;
    long int countb = 0;

    // active-active part

    for (int h = 0; h < nirrep_; h++) {

        for (int i = 0; i < amopi_[h]; i++) {

            int ifull = full_basis[i + pitzer_offset[h]];

            for (int j = 0; j < amopi_[h]; j++) {

                int jfull = full_basis[j + pitzer_offset[h]];

                double vala = x_p[d1aoff[h] + i*amopi_[h] + j];
                double valb = x_p[d1boff[h] + i*amopi_[h] + j];

                opdm d1;

                d1.i   = ifull;
                d1.j   = jfull;

                d1.val = vala;
                psio->write(PSIF_V2RDM_D1A,"D1a",(char*)&d1,sizeof(opdm),addr_a,&addr_a);
                counta++;

                d1.val = valb;
                psio->write(PSIF_V2RDM_D1B,"D1b",(char*)&d1,sizeof(opdm),addr_b,&addr_b);
                countb++;

            }
        }
    }

    // core-core
    for (int hi = 0; hi < nirrep_; hi++) {

        for (int i = 0; i < rstcpi_[hi] + frzcpi_[hi]; i++) {

            int ifull      = i + pitzer_offset_full[hi];

            opdm d1;

            d1.i   = ifull;
            d1.j   = ifull;

            d1.val = 1.0;

            psio->write(PSIF_V2RDM_D1A,"D1a",(char*)&d1,sizeof(opdm),addr_a,&addr_a);
            counta++;

            psio->write(PSIF_V2RDM_D1B,"D1b",(char*)&d1,sizeof(opdm),addr_b,&addr_b);
            countb++;

        }
    }

    // write the number of entries in each file
    psio->write_entry(PSIF_V2RDM_D1A,"length",(char*)&counta,sizeof(long int));
    psio->write_entry(PSIF_V2RDM_D1B,"length",(char*)&countb,sizeof(long int));

    // close files
    psio->close(PSIF_V2RDM_D1A,1);
    psio->close(PSIF_V2RDM_D1B,1);

}

}} //end namespaces


