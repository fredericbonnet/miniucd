/**
 * @file coatlUcd_propertyTypes.js
 *
 * Unicode Character Database property types and classifications.
 * Converted from coatlUcd_propertyTypes.tcl
 *
 * Adapted from PropertyAliases-16.0.0.txt and UAX #44
 */

export const properties = {
  // ================================================
  // Numeric Properties
  // ================================================
  // TODO UAX #38
  // cjkAccountingNumeric     ; kAccountingNumeric
  // cjkOtherNumeric          ; kOtherNumeric
  // cjkPrimaryNumeric        ; kPrimaryNumeric
  nv: { type: "numeric" },

  // ================================================
  // String Properties
  // ================================================
  bmg: { type: "cp" },
  bpb: { type: "cp" },
  cf: { type: "mcp" },
  // TODO UAX #38
  // cjkCompatibilityVariant  ; kCompatibilityVariant
  dm: { type: "mcp" },
  EqUIdeo: { type: "cp" },
  FC_NFKC: { type: "deprecated" },
  lc: { type: "mcp" },
  NFKC_CF: { type: "mcp" },
  NFKC_SCF: { type: "mcp" },
  scf: { type: "cp" },
  slc: { type: "cp" },
  stc: { type: "cp" },
  suc: { type: "cp" },
  tc: { type: "mcp" },
  uc: { type: "mcp" },

  // ================================================
  // Miscellaneous Properties
  // ================================================
  isc: { type: "deprecated" },
  JSN: { type: "name" },
  na: { type: "name" },
  na1: { type: "name" },
  Name_Alias: { type: "name" },
  scx: { type: "enumList", enumType: "sc" },
  // TODO UAX #38
  //    cjkIICore                ; kIICore
  //    cjkIRG_GSource           ; kIRG_GSource
  //    cjkIRG_HSource           ; kIRG_HSource
  //    cjkIRG_JSource           ; kIRG_JSource
  //    cjkIRG_KPSource          ; kIRG_KPSource
  //    cjkIRG_KSource           ; kIRG_KSource
  //    cjkIRG_MSource           ; kIRG_MSource
  //    cjkIRG_TSource           ; kIRG_TSource
  //    cjkIRG_USource           ; kIRG_USource
  //    cjkIRG_VSource           ; kIRG_VSource
  //    cjkRSUnicode             ; kRSUnicode                  ; Unicode_Radical_Stroke; URS
  // TODO UAX #57
  //    kEH_Cat                  ; kEH_Cat
  //    kEH_Desc                 ; kEH_Desc
  //    kEH_HG                   ; kEH_HG
  //    kEH_IFAO                 ; kEH_IFAO
  //    kEH_JSesh                ; kEH_JSesh

  // ================================================
  // Catalog Properties
  // ================================================
  age: { type: "enum" },
  blk: { type: "enum" },
  sc: { type: "enum" },

  // ================================================
  // Enumerated Properties
  // ================================================
  bc: { type: "enum" },
  bpt: { type: "enum" },
  ccc: { type: "enum" },
  dt: { type: "enum" },
  ea: { type: "enum" },
  gc: { type: "enum" },
  GCB: { type: "enum" },
  hst: { type: "enum" },
  InCB: { type: "enum" },
  InPC: { type: "enum" },
  InSC: { type: "enum" },
  jg: { type: "enum" },
  jt: { type: "enum" },
  lb: { type: "enum" },
  NFC_QC: { type: "enum" },
  NFD_QC: { type: "enum" },
  NFKC_QC: { type: "enum" },
  NFKD_QC: { type: "enum" },
  nt: { type: "enum" },
  SB: { type: "enum" },
  vo: { type: "enum" },
  WB: { type: "enum" },

  // ================================================
  // Binary Properties
  // ================================================
  AHex: { type: "boolean" },
  Alpha: { type: "boolean" },
  Bidi_C: { type: "boolean" },
  Bidi_M: { type: "boolean" },
  Cased: { type: "boolean" },
  CE: { type: "boolean" },
  CI: { type: "boolean" },
  Comp_Ex: { type: "boolean" },
  CWCF: { type: "boolean" },
  CWCM: { type: "boolean" },
  CWKCF: { type: "boolean" },
  CWL: { type: "boolean" },
  CWT: { type: "boolean" },
  CWU: { type: "boolean" },
  Dash: { type: "boolean" },
  Dep: { type: "boolean" },
  DI: { type: "boolean" },
  Dia: { type: "boolean" },
  EBase: { type: "boolean" },
  EComp: { type: "boolean" },
  EMod: { type: "boolean" },
  Emoji: { type: "boolean" },
  EPres: { type: "boolean" },
  Ext: { type: "boolean" },
  ExtPict: { type: "boolean" },
  Gr_Base: { type: "boolean" },
  Gr_Ext: { type: "boolean" },
  Gr_Link: { type: "deprecated" },
  Hex: { type: "boolean" },
  Hyphen: { type: "deprecated" },
  ID_Compat_Math_Continue: { type: "boolean" },
  ID_Compat_Math_Start: { type: "boolean" },
  IDC: { type: "boolean" },
  Ideo: { type: "boolean" },
  IDS: { type: "boolean" },
  IDSB: { type: "boolean" },
  IDST: { type: "boolean" },
  IDSU: { type: "boolean" },
  Join_C: { type: "boolean" },
  // TODO UAX #57
  //    kEH_NoMirror                  ; kEH_Cat
  //    kEH_NoRotate                  ; kEH_Cat
  LOE: { type: "boolean" },
  Lower: { type: "boolean" },
  Math: { type: "boolean" },
  MCM: { type: "boolean" },
  NChar: { type: "boolean" },
  OAlpha: { type: "boolean" },
  ODI: { type: "boolean" },
  OGr_Ext: { type: "boolean" },
  OIDC: { type: "boolean" },
  OIDS: { type: "boolean" },
  OLower: { type: "boolean" },
  OMath: { type: "boolean" },
  OUpper: { type: "boolean" },
  Pat_Syn: { type: "boolean" },
  Pat_WS: { type: "boolean" },
  PCM: { type: "boolean" },
  QMark: { type: "boolean" },
  Radical: { type: "boolean" },
  RI: { type: "boolean" },
  SD: { type: "boolean" },
  STerm: { type: "boolean" },
  Term: { type: "boolean" },
  UIdeo: { type: "boolean" },
  Upper: { type: "boolean" },
  VS: { type: "boolean" },
  WSpace: { type: "boolean" },
  XIDC: { type: "boolean" },
  XIDS: { type: "boolean" },
  XO_NFC: { type: "deprecated" },
  XO_NFD: { type: "deprecated" },
  XO_NFKC: { type: "deprecated" },
  XO_NFKD: { type: "deprecated" },
};

// Total: 142
