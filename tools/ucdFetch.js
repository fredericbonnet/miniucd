import { version , ucdroot} from "./config.js";
import { createWriteStream, mkdirSync, existsSync } from 'fs';
import { join } from 'path';
import { pipeline } from 'stream';
import { promisify } from 'util';
import AdmZip from 'adm-zip';

const pipelineAsync = promisify(pipeline);

const ucdxml = `https://www.unicode.org/Public/${version}/ucdxml/ucd.all.grouped.zip`;
const PropertyAliases = `https://www.unicode.org/Public/${version}/ucd/PropertyAliases.txt`;
const PropertyValueAliases = `https://www.unicode.org/Public/${version}/ucd/PropertyValueAliases.txt`;

// Ensure directories exist
function ensureDir(dirPath) {
    if (!existsSync(dirPath)) {
        mkdirSync(dirPath, { recursive: true });
        console.log(`Created directory: ${dirPath}`);
    }
}

// Download a file from URL to local path
async function downloadFile(url, localPath) {
    console.log(`Downloading ${url}...`);
    
    try {
        const response = await fetch(url);
        
        if (!response.ok) {
            throw new Error(`Failed to download ${url}: ${response.status} ${response.statusText}`);
        }
        
        const fileStream = createWriteStream(localPath);
        await pipelineAsync(response.body, fileStream);
        
        console.log(`✓ Downloaded: ${localPath}`);
    } catch (error) {
        throw new Error(`Error downloading ${url}: ${error.message}`);
    }
}

// Extract ZIP file using AdmZip
function extractZip(zipPath, extractPath) {
    try {
        console.log(`Extracting ${zipPath} to ${extractPath}...`);
        const zip = new AdmZip(zipPath);
        zip.extractAllTo(extractPath, /*overwrite*/ true);
        console.log(`✓ Extracted: ${zipPath}`);
    } catch (error) {
        console.error(`Failed to extract ${zipPath}:`, error.message);
        throw error;
    }
}

async function main() {
    try {
        console.log("Fetching UCD data files...");
        
        // Create directories
        ensureDir(ucdroot);
        
        // Download text files to ucdroot
        await downloadFile(PropertyAliases, join(ucdroot, 'PropertyAliases.txt'));
        await downloadFile(PropertyValueAliases, join(ucdroot, 'PropertyValueAliases.txt'));
        
        // Download ZIP file
        const zipPath = join(ucdroot, 'ucd.all.grouped.zip');
        await downloadFile(ucdxml, zipPath);
        
        // Extract ZIP file
        extractZip(zipPath, ucdroot);
        
        console.log("✓ All UCD data files fetched successfully!");
        
    } catch (error) {
        console.error("Error fetching UCD data files:", error.message);
        process.exit(1);
    }
}

main();
