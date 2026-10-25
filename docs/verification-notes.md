# Preservation and verification notes

Prepared on 25 October 2026. Original project files were read without modifying the source folder.

## What was checked

- Opened all 28 native `.cas.h5` files with an HDF5 reader and recorded version, cell-zone extent, model flags, reference velocity, and reference length in `case-inventory.csv`.
- Confirmed that source case files use Fluent version 26.1 and contain an obstacle cell zone plus three fluid zones.
- Inspected the original PDF reports, LaTeX source, inlet UDF sources, file layout, and plotting scripts.
- Checked local links, archive integrity, source-file checksums, and Python-script syntax. No Fluent solution was rerun, no UDF was compiled, and no original result figure was regenerated during packaging.

## Discrepancies to resolve before claiming validation

1. **Viscous model:** saved Rampant variables have `rp-lam? #f` and `rp-kw? #t`, while the report describes a laminar simulation. Verify the selected model in Fluent and establish which settings generated each reported result. The package preserves the saved settings rather than editing binary case files to match the narrative.
2. **Reynolds-number convention:** UDF code defines Re using H; many case-specific constants are half the folder label. For example, label 1000 uses `RE_H=500`, while the report uses a hydraulic diameter of 2H. The generic template differs from several case-specific copies. Use the source and saved fields appropriate to each case, and reconcile the benchmark convention before comparing curves.
3. **Historical UDF provenance:** source was not located beside three cases: Re200 at ratios 1 and 100, and Re2000 at ratio 100. Available sources are retained, including `libudf/src` copies. No replacement UDF has been invented for missing sources. Even a matching source file does not prove that it produced an existing compiled library or solution.
4. **Nusselt computation:** `compute_nusselt` has an empty body. Its comments are not evidence of executed post-processing. Confirm reference length, temperature, conductivity, heat flux, and face averaging before relying on surface Nusselt exports.
5. **Grid-independence claim:** a standard case with maximum cell ID 115,933 and a finer case with maximum cell ID 253,687 are present. Their existence does not establish the report's claimed three-grid study or less-than-1% difference. No independently audited grid-comparison table was produced here.
6. **Validation percentage:** the original report's 1.15% mean error is retained as a report claim, not adopted as an independently verified result. The reference correlation, Reynolds convention, exported averages, and spreadsheet calculations need to be reconciled first.
7. **Different report scopes:** the presentation describes Re1000 at conductivity ratio 1; the later report describes a ratio-10 sweep. The presentation also contains a stray drag-coefficient objective. Both PDFs remain unchanged as historical source documents.
8. **Draft LaTeX:** preserved as a source archive; it contains unresolved figure paths/placeholders and is not claimed to reproduce the final report PDF.
9. **Mislabelled image:** `contour_plot/streamline plot and isotherms_200/isotherms_Re1000.png` is retained under `results/figures/Re200/` with its original filename. Confirm its actual Reynolds number before reusing it independently.

## Packaging choices

All 28 case inputs are retained, grouped by their original Reynolds-number folder in `native/Re*.zip`. Case files include their mesh, so duplicate standalone mesh exports and incomplete Workbench project-cache material were excluded. Native SpaceClaim geometry is retained separately.

One original solution data file is included: Re1000, `ks_by_kf=10/spaceclaim_geometry.dat.h5`. Other generated `.dat.h5` solutions remain in the untouched source folder and are excluded to keep the public repository manageable. The case inventory distinguishes source data-pair availability from inclusion in the archive.

Original CSV field exports and their scripts are retained for all five supplied contour folders. The workbooks, selected figures, reports, small surface exports, and transcripts are retained. Compiled UDF DLLs, executable build tools, object files, build caches, cleanup scripts, and temporary session files are excluded.

`source-manifest.json` lists each retained source file, its archive location or repository path, size, and SHA-256 digest. No simulation inputs or source documents were numerically altered during packaging.
