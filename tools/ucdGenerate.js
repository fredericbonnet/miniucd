#!/usr/bin/env node

/**
 * @file ucdGenerate.js
 *
 * Generate Unicode Character Database C code from UCD data and templates.
 * Converted from ucdGenerate.tcl to use JavaScript and JSON data.
 */

import { readFileSync, writeFileSync, existsSync, mkdirSync } from "fs";
import ejs from "ejs";

import { version, ucdroot, datafile, outputdir } from "./config.js";

import { properties } from "./ucdPropertyTypes.js";
import { classes, classes_nocase, symbols } from "./ucdRegex.js";

// Check if required files exist
if (!existsSync(datafile)) {
  console.error(
    `Error: Data file ${datafile} not found. Run ucdParse.js first.`
  );
  process.exit(1);
}

if (!existsSync(ucdroot)) {
  console.error(`Error: UCD directory ${ucdroot} not found`);
  process.exit(1);
}

if (!existsSync(outputdir)) {
  mkdirSync(outputdir, { recursive: true });
  console.log(`Created directory: ${outputdir}`);
}

/**
 * Capitalize first letter of string
 */
function capitalize(str) {
  return str.charAt(0).toUpperCase() + str.slice(1);
}

/**
 * Parse PropertyAliases.txt and generate enum values for property names.
 */
function parsePropertyAliases() {
  const filePath = `${ucdroot}/PropertyAliases.txt`;
  const content = readFileSync(filePath, "utf8");

  const propertyAliases = {};
  for (const line of content.split("\n").map((line) => line.trim())) {
    // Skip blank lines.
    if (!line) continue;

    // Skip comments.
    if (line.startsWith("#")) continue;

    // Semicolon-separated list of property aliases
    const split = line.split(";");
    const aliases = split.map((alias) => alias.trim());
    for (const alias of aliases) {
      propertyAliases[alias] = aliases;
    }
  }

  return propertyAliases;
}

/**
 * Format array values with property-specific line wrapping (matching Tcl output exactly)
 */
function formatArrayValues(values, elementsPerLine = 8) {
  if (values.length === 0) return "";

  let result = ""; // Don't start with newline - let Tcl-style logic handle it
  let nb = 0;
  for (const value of values) {
    if (nb % elementsPerLine === 0) {
      // Wrap line.
      result += "\n    "; // Newline + 4-space indentation
    }
    result += value + ", ";
    nb++;
  }
  result += "\n"; // End with newline only
  return result;
}

const toHex = (v) => `0x${v.toString(16).toUpperCase().padStart(4, "0")}`;
const toConstant = (v) => v.toUpperCase().replace(/[.\-]/g, "_");

/**
 * Generate indices and uniqueValues for coatlUcd.c
 */
function processRangeValues(values, type) {
  // CRITICAL: Use the same string format as Tcl for sorting if we want to preserve output
  const valueKey = (value) => {
    if (Array.isArray(value)) {
      if (type === "enumList") {
        // For enumList, use space-separated string format (e.g., "Arab Copt")
        return value.join(" ");
      } else {
        // For mcp types, use hex format (e.g., "0x0073 0x0073")
        return value.map(toHex).join(" ");
      }
    } else {
      return String(value);
    }
  };
  const uniqueMap = new Map();
  for (const value of values) {
    uniqueMap.set(valueKey(value), value);
  }
  const uniqueValues = Array.from(uniqueMap.values()).sort((a, b) => {
    const aStr = valueKey(a);
    const bStr = valueKey(b);
    if (aStr < bStr) return -1;
    if (aStr > bStr) return 1;
    return 0;
  });
  const valueIndices = new Map();
  let nb = 0;
  for (const value of uniqueValues) {
    valueIndices.set(valueKey(value), nb);
    nb++;
    switch (type) {
      case "numeric":
      case "string":
        break;
      default:
        if (value !== 0 && value !== "0") {
          const length = Array.isArray(value) ? value.length : 1;
          nb += length;
        }
    }
  }
  const indices = values.map((val) => valueIndices.get(valueKey(val)));

  return [indices, uniqueValues];
}

/**
 * Generate charValues and rangeValues for regc_ucd.inc
 */
function processClassCriteria(criteria, propertyValues) {
  const charValues = [];
  const rangeValues = [];

  // Handle single character
  if (criteria.char) {
    const charCode = criteria.char.charCodeAt(0);
    charValues.push(toHex(charCode));
  }

  // Handle multiple character ranges (like xdigit)
  if (criteria.range && Array.isArray(criteria.range)) {
    for (const rangePair of criteria.range) {
      const [start, end] = rangePair;
      const startCode = start.charCodeAt(0);
      const endCode = end.charCodeAt(0);
      rangeValues.push(toHex(startCode), toHex(endCode));
    }
  }

  // Handle property-based criteria
  if (criteria.property) {
    for (const [property, regex] of Object.entries(criteria.property)) {
      let match = false;
      let lastCp = 0;
      for (const [cpHex, value] of Object.entries(propertyValues[property])) {
        const cp = parseInt(cpHex, 16);
        if (regex.test(value)) {
          if (!match) {
            lastCp = cp;
            match = true;
          }
        } else if (match && cp != 0) {
          if (cp - lastCp === 1) {
            charValues.push(toHex(lastCp));
          } else {
            rangeValues.push(toHex(lastCp), toHex(cp - 1));
          }
          match = false;
        }
      }
      if (match) {
        rangeValues.push(toHex(lastCp), "COL_CHAR_MAX");
      }
    }
  }

  return { charValues, rangeValues };
}

/*
 * Parse PropertyValueAliases.txt and generate values for enumerated properties.
 */
function parsePropertyValueAliases(properties) {
  const filePath = `${ucdroot}/PropertyValueAliases.txt`;
  const content = readFileSync(filePath, "utf8");
  for (const line of content.split("\n")) {
    // Trailing comment.
    let [lineContent, comment] = line.split("#");

    lineContent = lineContent.trim();

    // Skip blank lines.
    if (!lineContent) continue;

    // Semicolon-separated list.
    const split = lineContent.split(";");

    // First one is property name.
    const property = split[0].trim();

    // Skip unknown & non-enum properties.
    if (!properties[property] || properties[property].type !== "enum") continue;

    if (property === "gc" && comment) {
      // Special case: don't define OR-ed General Category values (e.g.
      // L/Letter = Ll | Lm | Lo | Lt | Lu).
      continue;
    }

    // Remainder are value aliases.
    const aliases = [...new Set(split.slice(1).map((alias) => alias.trim()))];

    if (!properties[property].values) {
      properties[property].values = [];
    }
    properties[property].values.push(aliases);
  }
}

/**
 * Main generation function
 */
function main() {
  console.log("Loading data...");

  // Load parsed UCD data from JSON
  const ucdData = JSON.parse(readFileSync(datafile, "utf8"));
  const { propertyValues, lastCp } = ucdData;

  // Parse property aliases
  const propertyAliases = parsePropertyAliases();

  parsePropertyValueAliases(properties);

  ///////////////////////////////////////////////////////////////////////////
  // Generate miniucd.h
  ///////////////////////////////////////////////////////////////////////////

  console.log("Generating UCD header...");
  const miniucdTemplate = readFileSync("./miniucd.tmpl.ejs.h").toString();
  const miniucdContent = ejs.render(miniucdTemplate, {
    version,
    properties,
    propertyAliases,
    propertyValues,
    processRangeValues,
    formatArrayValues,
    toConstant,
    toHex,
    capitalize,
  });
  writeFileSync(`${outputdir}/miniucd.h`, miniucdContent);

  ///////////////////////////////////////////////////////////////////////////
  // Generate regc_ucd.inc
  ///////////////////////////////////////////////////////////////////////////

  console.log("Generating regex locale...");
  const regexTemplate = readFileSync("./regc_ucd.tmpl.ejs.inc").toString();
  const regexContent = ejs.render(regexTemplate, {
    classes,
    classes_nocase,
    symbols,
    propertyValues,
    processClassCriteria,
    formatArrayValues,
  });
  writeFileSync(`${outputdir}/regc_ucd.inc`, regexContent);

  console.log("Done generating files");
}

// Run main function
main();
