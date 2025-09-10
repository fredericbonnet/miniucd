/**
 * @file regc_ucd.inc
 *
 * This file implements Unicode regexp symbol and class support for Henry
 * Spencer's regexp library. Simply include it in regc_locale.c.
  * 
 * It is automatically generated from the UCD source files using the script
 * `ucdGenerate.js`; Do not modify.
*/

/*
===========================================================================*//*!
\defgroup regexp Henry Spencer's regexp library integration
\{*//*==========================================================================
*/

/*
 * Prototypes for functions used only in this file.
 */

/*! \cond IGNORE */
<% 
  for (const className in classes) {
%>static struct cvec *    miniucdGetCvec_<%-className%>(struct vars *v);
<% 
  }
%>/*! \endcond *//* IGNORE */


/**
 * POSIX+ASCII symbolic names, for use with the '*[.symbol.]*' syntax within
 * bracket expressions.
 *
 * We use the compiled UCD data accordingly to generate character ranges 
 * suitable for the regexp engine.
 */
typedef struct SymbolInfo {
    const char *const name;
    const celt code;
} SymbolInfo;
static const SymbolInfo symbols[] = {<%
  for (const [symbol, char] of Object.entries(symbols)) {
    let charCode = char.charCodeAt(0);
    let displayChar = char;

    if (charCode < 0x20 || charCode == 0x7F) {
      displayChar = "\\x" + charCode
        .toString(16)
        .toUpperCase()
        .padStart(2, "0");
    } else {
      switch (char) {
        case "\\":
        case "'":
        case '"':
          displayChar = "\\" + char;
          break;
      }
    }

    const padding = " ".repeat(Math.max(0, 23 - symbol.length));
%>
    {"<%-symbol%>",<%-padding%> '<%-displayChar%>'},<%
  }
%>
    {NULL}
};

/**
 * POSIX character classes, for use with the '*[:classname:]*' syntax within
 * bracket expressions.
 *
 * We use the compiled UCD data accordingly to generate character ranges 
 * suitable for the regexp engine.
 *
 * @see http://unicode.org/reports/tr18/
 * @see http://www.regular-expressions.info/posixbrackets.html
 */
typedef struct cvec * (ClassCvecProc) (struct vars *v);
typedef struct ClassInfo {
    const char *name;
     ClassCvecProc *proc;
     ClassCvecProc *procNocase;
} ClassInfo;
static const ClassInfo classes[] = {<%
  for (const className in classes) {
    const classNocase = classes_nocase[className] || className;
    const padding = " ".repeat(Math.max(0, 6 - className.length));
%>
    {"<%-className%>",<%-padding%>  miniucdGetCvec_<%-className%>,<%-padding%> miniucdGetCvec_<%-classNocase%>},<% 
  } 
%>
    {NULL}
};
<% 
  for (const [className, criteria] of Object.entries(classes)) {
    const { charValues, rangeValues } = processClassCriteria(
      criteria,
      propertyValues
    );
    const CLASS = className.toUpperCase();
    const padding = " ".repeat(Math.max(0, 6 - className.length));
    const nbChars = charValues.length;
    const nbRanges = Math.floor(rangeValues.length / 2);

    if (nbChars > 0) {
%>
/**
 * Codepoints of individual characters for class '**<%-className%>**'.
 *
 * Automatically generated from the UCD.
 *
 * @see ranges_<%-className%>
 */
static const chr chars_<%-className%>[] = {<%- formatArrayValues(charValues, 8) %>};
<% 
    } 
%>
/**
 * Number of individual characters for class '**<%-className%>**'.
 *
 * Automatically generated from the UCD.
 *
 * @see chars_<%-className%>
 */
#define NBCHARS_<%-CLASS%><%-padding%>  <%-nbChars%>
<%
    if (nbRanges > 0) {
%>
/**
 * Codepoint ranges of characters for class '**<%-className%>**'.
 *
 * Automatically generated from the UCD.
 *
 * @see chars_<%-className%>
 */
static const chr ranges_<%-className%>[] = {<%- formatArrayValues(rangeValues, 8) %>};
<%
    }
%>
/**
 * Number of character ranges for class '**<%-className%>**'.
 *
 * Automatically generated from the UCD.
 *
 * @see ranges_<%-className%>
 */
#define NBRANGES_<%-CLASS%><%-padding%> <%-nbRanges%>
<%
  }

  for (const className in classes) {
    const CLASS = className.toUpperCase();
%>
/**
 * Build vector of characters and ranges for class '**<%-className%>**'.
 *
 * @return Vector for class '**<%-className%>**'.
 *
 * @see NBCHARS_<%-CLASS%>
 * @see NBRANGES_<%-CLASS%>
 * @see classes
 */
static struct cvec *
miniucdGetCvec_<%-className%>(
    struct vars *v) /*!< Context. */
{
    struct cvec *cv;
    const chr *c;

    cv = getcvec(v, NBCHARS_<%-CLASS%>, NBRANGES_<%-CLASS%>);
    NOERRN();
#if NBCHARS_<%-CLASS%> > 0
    for (c=chars_<%-className%>; c < chars_<%-className%>+NBCHARS_<%-CLASS%>; c++) {
        addchr(cv, *c);
    }
#endif
#if NBRANGES_<%-CLASS%> > 0
    for (c=ranges_<%-className%>; c < ranges_<%-className%>+NBRANGES_<%-CLASS%>*2; c+=2) {
        addrange(cv, *c, *(c+1));
    }
#endif
    return cv;
}
<%
  }
%>

/* End of Henry Spencer's regexp library integration *//*!\}*/
