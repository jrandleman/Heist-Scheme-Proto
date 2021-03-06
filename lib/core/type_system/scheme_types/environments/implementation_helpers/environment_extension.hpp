// Author: Jordan Randleman -- jordanran199@gmail.com -- environment_extension.hpp
// => Contains environmental extension logic of "struct environment" for the 
//    C++ Heist Scheme Interpreter
// => Defines the "extend_environment" procedure

#ifndef HEIST_SCHEME_CORE_ENVIRONMENT_EXTENSION_HPP_
#define HEIST_SCHEME_CORE_ENVIRONMENT_EXTENSION_HPP_

namespace heist {

  // Helper procedure provided by "lib/core/reader/parser.hpp"
  bool string_begins_with(const string& str, const char* substr, size_type begin = 0)noexcept;

  /******************************************************************************
  * ENVIRONMENTAL EXTENSION -- VALIDATION & VA-ARGS HELPER FUNCTIONS
  ******************************************************************************/

  data generate_va_arg_list(const data_vector::iterator& obj, const data_vector::iterator& null_obj)noexcept{
    if(obj == null_obj) return symconst::emptylist;
    data new_pair = data(make_par());
    new_pair.par->first = *obj;
    new_pair.par->second = generate_va_arg_list(obj+1,null_obj);
    return new_pair;
  }


  // Generate an improper procedure call error message
  string improper_call_alert(string name, const data_vector& vals, const str_vector& vars)noexcept{
    if(name.empty()) name = "#<procedure>"; // anonymous lambda
    // Generate the call signature
    auto call_signature = procedure_call_signature(name,vals);
    // Generate the definition signature
    string defn_signature('(' + name);
    for(const auto& var : vars) defn_signature += ' ' + var;
    // Return the comparison between the called & defined procedure signatures
    return '\n' + string(HEIST_AFMT(AFMT_35)) + "  >> Invalid Syntax:" + 
                  string(HEIST_AFMT(AFMT_01)) + ' ' + call_signature + 
           '\n' + string(HEIST_AFMT(AFMT_35)) + "  >> Defined Syntax:" + 
                  string(HEIST_AFMT(AFMT_01)) + ' ' + defn_signature + ')';
  }


  // Transforms the appropriate 'vals' into a list (for the given variadic arg)
  //   => ((lambda (. l) l) <arg1> <arg2> ... <argN>)      [ BECOMES -> ]
  //      ((lambda (l) l) (list <arg1> <arg2> ... <argN>))
  void transform_variadic_vals_into_a_list(str_vector& vars,data_vector& vals,const size_type continuation_offset)noexcept{
    const size_type va_arg_idx = vars.size()-2-continuation_offset;
    // Transform the arg names & vals as needed
    vars[va_arg_idx] = vars[va_arg_idx+1]; // shift up variadic arg name (erasing '.')
    vars.erase(vars.begin()+va_arg_idx+1); // erase the now-duplicate var-arg name
    data list_of_vals;
    if(vals.empty())
      list_of_vals = symconst::emptylist;
    else
      list_of_vals = generate_va_arg_list(vals.begin()+va_arg_idx, vals.end()-continuation_offset);
    vals.erase(vals.begin()+va_arg_idx, vals.end()-continuation_offset); // erase individual arg instances
    vals.insert(vals.end()-continuation_offset, list_of_vals); // reinsert args as a list
  }


  // Wrapper composing the above helpers
  void confirm_valid_environment_extension(str_vector& vars, const param_stats& stats, data_vector& vals, const string& name){
    auto vals_size = vals.size();
    // Confirm nullary signature matches
    if(!vals_size && stats.non_nullary_params())
      HEIST_THROW_ERR("Too few arguments supplied!" << improper_call_alert(name,vals,vars));
    if(vals_size && stats.nullary_params())
      HEIST_THROW_ERR("Too many arguments supplied!" << improper_call_alert(name,vals,vars));
    // Transform variadic arg's corresponding values into a list (if present)
    if(stats.variadic_params()) {
      if(vals_size < stats.mandatory_number_of_args())
        HEIST_THROW_ERR("Too few arguments supplied!" << improper_call_alert(name,vals,vars));
      transform_variadic_vals_into_a_list(vars,vals,stats.cps_variadic_params());
      vals_size = vals.size(); // update size after combining variadic args into a single list
    }
    // Confirm argument number and parameter number match
    const auto vars_length = vars.size();
    if(vars_length < vals_size)
      HEIST_THROW_ERR("Too many arguments supplied!" << improper_call_alert(name,vals,vars));
    else if(vars_length > vals_size)
      HEIST_THROW_ERR("Too few arguments supplied!" << improper_call_alert(name,vals,vars));
  }

  /******************************************************************************
  * ENVIRONMENT DATA STRUCTURE IMPLEMENTATION
  ******************************************************************************/

  template<bool INITIALIZING_GLOBAL_ENV = false>
  env_type extend_environment(str_vector&& vars, const param_stats& stats, data_vector& vals, env_type& base_env, const string& name = ""){
    // If valid extension, return environment w/ a new frame prepended
    if constexpr (!INITIALIZING_GLOBAL_ENV) confirm_valid_environment_extension(vars,stats,vals,name); // guarenteed valid for global init
    env_type extended_env(make_env());
    extended_env->frame = create_frame(vars,vals);
    extended_env->parent = base_env;
    return extended_env;
  }

  // R-value overload is _ONLY_ to launch the global environment
  env_type extend_environment(str_vector&& vars, data_vector&& vals, env_type& base_env){
    return extend_environment<true>(std::move(vars),param_stats(),vals,base_env,"");
  }
}

#endif