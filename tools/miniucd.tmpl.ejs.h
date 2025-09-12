/**
 * @file miniucd.h
 *
 * This file is a single-header implementation of the Unicode Character Database
 * (UCD) version <%- version %>. 
 * 
 * It is automatically generated from the UCD source files using the script
 * `ucdGenerate.js`; Do not modify.
 */

#ifndef _MINIUCD
#define _MINIUCD

#ifndef MiniUCD_Char
#   define MiniUCD_Char uint32_t
#endif
#ifndef MINIUCD_DEF
#   define MINIUCD_DEF
#endif


/*
===========================================================================*//*!
\defgroup ucd Unicode Character Database
\{*//*==========================================================================
*/

/***************************************************************************//*!
 * \defgroup properties UCD Properties
 *
 * Unicode Character Database (UCD) property types & values.
 ***************************************************************************\{*/

/**
 * Unicode character properties.
 *
 * Names are automatically generated from the UCD file `PropertyAliases.txt`.
 */
typedef enum MiniUCD_Property {<%
  // Generate comments & code.

  let number = 0;
  for (const [property, info] of Object.entries(properties)) {
    if (info.type === "deprecated") continue;

    number++;

    const PROPERTY = property.toUpperCase();
    const aliases = propertyAliases[property];
%>
    /** <%- aliases.map(alias => `'**${alias}**'`).join(", ") %> */
    MINIUCD_<%- PROPERTY %>=<%- number %>,
<%
    const ALIASES = [];
    for (const alias of aliases) {
      const ALIAS = alias.toUpperCase();
      if (ALIASES.includes(ALIAS)) continue;
      ALIASES.push(ALIAS);
      if (ALIAS === PROPERTY) continue;
%>        /** Alias for #MINIUCD_<%- PROPERTY %> */
        MINIUCD_<%- ALIAS %>=MINIUCD_<%- PROPERTY %>,
<%
    }
  }
%>} MiniUCD_Property;

/**
 * Number of Unicode character properties.
 *
 * @see MiniUCD_Property
 */
#define MINIUCD_NBPROPERTIES  <%- number %>

<%
  // Generate comments & code.

  for (const [property, info] of Object.entries(properties)) {
    if (info.type !== "enum" || !info.values) continue;

    const Property = capitalize(property);
    const PROPERTY = property.toUpperCase();
    const padding = " ".repeat(Math.max(0, 10 - property.length));
%>/**
 * Values for Unicode character property '**<%- property %>**'.
 *
 * Names are automatically generated from the UCD file 
 * `PropertyValueAliases.txt`.
 *
 * @see MINIUCD_<%- PROPERTY %>
 */
typedef enum MiniUCD_<%- Property %> {<%
    let number = 0;
    for (const valueAliases of info.values) {
      number++;

      const aliases = [];
      const ALIASES = [];

      for (const alias of valueAliases) {
        const ALIAS = toConstant(alias);
          aliases.push(alias);
        if (!ALIASES.includes(ALIAS)) {ALIASES.push(ALIAS);}
      }
      
      const VALUE = ALIASES[0] || "";
%>
    /** <%- aliases.map(alias => `'**${alias}**'`).join(", ") %> */ 
    MINIUCD_<%- PROPERTY %>_<%- VALUE %>=<%- number %>,
<%
      for (const ALIAS of ALIASES) {
        if (ALIAS === VALUE) continue;
%>        /** Alias for #MINIUCD_<%- PROPERTY %>_<%- VALUE %> */
        MINIUCD_<%- PROPERTY %>_<%- ALIAS %>=MINIUCD_<%- PROPERTY %>_<%- VALUE %>,
<%
      }
    }
%>} MiniUCD_<%- Property %>;

/**
 * Number of values for Unicode character property '**<%- property %>**'.
 *
 * @see MiniUCD_<%- Property %>
 */
#define MINIUCD_<%- PROPERTY %>_NBVALUES<%- padding %> <%- number %>

<%
  }
%>/* End of UCD Properties *//*!\}*/


/***************************************************************************//*!
 * \defgroup accessors UCD Property Accessors
 ***************************************************************************\{*/

<%
  //
  // Generate accessor declarations for all known properties.
  //

  for (const [property, info] of Object.entries(properties)) {
    if (["deprecated", "name"].includes(info.type)) continue;

    const Property = capitalize(property);

    switch (info.type) {
      case "boolean":
%>MINIUCD_DEF int                 MiniUCD_GetProperty_<%- Property %>(MiniUCD_Char c);
<%      
        break;

      case "enum": {
        const padding = " ".repeat(Math.max(0, 7 - property.length));
%>MINIUCD_DEF MiniUCD_<%- Property %><%- padding %> MiniUCD_GetProperty_<%- Property %>(MiniUCD_Char c);
<%      
        break;
      }

      case "cp":
%>MINIUCD_DEF MiniUCD_Char    MiniUCD_GetProperty_<%- Property %>(MiniUCD_Char c);
<%      
        break;

      case "numeric":
%>MINIUCD_DEF const char *    MiniUCD_GetProperty_<%- Property %>(MiniUCD_Char c);
<%      
        break;

      case "string":
%>MINIUCD_DEF const char *    MiniUCD_GetProperty_<%- Property %>(MiniUCD_Char c);
<%      
        break;

      case "mcp":
%>MINIUCD_DEF const int *     MiniUCD_GetProperty_<%- Property %>(MiniUCD_Char c,
                                size_t *lengthPtr);
<%      
        break;

      case "enumList": {
        const elementProperty = info.enumType

        const nbValues = properties[elementProperty].values.length;
        const valueType = nbValues < 256 ? "char" : nbValues < 65536 ? "short" : "int";
%>MINIUCD_DEF const unsigned <%- valueType %> * MiniUCD_GetProperty_<%- Property %>(
                            MiniUCD_Char c,size_t *lengthPtr);
<%      
        break;
      }
    }
  }

  //
  // Generate accessor macros for all known property aliases.
  //

  for (const [property, info] of Object.entries(properties)) {
    if (["deprecated", "name"].includes(info.type)) continue;

    const Property = capitalize(property);
    const PROPERTY = property.toUpperCase();

    const Aliases = []
    for (const alias of propertyAliases[property]) {
      const Alias = capitalize(alias);
      if (alias === property || Alias === Property || Aliases.includes(Alias)) continue;
      Aliases.push(Alias);
      const ALIAS = alias.toUpperCase();
%>
/**
 * Get value of UCD property '**<%- alias %>**' for given codepoint.
 * Alias for property UCD property accessor '**<%- Property %>**'
 *
 * @param c Codepoint to get UCD property value for.
 *<%
      switch (info.type) {
        case "enumList": {
          const ElementProperty = capitalize(info.enumType);
%>
 * @param[out] lengthPtr    Value list length.
 *
 * @return Beginning of value list for UCD property '**<%- property %>**'. 
 *
 * Entries can be safely cast to <MiniUCD_<%- ElementProperty %>>.
<%
          break;
        }

        case "mcp":
%>
 * @param[out] lengthPtr    Value list length.
 *
 * @return Beginning of value list for UCD property '**<%- property %>**'. 
<%
          break;

        default:
%>
 * @return Value of UCD property '**<%- alias %>**'.
<%
      }
%> *
 * @see MINIUCD_<%- PROPERTY %>
 * @see MiniUCD_GetProperty_<%- Property %>
 * @see MINIUCD_<%- ALIAS %>
 */<%
      switch (info.type) {
        case "enumList":
        case "mcp":
%>
#define MiniUCD_GetProperty_<%- Alias %>(c, lengthPtr) \
    MiniUCD_GetProperty_<%- Property %>((c), (lengthPtr))
<%
          break;

        default:
%>
#define MiniUCD_GetProperty_<%- Alias %>(c) \
    MiniUCD_GetProperty_<%- Property %>(c)
<%
      }
    }
  }
%>
/* End of UCD Property Accessors *//*!\}*/


#ifdef MINIUCD_IMPLEMENTATION
#include <limits.h>

/*
 * Prototypes for functions used only in this file.
 */

/*! \cond IGNORE */
static int              miniucdGetRange(MiniUCD_Char c, 
                            const MiniUCD_Char *ranges, int nbRanges);
/*! \endcond *//* IGNORE */


/*
===========================================================================*//*!
\internal \defgroup ucd_data Compiled UCD Data

Statically compiled Unicode Character Database (UCD).

The Unicode Character Database (UCD) defines a set of properties for every known
codepoint in the standard. This represents a huge quantity of raw data for over
one million codepoints. Storing this data as flat C arrays of high level
structures is not realistic, as it would inflate the size of the compiled binary
by tens of megabytes and would overburden the client processes' memory.

To solve this problem we compile the UCD in the most compact way possible using
a combination of techniques:

- Don't store flat arrays of values, but contiguous ranges of values that are
  binary-searched during lookup; as neighboring codepoints in the UCD often
  share the same properties, this technique is very efficient.
- Use enums for all enumerated properties to minimize storage (values fit a
  single byte in most cases).
- Boolean properties need only the first value since consecutive ranges have
  alternate values.
- Use atom tables for strings and complex datatypes to reuse storage.

That way, all but the name properties are statically defined in the library
without too much overhead. By comparison the uncompressed textual version of the
UCD weighs around 15MB, and the grouped XML version around 40MB.
\{*//*==========================================================================
*/

/** @beginprivate @cond PRIVATE */

<%
  // At this point we have per-property ranges of values, now output C file.
  // For each property we output two C arrays:
  //
  //  - First one is used for indexing; it gives the first codepoint in a
  //    contiguous range of values (omitting the first entry which is always zero).
  //    This array is binary-searched during lookup until we find the range
  //    containing a given codepoint.
  //  - Second one is used for values. The index computed during lookup in the
  //    first array gives the index in this second array (1 is added for the
  //    missing first entry).
  //  - If the value type is enumerated, values are directly stored in the second
  //    array. Otherwise, the latter stores an index to a third array containing
  //    the set of unique values. This can be a C string or the first element in
  //    a list (usually zero-terminated, depending on the type).
  //
  // The code always chooses the shortest integral type for indices & value storage
  // (e.g. char for values < 256).

  for (const [property, info] of Object.entries(properties)) {
    if (["deprecated", "name"].includes(info.type)) continue;

    const rangeValues = propertyValues[property];

    const Property = capitalize(property);
    const PROPERTY = property.toUpperCase();
    const padding = " ".repeat(Math.max(0, 10 - property.length));

    //
    // Range array
    //

    const ranges = Object.keys(rangeValues).filter((cp) => parseInt(cp) !== 0);
    const nbRanges = ranges.length + 1;
%>/**
 * Codepoint ranges of values for UCD property '**<%- property %>**'.
 *
 * Automatically generated from the UCD.
 *<%
    if (info.type === "boolean") {
%>
 * @see UCD_VALUE_0_<%- PROPERTY %>
<%
    } else {
%>
 * @see ucdValues_<%- property %>
<%
    }
%> * @see MINIUCD_<%- PROPERTY %>
 */
static const MiniUCD_Char ucdRanges_<%- property %>[] = {<%-
  formatArrayValues(ranges)
%>};

/**
 * Number of ranges of values for UCD property '**<%- property %>**'.
 *
 * Automatically generated from the UCD.
 *
 * @see ucdRanges_<%- property %>
 * @see MINIUCD_<%- PROPERTY %>
 */
#define UCD_NBRANGES_<%- PROPERTY %><%- padding %> <%- nbRanges %>

<%
    //
    // Value array.
    //
    
    switch (info.type) {
      case "boolean": {
        // Boolean property, only store first value as consecutive ranges have
        // alternating values
        const rawValue = rangeValues["0x0000"] || 0;
        const value0 = rawValue === "Y" || rawValue === 1 ? 1 : 0;
%>/**
 * Value of first range for UCD property '**<%- property %>**'.
 *
 * This property is boolean, so storing the first value is sufficient as
 * consecutive ranges have alternating values.
 *
 * Automatically generated from the UCD.
 *
 * @see ucdRanges_<%- property %>
 * @see MINIUCD_<%- PROPERTY %>
 */
#define UCD_VALUE_0_<%- PROPERTY %><%- padding %>  <%- value0 %>
<%
        break;
      }

      case "enum": {
        // Enumerated values.
        const values = Object.values(rangeValues);
        const enumValues = values.map(value => 
          `MINIUCD_${PROPERTY}_${toConstant(value)}`
        );
%>/**
 * Per-range values for UCD property '**<%- property %>**'.
 *
 * Automatically generated from the UCD.
 *
 * @see ucdRanges_<%- property %>
 * @see MINIUCD_<%- PROPERTY %>
 * @see MiniUCD_<%- Property %>
 */
static const MiniUCD_<%- Property %> ucdValues_<%- property %>[] = {<%-
  formatArrayValues(enumValues, 4)
%>};
<%
        break;
      }

      case "cp": {
        // Codepoint offset values.
        const values = Object.values(rangeValues);
%>/**
 * Per-range values for UCD property '**<%- property %>**'.
 *
 * This property is single-codepoint, stored as offsets from the codepoint
 * value so as to build large ranges of identical values.
 *
 * Automatically generated from the UCD.
 *
 * @see ucdRanges_<%- property %>
 * @see MINIUCD_<%- PROPERTY %>
 */
static const MiniUCD_Char ucdValues_<%- property %>[] = {<%-
  formatArrayValues(values, 16)
%>};
<%
        break;
      }

      case "numeric":
      case "string":
      case "mcp":
      case "enumList": {
        // Value array contains indices to an atom array storing unique values.
        // Note: numeric properties are stored as strings.
        const values = Object.values(rangeValues);
        const [indices, uniqueValues] = processRangeValues(values, info.type);
        const valueType =
          indices.length < 256
            ? "char"
            : indices.length < 65536
            ? "short"
            : "int";
%>/**
 * Per-range atom indices for UCD property '**<%- property %>**'.
 *
 * This array stores the index of the value in the atom array for each range. 
 * Atoms are stored in the separate array #ucdAtoms_<%- property %>.
 *
 * Automatically generated from the UCD.
 *
 * @see ucdRanges_<%- property %>
 * @see ucdAtoms_<%- property %>
 * @see MINIUCD_<%- PROPERTY %>
 */
static const unsigned <%- valueType %> ucdValues_<%- property %>[] = {<%-
  formatArrayValues(indices, 16)
%>};

/**
 * Atoms for UCD property '**<%- property %>**'.
 *
 * Automatically generated from the UCD.
 *
 * @see ucdValues_<%- property %>
 * @see MINIUCD_<%- PROPERTY %>
<%
        switch (info.type) {
          case "numeric":
          case "string":
            // Strings.
%> */
static const char * const ucdAtoms_<%- property %>[] = {<%-
  formatArrayValues(
    uniqueValues.map(
      atom => 
      `"${atom.replace(/[\\"]/g, "\$&")}"`
    ),
  8)
%>};
<%      
            break;

          case "mcp": {
            // NUL-terminated codepoint lists.
            // - For length=0, zero.
            // - For length=1, offset then zero.
            // - For length>1, zero-terminated list of codepoint.
            const atoms = [];
            for (const value of uniqueValues) {
              if (value !== 0 && value !== "0") {
                // Values.
                if (Array.isArray(value)) {
                  atoms.push(...value.map(toHex));
                } else {
                  atoms.push(value);
                }
              }
      
              // Terminator.
              atoms.push(0);
            }
%> */
static const int ucdAtoms_<%- property %>[] = {<%-
  formatArrayValues(atoms, 16)
%>};
<%
            break;
          }

          case "enumList":{
            // Zero-terminated list of enum values.
            const elementProperty = info.enumType;
            const ElementProperty = capitalize(elementProperty);
            const ELEMENT_PROPERTY = elementProperty.toUpperCase();
            const nbValues = properties[elementProperty].values.length;
            const valueType =
              nbValues < 256 ? "char" : nbValues < 65536 ? "short" : "int";
            const atomType = `unsigned ${valueType}`;
            const atoms = [];
            for (const value of uniqueValues) {
              if (Array.isArray(value)) {
                atoms.push(...value);
              } else {
                atoms.push(value);
              }

              // Terminator.
              atoms.push(0);
            }
%> * @see MiniUCD_<%- ElementProperty %>
 */
static const unsigned <%- valueType %> ucdAtoms_<%- property %>[] = {<%-
  formatArrayValues(
    atoms.map(
      atom =>
      atom === 0 ? 0 : `MINIUCD_${ELEMENT_PROPERTY}_${toConstant(atom)}`
    ),
  4)
%>};
<%      
            break;
          }
        }
        break;
      }
    }
%>
<%      
  }
%>/** @endcond @endprivate */

/* End of Compiled UCD Data *//*!\}*/


/*******************************************************************************
 * UCD Property Accessors
 ******************************************************************************/

/** @beginprivate @cond PRIVATE */

/**
 * Get the range index containing the given codepoint.
 *
 * @return Range index.*/
static int
miniucdGetRange(
    MiniUCD_Char c,             /*!< Codepoint to lookup. */
    const MiniUCD_Char *ranges, /*!< Array of ranges boundaries. First one is
                                     omitted (first codepoint is always zero).*/
    int nbRanges)               /*!< Number of ranges. */
{
    int min, max;
    if (c < ranges[0] || nbRanges < 2) return 0;
    if (c >= ranges[nbRanges-2]) return nbRanges-1;
    for (min=1, max=nbRanges-1; max-min > 1;) {
        int mid = min+((max-min)>>1);
        if (c < ranges[mid-1]) max = mid;
        else min = mid;
    }
    return min;
}

/** @endcond @endprivate */
<%
  //
  // Now generate accessors for all known properties
  //

  for (const [property, info] of Object.entries(properties)) {
    if (!info || ["deprecated", "name"].includes(info.type)) continue;

    const Property = capitalize(property);
    const PROPERTY = property.toUpperCase();
%>
/**
 * Get value of UCD property '**<%- property %>**' for given codepoint.
 *<%
    switch (info.type) {
      case "boolean":
%>
 * @return Value of UCD property '**<%- property %>**'.
 *
 * @see MINIUCD_<%- PROPERTY %>
 */
int
MiniUCD_GetProperty_<%- Property %>(
    MiniUCD_Char c) /*!< Codepoint to get UCD property value for. */
{
    return (miniucdGetRange(c, ucdRanges_<%- property %>, UCD_NBRANGES_<%- PROPERTY %>)%2)
        ? !UCD_VALUE_0_<%- PROPERTY %>
        : UCD_VALUE_0_<%- PROPERTY %>;
}
<%
        break;

      case "enum":
%>
 * @return Value of UCD property '**<%- property %>**'.
 *
 * @see MINIUCD_<%- PROPERTY %>
 * @see MiniUCD_<%- Property %>
 */
MiniUCD_<%- Property %>
MiniUCD_GetProperty_<%- Property %>(
    MiniUCD_Char c) /*!< Codepoint to get UCD property value for. */
{
    return ucdValues_<%- property %>[miniucdGetRange(c, ucdRanges_<%- property %>, UCD_NBRANGES_<%- PROPERTY %>)];
}
<%
        break;

      case "cp":
%>
 * @return Value of UCD property '**<%- property %>**'.
 *
 * @see MINIUCD_<%- PROPERTY %>
 */
MiniUCD_Char
MiniUCD_GetProperty_<%- Property %>(
    MiniUCD_Char c) /*!< Codepoint to get UCD property value for. */
{
    return c+ucdValues_<%- property %>[miniucdGetRange(c, ucdRanges_<%- property %>, UCD_NBRANGES_<%- PROPERTY %>)];
}
<%
        break;

      case "numeric":
%>
 * @return Value of UCD property '**<%- property %>**'.
 *
 * @see MINIUCD_<%- PROPERTY %>
 */
const char *
MiniUCD_GetProperty_<%- Property %>(
    MiniUCD_Char c) /*!< Codepoint to get UCD property value for. */
{
    return ucdAtoms_<%- property %>[ucdValues_<%- property %>[miniucdGetRange(c, ucdRanges_<%- property %>, UCD_NBRANGES_<%- PROPERTY %>)]];
}
<%
        break;

      case "string":
%>
 * @return Value of UCD property '**<%- property %>**'.
 *
 * @see MINIUCD_<%- PROPERTY %>
 */
const char *
MiniUCD_GetProperty_<%- Property %>(
    MiniUCD_Char c) /*!< Codepoint to get UCD property value for. */
{
    return ucdAtoms_<%- property %>[ucdValues_<%- property %>[miniucdGetRange(c, ucdRanges_<%- property %>, UCD_NBRANGES_<%- PROPERTY %>)]];
}
<%
        break;

      case "mcp":
%>
 * @return Beginning of value list for UCD property '**<%- property %>**'.
 *
 * If list length is 1, the first entry gives the offset from the input
 * codepoint to get the actual single-value codepoint. Else, list entries give
 * codepoint values that can be safely cast to MiniUCD_Char.
 *
 * @see MINIUCD_<%- PROPERTY %>
 */
const int *
MiniUCD_GetProperty_<%- Property %>(
    MiniUCD_Char c, /*!< Codepoint to get UCD property value for. */

    /*! [out] Value list length. */
    size_t *lengthPtr)
{
    const int *first = ucdAtoms_<%- property %> + ucdValues_<%- property %>[miniucdGetRange(c, ucdRanges_<%- property %>, UCD_NBRANGES_<%- PROPERTY %>)],
        *last = first;
    while (*last != 0) last++;
    *lengthPtr = last-first;
    return first;
}
<%
        break;

      case "enumList":
        const elementProperty = info.enumType;
        const ElementProperty = capitalize(elementProperty);
        const nbValues = properties[elementProperty]?.values?.length || 0;
        const valueType = nbValues < 256 ? "char" : nbValues < 65536 ? "short" : "int";
%>
 * @return Beginning of value list for UCD property '**<%- property %>**'.
 *
 * Entries can be safely cast to #MiniUCD_<%- ElementProperty %>.
 *
 * @see MINIUCD_<%- PROPERTY %>
 * @see MiniUCD_<%- ElementProperty %>
 */
const unsigned <%- valueType %> *
MiniUCD_GetProperty_<%- Property %>(
    MiniUCD_Char c, /*!< Codepoint to get UCD property value for. */

    /*! [out] Value list length. */
    size_t *lengthPtr)
{
    const unsigned <%- valueType %>
        *first = ucdAtoms_<%- property %> + ucdValues_<%- property %>[miniucdGetRange(c, ucdRanges_<%- property %>, UCD_NBRANGES_<%- PROPERTY %>)],
        *last = first;
    while (*last != 0) last++;
    *lengthPtr = last-first;
    return first;
}
<%
        break;
    }
  }
%>
/* End of UCD Property Accessors */

#endif /* MINIUCD_IMPLEMENTATION */

/* End of Unicode Character Database *//*!\}*/

#endif /* _MINIUCD */
