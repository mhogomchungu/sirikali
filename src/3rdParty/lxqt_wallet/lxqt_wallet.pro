#-------------------------------------------------
#
# Project created by QtCreator 2013-08-11T17:17:44
#
#-------------------------------------------------

QT       += core gui

TARGET = lxqt_wallet

TEMPLATE = app

INCLUDEPATH += /home/local/KDE4/include /usr/include /home/ink/src/lxqt_wallet-build /home/ink/src/wallet_manager /usr/include/ /usr/include/libsecret-1 /usr/include/libsecret-1/libsecret /usr/include/glib-2.0 /usr/lib/glib-2.0/include

LIBS += -lgcrypt -lkwalletbackend -L/home/local/KDE4/lib -lsecret-1

OTHER_FILES += \
    frontend/README \
    frontend/CMakeLists.txt \
    backend/README \
    backend/CMakeLists.txt \
    test/README \
    frontend/secret_service/CMakeLists.txt

HEADERS += \
    frontend/password_dialog.h \
    frontend/task.h \
    frontend/lxqt_wallet.h \
    frontend/lxqt_kwallet.h \
    frontend/lxqt_internal_wallet.h \
    frontend/changepassworddialog.h \
    backend/lxqtwallet.h \
    /home/ink/src/build-lxde-wallet/mainwindow.h \
    frontend/lxqt_secret_service.h \
    frontend/secret_service.h

SOURCES += \
    frontend/password_dialog.cpp \
    frontend/task.cpp \
    frontend/lxqt_wallet.cpp \
    frontend/lxqt_kwallet.cpp \
    frontend/lxqt_internal_wallet.cpp \
    frontend/changepassworddialog.cpp \
    backend/lxqtwallet.c \
    /home/ink/src/build-lxde-wallet/mainwindow.cpp \
    /home/ink/src/build-lxde-wallet/main.cpp \
    frontend/lxqt_secret_service.cpp \
    frontend/secret_service.c

FORMS += \
    frontend/password_dialog.ui \
    frontend/changepassworddialog.ui \
    /home/ink/src/build-lxde-wallet/mainwindow.ui

    TRANSLATIONS += translations/translations.ts
    TRANSLATIONS += translations/lxqt-wallet_de_DE.ts
    TRANSLATIONS += translations/lxqt-wallet_gr_GR.ts
    TRANSLATIONS += translations/lxqt-wallet_nl_NL.ts
    TRANSLATIONS += translations/lxqt-wallet_fr_FR.ts
    TRANSLATIONS += translations/lxqt-wallet_es_ES.ts
    TRANSLATIONS += translations/lxqt-wallet_pl_PL.ts
