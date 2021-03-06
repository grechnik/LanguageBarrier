#include "GameText.h"
#include <list>
#include <vector>
#include "Game.h"
#include "LanguageBarrier.h"
#include "SigScan.h"

typedef struct __declspec(align(4)) {
  char gap0[316];
  int somePageNumber;
  char gap140[12];
  const char *pString;
} Sc3_t;

// this is my own, not from the game
typedef struct {
  int lines;
  int length;
  int textureStartX[lb::MAX_PROCESSED_STRING_LENGTH];
  int textureStartY[lb::MAX_PROCESSED_STRING_LENGTH];
  int textureWidth[lb::MAX_PROCESSED_STRING_LENGTH];
  int textureHeight[lb::MAX_PROCESSED_STRING_LENGTH];
  int displayStartX[lb::MAX_PROCESSED_STRING_LENGTH];
  int displayStartY[lb::MAX_PROCESSED_STRING_LENGTH];
  int displayEndX[lb::MAX_PROCESSED_STRING_LENGTH];
  int displayEndY[lb::MAX_PROCESSED_STRING_LENGTH];
  int color[lb::MAX_PROCESSED_STRING_LENGTH];
  int glyph[lb::MAX_PROCESSED_STRING_LENGTH];
  uint8_t linkNumber[lb::MAX_PROCESSED_STRING_LENGTH];
  int linkCharCount;
  int linkCount;
  int curLinkNumber;
  int curColor;
  bool error;
} ProcessedSc3String_t;

// also my own
typedef struct {
  const char *start;
  const char *end;
  uint16_t cost;
  bool startsWithSpace;
  bool endsWithLinebreak;
} StringWord_t;

typedef struct __declspec(align(4)) {
  int linkNumber;
  int displayX;
  int displayY;
  int displayWidth;
  int displayHeight;
} LinkMetrics_t;

typedef struct {
  int field_0;
  int field_4;
  int drawNextPageNow;
  int pageLength;
  int field_10;
  char field_14;
  char field_15;
  char field_16;
  char field_17;
  int field_18;
  int field_1C;
  int field_20;
  int field_24;
  int field_28;
  int field_2C;
  int field_30;
  int field_34;
  int field_38;
  int fontNumber[lb::MAX_DIALOGUE_PAGE_LENGTH];
  int charColor[lb::MAX_DIALOGUE_PAGE_LENGTH];
  int charOutlineColor[lb::MAX_DIALOGUE_PAGE_LENGTH];
  char glyphCol[lb::MAX_DIALOGUE_PAGE_LENGTH];
  char glyphRow[lb::MAX_DIALOGUE_PAGE_LENGTH];
  char glyphOrigWidth[lb::MAX_DIALOGUE_PAGE_LENGTH];
  char glyphOrigHeight[lb::MAX_DIALOGUE_PAGE_LENGTH];
  __int16 charDisplayX[lb::MAX_DIALOGUE_PAGE_LENGTH];
  __int16 charDisplayY[lb::MAX_DIALOGUE_PAGE_LENGTH];
  __int16 glyphDisplayWidth[lb::MAX_DIALOGUE_PAGE_LENGTH];
  __int16 glyphDisplayHeight[lb::MAX_DIALOGUE_PAGE_LENGTH];
  char field_BBBC[lb::MAX_DIALOGUE_PAGE_LENGTH];
  int field_C38C[lb::MAX_DIALOGUE_PAGE_LENGTH];
  char charDisplayOpacity[lb::MAX_DIALOGUE_PAGE_LENGTH];
} DialoguePage_t;

typedef void(__cdecl *DrawDialogueProc)(int fontNumber, int pageNumber,
                                        int opacity, int xOffset, int yOffset);
static DrawDialogueProc gameExeDrawDialogue =
    NULL;  // = (DrawDialogueProc)0x44B500;
static DrawDialogueProc gameExeDrawDialogueReal = NULL;

typedef void(__cdecl *DrawDialogue2Proc)(int fontNumber, int pageNumber,
                                         int opacity);
static DrawDialogue2Proc gameExeDrawDialogue2 =
    NULL;  // = (DrawDialogue2Proc)0x44B0D0;
static DrawDialogue2Proc gameExeDrawDialogue2Real = NULL;

typedef int(__cdecl *DialogueLayoutRelatedProc)(int unk0, int *unk1, int *unk2,
                                                int unk3, int unk4, int unk5,
                                                int unk6, int yOffset,
                                                int lineHeight);
static DialogueLayoutRelatedProc gameExeDialogueLayoutRelated =
    NULL;  // = (DialogueLayoutRelatedProc)0x448790;
static DialogueLayoutRelatedProc gameExeDialogueLayoutRelatedReal = NULL;

typedef void(__cdecl *DrawGlyphProc)(int textureId, float glyphInTextureStartX,
                                     float glyphInTextureStartY,
                                     float glyphInTextureWidth,
                                     float glyphInTextureHeight,
                                     float displayStartX, float displayStartY,
                                     float displayEndX, float displayEndY,
                                     int color, uint32_t opacity);
static DrawGlyphProc gameExeDrawGlyph = NULL;  // = (DrawGlyphProc)0x42F950;
static DrawGlyphProc gameExeDrawGlyphReal = NULL;

typedef void(__cdecl *DrawGlyphMaskedProc)(int fontTextureId,
                                           int maskTextureId,
                                           float glyphInTextureStartX,
                                           float glyphInTextureStartY,
                                           float glyphInTextureWidth,
                                           float glyphInTextureHeight,
                                           float glyphInMaskStartX,
                                           float glyphInMaskStartY,
                                           float displayStartX,
                                           float displayStartY,
                                           float displayEndX,
                                           float displayEndY,
                                           int color, uint32_t opacity);
static DrawGlyphMaskedProc gameExeDrawGlyphMasked = NULL;
static DrawGlyphMaskedProc gameExeDrawGlyphMaskedReal = NULL;

typedef void(__cdecl *DrawGlyphMaskedProc2)(int fontTextureId,
                                            int maskTextureId,
                                            float glyphInTextureStartX,
                                            float glyphInTextureStartY,
                                            float glyphInTextureWidth,
                                            float glyphInTextureHeight,
                                            float displayStartX,
                                            float displayStartY,
                                            float displayEndX,
                                            float displayEndY,
                                            int color, uint32_t opacity);
static DrawGlyphMaskedProc2 gameExeDrawGlyphMasked2 = NULL;
static DrawGlyphMaskedProc2 gameExeDrawGlyphMasked2Real = NULL;

typedef int(__cdecl *DrawRectangleProc)(float X, float Y, float width,
                                        float height, int color,
                                        uint32_t opacity);
static DrawRectangleProc gameExeDrawRectangle =
    NULL;  // = (DrawRectangleProc)0x42F890;

typedef int(__cdecl *DrawPhoneTextProc)(int textureId, int xOffset, int yOffset,
                                        int lineLength, char *sc3string,
                                        int lineSkipCount, int lineDisplayCount,
                                        int color, int baseGlyphSize,
                                        int opacity);
static DrawPhoneTextProc gameExeDrawPhoneText =
    NULL;  // = (DrawPhoneTextProc)0x444F70;
static DrawPhoneTextProc gameExeDrawPhoneTextReal = NULL;

typedef int(__cdecl *GetSc3StringDisplayWidthProc)(const char *string,
                                                   unsigned int maxCharacters,
                                                   int baseGlyphSize);
static GetSc3StringDisplayWidthProc gameExeGetSc3StringDisplayWidthFont1 =
    NULL;  // = (GetSc3StringDisplayWidthProc)0x4462E0;
static GetSc3StringDisplayWidthProc gameExeGetSc3StringDisplayWidthFont1Real =
    NULL;
static GetSc3StringDisplayWidthProc gameExeGetSc3StringDisplayWidthFont2 =
    NULL;  // = (GetSc3StringDisplayWidthProc)0x4461F0;
static GetSc3StringDisplayWidthProc gameExeGetSc3StringDisplayWidthFont2Real =
    NULL;

typedef int(__cdecl *Sc3EvalProc)(Sc3_t *sc3, int *pOutResult);
static Sc3EvalProc gameExeSc3Eval = NULL;  // = (Sc3EvalProc)0x4181D0;

typedef int(__cdecl *GetLinksFromSc3StringProc)(int xOffset, int yOffset,
                                                int lineLength, char *sc3string,
                                                int lineSkipCount,
                                                int lineDisplayCount,
                                                int baseGlyphSize,
                                                LinkMetrics_t *result);
static GetLinksFromSc3StringProc gameExeGetLinksFromSc3String =
    NULL;  // = (GetLinksFromSc3StringProc)0x445EA0;
static GetLinksFromSc3StringProc gameExeGetLinksFromSc3StringReal = NULL;

typedef int(__cdecl *DrawInteractiveMailProc)(
    int textureId, int xOffset, int yOffset, signed int lineLength,
    char *sc3string, unsigned int lineSkipCount, unsigned int lineDisplayCount,
    int color, unsigned int baseGlyphSize, int opacity, int unselectedLinkColor,
    int selectedLinkColor, int selectedLink);
static DrawInteractiveMailProc gameExeDrawInteractiveMail =
    NULL;  // = (DrawInteractiveMailProc)0x4453D0;
static DrawInteractiveMailProc gameExeDrawInteractiveMailReal = NULL;

typedef int(__cdecl *DrawLinkHighlightProc)(
    int xOffset, int yOffset, int lineLength, char *sc3string,
    unsigned int lineSkipCount, unsigned int lineDisplayCount, int color,
    unsigned int baseGlyphSize, int opacity, int selectedLink);
static DrawLinkHighlightProc gameExeDrawLinkHighlight =
    NULL;  // = (DrawLinkHighlightProc)0x444B90;
static DrawLinkHighlightProc gameExeDrawLinkHighlightReal = NULL;

typedef int(__cdecl *GetSc3StringLineCountProc)(int lineLength, char *sc3string,
                                                unsigned int baseGlyphSize);
static GetSc3StringLineCountProc gameExeGetSc3StringLineCount =
    NULL;  // = (GetSc3StringLineCountProc)0x442790;
static GetSc3StringLineCountProc gameExeGetSc3StringLineCountReal = NULL;
typedef void(__cdecl *GetVisibleLinksProc)(
    unsigned lineLength, const char *sc3string,
    unsigned lineSkipCount, unsigned lineDisplayCount,
    unsigned baseGlyphSize, char *result);
static GetVisibleLinksProc gameExeGetVisibleLinks = NULL;

static uintptr_t gameExeDialogueLayoutWidthLookup1 = NULL;
static uintptr_t gameExeDialogueLayoutWidthLookup1Return = NULL;
static uintptr_t gameExeDialogueLayoutWidthLookup2 = NULL;
static uintptr_t gameExeDialogueLayoutWidthLookup2Return = NULL;
static uintptr_t gameExeDialogueLayoutWidthLookup3 = NULL;
static uintptr_t gameExeDialogueLayoutWidthLookup3Return = NULL;
static uintptr_t gameExeTipsListWidthLookup = NULL;
static uintptr_t gameExeTipsListWidthLookupReturn = NULL;
static uintptr_t gameExeNewTipWidthLookup = NULL;
static uintptr_t gameExeNewTipWidthLookupReturn = NULL;
static uintptr_t gameExeTipAltTitleWidthLookup = NULL;
static uintptr_t gameExeTipAltTitleWidthLookupReturn = NULL;
static uintptr_t gameExeDialogueSetLineBreakFlags = NULL;
static uintptr_t gameExeDialogueSetLineBreakFlagsReturn = NULL;

static DialoguePage_t *gameExeDialoguePages =
    NULL;  // (DialoguePage_t *)0x164D680;

static uint8_t *gameExeGlyphWidthsFont1 = NULL;        // = (uint8_t *)0x52C7F0;
static uint8_t *gameExeGlyphWidthsFont2 = NULL;        // = (uint8_t *)0x52E058;
static int *gameExeColors = NULL;                      // = (int *)0x52E1E8;
static uint8_t *gameExeBacklogHighlightHeight = NULL;  // = (uint8_t *)0x435DD4;
static uint8_t *gameExeLineBreakFlags = NULL;

static uint8_t widths[lb::TOTAL_NUM_CHARACTERS];
static uint8_t charFlags[lb::TOTAL_NUM_CHARACTERS];

static std::string fontBuffers[4];
static bool hasSplitFont = false;

// MSVC doesn't like having these inside namespaces
__declspec(naked) void dialogueLayoutWidthLookup1Hook() {
  __asm {
    movzx edx, widths[ecx]
    jmp gameExeDialogueLayoutWidthLookup1Return
  }
}

__declspec(naked) void dialogueLayoutWidthLookup2Hook() {
  __asm {
    push ebx
    movzx ebx, [lb::FONT_ROW_LENGTH]
    add eax, ebx
    movzx ecx, widths[eax]
    sub eax, ebx
    pop ebx
    jmp gameExeDialogueLayoutWidthLookup2Return
  }
}

__declspec(naked) void dialogueLayoutWidthLookup3Hook() {
  __asm {
    movzx ecx, widths[edx]
    jmp gameExeDialogueLayoutWidthLookup3Return
  }
}

__declspec(naked) void tipsListWidthLookupHook() {
  __asm {
    movzx eax, widths[edx]
    jmp gameExeTipsListWidthLookupReturn
  }
}

__declspec(naked) void newTipWidthLookupHook() {
  __asm {
    movzx edi, widths[edx]
    jmp gameExeNewTipWidthLookupReturn
  }
}

__declspec(naked) void tipAltTitleWidthLookupHook() {
  __asm {
    movzx ecx, widths[edx]
    jmp gameExeTipAltTitleWidthLookupReturn
  }
}

__declspec(naked) void dialogueSetLineBreakFlagsHook() {
  __asm {
    test charFlags[esi], 1
    jz nope
    mov ecx, [gameExeLineBreakFlags]
    test charFlags[edx], 1
    jz noprev
    or byte ptr [eax+ecx], 9
noprev:
    test charFlags[edi], 1
    jz nope
    or byte ptr [eax+ecx], 0Ah
nope:
    jmp gameExeDialogueSetLineBreakFlagsReturn
  }
}

namespace lb {
void __cdecl drawGlyphHook(int textureId,
                           float glyphInTextureStartX,
                           float glyphInTextureStartY,
                           float glyphInTextureWidth,
                           float glyphInTextureHeight,
                           float displayStartX, float displayStartY,
                           float displayEndX, float displayEndY,
                           int color, uint32_t opacity);
void __cdecl drawGlyphMaskedHook(int fontTextureId, int maskTextureId,
                                 float glyphInTextureStartX,
                                 float glyphInTextureStartY,
                                 float glyphInTextureWidth,
                                 float glyphInTextureHeight,
                                 float glyphInMaskStartX,
                                 float glyphInMaskStartY,
                                 float displayStartX, float displayStartY,
                                 float displayEndX, float displayEndY,
                                 int color, uint32_t opacity);
void __cdecl drawGlyphMasked2Hook(int fontTextureId, int maskTextureId,
                                  float glyphInTextureStartX,
                                  float glyphInTextureStartY,
                                  float glyphInTextureWidth,
                                  float glyphInTextureHeight,
                                  float displayStartX, float displayStartY,
                                  float displayEndX, float displayEndY,
                                  int color, uint32_t opacity);
void __cdecl drawDialogueHook(int fontNumber, int pageNumber, uint32_t opacity,
                              int xOffset, int yOffset);
void __cdecl drawDialogue2Hook(int fontNumber, int pageNumber,
                               uint32_t opacity);
int __cdecl dialogueLayoutRelatedHook(int unk0, int *unk1, int *unk2, int unk3,
                                      int unk4, int unk5, int unk6, int yOffset,
                                      int lineHeight);
int __cdecl drawPhoneTextHook(int textureId, int xOffset, int yOffset,
                              int lineLength, char *sc3string,
                              int lineSkipCount, int lineDisplayCount,
                              int color, int baseGlyphSize, int opacity);
void semiTokeniseSc3String(const char *sc3string, std::list<StringWord_t> &words,
                           int baseGlyphSize, int lineLength);
void processSc3TokenList(int xOffset, int yOffset, int lineLength,
                         std::list<StringWord_t> &words, int lineCount,
                         int color, int baseGlyphSize,
                         ProcessedSc3String_t *result, bool measureOnly,
                         float multiplier, int lastLinkNumber,
                         int curLinkNumber, int currentColor,
                         bool truncateExcessWord = false);
int __cdecl getSc3StringDisplayWidthHook(const char *sc3string,
                                         unsigned int maxCharacters,
                                         int baseGlyphSize);
int __cdecl getLinksFromSc3StringHook(int xOffset, int yOffset, int lineLength,
                                      char *sc3string, int lineSkipCount,
                                      int lineDisplayCount, int baseGlyphSize,
                                      LinkMetrics_t *result);
int __cdecl drawInteractiveMailHook(int textureId, int xOffset, int yOffset,
                                    signed int lineLength, char *string,
                                    unsigned int lineSkipCount,
                                    unsigned int lineDisplayCount, int color,
                                    unsigned int glyphSize, int opacity,
                                    int unselectedLinkColor,
                                    int selectedLinkColor, int selectedLink);
int __cdecl drawLinkHighlightHook(int xOffset, int yOffset, int lineLength,
                                  char *sc3string, unsigned int lineSkipCount,
                                  unsigned int lineDisplayCount, int color,
                                  unsigned int baseGlyphSize, int opacity,
                                  int selectedLink);
void __cdecl getVisibleLinksHook(unsigned lineLength, const char *sc3string,
                                 unsigned lineSkipCount, unsigned lineDisplayCount,
                                 unsigned baseGlyphSize, char *result);
int __cdecl getSc3StringLineCountHook(int lineLength, char *sc3string,
                                      unsigned int baseGlyphSize);
// There are a bunch more functions like these but I haven't seen them get hit
// during debugging and the original code *mostly* works okay if it recognises
// western text as variable-width
// (which some functions do, and others don't, except for symbols (also used in
// Western translations) it considers full-width)

void gameTextInit() {
  // gee I sure hope nothing important ever goes in OUTLINE_TEXTURE_ID...
  try {
    // assigned texture ids should be consistent with fixTextureForSplitFont
    fontBuffers[2] = slurpFile("languagebarrier\\font_a.png");
    fontBuffers[3] = slurpFile("languagebarrier\\font_b.png");
    fontBuffers[0] = slurpFile("languagebarrier\\font-outline_a.png");
    fontBuffers[1] = slurpFile("languagebarrier\\font-outline_b.png");
    hasSplitFont = true;
    for (int i = 0; i < 4; i++) {
      gameLoadTexture(OUTLINE_TEXTURE_ID + i, &(fontBuffers[i][0]), fontBuffers[i].size());
    }
    LanguageBarrierLog("split font loaded");
  } catch (std::runtime_error&) {
    LanguageBarrierLog("failed to load split font");
  }
  if (!hasSplitFont) {
    fontBuffers[0] = slurpFile("languagebarrier\\font-outline.png");
    gameLoadTexture(OUTLINE_TEXTURE_ID, &(fontBuffers[0][0]),
                    fontBuffers[0].size());
  }
  // the game loads this asynchronously - I'm not sure how to be notified it's
  // done and I can free the buffer
  // so I'll just do it in a hook

  if (hasSplitFont) {
    scanCreateEnableHook(
        "game", "drawGlyph", (uintptr_t *)&gameExeDrawGlyph,
        (LPVOID)drawGlyphHook, (LPVOID *)&gameExeDrawGlyphReal);
    scanCreateEnableHook(
        "game", "drawGlyphMasked", (uintptr_t*)&gameExeDrawGlyphMasked,
        (LPVOID)drawGlyphMaskedHook, (LPVOID*)&gameExeDrawGlyphMaskedReal);
    scanCreateEnableHook("game", "drawGlyphMasked2", (uintptr_t*)&gameExeDrawGlyphMasked2,
        (LPVOID)drawGlyphMasked2Hook, (LPVOID*)&gameExeDrawGlyphMasked2Real);
  } else {
    gameExeDrawGlyph = (DrawGlyphProc)sigScan("game", "drawGlyph");
  }
  gameExeDrawRectangle = (DrawRectangleProc)sigScan("game", "drawRectangle");
  gameExeSc3Eval = (Sc3EvalProc)sigScan("game", "sc3Eval");
  gameExeBacklogHighlightHeight =
      (uint8_t *)sigScan("game", "backlogHighlightHeight");

  if (gameExeBacklogHighlightHeight) {
    // gameExeBacklogHighlightHeight is (negative) offset (from vertical end of
    // glyph):
    // add eax,-0x22 (83 C0 DE) -> add eax,-0x17 (83 C0 E9)
    DWORD oldProtect;
    VirtualProtect(gameExeBacklogHighlightHeight, 1, PAGE_READWRITE, &oldProtect);
    *gameExeBacklogHighlightHeight = 0xE9;
    VirtualProtect(gameExeBacklogHighlightHeight, 1, oldProtect, &oldProtect);
  }

  scanCreateEnableHook(
      "game", "drawDialogue", (uintptr_t *)&gameExeDrawDialogue,
      (LPVOID)drawDialogueHook, (LPVOID *)&gameExeDrawDialogueReal);
  scanCreateEnableHook(
      "game", "drawDialogue2", (uintptr_t *)&gameExeDrawDialogue2,
      (LPVOID)drawDialogue2Hook, (LPVOID *)&gameExeDrawDialogue2Real);
  scanCreateEnableHook("game", "dialogueLayoutRelated",
                       (uintptr_t *)&gameExeDialogueLayoutRelated,
                       (LPVOID)dialogueLayoutRelatedHook,
                       (LPVOID *)&gameExeDialogueLayoutRelatedReal);
  scanCreateEnableHook(
      "game", "drawPhoneText", (uintptr_t *)&gameExeDrawPhoneText,
      (LPVOID)drawPhoneTextHook, (LPVOID *)&gameExeDrawPhoneTextReal);
  // The following both have the same pattern and 'occurrence: 0' in the
  // signatures.json.
  // That's because after you hook one, the first match goes away.
  scanCreateEnableHook("game", "getSc3StringDisplayWidthFont1",
                       (uintptr_t *)&gameExeGetSc3StringDisplayWidthFont1,
                       (LPVOID)getSc3StringDisplayWidthHook,
                       (LPVOID *)&gameExeGetSc3StringDisplayWidthFont1Real);
  scanCreateEnableHook("game", "getSc3StringDisplayWidthFont2",
                       (uintptr_t *)&gameExeGetSc3StringDisplayWidthFont2,
                       (LPVOID)getSc3StringDisplayWidthHook,
                       (LPVOID *)&gameExeGetSc3StringDisplayWidthFont2Real);
  scanCreateEnableHook("game", "getLinksFromSc3String",
                       (uintptr_t *)&gameExeGetLinksFromSc3String,
                       (LPVOID)getLinksFromSc3StringHook,
                       (LPVOID *)&gameExeGetLinksFromSc3StringReal);
  scanCreateEnableHook("game", "drawInteractiveMail",
                       (uintptr_t *)&gameExeDrawInteractiveMail,
                       (LPVOID)drawInteractiveMailHook,
                       (LPVOID *)&gameExeDrawInteractiveMailReal);
  scanCreateEnableHook(
      "game", "drawLinkHighlight", (uintptr_t *)&gameExeDrawLinkHighlight,
      (LPVOID)drawLinkHighlightHook, (LPVOID *)&gameExeDrawLinkHighlightReal);
  scanCreateEnableHook("game", "getVisibleLinks",
                       (uintptr_t *)&gameExeGetVisibleLinks,
                       (LPVOID)getVisibleLinksHook,
                       NULL);
  scanCreateEnableHook("game", "getSc3StringLineCount",
                       (uintptr_t *)&gameExeGetSc3StringLineCount,
                       (LPVOID)getSc3StringLineCountHook,
                       (LPVOID *)&gameExeGetSc3StringLineCountReal);

  gameExeDialoguePages =
      (DialoguePage_t *)(*((uint32_t *)((uint8_t *)(gameExeDrawDialogue) +
                                        0x18)) -
                         0xC);
  gameExeGlyphWidthsFont1 =
      *(uint8_t **)((uint8_t *)(gameExeDrawPhoneText) + 0x83);
  gameExeGlyphWidthsFont2 =
      *(uint8_t **)((uint8_t *)(gameExeDrawPhoneText) + 0x74);
  gameExeColors =
      (int *)(*(uint32_t *)((uint8_t *)(gameExeDrawPhoneText) + 0x272) - 0x4);

  scanCreateEnableHook("game", "dialogueLayoutWidthLookup1",
                       &gameExeDialogueLayoutWidthLookup1,
                       dialogueLayoutWidthLookup1Hook, NULL);
  gameExeDialogueLayoutWidthLookup1Return =
      (uintptr_t)((uint8_t *)gameExeDialogueLayoutWidthLookup1 + 0x27);
  scanCreateEnableHook("game", "dialogueLayoutWidthLookup2",
                       &gameExeDialogueLayoutWidthLookup2,
                       dialogueLayoutWidthLookup2Hook, NULL);
  gameExeDialogueLayoutWidthLookup2Return =
      (uintptr_t)((uint8_t *)gameExeDialogueLayoutWidthLookup2 + 0x12);
  scanCreateEnableHook("game", "dialogueLayoutWidthLookup3",
                       &gameExeDialogueLayoutWidthLookup3,
                       dialogueLayoutWidthLookup3Hook, NULL);
  gameExeDialogueLayoutWidthLookup3Return =
      (uintptr_t)((uint8_t *)gameExeDialogueLayoutWidthLookup3 + 0x7);
  scanCreateEnableHook("game", "tipsListWidthLookup",
                       &gameExeTipsListWidthLookup,
                       tipsListWidthLookupHook, NULL);
  gameExeTipsListWidthLookupReturn =
      (uintptr_t)((uint8_t *)gameExeTipsListWidthLookup + 0x14);
  scanCreateEnableHook("game", "dialogueSetLineBreakFlags",
                       &gameExeDialogueSetLineBreakFlags,
                       dialogueSetLineBreakFlagsHook, NULL);
  gameExeLineBreakFlags =
      (uint8_t *)(*(uint32_t *)((uint8_t *)gameExeDialogueSetLineBreakFlags + 0x12));
  gameExeDialogueSetLineBreakFlagsReturn =
      (uintptr_t)((uint8_t *)gameExeDialogueSetLineBreakFlags + 0x26);
  scanCreateEnableHook("game", "newTipWidthLookup",
                       &gameExeNewTipWidthLookup,
                       newTipWidthLookupHook, NULL);
  gameExeNewTipWidthLookupReturn =
      (uintptr_t)((uint8_t *)gameExeNewTipWidthLookup + 0x13);
  scanCreateEnableHook("game", "tipAltTitleWidthLookup",
                       &gameExeTipAltTitleWidthLookup,
                       tipAltTitleWidthLookupHook, NULL);
  gameExeTipAltTitleWidthLookupReturn =
      (uintptr_t)((uint8_t *)gameExeTipAltTitleWidthLookup + 0x18);

  FILE *widthsfile = fopen("languagebarrier\\widths.bin", "rb");
  fread(widths, 1, TOTAL_NUM_CHARACTERS, widthsfile);
  fclose(widthsfile);
  memcpy(gameExeGlyphWidthsFont1, widths, GLYPH_RANGE_FULLWIDTH_START);
  memcpy(gameExeGlyphWidthsFont2, widths, GLYPH_RANGE_FULLWIDTH_START);

  FILE *charFlagsFile = fopen("languagebarrier\\charflags.bin", "rb");
  if (charFlagsFile) {
    fread(charFlags, 1, TOTAL_NUM_CHARACTERS, charFlagsFile);
    fclose(charFlagsFile);
  } else {
    // fallback to default: english letters and digits
    memset(charFlags + 1, 1, 26 + 26 + 10);
    memset(charFlags + 0x80, 1, 26 + 26 + 10);
  }
}

int __cdecl dialogueLayoutRelatedHook(int unk0, int *unk1, int *unk2, int unk3,
                                      int unk4, int unk5, int unk6, int yOffset,
                                      int lineHeight) {
  // release buffers
  // let's just do this here, should be loaded by now...
  for (int i = 0; i < 4; i++)
    fontBuffers[i] = std::string{};

  return gameExeDialogueLayoutRelatedReal(
      unk0, unk1, unk2, unk3, unk4, unk5, unk6,
      yOffset + DIALOGUE_REDESIGN_YOFFSET_SHIFT,
      lineHeight + DIALOGUE_REDESIGN_LINEHEIGHT_SHIFT);
}

static void fixTextureForSplitFont(int& textureId, float& y) {
  bool shouldCorrect = false;
  if (textureId == FIRST_FONT_ID || textureId == FIRST_FONT_ID + 1) {
    textureId = OUTLINE_TEXTURE_ID + 2;
    shouldCorrect = true;
  } else if (textureId == OUTLINE_TEXTURE_ID) { // call from our own code
    shouldCorrect = true;
  }
  if (shouldCorrect && y >= 4080) {
    textureId++;
    y -= 4080;
  }
}

void __cdecl drawGlyphHook(int textureId,
                           float glyphInTextureStartX,
                           float glyphInTextureStartY,
                           float glyphInTextureWidth,
                           float glyphInTextureHeight,
                           float displayStartX, float displayStartY,
                           float displayEndX, float displayEndY,
                           int color, uint32_t opacity) {
  // this hook should be installed only if hasSplitFont is true
  fixTextureForSplitFont(textureId, glyphInTextureStartY);
  gameExeDrawGlyphReal(textureId, glyphInTextureStartX, glyphInTextureStartY,
                       glyphInTextureWidth, glyphInTextureHeight,
                       displayStartX, displayStartY, displayEndX, displayEndY,
                       color, opacity);
}

// used for main text on Tips screen, decay effect for long texts at boundaries of view area
// maskTextureId is always 0x9D = TIPSMASK.DDS
void __cdecl drawGlyphMaskedHook(int fontTextureId, int maskTextureId,
                                 float glyphInTextureStartX,
                                 float glyphInTextureStartY,
                                 float glyphInTextureWidth,
                                 float glyphInTextureHeight,
                                 float glyphInMaskStartX,
                                 float glyphInMaskStartY,
                                 float displayStartX, float displayStartY,
                                 float displayEndX, float displayEndY,
                                 int color, uint32_t opacity) {
  fixTextureForSplitFont(fontTextureId, glyphInTextureStartY);
  gameExeDrawGlyphMaskedReal(fontTextureId, maskTextureId,
                             glyphInTextureStartX, glyphInTextureStartY,
                             glyphInTextureWidth, glyphInTextureHeight,
                             glyphInMaskStartX, glyphInMaskStartY,
                             displayStartX, displayStartY,
                             displayEndX, displayEndY,
                             color, opacity);
}

// used for the backlog, decay effect for boundaries of view area
// same as drawGlyphMaskedHook, glyphInMaskStartX/Y are taken from displayStartX/Y
// maskTextureId is always 0x9B = BLOGMASK.DDS
void __cdecl drawGlyphMasked2Hook(int fontTextureId, int maskTextureId,
                                  float glyphInTextureStartX,
                                  float glyphInTextureStartY,
                                  float glyphInTextureWidth,
                                  float glyphInTextureHeight,
                                  float displayStartX, float displayStartY,
                                  float displayEndX, float displayEndY,
                                  int color, uint32_t opacity) {
  fixTextureForSplitFont(fontTextureId, glyphInTextureStartY);
  gameExeDrawGlyphMasked2Real(fontTextureId, maskTextureId,
                              glyphInTextureStartX, glyphInTextureStartY,
                              glyphInTextureWidth, glyphInTextureHeight,
                              displayStartX, displayStartY,
                              displayEndX, displayEndY,
                              color, opacity);
}

void __cdecl drawDialogueHook(int fontNumber, int pageNumber, uint32_t opacity,
                              int xOffset, int yOffset) {
  DialoguePage_t *page = &gameExeDialoguePages[pageNumber];

  for (int i = 0; i < page->pageLength; i++) {
    if (fontNumber == page->fontNumber[i]) {
      int displayStartX = (page->charDisplayX[i] + xOffset) * COORDS_MULTIPLIER;
      int displayStartY = (page->charDisplayY[i] + yOffset) * COORDS_MULTIPLIER;

      uint32_t _opacity = (page->charDisplayOpacity[i] * opacity) >> 8;

      if (page->charOutlineColor[i] != -1) {
        gameExeDrawGlyph(
            OUTLINE_TEXTURE_ID,
            FONT_CELL_WIDTH * page->glyphCol[i] * COORDS_MULTIPLIER,
            FONT_CELL_HEIGHT * page->glyphRow[i] * COORDS_MULTIPLIER,
            page->glyphOrigWidth[i] * COORDS_MULTIPLIER + (2 * OUTLINE_EXTRA_X),
            page->glyphOrigHeight[i] * COORDS_MULTIPLIER,
            displayStartX - OUTLINE_EXTRA_X, displayStartY,
            displayStartX + (COORDS_MULTIPLIER * page->glyphDisplayWidth[i]) +
                OUTLINE_EXTRA_X,
            displayStartY + (COORDS_MULTIPLIER * page->glyphDisplayHeight[i]),
            page->charOutlineColor[i], _opacity);
      }

      gameExeDrawGlyph(
          fontNumber + FIRST_FONT_ID,
          FONT_CELL_WIDTH * page->glyphCol[i] * COORDS_MULTIPLIER,
          FONT_CELL_HEIGHT * page->glyphRow[i] * COORDS_MULTIPLIER,
          page->glyphOrigWidth[i] * COORDS_MULTIPLIER,
          page->glyphOrigHeight[i] * COORDS_MULTIPLIER, displayStartX,
          displayStartY,
          displayStartX + (COORDS_MULTIPLIER * page->glyphDisplayWidth[i]),
          displayStartY + (COORDS_MULTIPLIER * page->glyphDisplayHeight[i]),
          page->charColor[i], _opacity);
    }
  }
}

void __cdecl drawDialogue2Hook(int fontNumber, int pageNumber,
                               uint32_t opacity) {
  // dunno if this is ever actually called but might as well
  drawDialogueHook(fontNumber, pageNumber, opacity, 0, 0);
}

void semiTokeniseSc3String(const char *sc3string, std::list<StringWord_t> &words,
                           int baseGlyphSize, int lineLength) {
  lineLength -= 2 * PHONE_X_PADDING;

  Sc3_t sc3;
  int sc3evalResult;
  StringWord_t word = {sc3string, NULL, 0, false, false};
  char c;
  while (sc3string != NULL) {
    c = *sc3string;
    switch (c) {
      case -1:
        word.end = sc3string - 1;
        words.push_back(word);
        return;
      case 0:
        word.end = sc3string - 1;
        word.endsWithLinebreak = true;
        words.push_back(word);
        word = {++sc3string, NULL, 0, false, false};
        break;
      case 4:
        sc3.pString = sc3string + 1;
        gameExeSc3Eval(&sc3, &sc3evalResult);
        sc3string = sc3.pString;
        break;
      case 9:
      case 0xB:
      case 0x1E:
        sc3string++;
        break;
      default:
        int glyphId = (uint8_t)sc3string[1] + ((c & 0x7f) << 8);
        int glyphWidth = (baseGlyphSize * widths[glyphId]) / FONT_CELL_WIDTH;
        if (glyphId == GLYPH_ID_FULLWIDTH_SPACE ||
            glyphId == GLYPH_ID_HALFWIDTH_SPACE) {
          word.end = sc3string - 1;
          words.push_back(word);
          word = {sc3string, NULL, (uint16_t)glyphWidth, true, false};
        } else {
          if (word.cost + glyphWidth > lineLength) {
            word.end = sc3string - 1;
            words.push_back(word);
            word = {sc3string, NULL, 0, false, false};
          }
          word.cost += glyphWidth;
        }
        sc3string += 2;
        break;
    }
  }
}

void processSc3TokenList(int xOffset, int yOffset, int lineLength,
                         std::list<StringWord_t> &words, int lineCount,
                         int color, int baseGlyphSize,
                         ProcessedSc3String_t *result, bool measureOnly,
                         float multiplier, int lastLinkNumber,
                         int curLinkNumber, int currentColor,
                         bool truncateExcessWord) {
  Sc3_t sc3;
  int sc3evalResult;

  // some padding, to make things look nicer.
  // note that with more padding (e.g. xOffset += 5, lineLength -= 10) an extra
  // empty line may appear at the start of a mail
  // I'm not 100% sure why that is, and this'll probably come back to bite me
  // later, but whatever...
  xOffset += PHONE_X_PADDING;
  lineLength -= 2 * PHONE_X_PADDING;

  memset(result, 0, sizeof(ProcessedSc3String_t));

  int curProcessedStringLength = 0;
  int curLineLength = 0;

  int spaceCost =
      (widths[GLYPH_ID_FULLWIDTH_SPACE] * baseGlyphSize) / FONT_CELL_WIDTH;

  for (auto it = words.begin(); it != words.end(); it++) {
    if (result->lines >= lineCount) {
      words.erase(words.begin(), it);
      break;
    }
    int wordCost =
        it->cost -
        ((curLineLength == 0 && it->startsWithSpace == true) ? spaceCost : 0);
    if (curLineLength + wordCost > lineLength) {
      if (!(truncateExcessWord && result->lines + 1 == lineCount)) {
        if (curLineLength != 0 && it->startsWithSpace == true)
          wordCost -= spaceCost;
        result->lines++;
        curLineLength = 0;
      }
    }
    if (result->lines >= lineCount) {
      words.erase(words.begin(), it);
      break;
    };

    char c;
    const char *sc3string = (curLineLength == 0 && it->startsWithSpace == true)
                                ? it->start + 2
                                : it->start;
    while (sc3string <= it->end) {
      c = *sc3string;
      switch (c) {
        case -1:
          goto afterWord;
          break;
        case 0:
          goto afterWord;
          break;
        case 4:
          sc3.pString = sc3string + 1;
          gameExeSc3Eval(&sc3, &sc3evalResult);
          sc3string = sc3.pString;
          if (color)
            currentColor = gameExeColors[2 * sc3evalResult];
          else
            currentColor = gameExeColors[2 * sc3evalResult + 1];
          break;
        case 9:
          curLinkNumber = ++lastLinkNumber;
          sc3string++;
          break;
        case 0xB:
          curLinkNumber = NOT_A_LINK;
          sc3string++;
          break;
        case 0x1E:
          sc3string++;
          break;
        default:
          int glyphId = (uint8_t)sc3string[1] + ((c & 0x7f) << 8);
          int i = result->length;

          if (result->lines >= lineCount) break;
          if (curLinkNumber != NOT_A_LINK) {
            result->linkCharCount++;
          }
          uint16_t glyphWidth =
              (baseGlyphSize * widths[glyphId]) / FONT_CELL_WIDTH;
          if (curLineLength + glyphWidth > lineLength) {
            // possible only for the last word if truncateExcessWord
            result->lines++;
            goto afterWord;
          }
          curLineLength += glyphWidth;
          if (!measureOnly) {
            // anything that's part of an array needs to go here, otherwise we
            // get buffer overflows with long mails
            result->linkNumber[i] = curLinkNumber;
            result->glyph[i] = glyphId;
            result->textureStartX[i] =
                FONT_CELL_WIDTH * multiplier * (glyphId % FONT_ROW_LENGTH);
            result->textureStartY[i] =
                FONT_CELL_HEIGHT * multiplier * (glyphId / FONT_ROW_LENGTH);
            result->textureWidth[i] = widths[glyphId] * multiplier;
            result->textureHeight[i] = FONT_CELL_HEIGHT * multiplier;
            result->displayStartX[i] =
                (xOffset + (curLineLength - glyphWidth)) * multiplier;
            result->displayStartY[i] =
                (yOffset + (result->lines * baseGlyphSize)) * multiplier;
            result->displayEndX[i] = (xOffset + curLineLength) * multiplier;
            result->displayEndY[i] =
                (yOffset + ((result->lines + 1) * baseGlyphSize)) * multiplier;
            result->color[i] = currentColor;
          }
          result->length++;
          sc3string += 2;
          break;
      }
    }
  afterWord:
    if (it->endsWithLinebreak) {
      result->lines++;
      curLineLength = 0;
    }
  }

  if (curLineLength == 0) result->lines--;

  result->linkCount = lastLinkNumber + 1;
  result->curColor = currentColor;
  result->curLinkNumber = curLinkNumber;
}

int __cdecl drawPhoneTextHook(int textureId, int xOffset, int yOffset,
                              int lineLength, char *sc3string,
                              int lineSkipCount, int lineDisplayCount,
                              int color, int baseGlyphSize, int opacity) {
  ProcessedSc3String_t str;

  if (!lineLength) lineLength = DEFAULT_LINE_LENGTH;

  bool truncateExcessWord = (lineSkipCount == 0 && lineDisplayCount == 1);

  std::list<StringWord_t> words;
  semiTokeniseSc3String(sc3string, words, baseGlyphSize, lineLength);
  processSc3TokenList(xOffset, yOffset, lineLength, words, lineSkipCount, color,
                      baseGlyphSize, &str, true, COORDS_MULTIPLIER, -1,
                      NOT_A_LINK, color);
  processSc3TokenList(xOffset, yOffset, lineLength, words, lineDisplayCount,
                      color, baseGlyphSize, &str, false, COORDS_MULTIPLIER,
                      str.linkCount - 1, str.curLinkNumber, str.curColor, truncateExcessWord);

  for (int i = 0; i < str.length; i++) {
    gameExeDrawGlyph(textureId, str.textureStartX[i], str.textureStartY[i],
                     str.textureWidth[i], str.textureHeight[i],
                     str.displayStartX[i], str.displayStartY[i],
                     str.displayEndX[i], str.displayEndY[i], str.color[i],
                     opacity);
  }
  return str.lines;
}

int __cdecl getSc3StringDisplayWidthHook(const char *sc3string,
                                         unsigned int maxCharacters,
                                         int baseGlyphSize) {
  if (!maxCharacters) maxCharacters = DEFAULT_MAX_CHARACTERS;
  Sc3_t sc3;
  int sc3evalResult;
  int result = 0;
  int i = 0;
  signed char c;
  while (i <= maxCharacters && (c = *sc3string) != -1) {
    if (c == 4) {
      sc3.pString = sc3string + 1;
      gameExeSc3Eval(&sc3, &sc3evalResult);
      sc3string = sc3.pString;
    } else if (c < 0) {
      int glyphId = (uint8_t)sc3string[1] + ((c & 0x7f) << 8);
      result += (baseGlyphSize * widths[glyphId]) / FONT_CELL_WIDTH;
      i++;
      sc3string += 2;
    }
  }
  return result;
}

int __cdecl getLinksFromSc3StringHook(int xOffset, int yOffset, int lineLength,
                                      char *sc3string, int lineSkipCount,
                                      int lineDisplayCount, int baseGlyphSize,
                                      LinkMetrics_t *result) {
  ProcessedSc3String_t str;

  if (!lineLength) lineLength = DEFAULT_LINE_LENGTH;
  lineLength -= 2; // see the comment in getSc3StringLineCountHook

  std::list<StringWord_t> words;
  semiTokeniseSc3String(sc3string, words, baseGlyphSize, lineLength);
  processSc3TokenList(xOffset, yOffset, lineLength, words, lineSkipCount, 0,
                      baseGlyphSize, &str, true, 1.0f, -1, NOT_A_LINK, 0);
  processSc3TokenList(xOffset, yOffset, lineLength, words, lineDisplayCount, 0,
                      baseGlyphSize, &str, false, 1.0f, str.linkCount - 1,
                      str.curLinkNumber, str.curColor);

  int j = 0;
  int processedChars = 0;
  for (int i = 0; i < str.length; i++) {
    if (str.linkNumber[i] != NOT_A_LINK) {
      // merge consequent characters in one rectangle; the engine has the limit of 20 rectangles per link
      if (j && str.linkNumber[i] == result[j - 1].linkNumber
            && str.displayStartX[i] == result[j - 1].displayX + result[j - 1].displayWidth
            && str.displayStartY[i] == result[j - 1].displayY
            && str.displayEndY[i] == result[j - 1].displayY + result[j - 1].displayHeight)
      {
        result[j - 1].displayWidth = str.displayEndX[i] - result[j - 1].displayX;
      } else {
        result[j].linkNumber = str.linkNumber[i];
        result[j].displayX = str.displayStartX[i];
        result[j].displayY = str.displayStartY[i];
        result[j].displayWidth = str.displayEndX[i] - str.displayStartX[i];
        result[j].displayHeight = str.displayEndY[i] - str.displayStartY[i];
        j++;
      }

      processedChars++;
      if (processedChars >= str.linkCharCount) break;
    }
  }
  return j;
}

// This is also used for @channel threads
int __cdecl drawInteractiveMailHook(int textureId, int xOffset, int yOffset,
                                    signed int lineLength, char *sc3string,
                                    unsigned int lineSkipCount,
                                    unsigned int lineDisplayCount, int color,
                                    unsigned int baseGlyphSize, int opacity,
                                    int unselectedLinkColor,
                                    int selectedLinkColor, int selectedLink) {
  ProcessedSc3String_t str;

  if (!lineLength) lineLength = DEFAULT_LINE_LENGTH;

  std::list<StringWord_t> words;
  semiTokeniseSc3String(sc3string, words, baseGlyphSize, lineLength);
  processSc3TokenList(xOffset, yOffset, lineLength, words, lineSkipCount, color,
                      baseGlyphSize, &str, true, COORDS_MULTIPLIER, -1,
                      NOT_A_LINK, color);
  processSc3TokenList(xOffset, yOffset, lineLength, words, lineDisplayCount,
                      color, baseGlyphSize, &str, false, COORDS_MULTIPLIER,
                      str.linkCount - 1, str.curLinkNumber, str.curColor);

  for (int i = 0; i < str.length; i++) {
    int curColor = str.color[i];
    if (str.linkNumber[i] != NOT_A_LINK) {
      if (str.linkNumber[i] == selectedLink)
        curColor = selectedLinkColor;
      else
        curColor = unselectedLinkColor;

      gameExeDrawGlyph(
          textureId, UNDERLINE_GLYPH_X, UNDERLINE_GLYPH_Y, str.textureWidth[i],
          str.textureHeight[i], str.displayStartX[i], str.displayStartY[i],
          str.displayEndX[i], str.displayEndY[i], curColor, opacity);
    }

    gameExeDrawGlyph(textureId, str.textureStartX[i], str.textureStartY[i],
                     str.textureWidth[i], str.textureHeight[i],
                     str.displayStartX[i], str.displayStartY[i],
                     str.displayEndX[i], str.displayEndY[i], curColor, opacity);
  }
  return str.lines;
}

int __cdecl drawLinkHighlightHook(int xOffset, int yOffset, int lineLength,
                                  char *sc3string, unsigned int lineSkipCount,
                                  unsigned int lineDisplayCount, int color,
                                  unsigned int baseGlyphSize, int opacity,
                                  int selectedLink) {
  ProcessedSc3String_t str;

  if (!lineLength) lineLength = DEFAULT_LINE_LENGTH;

  std::list<StringWord_t> words;
  semiTokeniseSc3String(sc3string, words, baseGlyphSize, lineLength);
  processSc3TokenList(xOffset, yOffset, lineLength, words, lineSkipCount, color,
                      baseGlyphSize, &str, true, COORDS_MULTIPLIER, -1,
                      NOT_A_LINK, color);
  processSc3TokenList(xOffset, yOffset, lineLength, words, lineDisplayCount,
                      color, baseGlyphSize, &str, false, COORDS_MULTIPLIER,
                      str.linkCount - 1, str.curLinkNumber, str.curColor);

  if (selectedLink == NOT_A_LINK) return str.lines;

  for (int i = 0; i < str.length; i++) {
    if (str.linkNumber[i] == selectedLink) {
      gameExeDrawRectangle(str.displayStartX[i], str.displayStartY[i],
                           str.displayEndX[i] - str.displayStartX[i],
                           str.displayEndY[i] - str.displayStartY[i], color,
                           opacity);
    }
  }
  return str.lines;
}

// This is used to process keyboard up/down arrows in mails,
// moving to the next/prev link or scrolling depending on results
void __cdecl getVisibleLinksHook(unsigned lineLength, const char *sc3string,
	unsigned lineSkipCount, unsigned lineDisplayCount,
	unsigned baseGlyphSize, char *result)
{
	ProcessedSc3String_t str;
	if (!lineLength) lineLength = DEFAULT_LINE_LENGTH;
	lineLength -= 2; // see the comment in getSc3StringLineCountHook

	std::list<StringWord_t> words;
	semiTokeniseSc3String(sc3string, words, baseGlyphSize, lineLength);
	processSc3TokenList(0, 0, lineLength, words, lineSkipCount, 0,
		baseGlyphSize, &str, true, COORDS_MULTIPLIER, -1, NOT_A_LINK, 0);
	size_t firstVisibleLink = (str.curLinkNumber == NOT_A_LINK ? str.linkCount : str.curLinkNumber);
	processSc3TokenList(0, 0, lineLength, words, lineDisplayCount, 0,
		baseGlyphSize, &str, true, COORDS_MULTIPLIER, str.linkCount - 1, str.curLinkNumber, str.curColor);
	for (size_t i = firstVisibleLink; i < str.linkCount; i++)
		result[i] = 1;
}

// This is used to set bounds for scrolling
int __cdecl getSc3StringLineCountHook(int lineLength, char *sc3string,
                                      unsigned int baseGlyphSize) {
  ProcessedSc3String_t str;
  if (!lineLength) lineLength = DEFAULT_LINE_LENGTH;

  // The game calls getSc3StringLineCount() with lineLength == 0xFE for mail subject
  // and lineLength == 0x116 for mail body;
  // after that, the game calls drawPhoneTextHook with lineLength == 0xFC and 0x114 respectively.
  // The difference is probably not important for the in-game fixed-width font,
  // but once in a while it changes the layout generated by our algorithm.
  //
  // Note: if they will actually fix the arguments in the game,
  // we will operate with less-than-needed lines, and
  // this code could occasionally generate an extra empty line;
  // this is acceptable unlike an inaccessible last line that happens
  // if we operate with greater-than-needed lines.
  lineLength -= 2;

  std::list<StringWord_t> words;
  semiTokeniseSc3String(sc3string, words, baseGlyphSize, lineLength);
  processSc3TokenList(0, 0, lineLength, words, LINECOUNT_DISABLE_OR_ERROR, 0,
                      baseGlyphSize, &str, true, 1.0f, -1, NOT_A_LINK, 0);
  return str.lines + 1;
}
}