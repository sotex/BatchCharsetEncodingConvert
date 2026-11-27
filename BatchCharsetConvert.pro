#-------------------------------------------------
#
# Project created by QtCreator 2016-12-05T15:01:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BatchCharsetConvert
TEMPLATE = app

# Application icon (Windows)
win32:RC_FILE = app.rc

INCLUDEPATH += $$PWD/universalchardet


# --- universalchardet sources ---
UCHARDET_SOURCES = \
    $$PWD/universalchardet/uchardet.cpp \
    $$PWD/universalchardet/nsUniversalDetector.cpp \
    $$PWD/universalchardet/nsCharSetProber.cpp \
    $$PWD/universalchardet/nsSBCharSetProber.cpp \
    $$PWD/universalchardet/nsSBCSGroupProber.cpp \
    $$PWD/universalchardet/nsMBCSGroupProber.cpp \
    $$PWD/universalchardet/nsEscCharsetProber.cpp \
    $$PWD/universalchardet/nsEscSM.cpp \
    $$PWD/universalchardet/nsMBCSSM.cpp \
    $$PWD/universalchardet/nsBig5Prober.cpp \
    $$PWD/universalchardet/nsEUCJPProber.cpp \
    $$PWD/universalchardet/nsEUCKRProber.cpp \
    $$PWD/universalchardet/nsEUCTWProber.cpp \
    $$PWD/universalchardet/nsGB2312Prober.cpp \
    $$PWD/universalchardet/nsSJISProber.cpp \
    $$PWD/universalchardet/nsUTF8Prober.cpp \
    $$PWD/universalchardet/nsLatin1Prober.cpp \
    $$PWD/universalchardet/nsHebrewProber.cpp \
    $$PWD/universalchardet/CharDistribution.cpp \
    $$PWD/universalchardet/JpCntx.cpp \
    $$PWD/universalchardet/LangModels/LangArabicModel.cpp \
    $$PWD/universalchardet/LangModels/LangBulgarianModel.cpp \
    $$PWD/universalchardet/LangModels/LangCroatianModel.cpp \
    $$PWD/universalchardet/LangModels/LangCzechModel.cpp \
    $$PWD/universalchardet/LangModels/LangDanishModel.cpp \
    $$PWD/universalchardet/LangModels/LangEsperantoModel.cpp \
    $$PWD/universalchardet/LangModels/LangEstonianModel.cpp \
    $$PWD/universalchardet/LangModels/LangFinnishModel.cpp \
    $$PWD/universalchardet/LangModels/LangFrenchModel.cpp \
    $$PWD/universalchardet/LangModels/LangGermanModel.cpp \
    $$PWD/universalchardet/LangModels/LangGreekModel.cpp \
    $$PWD/universalchardet/LangModels/LangHebrewModel.cpp \
    $$PWD/universalchardet/LangModels/LangHungarianModel.cpp \
    $$PWD/universalchardet/LangModels/LangIrishModel.cpp \
    $$PWD/universalchardet/LangModels/LangItalianModel.cpp \
    $$PWD/universalchardet/LangModels/LangLatvianModel.cpp \
    $$PWD/universalchardet/LangModels/LangLithuanianModel.cpp \
    $$PWD/universalchardet/LangModels/LangMalteseModel.cpp \
    $$PWD/universalchardet/LangModels/LangNorwegianModel.cpp \
    $$PWD/universalchardet/LangModels/LangPolishModel.cpp \
    $$PWD/universalchardet/LangModels/LangPortugueseModel.cpp \
    $$PWD/universalchardet/LangModels/LangRomanianModel.cpp \
    $$PWD/universalchardet/LangModels/LangRussianModel.cpp \
    $$PWD/universalchardet/LangModels/LangSlovakModel.cpp \
    $$PWD/universalchardet/LangModels/LangSloveneModel.cpp \
    $$PWD/universalchardet/LangModels/LangSpanishModel.cpp \
    $$PWD/universalchardet/LangModels/LangSwedishModel.cpp \
    $$PWD/universalchardet/LangModels/LangThaiModel.cpp \
    $$PWD/universalchardet/LangModels/LangTurkishModel.cpp \
    $$PWD/universalchardet/LangModels/LangVietnameseModel.cpp

SOURCES += code/main.cpp \
    code/Widget.cpp \
    code/FileScanWorker.cpp \
    code/EncodingDetectWorker.cpp \
    code/FileConvertWorker.cpp \
    code/CharsetDetector.cpp \
    code/ComboxDelegate.cpp \
    $$UCHARDET_SOURCES

HEADERS += \
    code/Widget.hpp \
    code/FileScanWorker.hpp \
    code/EncodingDetectWorker.hpp \
    code/FileConvertWorker.hpp \
    code/CharsetDetector.hpp \
    code/ComboxDelegate.hpp \
    $$PWD/universalchardet/uchardet.h \
    $$PWD/universalchardet/nsUniversalDetector.h \
    $$PWD/universalchardet/nsCharSetProber.h \
    $$PWD/universalchardet/nsSBCharSetProber.h \
    $$PWD/universalchardet/nsSBCSGroupProber.h \
    $$PWD/universalchardet/nsMBCSGroupProber.h \
    $$PWD/universalchardet/nsEscCharsetProber.h \
    $$PWD/universalchardet/nsBig5Prober.h \
    $$PWD/universalchardet/nsEUCJPProber.h \
    $$PWD/universalchardet/nsEUCKRProber.h \
    $$PWD/universalchardet/nsEUCTWProber.h \
    $$PWD/universalchardet/nsGB2312Prober.h \
    $$PWD/universalchardet/nsSJISProber.h \
    $$PWD/universalchardet/nsUTF8Prober.h \
    $$PWD/universalchardet/nsLatin1Prober.h \
    $$PWD/universalchardet/nsHebrewProber.h \
    $$PWD/universalchardet/nsCodingStateMachine.h \
    $$PWD/universalchardet/nscore.h \
    $$PWD/universalchardet/nsPkgInt.h \
    $$PWD/universalchardet/prmem.h \
    $$PWD/universalchardet/CharDistribution.h \
    $$PWD/universalchardet/JpCntx.h

# Translations
TRANSLATIONS = translations/zh_CN.ts

# Generate .qm from .ts automatically
translations.commands = lrelease ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
translations.input = TRANSLATIONS
translations.output = translations/${QMAKE_FILE_BASE}.qm
translations.CONFIG = no_link
QMAKE_EXTRA_COMPILERS += translations

RESOURCES = translations.qrc
