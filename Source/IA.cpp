// IA.cpp

#include "IA.h"
#include "IncrementalIntervalAssignment.h"
#include "IAResultImplementation.h"

namespace IIA
{
  using IIA_Internal::IncrementalIntervalAssignment;
  using IIA_Internal::IAResultImplementation;
  
  IA::IA()
  {
    auto r = new IAResultImplementation();
    result = r;
    ia = new IncrementalIntervalAssignment(r);
  }
  
  IA::~IA()
  {
    delete result;
    result = nullptr;
    delete ia;
    ia = nullptr;
  }
  
  IA::IA(IA &copy_ia)
  {
    auto r = new IAResultImplementation();
    result = r;
    ia = new IncrementalIntervalAssignment(r);

    *result = *copy_ia.result;
    copy_ia.ia->copy_me( ia );
  }
  
  void IA::copy_me(IA &target)
  {
    *target.result = *result;
    ia->copy_me( target.ia );
  }

  
  //== space
  void IA::clear()
  {
    // could do this more efficiently...
    delete ia;
    ia = new IncrementalIntervalAssignment( dynamic_cast<IAResultImplementation*>(result) );
  }
  
  void IA::reserve(size_t nrows, size_t ncols)
  {
    ia->reserve_rows((int)nrows);
    ia->reserve_cols((int)ncols);
  }
  void IA::reserve_rows(size_t nrows)
  {
    ia->reserve_rows((int)nrows);
  }
  void IA::reserve_cols(size_t ncols)
  {
    ia->reserve_cols((int)ncols);
  }
  
  void IA::resize(size_t nrows, size_t ncols)
  {
    ia->reserve_rows((int)nrows);
    ia->reserve_cols((int)ncols);
    ia->freeze_problem_size();
  }
  void IA::resize_rows(size_t nrows)
  {
    ia->reserve_rows((int)nrows);
    ia->freeze_problem_size();
  }
  void IA::resize_cols(size_t ncols)
  {
    ia->reserve_cols((int)ncols);
    ia->freeze_problem_size();
  }
  
  std::pair<size_t, size_t> IA::size() const
  {
    return std::make_pair(size_rows(), size_cols());
  }
  size_t IA::size_rows() const
  {
    return ia->num_rows();
  }
  size_t IA::size_cols() const
  {
    return ia->num_cols();
  }
  int IA::used_rows() const
  {
    return ia->num_used_rows();
  }
  int IA::used_cols() const
  {
    return ia->num_used_cols();
  }
  std::pair<int, int> IA::used() const
  {
    return std::make_pair(used_rows(),used_cols());
  }
  int IA::next_row()
  {
    return ia->next_available_row();
  }
  int IA::next_col()
  {
    return ia->next_intvar();
  }
  int IA::new_row(int num_rows)
  {
    return ia->new_row(num_rows);
  }
  int IA::new_col(int num_cols)
  {
    return ia->new_col(num_cols);
  }
  void IA::clear_row(int row)
  {
    ia->clear_M(row);
  }

  void IA::set_row(int row, std::vector<int> &cols, std::vector<int> &vals)
  {
    ia->set_M(row, cols, vals);
  }
  void IA::set_row_col(int row, int col, int val)
  {
    ia->set_M(row,col,val);
  }
  void IA::set_constraint(int row, ConstraintType constraint_type)
  {
    ia->set_constraint(row, constraint_type);
  }
  
  void IA::set_rhs(int row, int val)
  {
    ia->set_B(row,val);
  }
  
  // x
  void IA::set_bounds(int col, int lo, int hi)
  {
    ia->set_lower(col,lo);
    ia->set_upper(col,hi);
  }
  void IA::set_bound_lo(int col, int lo)
  {
    ia->set_lower(col,lo);
  }
  void IA::set_bound_hi(int col, int hi)
  {
    ia->set_upper(col,hi);
  }
  void IA::set_no_bound_lo(int col)
  {
    ia->set_lower(col,std::numeric_limits<int>::lowest());
  }
  void IA::set_no_bound_hi(int col)
  {
    ia->set_upper( col, std::numeric_limits<int>::max() );
  }
  void IA::set_no_bounds(int col)
  {
    ia->set_lower( col, std::numeric_limits<int>::lowest() );
    ia->set_upper( col, std::numeric_limits<int>::max() );
  }
  // goals must be in range (0,infinity) (int max in practice)
  // goals are scaled to be in this range and the modified value is returned
  //   if goals outside this range are required, you must scale the col variable instead
  double IA::set_goal(int col, double goal)
  {
    return ia->set_goal(col,goal);
  }
  void IA::set_no_goal(int col)
  {
    return ia->set_no_goal(col);
  }
  
  // get versions of the set methods
  void IA::get_row(int row, const std::vector<int> *&cols, const std::vector<int> *&vals) const
  {
    return ia->get_M(row,cols,vals);
  }
  int IA::get_row_col(int row, int col) const
  {
    return ia->get_M_unsorted(row,col);
  }
  void IA::get_col(int col, const std::vector<int> *&rows) const
  {
    ia->get_rows(col, rows);
  }
  void IA::fill_in_cols_from_rows()
  {
    ia->fill_in_cols_from_rows();
  }

  ConstraintType IA::get_constraint(int row) const
  {
    return ia->get_constraint(row);
  }
  int IA::get_rhs(int row) const
  {
    return ia->get_B(row);
  }
  
  void IA::get_bounds(int col, int &lo, int &hi) const
  {
    lo = ia->get_lower(col);
    hi = ia->get_upper(col);
  }
  int IA::get_bound_lo(int col) const
  {
    return ia->get_lower(col);
  }
  int IA::get_bound_hi(int col) const
  {
    return ia->get_upper(col);
  }
  bool IA::has_goal(int col) const
  {
    return ia->get_goal(col) != 0.;
  }
  double IA::get_goal(int col) const
  {
    return ia->get_goal(col);
  }
  
  // solve
  bool IA::solve_from_scratch()
  {
    return ia->solve(true, true);
  }
  
  bool IA::solve()
  {
    // works for re-solving, too
    return ia->solve(true, false);
  }
  
  bool IA::solve_feasible()
  {
    return ia->solve(false, false);
  }
  
  void IA::solve_from_scratch_next_time()
  {
    ia->set_is_unsolved();
  }
  
  bool IA::is_solved() const
  {
    return ia->get_is_solved();
  }
  
  int IA::get_solution(int col) const
  {
    return ia->get_solution(col);
  }
  
  const std::vector<int> &IA::get_solution() const
  {
    return ia->get_solution();
  }
  
  int IA::solution_is_better_than_Y( std::vector<int> &Y, bool print_summary, bool print_detail )
  {
    return ia->solution_is_better_than_Y( Y, print_summary, print_detail );
  }
  
  void IA::set_use_map_nullspace(bool use_map_nullspace)
  {
    ia->use_map_nullspace = use_map_nullspace;
  }

} // namespace

