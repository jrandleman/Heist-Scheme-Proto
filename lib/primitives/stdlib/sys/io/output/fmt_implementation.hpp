// Author: Jordan Randleman -- jordanran199@gmail.com -- fmt_implementation.hpp
// => Defines "fmt:" & "string->art" C++ primitives for the Heist Scheme Interpreter

// The below has been adapted from my "color.h" C library:
// => https://github.com/jrandleman/C-Libraries/tree/master/C-CPP-Colors

// Sources For ANSI Esc Codes: 
// 1) https://www.lihaoyi.com/post/BuildyourownCommandLinewithANSIescapecodes.html
// 2) http://www.climagic.org/mirrors/VT100_Escape_Codes.html

#ifndef HEIST_SCHEME_CORE_STDLIB_OUTPUT_FMT_IMPLEMENTATION_HPP_
#define HEIST_SCHEME_CORE_STDLIB_OUTPUT_FMT_IMPLEMENTATION_HPP_

namespace heist_fmt {
  data generate_ANSI_fmt_string(const char* name, const data_vector& args, const char* ansi_str){
    if(!args.empty()) HEIST_THROW_ERR('\''<<name<<" doesn't take any args!\n     ("<<name<<')'<<HEIST_FCN_ERR(name,args));
    if(G.USING_ANSI_ESCAPE_SEQUENCES) return make_str(ansi_str);
    return make_str("");
  }
}

/*******************************************************************************
 * RESET STYLE / CLEAR SCREEN
*******************************************************************************/

// reset style settings
data primitive_fmt_reset(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:reset",args,"\x1b[0m");}

// clear current screen
data primitive_fmt_clear(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:clear",args,"\x1b[2J");}

/*******************************************************************************
 * TEXT DECORATION - BOLD, UNDERLINE, REVERSE BACKGROUND/TEXT COLORS
*******************************************************************************/

// bold, underline, & reverse background/text fmts
data primitive_fmt_bold(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bold",args,"\x1b[1m");}
data primitive_fmt_line(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:line",args,"\x1b[4m");}
data primitive_fmt_rev(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:rev",args,"\x1b[7m");}

/*******************************************************************************
 * TEXT COLORS
*******************************************************************************/

// basic 8 fmts, standard amongst even basic terminals:
data primitive_fmt_black(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:black",args,"\x1b[30m");}
data primitive_fmt_red(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:red",args,"\x1b[31m");}
data primitive_fmt_green(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:green",args,"\x1b[32m");}
data primitive_fmt_yellow(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:yellow",args,"\x1b[33m");}
data primitive_fmt_blue(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:blue",args,"\x1b[34m");}
data primitive_fmt_magenta(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:magenta",args,"\x1b[35m");}
data primitive_fmt_cyan(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:cyan",args,"\x1b[36m");}
data primitive_fmt_white(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:white",args,"\x1b[37m");}

// each of 8 basic fmts extended along their own 8 gradients
// from 1 = dark, to 8 = bright
data primitive_fmt_black1(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:black1",args,"\x1b[38;5;0m");}
data primitive_fmt_black2(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:black2",args,"\x1b[38;5;233m");}
data primitive_fmt_black3(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:black3",args,"\x1b[38;5;234m");}
data primitive_fmt_black4(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:black4",args,"\x1b[38;5;235m");}
data primitive_fmt_black5(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:black5",args,"\x1b[38;5;237m");}
data primitive_fmt_black6(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:black6",args,"\x1b[38;5;238m");}
data primitive_fmt_black7(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:black7",args,"\x1b[38;5;239m");}
data primitive_fmt_black8(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:black8",args,"\x1b[38;5;8m");}
data primitive_fmt_red1(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:red1",args,"\x1b[38;5;52m");}
data primitive_fmt_red2(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:red2",args,"\x1b[38;5;88m");}
data primitive_fmt_red3(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:red3",args,"\x1b[38;5;1m");}
data primitive_fmt_red4(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:red4",args,"\x1b[38;5;124m");}
data primitive_fmt_red5(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:red5",args,"\x1b[38;5;160m");}
data primitive_fmt_red6(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:red6",args,"\x1b[38;5;9m");}
data primitive_fmt_red7(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:red7",args,"\x1b[38;5;196m");}
data primitive_fmt_red8(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:red8",args,"\x1b[38;5;197m");}
data primitive_fmt_green1(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:green1",args,"\x1b[38;5;22m");}
data primitive_fmt_green2(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:green2",args,"\x1b[38;5;28m");}
data primitive_fmt_green3(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:green3",args,"\x1b[38;5;70m");}
data primitive_fmt_green4(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:green4",args,"\x1b[38;5;40m");}
data primitive_fmt_green5(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:green5",args,"\x1b[38;5;82m");}
data primitive_fmt_green6(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:green6",args,"\x1b[38;5;118m");}
data primitive_fmt_green7(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:green7",args,"\x1b[38;5;119m");}
data primitive_fmt_green8(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:green8",args,"\x1b[38;5;156m");}
data primitive_fmt_yellow1(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:yellow1",args,"\x1b[38;5;208m");}
data primitive_fmt_yellow2(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:yellow2",args,"\x1b[38;5;214m");}
data primitive_fmt_yellow3(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:yellow3",args,"\x1b[38;5;220m");}
data primitive_fmt_yellow4(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:yellow4",args,"\x1b[38;5;226m");}
data primitive_fmt_yellow5(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:yellow5",args,"\x1b[38;5;227m");}
data primitive_fmt_yellow6(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:yellow6",args,"\x1b[38;5;228m");}
data primitive_fmt_yellow7(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:yellow7",args,"\x1b[38;5;229m");}
data primitive_fmt_yellow8(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:yellow8",args,"\x1b[38;5;230m");}
data primitive_fmt_blue1(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:blue1",args,"\x1b[38;5;17m");}
data primitive_fmt_blue2(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:blue2",args,"\x1b[38;5;18m");}
data primitive_fmt_blue3(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:blue3",args,"\x1b[38;5;19m");}
data primitive_fmt_blue4(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:blue4",args,"\x1b[38;5;4m");}
data primitive_fmt_blue5(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:blue5",args,"\x1b[38;5;20m");}
data primitive_fmt_blue6(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:blue6",args,"\x1b[38;5;21m");}
data primitive_fmt_blue7(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:blue7",args,"\x1b[38;5;12m");}
data primitive_fmt_blue8(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:blue8",args,"\x1b[38;5;27m");}
data primitive_fmt_magenta1(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:magenta1",args,"\x1b[38;5;54m");}
data primitive_fmt_magenta2(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:magenta2",args,"\x1b[38;5;90m");}
data primitive_fmt_magenta3(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:magenta3",args,"\x1b[38;5;127m");}
data primitive_fmt_magenta4(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:magenta4",args,"\x1b[38;5;164m");}
data primitive_fmt_magenta5(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:magenta5",args,"\x1b[38;5;13m");}
data primitive_fmt_magenta6(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:magenta6",args,"\x1b[38;5;207m");}
data primitive_fmt_magenta7(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:magenta7",args,"\x1b[38;5;213m");}
data primitive_fmt_magenta8(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:magenta8",args,"\x1b[38;5;183m");}
data primitive_fmt_cyan1(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:cyan1",args,"\x1b[38;5;30m");}
data primitive_fmt_cyan2(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:cyan2",args,"\x1b[38;5;6m");}
data primitive_fmt_cyan3(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:cyan3",args,"\x1b[38;5;37m");}
data primitive_fmt_cyan4(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:cyan4",args,"\x1b[38;5;80m");}
data primitive_fmt_cyan5(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:cyan5",args,"\x1b[38;5;14m");}
data primitive_fmt_cyan6(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:cyan6",args,"\x1b[38;5;51m");}
data primitive_fmt_cyan7(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:cyan7",args,"\x1b[38;5;123m");}
data primitive_fmt_cyan8(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:cyan8",args,"\x1b[38;5;159m");}
data primitive_fmt_white1(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:white1",args,"\x1b[38;5;242m");}
data primitive_fmt_white2(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:white2",args,"\x1b[38;5;244m");}
data primitive_fmt_white3(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:white3",args,"\x1b[38;5;248m");}
data primitive_fmt_white4(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:white4",args,"\x1b[38;5;7m");}
data primitive_fmt_white5(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:white5",args,"\x1b[38;5;252m");}
data primitive_fmt_white6(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:white6",args,"\x1b[38;5;15m");}
data primitive_fmt_white7(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:white7",args,"\x1b[38;5;255m");}
data primitive_fmt_white8(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:white8",args,"\x1b[38;5;231m");}

/*******************************************************************************
 * BACKGROUND COLORS - PREFIXED BY "b"
*******************************************************************************/

// basic 8 fmts, standard amongst even basic terminals:
data primitive_fmt_bblack(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bblack",args,"\x1b[40m");}
data primitive_fmt_bred(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bred",args,"\x1b[41m");}
data primitive_fmt_bgreen(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bgreen",args,"\x1b[42m");}
data primitive_fmt_byellow(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:byellow",args,"\x1b[43m");}
data primitive_fmt_bblue(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bblue",args,"\x1b[44m");}
data primitive_fmt_bmagenta(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bmagenta",args,"\x1b[45m");}
data primitive_fmt_bcyan(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bcyan",args,"\x1b[46m");}
data primitive_fmt_bwhite(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bwhite",args,"\x1b[47m");}

// each of 8 basic fmts extended along their own 8 gradients
// from 1 = dark, to 8 = bright
data primitive_fmt_bblack1(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bblack1",args,"\x1b[48;5;0m");}
data primitive_fmt_bblack2(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bblack2",args,"\x1b[48;5;233m");}
data primitive_fmt_bblack3(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bblack3",args,"\x1b[48;5;234m");}
data primitive_fmt_bblack4(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bblack4",args,"\x1b[48;5;235m");}
data primitive_fmt_bblack5(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bblack5",args,"\x1b[48;5;237m");}
data primitive_fmt_bblack6(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bblack6",args,"\x1b[48;5;238m");}
data primitive_fmt_bblack7(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bblack7",args,"\x1b[48;5;239m");}
data primitive_fmt_bblack8(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bblack8",args,"\x1b[48;5;8m");}
data primitive_fmt_bred1(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bred1",args,"\x1b[48;5;52m");}
data primitive_fmt_bred2(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bred2",args,"\x1b[48;5;88m");}
data primitive_fmt_bred3(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bred3",args,"\x1b[48;5;1m");}
data primitive_fmt_bred4(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bred4",args,"\x1b[48;5;124m");}
data primitive_fmt_bred5(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bred5",args,"\x1b[48;5;160m");}
data primitive_fmt_bred6(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bred6",args,"\x1b[48;5;9m");}
data primitive_fmt_bred7(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bred7",args,"\x1b[48;5;196m");}
data primitive_fmt_bred8(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bred8",args,"\x1b[48;5;197m");}
data primitive_fmt_bgreen1(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bgreen1",args,"\x1b[48;5;22m");}
data primitive_fmt_bgreen2(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bgreen2",args,"\x1b[48;5;28m");}
data primitive_fmt_bgreen3(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bgreen3",args,"\x1b[48;5;70m");}
data primitive_fmt_bgreen4(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bgreen4",args,"\x1b[48;5;40m");}
data primitive_fmt_bgreen5(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bgreen5",args,"\x1b[48;5;82m");}
data primitive_fmt_bgreen6(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bgreen6",args,"\x1b[48;5;118m");}
data primitive_fmt_bgreen7(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bgreen7",args,"\x1b[48;5;119m");}
data primitive_fmt_bgreen8(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bgreen8",args,"\x1b[48;5;156m");}
data primitive_fmt_byellow1(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:byellow1",args,"\x1b[48;5;208m");}
data primitive_fmt_byellow2(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:byellow2",args,"\x1b[48;5;214m");}
data primitive_fmt_byellow3(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:byellow3",args,"\x1b[48;5;220m");}
data primitive_fmt_byellow4(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:byellow4",args,"\x1b[48;5;226m");}
data primitive_fmt_byellow5(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:byellow5",args,"\x1b[48;5;227m");}
data primitive_fmt_byellow6(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:byellow6",args,"\x1b[48;5;228m");}
data primitive_fmt_byellow7(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:byellow7",args,"\x1b[48;5;229m");}
data primitive_fmt_byellow8(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:byellow8",args,"\x1b[48;5;230m");}
data primitive_fmt_bblue1(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bblue1",args,"\x1b[48;5;17m");}
data primitive_fmt_bblue2(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bblue2",args,"\x1b[48;5;18m");}
data primitive_fmt_bblue3(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bblue3",args,"\x1b[48;5;19m");}
data primitive_fmt_bblue4(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bblue4",args,"\x1b[48;5;4m");}
data primitive_fmt_bblue5(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bblue5",args,"\x1b[48;5;20m");}
data primitive_fmt_bblue6(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bblue6",args,"\x1b[48;5;21m");}
data primitive_fmt_bblue7(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bblue7",args,"\x1b[48;5;12m");}
data primitive_fmt_bblue8(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bblue8",args,"\x1b[48;5;27m");}
data primitive_fmt_bmagenta1(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bmagenta1",args,"\x1b[48;5;54m");}
data primitive_fmt_bmagenta2(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bmagenta2",args,"\x1b[48;5;90m");}
data primitive_fmt_bmagenta3(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bmagenta3",args,"\x1b[48;5;127m");}
data primitive_fmt_bmagenta4(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bmagenta4",args,"\x1b[48;5;164m");}
data primitive_fmt_bmagenta5(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bmagenta5",args,"\x1b[48;5;13m");}
data primitive_fmt_bmagenta6(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bmagenta6",args,"\x1b[48;5;207m");}
data primitive_fmt_bmagenta7(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bmagenta7",args,"\x1b[48;5;213m");}
data primitive_fmt_bmagenta8(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bmagenta8",args,"\x1b[48;5;183m");}
data primitive_fmt_bcyan1(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bcyan1",args,"\x1b[48;5;30m");}
data primitive_fmt_bcyan2(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bcyan2",args,"\x1b[48;5;6m");}
data primitive_fmt_bcyan3(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bcyan3",args,"\x1b[48;5;37m");}
data primitive_fmt_bcyan4(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bcyan4",args,"\x1b[48;5;80m");}
data primitive_fmt_bcyan5(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bcyan5",args,"\x1b[48;5;14m");}
data primitive_fmt_bcyan6(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bcyan6",args,"\x1b[48;5;51m");}
data primitive_fmt_bcyan7(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bcyan7",args,"\x1b[48;5;123m");}
data primitive_fmt_bcyan8(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bcyan8",args,"\x1b[48;5;159m");}
data primitive_fmt_bwhite1(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bwhite1",args,"\x1b[48;5;242m");}
data primitive_fmt_bwhite2(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bwhite2",args,"\x1b[48;5;244m");}
data primitive_fmt_bwhite3(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bwhite3",args,"\x1b[48;5;248m");}
data primitive_fmt_bwhite4(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bwhite4",args,"\x1b[48;5;7m");}
data primitive_fmt_bwhite5(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bwhite5",args,"\x1b[48;5;252m");}
data primitive_fmt_bwhite6(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bwhite6",args,"\x1b[48;5;15m");}
data primitive_fmt_bwhite7(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bwhite7",args,"\x1b[48;5;255m");}
data primitive_fmt_bwhite8(data_vector&& args){return heist_fmt::generate_ANSI_fmt_string("fmt:bwhite8",args,"\x1b[48;5;231m");}

/******************************************************************************
* PRIVATE INTERFACES: ASCII/WHITESPACE ART PRINTING HELPER FUNCTIONS
******************************************************************************/

#include <cstring>

namespace heist_fmt {
  // max strlen, ASCII/Whitespace art alphabet length, # rows per alpha art letter, '\b' alpha art idx
  static constexpr const std::size_t ALPHA_ART_ALPHABET_LENGTH = 74;
  static constexpr const std::size_t TOTAL_ASCII_ART_ROWS      = 3; // top mid, bot
  static constexpr const std::size_t BACKSPACE_ART_IDX         = ALPHA_ART_ALPHABET_LENGTH - 2;
  static constexpr const std::size_t ANSI_ESC_ART_IDX          = ALPHA_ART_ALPHABET_LENGTH - 1;

  // chars w/ ASCII art equivalents (lowercase letters too, but get uppercased)
  static constexpr const char* VALID_ALPHA_ART_CHARS = "\t\n !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`{|}~\b";

  // ASCII art alphabet sorted by ascii code.
  // => '\b' char repn is missing as the # of '\b's used is evaluated at run-time as per width of the previously printed char. 
  // => each row's last string repns non-lowercase chars also not found in "VALID_ALPHA_ART_CHARS"
  const char* ASCII_ART_ROW_TOP[ALPHA_ART_ALPHABET_LENGTH] = {"\t\t", "newline",  "      ", "  // ", "(/(/ ", "_||_||_ ", "//=||==\\ ",    "@ // ", "((^))   ",     ")) ", " // ",   "\\\\  ", " \\\\|//  ", "   ||    ", "   ", "      ", "    ", "  // ", "//=\\\\ ", "//|  ", "(==\\\\  ", "/==\\\\ ", " //||  ", " /|===) ",   "//==) ",   "(====) ", "((^)) ",   "//==|\\ ",  "    ", "    ", "   ",   "        ", "   ",   "//^\\\\ ", " //==\\\\ ", " //^\\\\  ", "||^\\\\  ", "//===) ",   "||^\\\\  ", "/|==\\ ", "/===\\ ", "//===\\ ",  "|| || ", "==== ", "(====) ",   "||// ",   "/|    ",  "/\\\\  //\\ ", "/\\\\ || ", "//==\\\\ ", "/|==\\\\ ", "//==\\\\ ",   "||^\\\\ ", " //==) ",   "==== ", "/| |\\ ",  "/|  |\\ ",  "/| /\\ |\\ ",   "\\\\ // ", "(\\  /) ",  "/===)  ", "|=] ", "\\\\   ", "[=| ", "//^\\\\ ", "      ", "\\\\ ", "(( ", "|| ", ")) ", "      ",  "###### "};
  const char* ASCII_ART_ROW_MID[ALPHA_ART_ALPHABET_LENGTH] = {"\t\t", "newline",  "      ", " //  ", "     ", "_||_||_ ", "\\\\=||=\\\\ ", " //  ", "//^\\\\// ",   "   ", "||  ",   " || ",   "<==*==> ",   "[==++==] ", "_  ", "+===+ ", "    ", " //  ", "| + | ",   " ||  ", " __//  ",   "  =|| ",   "((=||) ", " \\|==\\  ", "||/=\\ ",  "   //  ", "//^\\\\ ", "\\\\==|| ", "<*> ", "<*> ", "// ",   "[=====] ", "\\\\ ", "  _// ",   "((  _|| ",   "|/===\\| ",  "||-//_ ",   "||     ",   "||  )) ",   "||=   ",  "|==   ",  "|| ==\\ ",  "|===| ", " ||  ", "_ ||   ",   "|((  ",   "||    ",  "||\\\\//|| ",  "||\\\\|| ", "||  || ",   "||==// ",   "|| _|| ",     "||_// ",   " \\==\\  ", " ||  ", "|| || ",   "\\\\  // ", "\\\\//\\\\// ", " )X(  ",   " \\\\//  ", "  //   ", "|   ", " \\\\  ", "  | ", "      ",   "      ", "   ",   "<< ", "|| ", ">> ", "//\\// ", "###### "};
  const char* ASCII_ART_ROW_BOT[ALPHA_ART_ALPHABET_LENGTH] = {"\t\t", "newline",  "      ", "<*>  ", "     ", " || ||  ", "\\==||=// ",    "// @ ", "\\\\_//\\\\ ", "   ", " \\\\ ", "//  ",   " //|\\\\  ", "   ||    ", ")) ", "      ", "<*> ", "//   ", "\\\\=// ", "==== ", "(====/ ",   "\\==// ",  "   ||  ", "\\====/  ",  "\\\\==/ ", "  //   ", "\\\\_// ", "    \\| ",  "<*> ", " )) ", "\\\\ ", "[=====] ", "// ",   "  @   ",   " \\\\(_|| ", "||   || ",   "||__// ",   "\\\\===) ", "||_//  ",   "\\|==/ ", "||    ",  "\\\\==// ", "|| || ", "==== ", "\\\\//   ", "||\\\\ ", "\\===/ ", "|| \\/ || ",   "|| \\// ",  "\\\\==// ", "||     ",   "\\\\==\\\\ ", "|| \\\\ ", "\\==//  ",  " ||  ", "\\\\=// ", " \\\\//  ", " \\/  \\/  ",   "// \\\\ ", "  ||   ",   " (===/ ", "|=] ", "  \\\\ ", "[=| ", "      ",   "+===+ ", "   ",   "(( ", "|| ", ")) ", "      ",  "###### "};
  const char** ASCII_ART_MATRIX[TOTAL_ASCII_ART_ROWS] = {
    ASCII_ART_ROW_TOP, ASCII_ART_ROW_MID, ASCII_ART_ROW_BOT
  };

  // rows in WHITESPACE alphabet art matrix
  static constexpr const std::size_t TOTAL_WHITESPACE_ART_ROWS = 5;

  // WHITESPACE art alphabet sorted by ascii code
  const char* WHITESPACE_ART_ROW_CEIL[ALPHA_ART_ALPHABET_LENGTH]  = {"%s%s%s%s\t\t\t", "%s%s%s%snewline", "%s%s%s%s      ", "%s%s%s\x1b[7m     \x1b[0m%s ", "%s%s\x1b[7m  \x1b[0m%s \x1b[7m  \x1b[0m%s ", "%s%s   \x1b[7m \x1b[0m%s   \x1b[7m \x1b[0m%s    ", "%s%s%s  \x1b[7m       \x1b[0m%s ", "%s%s\x1b[7m  \x1b[0m%s  \x1b[7m  \x1b[0m%s ", "%s%s%s \x1b[7m    \x1b[0m%s     ", "%s%s%s\x1b[7m  \x1b[0m%s ", "%s%s%s  \x1b[7m  \x1b[0m%s ", "%s%s%s\x1b[7m  \x1b[0m%s   ", "%s\x1b[7m  \x1b[0m%s \x1b[7m \x1b[0m%s \x1b[7m  \x1b[0m%s ", "%s%s%s    \x1b[7m \x1b[0m%s     ", "%s%s%s%s   ", "%s%s%s%s      ", "%s%s%s%s    ", "%s%s%s    \x1b[7m  \x1b[0m%s ", "%s%s%s \x1b[7m     \x1b[0m%s  ", "%s%s%s \x1b[7m   \x1b[0m%s    ", "%s%s%s \x1b[7m    \x1b[0m%s    ", "%s%s%s\x1b[7m      \x1b[0m%s ", "%s%s\x1b[7m  \x1b[0m%s \x1b[7m  \x1b[0m%s ", "%s%s%s\x1b[7m      \x1b[0m%s ", "%s%s%s \x1b[7m      \x1b[0m%s  ", "%s%s%s\x1b[7m        \x1b[0m%s ", "%s%s%s \x1b[7m    \x1b[0m%s  ", "%s%s%s \x1b[7m    \x1b[0m%s  ", "%s%s%s%s   ", "%s%s%s%s   ", "%s%s%s%s    ", "%s%s%s%s       ", "%s%s%s%s    ", "%s%s%s \x1b[7m     \x1b[0m%s  ", "%s%s%s \x1b[7m     \x1b[0m%s  ", "%s%s%s \x1b[7m   \x1b[0m%s  ", "%s%s%s\x1b[7m    \x1b[0m%s  ", "%s%s%s \x1b[7m    \x1b[0m%s  ", "%s%s%s\x1b[7m   \x1b[0m%s   ", "%s%s%s \x1b[7m    \x1b[0m%s  ", "%s%s%s \x1b[7m     \x1b[0m%s ", "%s%s%s \x1b[7m    \x1b[0m%s   ", "%s%s\x1b[7m  \x1b[0m%s \x1b[7m  \x1b[0m%s ", "%s%s%s\x1b[7m   \x1b[0m%s ", "%s%s%s\x1b[7m       \x1b[0m%s ", "%s%s\x1b[7m  \x1b[0m%s  \x1b[7m  \x1b[0m%s  ", "%s%s%s\x1b[7m  \x1b[0m%s    ", "%s%s\x1b[7m   \x1b[0m%s   \x1b[7m   \x1b[0m%s ", "%s%s \x1b[7m  \x1b[0m%s   \x1b[7m   \x1b[0m%s ", "%s%s%s \x1b[7m   \x1b[0m%s  ", "%s%s%s\x1b[7m    \x1b[0m%s ", "%s%s%s \x1b[7m   \x1b[0m%s   ", "%s%s%s\x1b[7m    \x1b[0m%s   ", "%s%s%s   \x1b[7m    \x1b[0m%s  ", "%s%s%s\x1b[7m     \x1b[0m%s ", "%s%s\x1b[7m  \x1b[0m%s \x1b[7m  \x1b[0m%s ", "%s%s\x1b[7m    \x1b[0m%s \x1b[7m    \x1b[0m%s ", "%s%s\x1b[7m    \x1b[0m%s     \x1b[7m    \x1b[0m%s ", "%s%s\x1b[7m   \x1b[0m%s   \x1b[7m   \x1b[0m%s ", "%s%s\x1b[7m  \x1b[0m%s   \x1b[7m  \x1b[0m%s ", "%s%s%s\x1b[7m      \x1b[0m%s  ", "%s%s%s\x1b[7m    \x1b[0m%s ", "%s%s%s\x1b[7m  \x1b[0m%s     ", "%s%s%s\x1b[7m    \x1b[0m%s ", "%s%s%s \x1b[7m   \x1b[0m%s  ", "%s%s%s%s       ", "%s%s%s\x1b[7m  \x1b[0m%s  ", "%s%s%s\x1b[7m  \x1b[0m%s ", "%s%s%s\x1b[7m  \x1b[0m%s ", "%s%s%s\x1b[7m  \x1b[0m%s ", "%s%s%s%s        ", "%s%s%s\x1b[7m      \x1b[0m%s "};
  const char* WHITESPACE_ART_ROW_TOP[ALPHA_ART_ALPHABET_LENGTH]   = {"%s%s%s%s\t\t\t", "%s%s%s%snewline", "%s%s%s%s      ", "%s%s%s \x1b[7m   \x1b[0m%s  ", "%s%s\x1b[7m \x1b[0m%s  \x1b[7m \x1b[0m%s  ", "%s%s%s\x1b[7m           \x1b[0m%s ", "%s \x1b[7m  \x1b[0m%s \x1b[7m \x1b[0m%s   \x1b[7m \x1b[0m%s ", "%s%s%s   \x1b[7m  \x1b[0m%s  ", "%s%s\x1b[7m  \x1b[0m%s  \x1b[7m  \x1b[0m%s    ", "%s%s%s \x1b[7m \x1b[0m%s ", "%s%s%s \x1b[7m  \x1b[0m%s  ", "%s%s%s \x1b[7m  \x1b[0m%s  ", "%s%s%s \x1b[7m     \x1b[0m%s  ", "%s%s%s    \x1b[7m \x1b[0m%s     ", "%s%s%s%s   ", "%s%s%s%s      ", "%s%s%s%s    ", "%s%s%s   \x1b[7m  \x1b[0m%s  ", "%s%s\x1b[7m  \x1b[0m%s   \x1b[7m  \x1b[0m%s ", "%s%s\x1b[7m  \x1b[0m%s \x1b[7m \x1b[0m%s    ", "%s%s\x1b[7m  \x1b[0m%s  \x1b[7m  \x1b[0m%s   ", "%s%s\x1b[7m \x1b[0m%s  \x1b[7m  \x1b[0m%s  ", "%s%s\x1b[7m \x1b[0m%s  \x1b[7m \x1b[0m%s  ", "%s%s%s\x1b[7m \x1b[0m%s      ", "%s%s\x1b[7m  \x1b[0m%s    \x1b[7m  \x1b[0m%s ", "%s%s\x1b[7m  \x1b[0m%s   \x1b[7m  \x1b[0m%s  ", "%s%s\x1b[7m  \x1b[0m%s  \x1b[7m  \x1b[0m%s ", "%s%s\x1b[7m  \x1b[0m%s  \x1b[7m  \x1b[0m%s ", "%s%s%s\x1b[7m  \x1b[0m%s ", "%s%s%s\x1b[7m  \x1b[0m%s ", "%s%s%s \x1b[7m  \x1b[0m%s ", "%s%s%s\x1b[7m      \x1b[0m%s ", "%s%s%s\x1b[7m  \x1b[0m%s  ", "%s%s\x1b[7m  \x1b[0m%s   \x1b[7m  \x1b[0m%s ", "%s%s\x1b[7m  \x1b[0m%s   \x1b[7m  \x1b[0m%s ", "%s%s \x1b[7m \x1b[0m%s \x1b[7m \x1b[0m%s  ", "%s%s \x1b[7m \x1b[0m%s \x1b[7m \x1b[0m%s  ", "%s%s\x1b[7m  \x1b[0m%s  \x1b[7m  \x1b[0m%s ", "%s%s\x1b[7m \x1b[0m%s \x1b[7m  \x1b[0m%s  ", "%s%s\x1b[7m  \x1b[0m%s  \x1b[7m  \x1b[0m%s ", "%s%s \x1b[7m \x1b[0m%s   \x1b[7m \x1b[0m%s ", "%s%s%s\x1b[7m  \x1b[0m%s      ", "%s%s \x1b[7m \x1b[0m%s \x1b[7m \x1b[0m%s  ", "%s%s%s \x1b[7m \x1b[0m%s  ", "%s%s   \x1b[7m \x1b[0m%s  \x1b[7m \x1b[0m%s ", "%s%s \x1b[7m \x1b[0m%s \x1b[7m  \x1b[0m%s   ", "%s%s%s \x1b[7m \x1b[0m%s    ", "%s%s \x1b[7m   \x1b[0m%s \x1b[7m   \x1b[0m%s  ", "%s%s \x1b[7m   \x1b[0m%s  \x1b[7m \x1b[0m%s   ", "%s%s\x1b[7m  \x1b[0m%s \x1b[7m  \x1b[0m%s ", "%s%s \x1b[7m \x1b[0m%s \x1b[7m \x1b[0m%s ", "%s%s\x1b[7m  \x1b[0m%s \x1b[7m  \x1b[0m%s  ", "%s%s \x1b[7m \x1b[0m%s \x1b[7m  \x1b[0m%s  ", "%s%s  \x1b[7m  \x1b[0m%s  \x1b[7m  \x1b[0m%s ", "%s\x1b[7m \x1b[0m%s \x1b[7m \x1b[0m%s \x1b[7m \x1b[0m%s ", "%s%s \x1b[7m \x1b[0m%s \x1b[7m \x1b[0m%s  ", "%s%s \x1b[7m  \x1b[0m%s   \x1b[7m  \x1b[0m%s  ", "%s \x1b[7m  \x1b[0m%s   \x1b[7m \x1b[0m%s   \x1b[7m  \x1b[0m%s  ", "%s%s  \x1b[7m  \x1b[0m%s \x1b[7m  \x1b[0m%s   ", "%s%s \x1b[7m  \x1b[0m%s \x1b[7m  \x1b[0m%s  ", "%s%s\x1b[7m \x1b[0m%s  \x1b[7m  \x1b[0m%s   ", "%s%s%s\x1b[7m  \x1b[0m%s   ", "%s%s%s \x1b[7m  \x1b[0m%s    ", "%s%s%s  \x1b[7m  \x1b[0m%s ", "%s%s\x1b[7m  \x1b[0m%s \x1b[7m  \x1b[0m%s ", "%s%s%s%s       ", "%s%s%s \x1b[7m  \x1b[0m%s ", "%s%s%s \x1b[7m \x1b[0m%s ", "%s%s%s\x1b[7m  \x1b[0m%s ", "%s%s%s\x1b[7m \x1b[0m%s  ", "%s%s \x1b[7m   \x1b[0m%s \x1b[7m  \x1b[0m%s ", "%s%s%s\x1b[7m      \x1b[0m%s "};
  const char* WHITESPACE_ART_ROW_MID[ALPHA_ART_ALPHABET_LENGTH]   = {"%s%s%s%s\t\t\t", "%s%s%s%snewline", "%s%s%s%s      ", "%s%s%s  \x1b[7m \x1b[0m%s   ", "%s%s%s%s      ", "%s%s   \x1b[7m \x1b[0m%s   \x1b[7m \x1b[0m%s    ", "%s%s%s  \x1b[7m     \x1b[0m%s   ", "%s%s%s  \x1b[7m  \x1b[0m%s   ", "%s%s \x1b[7m     \x1b[0m%s \x1b[7m  \x1b[0m%s ", "%s%s%s%s   ", "%s%s%s\x1b[7m  \x1b[0m%s   ", "%s%s%s  \x1b[7m  \x1b[0m%s ", "%s%s%s\x1b[7m       \x1b[0m%s ", "%s%s%s\x1b[7m         \x1b[0m%s ", "%s%s%s%s   ", "%s%s%s\x1b[7m     \x1b[0m%s ", "%s%s%s%s    ", "%s%s%s  \x1b[7m  \x1b[0m%s   ", "%s\x1b[7m \x1b[0m%s  \x1b[7m \x1b[0m%s  \x1b[7m \x1b[0m%s ", "%s%s%s   \x1b[7m \x1b[0m%s    ", "%s%s%s   \x1b[7m  \x1b[0m%s    ", "%s%s%s  \x1b[7m  \x1b[0m%s   ", "%s%s%s\x1b[7m     \x1b[0m%s ", "%s%s%s\x1b[7m    \x1b[0m%s   ", "%s%s%s\x1b[7m     \x1b[0m%s    ", "%s%s%s    \x1b[7m  \x1b[0m%s   ", "%s%s%s \x1b[7m    \x1b[0m%s  ", "%s%s%s \x1b[7m    \x1b[0m%s  ", "%s%s%s%s   ", "%s%s%s%s   ", "%s%s%s\x1b[7m  \x1b[0m%s  ", "%s%s%s%s       ", "%s%s%s \x1b[7m  \x1b[0m%s ", "%s%s%s    \x1b[7m  \x1b[0m%s  ", "%s%s\x1b[7m \x1b[0m%s  \x1b[7m    \x1b[0m%s ", "%s%s%s \x1b[7m   \x1b[0m%s  ", "%s%s%s \x1b[7m    \x1b[0m%s ", "%s%s%s\x1b[7m  \x1b[0m%s     ", "%s%s\x1b[7m \x1b[0m%s  \x1b[7m  \x1b[0m%s ", "%s%s%s \x1b[7m  \x1b[0m%s    ", "%s%s%s \x1b[7m   \x1b[0m%s   ", "%s%s\x1b[7m  \x1b[0m%s \x1b[7m    \x1b[0m%s ", "%s%s%s \x1b[7m   \x1b[0m%s  ", "%s%s%s \x1b[7m \x1b[0m%s  ", "%s%s\x1b[7m  \x1b[0m%s \x1b[7m \x1b[0m%s    ", "%s%s%s \x1b[7m   \x1b[0m%s    ", "%s%s%s \x1b[7m \x1b[0m%s    ", "%s \x1b[7m \x1b[0m%s \x1b[7m   \x1b[0m%s \x1b[7m \x1b[0m%s  ", "%s \x1b[7m \x1b[0m%s \x1b[7m  \x1b[0m%s \x1b[7m \x1b[0m%s   ", "%s%s\x1b[7m \x1b[0m%s   \x1b[7m \x1b[0m%s ", "%s%s%s \x1b[7m   \x1b[0m%s ", "%s%s\x1b[7m \x1b[0m%s   \x1b[7m \x1b[0m%s  ", "%s%s%s \x1b[7m   \x1b[0m%s   ", "%s%s%s   \x1b[7m  \x1b[0m%s    ", "%s%s%s  \x1b[7m \x1b[0m%s   ", "%s%s \x1b[7m \x1b[0m%s \x1b[7m \x1b[0m%s  ", "%s%s  \x1b[7m  \x1b[0m%s \x1b[7m  \x1b[0m%s   ", "%s  \x1b[7m  \x1b[0m%s \x1b[7m   \x1b[0m%s \x1b[7m  \x1b[0m%s   ", "%s%s%s   \x1b[7m   \x1b[0m%s    ", "%s%s%s  \x1b[7m   \x1b[0m%s   ", "%s%s%s  \x1b[7m  \x1b[0m%s    ", "%s%s%s\x1b[7m  \x1b[0m%s   ", "%s%s%s  \x1b[7m  \x1b[0m%s   ", "%s%s%s  \x1b[7m  \x1b[0m%s ", "%s%s%s%s      ", "%s%s%s%s       ", "%s%s%s%s    ", "%s%s%s\x1b[7m  \x1b[0m%s ", "%s%s%s\x1b[7m  \x1b[0m%s ", "%s%s%s\x1b[7m  \x1b[0m%s ", "%s%s\x1b[7m  \x1b[0m%s \x1b[7m   \x1b[0m%s  ", "%s%s%s\x1b[7m      \x1b[0m%s "};
  const char* WHITESPACE_ART_ROW_BOT[ALPHA_ART_ALPHABET_LENGTH]   = {"%s%s%s%s\t\t\t", "%s%s%s%snewline", "%s%s%s%s      ", "%s%s%s%s      ", "%s%s%s%s      ", "%s%s%s\x1b[7m           \x1b[0m%s ", "%s\x1b[7m \x1b[0m%s   \x1b[7m \x1b[0m%s \x1b[7m  \x1b[0m%s  ", "%s%s%s \x1b[7m  \x1b[0m%s    ", "%s%s\x1b[7m  \x1b[0m%s  \x1b[7m    \x1b[0m%s  ", "%s%s%s%s   ", "%s%s%s \x1b[7m  \x1b[0m%s  ", "%s%s%s \x1b[7m  \x1b[0m%s  ", "%s%s%s \x1b[7m     \x1b[0m%s  ", "%s%s%s    \x1b[7m \x1b[0m%s     ", "%s%s%s \x1b[7m \x1b[0m%s ", "%s%s%s%s      ", "%s%s%s%s    ", "%s%s%s \x1b[7m  \x1b[0m%s    ", "%s%s\x1b[7m  \x1b[0m%s   \x1b[7m  \x1b[0m%s ", "%s%s%s   \x1b[7m \x1b[0m%s    ", "%s%s  \x1b[7m  \x1b[0m%s   \x1b[7m \x1b[0m%s ", "%s%s\x1b[7m \x1b[0m%s  \x1b[7m  \x1b[0m%s  ", "%s%s%s   \x1b[7m \x1b[0m%s  ", "%s%s%s   \x1b[7m  \x1b[0m%s  ", "%s%s\x1b[7m  \x1b[0m%s  \x1b[7m  \x1b[0m%s   ", "%s%s%s   \x1b[7m  \x1b[0m%s    ", "%s%s\x1b[7m  \x1b[0m%s  \x1b[7m  \x1b[0m%s ", "%s%s%s    \x1b[7m \x1b[0m%s  ", "%s%s%s\x1b[7m  \x1b[0m%s ", "%s%s%s \x1b[7m \x1b[0m%s ", "%s%s%s \x1b[7m  \x1b[0m%s ", "%s%s%s\x1b[7m      \x1b[0m%s ", "%s%s%s\x1b[7m  \x1b[0m%s  ", "%s%s%s   \x1b[7m \x1b[0m%s    ", "%s\x1b[7m \x1b[0m%s \x1b[7m  \x1b[0m%s  \x1b[7m \x1b[0m%s ", "%s%s \x1b[7m \x1b[0m%s \x1b[7m \x1b[0m%s  ", "%s%s \x1b[7m \x1b[0m%s  \x1b[7m \x1b[0m%s ", "%s%s\x1b[7m  \x1b[0m%s  \x1b[7m  \x1b[0m%s ", "%s%s\x1b[7m \x1b[0m%s \x1b[7m  \x1b[0m%s  ", "%s%s\x1b[7m  \x1b[0m%s  \x1b[7m  \x1b[0m%s ", "%s%s%s \x1b[7m \x1b[0m%s     ", "%s\x1b[7m  \x1b[0m%s  \x1b[7m \x1b[0m%s \x1b[7m \x1b[0m%s ", "%s%s \x1b[7m \x1b[0m%s \x1b[7m \x1b[0m%s  ", "%s%s%s \x1b[7m \x1b[0m%s  ", "%s%s \x1b[7m \x1b[0m%s \x1b[7m \x1b[0m%s    ", "%s%s \x1b[7m \x1b[0m%s \x1b[7m  \x1b[0m%s   ", "%s%s \x1b[7m \x1b[0m%s \x1b[7m  \x1b[0m%s ", "%s \x1b[7m \x1b[0m%s  \x1b[7m \x1b[0m%s  \x1b[7m \x1b[0m%s  ", "%s%s \x1b[7m \x1b[0m%s  \x1b[7m   \x1b[0m%s   ", "%s%s\x1b[7m  \x1b[0m%s \x1b[7m  \x1b[0m%s ", "%s%s%s \x1b[7m \x1b[0m%s   ", "%s%s\x1b[7m  \x1b[0m%s \x1b[7m   \x1b[0m%s ", "%s%s \x1b[7m \x1b[0m%s \x1b[7m  \x1b[0m%s  ", "%s%s\x1b[7m  \x1b[0m%s  \x1b[7m  \x1b[0m%s   ", "%s%s%s  \x1b[7m \x1b[0m%s   ", "%s%s \x1b[7m \x1b[0m%s \x1b[7m \x1b[0m%s  ", "%s%s%s   \x1b[7m   \x1b[0m%s    ", "%s%s   \x1b[7m   \x1b[0m%s \x1b[7m   \x1b[0m%s    ", "%s%s  \x1b[7m  \x1b[0m%s \x1b[7m  \x1b[0m%s   ", "%s%s%s   \x1b[7m \x1b[0m%s    ", "%s%s \x1b[7m  \x1b[0m%s  \x1b[7m  \x1b[0m%s ", "%s%s%s\x1b[7m  \x1b[0m%s   ", "%s%s%s   \x1b[7m  \x1b[0m%s  ", "%s%s%s  \x1b[7m  \x1b[0m%s ", "%s%s%s%s      ", "%s%s%s%s       ", "%s%s%s%s    ", "%s%s%s \x1b[7m \x1b[0m%s ", "%s%s%s\x1b[7m  \x1b[0m%s ", "%s%s%s\x1b[7m \x1b[0m%s  ", "%s%s%s%s        ", "%s%s%s\x1b[7m      \x1b[0m%s "};
  const char* WHITESPACE_ART_ROW_FLOOR[ALPHA_ART_ALPHABET_LENGTH] = {"%s%s%s%s\t\t\t", "%s%s%s%snewline", "%s%s%s%s      ", "%s%s%s \x1b[7m   \x1b[0m%s  ", "%s%s%s%s      ", "%s%s   \x1b[7m \x1b[0m%s   \x1b[7m \x1b[0m%s    ", "%s%s%s\x1b[7m       \x1b[0m%s   ", "%s%s\x1b[7m  \x1b[0m%s  \x1b[7m  \x1b[0m%s ", "%s%s \x1b[7m    \x1b[0m%s  \x1b[7m  \x1b[0m%s ", "%s%s%s%s   ", "%s%s%s  \x1b[7m  \x1b[0m%s ", "%s%s%s\x1b[7m  \x1b[0m%s   ", "%s\x1b[7m  \x1b[0m%s \x1b[7m \x1b[0m%s \x1b[7m  \x1b[0m%s ", "%s%s%s    \x1b[7m \x1b[0m%s     ", "%s%s%s\x1b[7m  \x1b[0m%s ", "%s%s%s%s      ", "%s%s%s\x1b[7m   \x1b[0m%s ", "%s%s%s\x1b[7m  \x1b[0m%s     ", "%s%s%s \x1b[7m     \x1b[0m%s  ", "%s%s%s\x1b[7m       \x1b[0m%s ", "%s%s%s\x1b[7m        \x1b[0m%s ", "%s%s%s\x1b[7m     \x1b[0m%s  ", "%s%s%s  \x1b[7m   \x1b[0m%s ", "%s%s%s\x1b[7m    \x1b[0m%s   ", "%s%s%s\x1b[7m     \x1b[0m%s    ", "%s%s%s  \x1b[7m  \x1b[0m%s     ", "%s%s%s \x1b[7m    \x1b[0m%s  ", "%s%s%s \x1b[7m    \x1b[0m%s  ", "%s%s%s%s   ", "%s%s%s\x1b[7m  \x1b[0m%s ", "%s%s%s%s    ", "%s%s%s%s       ", "%s%s%s%s    ", "%s%s%s  \x1b[7m   \x1b[0m%s   ", "%s%s\x1b[7m \x1b[0m%s  \x1b[7m    \x1b[0m%s ", "%s%s\x1b[7m  \x1b[0m%s \x1b[7m  \x1b[0m%s ", "%s%s%s\x1b[7m     \x1b[0m%s ", "%s%s%s \x1b[7m    \x1b[0m%s  ", "%s%s%s\x1b[7m   \x1b[0m%s   ", "%s%s%s \x1b[7m    \x1b[0m%s  ", "%s%s%s\x1b[7m  \x1b[0m%s     ", "%s%s%s \x1b[7m    \x1b[0m%s   ", "%s%s\x1b[7m  \x1b[0m%s \x1b[7m  \x1b[0m%s ", "%s%s%s\x1b[7m   \x1b[0m%s ", "%s%s%s \x1b[7m   \x1b[0m%s    ", "%s%s\x1b[7m  \x1b[0m%s  \x1b[7m   \x1b[0m%s ", "%s%s%s \x1b[7m   \x1b[0m%s  ", "%s%s\x1b[7m  \x1b[0m%s     \x1b[7m  \x1b[0m%s ", "%s%s\x1b[7m  \x1b[0m%s   \x1b[7m  \x1b[0m%s   ", "%s%s%s \x1b[7m   \x1b[0m%s  ", "%s%s%s \x1b[7m \x1b[0m%s   ", "%s%s \x1b[7m   \x1b[0m%s \x1b[7m \x1b[0m%s ", "%s%s \x1b[7m \x1b[0m%s  \x1b[7m  \x1b[0m%s ", "%s%s%s \x1b[7m    \x1b[0m%s    ", "%s%s%s \x1b[7m   \x1b[0m%s  ", "%s%s%s \x1b[7m   \x1b[0m%s  ", "%s%s%s    \x1b[7m \x1b[0m%s     ", "%s%s    \x1b[7m \x1b[0m%s   \x1b[7m \x1b[0m%s     ", "%s%s\x1b[7m   \x1b[0m%s   \x1b[7m   \x1b[0m%s ", "%s%s%s  \x1b[7m   \x1b[0m%s   ", "%s%s%s\x1b[7m      \x1b[0m%s  ", "%s%s%s\x1b[7m    \x1b[0m%s ", "%s%s%s    \x1b[7m  \x1b[0m%s ", "%s%s%s\x1b[7m    \x1b[0m%s ", "%s%s%s%s      ", "%s%s%s\x1b[7m      \x1b[0m%s ", "%s%s%s%s    ", "%s%s%s\x1b[7m  \x1b[0m%s ", "%s%s%s\x1b[7m  \x1b[0m%s ", "%s%s%s\x1b[7m  \x1b[0m%s ", "%s%s%s%s        ", "%s%s%s\x1b[7m      \x1b[0m%s "};
  const char** WHITESPACE_ART_MATRIX[TOTAL_WHITESPACE_ART_ROWS]   = {
    WHITESPACE_ART_ROW_CEIL, WHITESPACE_ART_ROW_TOP, WHITESPACE_ART_ROW_MID, WHITESPACE_ART_ROW_BOT, WHITESPACE_ART_ROW_FLOOR
  };


  // converts all string lowercase to uppercase (unless in an ANSI Esc Sequence)
  void mk_string_uppercase(std::string& s)noexcept{
    for(std::size_t i = 0, n = s.size(); i < n; ++i) {
      if(s[i] == '\x1b')
        while(i < n && !isalpha(s[i])) ++i;
      else
        s[i] = toupper(s[i]);
    }
  }


  // skips over ANSI Escape sequences (formats terminal output, each start w/ '\x1b')
  void skip_over_ANSI_Esc_sequence(std::string& non_art_buffer, std::size_t& idx, 
                                   std::size_t ANSI_ESC_IDXS[], std::size_t& TOTAL_ANSI_ESC_SEQUENCES)noexcept{
    ANSI_ESC_IDXS[TOTAL_ANSI_ESC_SEQUENCES++] = idx; // save ANSI esc idx in string
    for(std::size_t n = non_art_buffer.size(); idx < n && !isalpha(non_art_buffer[idx]); ++idx);
  }


  // returns hashed idx of Alpha-Art letter position in rows of ASCII_ART_MATRIX/WHITESPACE_ART_MATRIX
  std::size_t ascii_art_idx_hash_function(const char c)noexcept{
    // idx of ASCII-art-unknown char repn & total prefixing cntrl chars '\t\n' 
    static constexpr const std::size_t UNKNOWN_ART_IDX = ALPHA_ART_ALPHABET_LENGTH - 3;
    static constexpr const std::size_t TOTAL_CNTRL_CHARS = 2; // '\t', '\n'
    // check if char has an equivalent repn in the ASCII art alphabet
    if(!strrchr(VALID_ALPHA_ART_CHARS, c)) return UNKNOWN_ART_IDX;
    // return valid ASCII-Art char's hashed idx 
    if(c == '\b') return BACKSPACE_ART_IDX;
    return (c == '\t' || c == '\n') ? c - '\t' : TOTAL_CNTRL_CHARS + c - ' ' - ((c >= 'a') * 26);
  }


  // fills "idxs_container" with the hashed idx of each char in "non_art_buffer"
  void get_hashed_art_idxs(std::vector<std::size_t>& idxs_container, std::string& non_art_buffer, 
                           std::size_t ANSI_ESC_IDXS[], std::size_t& TOTAL_ANSI_ESC_SEQUENCES)noexcept{
    for(std::size_t idx = 0, n = non_art_buffer.size(); idx < n; ++idx) {
      if(non_art_buffer[idx] == '\x1b') { // skip ANSI esc sequences
        skip_over_ANSI_Esc_sequence(non_art_buffer, idx, ANSI_ESC_IDXS, TOTAL_ANSI_ESC_SEQUENCES);
        idxs_container.push_back(ANSI_ESC_ART_IDX); // store idx to read the '\x1b' instance from "non_art_buffer" later on
      } else {
        idxs_container.push_back(ascii_art_idx_hash_function(non_art_buffer[idx]));
      }
    }
  }


  // returns whether 'p' points to the particular substring 'substr'
  bool at_ANSI_clear_substring(std::string::const_iterator p, std::string::const_iterator end)noexcept{
    const char* substr = "\x1b[0m";
    while(*substr != '\0' && p != end && *p == *substr) ++p, ++substr;
    return (*substr == '\0');
  }


  // fills "art_buffer" with "non_art_buffer"'s chars in ASCII/Whitespace Art
  std::string convert_non_art_string_to_alpha_art(std::string& non_art_buffer, const std::size_t TOTAL_ROWS, const char*** ALPHA_ART_MATRIX)noexcept{
    std::string art_buffer;
    static constexpr const std::size_t NEWLINE_ART_IDX = 1; // idx (hash value) of '\n' ASCII art repn
    bool is_ascii_art = (TOTAL_ROWS == TOTAL_ASCII_ART_ROWS);

    // vector of hashed letter idxs for ASCII/Whitespace-art variant's row positions
    std::vector<std::size_t> alpha_art_letter_idxs;

    // convert lowercase letters to uppercase, non_art_buffer holds uppercased/formatted 
    // "non_art_string" & "art_buffer" holds ascii/whitespace art
    mk_string_uppercase(non_art_buffer);

    // Idxs of ANSI Esc key instances (ie from using the colors/text-deco above) in the client's non-art string
    std::size_t ANSI_ESC_IDXS[ALPHA_ART_ALPHABET_LENGTH], TOTAL_ANSI_ESC_SEQUENCES = 0;
    for(std::size_t i = 0; i < ALPHA_ART_ALPHABET_LENGTH; ++i) ANSI_ESC_IDXS[i] = 0;

    // get hashed idxs of letter ASCII/Whitespace-art variant positions in the "ALPHA_ART_MATRIX"
    get_hashed_art_idxs(alpha_art_letter_idxs, non_art_buffer, ANSI_ESC_IDXS, TOTAL_ANSI_ESC_SEQUENCES);

    // get each row for the alpha art letters, sprintf'ing each until '\0' or '\n' is reached (if '\n' 
    // reached, restart the process of getting rows but ONLY for chars AFTER the '\n' in "non_art_buffer")
    std::size_t prior_char_idx = std::size_t(-1), newline_or_end = 0, PRIOR_NEWLINE_ANSI_IDX = 0, ANSI_IDX = 0, idx = 0;

    for(std::size_t string_start = 0, total_idxs = alpha_art_letter_idxs.size(); string_start < total_idxs; ++string_start) {
      // for each alpha art row
      for(std::size_t row = 0; row < TOTAL_ROWS; ++row) {
        // ANSI buffer to hold user-defined ansi cmds passed for whitespaces (requires special means of splicing them in)
        std::string whitespace_art_ANSI_cmd_buffer;
        ANSI_IDX = PRIOR_NEWLINE_ANSI_IDX;
        // up until '\0' or '\n' is reached
        for(idx = string_start; idx < total_idxs && alpha_art_letter_idxs[idx] != NEWLINE_ART_IDX; ++idx) {
          
          // sprintf ANSI esc sequences directly from the non-art string
          if(alpha_art_letter_idxs[idx] == ANSI_ESC_ART_IDX) {
            auto ansi_ptr = non_art_buffer.begin() + ANSI_ESC_IDXS[ANSI_IDX++], end = non_art_buffer.end();
            // sprintf the ANSI Esc sequence (text deco/coloring) to the string
            // => ENABLES USING ANSI ESC COLORS/TEXT-DECO W/ ALPHA ART!
            // save ANSI commands passed by user to sprintf for each Whitespace art letter
            if(at_ANSI_clear_substring(ansi_ptr, end))
              whitespace_art_ANSI_cmd_buffer.clear();

            // copy ANSI cmds from user into the "whitespace_art_ANSI_cmd_buffer" & the "art_buffer" strings
            while(ansi_ptr != end && !isalpha(*ansi_ptr)) art_buffer += *ansi_ptr, whitespace_art_ANSI_cmd_buffer += *ansi_ptr++;
            art_buffer += *ansi_ptr, whitespace_art_ANSI_cmd_buffer += *ansi_ptr++; // sprintf last char in ANSI Esc Sequence

          // if at a backspace, move back the width of the preceding character
          } else if(alpha_art_letter_idxs[idx] == BACKSPACE_ART_IDX) {
            if(prior_char_idx != std::size_t(-1)) {
              std::size_t prior_char_length = strlen(ALPHA_ART_MATRIX[row][alpha_art_letter_idxs[prior_char_idx]]);
              for(std::size_t i = 0; i < prior_char_length; ++i) art_buffer += "\b";
              --prior_char_idx;
            }
          } else { // non-backspace & non-ANSI cmd
            prior_char_idx = idx;
            if(is_ascii_art) { // don't sprintf ANSI codes into letter for ASCII
              art_buffer += ALPHA_ART_MATRIX[row][alpha_art_letter_idxs[idx]];
            } else {
              const std::size_t n = strlen(ALPHA_ART_MATRIX[row][alpha_art_letter_idxs[idx]])+4*whitespace_art_ANSI_cmd_buffer.size();
              char* str = new char [n+1];
              snprintf(str, n, ALPHA_ART_MATRIX[row][alpha_art_letter_idxs[idx]], 
                whitespace_art_ANSI_cmd_buffer.c_str(), whitespace_art_ANSI_cmd_buffer.c_str(), 
                whitespace_art_ANSI_cmd_buffer.c_str(), whitespace_art_ANSI_cmd_buffer.c_str());
              art_buffer += str;
              delete[] str;
            }
          }
        }
        art_buffer += HEIST_AFMT(AFMT_0);
        if(row == TOTAL_ROWS - 1) {
          PRIOR_NEWLINE_ANSI_IDX = ANSI_IDX; // store the last ANSI cmd index to invoke after a '\n'
          // add '\n' as needed
          if(idx < total_idxs) {
            art_buffer += '\n';
            if(ALPHA_ART_MATRIX == WHITESPACE_ART_MATRIX)
              art_buffer += '\n';
          }
        } else {
          art_buffer += '\n';
        }
        newline_or_end = idx; // IMPLICIT "RESET" BEFORE '\n' WRT TO ASCII/WHITESPACE ART PRINTING!
      }
      string_start = newline_or_end;
    }
    art_buffer += HEIST_AFMT(AFMT_0);
    return art_buffer;
  }

  /******************************************************************************
  * PRIVATE INTERFACES: ASCII/WHITESPACE ART PRINTING/STRLEN MAIN FUNCTIONS
  ******************************************************************************/

  std::string convert_to_ascii_art(std::string non_art_string)noexcept{
    return convert_non_art_string_to_alpha_art(non_art_string, TOTAL_ASCII_ART_ROWS, ASCII_ART_MATRIX);
  }

  std::string convert_to_whitespace_art(std::string non_art_string)noexcept{
    return convert_non_art_string_to_alpha_art(non_art_string, TOTAL_WHITESPACE_ART_ROWS, WHITESPACE_ART_MATRIX);
  }
} // End of namespace heist_fmt
#endif