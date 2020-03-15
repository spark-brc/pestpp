#ifndef MOEA_H_
#define MOEA_H_

#include <unordered_map>
#include <random>
#include "FileManager.h"
#include "ObjectiveFunc.h"
#include "OutputFileWriter.h"
#include "PerformanceLog.h"
#include "RunStorage.h"

class Pest;
class RunManagerAbstract;
class RestartController;
class Objectives;
class Constraints;

class MOEA
{
public:
	static mt19937_64 rand_engine;
	MOEA(Pest &_pest_scenario, FileManager &_file_manager, Objectives *_objs, Constraints *_cons,
		const ParamTransformSeq &_par_transform, OutputFileWriter &_output_file_writer,
		PerformanceLog *_performance_log, RunManagerAbstract* _run_mgr_ptr);
	void initialize(RunManagerAbstract &run_manager, int d);
	void solve(RunManagerAbstract &run_manager, RestartController &restart_controller,
		int max_gen, double f, double cr, bool _dither_f, ModelRun &cur_run);
	~MOEA();
private:
	Pest& pest_scenario;
	const static string solver_type_name;
	FileManager &file_manager;
	Objectives *objs_ptr;
	Constraints *cons_ptr;
	const ParameterInfo *ctl_par_info_ptr;
	const ParameterGroupInfo *par_group_info_ptr;
	ParamTransformSeq par_transform;
	OutputFileWriter &output_file_writer;
	PerformanceLog *performance_log;
	RunManagerAbstract* run_mgr_ptr;
	const ObservationInfo *obs_info_ptr;
	const PriorInformation *prior_info_ptr;
	std::vector<std::string> par_list;
	Parameters max_numeric_pars;
	Parameters min_numeric_pars;
	RunStorage gen_1;
	int best_run_idx;
	int failed_runs_old;
	int failed_runs_new;
	double best_phi;
	double phi_avg_old;
	double phi_avg_new;

	void initialize_vector(Parameters &ctl_pars);
	void mutation(RunManagerAbstract &run_manager, double f, bool dither_f, double cr);
	int recombination(RunManagerAbstract &run_manager);
	void write_run_summary(std::ostream &os,
		int nrun_par, double avg_par, double min_par, double max_par,
		int nrun_can, double avg_can, double min_can, double max_can,
		int nrun_child, double avg_child, double min_child, double max_child);
};

#endif //MOEA_H_
