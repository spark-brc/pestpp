#include <random>
#include <map>
#include <iomanip>
#include <mutex>
#include <thread>
#include "Ensemble.h"
#include "RestartController.h"
#include "utilities.h"
#include "Ensemble.h"
#include "EnsembleSmoother.h"
#include "ObjectiveFunc.h"
#include "covariance.h"
#include "RedSVD-h.h"
#include "SVDPackage.h"
#include "eigen_tools.h"
#include "EnsembleMethodUtils.h"




void IterEnsembleSmoother::throw_ies_error(string message)
{
	EnsembleMethod::throw_em_error(message);
}

void IterEnsembleSmoother::sanity_checks()
{
	EnsembleMethod::sanity_checks();
}



void IterEnsembleSmoother::iterate_2_solution()
{
	stringstream ss;
	ofstream &frec = file_manager.rec_ofstream();
	
	bool accept;
	int n_iter_mean = pest_scenario.get_pestpp_options().get_ies_n_iter_mean();

	for (int i = 0; i < pest_scenario.get_control_info().noptmax; i++)
	{
		iter++;
		message(0, "starting solve for iteration:", iter);
		ss.str("");
		ss << "starting solve for iteration: " << iter;
		performance_log->log_event(ss.str());
		if (pest_scenario.get_pestpp_options().get_ies_use_mda())
        {
		    accept = solve_mda(false);
        }
		else {
            accept = solve_glm();
        }
		report_and_save(NetPackage::NULL_DA_CYCLE);
		ph.update(oe,pe, weights);
		last_best_mean = ph.get_mean(L2PhiHandler::phiType::COMPOSITE);
		last_best_std = ph.get_std(L2PhiHandler::phiType::COMPOSITE);
		ph.report(true);
		ph.write(iter, run_mgr_ptr->get_total_runs());
		if (pest_scenario.get_pestpp_options().get_ies_save_rescov())
			ph.save_residual_cov(oe,iter);
		ss.str("");
		ss << file_manager.get_base_filename() << "." << iter << ".pcs.csv";
		pcs.summarize(pe,ss.str());
			
			
		if (accept)
			consec_bad_lambda_cycles = 0;
		else
			consec_bad_lambda_cycles++;

		if (iter == n_iter_mean)
        {
            reset_par_ensemble_to_prior_mean(pe,oe);
		    double phi_lam = get_lambda();
		    //if (phi_lam > last_best_lam)
            //{
            last_best_lam = phi_lam;
            message(1,"iter = ies_n_iter_mean, resetting lambda to ",last_best_lam);
            //}
		    consec_bad_lambda_cycles = 0;
            best_mean_phis.clear();
            ph = L2PhiHandler(&pest_scenario, &file_manager, &oe_base, &pe_base, &parcov);
            ph.update(oe,pe);
            last_best_mean = ph.get_mean(L2PhiHandler::phiType::COMPOSITE);
            last_best_std = ph.get_std(L2PhiHandler::phiType::COMPOSITE);
        }

		if (should_terminate())
        {
		    if (iter > pest_scenario.get_pestpp_options().get_ies_n_iter_mean()) {
                break;
            }
		    else{
		        message(1,"continuing iterations to satisfy ies_n_iter_mean");
		    }
        }

	}
}


void IterEnsembleSmoother::finalize()
{

}
