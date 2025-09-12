#include <picotest.h>
#include <stdint.h>

#define MINIUCD_IMPLEMENTATION
#define MiniUCD_Char uint32_t
#include <miniucd.h>

#include "hooks.h"

/* UCD Property Accessors */
PICOTEST_SUITE(testUcdPropertyAccessors, testUcdNumericProperties,
               testUcdStringProperties, testUcdMiscellaneousProperties,
               testUcdCatalogProperties, testUcdEnumeratedProperties,
               testUcdBinaryProperties);

static void checkNumericProperty(const char *(*proc)(uint32_t), uint32_t c,
                                 const char *value) {
    PICOTEST_ASSERT(strcmp(proc(c), value) == 0);
}
static void checkMcpPropertyIdentity(const int *(*proc)(uint32_t, size_t *),
                                     uint32_t c) {
    size_t length;
    const int *list = proc(c, &length);
    PICOTEST_ASSERT(length == 0);
}
static void checkMcpPropertySingle(const int *(*proc)(uint32_t, size_t *),
                                   uint32_t c, uint32_t e) {
    size_t length;
    const int *list = proc(c, &length);
    PICOTEST_ASSERT(length == 1);
    PICOTEST_ASSERT(e == c + *list);
}
static void checkMcpPropertyMultiple(const int *(*proc)(uint32_t, size_t *),
                                     uint32_t c, wchar_t *e) {
    size_t length;
    const int *list = proc(c, &length);
    PICOTEST_ASSERT(length > 1);
    for (int i = 0; i < length; i++) {
        PICOTEST_ASSERT(e[i] == list[i]);
    }
}
static void checkCpProperty(uint32_t (*proc)(uint32_t), uint32_t c,
                            uint32_t e) {
    PICOTEST_ASSERT(proc(c) == e);
}
static void checkEnumListPropertySingle(const unsigned char *(*proc)(uint32_t,
                                                                     size_t *),
                                        uint32_t c, int value) {
    size_t length;
    const unsigned char *list = proc(c, &length);
    PICOTEST_ASSERT(length == 1);
    PICOTEST_ASSERT(*list == value);
}
static void
checkEnumListPropertyMultiple(const unsigned char *(*proc)(uint32_t, size_t *),
                              uint32_t c, ...) {
    va_list values;
    size_t nb;
    size_t length, i, j;
    const unsigned char *list = proc(c, &length);
    va_start(values, c);
    for (nb = 0; va_arg(values, int); nb++)
        ;
    va_end(values);
    PICOTEST_ASSERT(length == nb);
    for (i = 0; i < length; i++) {
        va_start(values, c);
        for (j = 0; j < length; j++) {
            if (va_arg(values, int) == list[i])
                break;
        }
        va_end(values);
        PICOTEST_ASSERT(j != length);
    }
}

PICOTEST_SUITE(testUcdNumericProperties, testUcdPropertyNv);
PICOTEST_CASE(testUcdPropertyNv) {
    checkNumericProperty(MiniUCD_GetProperty_Nv, '$', "NaN");
    checkNumericProperty(MiniUCD_GetProperty_Nv, '0', "0");
    checkNumericProperty(MiniUCD_GetProperty_Nv, '1', "1");
    checkNumericProperty(MiniUCD_GetProperty_Nv, '9', "9");
    checkNumericProperty(MiniUCD_GetProperty_Nv, 0xBD, "1/2");    /* ½ */
    checkNumericProperty(MiniUCD_GetProperty_Nv, 0x0D5C, "1/10"); /* ൜ */
    checkNumericProperty(MiniUCD_GetProperty_Nv, 'a', "NaN");
}

PICOTEST_SUITE(testUcdStringProperties, testUcdPropertyCf, testUcdPropertyDm,
               testUcdPropertyLc, testUcdPropertyNFKC_CF, testUcdPropertyScf,
               testUcdPropertySlc, testUcdPropertyStc, testUcdPropertySuc,
               testUcdPropertyTc, testUcdPropertyUc);
PICOTEST_CASE(testUcdPropertyCf) {
    checkMcpPropertyIdentity(MiniUCD_GetProperty_Cf, '0');
    checkMcpPropertySingle(MiniUCD_GetProperty_Cf, 'A', 'a');
    checkMcpPropertyIdentity(MiniUCD_GetProperty_Cf, 'b');
    checkMcpPropertySingle(MiniUCD_GetProperty_Cf, 0xB5, 0x03BC);  /* µ -> μ */
    checkMcpPropertyMultiple(MiniUCD_GetProperty_Cf, 0xDF, L"ss"); /* ß -> ss */
    checkMcpPropertyMultiple(MiniUCD_GetProperty_Cf, 0xFB04,
                             L"ffl"); /* ﬄ -> ffl */
    checkMcpPropertySingle(MiniUCD_GetProperty_Cf, 0x0533, 0x0563); /* Գ -> գ */
    checkMcpPropertyMultiple(MiniUCD_GetProperty_Cf, 0x1FFC,
                             L"\u03C9\u03B9"); /* ῼ -> ωι */
}
PICOTEST_CASE(testUcdPropertyDm) {
    checkMcpPropertyIdentity(MiniUCD_GetProperty_Dm, '$');
    checkMcpPropertyIdentity(MiniUCD_GetProperty_Dm, 'A');
    checkMcpPropertyIdentity(MiniUCD_GetProperty_Dm, 'b');
    checkMcpPropertySingle(MiniUCD_GetProperty_Dm, 0xB5, 0x03BC); /* µ -> μ */
    checkMcpPropertySingle(MiniUCD_GetProperty_Dm, 0x2460, '1');  /* ① -> 1 */
    checkMcpPropertyMultiple(MiniUCD_GetProperty_Dm, 0xFB03,
                             L"ffi");                         /* ﬃ -> ffi */
    checkMcpPropertyIdentity(MiniUCD_GetProperty_Dm, 0x0533); /* Գ */
    checkMcpPropertyMultiple(MiniUCD_GetProperty_Dm, 0x1FFC,
                             L"\u03A9\u0345"); /* ῼ -> Ω◌ͅ */
}
PICOTEST_CASE(testUcdPropertyLc) {
    checkMcpPropertyIdentity(MiniUCD_GetProperty_Lc, '&');
    checkMcpPropertyIdentity(MiniUCD_GetProperty_Lc, 'a');
    checkMcpPropertySingle(MiniUCD_GetProperty_Lc, 'B', 'b');
    checkMcpPropertySingle(MiniUCD_GetProperty_Lc, 0x03A3, 0x03C3); /* Σ -> σ */
    checkMcpPropertyMultiple(MiniUCD_GetProperty_Lc, 0x0130,
                             L"i\u0307"); /* İ -> i◌̇  */
}
PICOTEST_CASE(testUcdPropertyNFKC_CF) {
    checkMcpPropertyIdentity(MiniUCD_GetProperty_NFKC_CF, 'c');
    checkMcpPropertySingle(MiniUCD_GetProperty_NFKC_CF, 'D', 'd');
    checkMcpPropertyMultiple(MiniUCD_GetProperty_NFKC_CF, 0xBC,
                             L"1\u20444"); /* ¼ -> 1⁄4 */
}
PICOTEST_CASE(testUcdPropertyScf) {
    checkCpProperty(MiniUCD_GetProperty_Scf, 'E', 'e');
    checkCpProperty(MiniUCD_GetProperty_Scf, 'f', 'f');
    checkCpProperty(MiniUCD_GetProperty_Scf, 0xB5, 0x03BC);   /* µ -> μ */
    checkCpProperty(MiniUCD_GetProperty_Scf, 0xDF, 0xDF);     /* ß */
    checkCpProperty(MiniUCD_GetProperty_Scf, 0xFB04, 0xFB04); /* ﬄ */
}
PICOTEST_CASE(testUcdPropertySlc) {
    checkCpProperty(MiniUCD_GetProperty_Slc, 'g', 'g');
    checkCpProperty(MiniUCD_GetProperty_Slc, 'H', 'h');
    checkCpProperty(MiniUCD_GetProperty_Slc, 0xC0, 0xE0);     /* À -> à */
    checkCpProperty(MiniUCD_GetProperty_Slc, 0xDF, 0xDF);     /* ß */
    checkCpProperty(MiniUCD_GetProperty_Slc, 0x0104, 0x0105); /* Ą -> ą */
    checkCpProperty(MiniUCD_GetProperty_Slc, 0x0240, 0x0240); /* ɀ */
    checkCpProperty(MiniUCD_GetProperty_Slc, 0x03AB, 0x03CB); /* Ϋ -> ϋ */
    checkCpProperty(MiniUCD_GetProperty_Slc, 0x2CF2, 0x2CF3); /* Ⳳ -> ⳳ */
}
PICOTEST_CASE(testUcdPropertyStc) {
    checkCpProperty(MiniUCD_GetProperty_Stc, 'i', 'I');
    checkCpProperty(MiniUCD_GetProperty_Stc, 'J', 'J');
    checkCpProperty(MiniUCD_GetProperty_Stc, 0xB5, 0x039C);   /* µ -> Μ */
    checkCpProperty(MiniUCD_GetProperty_Stc, 0xDF, 0xDF);     /* ß */
    checkCpProperty(MiniUCD_GetProperty_Stc, 0x0133, 0x0132); /* ĳ -> Ĳ */
    checkCpProperty(MiniUCD_GetProperty_Stc, 0x01C7, 0x01C8); /* Ǉ -> ǈ */
    checkCpProperty(MiniUCD_GetProperty_Stc, 0x01C9, 0x01C8); /* ǉ -> ǈ */
}
PICOTEST_CASE(testUcdPropertySuc) {
    checkCpProperty(MiniUCD_GetProperty_Suc, 'k', 'K');
    checkCpProperty(MiniUCD_GetProperty_Suc, 'l', 'L');
    checkCpProperty(MiniUCD_GetProperty_Suc, 0xDF, 0xDF);     /* ß */
    checkCpProperty(MiniUCD_GetProperty_Suc, 0x03BC, 0x039C); /* μ -> Μ */
    checkCpProperty(MiniUCD_GetProperty_Suc, 0x0133, 0x0132); /* ĳ -> Ĳ */
    checkCpProperty(MiniUCD_GetProperty_Suc, 0x01C9, 0x01C7); /* ǉ -> Ǉ */
    checkCpProperty(MiniUCD_GetProperty_Suc, 0x01C8, 0x01C7); /* ǈ -> Ǉ */
}
PICOTEST_CASE(testUcdPropertyTc) {
    checkMcpPropertySingle(MiniUCD_GetProperty_Tc, 'm', 'M');
    checkMcpPropertyIdentity(MiniUCD_GetProperty_Tc, 'N');
    checkMcpPropertyMultiple(MiniUCD_GetProperty_Tc, 0xDF, L"Ss"); /* ß -> Ss */
    checkMcpPropertyMultiple(MiniUCD_GetProperty_Tc, 0x01F0,
                             L"J\u030C"); /* ǰ -> J◌̌  */
    checkMcpPropertySingle(MiniUCD_GetProperty_Tc, 0x03BC, 0x039C); /* μ -> Μ */
    checkMcpPropertySingle(MiniUCD_GetProperty_Tc, 0x0133, 0x0132); /* ĳ -> Ĳ */
    checkMcpPropertySingle(MiniUCD_GetProperty_Tc, 0x01C9, 0x01C8); /* ǉ -> ǈ */
    checkMcpPropertyIdentity(MiniUCD_GetProperty_Tc, 0x01C8);       /* ǈ */
}
PICOTEST_CASE(testUcdPropertyUc) {
    checkMcpPropertyIdentity(MiniUCD_GetProperty_Uc, 'M');
    checkMcpPropertySingle(MiniUCD_GetProperty_Uc, 'n', 'N');
    checkMcpPropertyMultiple(MiniUCD_GetProperty_Uc, 0xDF, L"SS"); /* ß -> SS */
    checkMcpPropertySingle(MiniUCD_GetProperty_Uc, 0x03BC, 0x039C); /* μ -> Μ */
    checkMcpPropertySingle(MiniUCD_GetProperty_Uc, 0x0133, 0x0132); /* ĳ -> Ĳ */
    checkMcpPropertySingle(MiniUCD_GetProperty_Uc, 0x01C9, 0x01C7); /* ǉ -> Ǉ */
    checkMcpPropertySingle(MiniUCD_GetProperty_Uc, 0x01C8, 0x01C7); /* ǈ -> Ǉ */
    checkMcpPropertySingle(MiniUCD_GetProperty_Uc, 0x01C8, 0x01C7); /* ǈ -> Ǉ */
}

PICOTEST_SUITE(testUcdMiscellaneousProperties, testUcdPropertyBmg,
               testUcdPropertyBpb, testUcdPropertyScx);
PICOTEST_CASE(testUcdPropertyBmg) {
    checkCpProperty(MiniUCD_GetProperty_Bmg, 'A', 'A');
    checkCpProperty(MiniUCD_GetProperty_Bmg, '(', ')');
    checkCpProperty(MiniUCD_GetProperty_Bmg, ')', '(');
    checkCpProperty(MiniUCD_GetProperty_Bmg, 0xAB, 0xBB);     /* « -> » */
    checkCpProperty(MiniUCD_GetProperty_Bmg, 0x2208, 0x220B); /* ∈ -> ∋ */
}
PICOTEST_CASE(testUcdPropertyBpb) {
    checkCpProperty(MiniUCD_GetProperty_Bpb, 'A', 'A');
    checkCpProperty(MiniUCD_GetProperty_Bpb, '(', ')');
    checkCpProperty(MiniUCD_GetProperty_Bpb, ')', '(');
    checkCpProperty(MiniUCD_GetProperty_Bpb, 0xAB, 0xAB);     /* « */
    checkCpProperty(MiniUCD_GetProperty_Bpb, 0x0F3A, 0x0F3B); /* ༺ -> ༻ */
}
PICOTEST_CASE(testUcdPropertyScx) {
    checkEnumListPropertySingle(MiniUCD_GetProperty_Scx, '\t',
                                MINIUCD_SC_COMMON);
    checkEnumListPropertySingle(MiniUCD_GetProperty_Scx, '!',
                                MINIUCD_SC_COMMON);
    checkEnumListPropertySingle(MiniUCD_GetProperty_Scx, 'A', MINIUCD_SC_LATIN);
    checkEnumListPropertySingle(MiniUCD_GetProperty_Scx, 0xB5,
                                MINIUCD_SC_COMMON); /* µ */
    checkEnumListPropertySingle(MiniUCD_GetProperty_Scx, 0x03BC,
                                MINIUCD_SC_GREEK); /* μ */
    checkEnumListPropertySingle(MiniUCD_GetProperty_Scx, 0x0F3A,
                                MINIUCD_SC_TIBETAN); /* ༺ */
    checkEnumListPropertyMultiple(MiniUCD_GetProperty_Scx, 0x0663,
                                  MINIUCD_SC_ARABIC, MINIUCD_SC_YEZIDI,
                                  MINIUCD_SC_THAANA, NULL); /* ٣ */
}

PICOTEST_SUITE(testUcdCatalogProperties, testUcdPropertyAge, testUcdPropertyBlk,
               testUcdPropertySc);
PICOTEST_CASE(testUcdPropertyAge) {
    PICOTEST_VERIFY(MiniUCD_GetProperty_Age('A') == MINIUCD_AGE_V1_1);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Age(0x0220) == MINIUCD_AGE_V3_2);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Age(0x0221) == MINIUCD_AGE_V4_0);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Age(0x08BE) == MINIUCD_AGE_V13_0);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Age(0x0378) == MINIUCD_AGE_UNASSIGNED);
}
PICOTEST_CASE(testUcdPropertyBlk) {
    PICOTEST_VERIFY(MiniUCD_GetProperty_Blk('A') == MINIUCD_BLK_ASCII);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Blk(' ') == MINIUCD_BLK_ASCII);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Blk(0xE9) ==
                    MINIUCD_BLK_LATIN_1); /* é */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Blk(0xE9) ==
                    MINIUCD_BLK_LATIN_1); /* é */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Blk(0x0628) ==
                    MINIUCD_BLK_ARABIC); /* ب */
}
PICOTEST_CASE(testUcdPropertySc) {
    PICOTEST_VERIFY(MiniUCD_GetProperty_Sc('\t') == MINIUCD_SC_COMMON);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Sc('!') == MINIUCD_SC_COMMON);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Sc('A') == MINIUCD_SC_LATIN);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Sc(0xB5) == MINIUCD_SC_COMMON);  /* µ */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Sc(0x03BC) == MINIUCD_SC_GREEK); /* μ */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Sc(0x0F3A) ==
                    MINIUCD_SC_TIBETAN); /* ༺ */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Sc(0x0663) ==
                    MINIUCD_SC_ARABIC); /* ٣ */
}

PICOTEST_SUITE(testUcdEnumeratedProperties, testUcdPropertyBc,
               testUcdPropertyBpt, testUcdPropertyCcc, testUcdPropertyDt,
               testUcdPropertyEa, testUcdPropertyGc, testUcdPropertyGCB,
               testUcdPropertyHst, testUcdPropertyInPC, testUcdPropertyInSC,
               testUcdPropertyJg, testUcdPropertyJt, testUcdPropertyLb,
               testUcdPropertyNFC_QC, testUcdPropertyNFD_QC,
               testUcdPropertyNFKC_QC, testUcdPropertyNFKD_QC,
               testUcdPropertyNt, testUcdPropertySB, testUcdPropertyVo,
               testUcdPropertyWB);
PICOTEST_CASE(testUcdPropertyBc) {
    PICOTEST_VERIFY(MiniUCD_GetProperty_Bc(' ') == MINIUCD_BC_WHITE_SPACE);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Bc('A') == MINIUCD_BC_LEFT_TO_RIGHT);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Bc(0x0629) ==
                    MINIUCD_BC_ARABIC_LETTER); /* ة */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Bc(0x0663) ==
                    MINIUCD_BC_ARABIC_NUMBER); /* ٣ */
}
PICOTEST_CASE(testUcdPropertyBpt) {
    PICOTEST_VERIFY(MiniUCD_GetProperty_Bpt('A') == MINIUCD_BPT_NONE);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Bpt('(') == MINIUCD_BPT_OPEN);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Bpt(')') == MINIUCD_BPT_CLOSE);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Bpt(0x169B) ==
                    MINIUCD_BPT_OPEN); /* ᚛ */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Bpt(0x169C) ==
                    MINIUCD_BPT_CLOSE); /* ᚜ */
}
PICOTEST_CASE(testUcdPropertyCcc) {
    PICOTEST_VERIFY(MiniUCD_GetProperty_Ccc('A') == MINIUCD_CCC_0);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Ccc(0x0303) ==
                    MINIUCD_CCC_ABOVE); /* ◌̃  */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Ccc(0x0334) ==
                    MINIUCD_CCC_OVERLAY); /* ◌̴  */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Ccc(0x0327) ==
                    MINIUCD_CCC_ATTACHED_BELOW); /* ◌̧  */
}
PICOTEST_CASE(testUcdPropertyDt) {
    PICOTEST_VERIFY(MiniUCD_GetProperty_Dt(' ') == MINIUCD_DT_NONE);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Dt('A') == MINIUCD_DT_NONE);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Dt(0xA0) == MINIUCD_DT_NOBREAK);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Dt(0xCF) ==
                    MINIUCD_DT_CANONICAL); /* Ï */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Dt(0xBD) ==
                    MINIUCD_DT_FRACTION); /* ½ */
}
PICOTEST_CASE(testUcdPropertyEa) {
    PICOTEST_VERIFY(MiniUCD_GetProperty_Ea('\t') == MINIUCD_EA_NEUTRAL);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Ea(' ') == MINIUCD_EA_NARROW);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Ea('A') == MINIUCD_EA_NARROW);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Ea(0x111A) == MINIUCD_EA_WIDE); /* ᄚ */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Ea(0xFFE6) ==
                    MINIUCD_EA_FULLWIDTH); /* ￦ */
}
PICOTEST_CASE(testUcdPropertyGc) {
    PICOTEST_VERIFY(MiniUCD_GetProperty_Gc('\t') == MINIUCD_GC_CONTROL);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Gc(' ') == MINIUCD_GC_SPACE_SEPARATOR);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Gc('!') ==
                    MINIUCD_GC_OTHER_PUNCTUATION);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Gc('A') == MINIUCD_GC_UPPERCASE_LETTER);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Gc('b') == MINIUCD_GC_LOWERCASE_LETTER);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Gc(0x01C8) ==
                    MINIUCD_GC_TITLECASE_LETTER); /* ǈ */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Gc(0x0378) == MINIUCD_GC_UNASSIGNED);
}
PICOTEST_CASE(testUcdPropertyGCB) {
    PICOTEST_VERIFY(MiniUCD_GetProperty_GCB('\b') == MINIUCD_GCB_CONTROL);
    PICOTEST_VERIFY(MiniUCD_GetProperty_GCB('\n') == MINIUCD_GCB_LF);
    PICOTEST_VERIFY(MiniUCD_GetProperty_GCB('\r') == MINIUCD_GCB_CR);
    PICOTEST_VERIFY(MiniUCD_GetProperty_GCB(0x0300) ==
                    MINIUCD_GCB_EXTEND); /* ◌̀  */
}
PICOTEST_CASE(testUcdPropertyHst) {
    PICOTEST_VERIFY(MiniUCD_GetProperty_Hst('A') == MINIUCD_HST_NOT_APPLICABLE);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Hst(0x1163) ==
                    MINIUCD_HST_VOWEL_JAMO); /* ᅣ */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Hst(0x11AD) ==
                    MINIUCD_HST_TRAILING_JAMO); /* ᆭ */
}
PICOTEST_CASE(testUcdPropertyInPC) {
    PICOTEST_VERIFY(MiniUCD_GetProperty_InPC('A') == MINIUCD_INPC_NA);
    PICOTEST_VERIFY(MiniUCD_GetProperty_InPC(0x0900) ==
                    MINIUCD_INPC_TOP); /* ◌ऀ */
    PICOTEST_VERIFY(MiniUCD_GetProperty_InPC(0x10A01) ==
                    MINIUCD_INPC_OVERSTRUCK); /* ◌𐨁 */
}
PICOTEST_CASE(testUcdPropertyInSC) {
    PICOTEST_VERIFY(MiniUCD_GetProperty_InSC('A') == MINIUCD_INSC_OTHER);
    PICOTEST_VERIFY(MiniUCD_GetProperty_InSC('3') == MINIUCD_INSC_NUMBER);
    PICOTEST_VERIFY(MiniUCD_GetProperty_InSC(0xA9D9) ==
                    MINIUCD_INSC_NUMBER); /* ꧙ */
    PICOTEST_VERIFY(MiniUCD_GetProperty_InSC('-') ==
                    MINIUCD_INSC_CONSONANT_PLACEHOLDER);
    PICOTEST_VERIFY(MiniUCD_GetProperty_InSC(0x0905) ==
                    MINIUCD_INSC_VOWEL_INDEPENDENT); /* अ */
}
PICOTEST_CASE(testUcdPropertyJg) {
    PICOTEST_VERIFY(MiniUCD_GetProperty_Jg('A') == MINIUCD_JG_NO_JOINING_GROUP);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Jg(0x08AC) ==
                    MINIUCD_JG_ROHINGYA_YEH); /* ࢬ */
}
PICOTEST_CASE(testUcdPropertyJt) {
    PICOTEST_VERIFY(MiniUCD_GetProperty_Jt('A') == MINIUCD_JT_NON_JOINING);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Jt(0xAD) ==
                    MINIUCD_JT_TRANSPARENT); /* SOFT HYPHEN*/
    PICOTEST_VERIFY(MiniUCD_GetProperty_Jt(0x0629) ==
                    MINIUCD_JT_RIGHT_JOINING); /* ة */
}
PICOTEST_CASE(testUcdPropertyLb) {
    PICOTEST_VERIFY(MiniUCD_GetProperty_Lb('\t') == MINIUCD_LB_BREAK_AFTER);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Lb('\n') == MINIUCD_LB_LINE_FEED);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Lb('\f') == MINIUCD_LB_MANDATORY_BREAK);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Lb('A') == MINIUCD_LB_ALPHABETIC);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Lb(' ') == MINIUCD_LB_SPACE);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Lb(']') ==
                    MINIUCD_LB_CLOSE_PARENTHESIS);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Lb(0x05D0) ==
                    MINIUCD_LB_HEBREW_LETTER); /* א */
}
PICOTEST_CASE(testUcdPropertyNFC_QC) {
    PICOTEST_VERIFY(MiniUCD_GetProperty_NFC_QC('A') == MINIUCD_NFC_QC_YES);
    PICOTEST_VERIFY(MiniUCD_GetProperty_NFC_QC(0x0958) ==
                    MINIUCD_NFC_QC_NO); /* क़ */
    PICOTEST_VERIFY(MiniUCD_GetProperty_NFC_QC(0x0302) ==
                    MINIUCD_NFC_QC_MAYBE); /* ◌̂  */
}
PICOTEST_CASE(testUcdPropertyNFD_QC) {
    PICOTEST_VERIFY(MiniUCD_GetProperty_NFD_QC('A') == MINIUCD_NFD_QC_YES);
    PICOTEST_VERIFY(MiniUCD_GetProperty_NFD_QC(0x0958) ==
                    MINIUCD_NFD_QC_NO); /* क़ */
}
PICOTEST_CASE(testUcdPropertyNFKC_QC) {
    PICOTEST_VERIFY(MiniUCD_GetProperty_NFKC_QC('A') == MINIUCD_NFKC_QC_YES);
    PICOTEST_VERIFY(MiniUCD_GetProperty_NFKC_QC(0x0958) ==
                    MINIUCD_NFKC_QC_NO); /* क़ */
    PICOTEST_VERIFY(MiniUCD_GetProperty_NFKC_QC(0x0302) ==
                    MINIUCD_NFKC_QC_MAYBE); /* ◌̂  */
}
PICOTEST_CASE(testUcdPropertyNFKD_QC) {
    PICOTEST_VERIFY(MiniUCD_GetProperty_NFKD_QC('A') == MINIUCD_NFKD_QC_YES);
    PICOTEST_VERIFY(MiniUCD_GetProperty_NFKD_QC(0x0958) ==
                    MINIUCD_NFKD_QC_NO); /* क़ */
}
PICOTEST_CASE(testUcdPropertyNt) {
    PICOTEST_VERIFY(MiniUCD_GetProperty_Nt('A') == MINIUCD_NT_NONE);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Nt('1') == MINIUCD_NT_DECIMAL);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Nt(0xB2) == MINIUCD_NT_DIGIT); /* ² */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Nt(0x0664) ==
                    MINIUCD_NT_DECIMAL);                                 /* ٤ */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Nt(0x136F) == MINIUCD_NT_DIGIT); /* ፯ */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Nt(0x1373) ==
                    MINIUCD_NT_NUMERIC); /* ፳ */
}
PICOTEST_CASE(testUcdPropertySB) {
    PICOTEST_VERIFY(MiniUCD_GetProperty_SB('\t') == MINIUCD_SB_SP);
    PICOTEST_VERIFY(MiniUCD_GetProperty_SB('\'') == MINIUCD_SB_CLOSE);
    PICOTEST_VERIFY(MiniUCD_GetProperty_SB('.') == MINIUCD_SB_ATERM);
    PICOTEST_VERIFY(MiniUCD_GetProperty_SB('!') == MINIUCD_SB_STERM);
    PICOTEST_VERIFY(MiniUCD_GetProperty_SB(0x0700) == MINIUCD_SB_STERM); /* ܀ */
}
PICOTEST_CASE(testUcdPropertyVo) {
    PICOTEST_VERIFY(MiniUCD_GetProperty_Vo('\t') == MINIUCD_VO_ROTATED);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Vo('A') == MINIUCD_VO_ROTATED);
    PICOTEST_VERIFY(MiniUCD_GetProperty_Vo(0xA9) == MINIUCD_VO_UPRIGHT); /* © */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Vo(0x2329) ==
                    MINIUCD_VO_TRANSFORMED_ROTATED); /* 〈 */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Vo(0x3041) ==
                    MINIUCD_VO_TRANSFORMED_UPRIGHT); /* ぁ */
}
PICOTEST_CASE(testUcdPropertyWB) {
    PICOTEST_VERIFY(MiniUCD_GetProperty_WB('\n') == MINIUCD_WB_LF);
    PICOTEST_VERIFY(MiniUCD_GetProperty_WB('\f') == MINIUCD_WB_NEWLINE);
    PICOTEST_VERIFY(MiniUCD_GetProperty_WB(' ') == MINIUCD_WB_WSEGSPACE);
    PICOTEST_VERIFY(MiniUCD_GetProperty_WB('.') == MINIUCD_WB_MIDNUMLET);
    PICOTEST_VERIFY(MiniUCD_GetProperty_WB('@') == MINIUCD_WB_OTHER);
    PICOTEST_VERIFY(MiniUCD_GetProperty_WB('A') == MINIUCD_WB_ALETTER);
    PICOTEST_VERIFY(MiniUCD_GetProperty_WB(0x3032) ==
                    MINIUCD_WB_KATAKANA); /* 〲 */
}

PICOTEST_SUITE(testUcdBinaryProperties, testUcdPropertyAHex,
               testUcdPropertyAlpha, testUcdPropertyBidi_C,
               testUcdPropertyBidi_M, testUcdPropertyCased, testUcdPropertyCE,
               testUcdPropertyCI, testUcdPropertyComp_Ex, testUcdPropertyCWCF,
               testUcdPropertyCWCM, testUcdPropertyCWKCF, testUcdPropertyCWL,
               testUcdPropertyCWT, testUcdPropertyCWU, testUcdPropertyDash,
               testUcdPropertyDep, testUcdPropertyDI, testUcdPropertyDia,
               testUcdPropertyEBase, testUcdPropertyEComp, testUcdPropertyEMod,
               testUcdPropertyEmoji, testUcdPropertyEPres, testUcdPropertyExt,
               testUcdPropertyExtPict, testUcdPropertyGr_Base,
               testUcdPropertyGr_Ext, testUcdPropertyHex, testUcdPropertyIDC,
               testUcdPropertyIdeo, testUcdPropertyIDS, testUcdPropertyIDSB,
               testUcdPropertyIDST, testUcdPropertyJoin_C, testUcdPropertyLOE,
               testUcdPropertyLower, testUcdPropertyMath, testUcdPropertyNChar,
               testUcdPropertyOAlpha, testUcdPropertyODI,
               testUcdPropertyOGr_Ext, testUcdPropertyOIDC, testUcdPropertyOIDS,
               testUcdPropertyOLower, testUcdPropertyOMath,
               testUcdPropertyOUpper, testUcdPropertyPat_Syn,
               testUcdPropertyPat_WS, testUcdPropertyPCM, testUcdPropertyQMark,
               testUcdPropertyRadical, testUcdPropertyRI, testUcdPropertySD,
               testUcdPropertySTerm, testUcdPropertyTerm, testUcdPropertyUIdeo,
               testUcdPropertyUpper, testUcdPropertyVS, testUcdPropertyWSpace,
               testUcdPropertyXIDC, testUcdPropertyXIDS);
PICOTEST_CASE(testUcdPropertyAHex) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_AHex('/'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_AHex('0'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_AHex('9'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_AHex(';'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_AHex('@'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_AHex('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_AHex('F'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_AHex('G'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_AHex('`'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_AHex('a'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_AHex('f'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_AHex('g'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_AHex(0xFF11)); /* １ */
}
PICOTEST_CASE(testUcdPropertyAlpha) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Alpha(' '));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Alpha('A'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Alpha(0xA9));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Alpha(0xAA));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Alpha(0x0345)); /* ◌ͅ  */
}
PICOTEST_CASE(testUcdPropertyBidi_C) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Bidi_C('A'));
    PICOTEST_VERIFY(
        MiniUCD_GetProperty_Bidi_C(0x061C)); /* ARABIC LETTER MARK */
}
PICOTEST_CASE(testUcdPropertyBidi_M) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Bidi_M('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Bidi_M('('));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Bidi_M(']'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Bidi_M(0x0F3D)); /* ༽ */
}
PICOTEST_CASE(testUcdPropertyCased) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Cased(' '));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Cased('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Cased(0x02B4)); /* ʴ */
}
PICOTEST_CASE(testUcdPropertyCE) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_CE('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_CE(0x0958)); /* क़ */
}
PICOTEST_CASE(testUcdPropertyCI) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_CI('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_CI(':'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_CI(0x055F)); /* ՟ */
}
PICOTEST_CASE(testUcdPropertyComp_Ex) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Comp_Ex('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Comp_Ex(0x0341)); /* ◌́ */
}
PICOTEST_CASE(testUcdPropertyCWCF) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_CWCF('!'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_CWCF('A'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_CWCF('a'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_CWCF(0xA0));   /* NO-BREAK SPACE */
    PICOTEST_VERIFY(MiniUCD_GetProperty_CWCF(0xB5));    /* µ */
    PICOTEST_VERIFY(!MiniUCD_GetProperty_CWCF(0x0341)); /* ◌́ */
    PICOTEST_VERIFY(MiniUCD_GetProperty_CWCF(0x0345));  /* ◌ͅ */
}
PICOTEST_CASE(testUcdPropertyCWCM) {
    PICOTEST_VERIFY(MiniUCD_GetProperty_CWCM('A'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_CWCM(0xA0)); /* NO-BREAK SPACE */
}
PICOTEST_CASE(testUcdPropertyCWKCF) {
    PICOTEST_VERIFY(MiniUCD_GetProperty_CWKCF('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_CWKCF(0xA0)); /* NO-BREAK SPACE */
}
PICOTEST_CASE(testUcdPropertyCWL) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_CWL(' '));
    PICOTEST_VERIFY(MiniUCD_GetProperty_CWL('A'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_CWL('a'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_CWL(0x0341)); /* ◌́ */
    PICOTEST_VERIFY(!MiniUCD_GetProperty_CWL(0x0345)); /* ◌ͅ */
}
PICOTEST_CASE(testUcdPropertyCWT) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_CWT(' '));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_CWT('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_CWT('a'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_CWT(0x0133));  /* ĳ */
    PICOTEST_VERIFY(MiniUCD_GetProperty_CWT(0x01C7));  /* Ǉ */
    PICOTEST_VERIFY(MiniUCD_GetProperty_CWT(0x01C9));  /* ǉ */
    PICOTEST_VERIFY(!MiniUCD_GetProperty_CWT(0x01C8)); /* ǈ */
}
PICOTEST_CASE(testUcdPropertyCWU) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_CWU(' '));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_CWU('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_CWU('a'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_CWU(0x0133));  /* ĳ */
    PICOTEST_VERIFY(MiniUCD_GetProperty_CWU(0x01C9));  /* ǉ */
    PICOTEST_VERIFY(MiniUCD_GetProperty_CWU(0x01C8));  /* ǈ */
    PICOTEST_VERIFY(!MiniUCD_GetProperty_CWU(0x01C7)); /* Ǉ */
}
PICOTEST_CASE(testUcdPropertyDash) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Dash('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Dash('-'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Dash(0x058A)); /* ֊ */
}
PICOTEST_CASE(testUcdPropertyDep) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Dep('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Dep(0x0149)); /* ŉ */
}
PICOTEST_CASE(testUcdPropertyDI) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_DI('A'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_DI('-'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_DI(0xAD));   /* SOFT HYPHEN */
    PICOTEST_VERIFY(MiniUCD_GetProperty_DI(0x034F)); /* ◌͏ */
}
PICOTEST_CASE(testUcdPropertyDia) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Dia('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Dia('^'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Dia(0x0374)); /* ʹ */
}
PICOTEST_CASE(testUcdPropertyEBase) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_EBase('A'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_EBase('#'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_EBase('*'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_EBase(0x261D));   /* ☝ */
    PICOTEST_VERIFY(!MiniUCD_GetProperty_EBase(0x1F1E6)); /* 🇦 */
}
PICOTEST_CASE(testUcdPropertyEComp) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_EComp('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_EComp('#'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_EComp('*'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_EComp(0x1F1E6)); /* 🇦 */
}
PICOTEST_CASE(testUcdPropertyEMod) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_EMod('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_EMod(
        0x1F3FB)); /* EMOJI MODIFIER FITZPATRICK TYPE-3 */
}
PICOTEST_CASE(testUcdPropertyEmoji) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Emoji('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Emoji('#'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Emoji('*'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Emoji(0x261D)); /* ☝ */
}
PICOTEST_CASE(testUcdPropertyEPres) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_EPres('A'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_EPres(0x261D)); /* ☝ */
    PICOTEST_VERIFY(MiniUCD_GetProperty_EPres(0x231B));  /* ⌛ */
}
PICOTEST_CASE(testUcdPropertyExt) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Ext('A'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Ext(':'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Ext(0xB7));   /* · */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Ext(0x02D0)); /* ː */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Ext(0x0E46)); /* ๆ */
}
PICOTEST_CASE(testUcdPropertyExtPict) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_ExtPict('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_ExtPict(0xA9));   /* © */
    PICOTEST_VERIFY(MiniUCD_GetProperty_ExtPict(0x203C)); /* ‼ */
    PICOTEST_VERIFY(MiniUCD_GetProperty_ExtPict(0x2197)); /* ↗ */
    PICOTEST_VERIFY(MiniUCD_GetProperty_ExtPict(0x231B)); /* ⌛ */
}
PICOTEST_CASE(testUcdPropertyGr_Base) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Gr_Base('\t'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Gr_Base('A'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Gr_Base(0xAD));   /* SOFT HYPHEN */
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Gr_Base(0x0300)); /* ◌̀  */
}
PICOTEST_CASE(testUcdPropertyGr_Ext) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Gr_Ext('A'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Gr_Ext('\t'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Gr_Ext(0xAD));  /* SOFT HYPHEN */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Gr_Ext(0x0300)); /* ◌̀  */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Gr_Ext(0x09BE)); /* া */
}
PICOTEST_CASE(testUcdPropertyHex) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Hex('/'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Hex('0'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Hex('9'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Hex(';'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Hex('@'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Hex('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Hex('F'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Hex('G'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Hex('`'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Hex('a'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Hex('f'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Hex('g'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Hex(0xFF11)); /* １ */
}
PICOTEST_CASE(testUcdPropertyIDC) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_IDC(' '));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_IDC('!'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_IDC('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_IDC(0xB7));   /* · */
    PICOTEST_VERIFY(MiniUCD_GetProperty_IDC(0x037A)); /* ͺ */
    PICOTEST_VERIFY(MiniUCD_GetProperty_IDC(0x0387)); /* · */
}
PICOTEST_CASE(testUcdPropertyIdeo) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Ideo('A'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Ideo(0x3005)); /* 々 */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Ideo(0x3006));  /* 〆 */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Ideo(0x3400));  /* 㐀 */
}
PICOTEST_CASE(testUcdPropertyIDS) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_IDS(' '));
    PICOTEST_VERIFY(MiniUCD_GetProperty_IDS('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_IDS(0x037A)); /* ͺ */
    PICOTEST_VERIFY(MiniUCD_GetProperty_IDS(0x1885)); /* ◌ᢅ */
}
PICOTEST_CASE(testUcdPropertyIDSB) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_IDSB(' '));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_IDSB('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_IDSB(0x2FF0));  /* ⿰ */
    PICOTEST_VERIFY(!MiniUCD_GetProperty_IDSB(0x2FF2)); /* ⿲ */
}
PICOTEST_CASE(testUcdPropertyIDST) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_IDST(' '));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_IDST('A'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_IDST(0x2FF0)); /* ⿰ */
    PICOTEST_VERIFY(MiniUCD_GetProperty_IDST(0x2FF2));  /* ⿲ */
}
PICOTEST_CASE(testUcdPropertyJoin_C) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Join_C('A'));
    PICOTEST_VERIFY(
        MiniUCD_GetProperty_Join_C(0x200C)); /* ZERO WIDTH NON-JOINER */
}
PICOTEST_CASE(testUcdPropertyLOE) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_LOE('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_LOE(0x0E44));  /* ไ */
    PICOTEST_VERIFY(!MiniUCD_GetProperty_LOE(0x0E45)); /* ๅ */
}
PICOTEST_CASE(testUcdPropertyLower) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Lower(' '));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Lower('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Lower('a'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Lower(0xAA));    /* ª */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Lower(0xDF));    /* ß */
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Lower(0x0182)); /* Ƃ */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Lower(0x0183));  /* ƃ */
}
PICOTEST_CASE(testUcdPropertyMath) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Math('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Math('+'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Math('='));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Math(0xB1));   /* ± */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Math(0x03D0)); /* ϐ */
}
PICOTEST_CASE(testUcdPropertyNChar) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_NChar('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_NChar(0xFFFE));
    PICOTEST_VERIFY(MiniUCD_GetProperty_NChar(0xFFFF));
}
PICOTEST_CASE(testUcdPropertyOAlpha) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_OAlpha(' '));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_OAlpha('A'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_OAlpha(0xA9));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_OAlpha(0xAA));
    PICOTEST_VERIFY(MiniUCD_GetProperty_OAlpha(0x0345)); /* ◌ͅ  */
}
PICOTEST_CASE(testUcdPropertyODI) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_ODI('A'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_ODI('-'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_ODI(0xAD));  /* SOFT HYPHEN */
    PICOTEST_VERIFY(MiniUCD_GetProperty_ODI(0x034F)); /* ◌͏ */
}
PICOTEST_CASE(testUcdPropertyOGr_Ext) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_OGr_Ext('A'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_OGr_Ext('\t'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_OGr_Ext(0xAD));   /* SOFT HYPHEN */
    PICOTEST_VERIFY(!MiniUCD_GetProperty_OGr_Ext(0x0300)); /* ◌̀  */
    PICOTEST_VERIFY(MiniUCD_GetProperty_OGr_Ext(0x09BE));  /* া */
}
PICOTEST_CASE(testUcdPropertyOIDC) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_OIDC(' '));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_OIDC('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_OIDC(0xB7));   /* · */
    PICOTEST_VERIFY(MiniUCD_GetProperty_OIDC(0x0387)); /* · */
}
PICOTEST_CASE(testUcdPropertyOIDS) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_OIDS(' '));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_OIDS('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_OIDS(0x1885)); /* ◌ᢅ */
}
PICOTEST_CASE(testUcdPropertyOLower) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_OLower(' '));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_OLower('A'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_OLower('a'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_OLower(0xAA));    /* ª */
    PICOTEST_VERIFY(!MiniUCD_GetProperty_OLower(0xDF));   /* ß */
    PICOTEST_VERIFY(!MiniUCD_GetProperty_OLower(0x0182)); /* Ƃ */
    PICOTEST_VERIFY(!MiniUCD_GetProperty_OLower(0x0183)); /* ƃ */
}
PICOTEST_CASE(testUcdPropertyOMath) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_OMath('A'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_OMath('+'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_OMath('='));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_OMath(0xB1));  /* ± */
    PICOTEST_VERIFY(MiniUCD_GetProperty_OMath(0x03D0)); /* ϐ */
}
PICOTEST_CASE(testUcdPropertyOUpper) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_OUpper(' '));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_OUpper('A'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_OUpper('a'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_OUpper(0xAA));   /* ª */
    PICOTEST_VERIFY(!MiniUCD_GetProperty_OUpper(0xDF));   /* ß */
    PICOTEST_VERIFY(!MiniUCD_GetProperty_OUpper(0x0182)); /* Ƃ */
    PICOTEST_VERIFY(MiniUCD_GetProperty_OUpper(0x2163));  /* Ⅳ */
}
PICOTEST_CASE(testUcdPropertyPat_Syn) {
    PICOTEST_VERIFY(MiniUCD_GetProperty_Pat_Syn('!'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Pat_Syn('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Pat_Syn(0xFE46)); /* ﹆ */
}
PICOTEST_CASE(testUcdPropertyPat_WS) {
    PICOTEST_VERIFY(MiniUCD_GetProperty_Pat_WS('\t'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Pat_WS(' '));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Pat_WS('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Pat_WS(0x85));   /* NEXT LINE */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Pat_WS(0x2028)); /* LINE SEPARATOR */
    PICOTEST_VERIFY(
        !MiniUCD_GetProperty_Pat_WS(0x3000)); /* IDEOGRAPHIC SPACE */
}
PICOTEST_CASE(testUcdPropertyPCM) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_PCM('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_PCM(0x0600)); /* ARABIC NUMBER SIGN */
}
PICOTEST_CASE(testUcdPropertyQMark) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_QMark('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_QMark('"'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_QMark(0x2018)); /* ‘ */
}
PICOTEST_CASE(testUcdPropertyRadical) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Radical('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Radical(0x2E8E)); /* ⺎ */
}
PICOTEST_CASE(testUcdPropertyRI) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_RI('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_RI(0x1F1E6)); /* 🇦 */
}
PICOTEST_CASE(testUcdPropertySD) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_SD('A'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_SD('I'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_SD('i'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_SD(0x012F));  /* į */
    PICOTEST_VERIFY(!MiniUCD_GetProperty_SD(0x0130)); /* İ */
}
PICOTEST_CASE(testUcdPropertySTerm) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_STerm('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_STerm('.'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_STerm(','));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_STerm(0x037E)); /* ; */
    PICOTEST_VERIFY(!MiniUCD_GetProperty_STerm(0x061B)); /* ; */
    PICOTEST_VERIFY(MiniUCD_GetProperty_STerm(0x061E));  /* ؞ */
}
PICOTEST_CASE(testUcdPropertyTerm) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Term('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Term('.'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Term(','));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Term(0x037E)); /* ; */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Term(0x061B)); /* ; */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Term(0x061E)); /* ؞ */
}
PICOTEST_CASE(testUcdPropertyUIdeo) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_UIdeo('A'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_UIdeo(0x3005)); /* 々 */
    PICOTEST_VERIFY(!MiniUCD_GetProperty_UIdeo(0x3006)); /* 〆 */
    PICOTEST_VERIFY(MiniUCD_GetProperty_UIdeo(0x3400));  /* 㐀 */
}
PICOTEST_CASE(testUcdPropertyUpper) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Upper(' '));
    PICOTEST_VERIFY(MiniUCD_GetProperty_Upper('A'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Upper('a'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Upper(0xAA));   /* ª */
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Upper(0xDF));   /* ß */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Upper(0x0182));  /* Ƃ */
    PICOTEST_VERIFY(!MiniUCD_GetProperty_Upper(0x0183)); /* ƃ */
    PICOTEST_VERIFY(MiniUCD_GetProperty_Upper(0x2163));  /* Ⅳ */
}
PICOTEST_CASE(testUcdPropertyVS) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_VS('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_VS(0xFE00)); /* VARIATION SELECTOR-1 */
}
PICOTEST_CASE(testUcdPropertyWSpace) {
    PICOTEST_VERIFY(MiniUCD_GetProperty_WSpace('\t'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_WSpace(' '));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_WSpace('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_WSpace(0x85));   /* NEXT LINE */
    PICOTEST_VERIFY(MiniUCD_GetProperty_WSpace(0x2028)); /* LINE SEPARATOR */
    PICOTEST_VERIFY(MiniUCD_GetProperty_WSpace(0x3000)); /* IDEOGRAPHIC SPACE */
}
PICOTEST_CASE(testUcdPropertyXIDC) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_XIDC(' '));
    PICOTEST_VERIFY(MiniUCD_GetProperty_XIDC('A'));
    PICOTEST_VERIFY(MiniUCD_GetProperty_XIDC(0xB7));    /* · */
    PICOTEST_VERIFY(!MiniUCD_GetProperty_XIDC(0x037A)); /* ͺ */
    PICOTEST_VERIFY(MiniUCD_GetProperty_XIDC(0x0387));  /* · */
}
PICOTEST_CASE(testUcdPropertyXIDS) {
    PICOTEST_VERIFY(!MiniUCD_GetProperty_XIDS(' '));
    PICOTEST_VERIFY(MiniUCD_GetProperty_XIDS('A'));
    PICOTEST_VERIFY(!MiniUCD_GetProperty_XIDS(0x037A)); /* ͺ */
    PICOTEST_VERIFY(MiniUCD_GetProperty_XIDS(0x1885));  /* ◌ᢅ */
}
