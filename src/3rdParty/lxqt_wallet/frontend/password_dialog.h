/*
 * copyright: 2013-2015
 * name : Francis Banyikwa
 * email: mhogomchungu@gmail.com
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef PASSWORD_DIALOG_H
#define PASSWORD_DIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include <QString>
#include <QMessageBox>
#include <QEvent>
#include <QKeyEvent>

#include <functional>
#include <utility>

namespace Ui
{
class password_dialog;
}

namespace LXQt
{

namespace Wallet
{

class password_dialog : public QDialog
{
    Q_OBJECT
public:
    static password_dialog &instance(QWidget *w,
                                     const QString &walletName,
                                     const QString &appName,
                                     std::function< void(const QString &) > && p,
                                     std::function< void() > q,
                                     std::function< void(bool) > * z)
    {
        return *(new password_dialog(w, walletName, appName, std::move(p), std::move(q), z));
    }
    explicit password_dialog(QWidget *parent,
                             const QString &walletName,
                             const QString &appName,
                             std::function< void(const QString &) > && ,
                             std::function< void() > && ,
                             std::function< void(bool) > *);
    ~password_dialog();
private slots:
    void pbSend(void);
    void pbCancel(void);
    void pbOK(void);
    void pbOK_2(void);
private:
    void HideUI(void);
    void closeEvent(QCloseEvent *);
    bool eventFilter(QObject *watched, QEvent *event);
    Ui::password_dialog *m_ui;
    QString m_banner;

    std::function< void(const QString &) > m_password;
    std::function< void() > m_cancel;
};

}

}

#endif // PASSWORD_DIALOG_H
