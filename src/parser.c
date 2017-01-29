/*
** 2000-05-29
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** Driver template for the LEMON parser generator.
**
** The "lemon" program processes an LALR(1) input grammar file, then uses
** this template to construct a parser.  The "lemon" program inserts text
** at each "%%" line.  Also, any "P-a-r-s-e" identifer prefix (without the
** interstitial "-" characters) contained in this template is changed into
** the value of the %name directive from the grammar.  Otherwise, the content
** of this template is copied straight through into the generate parser
** source file.
**
** The following is the concatenation of all %include directives from the
** input grammar file:
*/
#include <stdio.h>
/************ Begin %include sections from the grammar ************************/

	#include <assert.h>
	#include <stdio.h>
	#include <stdlib.h>

	#include "libMultiMarkdown.h"
	#include "mmd.h"
	#include "parser.h"
	#include "token.h"
/**************** End of %include directives **********************************/
/* These constants specify the various numeric values for terminal symbols
** in a format understandable to "makeheaders".  This section is blank unless
** "lemon" is run with the "-m" command-line option.
***************** Begin makeheaders token definitions *************************/
/**************** End makeheaders token definitions ***************************/

/* The next sections is a series of control #defines.
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used to store the integer codes
**                       that represent terminal and non-terminal symbols.
**                       "unsigned char" is used if there are fewer than
**                       256 symbols.  Larger types otherwise.
**    YYNOCODE           is a number of type YYCODETYPE that is not used for
**                       any terminal or nonterminal symbol.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       (also known as: "terminal symbols") have fall-back
**                       values which should be used if the original symbol
**                       would not parse.  This permits keywords to sometimes
**                       be used as identifiers, for example.
**    YYACTIONTYPE       is the data type used for "action codes" - numbers
**                       that indicate what to do in response to the next
**                       token.
**    ParseTOKENTYPE     is the data type used for minor type for terminal
**                       symbols.  Background: A "minor type" is a semantic
**                       value associated with a terminal or non-terminal
**                       symbols.  For example, for an "ID" terminal symbol,
**                       the minor type might be the name of the identifier.
**                       Each non-terminal can have a different minor type.
**                       Terminal symbols all have the same minor type, though.
**                       This macros defines the minor type for terminal 
**                       symbols.
**    YYMINORTYPE        is the data type used for all minor types.
**                       This is typically a union of many types, one of
**                       which is ParseTOKENTYPE.  The entry in the union
**                       for terminal symbols is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    ParseARG_SDECL     A static variable declaration for the %extra_argument
**    ParseARG_PDECL     A parameter declaration for the %extra_argument
**    ParseARG_STORE     Code to store %extra_argument into yypParser
**    ParseARG_FETCH     Code to extract %extra_argument from yypParser
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YY_MAX_SHIFT       Maximum value for shift actions
**    YY_MIN_SHIFTREDUCE Minimum value for shift-reduce actions
**    YY_MAX_SHIFTREDUCE Maximum value for shift-reduce actions
**    YY_MIN_REDUCE      Maximum value for reduce actions
**    YY_ERROR_ACTION    The yy_action[] code for syntax error
**    YY_ACCEPT_ACTION   The yy_action[] code for accept
**    YY_NO_ACTION       The yy_action[] code for no-op
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/************* Begin control #defines *****************************************/
#define YYCODETYPE unsigned char
#define YYNOCODE 59
#define YYACTIONTYPE unsigned short int
#define ParseTOKENTYPE  token * 
typedef union {
  int yyinit;
  ParseTOKENTYPE yy0;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define ParseARG_SDECL  mmd_engine * engine ;
#define ParseARG_PDECL , mmd_engine * engine 
#define ParseARG_FETCH  mmd_engine * engine  = yypParser->engine 
#define ParseARG_STORE yypParser->engine  = engine 
#define YYFALLBACK 1
#define YYNSTATE             40
#define YYNRULE              93
#define YY_MAX_SHIFT         39
#define YY_MIN_SHIFTREDUCE   96
#define YY_MAX_SHIFTREDUCE   188
#define YY_MIN_REDUCE        189
#define YY_MAX_REDUCE        281
#define YY_ERROR_ACTION      282
#define YY_ACCEPT_ACTION     283
#define YY_NO_ACTION         284
/************* End control #defines *******************************************/

/* Define the yytestcase() macro to be a no-op if is not already defined
** otherwise.
**
** Applications can choose to define yytestcase() in the %include section
** to a macro that can assist in verifying code coverage.  For production
** code the yytestcase() macro should be turned off.  But it is useful
** for testing.
*/
#ifndef yytestcase
# define yytestcase(X)
#endif


/* Next are the tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.  
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N <= YY_MAX_SHIFT             Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   N between YY_MIN_SHIFTREDUCE       Shift to an arbitrary state then
**     and YY_MAX_SHIFTREDUCE           reduce by rule N-YY_MIN_SHIFTREDUCE.
**
**   N between YY_MIN_REDUCE            Reduce by rule N-YY_MIN_REDUCE
**     and YY_MAX_REDUCE
**
**   N == YY_ERROR_ACTION               A syntax error has occurred.
**
**   N == YY_ACCEPT_ACTION              The parser accepts its input.
**
**   N == YY_NO_ACTION                  No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as either:
**
**    (A)   N = yy_action[ yy_shift_ofst[S] + X ]
**    (B)   N = yy_default[S]
**
** The (A) formula is preferred.  The B formula is used instead if:
**    (1)  The yy_shift_ofst[S]+X value is out of range, or
**    (2)  yy_lookahead[yy_shift_ofst[S]+X] is not equal to X, or
**    (3)  yy_shift_ofst[S] equal YY_SHIFT_USE_DFLT.
** (Implementation note: YY_SHIFT_USE_DFLT is chosen so that
** YY_SHIFT_USE_DFLT+X will be out of range for all possible lookaheads X.
** Hence only tests (1) and (2) need to be evaluated.)
**
** The formulas above are for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the yy_reduce_ofst[] array is used in place of
** the yy_shift_ofst[] array and YY_REDUCE_USE_DFLT is used in place of
** YY_SHIFT_USE_DFLT.
**
** The following are the tables generated in this section:
**
**  yy_action[]        A single table containing all actions.
**  yy_lookahead[]     A table containing the lookahead for each entry in
**                     yy_action.  Used to detect hash collisions.
**  yy_shift_ofst[]    For each state, the offset into yy_action for
**                     shifting terminals.
**  yy_reduce_ofst[]   For each state, the offset into yy_action for
**                     shifting non-terminals after a reduce.
**  yy_default[]       Default action for each state.
**
*********** Begin parsing tables **********************************************/
#define YY_ACTTAB_COUNT (174)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */   189,  165,   21,  159,  160,   14,  101,  102,  103,  104,
 /*    10 */   105,  106,  112,   15,    5,    4,  174,    3,    2,   16,
 /*    20 */    13,   12,  119,  165,   11,   21,  159,  160,   14,  101,
 /*    30 */   102,  103,  104,  105,  106,  112,   15,    5,    4,  174,
 /*    40 */     3,    2,   16,   13,   12,  119,  165,   11,  283,    1,
 /*    50 */    98,   99,   10,   39,   37,   35,  110,   38,  113,  114,
 /*    60 */   115,  116,  117,  118,  122,  122,  188,   30,   30,  158,
 /*    70 */    37,  153,   35,   97,   99,   10,   39,   37,   35,  110,
 /*    80 */    38,  113,  114,  115,  116,  117,  118,   29,   26,  187,
 /*    90 */    29,  163,  164,   37,   29,   35,  180,  158,    6,    6,
 /*   100 */   181,    7,    7,  155,   29,   17,  121,   17,   19,   29,
 /*   110 */    19,  162,   18,   29,   18,    8,    8,  183,   29,  165,
 /*   120 */     9,    9,   22,  183,   22,  121,  125,   24,  148,   24,
 /*   130 */   121,   20,  121,   20,   29,  131,   23,  191,   23,  184,
 /*   140 */   183,  169,  151,   31,   31,  184,  121,   27,   27,  163,
 /*   150 */   164,   29,   25,  168,   25,    5,   28,   28,    4,  149,
 /*   160 */    32,   32,  184,   33,   33,   34,   34,   36,   36,  123,
 /*   170 */   139,  136,  126,  123,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     0,   23,    2,    3,    4,    5,    6,    7,    8,    9,
 /*    10 */    10,   11,   12,   13,   14,   15,   16,   17,   18,   19,
 /*    20 */    20,   21,   22,   23,   24,    2,    3,    4,    5,    6,
 /*    30 */     7,    8,    9,   10,   11,   12,   13,   14,   15,   16,
 /*    40 */    17,   18,   19,   20,   21,   22,   23,   24,   26,   27,
 /*    50 */    28,   29,   30,   31,   32,   33,   34,   35,   36,   37,
 /*    60 */    38,   39,   40,   41,   44,   45,    1,   56,   57,    1,
 /*    70 */    48,   55,   50,   28,   29,   30,   31,   32,   33,   34,
 /*    80 */    35,   36,   37,   38,   39,   40,   41,   31,   45,   24,
 /*    90 */    31,    3,    4,   48,   31,   50,    1,    1,   42,   43,
 /*   100 */     5,   42,   43,   57,   31,   49,   43,   51,   49,   31,
 /*   110 */    51,   23,   49,   31,   51,   42,   43,    1,   31,   23,
 /*   120 */    42,   43,   49,    1,   51,   43,   47,   49,   53,   51,
 /*   130 */    43,   49,   43,   51,   31,   50,   49,   58,   51,   23,
 /*   140 */     1,    1,   20,   52,   53,   23,   43,   54,   55,    3,
 /*   150 */     4,   31,   49,   13,   51,   14,   54,   55,   15,   20,
 /*   160 */    46,   47,   23,   42,   43,   42,   43,   42,   43,   23,
 /*   170 */    16,   51,   48,   23,
};
#define YY_SHIFT_USE_DFLT (174)
#define YY_SHIFT_COUNT    (39)
#define YY_SHIFT_MIN      (-22)
#define YY_SHIFT_MAX      (154)
static const short yy_shift_ofst[] = {
 /*     0 */    23,    0,   96,   96,   96,   96,   96,   96,   96,   96,
 /*    10 */    88,   65,  116,  116,   95,  140,   68,  -22,  -22,  -22,
 /*    20 */   -22,   68,  -22,  -22,  -22,  -22,   68,  122,  139,  146,
 /*    30 */    65,   95,  140,   68,   68,  143,   68,  141,  154,  150,
};
#define YY_REDUCE_USE_DFLT (-1)
#define YY_REDUCE_COUNT (37)
#define YY_REDUCE_MIN   (0)
#define YY_REDUCE_MAX   (125)
static const signed char yy_reduce_ofst[] = {
 /*     0 */    22,   45,   56,   59,   73,   78,   63,   82,   87,  103,
 /*    10 */    20,   11,   93,  102,   91,  114,  121,  120,  120,  120,
 /*    20 */   120,  123,  120,  120,  120,  120,  125,   16,   16,   43,
 /*    30 */    46,   75,   79,   89,   89,   85,   89,  124,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   282,  282,  269,  268,  228,  223,  237,  234,  226,  221,
 /*    10 */   193,  278,  282,  282,  271,  259,  270,  238,  236,  235,
 /*    20 */   233,  249,  227,  225,  222,  220,  231,  245,  243,  266,
 /*    30 */   247,  240,  217,  239,  213,  202,  230,  201,  204,  200,
};
/********** End of lemon-generated parsing tables *****************************/

/* The next table maps tokens (terminal symbols) into fallback tokens.  
** If a construct like the following:
** 
**      %fallback ID X Y Z.
**
** appears in the grammar, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
**
** This feature can be used, for example, to cause some keywords in a language
** to revert to identifiers if they keyword does not apply in the context where
** it appears.
*/
#ifdef YYFALLBACK
static const YYCODETYPE yyFallback[] = {
    0,  /*          $ => nothing */
    0,  /* LINE_CONTINUATION => nothing */
    1,  /* LINE_PLAIN => LINE_CONTINUATION */
    1,  /* LINE_INDENTED_TAB => LINE_CONTINUATION */
    1,  /* LINE_INDENTED_SPACE => LINE_CONTINUATION */
    0,  /*  LINE_HTML => nothing */
    5,  /* LINE_ATX_1 => LINE_HTML */
    5,  /* LINE_ATX_2 => LINE_HTML */
    5,  /* LINE_ATX_3 => LINE_HTML */
    5,  /* LINE_ATX_4 => LINE_HTML */
    5,  /* LINE_ATX_5 => LINE_HTML */
    5,  /* LINE_ATX_6 => LINE_HTML */
    5,  /*    LINE_HR => LINE_HTML */
    5,  /* LINE_BLOCKQUOTE => LINE_HTML */
    5,  /* LINE_LIST_BULLETED => LINE_HTML */
    5,  /* LINE_LIST_ENUMERATED => LINE_HTML */
    5,  /* LINE_TABLE => LINE_HTML */
    5,  /* LINE_DEF_CITATION => LINE_HTML */
    5,  /* LINE_DEF_FOOTNOTE => LINE_HTML */
    5,  /* LINE_DEF_LINK => LINE_HTML */
    5,  /* LINE_FENCE_BACKTICK => LINE_HTML */
    5,  /* LINE_FENCE_BACKTICK_START => LINE_HTML */
};
#endif /* YYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
**
** After the "shift" half of a SHIFTREDUCE action, the stateno field
** actually contains the reduce action for the second half of the
** SHIFTREDUCE.
*/
struct yyStackEntry {
  YYACTIONTYPE stateno;  /* The state-number, or reduce action in SHIFTREDUCE */
  YYCODETYPE major;      /* The major token value.  This is the code
                         ** number for the token at this stack level */
  YYMINORTYPE minor;     /* The user-supplied minor token value.  This
                         ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  yyStackEntry *yytos;          /* Pointer to top element of the stack */
#ifdef YYTRACKMAXSTACKDEPTH
  int yyhwm;                    /* High-water mark of the stack */
#endif
#ifndef YYNOERRORRECOVERY
  int yyerrcnt;                 /* Shifts left before out of the error */
#endif
  ParseARG_SDECL                /* A place to hold %extra_argument */
#if YYSTACKDEPTH<=0
  int yystksz;                  /* Current side of the stack */
  yyStackEntry *yystack;        /* The parser's stack */
  yyStackEntry yystk0;          /* First stack entry */
#else
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
#endif
};
typedef struct yyParser yyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *yyTraceFILE = 0;
static char *yyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/* 
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL 
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
void ParseTrace(FILE *TraceFILE, char *zTracePrompt){
  yyTraceFILE = TraceFILE;
  yyTracePrompt = zTracePrompt;
  if( yyTraceFILE==0 ) yyTracePrompt = 0;
  else if( yyTracePrompt==0 ) yyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *const yyTokenName[] = { 
  "$",             "LINE_CONTINUATION",  "LINE_PLAIN",    "LINE_INDENTED_TAB",
  "LINE_INDENTED_SPACE",  "LINE_HTML",     "LINE_ATX_1",    "LINE_ATX_2",  
  "LINE_ATX_3",    "LINE_ATX_4",    "LINE_ATX_5",    "LINE_ATX_6",  
  "LINE_HR",       "LINE_BLOCKQUOTE",  "LINE_LIST_BULLETED",  "LINE_LIST_ENUMERATED",
  "LINE_TABLE",    "LINE_DEF_CITATION",  "LINE_DEF_FOOTNOTE",  "LINE_DEF_LINK",
  "LINE_FENCE_BACKTICK",  "LINE_FENCE_BACKTICK_START",  "LINE_TOC",      "LINE_EMPTY",  
  "LINE_META",     "error",         "doc",           "blocks",      
  "block",         "para",          "indented_code",  "empty",       
  "list_bulleted",  "list_enumerated",  "blockquote",    "table",       
  "def_citation",  "def_footnote",  "def_link",      "html_block",  
  "fenced_block",  "meta_block",    "para_lines",    "para_line",   
  "code_line",     "indented_line",  "quote_lines",   "quote_line",  
  "item_bulleted",  "cont_blocks",   "item_enumerated",  "cont_block",  
  "html_block_lines",  "html_block_line",  "fenced_lines",  "fenced_line", 
  "meta_lines",    "meta_line",   
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "doc ::= blocks",
 /*   1 */ "blocks ::= blocks block",
 /*   2 */ "blocks ::= block",
 /*   3 */ "block ::= para",
 /*   4 */ "block ::= indented_code",
 /*   5 */ "block ::= LINE_ATX_1",
 /*   6 */ "block ::= LINE_ATX_2",
 /*   7 */ "block ::= LINE_ATX_3",
 /*   8 */ "block ::= LINE_ATX_4",
 /*   9 */ "block ::= LINE_ATX_5",
 /*  10 */ "block ::= LINE_ATX_6",
 /*  11 */ "block ::= empty",
 /*  12 */ "block ::= list_bulleted",
 /*  13 */ "block ::= list_enumerated",
 /*  14 */ "block ::= blockquote",
 /*  15 */ "block ::= table",
 /*  16 */ "block ::= LINE_HR",
 /*  17 */ "block ::= def_citation",
 /*  18 */ "block ::= def_footnote",
 /*  19 */ "block ::= def_link",
 /*  20 */ "block ::= html_block",
 /*  21 */ "block ::= fenced_block",
 /*  22 */ "block ::= meta_block",
 /*  23 */ "block ::= LINE_TOC",
 /*  24 */ "para ::= LINE_PLAIN para_lines",
 /*  25 */ "para_lines ::= para_lines para_line",
 /*  26 */ "indented_code ::= indented_code code_line",
 /*  27 */ "empty ::= empty LINE_EMPTY",
 /*  28 */ "blockquote ::= LINE_BLOCKQUOTE quote_lines",
 /*  29 */ "quote_lines ::= quote_lines quote_line",
 /*  30 */ "list_bulleted ::= list_bulleted item_bulleted",
 /*  31 */ "item_bulleted ::= LINE_LIST_BULLETED para_lines cont_blocks",
 /*  32 */ "item_bulleted ::= LINE_LIST_BULLETED para_lines",
 /*  33 */ "item_bulleted ::= LINE_LIST_BULLETED cont_blocks",
 /*  34 */ "item_bulleted ::= LINE_LIST_BULLETED",
 /*  35 */ "list_enumerated ::= list_enumerated item_enumerated",
 /*  36 */ "item_enumerated ::= LINE_LIST_ENUMERATED para_lines cont_blocks",
 /*  37 */ "item_enumerated ::= LINE_LIST_ENUMERATED para_lines",
 /*  38 */ "item_enumerated ::= LINE_LIST_ENUMERATED cont_blocks",
 /*  39 */ "item_enumerated ::= LINE_LIST_ENUMERATED",
 /*  40 */ "cont_blocks ::= cont_blocks cont_block",
 /*  41 */ "cont_block ::= empty indented_line para_lines",
 /*  42 */ "cont_block ::= empty indented_line",
 /*  43 */ "table ::= table LINE_TABLE",
 /*  44 */ "def_citation ::= LINE_DEF_CITATION para_lines cont_blocks",
 /*  45 */ "def_citation ::= LINE_DEF_CITATION para_lines",
 /*  46 */ "def_citation ::= LINE_DEF_CITATION cont_blocks",
 /*  47 */ "def_footnote ::= LINE_DEF_FOOTNOTE para_lines cont_blocks",
 /*  48 */ "def_footnote ::= LINE_DEF_FOOTNOTE para_lines",
 /*  49 */ "def_footnote ::= LINE_DEF_FOOTNOTE cont_blocks",
 /*  50 */ "def_link ::= LINE_DEF_LINK para_lines",
 /*  51 */ "html_block ::= LINE_HTML html_block_lines",
 /*  52 */ "html_block_lines ::= html_block_lines html_block_line",
 /*  53 */ "fenced_block ::= LINE_FENCE_BACKTICK fenced_lines LINE_FENCE_BACKTICK",
 /*  54 */ "fenced_block ::= LINE_FENCE_BACKTICK fenced_lines",
 /*  55 */ "fenced_block ::= LINE_FENCE_BACKTICK_START fenced_lines LINE_FENCE_BACKTICK",
 /*  56 */ "fenced_block ::= LINE_FENCE_BACKTICK_START fenced_lines",
 /*  57 */ "fenced_lines ::= fenced_lines fenced_line",
 /*  58 */ "meta_block ::= LINE_META meta_lines",
 /*  59 */ "meta_lines ::= meta_lines meta_line",
 /*  60 */ "para ::= LINE_PLAIN",
 /*  61 */ "para_lines ::= para_line",
 /*  62 */ "para_line ::= LINE_CONTINUATION",
 /*  63 */ "indented_code ::= LINE_INDENTED_TAB",
 /*  64 */ "indented_code ::= LINE_INDENTED_SPACE",
 /*  65 */ "code_line ::= indented_line",
 /*  66 */ "code_line ::= LINE_EMPTY",
 /*  67 */ "indented_line ::= LINE_INDENTED_TAB",
 /*  68 */ "indented_line ::= LINE_INDENTED_SPACE",
 /*  69 */ "empty ::= LINE_EMPTY",
 /*  70 */ "blockquote ::= LINE_BLOCKQUOTE",
 /*  71 */ "quote_lines ::= quote_line",
 /*  72 */ "quote_line ::= LINE_BLOCKQUOTE",
 /*  73 */ "quote_line ::= LINE_CONTINUATION",
 /*  74 */ "list_bulleted ::= item_bulleted",
 /*  75 */ "list_enumerated ::= item_enumerated",
 /*  76 */ "cont_blocks ::= cont_block",
 /*  77 */ "cont_block ::= empty",
 /*  78 */ "table ::= LINE_TABLE",
 /*  79 */ "def_citation ::= LINE_DEF_CITATION",
 /*  80 */ "def_footnote ::= LINE_DEF_FOOTNOTE",
 /*  81 */ "def_link ::= LINE_DEF_LINK",
 /*  82 */ "html_block ::= LINE_HTML",
 /*  83 */ "html_block_lines ::= html_block_line",
 /*  84 */ "html_block_line ::= LINE_CONTINUATION",
 /*  85 */ "html_block_line ::= LINE_HTML",
 /*  86 */ "fenced_lines ::= fenced_line",
 /*  87 */ "fenced_line ::= LINE_CONTINUATION",
 /*  88 */ "fenced_line ::= LINE_EMPTY",
 /*  89 */ "meta_block ::= LINE_META",
 /*  90 */ "meta_lines ::= meta_line",
 /*  91 */ "meta_line ::= LINE_META",
 /*  92 */ "meta_line ::= LINE_CONTINUATION",
};
#endif /* NDEBUG */


#if YYSTACKDEPTH<=0
/*
** Try to increase the size of the parser stack.  Return the number
** of errors.  Return 0 on success.
*/
static int yyGrowStack(yyParser *p){
  int newSize;
  int idx;
  yyStackEntry *pNew;

  newSize = p->yystksz*2 + 100;
  idx = p->yytos ? (int)(p->yytos - p->yystack) : 0;
  if( p->yystack==&p->yystk0 ){
    pNew = malloc(newSize*sizeof(pNew[0]));
    if( pNew ) pNew[0] = p->yystk0;
  }else{
    pNew = realloc(p->yystack, newSize*sizeof(pNew[0]));
  }
  if( pNew ){
    p->yystack = pNew;
    p->yytos = &p->yystack[idx];
#ifndef NDEBUG
    if( yyTraceFILE ){
      fprintf(yyTraceFILE,"%sStack grows from %d to %d entries.\n",
              yyTracePrompt, p->yystksz, newSize);
    }
#endif
    p->yystksz = newSize;
  }
  return pNew==0; 
}
#endif

/* Datatype of the argument to the memory allocated passed as the
** second argument to ParseAlloc() below.  This can be changed by
** putting an appropriate #define in the %include section of the input
** grammar.
*/
#ifndef YYMALLOCARGTYPE
# define YYMALLOCARGTYPE size_t
#endif

/* 
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to Parse and ParseFree.
*/
void *ParseAlloc(void *(*mallocProc)(YYMALLOCARGTYPE)){
  yyParser *pParser;
  pParser = (yyParser*)(*mallocProc)( (YYMALLOCARGTYPE)sizeof(yyParser) );
  if( pParser ){
#ifdef YYTRACKMAXSTACKDEPTH
    pParser->yyhwm = 0;
#endif
#if YYSTACKDEPTH<=0
    pParser->yytos = NULL;
    pParser->yystack = NULL;
    pParser->yystksz = 0;
    if( yyGrowStack(pParser) ){
      pParser->yystack = &pParser->yystk0;
      pParser->yystksz = 1;
    }
#endif
#ifndef YYNOERRORRECOVERY
    pParser->yyerrcnt = -1;
#endif
    pParser->yytos = pParser->yystack;
    pParser->yystack[0].stateno = 0;
    pParser->yystack[0].major = 0;
  }
  return pParser;
}

/* The following function deletes the "minor type" or semantic value
** associated with a symbol.  The symbol can be either a terminal
** or nonterminal. "yymajor" is the symbol code, and "yypminor" is
** a pointer to the value to be deleted.  The code used to do the 
** deletions is derived from the %destructor and/or %token_destructor
** directives of the input grammar.
*/
static void yy_destructor(
  yyParser *yypParser,    /* The parser */
  YYCODETYPE yymajor,     /* Type code for object to destroy */
  YYMINORTYPE *yypminor   /* The object to be destroyed */
){
  ParseARG_FETCH;
  switch( yymajor ){
    /* Here is inserted the actions which take place when a
    ** terminal or non-terminal is destroyed.  This can happen
    ** when the symbol is popped from the stack during a
    ** reduce or during error processing or when a parser is 
    ** being destroyed before it is finished parsing.
    **
    ** Note: during a reduce, the only symbols destroyed are those
    ** which appear on the RHS of the rule, but which are *not* used
    ** inside the C code.
    */
/********* Begin destructor definitions ***************************************/
/********* End destructor definitions *****************************************/
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
*/
static void yy_pop_parser_stack(yyParser *pParser){
  yyStackEntry *yytos;
  assert( pParser->yytos!=0 );
  assert( pParser->yytos > pParser->yystack );
  yytos = pParser->yytos--;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[yytos->major]);
  }
#endif
  yy_destructor(pParser, yytos->major, &yytos->minor);
}

/* 
** Deallocate and destroy a parser.  Destructors are called for
** all stack elements before shutting the parser down.
**
** If the YYPARSEFREENEVERNULL macro exists (for example because it
** is defined in a %include section of the input grammar) then it is
** assumed that the input pointer is never NULL.
*/
void ParseFree(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
  yyParser *pParser = (yyParser*)p;
#ifndef YYPARSEFREENEVERNULL
  if( pParser==0 ) return;
#endif
  while( pParser->yytos>pParser->yystack ) yy_pop_parser_stack(pParser);
#if YYSTACKDEPTH<=0
  if( pParser->yystack!=&pParser->yystk0 ) free(pParser->yystack);
#endif
  (*freeProc)((void*)pParser);
}

/*
** Return the peak depth of the stack for a parser.
*/
#ifdef YYTRACKMAXSTACKDEPTH
int ParseStackPeak(void *p){
  yyParser *pParser = (yyParser*)p;
  return pParser->yyhwm;
}
#endif

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
*/
static unsigned int yy_find_shift_action(
  yyParser *pParser,        /* The parser */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
  int stateno = pParser->yytos->stateno;
 
  if( stateno>=YY_MIN_REDUCE ) return stateno;
  assert( stateno <= YY_SHIFT_COUNT );
  do{
    i = yy_shift_ofst[stateno];
    assert( iLookAhead!=YYNOCODE );
    i += iLookAhead;
    if( i<0 || i>=YY_ACTTAB_COUNT || yy_lookahead[i]!=iLookAhead ){
#ifdef YYFALLBACK
      YYCODETYPE iFallback;            /* Fallback token */
      if( iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0])
             && (iFallback = yyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
             yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
        }
#endif
        assert( yyFallback[iFallback]==0 ); /* Fallback loop must terminate */
        iLookAhead = iFallback;
        continue;
      }
#endif
#ifdef YYWILDCARD
      {
        int j = i - iLookAhead + YYWILDCARD;
        if( 
#if YY_SHIFT_MIN+YYWILDCARD<0
          j>=0 &&
#endif
#if YY_SHIFT_MAX+YYWILDCARD>=YY_ACTTAB_COUNT
          j<YY_ACTTAB_COUNT &&
#endif
          yy_lookahead[j]==YYWILDCARD && iLookAhead>0
        ){
#ifndef NDEBUG
          if( yyTraceFILE ){
            fprintf(yyTraceFILE, "%sWILDCARD %s => %s\n",
               yyTracePrompt, yyTokenName[iLookAhead],
               yyTokenName[YYWILDCARD]);
          }
#endif /* NDEBUG */
          return yy_action[j];
        }
      }
#endif /* YYWILDCARD */
      return yy_default[stateno];
    }else{
      return yy_action[i];
    }
  }while(1);
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
*/
static int yy_find_reduce_action(
  int stateno,              /* Current state number */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
#ifdef YYERRORSYMBOL
  if( stateno>YY_REDUCE_COUNT ){
    return yy_default[stateno];
  }
#else
  assert( stateno<=YY_REDUCE_COUNT );
#endif
  i = yy_reduce_ofst[stateno];
  assert( i!=YY_REDUCE_USE_DFLT );
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
#ifdef YYERRORSYMBOL
  if( i<0 || i>=YY_ACTTAB_COUNT || yy_lookahead[i]!=iLookAhead ){
    return yy_default[stateno];
  }
#else
  assert( i>=0 && i<YY_ACTTAB_COUNT );
  assert( yy_lookahead[i]==iLookAhead );
#endif
  return yy_action[i];
}

/*
** The following routine is called if the stack overflows.
*/
static void yyStackOverflow(yyParser *yypParser){
   ParseARG_FETCH;
   yypParser->yytos--;
#ifndef NDEBUG
   if( yyTraceFILE ){
     fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
   }
#endif
   while( yypParser->yytos>yypParser->yystack ) yy_pop_parser_stack(yypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
/******** Begin %stack_overflow code ******************************************/
/******** End %stack_overflow code ********************************************/
   ParseARG_STORE; /* Suppress warning about unused %extra_argument var */
}

/*
** Print tracing information for a SHIFT action
*/
#ifndef NDEBUG
static void yyTraceShift(yyParser *yypParser, int yyNewState){
  if( yyTraceFILE ){
    if( yyNewState<YYNSTATE ){
      fprintf(yyTraceFILE,"%sShift '%s', go to state %d\n",
         yyTracePrompt,yyTokenName[yypParser->yytos->major],
         yyNewState);
    }else{
      fprintf(yyTraceFILE,"%sShift '%s'\n",
         yyTracePrompt,yyTokenName[yypParser->yytos->major]);
    }
  }
}
#else
# define yyTraceShift(X,Y)
#endif

/*
** Perform a shift action.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  int yyNewState,               /* The new state to shift in */
  int yyMajor,                  /* The major token to shift in */
  ParseTOKENTYPE yyMinor        /* The minor token to shift in */
){
  yyStackEntry *yytos;
  yypParser->yytos++;
#ifdef YYTRACKMAXSTACKDEPTH
  if( (int)(yypParser->yytos - yypParser->yystack)>yypParser->yyhwm ){
    yypParser->yyhwm++;
    assert( yypParser->yyhwm == (int)(yypParser->yytos - yypParser->yystack) );
  }
#endif
#if YYSTACKDEPTH>0 
  if( yypParser->yytos>=&yypParser->yystack[YYSTACKDEPTH] ){
    yyStackOverflow(yypParser);
    return;
  }
#else
  if( yypParser->yytos>=&yypParser->yystack[yypParser->yystksz] ){
    if( yyGrowStack(yypParser) ){
      yyStackOverflow(yypParser);
      return;
    }
  }
#endif
  if( yyNewState > YY_MAX_SHIFT ){
    yyNewState += YY_MIN_REDUCE - YY_MIN_SHIFTREDUCE;
  }
  yytos = yypParser->yytos;
  yytos->stateno = (YYACTIONTYPE)yyNewState;
  yytos->major = (YYCODETYPE)yyMajor;
  yytos->minor.yy0 = yyMinor;
  yyTraceShift(yypParser, yyNewState);
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static const struct {
  YYCODETYPE lhs;         /* Symbol on the left-hand side of the rule */
  unsigned char nrhs;     /* Number of right-hand side symbols in the rule */
} yyRuleInfo[] = {
  { 26, 1 },
  { 27, 2 },
  { 27, 1 },
  { 28, 1 },
  { 28, 1 },
  { 28, 1 },
  { 28, 1 },
  { 28, 1 },
  { 28, 1 },
  { 28, 1 },
  { 28, 1 },
  { 28, 1 },
  { 28, 1 },
  { 28, 1 },
  { 28, 1 },
  { 28, 1 },
  { 28, 1 },
  { 28, 1 },
  { 28, 1 },
  { 28, 1 },
  { 28, 1 },
  { 28, 1 },
  { 28, 1 },
  { 28, 1 },
  { 29, 2 },
  { 42, 2 },
  { 30, 2 },
  { 31, 2 },
  { 34, 2 },
  { 46, 2 },
  { 32, 2 },
  { 48, 3 },
  { 48, 2 },
  { 48, 2 },
  { 48, 1 },
  { 33, 2 },
  { 50, 3 },
  { 50, 2 },
  { 50, 2 },
  { 50, 1 },
  { 49, 2 },
  { 51, 3 },
  { 51, 2 },
  { 35, 2 },
  { 36, 3 },
  { 36, 2 },
  { 36, 2 },
  { 37, 3 },
  { 37, 2 },
  { 37, 2 },
  { 38, 2 },
  { 39, 2 },
  { 52, 2 },
  { 40, 3 },
  { 40, 2 },
  { 40, 3 },
  { 40, 2 },
  { 54, 2 },
  { 41, 2 },
  { 56, 2 },
  { 29, 1 },
  { 42, 1 },
  { 43, 1 },
  { 30, 1 },
  { 30, 1 },
  { 44, 1 },
  { 44, 1 },
  { 45, 1 },
  { 45, 1 },
  { 31, 1 },
  { 34, 1 },
  { 46, 1 },
  { 47, 1 },
  { 47, 1 },
  { 32, 1 },
  { 33, 1 },
  { 49, 1 },
  { 51, 1 },
  { 35, 1 },
  { 36, 1 },
  { 37, 1 },
  { 38, 1 },
  { 39, 1 },
  { 52, 1 },
  { 53, 1 },
  { 53, 1 },
  { 54, 1 },
  { 55, 1 },
  { 55, 1 },
  { 41, 1 },
  { 56, 1 },
  { 57, 1 },
  { 57, 1 },
};

static void yy_accept(yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(
  yyParser *yypParser,         /* The parser */
  unsigned int yyruleno        /* Number of the rule by which to reduce */
){
  int yygoto;                     /* The next state */
  int yyact;                      /* The next action */
  yyStackEntry *yymsp;            /* The top of the parser's stack */
  int yysize;                     /* Amount to pop the stack */
  ParseARG_FETCH;
  yymsp = yypParser->yytos;
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno<(int)(sizeof(yyRuleName)/sizeof(yyRuleName[0])) ){
    yysize = yyRuleInfo[yyruleno].nrhs;
    fprintf(yyTraceFILE, "%sReduce [%s], go to state %d.\n", yyTracePrompt,
      yyRuleName[yyruleno], yymsp[-yysize].stateno);
  }
#endif /* NDEBUG */

  /* Check that the stack is large enough to grow by a single entry
  ** if the RHS of the rule is empty.  This ensures that there is room
  ** enough on the stack to push the LHS value */
  if( yyRuleInfo[yyruleno].nrhs==0 ){
#ifdef YYTRACKMAXSTACKDEPTH
    if( (int)(yypParser->yytos - yypParser->yystack)>yypParser->yyhwm ){
      yypParser->yyhwm++;
      assert( yypParser->yyhwm == (int)(yypParser->yytos - yypParser->yystack));
    }
#endif
#if YYSTACKDEPTH>0 
    if( yypParser->yytos>=&yypParser->yystack[YYSTACKDEPTH-1] ){
      yyStackOverflow(yypParser);
      return;
    }
#else
    if( yypParser->yytos>=&yypParser->yystack[yypParser->yystksz-1] ){
      if( yyGrowStack(yypParser) ){
        yyStackOverflow(yypParser);
        return;
      }
      yymsp = yypParser->yytos;
    }
#endif
  }

  switch( yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
/********** Begin reduce actions **********************************************/
        YYMINORTYPE yylhsminor;
      case 0: /* doc ::= blocks */
{ engine->root = yymsp[0].minor.yy0; }
        break;
      case 1: /* blocks ::= blocks block */
{
		strip_line_tokens_from_block(engine, yymsp[0].minor.yy0);
		if (yymsp[-1].minor.yy0 == NULL) { yymsp[-1].minor.yy0 = yymsp[0].minor.yy0; yymsp[0].minor.yy0 = NULL;}
		yylhsminor.yy0 = yymsp[-1].minor.yy0;
		token_chain_append(yylhsminor.yy0, yymsp[0].minor.yy0);
		#ifndef NDEBUG
		fprintf(stderr, "Next block %d\n", yylhsminor.yy0->tail->type);
		#endif
	}
  yymsp[-1].minor.yy0 = yylhsminor.yy0;
        break;
      case 2: /* blocks ::= block */
{
		strip_line_tokens_from_block(engine, yymsp[0].minor.yy0);
		#ifndef NDEBUG
		fprintf(stderr, "First block %d\n", yymsp[0].minor.yy0->type);
		#endif
		yylhsminor.yy0 = yymsp[0].minor.yy0;
	}
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 3: /* block ::= para */
{ yylhsminor.yy0 = token_new_parent(yymsp[0].minor.yy0, BLOCK_PARA); is_para_html(engine, yylhsminor.yy0); }
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 4: /* block ::= indented_code */
{ yylhsminor.yy0 = token_new_parent(yymsp[0].minor.yy0, BLOCK_CODE_INDENTED); }
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 5: /* block ::= LINE_ATX_1 */
{ yylhsminor.yy0 = token_new_parent(yymsp[0].minor.yy0, BLOCK_H1); if (!(engine->extensions & EXT_NO_LABELS)) stack_push(engine->header_stack, yylhsminor.yy0); }
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 6: /* block ::= LINE_ATX_2 */
{ yylhsminor.yy0 = token_new_parent(yymsp[0].minor.yy0, BLOCK_H2); if (!(engine->extensions & EXT_NO_LABELS)) stack_push(engine->header_stack, yylhsminor.yy0); }
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 7: /* block ::= LINE_ATX_3 */
{ yylhsminor.yy0 = token_new_parent(yymsp[0].minor.yy0, BLOCK_H3); if (!(engine->extensions & EXT_NO_LABELS)) stack_push(engine->header_stack, yylhsminor.yy0); }
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 8: /* block ::= LINE_ATX_4 */
{ yylhsminor.yy0 = token_new_parent(yymsp[0].minor.yy0, BLOCK_H4); if (!(engine->extensions & EXT_NO_LABELS)) stack_push(engine->header_stack, yylhsminor.yy0); }
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 9: /* block ::= LINE_ATX_5 */
{ yylhsminor.yy0 = token_new_parent(yymsp[0].minor.yy0, BLOCK_H5); if (!(engine->extensions & EXT_NO_LABELS)) stack_push(engine->header_stack, yylhsminor.yy0); }
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 10: /* block ::= LINE_ATX_6 */
{ yylhsminor.yy0 = token_new_parent(yymsp[0].minor.yy0, BLOCK_H6); if (!(engine->extensions & EXT_NO_LABELS)) stack_push(engine->header_stack, yylhsminor.yy0); }
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 11: /* block ::= empty */
{ yylhsminor.yy0 = token_new_parent(yymsp[0].minor.yy0, BLOCK_EMPTY); }
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 12: /* block ::= list_bulleted */
{ yylhsminor.yy0 = token_new_parent(yymsp[0].minor.yy0, BLOCK_LIST_BULLETED); is_list_loose(yylhsminor.yy0); }
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 13: /* block ::= list_enumerated */
{ yylhsminor.yy0 = token_new_parent(yymsp[0].minor.yy0, BLOCK_LIST_ENUMERATED); is_list_loose(yylhsminor.yy0); }
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 14: /* block ::= blockquote */
{ yylhsminor.yy0 = token_new_parent(yymsp[0].minor.yy0, BLOCK_BLOCKQUOTE); recursive_parse_blockquote(engine, yylhsminor.yy0); }
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 15: /* block ::= table */
{ yylhsminor.yy0 = token_new_parent(yymsp[0].minor.yy0, BLOCK_TABLE); }
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 16: /* block ::= LINE_HR */
{ yylhsminor.yy0 = token_new_parent(yymsp[0].minor.yy0, BLOCK_HR); }
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 17: /* block ::= def_citation */
{ yylhsminor.yy0 = token_new_parent(yymsp[0].minor.yy0, BLOCK_DEF_CITATION); stack_push(engine->definition_stack, yylhsminor.yy0); }
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 18: /* block ::= def_footnote */
{ yylhsminor.yy0 = token_new_parent(yymsp[0].minor.yy0, BLOCK_DEF_FOOTNOTE); stack_push(engine->definition_stack, yylhsminor.yy0); }
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 19: /* block ::= def_link */
{ yylhsminor.yy0 = token_new_parent(yymsp[0].minor.yy0, BLOCK_DEF_LINK); stack_push(engine->definition_stack, yylhsminor.yy0); }
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 20: /* block ::= html_block */
{ yylhsminor.yy0 = token_new_parent(yymsp[0].minor.yy0, BLOCK_HTML); }
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 21: /* block ::= fenced_block */
{ yylhsminor.yy0 = token_new_parent(yymsp[0].minor.yy0, BLOCK_CODE_FENCED); yymsp[0].minor.yy0->child->type = CODE_FENCE; }
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 22: /* block ::= meta_block */
{ yylhsminor.yy0 = token_new_parent(yymsp[0].minor.yy0, BLOCK_META); }
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 23: /* block ::= LINE_TOC */
{ yylhsminor.yy0 = token_new_parent(yymsp[0].minor.yy0, BLOCK_TOC); }
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 24: /* para ::= LINE_PLAIN para_lines */
      case 25: /* para_lines ::= para_lines para_line */ yytestcase(yyruleno==25);
      case 26: /* indented_code ::= indented_code code_line */ yytestcase(yyruleno==26);
      case 27: /* empty ::= empty LINE_EMPTY */ yytestcase(yyruleno==27);
      case 28: /* blockquote ::= LINE_BLOCKQUOTE quote_lines */ yytestcase(yyruleno==28);
      case 29: /* quote_lines ::= quote_lines quote_line */ yytestcase(yyruleno==29);
      case 30: /* list_bulleted ::= list_bulleted item_bulleted */ yytestcase(yyruleno==30);
      case 35: /* list_enumerated ::= list_enumerated item_enumerated */ yytestcase(yyruleno==35);
      case 40: /* cont_blocks ::= cont_blocks cont_block */ yytestcase(yyruleno==40);
      case 43: /* table ::= table LINE_TABLE */ yytestcase(yyruleno==43);
      case 45: /* def_citation ::= LINE_DEF_CITATION para_lines */ yytestcase(yyruleno==45);
      case 46: /* def_citation ::= LINE_DEF_CITATION cont_blocks */ yytestcase(yyruleno==46);
      case 48: /* def_footnote ::= LINE_DEF_FOOTNOTE para_lines */ yytestcase(yyruleno==48);
      case 49: /* def_footnote ::= LINE_DEF_FOOTNOTE cont_blocks */ yytestcase(yyruleno==49);
      case 50: /* def_link ::= LINE_DEF_LINK para_lines */ yytestcase(yyruleno==50);
      case 51: /* html_block ::= LINE_HTML html_block_lines */ yytestcase(yyruleno==51);
      case 52: /* html_block_lines ::= html_block_lines html_block_line */ yytestcase(yyruleno==52);
      case 54: /* fenced_block ::= LINE_FENCE_BACKTICK fenced_lines */ yytestcase(yyruleno==54);
      case 56: /* fenced_block ::= LINE_FENCE_BACKTICK_START fenced_lines */ yytestcase(yyruleno==56);
      case 57: /* fenced_lines ::= fenced_lines fenced_line */ yytestcase(yyruleno==57);
      case 58: /* meta_block ::= LINE_META meta_lines */ yytestcase(yyruleno==58);
      case 59: /* meta_lines ::= meta_lines meta_line */ yytestcase(yyruleno==59);
{ yylhsminor.yy0 = yymsp[-1].minor.yy0; token_chain_append(yymsp[-1].minor.yy0, yymsp[0].minor.yy0); }
  yymsp[-1].minor.yy0 = yylhsminor.yy0;
        break;
      case 31: /* item_bulleted ::= LINE_LIST_BULLETED para_lines cont_blocks */
      case 36: /* item_enumerated ::= LINE_LIST_ENUMERATED para_lines cont_blocks */ yytestcase(yyruleno==36);
{ token_chain_append(yymsp[-2].minor.yy0, yymsp[-1].minor.yy0); token_chain_append(yymsp[-2].minor.yy0, yymsp[0].minor.yy0); yylhsminor.yy0 = token_new_parent(yymsp[-2].minor.yy0, BLOCK_LIST_ITEM); recursive_parse_list_item(engine, yylhsminor.yy0); }
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 32: /* item_bulleted ::= LINE_LIST_BULLETED para_lines */
      case 37: /* item_enumerated ::= LINE_LIST_ENUMERATED para_lines */ yytestcase(yyruleno==37);
{ token_chain_append(yymsp[-1].minor.yy0, yymsp[0].minor.yy0); yylhsminor.yy0 = token_new_parent(yymsp[-1].minor.yy0, BLOCK_LIST_ITEM_TIGHT); recursive_parse_list_item(engine, yylhsminor.yy0); }
  yymsp[-1].minor.yy0 = yylhsminor.yy0;
        break;
      case 33: /* item_bulleted ::= LINE_LIST_BULLETED cont_blocks */
{ token_chain_append(yymsp[-1].minor.yy0, yymsp[0].minor.yy0); yylhsminor.yy0 = token_new_parent(yymsp[-1].minor.yy0, BLOCK_LIST_ITEM); if (yymsp[0].minor.yy0) {recursive_parse_list_item(engine, yylhsminor.yy0);} }
  yymsp[-1].minor.yy0 = yylhsminor.yy0;
        break;
      case 34: /* item_bulleted ::= LINE_LIST_BULLETED */
      case 39: /* item_enumerated ::= LINE_LIST_ENUMERATED */ yytestcase(yyruleno==39);
{ yylhsminor.yy0 = token_new_parent(yymsp[0].minor.yy0, BLOCK_LIST_ITEM_TIGHT); }
  yymsp[0].minor.yy0 = yylhsminor.yy0;
        break;
      case 38: /* item_enumerated ::= LINE_LIST_ENUMERATED cont_blocks */
{ token_chain_append(yymsp[-1].minor.yy0, yymsp[0].minor.yy0); yylhsminor.yy0 = token_new_parent(yymsp[-1].minor.yy0, BLOCK_LIST_ITEM); recursive_parse_list_item(engine, yylhsminor.yy0); }
  yymsp[-1].minor.yy0 = yylhsminor.yy0;
        break;
      case 41: /* cont_block ::= empty indented_line para_lines */
{ yylhsminor.yy0 = yymsp[-2].minor.yy0; token_chain_append(yymsp[-2].minor.yy0, yymsp[-1].minor.yy0); token_chain_append(yymsp[-2].minor.yy0, yymsp[0].minor.yy0); yymsp[-1].minor.yy0->type = LINE_CONTINUATION; }
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 42: /* cont_block ::= empty indented_line */
{ yylhsminor.yy0 = yymsp[-1].minor.yy0; token_chain_append(yymsp[-1].minor.yy0, yymsp[0].minor.yy0); yymsp[0].minor.yy0->type = LINE_CONTINUATION; }
  yymsp[-1].minor.yy0 = yylhsminor.yy0;
        break;
      case 44: /* def_citation ::= LINE_DEF_CITATION para_lines cont_blocks */
      case 47: /* def_footnote ::= LINE_DEF_FOOTNOTE para_lines cont_blocks */ yytestcase(yyruleno==47);
{ yylhsminor.yy0 = yymsp[-2].minor.yy0; token_chain_append(yymsp[-2].minor.yy0, yymsp[-1].minor.yy0); token_chain_append(yymsp[-2].minor.yy0, yymsp[0].minor.yy0); }
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      case 53: /* fenced_block ::= LINE_FENCE_BACKTICK fenced_lines LINE_FENCE_BACKTICK */
      case 55: /* fenced_block ::= LINE_FENCE_BACKTICK_START fenced_lines LINE_FENCE_BACKTICK */ yytestcase(yyruleno==55);
{ yylhsminor.yy0 = yymsp[-2].minor.yy0; token_chain_append(yymsp[-2].minor.yy0, yymsp[-1].minor.yy0); token_chain_append(yymsp[-2].minor.yy0, yymsp[0].minor.yy0); yymsp[0].minor.yy0->child->type = CODE_FENCE; }
  yymsp[-2].minor.yy0 = yylhsminor.yy0;
        break;
      default:
      /* (60) para ::= LINE_PLAIN */ yytestcase(yyruleno==60);
      /* (61) para_lines ::= para_line (OPTIMIZED OUT) */ assert(yyruleno!=61);
      /* (62) para_line ::= LINE_CONTINUATION */ yytestcase(yyruleno==62);
      /* (63) indented_code ::= LINE_INDENTED_TAB */ yytestcase(yyruleno==63);
      /* (64) indented_code ::= LINE_INDENTED_SPACE */ yytestcase(yyruleno==64);
      /* (65) code_line ::= indented_line (OPTIMIZED OUT) */ assert(yyruleno!=65);
      /* (66) code_line ::= LINE_EMPTY */ yytestcase(yyruleno==66);
      /* (67) indented_line ::= LINE_INDENTED_TAB */ yytestcase(yyruleno==67);
      /* (68) indented_line ::= LINE_INDENTED_SPACE */ yytestcase(yyruleno==68);
      /* (69) empty ::= LINE_EMPTY */ yytestcase(yyruleno==69);
      /* (70) blockquote ::= LINE_BLOCKQUOTE */ yytestcase(yyruleno==70);
      /* (71) quote_lines ::= quote_line (OPTIMIZED OUT) */ assert(yyruleno!=71);
      /* (72) quote_line ::= LINE_BLOCKQUOTE */ yytestcase(yyruleno==72);
      /* (73) quote_line ::= LINE_CONTINUATION */ yytestcase(yyruleno==73);
      /* (74) list_bulleted ::= item_bulleted (OPTIMIZED OUT) */ assert(yyruleno!=74);
      /* (75) list_enumerated ::= item_enumerated (OPTIMIZED OUT) */ assert(yyruleno!=75);
      /* (76) cont_blocks ::= cont_block (OPTIMIZED OUT) */ assert(yyruleno!=76);
      /* (77) cont_block ::= empty */ yytestcase(yyruleno==77);
      /* (78) table ::= LINE_TABLE */ yytestcase(yyruleno==78);
      /* (79) def_citation ::= LINE_DEF_CITATION */ yytestcase(yyruleno==79);
      /* (80) def_footnote ::= LINE_DEF_FOOTNOTE */ yytestcase(yyruleno==80);
      /* (81) def_link ::= LINE_DEF_LINK */ yytestcase(yyruleno==81);
      /* (82) html_block ::= LINE_HTML */ yytestcase(yyruleno==82);
      /* (83) html_block_lines ::= html_block_line (OPTIMIZED OUT) */ assert(yyruleno!=83);
      /* (84) html_block_line ::= LINE_CONTINUATION */ yytestcase(yyruleno==84);
      /* (85) html_block_line ::= LINE_HTML */ yytestcase(yyruleno==85);
      /* (86) fenced_lines ::= fenced_line (OPTIMIZED OUT) */ assert(yyruleno!=86);
      /* (87) fenced_line ::= LINE_CONTINUATION */ yytestcase(yyruleno==87);
      /* (88) fenced_line ::= LINE_EMPTY */ yytestcase(yyruleno==88);
      /* (89) meta_block ::= LINE_META */ yytestcase(yyruleno==89);
      /* (90) meta_lines ::= meta_line (OPTIMIZED OUT) */ assert(yyruleno!=90);
      /* (91) meta_line ::= LINE_META */ yytestcase(yyruleno==91);
      /* (92) meta_line ::= LINE_CONTINUATION */ yytestcase(yyruleno==92);
        break;
/********** End reduce actions ************************************************/
  };
  assert( yyruleno<sizeof(yyRuleInfo)/sizeof(yyRuleInfo[0]) );
  yygoto = yyRuleInfo[yyruleno].lhs;
  yysize = yyRuleInfo[yyruleno].nrhs;
  yyact = yy_find_reduce_action(yymsp[-yysize].stateno,(YYCODETYPE)yygoto);
  if( yyact <= YY_MAX_SHIFTREDUCE ){
    if( yyact>YY_MAX_SHIFT ){
      yyact += YY_MIN_REDUCE - YY_MIN_SHIFTREDUCE;
    }
    yymsp -= yysize-1;
    yypParser->yytos = yymsp;
    yymsp->stateno = (YYACTIONTYPE)yyact;
    yymsp->major = (YYCODETYPE)yygoto;
    yyTraceShift(yypParser, yyact);
  }else{
    assert( yyact == YY_ACCEPT_ACTION );
    yypParser->yytos -= yysize;
    yy_accept(yypParser);
  }
}

/*
** The following code executes when the parse fails
*/
#ifndef YYNOERRORRECOVERY
static void yy_parse_failed(
  yyParser *yypParser           /* The parser */
){
  ParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yytos>yypParser->yystack ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
/************ Begin %parse_failure code ***************************************/

	fprintf(stderr, "Parser failed to successfully parse.\n");
/************ End %parse_failure code *****************************************/
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}
#endif /* YYNOERRORRECOVERY */

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  ParseTOKENTYPE yyminor         /* The minor type of the error token */
){
  ParseARG_FETCH;
#define TOKEN yyminor
/************ Begin %syntax_error code ****************************************/

#ifndef NDEBUG
	fprintf(stderr,"Parser syntax error.\n");
	int n = sizeof(yyTokenName) / sizeof(yyTokenName[0]);
	for (int i = 0; i < n; ++i) {
		int a = yy_find_shift_action(yypParser, (YYCODETYPE)i);
		if (a < YYNSTATE + YYNRULE) {
			fprintf(stderr,"expected token: %s\n", yyTokenName[i]);
		}
	}
#endif
/************ End %syntax_error code ******************************************/
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  ParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
  }
#endif
#ifndef YYNOERRORRECOVERY
  yypParser->yyerrcnt = -1;
#endif
  assert( yypParser->yytos==yypParser->yystack );
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
/*********** Begin %parse_accept code *****************************************/
/*********** End %parse_accept code *******************************************/
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "ParseAlloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure.)
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/
void Parse(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  ParseTOKENTYPE yyminor       /* The value for the token */
  ParseARG_PDECL               /* Optional %extra_argument parameter */
){
  YYMINORTYPE yyminorunion;
  unsigned int yyact;   /* The parser action. */
#if !defined(YYERRORSYMBOL) && !defined(YYNOERRORRECOVERY)
  int yyendofinput;     /* True if we are at the end of input */
#endif
#ifdef YYERRORSYMBOL
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
#endif
  yyParser *yypParser;  /* The parser */

  yypParser = (yyParser*)yyp;
  assert( yypParser->yytos!=0 );
#if !defined(YYERRORSYMBOL) && !defined(YYNOERRORRECOVERY)
  yyendofinput = (yymajor==0);
#endif
  ParseARG_STORE;

#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sInput '%s'\n",yyTracePrompt,yyTokenName[yymajor]);
  }
#endif

  do{
    yyact = yy_find_shift_action(yypParser,(YYCODETYPE)yymajor);
    if( yyact <= YY_MAX_SHIFTREDUCE ){
      yy_shift(yypParser,yyact,yymajor,yyminor);
#ifndef YYNOERRORRECOVERY
      yypParser->yyerrcnt--;
#endif
      yymajor = YYNOCODE;
    }else if( yyact <= YY_MAX_REDUCE ){
      yy_reduce(yypParser,yyact-YY_MIN_REDUCE);
    }else{
      assert( yyact == YY_ERROR_ACTION );
      yyminorunion.yy0 = yyminor;
#ifdef YYERRORSYMBOL
      int yymx;
#endif
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE,"%sSyntax Error!\n",yyTracePrompt);
      }
#endif
#ifdef YYERRORSYMBOL
      /* A syntax error has occurred.
      ** The response to an error depends upon whether or not the
      ** grammar defines an error token "ERROR".  
      **
      ** This is what we do if the grammar does define ERROR:
      **
      **  * Call the %syntax_error function.
      **
      **  * Begin popping the stack until we enter a state where
      **    it is legal to shift the error symbol, then shift
      **    the error symbol.
      **
      **  * Set the error count to three.
      **
      **  * Begin accepting and shifting new tokens.  No new error
      **    processing will occur until three tokens have been
      **    shifted successfully.
      **
      */
      if( yypParser->yyerrcnt<0 ){
        yy_syntax_error(yypParser,yymajor,yyminor);
      }
      yymx = yypParser->yytos->major;
      if( yymx==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE,"%sDiscard input token %s\n",
             yyTracePrompt,yyTokenName[yymajor]);
        }
#endif
        yy_destructor(yypParser, (YYCODETYPE)yymajor, &yyminorunion);
        yymajor = YYNOCODE;
      }else{
        while( yypParser->yytos >= yypParser->yystack
            && yymx != YYERRORSYMBOL
            && (yyact = yy_find_reduce_action(
                        yypParser->yytos->stateno,
                        YYERRORSYMBOL)) >= YY_MIN_REDUCE
        ){
          yy_pop_parser_stack(yypParser);
        }
        if( yypParser->yytos < yypParser->yystack || yymajor==0 ){
          yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
#ifndef YYNOERRORRECOVERY
          yypParser->yyerrcnt = -1;
#endif
          yymajor = YYNOCODE;
        }else if( yymx!=YYERRORSYMBOL ){
          yy_shift(yypParser,yyact,YYERRORSYMBOL,yyminor);
        }
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
#elif defined(YYNOERRORRECOVERY)
      /* If the YYNOERRORRECOVERY macro is defined, then do not attempt to
      ** do any kind of error recovery.  Instead, simply invoke the syntax
      ** error routine and continue going as if nothing had happened.
      **
      ** Applications can set this macro (for example inside %include) if
      ** they intend to abandon the parse upon the first syntax error seen.
      */
      yy_syntax_error(yypParser,yymajor, yyminor);
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      yymajor = YYNOCODE;
      
#else  /* YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( yypParser->yyerrcnt<=0 ){
        yy_syntax_error(yypParser,yymajor, yyminor);
      }
      yypParser->yyerrcnt = 3;
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      if( yyendofinput ){
        yy_parse_failed(yypParser);
#ifndef YYNOERRORRECOVERY
        yypParser->yyerrcnt = -1;
#endif
      }
      yymajor = YYNOCODE;
#endif
    }
  }while( yymajor!=YYNOCODE && yypParser->yytos>yypParser->yystack );
#ifndef NDEBUG
  if( yyTraceFILE ){
    yyStackEntry *i;
    char cDiv = '[';
    fprintf(yyTraceFILE,"%sReturn. Stack=",yyTracePrompt);
    for(i=&yypParser->yystack[1]; i<=yypParser->yytos; i++){
      fprintf(yyTraceFILE,"%c%s", cDiv, yyTokenName[i->major]);
      cDiv = ' ';
    }
    fprintf(yyTraceFILE,"]\n");
  }
#endif
  return;
}
