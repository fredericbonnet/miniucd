#!/usr/bin/env node

/**
 * @file ucdParse.js
 *
 * Parse Unicode Character Database (UCD) and generate property values.
 * Converted from ucdParse.tcl to output JSON instead of Tcl data.
 *
 * Requirements: npm install fast-xml-parser
 */

import { readFileSync, writeFileSync, existsSync } from "fs";
import { XMLParser } from "fast-xml-parser";

import { version, ucdroot, datafile } from "./config.js";

import { properties } from "./ucdPropertyTypes.js";

console.log("Parsing...");

// Check if required directories exist
if (!existsSync(ucdroot)) {
  console.error(`Error: UCD directory ${ucdroot} not found`);
  process.exit(1);
}

/**
 * Parse PropertyValueAliases.txt and generate values for enumerated properties.
 */
function parsePropertyValueAliases() {
  const filePath = `${ucdroot}/PropertyValueAliases.txt`;
  if (!existsSync(filePath)) {
    console.error(`Error: ${filePath} not found`);
    process.exit(1);
  }

  const content = readFileSync(filePath, "utf8");
  const lines = content.split("\n");

  const updatedProperties = { ...properties };

  for (const line of lines) {
    // Remove trailing comment
    let [lineContent] = line.split("#");
    lineContent = lineContent.trim();

    // Skip blank lines
    if (!lineContent) continue;

    // Semicolon-separated list
    const parts = lineContent.split(";").map((part) => part.trim());

    // First one is property name
    const property = parts[0];

    // Skip unknown & non-enum properties
    if (
      !updatedProperties[property] ||
      updatedProperties[property].type !== "enum"
    ) {
      continue;
    }

    // Special case: don't define OR-ed General Category values
    if (property === "gc" && line.includes("#")) {
      continue;
    }

    // Remainder are value aliases
    const aliases = [];
    for (let i = 1; i < parts.length; i++) {
      const alias = parts[i];
      if (alias && !aliases.includes(alias)) {
        aliases.push(alias);
      }
    }

    if (!updatedProperties[property].values) {
      updatedProperties[property].values = [];
    }
    updatedProperties[property].values.push(aliases);
  }

  return updatedProperties;
}

/**
 * Parse XML database and generate property values.
 */
function parseXMLDatabase(propertiesData) {
  console.log("  Parsing XML...");

  const xmlFile = `${ucdroot}/ucd.all.grouped.xml`;
  if (!existsSync(xmlFile)) {
    console.error(`Error: ${xmlFile} not found`);
    process.exit(1);
  }

  const xmlContent = readFileSync(xmlFile, "utf8");

  const parser = new XMLParser({
    ignoreAttributes: false,
    attributeNamePrefix: "@",
    parseAttributeValue: false,
    ignoreNameSpace: false,
    preserveOrder: false,
    parseTagValue: false,
  });

  const parsed = parser.parse(xmlContent);
  console.log(" Done");

  console.log("Building data...");

  const propertyValues = {};
  const lastValue = {};
  let lastCp = 0;

  // Navigate to the repertoire groups
  const groups = parsed.ucd?.repertoire?.group || [];
  const groupsArray = Array.isArray(groups) ? groups : [groups];

  const nbgroups = groupsArray.length;
  let igroup = 0;
  let dots = 0;

  for (const group of groupsArray) {
    process.stdout.write(`  Group ${++igroup}/${nbgroups}`);

    // Get group property values from attributes
    const groupAttributes = {};
    if (group) {
      for (const [key, value] of Object.entries(group)) {
        if (!key.startsWith("@")) continue;
        const property = key.substring(1);
        if (
          propertiesData[property] &&
          !["deprecated", "name"].includes(propertiesData[property].type)
        ) {
          groupAttributes[property] = value;
        }
      }
    }

    // Process ALL child elements (matching Tcl's childNodes behavior)
    const children = [];
    if (group.char) {
      const chars = Array.isArray(group.char) ? group.char : [group.char];
      children.push(...chars);
    }
    if (group.reserved) {
      const reserved = Array.isArray(group.reserved)
        ? group.reserved
        : [group.reserved];
      children.push(...reserved);
    }
    if (group.noncharacter) {
      const noncharacters = Array.isArray(group.noncharacter)
        ? group.noncharacter
        : [group.noncharacter];
      children.push(...noncharacters);
    }
    if (group.surrogate) {
      const surrogates = Array.isArray(group.surrogate)
        ? group.surrogate
        : [group.surrogate];
      children.push(...surrogates);
    }

    // CRITICAL: Sort children by codepoint to match Tcl processing order
    children.sort((a, b) => {
      const cpA = parseInt(a["@cp"] || a["@first-cp"] || "0", 16);
      const cpB = parseInt(b["@cp"] || b["@first-cp"] || "0", 16);
      return cpA - cpB;
    });

    for (const child of children) {
      if (!child) continue;

      // Get property values by merging child & group properties
      const attributes = { ...groupAttributes };

      // Add child-specific attributes
      if (child) {
        for (const [key, value] of Object.entries(child)) {
          if (!key.startsWith("@")) continue;
          const property = key.substring(1);
          if (
            propertiesData[property] &&
            !["deprecated", "name"].includes(propertiesData[property].type)
          ) {
            attributes[property] = value;
          }
        }
      }

      // Get codepoint info
      let cp, endCp;
      if (child["@cp"]) {
        cp = parseInt(child["@cp"], 16);
        endCp = cp;
      } else if (child["@first-cp"]) {
        cp = parseInt(child["@first-cp"], 16);
        endCp = parseInt(child["@last-cp"], 16);
      } else {
        continue;
      }

      lastCp = Math.max(lastCp, endCp);

      // Build value ranges - need to process ALL properties that exist in either group or child
      const allProperties = new Set([
        ...Object.keys(groupAttributes),
        ...Object.keys(child)
          .filter((k) => k.startsWith("@"))
          .map((k) => k.substring(1)),
      ]);

      for (const property of allProperties) {
        if (
          !propertiesData[property] ||
          ["deprecated", "name"].includes(propertiesData[property].type)
        ) {
          continue;
        }

        // Determine final value: child overrides group, or inherit from group
        let value;
        const childAttr = child[`@${property}`];
        if (childAttr !== undefined) {
          // Child has explicit override
          value = childAttr;
        } else if (groupAttributes[property] !== undefined) {
          // Inherit from group
          value = groupAttributes[property];
        } else {
          // No value available
          continue;
        }
        const propertyType = propertiesData[property]?.type;

        // Type-specific value handling
        let processedValue = value;

        switch (propertyType) {
          case "cp":
            // Single-codepoint property
            if (value === "" || value === "#") {
              processedValue = 0;
            } else {
              processedValue = parseInt(value, 16) - cp;
            }
            break;
          case "mcp":
            // Multiple-codepoint property
            if (value === "" || value === "#") {
              processedValue = 0;
            } else {
              const values = value.split(" ").filter((v) => v);
              if (values.length === 1) {
                processedValue = parseInt(values[0], 16) - cp;
              } else {
                processedValue = values.map((v) => parseInt(v, 16));
              }
            }
            break;
          case "enumList":
            // Enum list property (e.g., scx with multiple script values)
            if (value === "" || value === "#") {
              processedValue = [];
            } else {
              const values = value.split(" ").filter((v) => v);
              processedValue = values.length === 1 ? values[0] : values;
            }
            break;
        }

        // If value changed, this marks the beginning of a new range
        const key = `${property}`;

        // Deep comparison for arrays and objects
        function deepEqual(a, b) {
          if (a === b) return true;
          if (a == null || b == null) return false;
          if (Array.isArray(a) && Array.isArray(b)) {
            if (a.length !== b.length) return false;
            for (let i = 0; i < a.length; i++) {
              if (a[i] !== b[i]) return false;
            }
            return true;
          }
          return false;
        }

        if (!(key in lastValue) || !deepEqual(lastValue[key], processedValue)) {
          if (!propertyValues[property]) {
            propertyValues[property] = {};
          }
          propertyValues[property][
            `0x${cp.toString(16).toUpperCase().padStart(4, "0")}`
          ] = processedValue;
          lastValue[key] = processedValue;
        }

        dots++;
        if (dots % 1000 === 0) {
          process.stdout.write(".");
        }
      }
    }

    console.log("");
  }

  console.log("Done parsing");
  return { propertyValues, lastCp };
}

// Main execution
try {
  const propertiesData = parsePropertyValueAliases();
  const { propertyValues, lastCp } = parseXMLDatabase(propertiesData);

  // Save property values as JSON
  console.log("Saving data...");
  const data = {
    propertyValues,
    lastCp,
    version,
  };

  writeFileSync(datafile, JSON.stringify(data, null, 2));
  console.log(" Done");
} catch (error) {
  console.error("Error during processing:", error.message);
  process.exit(1);
}
