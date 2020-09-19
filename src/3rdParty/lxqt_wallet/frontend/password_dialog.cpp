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

#include "password_dialog.h"
#include "ui_password_dialog.h"

LXQt::Wallet::password_dialog::password_dialog(QWidget *parent,
        const QString &walletName,
        const QString &appName,
        std::function< void(const QString &) > && p,
        std::function< void() > && q,
        std::function< void(bool) > * z) :
    QDialog(parent), m_ui(new Ui::password_dialog), m_password(std::move(p)), m_cancel(std::move(q))
{
    m_ui->setupUi(this);

    this->setFixedSize(this->size());

    this->setWindowFlags(this->windowFlags()|Qt::WindowStaysOnTopHint);

    if (parent)
    {
        this->setWindowIcon(parent->windowIcon());
    }

    connect(m_ui->pushButtonSend, SIGNAL(clicked()), this, SLOT(pbSend()));
    connect(m_ui->pushButtonCancel, SIGNAL(clicked()), this, SLOT(pbCancel()));
    connect(m_ui->pushButtonOK_2, SIGNAL(clicked()), this, SLOT(pbOK_2()));

    m_ui->pushButtonOK_2->setVisible(false);
    m_ui->textEdit_2->setVisible(false);
    m_ui->textEdit->setVisible(false);
    m_ui->pushButtonOK->setVisible(false);

    m_banner = m_ui->textEdit->toHtml().arg(appName, walletName);
    m_ui->labelWalletDoesNotExist->setVisible(false);
    m_ui->labelHeader->setText(m_banner);

    *z = [this](bool correctPassword)
    {
        if (correctPassword)
        {
            this->HideUI();
        }
        else
        {
            m_ui->labelHeader->setText(tr("Wallet could not be opened with the presented key"));
            m_ui->textEdit->setVisible(false);
            m_ui->labelWalletDoesNotExist->setVisible(false);
            m_ui->labelHeader->setVisible(true);
            m_ui->lineEditKey->setVisible(true);
            m_ui->lineEditKey->setEnabled(false);
            m_ui->pushButtonSend->setVisible(false);
            m_ui->pushButtonCancel->setVisible(false);
            m_ui->pushButtonOK->setVisible(false);
            m_ui->pushButtonOK_2->setVisible(true);
            m_ui->pushButtonOK_2->setFocus();
        }
    };

    this->installEventFilter(this);

    this->show();
}

bool LXQt::Wallet::password_dialog::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == this)
    {
        if (event->type() == QEvent::KeyPress)
        {
            auto keyEvent = static_cast< QKeyEvent * >(event);

            if (keyEvent->key() == Qt::Key_Escape)
            {
                this->HideUI();

                return true;
            }
        }
    }

    return false;
}

LXQt::Wallet::password_dialog::~password_dialog()
{
    delete m_ui;
}

void LXQt::Wallet::password_dialog::pbSend()
{
    m_ui->textEdit->setEnabled(false);
    m_ui->labelHeader->setEnabled(true);
    m_ui->lineEditKey->setEnabled(false);
    m_ui->lineEditKey->setEnabled(false);
    m_ui->pushButtonSend->setEnabled(false);
    m_ui->pushButtonCancel->setEnabled(false);
    m_ui->pushButtonOK->setEnabled(false);
    m_ui->pushButtonOK_2->setEnabled(true);

    m_password(m_ui->lineEditKey->text());
}

void LXQt::Wallet::password_dialog::pbCancel()
{
    m_cancel();
    this->HideUI();
}

void LXQt::Wallet::password_dialog::pbOK()
{
    this->HideUI();
}

void LXQt::Wallet::password_dialog::pbOK_2()
{
    m_ui->labelHeader->setText(m_banner);
    m_ui->textEdit->setVisible(false);
    m_ui->labelWalletDoesNotExist->setVisible(false);
    m_ui->labelHeader->setVisible(true);
    m_ui->lineEditKey->setVisible(true);
    m_ui->lineEditKey->setEnabled(true);
    m_ui->pushButtonSend->setVisible(true);
    m_ui->pushButtonCancel->setVisible(true);
    m_ui->pushButtonOK->setVisible(false);
    m_ui->pushButtonOK_2->setVisible(false);
    m_ui->lineEditKey->clear();
    m_ui->lineEditKey->setFocus();
    m_ui->textEdit->setEnabled(true);
    m_ui->labelHeader->setEnabled(true);
    m_ui->lineEditKey->setEnabled(true);
    m_ui->lineEditKey->setEnabled(true);
    m_ui->pushButtonSend->setEnabled(true);
    m_ui->pushButtonCancel->setEnabled(true);
    m_ui->pushButtonOK->setEnabled(true);
    m_ui->pushButtonOK_2->setEnabled(true);
}

void LXQt::Wallet::password_dialog::HideUI()
{
    this->hide();
    this->deleteLater();
}

void LXQt::Wallet::password_dialog::closeEvent(QCloseEvent *e)
{
    e->ignore();
    this->HideUI();
}
