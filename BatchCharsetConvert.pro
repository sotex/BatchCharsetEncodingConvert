#-------------------------------------------------
#
# Project created by QtCreator 2016-12-05T15:01:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BatchCharsetConvert
TEMPLATE = app

INCLUDEPATH += $$PWD/universalchardet/src/base/


SOURCES += code/main.cpp\
        code/Widget.cpp \
    universalchardet/src/base/CharDistribution.cpp \
    universalchardet/src/base/JpCntx.cpp \
    universalchardet/src/base/nsCharSetProber.cpp \
    universalchardet/src/base/nsEscCharsetProber.cpp \
    universalchardet/src/base/nsEscSM.cpp \
    universalchardet/src/base/nsEUCJPProber.cpp \
    universalchardet/src/base/nsLatin1Prober.cpp \
    universalchardet/src/base/nsMBCSGroupProber.cpp \
    universalchardet/src/base/nsMBCSSM.cpp \
    universalchardet/src/base/nsSJISProber.cpp \
    universalchardet/src/base/nsUniversalDetector.cpp \
    universalchardet/src/base/nsUTF8Prober.cpp \
    universalchardet/src/base/LangBulgarianModel.cpp \
    universalchardet/src/base/LangCyrillicModel.cpp \
    universalchardet/src/base/LangGreekModel.cpp \
    universalchardet/src/base/LangHebrewModel.cpp \
    universalchardet/src/base/LangHungarianModel.cpp \
    universalchardet/src/base/LangThaiModel.cpp \
    universalchardet/src/base/nsBig5Prober.cpp \
    universalchardet/src/base/nsEUCKRProber.cpp \
    universalchardet/src/base/nsEUCTWProber.cpp \
    universalchardet/src/base/nsGB2312Prober.cpp \
    universalchardet/src/base/nsHebrewProber.cpp \
    universalchardet/src/base/nsSBCharSetProber.cpp \
    universalchardet/src/base/nsSBCSGroupProber.cpp \
    code/CharsetDetector.cpp \
    code/ComboxDelegate.cpp

HEADERS  += \
    code/Widget.hpp \
    universalchardet/src/base/nsUTF8Prober.h \
    universalchardet/src/base/CharDistribution.h \
    universalchardet/src/base/JpCntx.h \
    universalchardet/src/base/nsCharSetProber.h \
    universalchardet/src/base/nsCodingStateMachine.h \
    universalchardet/src/base/nsEscCharsetProber.h \
    universalchardet/src/base/nsEUCJPProber.h \
    universalchardet/src/base/nsLatin1Prober.h \
    universalchardet/src/base/nsMBCSGroupProber.h \
    universalchardet/src/base/nsPkgInt.h \
    universalchardet/src/base/nsSJISProber.h \
    universalchardet/src/base/nsUniversalDetector.h \
    universalchardet/src/base/nsBig5Prober.h \
    universalchardet/src/base/nscore.h \
    universalchardet/src/base/nsEUCKRProber.h \
    universalchardet/src/base/nsEUCTWProber.h \
    universalchardet/src/base/nsGB2312Prober.h \
    universalchardet/src/base/nsHebrewProber.h \
    universalchardet/src/base/nsSBCharSetProber.h \
    universalchardet/src/base/nsSBCSGroupProber.h \
    universalchardet/src/base/prmem.h \
    code/CharsetDetector.hpp \
    code/ComboxDelegate.hpp

DISTFILES += \
    universalchardet/src/base/Big5Freq.tab \
    universalchardet/src/base/EUCKRFreq.tab \
    universalchardet/src/base/EUCTWFreq.tab \
    universalchardet/src/base/GB2312Freq.tab \
    universalchardet/src/base/JISFreq.tab
