/*
 *    This file is part of acados.
 *
 *    acados is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 3 of the License, or (at your option) any later version.
 *
 *    acados is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with acados; if not, write to the Free Software Foundation,
 *    Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

// standard
#include <stdio.h>
#include <stdlib.h>
// acados
#include "acados/utils/print.h"
#include "acados_c/ocp_nlp_interface.h"
#include "acados_c/external_function_interface.h"
#include "acados_solver_{{ra.model_name}}.h"

int main() {

    int status = 0;
    status = acados_create();

    if (status) { 
        printf("acados_create() returned status %d. Exiting.\n", status); 
        exit(1); }

    // set initial condition
    double x0[{{ra.dims.nx}}];
    {% for i in range(ra.dims.nx): %}
    x0[{{i}}] = {{ra.constraints.x0[i]}};
    {%- endfor %}
    
    ocp_nlp_constraints_model_set(nlp_config, nlp_dims, nlp_in, 0, "lbx", x0);
    ocp_nlp_constraints_model_set(nlp_config, nlp_dims, nlp_in, 0, "ubx", x0);

    {% if ra.dims.np > 0:%}
    double p[{{ra.dims.np}}];
    {% for i in range(ra.dims.nx): %}
    p[{{i}}] = {{ra.constraints.p[i]}};
    {%- endfor %}
    {% endif %}
    
    {% if ra.dims.np > 0:%}
    {% if ra.solver_config.integrator_type == 'IRK': %}
    for (int ii = 0; ii < {{ra.dims.N}}; ii++) {
    impl_dae_fun[ii].set_param(impl_dae_fun+ii, p);
    impl_dae_fun_jac_x_xdot_z[ii].set_param(impl_dae_fun+ii, p);
    impl_dae_jac_x_xdot_u_z[ii].set_param(impl_dae_fun+ii, p);
    }
    {% else: %}
    for (int ii = 0; ii < {{ra.dims.N}}; ii++) {
    expl_vde_for[ii].set_param(expl_vde_for+ii, p);
    }
    {% endif %}
    {% endif %}

    double kkt_norm_inf = 1e12, elapsed_time;

    status = acados_solve();

    status = acados_solve();
    kkt_norm_inf = nlp_out->inf_norm_res;
    elapsed_time = nlp_out->total_time;
    printf(" iterations %2d | time  %f |  KKT %e\n", nlp_out->sqp_iter, elapsed_time, kkt_norm_inf);

    printf("\n--- solution ---\n");
    ocp_nlp_out_print(nlp_solver->dims, nlp_out);
    if (status) { 
        printf("acados_solve() returned status %d.\n", status); 
    }

    status = acados_free();

    if (status) { 
        printf("acados_free() returned status %d. \n", status); 
    }

    return status;
}
