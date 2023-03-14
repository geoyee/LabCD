# qt
QT += core gui widgets
TEMPLATE = app
TARGET = LabCD
DESTDIR = ./x64/Release
CONFIG += release
MOC_DIR += GeneratedFiles/$(ConfigurationName)
OBJECTS_DIR += release
UI_DIR += GeneratedFiles
RCC_DIR += GeneratedFiles
include(LabCD.pri)
TRANSLATIONS += i18n/English.ts

# third-party dependency
unix|win32: LIBS += -L$$PWD/depends/lib/ -lgdal_i
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/depends/lib/ -ljsoncpp_static
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/depends/lib/ -ljsoncpp_staticd
else:unix: LIBS += -L$$PWD/depends/lib/ -ljsoncpp_static
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/depends/lib/ -lopencv_world455
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/depends/lib/ -lopencv_world455d
else:unix: LIBS += -L$$PWD/depends/lib/ -lopencv_world455
INCLUDEPATH += $$PWD/depends
INCLUDEPATH += $$PWD/depends/include
DEPENDPATH += $$PWD/depends/include
