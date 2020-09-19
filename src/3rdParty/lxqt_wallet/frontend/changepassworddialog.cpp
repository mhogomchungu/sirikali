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

#include "changepassworddialog.h"
#include "ui_changepassworddialog.h"

#include <utility>

namespace Task = LXQt::Wallet::Task ;

LXQt::Wallet::changePassWordDialog::changePassWordDialog(QWidget *parent,
        const QString &walletName,
        const QString &applicationName) :
    QDialog(parent),
    m_ui(new Ui::changePassWordDialog),
    m_walletName(walletName),
    m_applicationName(applicationName)
{
    m_ui->setupUi(this) ;

    this->setFixedSize(this->size()) ;

    this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint) ;

    if (parent)
    {
        this->setWindowIcon(parent->windowIcon()) ;
    }

    connect(m_ui->pushButtonCancel, SIGNAL(clicked()), this, SLOT(cancel())) ;

    m_ui->pushButtonOK->setVisible(false) ;
    m_ui->textEdit->setVisible(false) ;
    m_ui->textEdit_2->setVisible(false) ;

    this->installEventFilter(this) ;
}

bool LXQt::Wallet::changePassWordDialog::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == this)
    {
        if (event->type() == QEvent::KeyPress)
        {
            auto keyEvent = static_cast< QKeyEvent * >(event) ;

            if (keyEvent->key() == Qt::Key_Escape)
            {
                this->HideUI() ;

                return true ;
            }
        }
    }

    return false ;
}

void LXQt::Wallet::changePassWordDialog::HideUI()
{
    this->hide() ;

    this->deleteLater() ;
}

void LXQt::Wallet::changePassWordDialog::changeShowUI(changeFunction && change)
{
    m_change = std::move(change) ;

    m_banner = m_ui->textEdit->toHtml().arg(m_applicationName, m_walletName) ;
    m_ui->label->setText(m_banner) ;

    connect(m_ui->pushButtonChange, SIGNAL(clicked()), this, SLOT(change())) ;
    connect(m_ui->pushButtonOK, SIGNAL(clicked()), this, SLOT(ok())) ;

    this->show() ;
    this->raise() ;
    this->activateWindow() ;
}

void LXQt::Wallet::changePassWordDialog::createShowUI(createFunction && create)
{
    m_create = std::move(create) ;

    this->setWindowTitle(tr("Create a new wallet")) ;

    m_ui->pushButtonChange->setText(tr("Create")) ;

    connect(m_ui->pushButtonChange, SIGNAL(clicked()), this, SLOT(create())) ;
    connect(m_ui->pushButtonOK, SIGNAL(clicked()), this, SLOT(ok_1())) ;

    m_banner = m_ui->textEdit_2->toHtml().arg(m_applicationName, m_walletName) ;
    m_ui->label->setText(m_banner) ;

    m_ui->label_2->setEnabled(false) ;
    m_ui->lineEditCurrentPassWord->setEnabled(false) ;

    this->show() ;
    this->raise() ;
    this->activateWindow() ;
}

LXQt::Wallet::changePassWordDialog::~changePassWordDialog()
{
    delete m_ui ;
}

void LXQt::Wallet::changePassWordDialog::create()
{
    if (m_ui->lineEditNewPassWord->text() == m_ui->lineEditNewPassWord_2->text())
    {
        m_create(m_ui->lineEditNewPassWord->text(), true) ;
        this->HideUI() ;
    }
    else
    {
        m_ui->label->setText(tr("Passwords do not match")) ;
        m_ui->pushButtonOK->setVisible(true) ;
        m_ui->pushButtonCancel->setVisible(false) ;
        m_ui->pushButtonChange->setVisible(false) ;

        m_ui->lineEditCurrentPassWord->setEnabled(false) ;
        m_ui->lineEditNewPassWord->setEnabled(false) ;
        m_ui->lineEditNewPassWord_2->setEnabled(false) ;
        m_ui->label->setEnabled(true) ;
        m_ui->label_2->setEnabled(false) ;
        m_ui->label_2->setEnabled(false) ;
        m_ui->label_3->setEnabled(false) ;
        m_ui->label_4->setEnabled(false) ;
    }
}

void LXQt::Wallet::changePassWordDialog::change()
{
    m_ui->lineEditCurrentPassWord->setEnabled(false) ;
    m_ui->lineEditNewPassWord->setEnabled(false) ;
    m_ui->lineEditNewPassWord_2->setEnabled(false) ;
    m_ui->pushButtonChange->setEnabled(false) ;
    m_ui->pushButtonCancel->setEnabled(false) ;
    m_ui->label->setEnabled(true) ;
    m_ui->label_2->setEnabled(false) ;
    m_ui->label_2->setEnabled(false) ;
    m_ui->label_3->setEnabled(false) ;
    m_ui->label_4->setEnabled(false) ;

    auto n = m_ui->lineEditNewPassWord->text() ;

    if (n == m_ui->lineEditNewPassWord_2->text())
    {
        auto c = m_ui->lineEditCurrentPassWord->text() ;

        auto m = m_change(c, n, false) ;

        if (m.failedToUnlock)
        {
            m_ui->pushButtonChange->setEnabled(true) ;
            m_ui->pushButtonCancel->setEnabled(true) ;
            m_ui->label->setText(tr("Wallet could not be opened with the presented key")) ;
            m_ui->pushButtonCancel->setVisible(false) ;
            m_ui->pushButtonChange->setVisible(false) ;
            m_ui->pushButtonOK->setVisible(true) ;
            m_ui->pushButtonOK->setFocus() ;

        }
        else if (m.failedToChange)
        {
            m_ui->pushButtonChange->setEnabled(true) ;
            m_ui->pushButtonCancel->setEnabled(true) ;
            m_ui->label->setText(tr("Wallet password could not be changed")) ;
            m_ui->pushButtonCancel->setVisible(false) ;
            m_ui->pushButtonChange->setVisible(false) ;
            m_ui->pushButtonOK->setVisible(true) ;
            m_ui->pushButtonOK->setFocus() ;
        }
        else
        {
            this->HideUI() ;
        }
    }
    else
    {
        m_ui->label->setText(tr("New passwords do not match")) ;
        m_ui->pushButtonOK->setVisible(true) ;
        m_ui->pushButtonCancel->setVisible(false) ;
        m_ui->pushButtonChange->setVisible(false) ;
        m_ui->pushButtonOK->setFocus() ;
    }
}

void LXQt::Wallet::changePassWordDialog::cancel()
{
    m_change(QString(), QString(), true) ;
    m_create(QString(), false) ;
    this->HideUI() ;
}

void LXQt::Wallet::changePassWordDialog::ok()
{
    m_ui->lineEditCurrentPassWord->setEnabled(true) ;
    m_ui->lineEditCurrentPassWord->clear() ;
    m_ui->lineEditCurrentPassWord->setFocus() ;
    m_ui->lineEditNewPassWord->setEnabled(true) ;
    m_ui->lineEditNewPassWord_2->setEnabled(true) ;
    m_ui->label->setEnabled(true) ;
    m_ui->label_2->setEnabled(true) ;
    m_ui->label_2->setEnabled(true) ;
    m_ui->label_3->setEnabled(true) ;
    m_ui->label_4->setEnabled(true) ;
    m_ui->pushButtonOK->setVisible(false) ;
    m_ui->pushButtonCancel->setVisible(true) ;
    m_ui->pushButtonChange->setVisible(true) ;
    m_ui->pushButtonCancel->setEnabled(true) ;
    m_ui->pushButtonChange->setEnabled(true) ;
    m_ui->label->setText(m_banner) ;
}

void LXQt::Wallet::changePassWordDialog::ok_1()
{
    if (m_ui->label->text() == tr("Passwords do not match"))
    {
        m_ui->lineEditCurrentPassWord->setEnabled(false) ;
        m_ui->lineEditNewPassWord->setEnabled(true) ;
        m_ui->lineEditNewPassWord->setFocus() ;
        m_ui->lineEditNewPassWord_2->setEnabled(true) ;
        m_ui->label->setEnabled(true) ;
        m_ui->label_2->setEnabled(true) ;
        m_ui->label_2->setEnabled(true) ;
        m_ui->label_3->setEnabled(true) ;
        m_ui->label_4->setEnabled(true) ;
        m_ui->pushButtonOK->setVisible(false) ;
        m_ui->pushButtonCancel->setVisible(true) ;
        m_ui->pushButtonChange->setVisible(true) ;
        m_ui->pushButtonCancel->setEnabled(true) ;
        m_ui->pushButtonChange->setEnabled(true) ;
        m_ui->label->setText(m_banner) ;
    }
}

void LXQt::Wallet::changePassWordDialog::closeEvent(QCloseEvent *e)
{
    e->ignore() ;
    this->cancel() ;
}
