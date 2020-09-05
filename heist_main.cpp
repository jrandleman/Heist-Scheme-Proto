// Author: Jordan Randleman -- jrandleman@scu.edu -- heist_main.cpp
// => Main execution and AST evaluation for the C++ Heist Scheme Interpreter

#ifndef HEIST_MAIN_CPP_
#define HEIST_MAIN_CPP_

/***
 * COMPILE: 
 *   $ clang++ -std=c++17 -O3 -o heist_main heist_main.cpp
 *
 * FLAG DESCRIPTIONS:
 *   0. "-std=c++17": [REQUIRED] compile using the C++17 standard
 *   1. "-O3": [RECOMMENDED FOR FASTEST EXECUTION] maximum optimization
 *             -> longest compile time, but fastest runtime
 *   2. "-Os": [RECOMMENDED FOR MOST BUILDS] optimizes for binary's size
 *             -> faster compile-time than -O3, smaller binary, & close runtime
 *
 * ON COMPILE TIME:
 *   0. Full -O3 compilation takes about 30s. Be patient. Compilation
 *      time has been traded for FAST runtime.
 *   1. -Os compilation takes about 20s. Generated binary is smaller than
 *      -O3's (as expected) & its runtime is nearly as fast
 */

/******************************************************************************
* ABSOLUTE FILE PATH TO HEIST INTERPRETERS DIRECTORY
******************************************************************************/

#if __has_include("heist_interpreter_headers/HEIST_FILEPATH.hpp")
  #include "heist_interpreter_headers/HEIST_FILEPATH.hpp"
#else
  #error "heist_installer.cpp" MUST BE COMPILED (USING "-std=c++17") AND RUN PRIOR THE INTERPRETER!
#endif

/******************************************************************************
* THE CIRCULAR EVALUATOR
******************************************************************************/

// +-----------------------------------------------------------------------+
// | It is no exaggeration to regard this as the most fundamental idea in  |
// | programming:                                                          |
// |    The evaluator, which determines the meaning of expressions in a    |
// |    programming language, is just another program. - SICP vol2, p.360  |
// +-----------------------------------------------------------------------+

/***
 * NUMBER SYSTEM:
 *   - EXACT INTERGERS (UNBOUND)
 *   - EXACT FRACTIONS (> LDBL_DIG <cfloat> DIGITS = COLLAPSE TO INEXACT FLOAT)
 *   - INEXACT FLOATS  (LONG DOUBLE)
 *   - UNSUPPORTED NUMERICS:
 *     > NUMBER::RECTANGULAR, NUMBER::POLAR, NUMBER::COMPLEX
 *     > S F D L INEXACT PRECISIONS (ALWAYS LDBL_DIG DIGITS)
 *
 * CHARS: USE ASCII ENCODING
 *
 * R4RS EXTENSIONS:
 *   - OPT-IN DYNAMIC SCOPING  ; call/ce & inline FUNCTION APPLICATIONS
 *   - OPT-IN CONTINUATIONS    ; "scm->cps" SPECIAL FORM W/ call/cc
 *   - NATIVE EVEN STREAMS     ; LISTS WITH DELAYED CAR & CDR
 *   - GENERIC ALGORITHMS      ; POLYMORPHIC ALGORITHM PRIMITIVES
 *   - SRFI PRIMITIVES         ; LIST, VECTOR, STRING, ETC.
 *   - EVAL                    ; EVALUATE SYMBOLIC DATA AS CODE
 *   - HYGIENIC MACROS         ; MATCH SYMBOL-LIST PATTERNS TO TEMPLATES
 *     > SYNTAX-RULES
 *     > DEFINE-SYNTAX
 *     > LET-SYNTAX
 *     > LETREC-SYNTAX
 *   - STRING I/O              ; READ/WRITE COMPATIBILITY W/ STRINGS AS PORTS
 *   - RECURSIVE DEPTH CONTROL ; SET THE INTERPRETER'S MAX RECURSION DEPTH
 *   - VECTOR-LITERAL          ; LONG-HAND VARIANT OF THE #( PREFIX
 *   - AND MORE!
 */

/***
 * RESERVED WORDS -VS- SPECIAL FORMS -VS- PRIMITIVES: 
 *   (A) RESERVED WORDS:
 *       - DEFINITION: IMPLEMENTATION-SPECIFIC RESERVED INTERNAL OBJECT NAMES
 *       - PROPERTIES: 1) ***RESERVED SYMBOLS BEGIN WITH "heist:core:"***
 *                     2) ***USE BY USER IS UNDEFINED BEHAVIOR***
 *
 *   (B) SPECIAL FORMS:
 *       - DEFINITION: EXPLICIT KEYWORDS PARSED FOR BY THE INTERPRETER
 *       - PROPERTIES: 1) ***REDEFINITION BY USER IS UNDEFINED BEHAVIOR***
 *                     2) SPECIAL FORM ARGS ARE NOT EVALUATED PRIOR APPLICATION
 *                     3) USERS CAN DEFINE THEIR OWN SPECIAL FORMS VIA MACROS
 *       - EXAMPLES:
 *         * '                ; quote
 *         * `                ; quasiquote
 *         * ,                ; unquote
 *         * ,@               ; unquote-splicing
 *         * .                ; VARIADIC ARGS, cons LITERAL
 *         * #\               ; CHARACTER PREFIX
 *         * =>               ; APPLY CONDITION RESULT (FOR cond)
 *         * quote            ; SYMBOLIZE ARGS (CONVERT CODE TO DATA)
 *         * quasiquote       ; SELECTIVELY eval AND SYMBOLIZE CODE
 *         * unquote          ; eval quasiquote CODE
 *         * unquote-splicing ; eval AND SPLICE IN quasiquote CODE'S RESULT
 *         * define-syntax    ; RUN-TIME MACRO DEFINITION
 *         * core-syntax      ; ANALYSIS-TIME GLOBAL MACRO DEFINITION
 *         * let-syntax       ; LOCALLY-SCOPED MACRO DEFINITION
 *         * letrec-syntax    ; LOCALLY-SCOPED RECURSIVE MACRO DEFINITION
 *         * syntax-rules     ; SYNTAX OBJECT
 *         * lambda           ; ANONYMOUS PROCEDURE
 *         * define           ; BIND VARIABLE TO VALUE
 *         * set!             ; ASSIGN VARIABLE A NEW VALUE (MUTATATION)
 *         * begin            ; SEQUENTIALLY eval ARGS
 *         * delay            ; DELAY ARG eval (RETURNS PROMISE)
 *         * if               ; ARG1 ? ARG2 : ARG3
 *         * and              ; ALL ARGS ARE TRUE
 *         * or               ; NOT ALL ARGS ARE FALSE
 *         * cond             ; ALTERNATIVE TO NESTED IF-ELSE CHAINS
 *         * case             ; SWITCH-STATEMENT EQUIVALENT IN HEIST SCHEME
 *         * let              ; LOCALLY-SCOPED DEFINITIONS
 *         * let*             ; let WITH BINDINGS IN TERMS OF ONE ANOTHER
 *         * letrec           ; let WITH RECURSIVE BINDINGS
 *         * do               ; ITERATION CONSTRUCT ('LOOP' MECHANISM)
 *         * scons            ; STREAM-PAIR CONSTRUCTION
 *         * stream           ; STREAM CONSTRUCTION
 *         * vector-literal   ; LONGHAND OF #( PREFIX
 *         * cps-quote        ; RETURNS DATA AS CPS-EXPANDED QUOTED LIST
 *         * scm->cps         ; SCOPED CPS TRANSFORMATION
 *
 *    (C) PRIMITIVES:
 *       - DEFINITION: C++ FUNCTIONS DEFINED IN THE HEIST GLOBAL ENVIRONMENT
 *       - PROPERTIES: 1) ***MAY BE REDEFINED BY USER AT RUN TIME***
 *                     2) MAY BE TREATED AS IF ANY OTHER HEIST PROCEDURE
 *       - EXAMPLES: 
 *         * (exit)             ; TERMINATE THE INTERPRETER
 *         * #t                 ; TRUE BOOLEAN VALUE
 *         * #f                 ; FALSE BOOLEAN VALUE
 *         * stream-null        ; EMPTY STREAM OBJECT
 *         * null-environment   ; eval IN DISJOINT GLOBAL ENVIRONMENT (FOR eval/load)
 *         * local-environment  ; eval IN LOCAL SCOPE (FOR eval/load)
 *         * global-environment ; eval IN GLOBAL SCOPE (FOR eval/load)
 *         * SEE "heist_primitives.hpp" FOR THE ALL PRIMITIVE IMPLEMENTATIONS
 */

#include "heist_interpreter_headers/heist_types.hpp"
#include "heist_interpreter_headers/heist_primitives.hpp"
#include "heist_interpreter_headers/heist_input_parser.hpp"

namespace heist {

  scm_list scm_eval(scm_list&& exp, env_type& env);
  exe_type scm_analyze(scm_list&& exp,const bool tail_call,const bool cps_block);
  scm_string generate_unique_cps_hash()noexcept;

  /******************************************************************************
  * AST-ANALYSIS HELPER FUNCTIONS
  ******************************************************************************/

  // Confirm whether list begins w/ designated symbol
  bool is_tagged_list(const scm_list& exp, const char* const tag)noexcept{
    return !exp.empty() && exp[0].is_type(types::sym) && exp[0].sym == tag;
  }


  // 'Casts' a data object to a scm_list object
  //   => If data = argless application, adds sentinel-arg
  //   => If data contains a scm_list, degrades data to scm_list
  //   => Else, wraps data in a scm_list
  scm_list scm_list_cast(const data& d)noexcept{
    // if at an argless application (ie a single entity in an expression)
    if(d.is_type(types::exp) && d.exp.size()==1) {
      scm_list argless_app(2); // add sentinel-arg to argless application
      argless_app[0] = d.exp[0], argless_app[1] = scm_list(2);
      argless_app[1].exp[0] = symconst::quote;
      argless_app[1].exp[1] = symconst::sentinel_arg;
      return argless_app;
    }
    if(d.is_type(types::exp)) return d.exp;
    return scm_list(1,d);
  }


  // 'Casts' a scm_list object to a data object
  //   => If scm_list only contains 1 data object, degrades scm_list to the data
  //   => Else, wraps scm_list in a datum
  data data_cast(const scm_list& l)noexcept{
    if(l.size() == 1) return l[0]; 
    return data(l);
  }


  // Generate a call signature from a procedure name & its given values
  sym_type procedure_call_signature(const sym_type& name,const frame_vals& vals)noexcept{
    if(no_args_given(vals) || data_is_the_SENTINEL_VAL(data(vals)))
      return '(' + name + ')';
    return '(' + name + ' ' + cio_expr_str<&data::write>(vals).substr(1);
  }


  // Generate an improper procedure call error message
  sym_type improper_call_alert(sym_type name, const frame_vals& vals,
                                              const frame_vars& vars)noexcept{
    if(name.empty()) name = " #<procedure>"; // anonymous lambda
    name.erase(0,1);                         // rm initial ' '
    // Generate the call signature
    auto call_signature = procedure_call_signature(name,vals);
    // Generate the definition signature (w/o sentinel arg)
    sym_type defn_signature('(' + name);
    if(vars.size() != 1 || vars[0] != symconst::sentinel_arg)
      for(const auto& var : vars) defn_signature += ' ' + var;
    // Return the comparison between the called & defined procedure signatures
    return '\n' + scm_string(afmt(AFMT_35)) + "  >> Invalid Syntax:" + 
                  scm_string(afmt(AFMT_01)) + ' ' + call_signature + 
           '\n' + scm_string(afmt(AFMT_35)) + "  >> Defined Syntax:" + 
                  scm_string(afmt(AFMT_01)) + ' ' + defn_signature + ')';
  }

  /******************************************************************************
  * AST DATA VALIDATION HELPER FUNCTIONS
  ******************************************************************************/

  // Throw an error if 'vars' contains duplicate or non-symbol arg names, 
  //   or improper (.) use
  void confirm_valid_procedure_parameters(const scm_list& vars,const scm_list& exp){
    const size_type n = vars.size();
    // variadic (.) arg must have a label afterwards
    if(n != 0 && vars[n-1].sym == symconst::period)
      THROW_ERR("Expected one item after variadic dot (.)! -- ANALYZE_LAMBDA"<<EXP_ERR(exp));
    // Search the vars list of the fcn's args for improper (.) use & duplicate arg names
    for(size_type i = 0; i < n; ++i) {
      if(!vars[i].is_type(types::sym)) // args must be symbols
        THROW_ERR("Non-Symbolic parameter [ "<<vars[i]<<" ] is an invalid arg name! -- ANALYZE_LAMBDA"<<EXP_ERR(exp));
      if(i+2 != n && vars[i].sym == symconst::period) { // variadic (.) must come just prior the last arg
        if(i+3 == n && vars[i+2].sym.find(symconst::continuation) == 0)
          continue; // allow continuations after variadic
        THROW_ERR("More than one item found after variadic dot (.)! -- ANALYZE_LAMBDA"<<EXP_ERR(exp));
      }
      for(size_type j = i+1; j < n; ++j)
        if(vars[i].sym == vars[j].sym) // duplicate arg name detected
          THROW_ERR("Duplicate arg name \""<<vars[i]<<"\" supplied! -- ANALYZE_LAMBDA"<<EXP_ERR(exp));
    }
  }


  // Confirm valid argument layout for variable assignment
  void confirm_valid_assignment(const scm_list& exp) {
    if(exp.size() != 3)
      THROW_ERR("'set! didn't receive 2 arguments: (set! <var> <val>)" << EXP_ERR(exp));
    if(!exp[1].is_type(types::sym))
      THROW_ERR("'set! 1st arg " << PROFILE(exp[1]) << " can't be reassigned"
        " (only symbols)!\n     (set! <var> <val>)" << EXP_ERR(exp));
  }


  // Confirm valid argument layout for variable & procedure definitions
  void confirm_valid_definition(const scm_list& exp) {
    if(exp.size() < 3)
      THROW_ERR("'define didn't receive enough arguments!\n     (define <var> <val>)"
        "\n     (define (<procedure-name> <args>) <body>)" << EXP_ERR(exp));
    if(!exp[1].is_type(types::sym) && !exp[1].is_type(types::exp))
      THROW_ERR("'define 1st arg [ " << exp[1] << " ] of type \"" 
        << exp[1].type_name() << "\" can't be defined (only symbols):"
        "\n     (define <var> <val>)"
        "\n     (define (<procedure-name> <args>) <body>)" << EXP_ERR(exp));
    if(exp[1].is_type(types::exp) && 
        (exp[1].exp.empty() || !exp[1].exp[0].is_type(types::sym)))
      THROW_ERR("'define procedure name [ " 
        << (exp[1].exp.empty() ? data("undefined") : exp[1].exp[0]) << " ] of type \"" 
        << (exp[1].exp.empty() ? "undefined" : exp[1].exp[0].type_name())
        << "\" is invalid (must be a symbol)!"
        "\n     (define <var> <val>)"
        "\n     (define (<procedure-name> <args>) <body>)" << EXP_ERR(exp));
    if(exp[1].is_type(types::sym) && exp.size() > 3)
      THROW_ERR("'define can only define 1 value to a variable (received " << exp.size()-2 << " vals)!"
        "\n     (define <var> <val>)"
        "\n     (define (<procedure-name> <args>) <body>)" << EXP_ERR(exp));
  }


  // Confirm valid argument layout for a lambda
  void confirm_valid_lambda(const scm_list& exp) {
    if(exp.size() < 3)
      THROW_ERR("'lambda special form didn't receive enough args: (lambda (<args>) <body>)"
        << EXP_ERR(exp));
    if(!exp[1].is_type(types::exp) && !primitive_IS_THE_EMPTY_LIST(exp[1]))
      THROW_ERR("'lambda 1st arg [ " << exp[1] << " ] of type \"" 
        << exp[1].type_name() << "\" wasn't a proper parameter list!"
           "\n     (lambda (<args>) <body>)"
        << EXP_ERR(exp));
  }

  /******************************************************************************
  * ENVIRONMENTAL EXTENSION -- FRAME VARS/VALS EVALUATION HELPER FUNCTIONS
  ******************************************************************************/

  // Transforms the appropriate 'vals' into a list (for the given variadic arg)
  //   => ((lambda (. l) l) <arg1> <arg2> ... <argN>)      [ BECOMES -> ]
  //      ((lambda (l) l) (list <arg1> <arg2> ... <argN>))
  void transform_variadic_vals_into_a_list(frame_vars& vars,frame_vals& vals)noexcept{
    const size_type continuation_offset = vars.rbegin()->find(symconst::continuation)==0;
    const size_type va_arg_idx = vars.size()-2-continuation_offset;
    // Transform the arg names & vals as needed
    vars[va_arg_idx] = vars[va_arg_idx+1]; // shift up variadic arg name (erasing '.')
    vars.erase(vars.begin()+va_arg_idx+1); // erase the now-duplicate var-arg name
    data list_of_vals;
    if(no_args_given(vals))
      list_of_vals = symconst::emptylist;
    else
      list_of_vals = primitive_LIST_to_CONS_constructor(vals.begin()+va_arg_idx, vals.end()-continuation_offset);
    vals.erase(vals.begin()+va_arg_idx, vals.end()-continuation_offset); // erase individual arg instances
    vals.insert(vals.end()-continuation_offset, list_of_vals); // reinsert args as a list
  }


  // Confirm given no args & NOT applying a void-arg fcn & NOT a variadic-arg fcn
  bool invalid_sentinel_arg_use(const frame_vars& vars,const frame_vals& vals)noexcept{
    return no_args_given(vals) &&
            !(vars.size()==1 && vars[0]==symconst::sentinel_arg) && 
            !(vars.size()==2 && vars[0][0] == '.' && !vars[0][1]);
  }


  // Confirm passing an arg to an argless procedure
  bool given_arg_for_argless_fcn(const frame_vars& vars,const frame_vals& vals)noexcept{
    return !no_args_given(vals) && vars.size()==1 && vars[0]==symconst::sentinel_arg;
  }


  // Determine whether proc takes variadic args
  bool variadic_arg_declaration(const frame_vars& vars) {
    return (vars.size() > 1 && vars[vars.size()-2] == symconst::period) || 
           (vars.size() > 2 && vars[vars.size()-1].find(symconst::continuation) == 0
                            && vars[vars.size()-3] == symconst::period);
  }


  // Determine whether enough vals for the variadic arg decl
  bool invalid_variadic_arg_declaration(const frame_vars& vars, const frame_vals& vals){
    return vals.size() < vars.size() - 2 -
      (vars.size() > 2 && vars[vars.size()-1].find(symconst::continuation) == 0
                       && vars[vars.size()-3] == symconst::period); // - again if at a continuation
  }


  // Wrapper composing the above helpers
  bool confirm_valid_environment_extension(frame_vars& vars, frame_vals& vals, 
                                                       const sym_type& name){
    // Confirm proper use of the SENTINEL_ARG
    if(invalid_sentinel_arg_use(vars,vals))
      THROW_ERR("Too few arguments supplied! -- EXTEND_ENVIRONMENT" 
        << improper_call_alert(name,vals,vars));
    if(given_arg_for_argless_fcn(vars,vals))
      THROW_ERR("Too many arguments supplied! -- EXTEND_ENVIRONMENT" 
        << improper_call_alert(name,vals,vars));
    // Transform variadic arg's corresponding values into a list (if present)
    if(variadic_arg_declaration(vars)) {
      if(invalid_variadic_arg_declaration(vars,vals))
        THROW_ERR("Too few arguments supplied! -- EXTEND_ENVIRONMENT" 
        << improper_call_alert(name,vals,vars));
      transform_variadic_vals_into_a_list(vars,vals);
    }
    return vars.size() == vals.size();
  }

  /******************************************************************************
  * ENVIRONMENT DATA STRUCTURE IMPLEMENTATION
  ******************************************************************************/

  // -- ENVIRONMENTAL EXTENSION
  env_type extend_environment(frame_vars&& vars, frame_vals& vals, env_type& base_env, 
                                                             const sym_type& name = ""){
    // If valid extension, return environment w/ a new frame prepended
    if(confirm_valid_environment_extension(vars,vals,name)) {
      env_type extended_env(make_env());
      frame_objs var_val_map;
      for(size_type i = 0, n = vars.size(); i < n; ++i)
        var_val_map[vars[i]] = frame_val_ptr(vals[i]);
      extended_env->push_back(make_frame(frame_t(var_val_map,frame_macs())));
      extended_env->insert(extended_env->end(), base_env->begin(), base_env->end());
      return extended_env;
    // Invalid extension
    } else if(vars.size() < vals.size()) {
      THROW_ERR("Too many arguments supplied! -- EXTEND_ENVIRONMENT" 
        << improper_call_alert(name,vals,vars));
    } else {
      THROW_ERR("Too few arguments supplied! -- EXTEND_ENVIRONMENT" 
        << improper_call_alert(name,vals,vars));
    }
  }

  // R-value overload is _ONLY_ to launch the global environment
  env_type extend_environment(frame_vars&& vars, frame_vals&& vals, env_type& base_env){
    return extend_environment(std::move(vars),vals,base_env,"");
  }


  // -- VARIABLE LOOKUP
  frame_val lookup_variable_value(const frame_var& var, env_type& env) {
    // Search Each Environment Frame
    for(size_type i = 0, total_frames = env->size(); i < total_frames; ++i) {
      // Get Variables & Values Map of the current frame
      auto& var_val_map = frame_objects(*env->operator[](i));
      auto var_pos = var_val_map.find(var);
      if(var_pos != var_val_map.end()) {
        if(var_pos->second->is_type(types::undefined))
          THROW_ERR("Unassigned Variable -- " << var);
        return *var_pos->second;
      }
    }
    THROW_ERR("Variable " << var << " is not bound!");
  }


  // -- NAME MANGLING
  // defining/set!ing an anonymous lambda generates a named procedure
  void mangle_bound_anonymous_procedure_name(const frame_var& var, frame_val& val)noexcept{
    if(val.is_type(types::exp) && is_tagged_list(val.exp,symconst::procedure) && val.exp[5].sym.empty())
      val.exp[5].sym = ' ' + var;
  }


  // -- VARIABLE SETTING: (set! <var> <val>)
  bool is_single_self_evaluating_object_in_list(frame_val& val)noexcept{
    return val.is_type(types::exp) && val.exp.size()==1;
  }
  void set_variable_value(const frame_var& var, frame_val&& val, env_type& env) {
    if(is_single_self_evaluating_object_in_list(val)) val = val.exp[0];
    // Search Each Environment Frame
    for(size_type i = 0, total_frames = env->size(); i < total_frames; ++i) {
      // Get Variables & Values Map of the current frame
      auto& var_val_map = frame_objects(*env->operator[](i));
      auto var_pos = var_val_map.find(var);
      if(var_pos != var_val_map.end()) {
        mangle_bound_anonymous_procedure_name(var,val);
        *var_pos->second = val; // var found, change val
        return;
      }
    }
    scm_list invalid_set_call(3);
    invalid_set_call[0] = symconst::set;
    invalid_set_call[1] = var, invalid_set_call[2] = val;
    THROW_ERR("Variable "<<var<<" is not bound!"<<EXP_ERR(invalid_set_call));
  }


  // -- VARIABLE DEFINITION: (define <var> <val>)
  void define_variable(const frame_var& var, frame_val val, env_type& env)noexcept{
    if(env->empty()) // add an empty frame to if given an empty environment
      env->push_back(make_frame(frame_t())); 
    if(is_single_self_evaluating_object_in_list(val)) val = val.exp[0];
    // Get Variables & Values Map of the foremost frame
    auto& var_val_map = frame_objects(*env->operator[](0));
    // Search Variable Bindings List of the Frame, Assign New Val if Found
    mangle_bound_anonymous_procedure_name(var,val);
    auto var_pos = var_val_map.find(var);
    if(var_pos != var_val_map.end())
      *var_pos->second = val; // var found, change val
    else
      var_val_map[var] = frame_val_ptr(val);
  }


  // -- MACRO DEFINITION: (define-syntax <label> <syntax-rules>)
  void define_syntax_extension(const frame_mac& mac, env_type& env)noexcept{
    if(env->empty()) // add an empty frame to if given an empty environment
      env->push_back(make_frame(frame_t())); 
    // macro list of the first frame
    auto& macs = frame_macros(*env->operator[](0)); 
    // if syntax is already defined, redefine it in the foremost frame
    for(auto& m : macs)
      if(m.label == mac.label) {
        m = mac;
        return;
      }
    // define the new syntax in the foremost frame & register its label
    macs.push_back(mac);
  }

  /******************************************************************************
  * REPRESENTING CONDITIONALS: (if <predicate> <consequent> <alternative>)
  ******************************************************************************/

  // -- IDENTIFICATION, GETTERS, & CONSTRUCTION
  bool     is_if(const scm_list& exp)  noexcept{return is_tagged_list(exp,symconst::if_t);}
  scm_list if_predicate(scm_list& exp) noexcept{return scm_list_cast(exp[1]);}
  scm_list if_consequent(scm_list& exp)noexcept{return scm_list_cast(exp[2]);}

  scm_list if_alternative(scm_list& exp)noexcept{
    if(exp.size() == 4) return scm_list_cast(exp[3]); // if has an <alternative>
    return G::VOID_DATA_EXPRESSION; // w/o <alternative> return VOID
  }

  scm_list make_if(const scm_list& predicate, const scm_list& consequent, 
                                              const scm_list& alternative)noexcept{
    scm_list if_exp(4);
    if_exp[0] = symconst::if_t, if_exp[1] = predicate;
    if_exp[2] = consequent, if_exp[3] = alternative;
    return if_exp;
  }


  // -- PREDICATE TESTING
  bool is_true(const scm_list& exp)noexcept{ // true is not false
    return !exp[0].is_type(types::bol) || exp[0].bol.val;
  }
  bool is_false(const scm_list& exp)noexcept{ // false is false
    return exp[0].is_type(types::bol) && !exp[0].bol.val;
  }


  // -- ANALYSIS
  void confirm_valid_if(const scm_list& exp) {
    if(exp.size() < 3) 
      THROW_ERR("IF didn't receive enough args:"
        "\n     (if <predicate> <consequent> <optional-alternative>)"
        << EXP_ERR(exp));
    if(exp.size() > 4) 
      THROW_ERR("IF received too many args:"
        "\n     (if <predicate> <consequent> <optional-alternative>)"
        << EXP_ERR(exp));
  }

  // Returns lambda so that if true, only eval consequent: 
  //   else, only eval alternative
  exe_type analyze_if(scm_list& exp,const bool tail_call=false,const bool cps_block=false) { 
    confirm_valid_if(exp);
    auto pproc = scm_analyze(if_predicate(exp),false,cps_block);
    auto cproc = scm_analyze(if_consequent(exp),tail_call,cps_block);
    auto aproc = scm_analyze(if_alternative(exp),tail_call,cps_block);
    return [pproc=std::move(pproc),cproc=std::move(cproc),
            aproc=std::move(aproc)](env_type& env){
      if(is_true(pproc(env))) 
        return cproc(env);
      return aproc(env);
    };
  }

  /******************************************************************************
  * REPRESENTING COMPOUND BOOLEANS: (and <val1> ...) (or <val1> ...)
  ******************************************************************************/

  bool is_and(const scm_list& exp)noexcept{return is_tagged_list(exp,symconst::and_t);}
  bool is_or(const scm_list& exp)noexcept{return is_tagged_list(exp,symconst::or_t);}


  // -- GENERATING CPS-STYLE VALUE EVALUATION (for unary 'and 'or in scm->cps blocks)
  exe_type generate_unary_cps_value_expansion(const data& d, const bool tail_call){
    scm_list cps_val(3);
    cps_val[0] = symconst::lambda;
    cps_val[1] = scm_list(1,generate_unique_cps_hash()); // "k"
    cps_val[2] = scm_list(2);
    cps_val[2].exp[0] = cps_val[1].exp[0];
    cps_val[2].exp[1] = d;
    return scm_analyze(std::move(cps_val),tail_call,true);
  }


  // GENERATING and AND or IN TERMS OF if
  void recursive_and_expansion_constructor(const scm_node& start,const scm_node& end,scm_list& expanded_and)noexcept{
    expanded_and[0] = symconst::if_t;
    expanded_and[1] = *start;
    expanded_and[3] = G::FALSE_DATA_BOOLEAN;
    if(start+2 != end) {
      expanded_and[2] = scm_list(4);
      recursive_and_expansion_constructor(start+1,end,expanded_and[2].exp);
    } else {
      expanded_and[2] = *(start+1);
    }
  }

  void recursive_or_expansion_constructor(const scm_node& start,const scm_node& end,scm_list& expanded_and)noexcept{
    expanded_and[1] = *start;
    expanded_and[0] = scm_list(3);
    expanded_and[0].exp[0] = symconst::lambda;
    expanded_and[0].exp[1] = scm_list(1,"test");
    expanded_and[0].exp[2] = scm_list(4);
    expanded_and[0].exp[2].exp[0] = symconst::if_t;
    expanded_and[0].exp[2].exp[1] = "test";
    expanded_and[0].exp[2].exp[2] = "test";
    if(start+2 != end) {
      expanded_and[0].exp[2].exp[3] = scm_list(2);
      recursive_or_expansion_constructor(start+1,end,expanded_and[0].exp[2].exp[3].exp);
    } else {
      expanded_and[0].exp[2].exp[3] = *(start+1);
    }
  }


  // -- AND: (and <condition1> <condition2> ...)
  // Returns an exec proc to confirm whether all exp's are true
  exe_type analyze_and(scm_list& and_exp,const bool tail_call=false,const bool cps_block=false) {
    const size_type n = and_exp.size();
    // (and) = #t
    if(n == 1 || data_is_the_SENTINEL_VAL(and_exp[1])) {
      if(cps_block) return generate_unary_cps_value_expansion(G::TRUE_DATA_BOOLEAN,tail_call);
      return [](env_type&){return scm_list(1,G::TRUE_DATA_BOOLEAN);};
    }
    // (and <obj>) = <obj>
    if(n == 2) {
      if(cps_block) return generate_unary_cps_value_expansion(and_exp[1],tail_call);
      return [res=and_exp[1]](env_type&){return scm_list(1,res);};
    }
    // (and <o1> <o2> ...) = (if <o1> (and <o2> ...) #f)
    data expanded_and(scm_list(4));
    recursive_and_expansion_constructor(and_exp.begin()+1,and_exp.end(),expanded_and.exp);
    if(cps_block) return scm_analyze(generate_fundamental_form_cps(expanded_and),tail_call,true);
    return scm_analyze(std::move(expanded_and.exp),tail_call,false);
  }


  // -- OR: (or <condition1> <condition2> ...)
  // Returns an exec proc to confirm whether >= 1 exp is true
  exe_type analyze_or(scm_list& or_exp,const bool tail_call=false,const bool cps_block=false) {
    const size_type n = or_exp.size();
    // (or) = #f
    if(n == 1 || data_is_the_SENTINEL_VAL(or_exp[1])) {
      if(cps_block) return generate_unary_cps_value_expansion(G::FALSE_DATA_BOOLEAN,tail_call);
      return [](env_type&){return scm_list(1,G::FALSE_DATA_BOOLEAN);};
    }
    // (or <obj>) = <obj>
    if(n == 2) {
      if(cps_block) return generate_unary_cps_value_expansion(or_exp[1],tail_call);
      return [res=or_exp[1]](env_type&){return scm_list(1,res);};
    }
    // (or <o1> <o2> ...) = ((lambda (test) (if test test (or <o2> ...))) <o1>)
    data expanded_or(scm_list(2));
    recursive_or_expansion_constructor(or_exp.begin()+1,or_exp.end(),expanded_or.exp);
    if(cps_block) return scm_analyze(generate_fundamental_form_cps(expanded_or),tail_call,true);
    return scm_analyze(std::move(expanded_or.exp),tail_call,false);
  }

  /******************************************************************************
  * REPRESENTING SEQUENCES: (begin <body>)
  ******************************************************************************/

  bool is_begin(const scm_list& exp)   noexcept{return is_tagged_list(exp,symconst::begin);}
  scm_list begin_actions(scm_list& exp)noexcept{return scm_list(exp.begin()+1, exp.end());}

  // Analyzes each expression, then returns an exec proc which 
  //   sequentially invokes each expression's exec proc
  exe_type analyze_sequence(scm_list&& exps,const bool tail_call=false,const bool cps_block=false){ // used for 'begin' & lambda bodies
    if(exps.empty() || (exps.size()==1 && data_is_the_SENTINEL_VAL(exps[0])))
      return [](env_type&){return G::VOID_DATA_EXPRESSION;}; // void data
    const size_type n = exps.size();
    std::vector<exe_type> sequence_exe_procs(exps.size());
    // Analyze each expression
    for(size_type i = 0, n = exps.size(); i < n; ++i)
      sequence_exe_procs[i] = scm_analyze(scm_list_cast(exps[i]),(i+1==n)&&tail_call,cps_block);
    // Return a lambda sequentially invoking each exec procedure
    return [n=std::move(n),sequence_exe_procs=std::move(sequence_exe_procs)]
    (env_type& env){
      for(size_type i = 0; i+1 < n; ++i)
        sequence_exe_procs[i](env);
      return sequence_exe_procs[n-1](env);
    };
  }

  // Convert a sequence into a single expression via 'begin
  scm_list convert_sequence_exp(scm_list seq)noexcept{ 
    scm_list begin_sequence(seq.size()+1); 
    begin_sequence[0] = symconst::begin;
    std::move(seq.begin(),seq.end(),begin_sequence.begin()+1);
    return begin_sequence;
  }

  /******************************************************************************
  * REPRESENTING ASSIGNMENT: (set! <var> <val>)
  ******************************************************************************/

  bool is_assignment(const scm_list& exp)       noexcept{return is_tagged_list(exp,symconst::set);}
  frame_var& assignment_variable(scm_list& exp) noexcept{return exp[1].sym;}
  scm_list assignment_value(const scm_list& exp)noexcept{return scm_list_cast(exp[2]);}

  // Analyzes value being assigned, & returns an execution procedure 
  //   to install it as the variable in the designated env
  exe_type analyze_assignment(scm_list& exp,const bool cps_block=false) { 
    confirm_valid_assignment(exp);
    auto& var       = assignment_variable(exp);
    auto value_proc = scm_analyze(assignment_value(exp),false,cps_block);
    return [var=std::move(var),value_proc=std::move(value_proc)](env_type& env){
      set_variable_value(var,data_cast(value_proc(env)),env);
      return G::VOID_DATA_EXPRESSION; // return is undefined
    };
  }

  /******************************************************************************
  * REPRESENTING DEFINITION: (define <var> <val>)
  ******************************************************************************/

  bool is_definition(const scm_list& exp)noexcept{return is_tagged_list(exp,symconst::define);}
  scm_list make_lambda(scm_list parameters, // Lambda ctor
                       scm_list body)noexcept; 

  frame_var& definition_variable(scm_list& exp)noexcept{
    // if defining a variable, else defining a procedure
    if(exp[1].is_type(types::sym)) return exp[1].sym; 
    return exp[1].exp[0].sym;
  }

  scm_list definition_value(scm_list& exp)noexcept{
    // if defining a variable, else defining a procedure
    if(exp[1].is_type(types::sym)) return scm_list_cast(exp[2]); 
    scm_list args(exp[1].exp.begin()+1,exp[1].exp.end());
    scm_list body(exp.begin()+2,exp.end());
    return make_lambda(args,body);
  }

  // Analyzes value being defined, & returns an execution procedure 
  //   to install it as the variable in the designated env
  exe_type analyze_definition(scm_list& exp,const bool cps_block=false) { 
    confirm_valid_definition(exp);
    auto& var       = definition_variable(exp);
    auto value_proc = scm_analyze(definition_value(exp),false,cps_block);
    return [var=std::move(var),value_proc=std::move(value_proc)](env_type& env){
      define_variable(var,data_cast(value_proc(env)),env);
      return G::VOID_DATA_EXPRESSION; // return is undefined
    };
  }

  /******************************************************************************
  * REPRESENTING PROMISES: (delay <expression>)
  ******************************************************************************/

  bool is_delay(const scm_list& exp)noexcept{return is_tagged_list(exp,symconst::delay);}

  // 'Delay' data struct consists of the delayed expression, its environment, 
  //   whether its already been forced, and the result of forcing it (if forced)
  scm_list make_delay(const scm_list& exp, env_type& env)noexcept{
    scm_list del(2); del[0] = symconst::delay, del[1] = make_del(exp,env);
    return del;
  }

  // Extracts the delayed expression and returns an exec proc ctor'ing a promise
  exe_type analyze_delay(scm_list& exp,const bool cps_block=false) {
    if(exp.size() != 2 || data_is_the_SENTINEL_VAL(exp[1])) {
      exp[0].sym += ' '; // add space to tag, avoiding #<delay> degradation
      auto delay_call = cio_expr_str<&data::write>(exp);
      delay_call.erase(6,1);   // erase appended space
      THROW_ERR("'delay expects 1 argument: (delay <delay-expression>)" 
        << EXP_ERR(delay_call));
    }
    if(!cps_block) {
      auto delay_list = scm_list_cast(exp[1]);
      return [delay_list=std::move(delay_list)](env_type& env){
        return make_delay(delay_list,env);
      };
    }
    // Bind delayed CPS expressions to always have 'id as the topmost continuation,
    //   since FORCE is defined outside a scm->cps block, would have its arg bound 
    //   to such regardless if implemented directly in heist scheme as well.
    scm_list delay_list(2); // cast to & from scm_list to apply the NIL-ARG as needed
    delay_list[0] = generate_fundamental_form_cps(data_cast(scm_list_cast(exp[1])));
    delay_list[1] = "id";
    return [delay_list=std::move(delay_list)](env_type& env){
      return make_delay(delay_list,env);
    };
  }

  /******************************************************************************
  * REPRESENTING STREAMS: (scons <stream-car> <stream-cdr>)
  ******************************************************************************/

  // -- scons: (scons <arg1> <arg2>) = (cons (delay <arg1>) (delay <arg2>))
  bool is_scons(const scm_list& exp)noexcept{return is_tagged_list(exp,symconst::scons);}

  exe_type analyze_scons(scm_list& exp,const bool cps_block=false) {
    if(exp.size() != 3)
      THROW_ERR("'scons expects 2 arguments: (scons <scar> <scdr>)"<<EXP_ERR(exp));
    scm_list scons_exp(3); 
    scons_exp[0] = symconst::cons;
    scons_exp[1] = scm_list(2);
    scons_exp[1].exp[0] = symconst::delay;
    scons_exp[1].exp[1] = exp[1];
    scons_exp[2] = scm_list(2);
    scons_exp[2].exp[0] = symconst::delay;
    scons_exp[2].exp[1] = exp[2];
    return scm_analyze(std::move(scons_exp),false,cps_block);
  }


  // -- stream: (stream <a> <b> ...) = (scons <a> (scons <b> ...))
  bool is_stream(const scm_list& exp)noexcept{return is_tagged_list(exp,symconst::stream);}

  void recursively_expand_stream_into_delayed_cons(const scm_node& start,const scm_node& end,scm_list& expanded)noexcept{
    expanded[0] = symconst::cons;
    expanded[1] = scm_list(2);
    expanded[1].exp[0] = symconst::delay;
    expanded[1].exp[1] = *start;
    if(start+1 != end) {
      expanded[2] = scm_list(3);
      recursively_expand_stream_into_delayed_cons(start+1,end,expanded[2].exp);
    } else {
      expanded[2] = symconst::emptylist;
    }
  }

  exe_type analyze_stream(scm_list& exp,const bool cps_block=false)noexcept{
    if(exp.size() == 1 || data_is_the_SENTINEL_VAL(exp[1])) {
      if(cps_block) return generate_unary_cps_value_expansion(symconst::emptylist,false);
      return [](env_type&){return G::EMPTY_LIST_EXPRESSION;};
    }
    if(!cps_block) // no need to expand AST _unless_ in a cps block (direct stream ctor)
      return [exp=std::move(exp)](env_type& env) mutable {
        return scm_list(1, primitive_STREAM_to_SCONS_constructor(exp.begin()+1,exp.end(),env));
      };
    scm_list expanded_stream(3);
    recursively_expand_stream_into_delayed_cons(exp.begin()+1,exp.end(),expanded_stream);
    return scm_analyze(std::move(expanded_stream),false,cps_block);    
  }

  /******************************************************************************
  * REPRESENTING QUOTATION: (quote <expression>)
  ******************************************************************************/

  bool is_quoted(const scm_list& exp)noexcept{return is_tagged_list(exp, symconst::quote);}

  // Quoting a vector literal is a special case of quotation
  bool is_vector_literal(const scm_list& exp)noexcept{
    return is_tagged_list(exp,symconst::vec_literal);
  }

  // Confirm whether quoting a vector literal
  bool quoting_a_vector_literal(const scm_list& exp)noexcept{
    return exp[1].is_type(types::exp) && !exp[1].exp.empty() && 
           is_vector_literal(exp[1].exp);
  }

  // Returns whether data is the (.) symbol
  bool data_is_dot_operator(const data& d)noexcept{
    return d.is_type(types::sym) && d.sym == symconst::period;
  }

  // Returns quoted data's contents
  data text_of_quotation(scm_list& exp)noexcept{
    if(!exp[1].is_type(types::sym)) return exp[1];
    if(exp[1].sym==symconst::false_t || exp[1].sym==symconst::true_t)
      return boolean(exp[1].sym==symconst::true_t);
    return convert_string_to_symbol(exp[1].sym);
  }


  // Returns whether the quoted exp is a 'cons' via the (.) operator.
  //   If not, exp is a list & this returns false.
  //   => Throws error if more than 1 (.) found (improper use)
  //   => If is a valid 'cons' quote, also rm's the (.)
  bool is_quoted_cons(scm_list& exp, const sym_type& quote_name) {
    // Confirm (.) does not terminate the list
    if(!exp.empty() && data_is_dot_operator(*exp.rbegin()))
      THROW_ERR("Unexpected dot (.) terminated the quoted list! -- ANALYZE_QUOTED"
        << FCN_ERR(quote_name,exp));
    // Iff exp begins w/ (.) & has a length of 2, it may be a valid instance of 
    //   quoting a variadic lambda [ ie '(lambda (. l) l) ] -- thus such is valid
    if(exp.size()==2 && data_is_dot_operator(exp[0]) && !data_is_dot_operator(exp[1]))
      return false;
    // Confirm no (.) prior the penultimate item in the list
    for(size_type i = 0, n = exp.size(); i+2 < n; ++i)
      if(data_is_dot_operator(exp[i]))
        THROW_ERR("Unexpected dot (.) at position #"<<i+1<<" in quotation! -- ANALYZE_QUOTED"
          << FCN_ERR(quote_name,exp));
    // Determine whether at a cons or a list
    if(exp.size() > 2 && data_is_dot_operator(*(exp.end()-2))) {
      exp.erase(exp.end()-2); // rm (.)
      return true; // cons
    }
    return false;  // list
  }


  // Analyzes the quote's vector literal & returns its execution procedure
  exe_type analyze_quoted_vector_literal(scm_list& exp) {
    scm_list args(exp.begin()+1,exp.end());
    if(is_quoted_cons(args, symconst::quote))
      THROW_ERR("'vector-literal had an unexpected dot (.)! -- ANALYZE_QUOTED_VECTOR_LITERAL"
        << EXP_ERR(exp));
    // return an empty vector if given no args
    if(no_args_given(args)) 
      return [](env_type&){return scm_list(1,make_vec(scm_list()));};
    // quote each item in the vector
    scm_list vector_literal(args.size()+1);
    vector_literal[0] = symconst::vector;
    for(size_type i = 0, n = args.size(); i < n; ++i) {
      vector_literal[i+1] = scm_list(2);
      vector_literal[i+1].exp[0] = symconst::quote;
      vector_literal[i+1].exp[1] = args[i];
    }
    // return analyzed vector
    return scm_analyze(std::move(vector_literal));
  }


  // Analyzes the quote's text & returns an execution procedure for such
  exe_type analyze_quoted(scm_list& exp) {
    if(exp.size() != 2 || data_is_the_SENTINEL_VAL(exp[1])) 
      THROW_ERR("'quote form expects one argument: (quote <quoted-data>)!"<<EXP_ERR(exp));
    
    // Quote vector literals as needed
    if(quoting_a_vector_literal(exp))
      return analyze_quoted_vector_literal(exp[1].exp);
    
    // Get quoted data
    auto quoted_data = text_of_quotation(exp);
    
    // If quoted data is atomic, return as-is
    if(!quoted_data.is_type(types::exp))
      return [quoted_data=std::move(quoted_data)](env_type&){
        return scm_list(1, quoted_data);
      };
    
    // If quoting an empty expression, return the empty list
    if(quoted_data.exp.empty())
      return [](env_type&){return G::EMPTY_LIST_EXPRESSION;};
    
    // Confirm whether appending last item. 
    //   => NOTE: also rm's (.) if so, hence this must be done 
    //            PRIOR quoting each item
    bool append_last_item = is_quoted_cons(quoted_data.exp,symconst::quote);

    // Since quoting an expression, expand such into a list of quoted data
    scm_list quote_val(quoted_data.exp.size()+1);
    quote_val[0] = symconst::list;
    
    // Wrap 'quote' around each item in the list
    for(size_type i = 0, n = quoted_data.exp.size(); i < n; ++i){
      quote_val[i+1] = scm_list(2);
      quote_val[i+1].exp[0] = symconst::quote;
      quote_val[i+1].exp[1] = quoted_data.exp[i];
    }
    
    // Unpack the last item in the list and append it if quoting 
    //   a non-null-terminated list
    if(append_last_item) {
      auto last_item = *quote_val.rbegin();
      quote_val.pop_back();
      scm_list append_exp(3); 
      append_exp[0] = symconst::append;
      append_exp[1] = quote_val, append_exp[2] = last_item;
      quote_val = std::move(append_exp);
    }
    
    // Return the analyzed quoted list expression
    return scm_analyze(std::move(quote_val));
  }

  /******************************************************************************
  * REPRESENTING SELF-EVALUATING EXPRESSIONS & VARIABLES
  ******************************************************************************/

  bool is_self_evaluating(const scm_list& exp)noexcept{
    return exp.size() == 1 && 
           (exp[0].is_type(types::num) || exp[0].is_type(types::str) || 
            exp[0].is_type(types::chr) || exp[0].is_type(types::par) || 
            exp[0].is_type(types::vec) || exp[0].is_type(types::bol) ||
            exp[0].is_type(types::syn) || exp[0].is_type(types::dne) || 
            exp[0].is_type(types::undefined));
  }

  bool is_variable(const scm_list& exp)noexcept{return exp[0].is_type(types::sym);}

  /******************************************************************************
  * REPRESENTING PROCEDURES
  ******************************************************************************/

  // PROCEDURE CONSTRUCTION
  scm_list make_procedure(const scm_list& parameters, const exe_type& body_proc,
                          env_type& env, const frame_var& name)noexcept{
    scm_list procedure_exp(7);
    procedure_exp[0] = symconst::procedure, procedure_exp[1] = parameters;
    procedure_exp[2] = body_proc,           procedure_exp[3] = env;
    procedure_exp[4] = make_cal(0),         procedure_exp[5] = name;
    procedure_exp[6] = boolean(false); // inline-invocation? (SEE cps-load & cps-eval)
    return procedure_exp;
  }

  bool is_compound_procedure(const scm_list& p)    noexcept{return is_tagged_list(p,symconst::procedure);}
  exe_type& procedure_body(scm_list& p)            noexcept{return p[2].exe;}
  env_type& procedure_environment(scm_list& p)     noexcept{return p[3].env;}
  size_type& procedure_recursive_depth(scm_list& p)noexcept{return *p[4].cal;}
  bool procedure_inlines_call(scm_list& p)         noexcept{return p[6].bol.val;}

  frame_var procedure_name(const scm_list& p)noexcept{
    if(is_compound_procedure(p)) // compound procedure name
      return p[5].sym;
    return ' ' + p[2].sym; // primitive procedure name
  }

  frame_var printable_procedure_name(const scm_list& p)noexcept{
    auto name = procedure_name(p);
    if(name.empty()) return "#<procedure>"; // lambda
    name.erase(0,1); // rm prefixing ' '
    return name;
  }

  frame_vars procedure_parameters(scm_list& p)noexcept{
    frame_vars var_names(p[1].exp.size());
    for(size_type i = 0, n = p[1].exp.size(); i < n; ++i)
      var_names[i] = p[1].exp[i].sym;
    return var_names;
  }


  // -- LAMBDAS: (lambda (<parameters>) <body>)
  bool     is_lambda(const scm_list& exp)  noexcept{return is_tagged_list(exp,symconst::lambda);}
  scm_list lambda_parameters(scm_list& exp)noexcept{return exp[1].exp;}
  scm_list lambda_body(scm_list& exp)      noexcept{return scm_list(exp.begin()+2,exp.end());}

  // Ctor for lambdas
  scm_list make_lambda(scm_list parameters,scm_list body)noexcept{
    scm_list new_lambda(body.size()+2); 
    new_lambda[0] = symconst::lambda, new_lambda[1] = std::move(parameters);
    if(new_lambda[1].exp.empty()) // add the sentinel arg as needed
      new_lambda[1].exp.push_back(symconst::sentinel_arg);
    std::move(body.begin(), body.end(), new_lambda.begin()+2);
    return new_lambda;
  }

  // Returns an exec proc to mk a lambda w/ the analyzed parameter list & body
  exe_type analyze_lambda(scm_list& exp,const bool cps_block=false) {
    confirm_valid_lambda(exp);
    auto vars = lambda_parameters(exp);
    confirm_valid_procedure_parameters(vars,exp);                      // validate parameters
    if(vars.empty()) vars.push_back(symconst::sentinel_arg);           // add sentinel-arg
    auto body_proc = analyze_sequence(lambda_body(exp),true,cps_block);// analyze body syntax
    return [vars=std::move(vars),body_proc=std::move(body_proc)]
      (env_type& env){
        return make_procedure(vars, body_proc, env, ""); // empty "" name by default (anon proc)
      };
  }


  // -- PROCEDURAL APPLICATION
  bool is_application(const scm_list& exp) noexcept{return exp.size()>1;}
  scm_list operator_of(const scm_list& exp)noexcept{return scm_list_cast(exp[0]);}
  scm_list operands(const scm_list& exp)   noexcept{return scm_list(exp.begin()+1, exp.end());}

  /******************************************************************************
  * DERIVING COND: (cond <clause1> ... <clauseN>)
  ******************************************************************************/

  bool is_cond(const scm_list& exp)noexcept{return is_tagged_list(exp,symconst::cond);}


  // -- CLAUSE VALIDATION
  void confirm_valid_clause(const scm_node& current_clause, const scm_list& exp){
    if(!current_clause->is_type(types::exp) || 
      current_clause->exp.empty()     || 
      data_is_the_SENTINEL_VAL(current_clause->exp[0]))
      THROW_ERR("Invalid COND clause [ " << *current_clause << " ], expects:"
        "\n     (cond <clause1> <clause2> ...)"
        "\n     <clause> = (<condition> <consequent>)  -- CONVERT_COND_IF"
        << EXP_ERR(exp));
  }


  // -- CLAUSE GETTERS
  scm_list cond_predicate(scm_node& clause)noexcept{
    if(clause->exp[0].is_type(types::exp))
      return clause->exp[0].exp; // expression
    scm_list cond_pred(2); 
    cond_pred[0] = symconst::and_t, cond_pred[1] = clause->exp[0];
    return cond_pred; // variable as an expression
  }

  scm_list cond_actions(scm_node& clause)noexcept{
    return convert_sequence_exp(scm_list(
            clause->exp.begin()+1,clause->exp.end()));
  }


  // -- CLAUSE ANALYSIS
  bool is_cond_else_clause(const scm_node& clause)noexcept{
    return is_tagged_list(clause->exp,symconst::else_t);
  }

  bool is_cond_arrow_clause(const scm_node& clause, const scm_list& exp) {
    const bool has_cond_arrow_notation = clause->exp.size() > 1 && 
                                         clause->exp[1].is_type(types::sym) && 
                                         clause->exp[1].sym == "=>";
    if(has_cond_arrow_notation && clause->exp.size() != 3)
      THROW_ERR("COND Clause '=> Notation didn't receive 3 args:"
        "\n     (cond <clause1> <clause2> ...)"
        "\n     (<condition> => <procedure>)  -- CONVERT_COND_IF"
        << EXP_ERR(exp));
    return has_cond_arrow_notation;
  }

  data cond_arrow_procedure(scm_node& clause)noexcept{
    return clause->exp[2];
  }


  // -- CLAUSE EXPANSION
  // Each clause consists of a <condition> & a <body>
  scm_list expand_clauses(scm_node& current_clause, const scm_node& end_clauses, const scm_list& exp) {
    // no else clause -- return is now implementation-dependant, I've chosen VOID
    if(current_clause == end_clauses) {
      scm_list void_exp(2); 
      void_exp[0] = symconst::and_t, void_exp[1] = G::VOID_DATA_OBJECT;
      return void_exp;
    }
    confirm_valid_clause(current_clause,exp);

    // Set <test> as <consequent> if only given a <test> in the clause
    if(current_clause->exp.size() == 1)
      current_clause->exp.push_back(current_clause->exp[0]);

    // Convert COND into a series of cascading 'if's
    scm_node rest_clauses = current_clause+1;
    if(is_cond_else_clause(current_clause)) {
      if(rest_clauses == end_clauses)
        return cond_actions(current_clause);
      else
        THROW_ERR("ELSE clause isn't last -- CONVERT_COND_IF\n     Total clauses after ELSE: " 
          << std::distance(current_clause,end_clauses)-1
          << "\n     (cond <clause1> <clause2> ...)"
          << EXP_ERR(exp));
    } else if(is_cond_arrow_clause(current_clause,exp)) {
      scm_list cond_arrow_application(2);
      cond_arrow_application[0] = cond_arrow_procedure(current_clause);
      cond_arrow_application[1] = cond_predicate(current_clause);
      return make_if(cond_predicate(current_clause), 
                     cond_arrow_application,
                     expand_clauses(rest_clauses,end_clauses,exp));
    } else {
      return make_if(cond_predicate(current_clause),
                     cond_actions(current_clause),
                     expand_clauses(rest_clauses,end_clauses,exp));
    }
  }


  // Expands 'cond into a series of nested 'if
  scm_list convert_cond_if(scm_list& exp,const bool cps_block=false) {
    if(exp.size() < 2 || data_is_the_SENTINEL_VAL(exp[1]))
      THROW_ERR("Invalid COND expression, NO CLAUSES GIVEN:"
        "\n     (cond <clause1> <clause2> ...)"
        "\n     <clause> = (<condition> <consequent>)"
        << EXP_ERR(exp));
    scm_node start_of_clauses = exp.begin()+1;
    const scm_node end_of_clauses = exp.end();
    if(!cps_block)
      return expand_clauses(start_of_clauses,end_of_clauses,exp);
    return generate_fundamental_form_cps(expand_clauses(start_of_clauses,end_of_clauses,exp));
  }

  /******************************************************************************
  * DERIVING LET: (let ((<var1> <val1>) ... (<varN> <valN>)) <body>)
  ******************************************************************************/

  // (let ((<var1> <val1>) ... (<varN> <valN>)) <body>) =>
  // ((lambda (<var1> ... <varN>) <body>) <val1> ... <valN>)

  bool     is_let(const scm_list& exp)        noexcept{return is_tagged_list(exp,symconst::let);}
  bool     is_named_let(const scm_list& exp)  noexcept{return exp[1].is_type(types::sym);}
  sym_type named_let_name(const scm_list& exp)noexcept{return exp[1].sym;}
  scm_list let_body(scm_list& exp)            noexcept{return scm_list(exp.begin()+2,exp.end());}
  scm_list named_let_body(scm_list& exp)      noexcept{return scm_list(exp.begin()+3,exp.end());}


  // PARAMETERS = (<varN> <valN>) OF 'let
  void validate_let_parameters(const data& params, const scm_list& exp) {
    static constexpr const char * const format = 
      "\n     (let (<var-bindings>) <body>)"
      "\n     <var-binding> = (<name> <value>)";
    if(!params.is_type(types::exp))
      THROW_ERR("Invalid LET expression, parameters "<<PROFILE(params)<<
        " aren't a list of bindings:" << format << EXP_ERR(exp));
    for(size_type i = 0, n = params.exp.size(); i < n; ++i) {
      if(!params.exp[i].is_type(types::exp))
        THROW_ERR("Invalid LET expression, binding #"<<i+1<<PROFILE(params.exp[i])<<
          " isn't binding list:" << format << EXP_ERR(exp));
      if(params.exp[i].exp.size() != 2)
        THROW_ERR("Invalid LET expression, binding #"<<i+1<<PROFILE(params.exp[i])<<
          " isn't binding list of length 2:" << format << EXP_ERR(exp));
    }
  }

  scm_list let_parameters(scm_list& exp) {
    validate_let_parameters(exp[1],exp); 
    return exp[1].exp;
  }

  scm_list named_let_parameters(scm_list& exp) {
    validate_let_parameters(exp[2],exp); 
    return exp[2].exp;
  }

  // Return list of parameter <var> names
  scm_list let_variables(const scm_list& parameters)noexcept{
    scm_list vars(parameters.size()); // return var names
    for(size_type i = 0, n = parameters.size(); i < n; ++i)
      vars[i] = parameters[i].exp[0];
    return vars;
  }

  // Return list of parameter <val> values
  scm_list let_expressions(const scm_list& parameters)noexcept{
    // returns variable values (ie their assinged expressions) of parameters
    scm_list exps(parameters.size());
    for(size_type i = 0, n = parameters.size(); i < n; ++i)
      exps[i] = parameters[i].exp[1];
    return exps;
  }


  // Convert 'let to a 'lambda
  scm_list convert_let_combination(scm_list exp,const bool cps_block=false) {
    if(exp.size() < 3)
      THROW_ERR("Invalid LET expression, didn't received 2 args:"
        "\n     (let (<var-bindings>) <body>)"
        "\n     <var-binding> = (<name> <value>)" << EXP_ERR(exp));
    const bool named = is_named_let(exp);
    if(named && exp.size() < 4)
      THROW_ERR("Invalid NAMED LET expression, didn't received 3 args:"
        "\n     (let <name> (<var-bindings>) <body>)"
        "\n     <var-binding> = (<name> <value>)" << EXP_ERR(exp));

    // list of var-val pairs
    scm_list params = (named ? named_let_parameters : let_parameters)(exp);

    // add the sentinel arg as a var name, if 'let' was given no parameters
    if(params.empty()) {
      params.push_back(scm_list(2)); // () => ((SENTINTEL_ARG, SENTINTEL_VAL))
      params[0].exp[0] = symconst::sentinel_arg;
      params[0].exp[1] = scm_list(2);
      params[0].exp[1].exp[0] = symconst::quote;
      params[0].exp[1].exp[1] = symconst::sentinel_arg;
    }

    // retreive let expressions & body
    auto exprs             = let_expressions(params);
    auto let_lambda_object = data(make_lambda(let_variables(params),(named ? named_let_body : let_body)(exp)));

    // convert let into a lambda (both named & unnamed)
    if(named) {
      auto let_name_symbol = named_let_name(exp);
      scm_list lambda_defn(3);
      lambda_defn[0] = symconst::define;
      lambda_defn[1] = let_name_symbol;
      lambda_defn[2] = std::move(let_lambda_object);
      scm_list self_invocation(exprs.size()+1);
      self_invocation[0] = std::move(let_name_symbol);
      // push back each expression as an arg for the self-invoked-lambda call
      std::move(exprs.begin(), exprs.end(), self_invocation.begin()+1);
      // bind lambda to name & immediately invoke
      scm_list let_exp(2); 
      let_exp[0] = std::move(lambda_defn), let_exp[1] = std::move(self_invocation);
      // wrap the defined fcn in a self-invoking lambda to bind it to a local env
      scm_list outer_lambda(2);
      outer_lambda[0] = scm_list(2+let_exp.size());
      outer_lambda[1] = symconst::sentinel_arg;
      outer_lambda[0].exp[0] = symconst::lambda;
      outer_lambda[0].exp[1] = scm_list(1,symconst::sentinel_arg);
      std::move(let_exp.begin(),let_exp.end(),outer_lambda[0].exp.begin()+2);
      if(!cps_block) return outer_lambda;
      return generate_fundamental_form_cps(outer_lambda);
    } else {
      scm_list nameless_self_invoke(exprs.size()+1);
      nameless_self_invoke[0] = std::move(let_lambda_object);
      // push back each expression as an arg for the self-invoked-lambda call
      std::move(exprs.begin(), exprs.end(), nameless_self_invoke.begin()+1);
      // immediately invoke lambda w/ exps for vars
      if(!cps_block) return nameless_self_invoke; 
      return generate_fundamental_form_cps(nameless_self_invoke);
    }
  }

  /******************************************************************************
  * DERIVING LET*: (let* ((<var1> <val1>) ... (<varN> <valN>)) <body>)
  ******************************************************************************/

  // -- LET*: "LET", BUT VARIABLES CAN INVOKE ONE ANOTHER IN BINDINGS
  bool is_let_star(const scm_list& exp)noexcept{return is_tagged_list(exp,symconst::let_star);}

  scm_list make_let(const scm_list& variable_bindings,scm_list&& body)noexcept{
    scm_list let_exp(3);
    let_exp[0] = symconst::let, let_exp[1] = variable_bindings, let_exp[2] = body;
    return let_exp;
  }

  // Recursively nest lets, each containing one of let*'s parameters
  scm_list nest_lets(scm_node param, const scm_node& empty_param, scm_list& exp,const bool cps_block=false){
    if(param == empty_param)
      return convert_sequence_exp(let_body(exp));
    else {
      return convert_let_combination( // transform into combination immediately
              make_let(scm_list(1,*param), nest_lets(param+1,empty_param,exp,cps_block)),
              cps_block
            );
    }
  }

  // Convert let* into a series of nested let's
  scm_list convert_let_star_nested_lets(scm_list& exp,const bool cps_block=false) {
    if(exp.size() < 3)
      THROW_ERR("Invalid LET* expression, didn't received 2 args:"
        "\n     (let* (<var-bindings>) <body>)"
        "\n     <var-binding> = (<name> <value>)" << EXP_ERR(exp));
    scm_list params = let_parameters(exp);
    if(params.empty()) 
      return convert_let_combination(exp,cps_block);
    return nest_lets(params.begin(),params.end(),exp,cps_block);
  }

  /******************************************************************************
  * DERIVING LETREC: (letrec ((<var1> <val1>) ... (<varN> <valN>)) <body>)
  ******************************************************************************/

  // -- LETREC: "LET" ENABLING RECURSIVE BINDINGS
  //            => IE "LET" W/ ALL VARS GUARENTEED TO BE EVAL'D SIMULTANEOUSLY
  //            => TRANSFORM INTO A LET ASSIGNING EACH VAL 2B <undefined>, THEN
  //               SETTING EACH VAL TO ITS VALUE 1ST THING W/IN THE LET'S BODY
  //               -> LOOKING UP AN <undefined> VARIABLE THROWS AN ERROR!
  bool is_letrec(const scm_list& exp)noexcept{return is_tagged_list(exp,symconst::letrec);}

  scm_list convert_letrec_let(scm_list& exp,const bool cps_block=false) {
    if(exp.size() < 3)
      THROW_ERR("Invalid LETREC expression, didn't received 2 args:"
        "\n     (letrec (<var-bindings>) <body>)"
        "\n     <var-binding> = (<name> <value>)" << EXP_ERR(exp));
    scm_list params = let_parameters(exp);
    // We convert a paramterless letrec directly into a let
    if(params.empty()) return convert_let_combination(exp,cps_block);
    scm_list body = let_body(exp);
    scm_list vars = let_variables(params);
    scm_list vals = let_expressions(params);
    // Assign var default values to be undefined
    scm_list dflt_value_params(vars.size()), set_var_undef(2);
    set_var_undef[1] = data();
    for(size_type i = 0, n = vars.size(); i < n; ++i) {
      set_var_undef[0] = vars[i]; // var name bound to undefined data type
      dflt_value_params[i] = set_var_undef; 
    }
    // Define a new let, un-recursed
    scm_list unrec_let(vars.size()+body.size()+2);
    unrec_let[0] = symconst::let, unrec_let[1] = std::move(dflt_value_params);
    // Set the each var's value w/in the 'letrec's body
    for(size_type i = 0, n = vars.size(); i < n; ++i) {
      unrec_let[i+2] = scm_list(3);
      unrec_let[i+2].exp[0] = symconst::set;
      unrec_let[i+2].exp[1] = vars[i];
      unrec_let[i+2].exp[2] = vals[i];
    }
    // Append elts in the body, now post-assignment
    std::move(body.begin(), body.end(), unrec_let.begin()+vars.size()+2);
    return convert_let_combination(unrec_let,cps_block);
  }

  /******************************************************************************
  * DERIVING CASE
  ******************************************************************************/

  // CASE => (case <val> ((<keys1>) <exp1>) ... (else <expN>))
  //      => (cond ((memv <val> <keys1>) <exp1>) ... (else <expN>))
  bool is_case(const scm_list& exp)noexcept{return is_tagged_list(exp,symconst::case_t);}

  // Confirm 'case clause is correctly formatted
  void confirm_valid_case_clause(const scm_node& clause,        const char* format, 
                                 const size_type& clause_count, const scm_list& exp) {
    // Confirm clause is an expression
    if(!clause->is_type(types::exp))
      THROW_ERR("'case clause #" << clause_count << " [ " << *clause << " ] isn't an expression!" 
        << format << EXP_ERR(exp));
    // Confirm clause contains 2 elts
    if(clause->exp.size() != 2)
      THROW_ERR("'case clause #" << clause_count << " isn't an expression of length 2 (length = "
        << (!no_args_given(clause->exp) * clause->exp.size()) << ")!" 
        << format << EXP_ERR(exp));
    // Confirm clause was given a list of keys
    if(!is_cond_else_clause(clause) && !clause->exp[0].is_type(types::exp))
      THROW_ERR("'case clause #" << clause_count << " [ " << clause->exp[0]
        << " ] doesn't have a list of keys as its 1st arg!" << format << EXP_ERR(exp));
  }

  // Construct a 'cond equality clause from the 'case clause
  scm_list case_equality_clause(scm_node& clause, data& sought_val)noexcept{
    data keys_list(scm_list(clause->exp[0].exp.size()+1));
    keys_list.exp[0] = symconst::list;
    std::copy(clause->exp[0].exp.begin(), 
              clause->exp[0].exp.end(), 
              keys_list.exp.begin()+1);
    scm_list memv_exp(3);
    memv_exp[0] = symconst::memv, memv_exp[1] = sought_val;
    memv_exp[2] = std::move(keys_list);
    return memv_exp;
  }

  // Extract a normal & <else> 'case clause
  scm_list case_clause(scm_node& clause, data& sought_val)noexcept{
    scm_list clause_exp(2);
    clause_exp[0] = case_equality_clause(clause,sought_val);
    clause_exp[1] = cond_actions(clause);
    return clause_exp;
  }
  scm_list case_else_clause(scm_node& clause)noexcept{
    scm_list else_exp(2);
    else_exp[0] = symconst::else_t;
    else_exp[1] = data_cast(cond_actions(clause));
    return else_exp;
  }

  // Transform 'case into 'cond
  scm_list convert_case_cond(scm_list& exp) {
    static constexpr const char * const format = "\n     (case <val> <clause1> ... <clauseN>)"
                                                 "\n     <clause> = (<keys-list> <consequent>)";
    if(exp.size() < 3)
      THROW_ERR("CASE expression didn't receive enough args:" << format << EXP_ERR(exp));
    data& sought_val = exp[1];
    size_type clause_count = 1;
    scm_list converted_case(1,symconst::cond);
    for(auto clause=exp.begin()+2, null_clause=exp.end(); clause!=null_clause; ++clause, ++clause_count) {
      confirm_valid_case_clause(clause, format, clause_count, exp);
      if(is_cond_else_clause(clause)) {
        if(clause+1 == null_clause)
          converted_case.push_back(case_else_clause(clause));
        else
          THROW_ERR("CASE ELSE clause isn't last!\n     Total clauses after ELSE: " 
            << std::distance(clause,null_clause)-1 << format << EXP_ERR(exp));
      } else
        converted_case.push_back(case_clause(clause,sought_val));
    }
    return converted_case;
  }

  /******************************************************************************
  * DERIVING DO
  ******************************************************************************/

  // (do ((<var-name1> <init-val1> <val-manip1>) 
  //      ... 
  //      (<var-nameN> <init-valN> <val-manipN>))
  //     (<test> <expression1> ... <expressionN>)
  //     <body>)

  // (letrec ((<HEIST-DO-LETREC> 
  //   (lambda (<var-name1> ... <var-nameN>)
  //     (if <test>)
  //         (begin <expression1> ... <expressionN>) ; <void> w/o "<expression>"
  //         (begin 
  //           <body>
  //           (set! <var1> <val-manip1>)
  //           ...
  //           (set! <varN> <val-manipN>)
  //           (<HEIST-DO-LETREC> <var-name1> ... <var-nameN>)))))
  //   (<HEIST-DO-LETREC> <init-val1> ... <init-valN>))
  bool is_do(const scm_list& exp)noexcept{return is_tagged_list(exp,symconst::do_t);}

  // Confirms do-expression's var_defn_list is valid in structure
  void confirm_valid_var_defn_list(const scm_list& var_defn_list, const scm_list& exp) {
    for(const auto& var_defn : var_defn_list) {
      if(!var_defn.is_type(types::exp) || (var_defn.is_type(types::exp) && 
          var_defn.exp.size() != 2 && var_defn.exp.size() != 3))
        THROW_ERR("DO expression has non-var-defn-expression in <var-defn> list! -- CONVERT_DO_LETREC\n     -> [ "
                    << PROFILE(var_defn) << " ]\n     <var-defn> = (<var> <init-val> <optional-iteration-mutation>)"
                    << EXP_ERR(exp));
      else if(!var_defn.exp[0].is_type(types::sym))
        THROW_ERR("DO expression has an invalid <var> name in its <var-defn> list! -- CONVERT_DO_LETREC\n     -> [ "
                    << PROFILE(var_defn.exp[0]) 
                    << " ]\n     <var-defn> = (<var> <init-val> <optional-iteration-mutation>)" << EXP_ERR(exp));
    }
  }

  // Returns list of var-init-mod inner lists
  scm_list do_var_defn_list(const data& defn_exp, const scm_list& exp) {
    if(defn_exp.is_type(types::exp) && defn_exp.exp.empty())
      return scm_list(); // empty list, no vars to speak of
    if(!defn_exp.is_type(types::exp))
      THROW_ERR("DO expression expects <var-defn>s at position 1 -- CONVERT_DO_LETREC!\n     -> [ "
        << PROFILE(defn_exp) << " ]\n     <var-defn> = (<var> <init-val> <optional-iteration-mutation>)"
        << EXP_ERR(exp));
    confirm_valid_var_defn_list(defn_exp.exp, exp);
    return defn_exp.exp;
  }

  // Returns a list of variable names being defined
  // PRECONDITION: 'var_defn_list' MUST BE VALIDATED
  frame_vars do_var_names(const scm_list& var_defn_list, const scm_list& exp) {
    if(var_defn_list.empty()) {
      frame_vars no_vars(1,symconst::sentinel_arg); 
      return no_vars;
    }
    frame_vars names(var_defn_list.size());
    // parse variable names
    for(size_type i = 0, n = var_defn_list.size(); i < n; ++i)
      names[i] = var_defn_list[i].exp[0].sym;
    // confirm no duplicate variables names
    for(size_type i = 0, n = names.size(); i+1 < n; ++i)
      for(size_type j = i+1; j < n; ++j) 
        if(names[i] == names[j])
          THROW_ERR("DO expression has a duplicate <var> name in its <var-defn> list -- CONVERT_DO_LETREC!\n     -> [ " 
            << names[i] << " ]\n     <var-defn> = (<var> <init-val> <optional-iteration-mutation>)"
            << EXP_ERR(exp));
    return names;
  }

  // Returns a list of variabl initial values
  // PRECONDITION: 'var_defn_list' MUST BE VALIDATED
  scm_list do_var_init_values(const scm_list& var_defn_list)noexcept{
    if(var_defn_list.empty()) {
      scm_list empty_inits(1,scm_list(2));
      empty_inits[0].exp[0] = symconst::quote;
      empty_inits[0].exp[1] = symconst::sentinel_arg;
      return empty_inits;
    }
    scm_list init_values(var_defn_list.size());
    for(size_type i = 0, n = var_defn_list.size(); i < n; ++i)
      init_values[i] = var_defn_list[i].exp[1];
    return init_values;
  }

  // Returns a list of variable per-iteration value modifications
  // PRECONDITION: 'var_defn_list' MUST BE VALIDATED
  scm_list do_var_iteration_updates(const scm_list& var_defn_list, 
                                    const frame_vars& names)noexcept{
    if(var_defn_list.empty()) return scm_list();
    scm_list modifications(var_defn_list.size());
    // parse variable modifications
    for(size_type i = 0, n = var_defn_list.size(); i < n; ++i){
      if(var_defn_list[i].exp.size() != 3)
        modifications[i] = names[i];
      else
        modifications[i] = var_defn_list[i].exp[2];
    }
    return modifications;
  }


  // Returns a list breaking conditions for the do-expression
  scm_list do_break_test_exps(const data& break_exp, const scm_list& exp) {
    if(!break_exp.is_type(types::exp) || break_exp.exp.empty())
      THROW_ERR("DO expression expects <break-condition-list> at position 2 -- CONVERT_DO_LETREC! -> [ " 
        << PROFILE(break_exp) << " ]\n     <break-condition-list> = (<break-condition> <optional-returned-expression>)"
        << EXP_ERR(exp));
    return break_exp.exp;
  }


  // Returns the do-exp's body + variable modifications (per-iteration) wrapped in a 'begin' clause
  scm_list do_modified_body(const scm_list& exp, const frame_vars& names, 
                                                 const scm_list&& mod_vals)noexcept{
    // Add the do-expression's body
    scm_list body(exp.begin()+3, exp.end());
    // Add the modifications of its values (per-iteration)
    if(names.size() > 1 || names[0] != symconst::sentinel_arg) {
      scm_list set_exp(3); set_exp[0] = symconst::set;
      for(size_type i = 0, n = names.size(); i < n; ++i) {
        set_exp[1] = names[i], set_exp[2] = mod_vals[i];
        body.push_back(set_exp);
      }
    }
    // Add the recursive call to reiterate the do-expression
    body.push_back(scm_list(1,symconst::do_label));
    auto& recursive_call = body[body.size()-1].exp;
    for(const auto& name : names) 
      recursive_call.push_back(name);
    return convert_sequence_exp(body); // Return as a 'begin' clause
  }


  // Returns a lambda to run for each iteration over the loop, until >= 1 break 
  //   condition is met
  scm_list do_iteration_lambda(const scm_list& body, const scm_list&& break_test_exps, 
                                                     const frame_vars& names)noexcept{
    // test the break condition
    scm_list conditioned_body(2);
    conditioned_body[0] = symconst::if_t;
    conditioned_body[1] = break_test_exps[0];
    // eval each expression & return the last one (iff provided optional exps)
    if(break_test_exps.size() > 1) {
      conditioned_body.push_back(scm_list(1, symconst::begin));
      auto& break_exps = conditioned_body.rbegin()->exp;
      break_exps.insert(break_exps.end(), break_test_exps.begin()+1, break_test_exps.end());
    // if NOT given the optional exps, return <void>
    } else {
      conditioned_body.push_back(G::VOID_DATA_OBJECT); 
    }
    // if break !condition, evaluate the body & reset values
    conditioned_body.push_back(body);
    // convert list of names to a scm_list of name symbols
    scm_list names_scm_list(names.size());
    std::copy(names.begin(), names.end(), names_scm_list.begin());
    scm_list lambda_exp(3);
    lambda_exp[0] = symconst::lambda;
    lambda_exp[1] = std::move(names_scm_list);
    lambda_exp[2] = std::move(conditioned_body);
    return lambda_exp;
  }


  // Converts the 'do expression into a 'letrec
  scm_list convert_do_letrec(scm_list& exp) {
    if(exp.size() < 3)
      THROW_ERR("DO expression didn't receive enough args!"
        "\n     (do ((<var> <init-val> <optional-iteration-mutation>) ...)"
        "\n         (<test> <optional-return-expressions>)"
        "\n         <optional-body>)"
        << EXP_ERR(exp));

    auto var_defns = do_var_defn_list(exp[1],exp);
    auto var_names = do_var_names(var_defns,exp);
    auto var_inits = do_var_init_values(var_defns);
    auto iteration_lambda = do_iteration_lambda(do_modified_body(exp, var_names, 
                                                  do_var_iteration_updates(var_defns,var_names)), 
                                                do_break_test_exps(exp[2],exp), var_names);
    // do expression's intial call to be invoked
    scm_list initial_do_call(var_inits.size()+1);
    initial_do_call[0] = symconst::do_label;
    std::move(var_inits.begin(), var_inits.end(), initial_do_call.begin()+1);
    // ctor the do expression: (letrec ((do-label iteration-lambda)) initial-do-call)
    scm_list letrec_conversion(3);
    letrec_conversion[0] = symconst::letrec;
    letrec_conversion[1] = scm_list(1,scm_list(2));
    letrec_conversion[1].exp[0].exp[0] = symconst::do_label;
    letrec_conversion[1].exp[0].exp[1] = std::move(iteration_lambda);
    letrec_conversion[2] = std::move(initial_do_call);
    return letrec_conversion;
  }

  /******************************************************************************
  * REPRESENTING QUASIQUOTE, UNQUOTE, UNQUOTE-SPLICING
  ******************************************************************************/

  bool is_quasiquote(const scm_list& exp)      noexcept{return is_tagged_list(exp,symconst::quasiquote);}
  bool is_unquote(const scm_list& exp)         noexcept{return is_tagged_list(exp,symconst::unquote);}
  bool is_unquote_splicing(const scm_list& exp)noexcept{return is_tagged_list(exp,symconst::unquo_splice);}

  // Returns a data object containing the unquoted data. If the data is an 
  //   expression, it ought to be analyzed by scm_analyze in order to convert 
  //   its token strings into scheme code objects
  data process_unquoted(const scm_list& exp) {
    if(exp.size() != 2)
      THROW_ERR("'unquote expects one argument: (unquote <expression>)"
        << EXP_ERR(exp));
    return exp[1];
  }


  // Requotes data if symbolic
  data unquote_splicing_atom(const data& d)noexcept{
    if(d.is_type(types::sym)) {
      scm_list quoted_sym(2);
      quoted_sym[0] = symconst::quote, quoted_sym[1] = d;
      return data(quoted_sym);
    }
    return d;
  }


  // Confirm whether 'd' is the AST's empty-list representation
  bool data_is_the_AST_empty_list(const data& d)noexcept{
    return d.is_type(types::exp) && d.exp.size() == 2 && 
           d.exp[0].is_type(types::sym) && 
           d.exp[0].sym == symconst::quote && 
           d.exp[1].is_type(types::sym) && 
           d.exp[1].sym == symconst::emptylist;
  }


  // Expands the given pair into a scm expression. 
  //   => Helper fcn for processing ,@
  //   => Returns whether pair sequence was NOT null-terminated.
  // PRECONDITION: 'pair_object' MUST BE ACYCLIC
  bool expand_list_into_exp(par_type& pair_object,scm_list& exp)noexcept{
    // unpack car
    exp.push_back(unquote_splicing_atom(pair_object->first));
    // unpack cdr
    if(pair_object->second.is_type(types::par))
      return expand_list_into_exp(pair_object->second.par, exp);
    // don't push the terminating '() if at the end of the list/pair-sequence
    else if(is_not_THE_EMPTY_LIST(pair_object->second)) {
      exp.push_back(unquote_splicing_atom(pair_object->second));
      return true;
    }
    return false;
  }


  // Returns an expression object containing the unquoted list/vector's elements.
  //   => Returns whether the expanded sequence was NOT null-terminated.
  enum class unsplice_status {list_star, list, atom};
  unsplice_status process_unquote_splicing(const scm_list& exp, env_type& env, scm_list& spliceable_data) {
    if(exp.size() != 2)
      THROW_ERR("'unquote-splicing didn't receive 1 arg!"
        "\n     (unquote-splicing <spliceable-expression>)"
        << EXP_ERR(exp));
    data eval_result = data_cast(scm_eval(scm_list_cast(exp[1]),env));
    // if splicing a non-pair
    if(!eval_result.is_type(types::par)) {
      spliceable_data.push_back(unquote_splicing_atom(eval_result)); // add as-is
      return unsplice_status::atom;                                  // trigger 'append'
    }
    // confirm an acyclic pair sequence otherwise
    if(primitive_list_is_acyclic_and_null_terminated(eval_result) == list_status::cyclic)
      THROW_ERR("'unquote-splicing "<<PROFILE(eval_result)<<" can't be spliced in with a cycle!"<<EXP_ERR(exp));
    return expand_list_into_exp(eval_result.par,spliceable_data) ? unsplice_status::list_star : unsplice_status::list;
  }


  // Handle appending 'atomic' or 'list_star' data to a quasiquote expression
  scm_list quasiquote_append_non_list(scm_list& spliceable_data, scm_list& quote_val,
                                      const scm_list& exp,        const bool& is_dotted_list, 
                                      const bool& quoting_vector, const bool& not_last_elt) {
    static constexpr const char * const bad_vector = 
      "'quasiquote can't append [via ,@] an improper list to a vector!\n     Tried to splice in: ";
    static constexpr const char * const mid_splice = 
      "'quasiquote can't splice [via ,@] an improper list into the middle of a list!\n     Tried to splice in: ";
    // confirm not splicing a list_star/atomic into a vector nor mid-list
    if(quoting_vector && is_dotted_list)
      THROW_ERR(bad_vector<<"(list_star "<<spliceable_data[0]<<' '<<spliceable_data[1]<<')'<<EXP_ERR(exp));
    if(quoting_vector)
      THROW_ERR(bad_vector<<spliceable_data[0]<<" of type \""<<spliceable_data[0].type_name()<<'"'<<EXP_ERR(exp));
    if(not_last_elt && is_dotted_list)
      THROW_ERR(mid_splice<<"(list_star "<<spliceable_data[0]<<' '<<spliceable_data[1]<<')'<<EXP_ERR(exp));
    if(not_last_elt)
      THROW_ERR(mid_splice<<spliceable_data[0]<<" of type \""<<spliceable_data[0].type_name()<<'"'<<EXP_ERR(exp));
    // return the current expression as an 'append' to the quasiquote expression
    scm_list appended_exp(3);
    appended_exp[0] = symconst::append, appended_exp[1] = quote_val;
    if(is_dotted_list) {
      spliceable_data.insert(spliceable_data.begin(), symconst::list_star);
      appended_exp[2] = spliceable_data;
      return appended_exp;
    }
    appended_exp[2] = spliceable_data[0];
    return appended_exp;
  }


  // Tag <quote_val> w/ 'vector 'list* or 'list as per <quoted_exp>
  void tag_quote_val(scm_list& quoted_exp, scm_list& quote_val, const scm_list& exp) {
    const bool is_vector      = is_vector_literal(quoted_exp);
    const bool is_dotted_list = is_quoted_cons(quoted_exp,symconst::quasiquote);
    if(is_vector && is_dotted_list) 
      THROW_ERR("'quasiquote found unexpected dot (.) in 'vector-literal! -- ANALYZE_QUOTE_VECTOR"<<EXP_ERR(exp));
    if(is_vector) {
      quoted_exp.erase(quoted_exp.begin(),quoted_exp.begin()+1); // erase 'vector-literal tag
      quote_val.push_back(symconst::vector);
    } else if(is_dotted_list)
      quote_val.push_back(symconst::list_star);
    else
      quote_val.push_back(symconst::list);
  }


  // Recursively unquotes/unquote-splices data as needed w/in quasiquote templates.
  // Also quotes each piece of data as needed.
  void unquote_quasiquote_template(scm_list& quote_val, scm_list& quoted_exp, env_type& env, 
                                   const scm_list& exp, const size_type& nested_level){
    // Account for whether splicing into a vector
    bool quoting_a_vector = (quote_val[0].sym == symconst::vector);
    
    // Wrap 'quote' around each obj in the expression, and expand unquote/unquote-splicing instances
    for(size_type i = 0, n = quoted_exp.size(); i < n; ++i) {
      // if quoting an empty expression, push the empty list 
      if(quoted_exp[i].is_type(types::exp) && quoted_exp[i].exp.empty()) {
        quote_val.push_back(scm_list(2));
        quote_val.rbegin()->exp[0] = symconst::quote;
        quote_val.rbegin()->exp[1] = symconst::emptylist;


      // UNQUOTE
      } else if(quoted_exp[i].is_type(types::exp) && is_unquote(quoted_exp[i].exp)) {
        if(!nested_level) { // if , add the data as-is
          quote_val.push_back(quoted_exp[i].exp[1]);
        } else { // in a nested quasiquote, recursively parse the unquote expression 1 level removed
          scm_list inner_exp;
          tag_quote_val(quoted_exp[i].exp, inner_exp, exp);
          unquote_quasiquote_template(inner_exp, quoted_exp[i].exp, env, exp, nested_level - 1);
          quote_val.push_back(inner_exp);
        }
      

      // UNQUOTE-SPLICING      
      } else if(quoted_exp[i].is_type(types::exp) && is_unquote_splicing(quoted_exp[i].exp)) {
        if(!nested_level) { // if ,@ eval the expression & splice in the resulting list's elts
          scm_list spliceable_data;
          auto unsplice_stat = process_unquote_splicing(quoted_exp[i].exp, env, spliceable_data);
          // If splicing the empty list, continue (does nothing)
          if(unsplice_stat == unsplice_status::atom && data_is_the_AST_empty_list(spliceable_data[0]))
            continue;
          // If splicing in a list_star or atom
          if(unsplice_stat == unsplice_status::list_star || unsplice_stat == unsplice_status::atom) {
            quote_val = quasiquote_append_non_list(spliceable_data, quote_val, exp,
                        (unsplice_stat == unsplice_status::list_star), quoting_a_vector, (i != n-1));
            return;
          // Otherwise (splicing a list), splice in data as-is
          } else {
            quote_val.insert(quote_val.end(), spliceable_data.begin(), spliceable_data.end());
          }
        } else { // in a nested quasiquote, recursively parse the unquote-splicing expression 1 level removed
          scm_list inner_exp;
          tag_quote_val(quoted_exp[i].exp, inner_exp, exp);
          unquote_quasiquote_template(inner_exp, quoted_exp[i].exp, env, exp, nested_level - 1);
          quote_val.push_back(inner_exp);
        }
        
        
      // else, quote the nested data
      } else {
        // if other expression, also recursively analyze for unquotes
        if(quoted_exp[i].is_type(types::exp)) {
          scm_list inner_exp;
          // tag <inner_exp> as needed w/ either 'vector, 'list_star, or 'list
          auto nested_expression_level = is_quasiquote(quoted_exp[i].exp) + nested_level;
          tag_quote_val(quoted_exp[i].exp, inner_exp, exp);
          // propagate quotations throughout the sub-expression
          unquote_quasiquote_template(inner_exp, quoted_exp[i].exp, env, exp, nested_expression_level);
          quote_val.push_back(inner_exp);
        

        // if atomic, simply quote the atom
        } else {
          quote_val.push_back(scm_list(2));
          quote_val.rbegin()->exp[0] = symconst::quote;
          quote_val.rbegin()->exp[1] = quoted_exp[i];
        }
      }
    }
  }


  // Works similarly to 'analyze_quoted', except that quoted 'unquote's are
  //   (wait for it) unquoted (BAYUM), and quoted 'unquote-splicing's are both
  //   unquoted & expanded.
  exe_type analyze_quasiquote(scm_list& exp,const bool cps_block=false) {
    if(exp.size() != 2 || data_is_the_SENTINEL_VAL(exp[1]))
      THROW_ERR("'quasiquote expects one argument: (quasiquote <expression>)"<<EXP_ERR(exp));
    // Get quasiquoted data
    auto quoted_data = text_of_quotation(exp);
    
    // If quasiquoted data is atomic, return as-is
    if(!quoted_data.is_type(types::exp)) {
      if(cps_block) return [unquoted_exp=generate_fundamental_form_cps(quoted_data)](env_type&){return unquoted_exp;};
      return [unquoted_exp=scm_list(1,quoted_data)](env_type&){return unquoted_exp;};
    }

    // If quoting an empty expression, return the empty list
    if(quoted_data.exp.empty()) {
      if(cps_block) return [](env_type&){return generate_fundamental_form_cps(symconst::emptylist);};
      return [](env_type&){return G::EMPTY_LIST_EXPRESSION;};
    }

    // If quasiquoted an unquote, unpack its data
    if(is_unquote(quoted_data.exp)) {
      data unquoted_data = process_unquoted(quoted_data.exp);
      // If an expression, return the exec proc of the analyzed expression
      if(unquoted_data.is_type(types::exp)) {
        if(cps_block) scm_analyze(generate_fundamental_form_cps(unquoted_data),false,cps_block);
        return scm_analyze(std::move(unquoted_data.exp));
      }
      // If an atomic, return the exec proc of its evaluation (in case a variable or some such)
      if(cps_block) scm_analyze(generate_fundamental_form_cps(unquoted_data),false,cps_block);
      return scm_analyze(scm_list(1,unquoted_data));
    }

    // If quasiquoted an expression, expand such into a list/list_star of quoted data
    return [quoted_exp=std::move(quoted_data.exp),exp=std::move(exp),cps_block=std::move(cps_block)]
      (env_type& env) {
        // Unquote/Splice-In data as needed throughout the quasiquote template
        scm_list quote_val, mutable_quoted_exp = quoted_exp;
        // tag <quote_val> as needed w/ either 'vector, 'list_star, or 'list
        tag_quote_val(mutable_quoted_exp, quote_val, exp);
        // propagate quotations throughout the sub-expression
        unquote_quasiquote_template(quote_val,mutable_quoted_exp,env,exp,0);
        if(cps_block) return scm_analyze(generate_fundamental_form_cps(quote_val),false,true)(env);
        return scm_eval(std::move(quote_val),env);
      };
  }

  /******************************************************************************
  * CONTINUATION-PASSING-STYLE EXPANSION OPTIMIZATION -- PASS 1
  ******************************************************************************/

  // Whether <cps_exp> contains <sym>
  bool CPS_exp_contains_symbol(const scm_list& cps_exp,const sym_type& sym)noexcept{
    for(size_type i = 0, n = cps_exp.size(); i < n; ++i)
      if((cps_exp[i].is_type(types::exp) && CPS_exp_contains_symbol(cps_exp[i].exp,sym)) ||
         (cps_exp[i].is_type(types::sym) && cps_exp[i].sym == sym))
        return true;
    return false;
  }


  // Is a lambda of 1 arg
  bool is_unary_arg_lambda_cps_exp(const scm_list& cps_exp)noexcept{
    return cps_exp.size() > 2 && is_tagged_list(cps_exp,symconst::lambda) && 
           cps_exp[1].is_type(types::exp) && cps_exp[1].exp.size() == 1 && 
           cps_exp[1].exp[0].is_type(types::sym);
  }


  // Optimizable (pass 1) CPS lambda
  bool is_optimizable_CPS_pass_1_exp(const scm_list& cps_exp)noexcept{
    return cps_exp.size() == 3 && is_unary_arg_lambda_cps_exp(cps_exp) && 
           cps_exp[2].is_type(types::exp) && cps_exp[2].exp.size() == 2 &&
           cps_exp[2].exp[0].is_type(types::exp) && cps_exp[2].exp[1].is_type(types::sym) &&
           cps_exp[2].exp[1].sym == cps_exp[1].exp[0].sym &&
           !CPS_exp_contains_symbol(cps_exp[2].exp[0].exp,cps_exp[2].exp[1].sym);
  }


  // (lambda (a) (<expression-w/o-a> a)) => <expression-w/o-a>
  void CPS_lambda_unwrapping_optimization_pass_1(scm_list& cps_exp)noexcept{
    if(is_optimizable_CPS_pass_1_exp(cps_exp)) {
      auto temp = cps_exp[2].exp[0].exp;
      cps_exp = temp;
      CPS_lambda_unwrapping_optimization_pass_1(cps_exp);
    } else {
      for(size_type i = 0, n = cps_exp.size(); i < n; ++i)
        if(cps_exp[i].is_type(types::exp))
          CPS_lambda_unwrapping_optimization_pass_1(cps_exp[i].exp);
    }
  }

  /******************************************************************************
  * CONTINUATION-PASSING-STYLE EXPANSION OPTIMIZATION -- PASS 2
  ******************************************************************************/

  bool data_is_continuation_parameter(const data&)noexcept;

  void replace_all_instances_of_symB_with_symA(scm_list& cps_exp,const sym_type& symA,
                                                                 const sym_type& symB)noexcept{
    for(size_type i = 0, n = cps_exp.size(); i < n; ++i) {
      if(cps_exp[i].is_type(types::exp))
        replace_all_instances_of_symB_with_symA(cps_exp[i].exp,symA,symB);
      else if(cps_exp[i].is_type(types::sym) && cps_exp[i].sym == symB)
        cps_exp[i].sym = symA;
    }
  }


  // Optimizable (pass 2) CPS lambda
  bool is_optimizable_CPS_pass_2_exp(const scm_list& cps_exp)noexcept{
    return cps_exp.size() == 2 && cps_exp[0].is_type(types::exp) && 
           data_is_continuation_parameter(cps_exp[1]) && 
           is_unary_arg_lambda_cps_exp(cps_exp[0].exp) && cps_exp[0].exp.size() == 3 &&
           cps_exp[0].exp[2].is_type(types::exp) &&
           data_is_continuation_parameter(cps_exp[0].exp[1].exp[0]) &&
           !CPS_exp_contains_symbol(cps_exp[0].exp,cps_exp[1].sym);
  }


  // ((lambda (b) <exp-w/o-a>) a) => <exp-w/o-a>,
  // 1. With each reference to <b> replaced by <a>
  // 2. Iff both <a> & <b> are continuations
  void CPS_lambda_unwrapping_optimization_pass_2(scm_list& cps_exp)noexcept{
    if(is_optimizable_CPS_pass_2_exp(cps_exp)) {
      const auto a_param = cps_exp[1].sym, b_param = cps_exp[0].exp[1].exp[0].sym;
      auto temp = cps_exp[0].exp[2].exp;
      cps_exp = temp;
      replace_all_instances_of_symB_with_symA(cps_exp,a_param,b_param);
      CPS_lambda_unwrapping_optimization_pass_2(cps_exp);
    } else {
      for(size_type i = 0, n = cps_exp.size(); i < n; ++i)
        if(cps_exp[i].is_type(types::exp))
          CPS_lambda_unwrapping_optimization_pass_2(cps_exp[i].exp);
    }
  }

  /******************************************************************************
  * CONTINUATION-PASSING-STYLE EXPANSION OPTIMIZATION -- PASS 3
  ******************************************************************************/

  // Replaces the 1 instance of <symB> w/ <objA>
  bool replace_instance_of_symB_with_objA(scm_list& cps_exp,const data& objA,
                                                            const sym_type& symB)noexcept{
    for(size_type i = 0, n = cps_exp.size(); i < n; ++i)
      if(cps_exp[i].is_type(types::exp) && replace_instance_of_symB_with_objA(cps_exp[i].exp,objA,symB)) {
        return true;
      } else if(cps_exp[i].is_type(types::sym) && cps_exp[i].sym == symB) {
        cps_exp[i] = objA;
        return true;
      }
    return false;
  }


  // counts instances of <sym> w/in <cps_exp>
  void CPS_exp_count_instances_of_symbol(const scm_list& cps_exp,const sym_type& sym,size_type& count)noexcept{
    for(size_type i = 0, n = cps_exp.size(); i < n; ++i)
      if(cps_exp[i].is_type(types::exp)) {
        CPS_exp_count_instances_of_symbol(cps_exp[i].exp,sym,count);
        if(count > 2) return;
      } else if(cps_exp[i].is_type(types::sym) && cps_exp[i].sym == sym) {
        if(++count > 2) return;
      }
  }


  // Optimizable (pass 3) CPS lambda
  bool is_optimizable_CPS_pass_3_exp(const scm_list& cps_exp)noexcept{
    if(cps_exp.size() == 2 && cps_exp[0].is_type(types::exp) && 
      is_unary_arg_lambda_cps_exp(cps_exp[0].exp) && cps_exp[0].exp.size() == 3 &&
      cps_exp[0].exp[2].is_type(types::exp) && data_is_continuation_parameter(cps_exp[0].exp[1].exp[0])) {
        size_type count = 0;
        CPS_exp_count_instances_of_symbol(cps_exp[0].exp[2].exp,cps_exp[0].exp[1].exp[0].sym,count);
        return count < 2;
      }
    return false;
  }


  // ((lambda (k) <exp-w/-only-1-instance-of-k>) <obj>) => <exp-w/-only-1-instance-of-k> 
  // 1. <k> is a continuation
  // 2. That 1 instance of <k> is replaced w/ <obj>
  void CPS_lambda_unwrapping_optimization_pass_3(scm_list& cps_exp)noexcept{
    if(is_optimizable_CPS_pass_3_exp(cps_exp)) {
      const data a_obj = cps_exp[1];
      const auto b_param = cps_exp[0].exp[1].exp[0].sym;
      auto temp = cps_exp[0].exp[2].exp;
      cps_exp = temp;
      replace_instance_of_symB_with_objA(cps_exp,a_obj,b_param);
      CPS_lambda_unwrapping_optimization_pass_3(cps_exp);
    } else {
      for(size_type i = 0, n = cps_exp.size(); i < n; ++i)
        if(cps_exp[i].is_type(types::exp))
          CPS_lambda_unwrapping_optimization_pass_3(cps_exp[i].exp);
    }
  }

  /******************************************************************************
  * CONTINUATION-PASSING-STYLE EXPANSION OPTIMIZATION -- PASS 4
  ******************************************************************************/

  // Expand the "ignore" lambda application
  void expand_CPS_lambda_pass_4_application(scm_list& cps_exp,const size_type& i)noexcept{
    scm_list unwrapped_exp(cps_exp[i].exp[0].exp.size()-1);
    unwrapped_exp[0] = cps_exp[i].exp[1];
    std::move(cps_exp[i].exp[0].exp.begin()+2,cps_exp[i].exp[0].exp.end(),unwrapped_exp.begin()+1);
    cps_exp.erase(cps_exp.begin()+i); // erase optimized lambda application
    cps_exp.insert(cps_exp.begin()+i, // insert expanded application
                   std::make_move_iterator(unwrapped_exp.begin()),
                   std::make_move_iterator(unwrapped_exp.end())); 
  }


  // Optimizable (pass 4) CPS lambda
  bool is_optimizable_CPS_pass_4_exp(const scm_list& cps_exp)noexcept{
    return cps_exp.size() == 2 && cps_exp[0].is_type(types::exp) &&
           is_unary_arg_lambda_cps_exp(cps_exp[0].exp) &&
           cps_exp[0].exp[1].exp[0].is_type(types::sym) &&
           cps_exp[0].exp[1].exp[0].sym == "ignore";
  }


  // ((lambda (ignore) <exp> ...) <obj>) => <obj> <exp> ...
  void CPS_lambda_unwrapping_optimization_pass_4(scm_list& cps_exp)noexcept{
    for(size_type i = 0, n = cps_exp.size(); i < n; ++i) {
      if(cps_exp[i].is_type(types::exp)) {
        if(is_optimizable_CPS_pass_4_exp(cps_exp[i].exp)) {
          expand_CPS_lambda_pass_4_application(cps_exp,i);
          CPS_lambda_unwrapping_optimization_pass_4(cps_exp);
          return;
        }
        CPS_lambda_unwrapping_optimization_pass_4(cps_exp[i].exp);
      }
    }
  }

  /******************************************************************************
  * CONTINUATION-PASSING-STYLE EXPANSION OPTIMIZATION -- PASS 5
  ******************************************************************************/

  // Revert the "lambda-set!" definition transformation
  void expand_CPS_lambda_pass_5_definition(scm_list& cps_exp,const size_type& i)noexcept{
    scm_list unwrapped_exp(cps_exp[i].exp[0].exp.size()-2);
    std::move(cps_exp[i].exp[0].exp.begin()+2,cps_exp[i].exp[0].exp.end(),unwrapped_exp.begin());
    unwrapped_exp[0].exp[0].sym = symconst::define;
    cps_exp.erase(cps_exp.begin()+i); // erase optimized lambda application
    cps_exp.insert(cps_exp.begin()+i, // insert expanded application
                   std::make_move_iterator(unwrapped_exp.begin()),
                   std::make_move_iterator(unwrapped_exp.end()));
  }


  // Optimizable (pass 5) CPS lambda
  bool is_optimizable_CPS_pass_5_exp(const scm_list& cps_exp)noexcept{
    return cps_exp.size() == 2 && cps_exp[0].is_type(types::exp) &&
           is_unary_arg_lambda_cps_exp(cps_exp[0].exp) &&
           ((cps_exp[1].is_type(types::bol) && !cps_exp[1].bol.val) || 
            (cps_exp[1].is_type(types::sym) && cps_exp[1].sym == symconst::false_t)) && 
           cps_exp[0].exp[2].is_type(types::exp) && cps_exp[0].exp[2].exp.size() == 3 && 
           cps_exp[0].exp[2].exp[0].is_type(types::sym) && cps_exp[0].exp[2].exp[0].sym == symconst::set;
  }


  // Pass 5: Reifying Definitions
  // ((lambda (<name>) (set! <name> <val>) <exp> ...) #f) => (define <name> <val>) <exp> ...
  void CPS_lambda_unwrapping_optimization_pass_5(scm_list& cps_exp)noexcept{
    for(size_type i = 0, n = cps_exp.size(); i < n; ++i) {
      if(cps_exp[i].is_type(types::exp)) {
        if(is_optimizable_CPS_pass_5_exp(cps_exp[i].exp)) {
          expand_CPS_lambda_pass_5_definition(cps_exp,i);
          CPS_lambda_unwrapping_optimization_pass_5(cps_exp);
          return;
        }
        CPS_lambda_unwrapping_optimization_pass_5(cps_exp[i].exp);
      }
    }
  }

  /******************************************************************************
  * CONTINUATION-PASSING-STYLE EXPANSION OPTIMIZATION -- DISPATCH
  ******************************************************************************/

  // Perform several optimization passes on the generated CPS to reduce lambda count 
  // NOTE: CPS atomics are already optimized @expansion-time
  void optimize_CPS_code_generation(scm_list& cps_exp)noexcept{
    CPS_lambda_unwrapping_optimization_pass_1(cps_exp);
    CPS_lambda_unwrapping_optimization_pass_2(cps_exp);
    CPS_lambda_unwrapping_optimization_pass_3(cps_exp);
    CPS_lambda_unwrapping_optimization_pass_4(cps_exp);
    CPS_lambda_unwrapping_optimization_pass_5(cps_exp);
  }

  /******************************************************************************
  * CONTINUATION-PASSING-STYLE EXPANSION
  ******************************************************************************/

  void confirm_valid_define_syntax(const scm_list&);

  bool is_scm_cps(const scm_list& exp)noexcept{return is_tagged_list(exp,symconst::scm_cps);}
  bool is_cps_quote(const scm_list& exp)noexcept{return is_tagged_list(exp,symconst::cps_quote);}
  bool is_cps_application(const scm_list& exp)noexcept{return is_tagged_list(exp,symconst::cps_app_tag);}

  // Heist-specific checker to not prefix C++ derived special forms w/ application tag
  bool is_HEIST_cpp_derived_special_form(const sym_type& app)noexcept{
    return app == symconst::cps_quote || app == symconst::scm_cps    || app == symconst::core_syn     ||
           app == symconst::and_t     || app == symconst::or_t       || app == symconst::scons        ||
           app == symconst::stream    || app == symconst::delay      || app == symconst::cond         ||
           app == symconst::case_t    || app == symconst::let        || app == symconst::let_star     ||
           app == symconst::letrec    || app == symconst::do_t       || app == symconst::quasiquote   ||
           app == symconst::let_syn   || app == symconst::letrec_syn || app == symconst::vec_literal  ||
           app == symconst::unquote   || app == symconst::unquo_splice;
  }


  // Generate a unique hashed variant of a cps identifier name
  // NOTE: Max Unique Hashes = (expt 18446744073709551615 18446744073709551615)
  scm_string generate_unique_cps_hash()noexcept{
    if(G::CPS_HASH_IDX_1 != G::MAX_SIZE_TYPE)
      return symconst::continuation
        + std::to_string(G::CPS_HASH_IDX_2) + '_' + std::to_string(G::CPS_HASH_IDX_1++);
    return symconst::continuation
      + std::to_string(++G::CPS_HASH_IDX_2) + '_' + std::to_string(G::CPS_HASH_IDX_1++);
  }


  // CPS atomics may be returned as is: (cps <cps-atomic>) -> <cps-atomic>
  bool data_is_cps_atomic(const data& d)noexcept{
    return !d.is_type(types::exp) || 
           is_tagged_list(d.exp,symconst::syn_rules) || 
           is_tagged_list(d.exp,symconst::quote);
  }


  scm_list cps_expand_application(const scm_list& application) {
    const auto app_len = application.size();
    scm_list cps_app(app_len+2), cps_exp(3);
    cps_exp[0] = symconst::lambda;
    cps_exp[1] = scm_list(1,generate_unique_cps_hash()); // "k"
    cps_exp[2] = scm_list(2);
    auto iter = cps_exp.begin()+2;
    for(size_type i = 0; i < app_len; ++i) {
      if(data_is_cps_atomic(application[i])) {
        cps_app[i+1] = application[i];
      } else {
        iter->exp[0] = generate_fundamental_form_cps(application[i]);
        iter->exp[1] = scm_list(3);
        iter->exp[1].exp[0] = symconst::lambda;
        iter->exp[1].exp[1] = scm_list(1,generate_unique_cps_hash()); // "arg"
        cps_app[i+1] = iter->exp[1].exp[1].exp[0];
        iter->exp[1].exp[2] = scm_list(2);
        iter = iter->exp[1].exp.begin()+2;
      }
    }
    cps_app[0] = symconst::cps_app_tag; // Add the cps-application prefix
    cps_app[app_len+1] = cps_exp[1].exp[0];
    iter->exp = std::move(cps_app);
    return cps_exp;
  }


  // Generates the procedure to set <var> to <val> after binding <var> as a lambda arg
  scm_list get_cps_defn_set_procedure(const data& continuation,const data& var,const data& val){
    scm_list set_exp(2);
    // Set atomic values and pass to the continuation
    if(data_is_cps_atomic(val)) {
      set_exp[0] = continuation;
      set_exp[1] = scm_list(3);
      set_exp[1].exp[0] = symconst::set;
      set_exp[1].exp[1] = var;
      set_exp[1].exp[2] = val;
    // Cps-ify non-atomic values, passing a lambda as a continuation that in 
    //   turn sets the recieved value & passes such to the continuation given here as an arg
    } else {
      set_exp[0] = generate_fundamental_form_cps(val,false);
      set_exp[1] = scm_list(3);
      set_exp[1].exp[0] = symconst::lambda;
      set_exp[1].exp[1] = scm_list(1,generate_unique_cps_hash()); // "defn-val"
      set_exp[1].exp[2] = scm_list(2);
      set_exp[1].exp[2].exp[0] = continuation;
      set_exp[1].exp[2].exp[1] = scm_list(3);
      set_exp[1].exp[2].exp[1].exp[0] = symconst::set;
      set_exp[1].exp[2].exp[1].exp[1] = var;
      set_exp[1].exp[2].exp[1].exp[2] = set_exp[1].exp[1].exp[0]; // defn-val
    }
    return set_exp;
  }


  // Generates a CPS definition in the middle of a BEGIN or LAMBDA BODY sequence, w/ <rest_exp>
  //   being the remaining expressions AFTER this definition in the sequence
  // PRECONDITION: !rest_exp.empty()
  scm_list generate_mid_seq_cps_var_defn(const scm_list& defn_exp, const data& rest_exp){
    scm_list cps_defn(3);
    cps_defn[0] = symconst::lambda;
    cps_defn[1] = scm_list(1,generate_unique_cps_hash()); // topmost continuation "k"
    cps_defn[2] = scm_list(2);

    cps_defn[2].exp[1] = G::FALSE_DATA_BOOLEAN; // initially bind defined symbol to #f
    cps_defn[2].exp[0] = scm_list(3);
    cps_defn[2].exp[0].exp[0] = symconst::lambda;
    cps_defn[2].exp[0].exp[1] = scm_list(1,defn_exp[1]); // defined symbol as an argument
    cps_defn[2].exp[0].exp[2] = scm_list(2);

    // Bind Var to Value
    cps_defn[2].exp[0].exp[2].exp[0] = scm_list(3);
    cps_defn[2].exp[0].exp[2].exp[0].exp[0] = symconst::lambda;
    cps_defn[2].exp[0].exp[2].exp[0].exp[1] = scm_list(1,generate_unique_cps_hash()); // continuation "k1" of set!
    cps_defn[2].exp[0].exp[2].exp[0].exp[2] = get_cps_defn_set_procedure(cps_defn[2].exp[0].exp[2].exp[0].exp[1].exp[0],
                                                                         defn_exp[1],defn_exp[2]);

    // Continue w/ expression after binding [THIS IS THE "k1" CONTINUATION OF THE EXPRESSION ABOVE]
    cps_defn[2].exp[0].exp[2].exp[1] = scm_list(3);
    cps_defn[2].exp[0].exp[2].exp[1].exp[0] = symconst::lambda;
    cps_defn[2].exp[0].exp[2].exp[1].exp[1] = scm_list(1,"ignore"); // result of set!
    cps_defn[2].exp[0].exp[2].exp[1].exp[2] = scm_list(2);

    // Pass <rest_exp> of expression to the topmost continuation if CPS-ATOMIC
    if(data_is_cps_atomic(rest_exp)) {
      cps_defn[2].exp[0].exp[2].exp[1].exp[2].exp[0] = cps_defn[1].exp[0];
      cps_defn[2].exp[0].exp[2].exp[1].exp[2].exp[1] = rest_exp;
    // Else CPS-ify <rest_exp> of expression & pass it the topmost continuation
    } else {
      cps_defn[2].exp[0].exp[2].exp[1].exp[2].exp[0] = generate_fundamental_form_cps(rest_exp,false);
      cps_defn[2].exp[0].exp[2].exp[1].exp[2].exp[1] = cps_defn[1].exp[0];
    }
    return cps_defn;
  }


  // Generates the CPS expression needed to evaluate <rest_exp> after defining new syntax
  scm_list generate_fundamental_form_cps_syn_defn_REST_EXP_continuation(const data& continuation,const data& rest_exp){
    scm_list rest_cont(2);
    // Pass <rest_exp> of expression to the topmost continuation if CPS-ATOMIC
    if(data_is_cps_atomic(rest_exp)) {
      rest_cont[0] = continuation;
      rest_cont[1] = rest_exp;
    // Else CPS-ify <rest_exp> of expression & pass it the topmost continuation
    } else {
      rest_cont[0] = generate_fundamental_form_cps(rest_exp,false);
      rest_cont[1] = continuation;
    }
    return rest_cont;
  }


  // Generates a CPS syntax definition in the middle of a BEGIN or LAMBDA BODY sequence, w/ <rest_exp>
  //   being the remaining expressions AFTER this syntax definition in the sequence
  // PRECONDITION: !rest_exp.empty()
  scm_list generate_mid_seq_cps_syn_defn(const scm_list& defn_exp, const data& rest_exp){
    const bool atomic_syntax_rules = data_is_cps_atomic(defn_exp[2]);
    scm_list cps_defn(3 + atomic_syntax_rules);
    cps_defn[0] = symconst::lambda;
    cps_defn[1] = scm_list(1,generate_unique_cps_hash()); // topmost continuation "k"
    // Atomic Syntax-Rules reduces # of lambdas needed by 1
    if(atomic_syntax_rules) {
      cps_defn[2] = scm_list(3);
      cps_defn[2].exp[0] = symconst::defn_syn;
      cps_defn[2].exp[1] = defn_exp[1];
      cps_defn[2].exp[2] = defn_exp[2];
      cps_defn[3] = generate_fundamental_form_cps_syn_defn_REST_EXP_continuation(cps_defn[1].exp[0],rest_exp);
      return cps_defn;
    }
    cps_defn[2] = scm_list(2);
    cps_defn[2].exp[0] = generate_fundamental_form_cps(defn_exp[2],false);
    cps_defn[2].exp[1] = scm_list(4);
    cps_defn[2].exp[1].exp[0] = symconst::lambda;
    cps_defn[2].exp[1].exp[1] = scm_list(1,generate_unique_cps_hash()); // "syntax-object"

    cps_defn[2].exp[1].exp[2] = scm_list(3);
    cps_defn[2].exp[1].exp[2].exp[0] = symconst::defn_syn;
    cps_defn[2].exp[1].exp[2].exp[1] = defn_exp[1];
    cps_defn[2].exp[1].exp[2].exp[2] = cps_defn[2].exp[1].exp[1].exp[0];
    cps_defn[2].exp[1].exp[3] = generate_fundamental_form_cps_syn_defn_REST_EXP_continuation(cps_defn[1].exp[0],rest_exp);
    return cps_defn;
  }


  template<scm_list(*generate_begin_defn)(const scm_list&,const data&)>
  scm_list generate_begin_mid_seq_defn(const scm_list& defn_exp,const data& begin){
    if(begin.exp.size() == 3 && data_is_cps_atomic(begin.exp[2])) {
      return generate_begin_defn(defn_exp,begin.exp[2]);
    } else {
      scm_list begin_tail(begin.exp.size()-1);
      begin_tail[0] = symconst::begin;
      std::copy(begin.exp.begin()+2,begin.exp.end(),begin_tail.begin()+1);
      return generate_begin_defn(defn_exp,begin_tail);
    }
  }


  scm_list convert_proc_defn_to_lambda_defn(const scm_list& defn_exp)noexcept{
    scm_list lambda_defn(3);
    lambda_defn[0] = symconst::define;
    lambda_defn[1] = defn_exp[1].exp[0]; // proc name
    lambda_defn[2] = scm_list(defn_exp.size());
    lambda_defn[2].exp[0] = symconst::lambda;
    lambda_defn[2].exp[1] = scm_list(defn_exp[1].exp.begin()+1,defn_exp[1].exp.end()); // args
    std::copy(defn_exp.begin()+2,defn_exp.end(),lambda_defn[2].exp.begin()+2); // append body to lambda
    return lambda_defn;
  }


  void get_cps_lambda_body(const scm_list& lambda_exp, scm_list& lambda_cps){
    lambda_cps[2] = scm_list(2); // lambda body
    // If single-expression body, NO NEED FOR "BEGIN"
    if(lambda_exp.size() == 3) { 
      if(data_is_cps_atomic(lambda_exp[2])) {
        lambda_cps[2].exp[0] = *lambda_cps[1].exp.rbegin(); // DYNAMIC CONTINUATION
        lambda_cps[2].exp[1] = lambda_exp[2];
      } else {
        lambda_cps[2].exp[0] = generate_fundamental_form_cps(lambda_exp[2],false);
        lambda_cps[2].exp[1] = *lambda_cps[1].exp.rbegin(); // DYNAMIC CONTINUATION
      }
    // If multi-expression body, WRAP W/ "BEGIN"
    } else {
      scm_list begin_recur(lambda_exp.size()-1);
      begin_recur[0] = symconst::begin;
      std::copy(lambda_exp.begin()+2,lambda_exp.end(),begin_recur.begin()+1);
      lambda_cps[2] = scm_list(2);
      lambda_cps[2].exp[0] = generate_fundamental_form_cps(begin_recur,false);
      lambda_cps[2].exp[1] = *lambda_cps[1].exp.rbegin();
    }
  }


  scm_list get_cps_IF_consequent(const data& code, const data& continuation){
    scm_list consequent(2);
    if(data_is_cps_atomic(code.exp[2])) { // (k <atomic-consequent>)
      consequent[0] = continuation;
      consequent[1] = code.exp[2];
    } else { // ((cps <consequent>) k)
      consequent[0] = generate_fundamental_form_cps(code.exp[2],false);
      consequent[1] = continuation;
    }
    return consequent;
  }


  // PRECONDITION: Assumes IF alternative exists
  scm_list get_cps_IF_alternative(const data& code, const data& continuation){
    scm_list alternative(2);
    if(data_is_cps_atomic(code.exp[3])) { // (k <atomic-alternative>)
      alternative[0] = continuation;
      alternative[1] = code.exp[3];
    } else { // ((cps <alternative>) k)
      alternative[0] = generate_fundamental_form_cps(code.exp[3],false);
      alternative[1] = continuation;
    }
    return alternative;
  }


  // NOTE: <topmost_call> signals to optimize the result prior returning
  scm_list generate_fundamental_form_cps(const data& code,const bool topmost_call){
    // ATOMIC / SYNTAX-RULES / QUOTE
    if(data_is_cps_atomic(code)) {
      scm_list lambda(3);
      lambda[0] = symconst::lambda;
      lambda[1] = scm_list(1,generate_unique_cps_hash()); // "k"
      lambda[2] = scm_list(2);
      lambda[2].exp[0] = lambda[1].exp[0];
      lambda[2].exp[1] = code;
      return lambda;

    // DEFINE-SYNTAX
    } else if(is_tagged_list(code.exp,symconst::defn_syn)) {
      confirm_valid_define_syntax(code.exp);
      scm_list cps_defn_syn(3);
      cps_defn_syn[0] = symconst::lambda;
      cps_defn_syn[1] = scm_list(1,generate_unique_cps_hash()); // "k"
      cps_defn_syn[2] = scm_list(2);
      if(data_is_cps_atomic(code.exp[2])) {
        cps_defn_syn[2].exp[0] = cps_defn_syn[1].exp[0];
        cps_defn_syn[2].exp[1] = scm_list(3);
        cps_defn_syn[2].exp[1].exp[0] = symconst::defn_syn;
        cps_defn_syn[2].exp[1].exp[1] = code.exp[1];
        cps_defn_syn[2].exp[1].exp[2] = code.exp[2];
        return cps_defn_syn;
      }
      cps_defn_syn[2].exp[0] = generate_fundamental_form_cps(code.exp[2],false);
      cps_defn_syn[2].exp[1] = scm_list(3);
      cps_defn_syn[2].exp[1].exp[0] = symconst::lambda;
      cps_defn_syn[2].exp[1].exp[1] = scm_list(1,generate_unique_cps_hash()); // "syntax-object"

      cps_defn_syn[2].exp[1].exp[2] = scm_list(2);
      cps_defn_syn[2].exp[1].exp[2].exp[0] = cps_defn_syn[1].exp[0];
      cps_defn_syn[2].exp[1].exp[2].exp[1] = scm_list(3);
      cps_defn_syn[2].exp[1].exp[2].exp[1].exp[0] = symconst::defn_syn;
      cps_defn_syn[2].exp[1].exp[2].exp[1].exp[1] = code.exp[1];
      cps_defn_syn[2].exp[1].exp[2].exp[1].exp[2] = cps_defn_syn[2].exp[1].exp[1].exp[0];
      if(topmost_call) optimize_CPS_code_generation(cps_defn_syn);
      return cps_defn_syn;

    // SET!
    } else if(is_tagged_list(code.exp,symconst::set)) {
      confirm_valid_assignment(code.exp);
      scm_list lambda(3);
      lambda[0] = symconst::lambda;
      lambda[1] = scm_list(1,generate_unique_cps_hash()); // "k"
      lambda[2] = scm_list(2);
      if(data_is_cps_atomic(code.exp[2])) {
        lambda[2].exp[0] = lambda[1].exp[0];
        lambda[2].exp[1] = code.exp;
        return lambda;
      }
      lambda[2].exp[0] = generate_fundamental_form_cps(code.exp[2],false);
      lambda[2].exp[1] = scm_list(3);
      lambda[2].exp[1].exp[0] = symconst::lambda;
      lambda[2].exp[1].exp[1] = scm_list(1,generate_unique_cps_hash()); // "value"
      lambda[2].exp[1].exp[2] = scm_list(2);
      lambda[2].exp[1].exp[2].exp[0] = lambda[1].exp[0];
      lambda[2].exp[1].exp[2].exp[1] = scm_list(3);
      lambda[2].exp[1].exp[2].exp[1].exp[0] = symconst::set;
      lambda[2].exp[1].exp[2].exp[1].exp[1] = code.exp[1];
      lambda[2].exp[1].exp[2].exp[1].exp[2] = lambda[2].exp[1].exp[1].exp[0];
      if(topmost_call) optimize_CPS_code_generation(lambda);
      return lambda;

    // BEGIN
    } else if(is_tagged_list(code.exp,symconst::begin)) {
      scm_list lambda(3);
      lambda[0] = symconst::lambda;
      lambda[1] = scm_list(1,generate_unique_cps_hash()); // "k"
      lambda[2] = scm_list(2);
      // 0 Args
      if(code.exp.size() == 1) {
        lambda[2].exp[0] = lambda[1].exp[0];
        lambda[2].exp[1] = scm_list(1,"void");
      // 1 Arg
      } else if(code.exp.size() == 2) {
        if(data_is_cps_atomic(code.exp[1])) {
          lambda[2].exp[0] = lambda[1].exp[0];
          lambda[2].exp[1] = code.exp[1];
        } else {
          lambda[2].exp[0] = generate_fundamental_form_cps(code.exp[1],false);
          lambda[2].exp[1] = lambda[1].exp[0];
        }
      // N Args
      } else {
        bool rec_idx = !data_is_cps_atomic(code.exp[1]);
        if(rec_idx) {
          if(is_tagged_list(code.exp[1].exp,symconst::define)) {
            if(code.exp[1].exp[1].is_type(types::exp)) // Convert procedure definition to a lambda defn
              lambda[2].exp[0] = generate_begin_mid_seq_defn<generate_mid_seq_cps_var_defn>(
                                                             convert_proc_defn_to_lambda_defn(code.exp[1].exp),code.exp);
            else
              lambda[2].exp[0] = generate_begin_mid_seq_defn<generate_mid_seq_cps_var_defn>(code.exp[1].exp,code.exp);
            lambda[2].exp[1] = lambda[1].exp[0];
            if(topmost_call) optimize_CPS_code_generation(lambda);
            return lambda;
          } else if(is_tagged_list(code.exp[1].exp,symconst::defn_syn)) {
            lambda[2].exp[0] = generate_begin_mid_seq_defn<generate_mid_seq_cps_syn_defn>(code.exp[1].exp,code.exp);
            lambda[2].exp[1] = lambda[1].exp[0];
            if(topmost_call) optimize_CPS_code_generation(lambda);
            return lambda;
          } else {
            lambda[2].exp[0] = generate_fundamental_form_cps(code.exp[1],false);
          }
        } else {
          lambda[2].exp[1] = code.exp[1];
        }
        lambda[2].exp[rec_idx] = scm_list(3);
        lambda[2].exp[rec_idx].exp[0] = symconst::lambda;
        lambda[2].exp[rec_idx].exp[1] = scm_list(1,"ignore");
        lambda[2].exp[rec_idx].exp[2] = scm_list(2);
        if(code.exp.size() == 3 && data_is_cps_atomic(code.exp[2])) { // 2 ARGS, THE LAST BEING CPS-ATOMIC
          lambda[2].exp[rec_idx].exp[2].exp[0] = lambda[1].exp[0];
          lambda[2].exp[rec_idx].exp[2].exp[1] = code.exp[2];
        } else { // 2+ ARGS, IF 2, 2ND != CPS-ATOMIC
          data begin_recur(scm_list(code.exp.size()-1));
          begin_recur.exp[0] = symconst::begin;
          std::copy(code.exp.begin()+2, code.exp.end(), begin_recur.exp.begin()+1);
          lambda[2].exp[rec_idx].exp[2].exp[0] = generate_fundamental_form_cps(begin_recur,false);
          lambda[2].exp[rec_idx].exp[2].exp[1] = lambda[1].exp[0];
        }
      }
      if(topmost_call) optimize_CPS_code_generation(lambda);
      return lambda;

    // LAMBDA
    } else if(is_tagged_list(code.exp,symconst::lambda)) {
      confirm_valid_lambda(code.exp);
      scm_list lambda(3);
      lambda[0] = symconst::lambda;
      lambda[1] = scm_list(1,generate_unique_cps_hash()); // "k"
      lambda[2] = scm_list(2);
      lambda[2].exp[0] = lambda[1].exp[0];

      lambda[2].exp[1] = scm_list(3);
      lambda[2].exp[1].exp[0] = symconst::lambda;
      if(code.exp[1].exp.empty()) { // ARGLESS
        lambda[2].exp[1].exp[1] = scm_list(2);
        lambda[2].exp[1].exp[1].exp[0] = symconst::sentinel_arg;
        lambda[2].exp[1].exp[1].exp[1] = generate_unique_cps_hash(); // "dyn-k"
      } else { // N ARGS
        const auto param_len = code.exp[1].exp.size();
        lambda[2].exp[1].exp[1] = scm_list(param_len+1);
        std::copy(code.exp[1].exp.begin(),code.exp[1].exp.end(),lambda[2].exp[1].exp[1].exp.begin());
        lambda[2].exp[1].exp[1].exp[param_len] = generate_unique_cps_hash(); // "dyn-k"
      }
      get_cps_lambda_body(code.exp,lambda[2].exp[1].exp);
      if(topmost_call) optimize_CPS_code_generation(lambda);
      return lambda;

    // IF
    } else if(is_tagged_list(code.exp,symconst::if_t)) {
      confirm_valid_if(code.exp);
      scm_list lambda(3);
      lambda[0] = symconst::lambda;
      lambda[1] = scm_list(1,generate_unique_cps_hash()); // "k"
      // Atomic IF test
      if(data_is_cps_atomic(code.exp[1])) { 
        lambda[2] = scm_list(code.exp.size());
        lambda[2].exp[0] = symconst::if_t;
        lambda[2].exp[1] = code.exp[1];
        lambda[2].exp[2] = get_cps_IF_consequent(code,lambda[1].exp[0]);
        if(code.exp.size() > 3) // Has IF alternative
          lambda[2].exp[3] = get_cps_IF_alternative(code,lambda[1].exp[0]);
        if(topmost_call) optimize_CPS_code_generation(lambda);
        return lambda;
      }
      // Non-Atomic IF test
      lambda[2] = scm_list(2);
      lambda[2].exp[0] = generate_fundamental_form_cps(code.exp[1],false);
      lambda[2].exp[1] = scm_list(3);
      lambda[2].exp[1].exp[0] = symconst::lambda;
      lambda[2].exp[1].exp[1] = scm_list(1,generate_unique_cps_hash()); // "test-result"
      lambda[2].exp[1].exp[2] = scm_list(code.exp.size());
      lambda[2].exp[1].exp[2].exp[0] = symconst::if_t;
      lambda[2].exp[1].exp[2].exp[1] = lambda[2].exp[1].exp[1].exp[0];
      lambda[2].exp[1].exp[2].exp[2] = get_cps_IF_consequent(code,lambda[1].exp[0]);
      if(code.exp.size() > 3) // Has IF alternative
        lambda[2].exp[1].exp[2].exp[3] = get_cps_IF_alternative(code,lambda[1].exp[0]);
      if(topmost_call) optimize_CPS_code_generation(lambda);
      return lambda;

    // DEFINE
    } else if(is_tagged_list(code.exp,symconst::define)) {
      confirm_valid_definition(code.exp);
      if(!code.exp[1].is_type(types::exp)) { // DEFINING VARIABLE
        scm_list cps_defn(3);
        cps_defn[0] = symconst::lambda;
        cps_defn[1] = scm_list(1,generate_unique_cps_hash()); // topmost continuation "k"
        cps_defn[2] = scm_list(2);
        cps_defn[2].exp[1] = G::FALSE_DATA_BOOLEAN; // initially bind defined symbol to #f
        cps_defn[2].exp[0] = scm_list(3);
        cps_defn[2].exp[0].exp[0] = symconst::lambda;
        cps_defn[2].exp[0].exp[1] = scm_list(1,code.exp[1]); // defined symbol as an argument, and bind via set! (below)
        cps_defn[2].exp[0].exp[2] = get_cps_defn_set_procedure(cps_defn[1].exp[0],code.exp[1],code.exp[2]);
        if(topmost_call) optimize_CPS_code_generation(cps_defn);
        return cps_defn;
      } else { // DEFINING PROCEDURE
        auto cps_defn = generate_fundamental_form_cps(convert_proc_defn_to_lambda_defn(code.exp),false); // cps lambda defn
        if(topmost_call) optimize_CPS_code_generation(cps_defn);
        return cps_defn;
      }

    // APPLICATION
    } else {
      if(code.exp.empty())
        THROW_ERR("'scm->cps CAN'T EVAL AN EMPTY EXPRESSION!" << EXP_ERR(code.exp));
      // Applications are expanded @eval-time (accounts for expanding macros)
      if(code.exp[0].is_type(types::sym) && is_HEIST_cpp_derived_special_form(code.exp[0].sym))
        return code.exp; // Don't tag applications of HEIST's C++ derived forms
      // Tag any other application
      bool no_tag = !is_cps_application(code.exp);
      scm_list app(code.exp.size()+no_tag);
      if(no_tag) app[0] = symconst::cps_app_tag;
      std::copy(code.exp.begin(),code.exp.end(),app.begin()+no_tag);
      return app;
    }
  }


  // Process convert & eval exp in CPS (Continuation Passing Style)
  exe_type analyze_scm_cps(scm_list& exp) {
    if(exp.size() == 1)
      THROW_ERR("'scm->cps expects at least 1 expression: (scm->cps <exp-1> ... <exp-N>)"<<EXP_ERR(exp));
    if(exp.size() == 2) {
      return scm_analyze(generate_fundamental_form_cps(exp[1]),false,true);
    } else { // Wrap multi-statement transforms in a BEGIN
      scm_list begin(exp.size());
      begin[0] = symconst::begin;
      std::copy(exp.begin()+1,exp.end(),begin.begin()+1);
      return scm_analyze(generate_fundamental_form_cps(begin),false,true);
    }
  }


  // Returns the generated CPS form of exp as a quoted list of data
  exe_type analyze_cps_quote(scm_list& exp,const bool cps_block) {
    if(exp.size() == 1)
      THROW_ERR("'cps-quote expects at least 1 expression: (cps-quote <exp-1> ... <exp-N>)"<<EXP_ERR(exp));
    scm_list quoted_cps(2);
    quoted_cps[0] = symconst::quote;
    if(exp.size() == 2) {
      quoted_cps[1] = generate_fundamental_form_cps(exp[1]);
    } else { // Wrap multi-statement transforms in a BEGIN
      scm_list begin(exp.size());
      begin[0] = symconst::begin;
      std::copy(exp.begin()+1,exp.end(),begin.begin()+1);
      quoted_cps[1] = generate_fundamental_form_cps(begin);
    }
    if(cps_block) return scm_analyze(generate_fundamental_form_cps(quoted_cps),false,true);
    return scm_analyze(std::move(quoted_cps));
  }

  /******************************************************************************
  * REPRESENTING MACRO SYNTACTIC EXTENSIONS -- GENERAL PARSING HELPER FUNCTIONS
  ******************************************************************************/

  // (define-syntax <label>
  //   (syntax-rules (<keywords>)
  //     ((<pattern>) <template>)
  //     ((<pattern>) <template>)
  //     ((<pattern>) <template>))
  //
  // (let-syntax ((<label-1> <syntax-rules-1>)
  //              ...
  //              (<label-N> <syntax-rules-N>))
  //             <body>)
  //
  // => token strings in the <keywords> list allow those symbols, and only 
  //    those symbols, in places where they are mentioned w/in <pattern>s

  // EXAMPLES:
  // ; Redefining λ to expand to 'lambda
  // (define-syntax λ
  //   (syntax-rules ()
  //     ((λ () body ...) (lambda () body ...))
  //     ((λ (arg ...) body ...) (lambda (arg ...) body ...))))
  //
  // ; Macro simulating variadic multiplication if '* were a binary operation
  // (define-syntax multiply-all 
  //   (syntax-rules ()
  //     ((multiply-all) 1)
  //     ((multiply-all a) a)
  //     ((multiply-all a b ...) (* a (multiply-all b ...)))))


  // 0. Each macro Id entry in <MACRO_ID_TABLE> (ie <MACRO_ID_VAR_TABLE.first>) represents 
  //      an instance of a macro identifier [in the pattern] to be expanded [in the template]
  //      into a value(s) [from the matched expression].
  // 1. <macId_name> gets the symbolic name of the identifier in question.
  // 2. <macId_val_pos_map> gets a vector of value-position pairs
  //    - "values" are those which the "name" should expand into
  //      * >1 "value" indicates a variadic expansion of the "name"
  //    - a "position" is a vector of indices to locate the "value" in the 
  //      pattern-matched expression (multiple idxs to traverse the nested vectors)
  // 3. <macId_values> returns a flatmap of the values in <macId_val_pos_map>
  //    - this is maintained alongside <macId_val_pos_map> in a seperate structure 
  //      internally in order to have fast reads (rather than generating a new instance
  //      from <macId_val_pos_map> each time)

  // 0. <VARARG_POSITIONS> (ie <MACRO_ID_VAR_TABLE.second>) tracks all of the position idx 
  //      vectors of '...' symbols found in the macro pattern (1st idx of each row in the matrix
  //      is detracted by 1 to disregard the intitial '_' & line up w/ the values of <MACRO_ID_TABLE>'s
  //      idxs of values [from <macId_val_pos_map>] in the matched expression)

  // 0. <MacroId_varArg_posPair> Holds 2 vectors, each holding nested vectors of position idxs 
  //    (of an elt w/in nested vectors) for:
  //    - .first: the current macro Id being parsed
  //    - .second: the current variadic '...' symbol being detected

  using macId_position_t = std::vector<size_type>;
  using MacroId_varArg_posPair = std::pair<macId_position_t,macId_position_t>;
  using MACRO_ID_VAL_POS_PAIR = std::pair<scm_list,macId_position_t>;
  using MACRO_ID_VAL_POS_PAIRS = std::vector<MACRO_ID_VAL_POS_PAIR>;
  using MACRO_ID = std::tuple<sym_type,MACRO_ID_VAL_POS_PAIRS,scm_list>;
  using MACRO_ID_TABLE = std::vector<MACRO_ID>;
  using VARARG_POSITIONS = std::vector<macId_position_t>;
  using MACRO_ID_VAR_TABLE = std::pair<MACRO_ID_TABLE,VARARG_POSITIONS>;

  // Node elt in the variadic expansion process
  struct macro_expansion_node {
    sym_type id_name;                             // Identifier name being expanded
    std::vector<macro_expansion_node> children;   // Variadic subgroup children
    std::vector<macId_position_t> positions;      // Position vector(s) of leaf node value(s)
    scm_list values;                              // Leaf node value(s)
    bool is_variadic = false;                     // Determines whether value corresponds to ...
    bool is_leaf()const{return children.empty();} // Determines valid elt: true ? value : children
    macro_expansion_node(const sym_type& name, const bool& variadic_node = false) : id_name(name), 
                                                                                    is_variadic(variadic_node) {}
  };

  // - Topmost node (ie node of in <MACRO_EXPANSION_TREE> is a symbol w/ children)
  //   * NON-VARIADIC identifiers are repn'd by nodes that are both ROOTS & a LEAF
  // - Leaves are ultimate values to be expanded into
  // - Intermediate nodes repn any multi-layered variadic expansion [ie (a ...) ...]
  using MACRO_EXPANSION_TREES_t = std::vector<macro_expansion_node>;


  // Accessors
  sym_type& macId_name(MACRO_ID& macId_instance)                      noexcept{return std::get<0>(macId_instance);}
  MACRO_ID_VAL_POS_PAIRS& macId_val_pos_map(MACRO_ID& macId_instance) noexcept{return std::get<1>(macId_instance);}
  scm_list& macId_values(MACRO_ID& macId_instance)                    noexcept{return std::get<2>(macId_instance);}


  // Confirm whether the given word is a keyword
  bool is_keyword(const sym_type& word, const frame_vars& keywords)noexcept{
    return std::find(keywords.begin(), keywords.end(), word) != keywords.end();
  }


  bool data_is_ellipsis(const data& d)noexcept{
    return d.is_type(types::sym) && d.sym == symconst::ellipsis;
  }


  // Primitive symbolic literals: #t #f '()
  bool is_primitive_symbolic_literal(const data& obj)noexcept{
    return obj.is_type(types::sym) && 
      (obj.sym == symconst::true_t || obj.sym == symconst::false_t || obj.sym == symconst::emptylist);
  }


  // Confirm <pat_entity> is a potential macro identifier
  // => WARNING: Doesn't check for whether is a keyword (used _only_ in expansion)!
  bool is_symbolic_macro_identifier(const data& pat_entity)noexcept{
    return pat_entity.is_type(types::sym) && !is_primitive_symbolic_literal(pat_entity) &&
      pat_entity.sym != symconst::ellipsis && pat_entity.sym != symconst::sentinel_arg;
  }


  // Confirm <pat_entity> is a macro argument (non-keyword) name
  bool is_macro_argument_label(const data& pat_entity, const frame_vars& keywords)noexcept{
    return is_symbolic_macro_identifier(pat_entity) && !is_keyword(pat_entity.sym, keywords);
  }


  // Confirm whether 'pattern' is argless but was given 'args' (or vise versa)
  bool incompatible_void_arg_use(const scm_list& pattern, const scm_list& args)noexcept{
    bool pattern_is_argless = pattern.size() == 2 && pattern[1].is_type(types::sym) && 
                              pattern[1].sym == symconst::sentinel_arg;
    bool args_is_argless    = args.size()==1 && data_is_the_SENTINEL_VAL(args[0]);
    return pattern_is_argless ^ args_is_argless;
  }


  // Associate a pattern's macro identifier to the objects it will expand into
  void register_macro_identifier_expansion_values(MACRO_ID_TABLE& ID_TO_VAL_MAP,const sym_type& id_name, 
                                                  scm_list&& expansion_values,  const macId_position_t& macId_pos_vector)noexcept{
    for(auto& id : ID_TO_VAL_MAP) {
      if(macId_name(id) == id_name) {
        // Add to the flatmap of values
        auto& id_values = macId_values(id);
        id_values.insert(id_values.end(), expansion_values.begin(), expansion_values.end());
        // Add to the map of values-to-positions
        auto& val_pos_map = macId_val_pos_map(id);
        val_pos_map.push_back(std::make_pair(expansion_values,macId_pos_vector));
        return;
      }
    }
    MACRO_ID_VAL_POS_PAIRS val_pos_pairs(1,std::make_pair(expansion_values,macId_pos_vector));
    ID_TO_VAL_MAP.push_back(std::make_tuple(id_name,val_pos_pairs,expansion_values));
  }

  /******************************************************************************
  * REPRESENTING MACRO SYNTACTIC EXTENSIONS -- PATTERN MATCHING HELPER FUNCTIONS
  ******************************************************************************/

  bool compare_pattern_args_exp_match(const scm_list&,const scm_list&,const frame_vars&,MACRO_ID_VAR_TABLE&,
                                                      const size_type&,MacroId_varArg_posPair)noexcept;
  

  // Determine whether only the par's elt OR only the arg's elt is a keyword
  bool mismatched_keywords(const data& pat_elt, const data& arg_elt, const frame_vars& keywords)noexcept{
    return (pat_elt.is_type(types::sym) && is_keyword(pat_elt.sym,keywords)) ^
           (arg_elt.is_type(types::sym) && is_keyword(arg_elt.sym,keywords));
  }


  // Confirm given 2 incompatible atomics
  bool mismatched_atomics(const data& pat_entity, const data& arg_entity)noexcept{
    if(is_primitive_symbolic_literal(pat_entity))
       return !is_primitive_symbolic_literal(arg_entity) || pat_entity.sym != arg_entity.sym;
    if(pat_entity.is_type(types::sym) || pat_entity.is_type(types::exp)) return false;
    if(pat_entity.type != arg_entity.type)                               return true;
    if(pat_entity.is_type(types::par))
      return !prm_compare_PAIRs(pat_entity.par, arg_entity.par);
    if(pat_entity.is_type(types::vec))
      return !prm_compare_VECTs(pat_entity.vec, arg_entity.vec);
    return !prm_compare_atomic_values(pat_entity, arg_entity, pat_entity.type);
  }


  // Confirm the 2 given pattern/arg elts are mismatched subexpressions
  bool mismatched_subexpressions(const data& pat_elt, const data& arg_elt, const frame_vars& keywords, 
                                 MACRO_ID_VAR_TABLE& MID_VARG_PAIR, MacroId_varArg_posPair macId_varArg_vecs, 
                                 const size_type& args_idx, const size_type& pat_idx)noexcept{
    if(!pat_elt.is_type(types::exp) || !arg_elt.is_type(types::exp)) return true;
    if(pat_elt.exp.empty()) return !arg_elt.exp.empty();
    macId_varArg_vecs.first.push_back(args_idx), macId_varArg_vecs.second.push_back(pat_idx);
    return !compare_pattern_args_exp_match(pat_elt.exp,arg_elt.exp,keywords,MID_VARG_PAIR,0,macId_varArg_vecs);
  }


  // Handle '...' pattern analysis
  bool account_for_pattern_ellipsis_and_return_whether_no_match(const scm_list& args_exp,    size_type& args_idx, const data& pat_obj_prior_ellipsis,
                                                                const size_type& number_args_left_after_variadic, const frame_vars& keywords,
                                                                MACRO_ID_VAR_TABLE& MID_VARG_PAIR,MacroId_varArg_posPair macId_varArg_vecs,
                                                                const size_type& pat_idx)noexcept{
    // Start associating objs based on the first obj prior "..."'s position
    --args_idx;
    // Confirm enough room in <args_exp> for the variadic
    const auto& args_size = args_exp.size();
    if(number_args_left_after_variadic + args_idx > args_size) return true;
    const auto va_objs_end = args_size - number_args_left_after_variadic;
    // Confirm each variadic obj in <args_exp> matches the layout of <pat_obj_prior_ellipsis>
    // Symbol Identifiers may expand to _any_ form
    if(pat_obj_prior_ellipsis.is_type(types::sym)) {
      macId_varArg_vecs.first.push_back(args_idx);
      register_macro_identifier_expansion_values(MID_VARG_PAIR.first,pat_obj_prior_ellipsis.sym,
                                                                     scm_list(args_exp.begin() + args_idx, 
                                                                              args_exp.begin() + va_objs_end),
                                                                     macId_varArg_vecs.first);
      const auto number_of_va_objs_in_args = va_objs_end - args_idx;
      args_idx += number_of_va_objs_in_args - 1; // advance <args_idx> to the last va obj associated
    // Expression Identifiers _may only_ expand into expressions of the same form
    } else {
      for(; args_idx < va_objs_end; ++args_idx)
        if(mismatched_subexpressions(pat_obj_prior_ellipsis,args_exp[args_idx],keywords,MID_VARG_PAIR,macId_varArg_vecs,args_idx,pat_idx))
          return true;
      --args_idx; // move back to the last associated obj (accounts for '++' in loop returning to)
    }
    // Save current position vector for ... identifier
    macId_varArg_vecs.second.push_back(pat_idx+1);
    MID_VARG_PAIR.second.push_back(macId_varArg_vecs.second);
    macId_varArg_vecs.second.pop_back();
    return false;
  }


  // Confirm whether the pattern sub-expression matches the 'args' sub-expression
  bool compare_pattern_args_exp_match(const scm_list& pat_exp, const scm_list& args_exp, const frame_vars& keywords,
                                      MACRO_ID_VAR_TABLE& MID_VARG_PAIR,                 const size_type& pat_idx_start, 
                                                                      MacroId_varArg_posPair macId_varArg_vecs)noexcept{
    // Confirm whether <pat_exp> & <args_exp> match one another
    size_type pat_idx = pat_idx_start, args_idx = 0, args_size = args_exp.size(), pat_size = pat_exp.size();
    for(; pat_idx < pat_size && args_idx < args_size; ++pat_idx, ++args_idx){
      // Check for proper "..." use in the pat_exp definition
      if(data_is_ellipsis(pat_exp[pat_idx])) { // Guarenteed pat_idx > 0 by syntax-rules analysis
        macId_varArg_vecs.second.push_back(pat_idx);
        MID_VARG_PAIR.second.push_back(macId_varArg_vecs.second);
        macId_varArg_vecs.second.pop_back();
        if(account_for_pattern_ellipsis_and_return_whether_no_match(args_exp, args_idx,pat_exp[pat_idx-1], pat_size-pat_idx-1,
                                                                        keywords, MID_VARG_PAIR,macId_varArg_vecs,pat_idx-1)){
          return false;
        }
      // Register the pat_exp's identifier & associated expansion value
      } else if(is_macro_argument_label(pat_exp[pat_idx],keywords)) {
        if(pat_idx+1 == pat_size || !data_is_ellipsis(pat_exp[pat_idx+1])) {
          macId_varArg_vecs.first.push_back(args_idx);
          register_macro_identifier_expansion_values(MID_VARG_PAIR.first,pat_exp[pat_idx].sym,scm_list(1,args_exp[args_idx]),macId_varArg_vecs.first);
          macId_varArg_vecs.first.pop_back();
        }
      // Verify matching subexpressions
      } else if(pat_exp[pat_idx].is_type(types::exp)) {
        if(!args_exp[args_idx].is_type(types::exp) || 
          ((pat_idx+1 == pat_size || !data_is_ellipsis(pat_exp[pat_idx+1])) &&
            mismatched_subexpressions(pat_exp[pat_idx],args_exp[args_idx],keywords,MID_VARG_PAIR,macId_varArg_vecs,args_idx,pat_idx))) {
          return false;
        }
      // Verify literal & keyword use are aligned
      } else if(mismatched_atomics(pat_exp[pat_idx],args_exp[args_idx]) || 
                mismatched_keywords(pat_exp[pat_idx],args_exp[args_idx],keywords)){
        return false;
      }
    }
    // Register the last identifier if variadic portion of expansion @ the end of the pattern & empty in args
    if(pat_idx+1 == pat_size && data_is_ellipsis(pat_exp[pat_idx]) && args_idx == args_size) {
      if(is_macro_argument_label(pat_exp[pat_idx-1],keywords)) {
        macId_varArg_vecs.second.push_back(pat_idx);
        MID_VARG_PAIR.second.push_back(macId_varArg_vecs.second);
        macId_varArg_vecs.second.pop_back();
        macId_varArg_vecs.first.push_back(args_idx-1);
        register_macro_identifier_expansion_values(MID_VARG_PAIR.first,pat_exp[pat_idx-1].sym,scm_list(1,args_exp[args_idx-1]),macId_varArg_vecs.first);
        macId_varArg_vecs.first.pop_back();
        return true;
      }
      return !account_for_pattern_ellipsis_and_return_whether_no_match(args_exp, args_idx, pat_exp[pat_idx-1], pat_size-pat_idx-1, 
                                                                           keywords, MID_VARG_PAIR, macId_varArg_vecs, pat_idx-1);
    }
    // Verify both <pat_exp> & <arg_exp> have been fully iterated
    return pat_idx == pat_size && args_idx == args_size;
  }


  // Confirm the given arg combo matches the given pattern (in terms of layout)
  bool is_pattern_match(const scm_list& args,const frame_vars& keywords,const scm_list& pattern,
                                                   MACRO_ID_VAR_TABLE& MID_VARG_PAIR)noexcept{
    if(incompatible_void_arg_use(pattern,args)) return false;
    MacroId_varArg_posPair macId_varArg_vecs;
    if(!compare_pattern_args_exp_match(pattern,args,keywords,MID_VARG_PAIR,1,macId_varArg_vecs)){
      MID_VARG_PAIR.first.clear();
      MID_VARG_PAIR.second.clear();
      return false;
    }
    return true;
  }


  // Returns whether the given args correspond to the given macro
  bool is_macro_match(const scm_list& args, const frame_mac& mac, size_type& match_idx, 
                                            MACRO_ID_VAR_TABLE& MID_VARG_PAIR)noexcept{
    for(size_type i = 0, n = mac.patterns.size(); i < n; ++i) {
      if(is_pattern_match(args, mac.keywords, mac.patterns[i], MID_VARG_PAIR)) {
        match_idx = i;
        return true;
      }
    }
    return false;
  }

  /******************************************************************************
  * MACRO SYNTACTIC EXTENSIONS -- EXPANSION HELPER FUNCTIONS
  ******************************************************************************/

  // Recursively prints <MACRO_EXPANSION_TREES> children subgroups
  void recur_stringify_MACRO_EXPANSION_TREES(const MACRO_EXPANSION_TREES_t& children,scm_string& buffer)noexcept{
    for(const auto& child : children) {
      if(!child.is_variadic) {
        buffer += "NON-VARIADIC = " + data(child.values).write() + ',';
      } else if(child.is_leaf()) {
        buffer += data(child.values).write() + ',';
      } else {
        buffer += '[';
        recur_stringify_MACRO_EXPANSION_TREES(child.children,buffer);
        buffer += ']';
      }
    }
  }

  // Stringifies <MACRO_EXPANSION_TREES> contents
  scm_string stringify_MACRO_EXPANSION_TREES(const MACRO_EXPANSION_TREES_t& MACRO_EXPANSION_TREES)noexcept{
    scm_string buffer("     ======================\n     MACRO_EXPANSION_TREES:\n     ");
    // for each tree
    for(const auto& tree : MACRO_EXPANSION_TREES) {
      buffer += "  " + tree.id_name + ": ";
      // print leaves immediately
      if(!tree.is_variadic) {
        buffer += "NON-VARIADIC = " + data(tree.values).write() + "\n     ";
      // recursively print subgroups
      } else if(tree.is_leaf()) {
        buffer += "LEAF: " + data(tree.values).write() + "\n     ";
      } else {
        buffer += '[';
        recur_stringify_MACRO_EXPANSION_TREES(tree.children,buffer);
        buffer += "]\n     ";
      }
    }
    return buffer + "======================";
  }


  // Generate a unique hashed id_name for the expanded symbol
  scm_string hash_macro_expansion_identifier(const scm_string& id_name,const bool& finished_expanding = false)noexcept{
    static size_type IDX_1 = 0, IDX_2 = 0;
    if(finished_expanding) {
      IDX_1 = IDX_2 = 0;
      return "";
    } else {
      if(IDX_1 != G::MAX_SIZE_TYPE)
        return id_name + '-' + std::to_string(IDX_2) + '-' + std::to_string(IDX_1++);
      return id_name + '-' + std::to_string(++IDX_2) + '-' + std::to_string(IDX_1++);
    }
  }


  // Changes all <id_name> in <id_node> & below to be <tagged_symbol>
  void propagate_new_tagged_identifier_name(const scm_string& tagged_symbol,macro_expansion_node& id_node)noexcept{
    id_node.id_name = tagged_symbol;
    if(id_node.is_leaf()) return;
    for(auto& child : id_node.children)
      propagate_new_tagged_identifier_name(tagged_symbol,child);
  }


  // Get idx of <id_name> in <MACRO_EXPANSION_TREES>. Returns <G::MAX_SIZE_TYPE> if not found.
  size_type find_macro_identifier_leaf_index(const MACRO_EXPANSION_TREES_t& MACRO_EXPANSION_TREES,const sym_type& id_name)noexcept{
    for(size_type i = 0, n = MACRO_EXPANSION_TREES.size(); i < n; ++i)
      if(MACRO_EXPANSION_TREES[i].id_name == id_name) return i;
    return G::MAX_SIZE_TYPE;
  }


  // Expand level-1 ids, tag all nested variadic ids, wrench up tagged children of nested variadic ids
  void tag_and_expand_identifiers_while_wrenching_up_children(const size_type expansion_No, scm_list& expansions,
                                                                    MACRO_EXPANSION_TREES_t& MACRO_EXPANSION_TREES)noexcept{
    for(size_type i = 0, n = expansions.size(); i < n; ++i) {
      if(expansions[i].is_type(types::exp)) {
        tag_and_expand_identifiers_while_wrenching_up_children(expansion_No,expansions[i].exp,MACRO_EXPANSION_TREES);
      } else if(is_symbolic_macro_identifier(expansions[i])) {
        auto val_idx = find_macro_identifier_leaf_index(MACRO_EXPANSION_TREES,expansions[i].sym);
        if(val_idx == G::MAX_SIZE_TYPE) continue; // symbol != variadic macro identifier
        // Splice in level-1 (non-nested) ... value
        if(MACRO_EXPANSION_TREES[val_idx].is_leaf()) {
          expansions[i] = MACRO_EXPANSION_TREES[val_idx].values[expansion_No];
        // Tag nested ... identifier to be expanded further, & wrench up the associated child node as a new (tagged) root
        } else {
          auto tagged_symbol = hash_macro_expansion_identifier(expansions[i].sym);
          expansions[i].sym = tagged_symbol; // tag symbol
          MACRO_EXPANSION_TREES.push_back(MACRO_EXPANSION_TREES[val_idx].children[expansion_No]); // wrench up child
          propagate_new_tagged_identifier_name(tagged_symbol,*MACRO_EXPANSION_TREES.rbegin()); // tag up wrenched child
        }
      }
    }
  }


  // <verify_all_identifiers_have_same_variadic_length> helper
  void confirm_identifier_variadic_length_is_consistent(size_type& total_expansions,  const scm_list& exp,
                                                        const scm_list& expanded_exp, const scm_list& args,
                                                        const sym_type& name,         const size_type& result, 
                                                        const MACRO_EXPANSION_TREES_t& MACRO_EXPANSION_TREES){
    if(total_expansions == G::MAX_SIZE_TYPE) {
      total_expansions = result;
    } else if(total_expansions != result && result != G::MAX_SIZE_TYPE) {
      THROW_ERR("'syntax-rules Different variadic identifiers can't expand in the same template expression!"
        "\n     Length 1 = " << total_expansions << ", Length 2 = " << result << 
        "\n     In subexpression: [ " << exp << " ]"
        "\n     Of template expansion: [ " << expanded_exp << " ]\n" 
        << stringify_MACRO_EXPANSION_TREES(MACRO_EXPANSION_TREES) << FCN_ERR(name,args));
    }
  }


  // Returns the length that the identifiers match (throw error if any are off)
  size_type verify_all_identifiers_have_same_variadic_length(const scm_list& args, const sym_type& name, const scm_list& exp, 
                                                             const scm_list& expanded_exp, const MACRO_EXPANSION_TREES_t& MACRO_EXPANSION_TREES){
    size_type total_expansions = G::MAX_SIZE_TYPE;
    for(auto& elt : exp) {
      if(elt.is_type(types::exp)) {
        confirm_identifier_variadic_length_is_consistent(total_expansions,exp,expanded_exp,args,name,
          verify_all_identifiers_have_same_variadic_length(args,name,elt.exp,expanded_exp,MACRO_EXPANSION_TREES),
          MACRO_EXPANSION_TREES);
      } else if(is_symbolic_macro_identifier(elt)) {
        auto val_idx = find_macro_identifier_leaf_index(MACRO_EXPANSION_TREES,elt.sym);
        if(val_idx == G::MAX_SIZE_TYPE) continue; // symbol != variadic macro identifier
        confirm_identifier_variadic_length_is_consistent(total_expansions,exp,expanded_exp,args,name,
          MACRO_EXPANSION_TREES[val_idx].is_leaf() ? MACRO_EXPANSION_TREES[val_idx].values.size() : 
                                                     MACRO_EXPANSION_TREES[val_idx].children.size(),
          MACRO_EXPANSION_TREES);
      }
    }
    return total_expansions;
  }


  // Non-Variadics have been expanded, expand all (possibly nested) variadics identifiers
  // NOTE: Traverses in POST-ORDER!
  void expand_macro_variadic_identifiers(const scm_list& args, const sym_type& name, scm_list& expanded_exp,
                                                               MACRO_EXPANSION_TREES_t& MACRO_EXPANSION_TREES){
    for(size_type i = 0; i < expanded_exp.size(); ++i) {
      if(i+1 < expanded_exp.size() && data_is_ellipsis(expanded_exp[i+1])) {
        // Expand variadic symbolic identifer immediately (no ctoring of any expression)
        if(is_symbolic_macro_identifier(expanded_exp[i])) {
          auto val_idx = find_macro_identifier_leaf_index(MACRO_EXPANSION_TREES,expanded_exp[i].sym);
          if(val_idx == G::MAX_SIZE_TYPE) continue; // symbol != variadic macro identifier
          // confirm expanding into a non-nested variadic identifier
          if(!MACRO_EXPANSION_TREES[val_idx].is_leaf())
            THROW_ERR("'syntax-rules Misplaced \"...\" after improper non-nested variadic identifier [ " 
              << expanded_exp[i].sym << " ]\n     in [ " << expanded_exp << " ]\n"
              << stringify_MACRO_EXPANSION_TREES(MACRO_EXPANSION_TREES) << FCN_ERR(name,args));
          // erase the identifier & "...", then insert the variadic expansion values
          auto& expansions = MACRO_EXPANSION_TREES[val_idx].values;
          expanded_exp.erase(expanded_exp.begin()+i,expanded_exp.begin()+i+2);
          expanded_exp.insert(expanded_exp.begin()+i,expansions.begin(),expansions.end());
          i += expansions.size() - 1; // -1 accounts for loop's "++i"
        // Expand variadic expressions by constructing N expressions filled in w/ N values
        } else if(expanded_exp[i].is_type(types::exp)) {
          // verify ... follows an expression using the same # of expansion values per identifier
          size_type total_expansions = verify_all_identifiers_have_same_variadic_length(args,name,expanded_exp[i].exp,
                                                                                        expanded_exp,MACRO_EXPANSION_TREES);
          if(total_expansions == G::MAX_SIZE_TYPE)
            THROW_ERR("'syntax-rules Misplaced \"...\" after non-variadic subexpression [ " 
              << expanded_exp[i].exp << " ]\n     in [ " << expanded_exp << " ]" << FCN_ERR(name,args));
          scm_list expansions(total_expansions,expanded_exp[i].exp);
          // tag <expansions> nested identifiers, tag associated tree groups & 
          //   wrench them up to be a root (WHILE KEEPING THE NEW ROOTS IN PLACE)
          for(size_type i = 0, n = expansions.size(); i < n; ++i)
            tag_and_expand_identifiers_while_wrenching_up_children(i,expansions[i].exp,MACRO_EXPANSION_TREES);
          // expand the ctord exps & re-traverse to expand any nested ...
          expanded_exp.erase(expanded_exp.begin()+i,expanded_exp.begin()+i+2);
          expanded_exp.insert(expanded_exp.begin()+i,expansions.begin(),expansions.end());
          --i; // mv back to account for loop's "++" & completely re-traverse expanded exp
        // NOTE: SHOULD NEVER BE INVOKED, BUT KEPT HERE AS A SAFETY GUARD
        } else {
          THROW_ERR("'syntax-rules Misplaced \"...\" after non-variadic identifier [ " 
            << expanded_exp[i].sym << " ]\n     in [ " << expanded_exp << " ]\n"
            << stringify_MACRO_EXPANSION_TREES(MACRO_EXPANSION_TREES) << FCN_ERR(name,args));
        }
      // Parse the nested non-variadic expression
      } else if(expanded_exp[i].is_type(types::exp)) {
        expand_macro_variadic_identifiers(args,name,expanded_exp[i].exp,MACRO_EXPANSION_TREES);
      }
    } // End of for
  }
  

  // Expands non-variadics, and guarentees:
  //   0. No expressions begin w/ ...
  //   2. Any SYMBOLIC identifier followed by ... is variadic
  //      => NOTE: EXPRESSIONS FOLLOWED BY ... HAVE __NOT__ BEEN VERIFIED THO !!!
  void expand_non_variadic_macro_symbols(const scm_list& args, const sym_type& name, scm_list& expanded_exp, 
                                                               MACRO_EXPANSION_TREES_t& MACRO_EXPANSION_TREES){
    for(size_type i = 0; i < expanded_exp.size(); ++i) {
      if(is_symbolic_macro_identifier(expanded_exp[i])) {
        // Expand non-variadic identifiers
        auto val_idx = find_macro_identifier_leaf_index(MACRO_EXPANSION_TREES,expanded_exp[i].sym);
        if(i+1 == expanded_exp.size() || !data_is_ellipsis(expanded_exp[i+1])) {
          if(val_idx != G::MAX_SIZE_TYPE && !MACRO_EXPANSION_TREES[val_idx].is_variadic)
            expanded_exp[i] = MACRO_EXPANSION_TREES[val_idx].values[0];
        // Skip past ... if at a variadic identifier (handled in <expand_macro_variadic_identifiers>)
        } else if(val_idx != G::MAX_SIZE_TYPE && MACRO_EXPANSION_TREES[val_idx].is_variadic) {
          ++i;
        // Catch non-macro syntax identifiers OR non-variadic syntax identifier followed by ...
        } else {
          THROW_ERR("'syntax-rules Misplaced \"...\" after non-variadic identifier [ " 
            << expanded_exp[i].sym << " ]\n     in [ " << expanded_exp << " ]" << FCN_ERR(name,args));
        }
      } else if(expanded_exp[i].is_type(types::exp)) {
        // Recursively expand symbolic identifiers
        expand_non_variadic_macro_symbols(args,name,expanded_exp[i].exp,MACRO_EXPANSION_TREES);
        // Skip past variadics after expressions (handled in <expand_macro_variadic_identifiers>)
        if(i+1 < expanded_exp.size() && data_is_ellipsis(expanded_exp[i+1])) ++i;
      } else if(data_is_ellipsis(expanded_exp[i])) {
        if(i) {
          THROW_ERR("'syntax-rules Misplaced \"...\" after non-variadic identifier [ " 
            << expanded_exp[i-1].sym << " ]\n     in [ " << expanded_exp << " ]" << FCN_ERR(name,args));
        } else {
          THROW_ERR("'syntax-rules Misplaced \"...\" at front of a template expression!" 
            << expanded_exp << FCN_ERR(name,args));
        }
      }
    }
  }

  /******************************************************************************
  * MACRO SYNTACTIC EXTENSIONS -- MATRIX DATA CLEANING HELPER FCNS PRIOR TREE
  ******************************************************************************/

  void remove_VA_POS_MATRIX_duplicate_instances(VARARG_POSITIONS& VA_POS_MATRIX)noexcept{
    const auto sameRows = [](auto& row1, auto& row2) {
      if(row1.size() != row2.size()) return false;
      for(size_type i = 0, n = row1.size(); i < n; ++i)
        if(row1[i] != row2[i]) return false;
      return true;
    };
    for(size_type i = 0; i < VA_POS_MATRIX.size(); ++i)
      for(size_type j = i+1; j < VA_POS_MATRIX.size();) {
        if(sameRows(VA_POS_MATRIX[i],VA_POS_MATRIX[j]))
          VA_POS_MATRIX.erase(VA_POS_MATRIX.begin()+j);
        else
           ++j;
      }
  }


  // Correlate positions in pattern moreso to those in args by negating result of skipping initial '_'
  void decrement_first_elt_of_each_VA_POS_VECTOR(VARARG_POSITIONS& VA_POS_MATRIX)noexcept{
    for(size_type i = 0, n = VA_POS_MATRIX.size(); i < n; ++i) --VA_POS_MATRIX[i][0];
  }


  // Compose the above 2 helper fcns functions to clean the position data
  void clean_VA_POS_MATRIX_for_MACRO_ID_comparision(VARARG_POSITIONS& VA_POS_MATRIX)noexcept{
    remove_VA_POS_MATRIX_duplicate_instances(VA_POS_MATRIX);
    decrement_first_elt_of_each_VA_POS_VECTOR(VA_POS_MATRIX);
    // Sort variadics based on descending # of idxs, 
    //   to process subgroups of nested ... prior outer ...
    std::sort(VA_POS_MATRIX.begin(),VA_POS_MATRIX.end(),
      [](macId_position_t& e1,macId_position_t& e2){return e1.size()>e2.size();});
  }


  // Break down sequential grouped instances of variadic identifier matches into individual instances
  void split_ID_TO_VAL_MAP_children_into_unique_entries(MACRO_ID_TABLE& ID_TO_VAL_MAP)noexcept{
    for(auto& id_val_pos_tuple : ID_TO_VAL_MAP) {
      auto& val_pos_pairs = macId_val_pos_map(id_val_pos_tuple);
      for(size_type i = 0; i < val_pos_pairs.size(); ++i) {
        // Expand the set of values into single value instances
        if(val_pos_pairs[i].first.size() > 1) {
          MACRO_ID_VAL_POS_PAIRS indiv_val_pos_instances;
          auto posv = val_pos_pairs[i].second;
          for(size_type j = 1, n = val_pos_pairs[i].first.size(); j < n; ++j) {
            ++(*posv.rbegin());
            indiv_val_pos_instances.push_back(std::make_pair(scm_list(1,val_pos_pairs[i].first[j]),posv));
          }
          // Erase the excess values in the original value set
          val_pos_pairs[i].first.erase(val_pos_pairs[i].first.begin()+1,val_pos_pairs[i].first.end());
          // Add the values/positions as individual instances
          val_pos_pairs.insert(val_pos_pairs.begin()+i+1,std::make_move_iterator(indiv_val_pos_instances.begin()),
                                                         std::make_move_iterator(indiv_val_pos_instances.end()));
        }
      }
    }
  }


  // Determine if <id_posv> begins w/ the elts in <prefix>
  bool id_posv_begins_with_prefix(const macId_position_t& id_posv, const macId_position_t& prefix)noexcept{
    if(id_posv.size() < prefix.size()) return false;
    for(size_type i = 0, n = prefix.size(); i < n; ++i)
      if(id_posv[i] != prefix[i]) return false;
    return true;
  }


  // init <va_prefix> w/ the prefix values of <id_posv>
  void get_new_VA_POSV_prefix(macId_position_t& va_prefix, const macId_position_t& id_posv)noexcept{
    std::copy(id_posv.begin(),id_posv.begin()+va_prefix.size(),va_prefix.begin());
  }


  // Cleans & reorganizes the <MACRO_ID_VAR_TABLE> table for easier analysis
  void clean_MID_VARG_PAIR_for_macro_expansion_analysis(MACRO_ID_VAR_TABLE& MID_VARG_PAIR) {
    split_ID_TO_VAL_MAP_children_into_unique_entries(MID_VARG_PAIR.first);
    clean_VA_POS_MATRIX_for_MACRO_ID_comparision(MID_VARG_PAIR.second);
  }


  // Adds values & positions to a <macro_expansion_node>
  // => NOTE: LAST UNUSED ARG IS JUST TO MATCH THE SAME FCN PTR TYPE AS <extract_id_children_subgroup>
  void accumulate_id_leaf_values_and_positions(macro_expansion_node& id_node, MACRO_ID_VAL_POS_PAIR& val_pos_pair,
                                                                              MACRO_EXPANSION_TREES_t&)noexcept{
    id_node.values.insert(id_node.values.end(),val_pos_pair.first.begin(),val_pos_pair.first.end());
    id_node.positions.push_back(val_pos_pair.second);
  }


  // Confirm <posv_matrix> contains <sought_posv>
  bool matrix_contains_vector(const macId_position_t& sought_posv, const std::vector<macId_position_t>& posv_matrix)noexcept{
    for(auto& posv : posv_matrix) {
      if(posv.size() != sought_posv.size()) continue;
      bool same_posv = true;
      for(size_type i = 0, n = posv.size(); i < n; ++i)
        if(posv[i] != sought_posv[i]) {
          same_posv = false;
          break;
        }
      if(same_posv) return true;
    }
    return false;
  }


  // Recursive search for <extract_id_children_subgroup>, returns whether found position in subgroup
  bool extract_id_children_subgroup_recur(macro_expansion_node& child, MACRO_ID_VAL_POS_PAIR& val_pos_pair)noexcept{
    if(child.is_leaf()) return matrix_contains_vector(val_pos_pair.second,child.positions);
    for(auto& grand_child : child.children)
      if(extract_id_children_subgroup_recur(grand_child,val_pos_pair))
        return true;
    return false;
  }


  // Extracts the subgroup from <generated_subgroups> containing <val_pos_pair> & puts it into <id_node>
  // => NOTE: if <generated_subgroups> DOESN'T have <val_pos_pair>, it is assumed to be already in <id_node>
  void extract_id_children_subgroup(macro_expansion_node& id_node, MACRO_ID_VAL_POS_PAIR& val_pos_pair,
                                                          MACRO_EXPANSION_TREES_t& generated_subgroups)noexcept{
    for(size_type i = 0, n = generated_subgroups.size(); i < n; ++i)
      if(extract_id_children_subgroup_recur(generated_subgroups[i],val_pos_pair)) {
        id_node.children.push_back(generated_subgroups[i]);
        generated_subgroups.erase(generated_subgroups.begin()+i);
        return; // found the subgroup, no more search needed!
      }
  }


  // Constructs <MACRO_EXPANSION_TREES> based on <ID_TO_VAL_MAP> & <VA_POS_MATRIX>
  // (transformation yields an easier means to expand nested variadic expressions by)
  void derive_and_construct_macro_expansion_tree(MACRO_ID_TABLE& ID_TO_VAL_MAP, VARARG_POSITIONS& VA_POS_MATRIX, 
                                                        MACRO_EXPANSION_TREES_t& MACRO_EXPANSION_TREES)noexcept{
    // For each identifier instance
    for(auto& macId_instance : ID_TO_VAL_MAP) {
      // Create the macro identifier expansion tree's root
      macro_expansion_node macId_node(macId_name(macId_instance));
      auto& val_pos_map = macId_val_pos_map(macId_instance);

      // For each ... instance
      for(auto& va_posv : VA_POS_MATRIX) {
        auto sought_id_posv_prefix = va_posv;
        --(*sought_id_posv_prefix.rbegin()); // 1st identifier associated w/ ... appears 1 idx prior ...
        
        // If identifier does match against ... instance
        if(id_posv_begins_with_prefix(val_pos_map[0].second,sought_id_posv_prefix)) {
          macId_node.is_variadic = true;
          sought_id_posv_prefix.pop_back(); // rm last idx to match against (no longer relevant to match)

          // Mk a subgroup node for the variadic expansion
          macro_expansion_node subgroup_node(macId_node.id_name,true);

          // If !macId_node.is_leaf(), keep a buffer of the current children subgroups
          //   from which to derive a higher level of subgroups (from nested ...)
          MACRO_EXPANSION_TREES_t generated_subgroups;

          // Fcn to build up the tree, based on whether currently aggregating leaves or combining subgroups
          void(*build_macro_expansion_tree)(macro_expansion_node&,MACRO_ID_VAL_POS_PAIR&,MACRO_EXPANSION_TREES_t&)noexcept = nullptr;
          // Add the leaf values as needed
          if(macId_node.is_leaf()) {
            accumulate_id_leaf_values_and_positions(subgroup_node,val_pos_map[0],generated_subgroups);
            build_macro_expansion_tree = accumulate_id_leaf_values_and_positions;
          // Get the current subgroup set as needed
          } else {
            generated_subgroups = std::move(macId_node.children);
            macId_node.children.clear();
            extract_id_children_subgroup(subgroup_node,val_pos_map[0],generated_subgroups);
            build_macro_expansion_tree = extract_id_children_subgroup;
          }

          // For each value instance of the identifier 
          for(size_type i = 1, n = val_pos_map.size(); i < n; ++i) {
            // if value posv matches the current ... subgroup instance
            if(id_posv_begins_with_prefix(val_pos_map[i].second,sought_id_posv_prefix)) {
              build_macro_expansion_tree(subgroup_node,val_pos_map[i],generated_subgroups);
            // if value posv matches a new ... subgroup instance
            } else {
              // AT A NEW SUBGROUP!
              get_new_VA_POSV_prefix(sought_id_posv_prefix,val_pos_map[i].second);
              // Add the current subgroup as a child, & reset the current subgroup node
              macId_node.children.push_back(subgroup_node);
              subgroup_node = macro_expansion_node(macId_node.id_name,true);
              build_macro_expansion_tree(subgroup_node,val_pos_map[i],generated_subgroups);
            }
          } // End of for

          // Add the current subgroup as a child
          macId_node.children.push_back(subgroup_node);

        } // End of if
      } // End of for
      if(!macId_node.is_variadic) {
        // Save the leaf non-variadic value (in this instance, root = leaf)
        macId_node.values = val_pos_map[0].first;
        macId_node.positions.push_back(val_pos_map[0].second);
      }

      // Register the generated macro id expansion tree
      if(macId_node.is_variadic && macId_node.children.size() == 1) 
        MACRO_EXPANSION_TREES.push_back(std::move(macId_node.children[0]));
      else
        MACRO_EXPANSION_TREES.push_back(std::move(macId_node));
    } // End of for
  }


  void expand_macro(const scm_list& args, const sym_type& name, scm_list& expanded_exp, 
                                                    MACRO_ID_VAR_TABLE& MID_VARG_PAIR){
    MACRO_EXPANSION_TREES_t MACRO_EXPANSION_TREES;
    clean_MID_VARG_PAIR_for_macro_expansion_analysis(MID_VARG_PAIR);
    derive_and_construct_macro_expansion_tree(MID_VARG_PAIR.first,MID_VARG_PAIR.second,MACRO_EXPANSION_TREES);
    expand_non_variadic_macro_symbols(args,name,expanded_exp,MACRO_EXPANSION_TREES);
    expand_macro_variadic_identifiers(args,name,expanded_exp,MACRO_EXPANSION_TREES);
    hash_macro_expansion_identifier("",true); // reset hash idxs
  }

  /******************************************************************************
  * MACRO SYNTACTIC EXTENSIONS -- EXPANSION MAIN FUNCTIONS
  ******************************************************************************/

  // Confirm whether 'application_label' is a potential macro label
  bool application_is_a_potential_macro(const scm_string& application_label, 
                                        const std::vector<scm_string>& label_registry)noexcept{
    if(application_label.empty()) return false;
    for(const auto& label : label_registry)
      if(label == application_label)
        return true;
    return false;
  }


  // Returns whether the given label & args form a macro found in 'macs'.
  // If true, it also transforms the macro by expanding it into 'expanded_exp'
  bool handle_macro_transformation(const sym_type& label,const scm_list& args, 
                                   const frame_macs& macs,scm_list& expanded_exp){
    //  Map of pattern identifier & expansion value pairs
    MACRO_ID_VAR_TABLE MID_VARG_PAIR;
    // search for macro matches
    for(const auto& mac : macs) {
      size_type match_idx = 0; // idx of the pattern & template w/in 'mac' that the label & args match
      if(label == mac.label && is_macro_match(args, mac, match_idx, MID_VARG_PAIR)) {
        expanded_exp = mac.templates[match_idx]; // prefilled, then has contents expanded into it
        expand_macro(args, mac.label, expanded_exp, MID_VARG_PAIR);
        expanded_exp = scm_list_cast(expanded_exp); // if only 1 sub-expression, degrade to the sub-expression
        return true;
      }
    }
    return false;
  }


  // Returns whether the given label & args form a macro found in 'env'.
  // If true, it also transforms the macro by expanding it into 'expanded_exp'
  bool expand_macro_if_in_env(const sym_type& label,const scm_list& args, 
                              env_type& env,scm_list& expanded_exp){
    // Search Each Environment Frame
    for(auto& frame : *env) {
      // Get Macro List of the current frame & search 
      //   for a match with the current label & args
      if(handle_macro_transformation(label,args,frame_macros(*frame),expanded_exp)) 
        return true;
    }
    return false;
  }

  /******************************************************************************
  * MACRO SYNTAX-RULES ANALYSIS VALIDATION HELPER FUNCTIONS:
  ******************************************************************************/

  // Confirm data is an unexpandable syntax-rules macro token
  bool data_is_literal_or_keyword(const data& pat_entity, const frame_vars& keywords)noexcept{
    return !pat_entity.is_type(types::exp) && !is_macro_argument_label(pat_entity,keywords);
  }


  // PRECONDITION: is_macro_argument_label(pattern[i],keywords) = true
  void confirm_unique_syntax_rules_pattern_identifier(const scm_list& pattern,const size_type& i,
                                                      const scm_list& exp, frame_vars& identifiers) {
    static constexpr const char * const format = 
      "\n     (syntax-rules (<keyword-list>) <pattern-template-clauses>)"
      "\n     <pattern-template-clause> = ((<pattern>) <template>)";
    // Confirm each pattern identifier only appears once
    if(std::find(identifiers.begin(),identifiers.end(),pattern[i].sym) != identifiers.end())
      THROW_ERR("'syntax-rules " << pattern[i].sym << " identifier found twice!"
        "\n     Each syntax-rules pattern identifier MUST be unique!\n     " 
        << cio_expr_str<&data::write>(pattern) << format << EXP_ERR(exp));
    identifiers.push_back(pattern[i].sym);
  }


  void confirm_proper_syntax_rules_pattern_layout(const scm_list& pattern,const scm_list& exp,
                                                  const frame_vars& keywords, frame_vars& identifiers){
    static constexpr const char * const format = 
      "\n     (syntax-rules (<keyword-list>) <pattern-template-clauses>)"
      "\n     <pattern-template-clause> = ((<pattern>) <template>)";
    // Confirm pattern subexpression doesn't begin w/ '...'
    if(pattern.empty()) return; // guarenteed to be part of a recursive call (topmost pattern asserted non-empty)
    if(!pattern.empty() && data_is_ellipsis(pattern[0]))
      THROW_ERR("'syntax-rules \"...\" may NEVER begin a pattern subexpression!"
        "\n     " << cio_expr_str<&data::write>(pattern) << format <<EXP_ERR(exp));
    if(is_macro_argument_label(pattern[0],keywords))
      confirm_unique_syntax_rules_pattern_identifier(pattern,0,exp,identifiers);
    bool seen_ellipses = false;
    for(size_type i = 1, n = pattern.size(); i < n; ++i) {
      if(data_is_ellipsis(pattern[i])) {
        // Confirm each subexpression has at most 1 '...'
        if(seen_ellipses){
          THROW_ERR("'syntax-rules \"...\" may only appear ONCE per pattern subexpression!"
            "\n     " << cio_expr_str<&data::write>(pattern) <<
            "\n     -> IE: (a ... (b ...)) => VALID: 1 '...' PER EXPRESSION!"
            "\n            (a ... b ...)   => INVALID: 2 '...' IN A SINGLE EXPRESSION!"
            << format <<EXP_ERR(exp));
        } 
        // Confirm '...' doesn't follow a literal or keyword in the pattern
        if(data_is_literal_or_keyword(pattern[i-1],keywords)){
          THROW_ERR("'syntax-rules \"...\" may only be preceded by a non-literal-non-keyword"
            "\n     symbol or expression!\n     " << cio_expr_str<&data::write>(pattern) 
            << format <<EXP_ERR(exp));
        }
        seen_ellipses = true;
      } else if(pattern[i].is_type(types::exp)) {
        confirm_proper_syntax_rules_pattern_layout(pattern[i].exp,exp,keywords,identifiers);
      } else if(is_macro_argument_label(pattern[i],keywords)) {
        confirm_unique_syntax_rules_pattern_identifier(pattern,i,exp,identifiers);
      }
    }
  }


  // Confirm proper '...' use & that each pattern identifier only appears once
  void confirm_proper_syntax_rules_pattern_template_clauses(const scm_list& exp,const syn_type& mac,const char* format){
    //   IE: ((a) ... (b ...)) is fine,     1 '...' per subexpression "depth"
    //       ((a) ... b ...)   is NOT fine, 2 '...' at a single "depth"
    //       (... a)         is NOT fine, '...' begins the subexpression/depth
    for(size_type i = 2, n = exp.size(); i < n; ++i) {
      // Confirm pattern-template clause begins w/ a non-empty pattern expression
      if(!exp[i].is_type(types::exp) || exp[i].exp.size() < 2 || 
         !exp[i].exp[0].is_type(types::exp) || exp[i].exp[0].exp.empty())
        THROW_ERR("'syntax-rules " << PROFILE(exp[i])
          << " is an invalid ((<pattern>) <template>) clause!" << format << EXP_ERR(exp));
      // Confirm pattern begins w/ a symbol
      if(!exp[i].exp[0].exp[0].is_type(types::sym))
        THROW_ERR("'syntax-rules patterns MUST begin with a symbol!"
          "\n     " << cio_expr_str<&data::write>(exp[i].exp[0].exp) << format << EXP_ERR(exp));
      // Confirm pattern's topmost subexpression depth doesn't start w/ '...'
      if(exp[i].exp[0].exp.size() > 1 && data_is_ellipsis(exp[i].exp[0].exp[1]))
        THROW_ERR("'syntax-rules pattern '...' identifier must be preceded"
          " by a symbol or expression identifier!\n     " 
          << cio_expr_str<&data::write>(exp[i].exp[0].exp) << format << EXP_ERR(exp));
      // Confirm each pattern identifier only appears once
      frame_vars identifiers;
      confirm_proper_syntax_rules_pattern_layout(exp[i].exp[0].exp,exp,mac.keywords,identifiers);
    }
  }


  // Confirm syntax-rules keywords list is valid, and extract it if so
  void extract_syntax_rules_keywords(const scm_list& exp,syn_type& mac,const char* format) {
    if(!exp[1].is_type(types::exp))
      THROW_ERR("'syntax-rules 1st arg "<<PROFILE(exp[1])<<" isn't a list of keyword symbols:"
        <<format<<EXP_ERR(exp));
    for(const auto& e : exp[1].exp) {
      if(!e.is_type(types::sym))
        THROW_ERR("'syntax-rules keyword "<<PROFILE(e)<<" must be a symbol!"<<format<<EXP_ERR(exp));
      if(e.sym == symconst::ellipsis)
        THROW_ERR("'syntax-rules \"...\" identifier may never be a keyword!"<<format<<EXP_ERR(exp));
      mac.keywords.push_back(e.sym); // Extract keywords
    }
  }


  void confirm_valid_syntax_rules_and_extract_keywords(const scm_list& exp, syn_type& mac) {
    static constexpr const char * const format = 
      "\n     (syntax-rules (<keyword-list>) <pattern-template-clauses>)"
      "\n     <pattern-template-clause> = ((<pattern>) <template>)";
    if(exp.size() < 3) // Confirm correct # of arguments
      THROW_ERR("'syntax-rules received incorrect # of arguments!"<<format<<EXP_ERR(exp));
    extract_syntax_rules_keywords(exp,mac,format);
    confirm_proper_syntax_rules_pattern_template_clauses(exp,mac,format);
  }

  /******************************************************************************
  * REPRESENTING SYNTAX: (syntax-rules <keyword-list> <pattern-template-clauses>)
  ******************************************************************************/

  bool is_syntax_rules(const scm_list& exp)noexcept{
    return is_tagged_list(exp,symconst::syn_rules);
  }


  // Generate a unique hashed (hygienic!) variant of the given macro arg name
  // NOTE: Max Unique Hashes = (expt 18446744073709551615 18446744073709551615)
  scm_string hygienically_hashed_macro_arg(const scm_string& label)noexcept{
    if(G::MACRO_HASH_IDX_1 != G::MAX_SIZE_TYPE)
      return "heist:core:" + label + '-' 
        + std::to_string(G::MACRO_HASH_IDX_2) + '-' 
        + std::to_string(G::MACRO_HASH_IDX_1++);
    return "heist:core:" + label + '-' 
      + std::to_string(++G::MACRO_HASH_IDX_2) + '-' 
      + std::to_string(G::MACRO_HASH_IDX_1++);
  }


  void recursively_hygienically_hash_macro_template(const scm_string& value, const scm_string& hash_value, 
                                                                             scm_list& mac_template)noexcept{
    for(size_type i = 0, n = mac_template.size(); i < n; ++i) {
      if(mac_template[i].is_type(types::exp)) {
        recursively_hygienically_hash_macro_template(value, hash_value, mac_template[i].exp);
      } else if(mac_template[i].is_type(types::sym) && mac_template[i].sym == value) {
        mac_template[i].sym = hash_value;
      }
    }
  }


  void recursively_hygienically_hash_macro_pattern(scm_list& pattern, scm_list& mac_template, 
                                                   const frame_vars& keywords, const size_type& start=0)noexcept{
    for(size_type i = start, n = pattern.size(); i < n; ++i) {
      if(pattern[i].is_type(types::exp)) {
        recursively_hygienically_hash_macro_pattern(pattern[i].exp, mac_template, keywords);
      } else if(is_macro_argument_label(pattern[i], keywords)) {
        scm_string original_label = pattern[i].sym;
        pattern[i].sym = hygienically_hashed_macro_arg(pattern[i].sym);
        recursively_hygienically_hash_macro_template(original_label, pattern[i].sym, mac_template);
      }
    }
  }


  exe_type analyze_syntax_rules(scm_list& exp) {
    syn_type mac("");
    confirm_valid_syntax_rules_and_extract_keywords(exp, mac);
    // Extract pattern-template clauses
    for(size_type i = 2, n = exp.size(); i < n; ++i) {
      mac.patterns.push_back(exp[i].exp[0].exp);
      // Add sentinel arg if pattern is argless
      if(exp[i].exp[0].exp.size() == 1)
        mac.patterns.rbegin()->push_back(symconst::sentinel_arg); 
      // Wrap 'begin' around templates prior evaluation (for multi-exp bodies)
      mac.templates.push_back(scm_list(exp[i].exp.size()));
      mac.templates.rbegin()->operator[](0) = symconst::begin;
      std::copy(exp[i].exp.begin()+1, 
                exp[i].exp.end(), 
                mac.templates.rbegin()->begin()+1);
    }
    // Hash macro args to prevent unintentional expansions => "hygienic" macros
    for(size_type i = 0, n = mac.patterns.size(); i < n; ++i) {
      // pass <1> to not hash 1st pattern symbol (not an arg)
      recursively_hygienically_hash_macro_pattern(mac.patterns[i],mac.templates[i],mac.keywords,1);
    }
    return [syntax_rule=scm_list(1,std::move(mac))](env_type&){return syntax_rule;};
  }

  /******************************************************************************
  * REPRESENTING SYNTAX EXTENSIONS: (define-syntax <label> <syntax-rules-object>)
  ******************************************************************************/

  bool is_define_syntax(const scm_list& exp)noexcept{return is_tagged_list(exp,symconst::defn_syn);}
  bool is_let_syntax   (const scm_list& exp)noexcept{return is_tagged_list(exp,symconst::let_syn);}
  bool is_letrec_syntax(const scm_list& exp)noexcept{return is_tagged_list(exp,symconst::letrec_syn);}


  void register_symbol_iff_new(std::vector<sym_type>& registry, const sym_type& label) {
    for(const auto& macro_label : registry)
      if(macro_label == label)
        return;
    registry.push_back(label);
  }


  void confirm_is_not_core_syntax_label(scm_list& exp) {
    if(std::find(G::ANALYSIS_TIME_MACRO_LABEL_REGISTRY.begin(),
                 G::ANALYSIS_TIME_MACRO_LABEL_REGISTRY.end(),exp[1].sym) != 
       G::ANALYSIS_TIME_MACRO_LABEL_REGISTRY.end()) {
      THROW_ERR("'define-syntax label \""<<exp[1].sym<<"\" is already 'core-syntax!"
        "\n     (define-syntax <label> <syntax-rules-object>)"<<EXP_ERR(exp));
    }
  }


  void confirm_valid_define_syntax(const scm_list& exp) {
    if(exp.size() != 3)
      THROW_ERR("'define-syntax expects 2 arguments:"
        "\n     (define-syntax <label> <syntax-rules-object>)"<<EXP_ERR(exp));
    if(!exp[1].is_type(types::sym))
      THROW_ERR("'define-syntax 1st arg "<<PROFILE(exp[1])
        <<" isn't a symbolic label:" << EXP_ERR(exp));
  }


  bool must_evaluate_2nd_arg_for_syntax_rules_object(scm_list& exp) {
    confirm_valid_define_syntax(exp);
    // If given a syntax-rules expression, immediately evaluate (no need for environment)
    if(exp[2].is_type(types::exp) && !exp[2].exp.empty() && 
      is_tagged_list(exp[2].exp,symconst::syn_rules)) {
      env_type nil_env = nullptr;
      exp[2] = scm_analyze(std::move(exp[2].exp))(nil_env)[0];
      return false;
    }
    return !exp[2].is_type(types::syn);
  }


  void confirm_valid_let_macro(const scm_list& exp, const char* name) {
    static constexpr const char * const format = 
      " (<syntactic-bindings-list>) <body>)"
      "\n     <syntactic-binding> = (<label> <syntax-rules>)";
    if(exp.size() < 2 || !exp[1].is_type(types::exp))
      THROW_ERR('\''<<name<<" 1st argument must be a syntactic bindings list:"
        "\n     (" << name << format << EXP_ERR(exp));
    if(exp.size() < 3)
      THROW_ERR('\''<<name<<" 2nd argument must be a body:"
        "\n     (" << name << format << EXP_ERR(exp));
    // Confirm bindings list is either EMPTY or a LIST OF LISTS, EACH OF LENGTH 2
    for(const auto& binding : exp[1].exp)
      if(!binding.is_type(types::exp) || binding.exp.size() != 2)
        THROW_ERR('\''<<name<<" binding "<<binding<<" isn't a proper syntactic binding:"
          "\n     (" << name << format << EXP_ERR(exp));
  }


  // Template for 'let-syntax' & 'letrec-syntax' special forms (letrec style is default)
  exe_type let_syntactic_extension_binding_template(scm_list& exp,const char* name,
                            const bool tail_call=false,const bool cps_block=false){
    // Convert let-syntax/letrec-syntax to an argless let defining syntax in its body
    confirm_valid_let_macro(exp,name);
    scm_list let_exp(exp[1].exp.size()+exp.size());
    let_exp[0] = symconst::let, let_exp[1] = scm_list();
    // Splice in syntax defns
    for(size_type i = 0, n = exp[1].exp.size(); i < n; ++i){ 
      let_exp[i+2] = scm_list(3);
      let_exp[i+2].exp[0] = symconst::defn_syn;
      let_exp[i+2].exp[1] = exp[1].exp[i].exp[0];
      let_exp[i+2].exp[2] = exp[1].exp[i].exp[1];
    }
    // Add let body
    std::copy(exp.begin()+2,exp.end(),let_exp.begin()+exp[1].exp.size()+2);
    // Analyze let of syntax defns
    if(!cps_block) return scm_analyze(std::move(let_exp),tail_call);
    return scm_analyze(generate_fundamental_form_cps(let_exp),tail_call,true);
  }


  exe_type analyze_define_syntax(scm_list& exp,const bool cps_block=false,const bool core_syntax=false) {
    if(must_evaluate_2nd_arg_for_syntax_rules_object(exp)) { 
      if(!core_syntax) confirm_is_not_core_syntax_label(exp);
      auto syntax_rules_obj_proc = scm_analyze(scm_list_cast(exp[2]),false,cps_block);
      return [syntax_rules_obj_proc=std::move(syntax_rules_obj_proc),
        exp=std::move(exp)](env_type& env)mutable{
        data mac = data_cast(syntax_rules_obj_proc(env));
        if(!mac.is_type(types::syn)) 
          THROW_ERR("'define-syntax 2nd arg "<<PROFILE(exp[2])
            <<" isn't a syntax-rules object:\n     (define-syntax "
              "<label> <syntax-rules-object>)"<<EXP_ERR(exp));
        mac.syn.label = exp[1].sym;     // assign macro label
        register_symbol_iff_new(G::MACRO_LABEL_REGISTRY,exp[1].sym);
        define_syntax_extension(mac.syn,env); // establish in environment
        return G::VOID_DATA_EXPRESSION;
      };
    }
    if(!core_syntax) confirm_is_not_core_syntax_label(exp);
    exp[2].syn.label = exp[1].sym; // assign macro label
    register_symbol_iff_new(G::MACRO_LABEL_REGISTRY,exp[1].sym);
    return [mac = std::move(exp[2].syn)](env_type& env){
      define_syntax_extension(mac,env); // establish in environment
      return G::VOID_DATA_EXPRESSION;
    };
  }


  exe_type analyze_let_syntax(scm_list& exp,const bool tail_call=false,const bool cps_block=false){
    return let_syntactic_extension_binding_template(exp,symconst::let_syn,tail_call,cps_block);
  }


  exe_type analyze_letrec_syntax(scm_list& exp,const bool tail_call=false,const bool cps_block=false){
    return let_syntactic_extension_binding_template(exp,symconst::letrec_syn,tail_call,cps_block);
  }

  /******************************************************************************
  * ANALYSIS-TIME & ALWAYS-GLOBAL-SCOPE MACRO
  ******************************************************************************/

  bool is_core_syntax(const scm_list& exp)noexcept{return is_tagged_list(exp,symconst::core_syn);}

  exe_type analyze_core_syntax(scm_list& exp,const bool cps_block=false) {
    static constexpr const char * const format = 
      "\n     (core-syntax <name> (<keyword> ...) (<pattern> <template>) ...)";
    if(exp.size() < 4)
      THROW_ERR("'core-syntax didn't recieve enough args!\n     In expression: " << exp << format);
    if(!exp[1].is_type(types::sym))
      THROW_ERR("'core-syntax didn't recieve enough args!\n     In expression: " << exp << format);
    // Register the core-syntax label
    register_symbol_iff_new(G::ANALYSIS_TIME_MACRO_LABEL_REGISTRY,exp[1].sym);
    // Transform core-syntax->define-syntax, evaling via global env, then registering the analysis-time label
    scm_list define_syntax_transform(3);
    define_syntax_transform[0] = symconst::defn_syn;
    define_syntax_transform[1] = exp[1];
    define_syntax_transform[2] = scm_list();
    define_syntax_transform[2].exp.push_back(symconst::syn_rules);
    define_syntax_transform[2].exp.insert(define_syntax_transform[2].exp.end(),exp.begin()+2,exp.end());
    // Eval the syntax defn AT ANALYSIS TIME in the global env
    analyze_define_syntax(define_syntax_transform,cps_block,true)(G::GLOBAL_ENVIRONMENT_POINTER);
    return [](env_type&){return G::VOID_DATA_EXPRESSION;};
  }

  /******************************************************************************
  * TRACING PROCEDURE CALLS
  ******************************************************************************/

  // Prints Debugging Call Trace (see <set-trace-calls!> primitive)
  void output_debug_call_trace(const scm_list& procedure,const scm_list& arguments,
                               const bool tail_call,     const bool callceing)noexcept{
    if(!primitive_data_is_a_procedure(procedure)) return;
    // Generate the call signature & Application-Affecting Call States
    auto call_signature = procedure_call_signature(printable_procedure_name(procedure),arguments);
    const char* in_tail_call = tail_call ? "#t" : "#f";
    const char* using_callce = callceing ? "#t" : "#f";
    const char* using_inline = G::USING_INLINE_INVOCATIONS ? "#t" : "#f";
    // Generate colors for truth-values (Green=#t, Red=#f) of Call States
    const auto tail_c_color = tail_call ? AFMT_32 : AFMT_31;
    const auto callce_color = callceing ? AFMT_32 : AFMT_31;
    const auto inline_color = G::USING_INLINE_INVOCATIONS ? AFMT_32 : AFMT_31;
    // Output Trace Data
    fprintf(G::CURRENT_OUTPUT_PORT,
            "\n%s-----------------------------------------------------"
            "\n%s#<CALL-TRACE>:%s Tail-Call: %s%s%s, Call/ce: %s%s%s, Inline: %s%s%s"
            "\n%s"
            "\n-----------------------------------------------------%s\n",
            afmt(AFMT_01), afmt(AFMT_35), afmt(AFMT_01), 
            afmt(tail_c_color), in_tail_call, afmt(AFMT_01), 
            afmt(callce_color), using_callce, afmt(AFMT_01), 
            afmt(inline_color), using_inline, afmt(AFMT_01), 
            call_signature.c_str(), afmt(AFMT_0));
    fflush(G::CURRENT_OUTPUT_PORT);
  }


  // Prints Call Trace (see <trace> primitive)
  void print_call_trace_depth_indentation(const scm_list& procedure,const bool tail_call=false)noexcept{
    size_type recursive_depth = 0; // default to 0 (level for all prims)
    if(is_compound_procedure(procedure)) {
      recursive_depth = *procedure[4].cal;
      if(recursive_depth && tail_call) --recursive_depth;
    }
    for(size_type i = 0; i <= recursive_depth; ++i) {
      if(i & 1) fputc(' ',G::CURRENT_OUTPUT_PORT);
      else      fputc('|',G::CURRENT_OUTPUT_PORT);
    }
    fflush(G::CURRENT_OUTPUT_PORT);
  }


  // Print the current recursive depth as indentation, and the current invocation signature
  void output_call_trace_invocation(const scm_list& procedure, const scm_list& arguments,const bool tail_call=false)noexcept{
    if(!primitive_data_is_a_procedure(procedure)) return;
    auto call_signature = procedure_call_signature(printable_procedure_name(procedure),arguments);
    print_call_trace_depth_indentation(procedure,tail_call);
    fprintf(G::CURRENT_OUTPUT_PORT, "%s\n", call_signature.c_str());
    fflush(G::CURRENT_OUTPUT_PORT);
  }


  // Print the current recursive depth as indentation, and the result string
  void output_call_trace_result(const scm_list& procedure, const data& result)noexcept{
    if(!primitive_data_is_a_procedure(procedure)) return;
    print_call_trace_depth_indentation(procedure);
    fprintf(G::CURRENT_OUTPUT_PORT, "%s\n", result.write().c_str());
    fflush(G::CURRENT_OUTPUT_PORT);
  }


  bool tracing_procedure(const scm_string& name)noexcept{
    return !G::TRACED_FUNCTION_NAME.empty() && G::TRACED_FUNCTION_NAME == name;
  }

  /******************************************************************************
  * APPLICATION
  ******************************************************************************/

  // -- PRIMITIVE PROCEDURES: identification & application
  bool is_primitive_procedure(const scm_list& p)noexcept{
    return is_tagged_list(p,symconst::primitive);
  }

  scm_list apply_primitive_procedure(scm_list& proc,scm_list& args,env_type& env,const bool tail_call){
    // Rm "sentinel-arg" value from args (if present from an argless application)
    if(args.size()==1 && args[0].is_type(types::sym) && args[0].sym == symconst::sentinel_arg)
      args.pop_back();
    // Output tracing information as needed
    auto tracing_proc = tracing_procedure(proc[2].sym);
    if(tracing_proc) output_call_trace_invocation(proc,args);
    // Provide the environment to primitives applying user-defined procedures
    if(primitive_requires_environment(proc[1].prm)) args.push_back(env);
    if(proc[1].prm == primitive_APPLY) args.push_back(boolean(tail_call));
    if(!tracing_proc) return scm_list_cast(proc[1].prm(args));
    // Output result's trace as needed
    auto result = scm_list_cast(proc[1].prm(args));
    output_call_trace_result(proc,data_cast(result));
    return result;
  }


  // -- APPLY
  // Applies the given procedure, & then reapplies iteratively if at a tail call
  scm_list apply_compound_procedure(exe_type& proc, env_type& extended_env) {
    auto result = proc(extended_env);
  tail_call_recur:
    if(is_tagged_list(result,symconst::tail_call)) { // if tail call
      result = result[1].exe(result[2].env);
      goto tail_call_recur;
    }
    return result;
  }

  // Analogue to "apply", except no need to analyze the body of compound 
  //   procedures (already done). Hence only calls the execution procedure 
  //   for the proc's body w/ the extended environment
  scm_list execute_application(scm_list& procedure,scm_list& arguments,env_type& env,
                                            const bool tail_call,const bool callceing){
    // output debugger call trace as needed
    if(G::TRACING_ALL_FUNCTION_CALLS)
      output_debug_call_trace(procedure,arguments,tail_call,callceing);
    // execute primitive procedure directly
    if(is_primitive_procedure(procedure))
      return apply_primitive_procedure(procedure,arguments,env,tail_call);
    else if(is_compound_procedure(procedure)) {
      // create the procedure body's extended environment frame
      auto extended_env = extend_environment(procedure_parameters(procedure),
                                             arguments,
                                             procedure_environment(procedure),
                                             procedure_name(procedure));
      // determine whether to inline call
      const bool inline_call = !G::USING_INLINE_INVOCATIONS && procedure_inlines_call(procedure);
      if(inline_call) G::USING_INLINE_INVOCATIONS = true;
      // splice in current env for dynamic scope as needed
      if(callceing || G::USING_INLINE_INVOCATIONS)
        extended_env->insert(extended_env->begin()+1, env->begin(), env->end());
      // confirm max recursive depth hasn't been exceeded
      auto& recursive_depth = procedure_recursive_depth(procedure);
      if(recursive_depth > G::MAX_RECURSION_DEPTH) {
        if(inline_call) G::USING_INLINE_INVOCATIONS = false;
        recursive_depth = 0;
        THROW_ERR("Maximum recursion depth of "<<G::MAX_RECURSION_DEPTH<<" exceeded!"
          << FCN_ERR(printable_procedure_name(procedure), arguments));
      }
      // output tracing information as needed
      auto tracing_proc = tracing_procedure(procedure[5].sym);
      if(tracing_proc) output_call_trace_invocation(procedure,arguments,tail_call);
      // store application data & return such back up to the last call if in a tail call
      if(tail_call) {
        if(inline_call) G::USING_INLINE_INVOCATIONS = false;
        scm_list tail_call_signature(3); // {tail-call-tag, proc-body, extended-env}
        tail_call_signature[0] = symconst::tail_call;
        tail_call_signature[1] = procedure_body(procedure);
        tail_call_signature[2] = extended_env;
        return tail_call_signature;
      }
      ++recursive_depth;
      auto result = apply_compound_procedure(procedure_body(procedure),extended_env);
      --recursive_depth;
      // output result's trace as needed
      if(tracing_proc) output_call_trace_result(procedure,data_cast(result));
      if(inline_call) G::USING_INLINE_INVOCATIONS = false;
      return result;
    }
    if(procedure.empty())
      THROW_ERR("Invalid application of unknown procedure type NULL (received an empty expression)!");
    THROW_ERR("Invalid application of unknown procedure "<<PROFILE(procedure[0])<<"! "<<EXP_ERR(procedure));
  }

  // R-value overload
  scm_list execute_application(scm_list&& procedure,scm_list& arguments,env_type& env,
                                             const bool tail_call,const bool callceing){
    return execute_application(procedure,arguments,env,tail_call,callceing);
  }

  /******************************************************************************
  * CPS-BLOCK APPLICATION HELPER FUNCTIONS
  ******************************************************************************/

  bool data_is_continuation_parameter(const data& d)noexcept{
    return d.is_type(types::sym) && d.sym.find(symconst::continuation) == 0;
  }

  bool procedure_defined_outside_of_CPS_block(const scm_list& p)noexcept{
    return is_primitive_procedure(p) || 
      (is_compound_procedure(p) && (p[1].exp.empty() || 
        !data_is_continuation_parameter(*p[1].exp.rbegin())));
  }

  bool procedure_requires_continuation(const scm_list& p)noexcept{
    // '1' for compound procs accounts for ' ' prefix from name mangling
    return (is_compound_procedure(p) && p[5].sym.find(symconst::pass_continuation) == 1) ||
           (is_primitive_procedure(p) && p[2].sym.find(symconst::pass_continuation) == 0);
  }

  /******************************************************************************
  * CPS-BLOCK APPLICATION
  ******************************************************************************/

  void eval_application_arg_procs(const std::vector<exe_type>& arg_procs,scm_list& arg_vals,env_type& env){
    for(size_type i = 0, n = arg_procs.size(); i < n; ++i)
      arg_vals[i] = data_cast(arg_procs[i](env));
  }


  // Application of CPS-block defined procedure in a CPS block
  exe_type analyze_CPS_block_application_of_CPS_proc(scm_list& exp,const bool tail_call){
    auto op_proc  = scm_analyze(operator_of(exp),false,true);
    auto arg_exps = operands(exp);
    std::vector<exe_type> arg_procs(arg_exps.size());
    for(size_type i = 0, n = arg_exps.size(); i < n; ++i)
      arg_procs[i] = scm_analyze(scm_list_cast(arg_exps[i]),false,true);
    return [op_proc=std::move(op_proc),arg_procs=std::move(arg_procs),
            tail_call=std::move(tail_call)](env_type& env)mutable{
      auto proc = op_proc(env);
      // Pass the result of the proc to the current continuation IFF
      //   proc was defined OUTSIDE of a scm->cps block
      if(procedure_defined_outside_of_CPS_block(proc)) {
        // Extract the continuation form the parameter list as needed
        auto continuation = (*arg_procs.rbegin())(env);
        bool passing_continuation = procedure_requires_continuation(proc);
        scm_list arg_vals(arg_procs.size() - !passing_continuation);
        // Eval each arg's exec proc to obtain the actual arg values
        if(!passing_continuation) {
          arg_procs.pop_back();
          eval_application_arg_procs(arg_procs,arg_vals,env);
          // Pass the result of the proc to the continuation
          auto result_arg = scm_list(1,data_cast(execute_application(proc,arg_vals,env)));
          // Pass the result of the proc to the continuation
          return execute_application(continuation,result_arg,env,tail_call);
        }
        // Apply the proc w/ the continuation
        size_type i = 0, n = arg_procs.size()-1;
        for(; i < n; ++i) arg_vals[i] = data_cast(arg_procs[i](env));
        arg_vals[n] = data_cast(continuation); // don't re-eval the continuation
        return execute_application(proc,arg_vals,env);
      // Else, apply the proc defined IN the CPS block as-is
      } else {
        scm_list arg_vals(arg_procs.size());
        eval_application_arg_procs(arg_procs,arg_vals,env);
        return execute_application(proc,arg_vals,env,tail_call);
      }
    };
  }


  // Application of a macro OR non-CPS-block defined procedure entity in a CPS block
  exe_type analyze_CPS_block_application_of_non_CPS_proc(scm_list& exp,const bool tail_call){
    auto arg_exps = operands(exp);
    // Add sentinel value as needed (signals possible empty macro application)
    if(arg_exps.empty()) {
      arg_exps.push_back(scm_list(2));
      arg_exps[0].exp[0] = symconst::quote, arg_exps[0].exp[1] = symconst::sentinel_arg;
    }
    // Save name of invoking entity (iff a symbol) to check for a possible macro
    sym_type op_name = exp[0].is_type(types::sym) ? exp[0].sym : "";
    // If possible analysis-time macro, expand and return analysis of the expansion
    if(application_is_a_potential_macro(op_name,G::ANALYSIS_TIME_MACRO_LABEL_REGISTRY)) {
      if(scm_list expanded; expand_macro_if_in_env(op_name, arg_exps, G::GLOBAL_ENVIRONMENT_POINTER, expanded)) {
        return scm_analyze(generate_fundamental_form_cps(expanded),tail_call,true);
      } else {
        THROW_ERR("'core-syntax expression (label \"" << op_name 
          << "\") didn't match any patterns!" << EXP_ERR(exp));
      }
    }
    // If possible macro, expand the application if so, else analyze args at eval
    return [arg_exps=std::move(arg_exps),op_name=std::move(op_name),exp=std::move(exp),
            tail_call=std::move(tail_call)](env_type& env)mutable{
      // check for a possible macro instance, & expand/cps/eval it if so
      if(scm_list expanded; expand_macro_if_in_env(op_name, arg_exps, env, expanded))
        return scm_analyze(generate_fundamental_form_cps(expanded),tail_call,true)(env);
      // else convert the function application to CPS & eval it
      if(exp.size() == 1) exp.push_back(symconst::sentinel_arg); // add sentinel arg as needed
      return scm_analyze(cps_expand_application(exp),tail_call,true)(env);
    };
  }


  // Macro/procedure application in a CPS block
  exe_type analyze_CPS_block_application(scm_list& exp,const bool tail_call=false){
    // If application is already expanded (given a continuation as last arg),
    //   GUARANTEED such is applying to a function and NOT a macro
    if(data_is_continuation_parameter(*exp.rbegin()))
      return analyze_CPS_block_application_of_CPS_proc(exp,tail_call);
    // If application is NOT already expanded
    return analyze_CPS_block_application_of_non_CPS_proc(exp,tail_call);
  }

  /******************************************************************************
  * ANALYSIS & EVALUATION
  ******************************************************************************/

  // -- EVAL 
  scm_list scm_eval(scm_list&& exp, env_type& env) { // evaluate expression environment
    return scm_analyze(std::move(exp))(env);
  }


  // Analyzes the operator & operands, then returns an exec proc passing 
  //   both the operator/operand proc exec's to 'execute-application'
  //   (after having checked for macro use as well)
  exe_type analyze_application(scm_list& exp,const bool tail_call=false,const bool cps_block=false){
    // If in a scm->cps block
    if(cps_block && is_cps_application(exp)) {
      exp.erase(exp.begin()); // Rm the cps-application prefix
      return analyze_CPS_block_application(exp,tail_call);
    }
    auto op_proc  = scm_analyze(operator_of(exp),false,cps_block);
    auto arg_exps = operands(exp);
    // Save name of invoking entity (iff a symbol) to check for a possible macro
    sym_type op_name = exp[0].is_type(types::sym) ? exp[0].sym : "";
    // If possible analysis-time macro, expand and return analysis of the expansion
    if(application_is_a_potential_macro(op_name,G::ANALYSIS_TIME_MACRO_LABEL_REGISTRY)) {
      if(scm_list expanded; expand_macro_if_in_env(op_name, arg_exps, G::GLOBAL_ENVIRONMENT_POINTER, expanded)) {
        return scm_analyze(std::move(expanded),false,cps_block);
      } else {
        THROW_ERR("'core-syntax expression (label \"" << op_name 
          << "\") didn't match any patterns!" << EXP_ERR(exp));
      }
    }
    // If _NOT_ a possible macro, analyze the applicator's args ahead of time
    if(!application_is_a_potential_macro(op_name,G::MACRO_LABEL_REGISTRY)) {
      std::vector<exe_type> arg_procs(arg_exps.size());
      for(size_type i = 0, n = arg_exps.size(); i < n; ++i)
        arg_procs[i] = scm_analyze(scm_list_cast(arg_exps[i]),false,cps_block);
      return [op_proc=std::move(op_proc),arg_procs=std::move(arg_procs),
              tail_call=std::move(tail_call)](env_type& env){
        scm_list arg_vals(arg_procs.size());
        eval_application_arg_procs(arg_procs,arg_vals,env);
        return execute_application(op_proc(env),arg_vals,env,tail_call);
      };
    }
    // If possible macro, expand the application if so, else analyze args at eval
    return [op_proc=std::move(op_proc),arg_exps=std::move(arg_exps),
            op_name=std::move(op_name),tail_call=std::move(tail_call),
            cps_block=std::move(cps_block)](env_type& env){
      // check for a possible macro instance, & expand/eval it if so
      if(scm_list expanded; expand_macro_if_in_env(op_name, arg_exps, env, expanded))
        return scm_analyze(std::move(expanded),false,cps_block)(env);
      // eval each arg's exec proc to obtain the actual arg values
      scm_list arg_vals(arg_exps.size());
      for(size_type i = 0, n = arg_exps.size(); i < n; ++i)
        arg_vals[i] = data_cast(scm_analyze(scm_list_cast(arg_exps[i]),false,cps_block)(env));
      return execute_application(op_proc(env),arg_vals,env,tail_call);
    };
  }


  // -- ANALYZE (SYNTAX)
  void throw_unknown_analysis_anomalous_error(const scm_list& exp) {
    scm_string err_str("ANALYZE: Invalid Syntax (received unknown expression)!"
                       "\n         Triggered By: " + cio_expr_str<&data::write>(exp) + 
                       "\n         Profile of data in expression:");
    for(size_type i = 0, n = exp.size(); i < n; ++i)
      err_str += (("\n         " + std::to_string(i+1) + ". " + 
                    exp[i].write() + " of type \"") + exp[i].type_name()) + '"';
    THROW_ERR(err_str << "\n         => If you believe this is a bug, please send your code"
                         "\n            to jrandleman@scu.edu to fix the interpreter's bug!");
  }


  exe_type scm_analyze(scm_list&& exp,const bool tail_call,const bool cps_block) { // analyze expression
    if(exp.empty())                         THROW_ERR("Can't eval an empty expression!"<<EXP_ERR("()"));
    else if(is_self_evaluating(exp)) return [exp=std::move(exp)](env_type&){return exp;};
    else if(is_scm_cps(exp))         return analyze_scm_cps(exp);
    else if(is_cps_quote(exp))       return analyze_cps_quote(exp,cps_block);
    else if(is_quoted(exp))          return analyze_quoted(exp);
    else if(is_assignment(exp))      return analyze_assignment(exp,cps_block);
    else if(is_definition(exp))      return analyze_definition(exp,cps_block);
    else if(is_if(exp))              return analyze_if(exp,tail_call,cps_block);
    else if(is_and(exp))             return analyze_and(exp,tail_call,cps_block);
    else if(is_or(exp))              return analyze_or(exp,tail_call,cps_block);
    else if(is_lambda(exp))          return analyze_lambda(exp,cps_block);
    else if(is_begin(exp))           return analyze_sequence(begin_actions(exp),tail_call,cps_block);
    else if(is_scons(exp))           return analyze_scons(exp,cps_block);
    else if(is_stream(exp))          return analyze_stream(exp,cps_block);
    else if(is_delay(exp))           return analyze_delay(exp,cps_block);
    else if(is_cond(exp))            return scm_analyze(convert_cond_if(exp,cps_block),tail_call,cps_block);
    else if(is_case(exp))            return scm_analyze(convert_case_cond(exp),tail_call,cps_block);
    else if(is_let(exp))             return scm_analyze(convert_let_combination(exp,cps_block),tail_call,cps_block);
    else if(is_let_star(exp))        return scm_analyze(convert_let_star_nested_lets(exp,cps_block),tail_call,cps_block);
    else if(is_letrec(exp))          return scm_analyze(convert_letrec_let(exp,cps_block),tail_call,cps_block);
    else if(is_do(exp))              return scm_analyze(convert_do_letrec(exp),tail_call,cps_block);
    else if(is_quasiquote(exp))      return analyze_quasiquote(exp,cps_block);
    else if(is_core_syntax(exp))     return analyze_core_syntax(exp,cps_block);
    else if(is_define_syntax(exp))   return analyze_define_syntax(exp,cps_block);
    else if(is_let_syntax(exp))      return analyze_let_syntax(exp,tail_call,cps_block);
    else if(is_letrec_syntax(exp))   return analyze_letrec_syntax(exp,tail_call,cps_block);
    else if(is_syntax_rules(exp))    return analyze_syntax_rules(exp);
    else if(is_vector_literal(exp))         THROW_ERR("Misplaced keyword 'vector-literal outside of a quotation! -- ANALYZE"   <<EXP_ERR(exp));
    else if(is_unquote(exp))                THROW_ERR("Misplaced keyword 'unquote outside of 'quasiquote ! -- ANALYZE"         <<EXP_ERR(exp));
    else if(is_unquote_splicing(exp))       THROW_ERR("Misplaced keyword 'unquote-splicing outside of 'quasiquote ! -- ANALYZE"<<EXP_ERR(exp));
    else if(is_application(exp))     return analyze_application(exp,tail_call,cps_block);
    else if(is_variable(exp)){
      return [exp=std::move(exp)](env_type& env){
        return scm_list_cast(lookup_variable_value(exp[0].sym,env));
      };
    }
    throw_unknown_analysis_anomalous_error(exp);
    return exe_type();
  }

  /******************************************************************************
  * GLOBAL ENVIRONMENT SETUP
  ******************************************************************************/

  void set_default_global_environment() {
    G::GLOBAL_ENVIRONMENT_POINTER = make_env();
    G::GLOBAL_ENVIRONMENT_POINTER = extend_environment(
      primitive_procedure_names(),
      primitive_procedure_objects(),
      G::GLOBAL_ENVIRONMENT_POINTER
    );
    define_variable(symconst::true_t,  G::TRUE_DATA_BOOLEAN,  G::GLOBAL_ENVIRONMENT_POINTER);
    define_variable(symconst::false_t, G::FALSE_DATA_BOOLEAN, G::GLOBAL_ENVIRONMENT_POINTER);
    define_variable("fl-precision", num_type(num_type::INEXACT_PRECISION), G::GLOBAL_ENVIRONMENT_POINTER);
    define_variable("fl-max",       num_type(num_type::INEXACT_MAX),       G::GLOBAL_ENVIRONMENT_POINTER);
    define_variable("fl-min",       num_type(num_type::INEXACT_MIN),       G::GLOBAL_ENVIRONMENT_POINTER);
    define_variable("fl-epsilon",   num_type(num_type::INEXACT_EPSILON),   G::GLOBAL_ENVIRONMENT_POINTER);
    define_variable("stream-null",          symconst::emptylist,    G::GLOBAL_ENVIRONMENT_POINTER);
    define_variable(symconst::null_env,     symconst::null_env,     G::GLOBAL_ENVIRONMENT_POINTER);
    define_variable(symconst::local_env,    symconst::local_env,    G::GLOBAL_ENVIRONMENT_POINTER);
    define_variable(symconst::global_env,   symconst::global_env,   G::GLOBAL_ENVIRONMENT_POINTER);
    define_variable(symconst::sentinel_arg, symconst::sentinel_arg, G::GLOBAL_ENVIRONMENT_POINTER);
    evaluate_primitives_written_in_heist_scheme(G::GLOBAL_ENVIRONMENT_POINTER);
  }

  /******************************************************************************
  * GLOBAL PORT REGISTRY CLEANUP
  ******************************************************************************/

  void close_port_registry()noexcept{
    for(size_type i = 2, n = G::PORT_REGISTRY.size(); i < n; ++i)
      if(G::PORT_REGISTRY[i] && 
         G::PORT_REGISTRY[i]!=stdout && G::PORT_REGISTRY[i]!=stderr &&
         G::PORT_REGISTRY[i]!=stdin){
        fclose(G::PORT_REGISTRY[i]);
        G::PORT_REGISTRY[i] = nullptr;
      }
  }

  /******************************************************************************
  * REPL DRIVER LOOP
  ******************************************************************************/

  void announce_input(FILE* outs)noexcept{fputs(G::REPL_PROMPT.c_str(), outs);}
  void indent_input(FILE* outs)  noexcept{fputs(G::REPL_TAB.c_str(), outs);}


  // Read & parse user expressions
  scm_list read_user_input(FILE* outs, FILE* ins, const bool& in_repl){
    scm_string input, tmp_buffer;
    scm_list abstract_syntax_tree;
    int ch;
    for(;;) {
      // Read input
      fflush(outs);
      tmp_buffer.clear();
      while((ch = fgetc(ins)) != '\n') tmp_buffer += ch;
      // Try parsing the expression, & read more input if unsuccessful 
      try {
        // Return AST if successfully parsed an expression
        parse_input_exp(input+'\n'+tmp_buffer,abstract_syntax_tree);
        return abstract_syntax_tree;
      } catch(const READER_ERROR& read_error) {
        // Continue parsing the current expression/string if incomplete
        if(is_non_repl_reader_error(read_error)) {
          input += '\n'+tmp_buffer;
          if(read_error == READER_ERROR::incomplete_expression) 
            indent_input(outs);
        // Alert user if read an invalid expression, then reprompt for input
        } else {
          if(!input.empty()) alert_reader_error(outs,read_error,input+'\n'+tmp_buffer);
          else               alert_reader_error(outs,read_error,tmp_buffer);
          fputc('\n', outs);
          if(in_repl) announce_input(outs);
          abstract_syntax_tree.clear(), input.clear();
        }
      // Alert user if detected unparsable input (-:- ANOMALY -:-)
      } catch(const size_type& read_error_index) {
        if(!input.empty()) alert_reader_error(outs,read_error_index,input+'\n'+tmp_buffer);
        else               alert_reader_error(outs,read_error_index,tmp_buffer);
        fputc('\n', outs);
        if(in_repl) announce_input(outs);
        abstract_syntax_tree.clear(), input.clear();
      }
    }
    return abstract_syntax_tree;
  }
} // End of namespace heist


// Account for whether REPL should print a newline
#ifndef HEIST_CPP_INTEROP_HPP_ // @NOT-EMBEDDED-IN-C++
#ifndef HEIST_INTERPRETING_COMPILED_AST // @ONLY-INTERPRETER
void print_repl_newline(const bool& printed_data)noexcept{ // after printing data
  if(!heist::G::LAST_PRINTED_NEWLINE_TO_STDOUT&&(printed_data||heist::G::LAST_PRINTED_TO_STDOUT))
    putchar('\n');
  heist::G::LAST_PRINTED_NEWLINE_TO_STDOUT = heist::G::LAST_PRINTED_TO_STDOUT = false;
}

void print_repl_newline()noexcept{ // after printing an error
  putchar('\n'), heist::G::LAST_PRINTED_NEWLINE_TO_STDOUT=heist::G::LAST_PRINTED_TO_STDOUT=false;
}

void account_for_whether_printed_data(const heist::scm_list& val,bool& printed_data)noexcept{
  printed_data = !val.empty() && !val[0].is_type(heist::types::dne);
}


// Print output object
void user_print(FILE* outs, heist::scm_list& object)noexcept{
  if(heist::is_compound_procedure(object) || heist::is_primitive_procedure(object))
    fprintf(outs, "#<procedure%s>", heist::procedure_name(object).c_str());
  else if(heist::is_delay(object) && object.size() > 1)
    fputs("#<delay>", outs);
  else
    fputs(object[0].pprint().c_str(), outs);
  fflush(outs);
}


// Wrap each entry in "scm->cps" (w/ "id" bound as the topmost cont.) 
//   if "-cps" cmd-line flag passed
void cpsify_inputs(heist::scm_list& AST) {
  const heist::size_type n = AST.size();
  heist::scm_list CPS_AST(n);
  for(heist::size_type i = 0; i < n; ++i) {
    CPS_AST[i] = heist::scm_list(2);
    CPS_AST[i].exp[0] = heist::scm_list(2);
    CPS_AST[i].exp[0].exp[0] = heist::symconst::scm_cps;
    CPS_AST[i].exp[0].exp[1] = AST[i];
    CPS_AST[i].exp[1] = "id";
  }
  AST = CPS_AST;
}


void driver_loop() {
  bool printed_data = true;
  print_repl_newline(printed_data);
  for(;;) {
    heist::announce_input(stdout);
    auto AST = heist::read_user_input(stdout,stdin); // AST = Abstract Syntax Tree
    if(heist::G::USING_CPS_CMD_LINE_FLAG) cpsify_inputs(AST);
    // Eval each expression given
    for(const auto& input : AST) {
      try {
        auto value = heist::scm_eval(heist::scm_list_cast(input),heist::G::GLOBAL_ENVIRONMENT_POINTER);
        account_for_whether_printed_data(value,printed_data);
        user_print(stdout, value);
        print_repl_newline(printed_data);
      } catch(const heist::SCM_EXCEPT& eval_throw) {
        if(eval_throw == heist::SCM_EXCEPT::EXIT) { puts("Adios!"); return; }
        if(eval_throw == heist::SCM_EXCEPT::JUMP)
          PRINT_ERR("Uncaught JUMP procedure! JUMPed value: " 
            << PROFILE(heist::G::JUMP_GLOBAL_PRIMITIVE_ARGUMENT));
        print_repl_newline();
      } catch(...) {
        PRINT_ERR("Uncaught C++ Exception Detected! -:- BUG ALERT -:-"
             "\n     Triggered By: " << input << 
             "\n  => Please send your code to jrandleman@scu.edu to fix"
             "\n     the interpreter's bug!"
             "\n  => Terminating Heist Scheme Interpretation.");
        return;
      }
    }
  }
}

/******************************************************************************
* COMMAND LINE ARGUMENT VALIDATION
******************************************************************************/

bool confirm_valid_command_line_args(int argc,char* argv[],int& script_pos,
                                     int& compile_pos,std::string& compile_as)noexcept{
  if(argc == 1) return true;
  
  constexpr const char * const cmd_line_options = 
    "\n> Interpret Script:    -script <script-filename>"
    "\n> Compile Script:      -compile <script-filename> <optional-compiled-filename>"
    "\n> With CPS Evaluation: -cps"
    "\n> Disable ANSI Colors: -nansi"
    "\n> Case Insensitivity:  -ci"
    "\n> Terminating Heist Scheme Interpretation.\n\n";

  // Validate argument layout
  if(argc > 7) {
    fprintf(stderr, "\n> Invalid # of command-line args (given %d)!%s", argc-1, cmd_line_options);
    return false;
  }

  // Parse input arguments
  for(int i = 1; i < argc; ++i) {
    if(std::string cmd_flag(argv[i]); cmd_flag == "-ci") {
      heist::G::USING_CASE_SENSITIVE_SYMBOLS = false;
    } else if(cmd_flag == "-nansi") {
      heist::G::USING_ANSI_ESCAPE_SEQUENCES = false;
    } else if(cmd_flag == "-cps") {
      heist::G::USING_CPS_CMD_LINE_FLAG = true;
    } else if(cmd_flag == "-script") {
      if(i == argc-1) {
        fprintf(stderr,"\n> \"-script\" wasn't followed by a file!%s",cmd_line_options);
        return false;
      }
      script_pos = ++i;
    } else if(cmd_flag == "-compile") {
      if(i == argc-1) {
        fprintf(stderr,"\n> \"-compile\" wasn't followed by a file!%s",cmd_line_options);
        return false;
      }
      compile_pos = ++i;
      if(std::string next_cmd(argv[i]); i < argc-1 && next_cmd != "-nansi" && 
         next_cmd != "-script" && next_cmd != "-compile" && next_cmd != "-ci")
        compile_as = argv[++i];
    } else {
      fprintf(stderr,"\n> Invalid command-line flag \"%s\"!%s",argv[i],cmd_line_options);
      return false;
    }
  }
  return true;
}

/******************************************************************************
* INTERPRET SCRIPT HELPER FUNCTION
******************************************************************************/

int load_script(char* argv[], const int& script_pos){
  // Load the script & immediately exit
  heist::scm_list load_args(2 + heist::G::USING_CPS_CMD_LINE_FLAG);
  load_args[0] = heist::make_str(argv[script_pos]);
  load_args[1 + heist::G::USING_CPS_CMD_LINE_FLAG] = heist::G::GLOBAL_ENVIRONMENT_POINTER;
  // Bind "id" as the topmost continuation if "-cps" was passed
  if(heist::G::USING_CPS_CMD_LINE_FLAG) {
    load_args[1] = heist::scm_list(3);
    load_args[1].exp[0] = heist::symconst::primitive;
    load_args[1].exp[1] = (heist::prm_type)[](heist::scm_list& args){return args[0];};
    load_args[1].exp[2] = "id";
  }
  try {
    if(heist::G::USING_CPS_CMD_LINE_FLAG)
      heist::primitive_CPS_LOAD(load_args);
    else
      heist::primitive_LOAD(load_args);
  } catch(const heist::SCM_EXCEPT& eval_throw) {
    if(eval_throw == heist::SCM_EXCEPT::JUMP)
      PRINT_ERR("Uncaught JUMP procedure! JUMPed value: " 
        << PROFILE(heist::G::JUMP_GLOBAL_PRIMITIVE_ARGUMENT));
    /* catch errors already output to stdout */ 
    putchar('\n');
  } catch(...) {
    /* catch uncaught C++ exceptions -:- ANOMALY -:- */
    PRINT_ERR(afmt(heist::AFMT_1) << 
      "\nUncaught C++ Exception Detected! -:- BUG ALERT -:-"
      "\n  => While interpreting script \"" << argv[script_pos] << "\""
      "\n  => Please send your code to jrandleman@scu.edu to fix"
      "\n     the interpreter's bug!"
      "\n  => Terminating Heist Scheme Interpretation.\n\n" << afmt(heist::AFMT_0));
    heist::close_port_registry();
    return 1;
  }
  heist::close_port_registry();
  return 0;
}

/******************************************************************************
* COMPILE SCRIPT HELPER FUNCTION
******************************************************************************/

int compile_script(char* argv[], const int& compile_pos, std::string& compile_as){
  // Compile the script & immediately exit
  heist::scm_list compile_args(2);
  compile_args[0] = heist::make_str(argv[compile_pos]);
  compile_args[1] = heist::make_str(compile_as);
  try {
    if(heist::G::USING_CPS_CMD_LINE_FLAG)
      heist::primitive_CPS_COMPILE(compile_args);
    else
      heist::primitive_COMPILE(compile_args);
  } catch(const heist::SCM_EXCEPT& eval_throw) {
    if(eval_throw == heist::SCM_EXCEPT::JUMP)
      PRINT_ERR("Uncaught JUMP procedure! JUMPed value: " 
        << PROFILE(heist::G::JUMP_GLOBAL_PRIMITIVE_ARGUMENT));
    /* catch errors already output to stdout */ 
    putchar('\n');
  } catch(...) {
    /* catch uncaught C++ exceptions -:- ANOMALY -:- */
    PRINT_ERR(afmt(heist::AFMT_1) << 
      "\nUncaught C++ Exception Detected! -:- BUG ALERT -:-"
      "\n  => While compiling script \"" << argv[compile_pos] << "\""
      "\n  => Please send your code to jrandleman@scu.edu to fix"
      "\n     the interpreter's bug!"
      "\n  => Terminating Heist Scheme Interpretation.\n\n" << afmt(heist::AFMT_0));
    heist::close_port_registry();
    return 1;
  }
  heist::close_port_registry();
  return 0;
}

/******************************************************************************
* MAIN INTERPRETER EXECUTION
******************************************************************************/

int main(int argc, char* argv[]) {
  // Validate arguments
  int script_pos = -1, compile_pos = -1;
  std::string compile_as = "HEIST_COMPILER_OUTPUT.cpp";
  if(!confirm_valid_command_line_args(argc,argv,script_pos,compile_pos,compile_as)) 
    return 1;
  // Set up the environment (allocates & fills G::GLOBAL_ENVIRONMENT_POINTER)
  heist::set_default_global_environment();
  // Interpret a Script (as needed)
  if(script_pos != -1) 
    return load_script(argv, script_pos);
  // Compile a Script (as needed)
  if(compile_pos != -1) 
    return compile_script(argv, compile_pos, compile_as);
  // Run the REPL
  puts("Heist Scheme Version 5.0\nEnter '(exit)' to Terminate REPL");
  driver_loop();
  heist::close_port_registry();
  return 0;
}

/******************************************************************************
* MAIN COMPILED-AST EXECUTION
******************************************************************************/

#else // @ONLY-COMPILER
void interpret_premade_AST_code(){
  heist::set_default_global_environment();
  POPULATE_HEIST_PRECOMPILED_READ_AST_EXPS();
  for(const auto& input : HEIST_PRECOMPILED_READ_AST_EXPS) {
    try {
      heist::scm_eval(heist::scm_list_cast(input),heist::G::GLOBAL_ENVIRONMENT_POINTER);
    } catch(const heist::SCM_EXCEPT& eval_throw) {
      if(eval_throw == heist::SCM_EXCEPT::EXIT) return;
      if(eval_throw == heist::SCM_EXCEPT::JUMP)
        PRINT_ERR("Uncaught JUMP procedure! JUMPed value: " 
          << PROFILE(heist::G::JUMP_GLOBAL_PRIMITIVE_ARGUMENT));
    } catch(...) {
      PRINT_ERR("Uncaught C++ Exception Detected! -:- BUG ALERT -:-"
           "\n     Triggered By: " << input << 
           "\n  => Please send your source & compiled code to jrandleman@scu.edu to fix"
           "\n     the interpreter's bug!"
           "\n  => Terminating Heist Scheme Interpretation.");
      return;
    }
  }
}

int main() {
  interpret_premade_AST_code(); 
  heist::close_port_registry();
  return 0;
}
#endif // @ONLY-COMPILER
#undef afmt
#undef HEIST_DIRECTORY_FILE_PATH
#undef ERR_HEADER
#undef BAD_SYNTAX
#undef EXP_ERR
#undef FCN_ERR
#undef PROFILE
#undef PRINT_ERR
#undef THROW_ERR
#endif // @NOT-EMBEDDED-IN-C++
#endif