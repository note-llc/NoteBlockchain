// Copyright (c) 2011-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/mnemonicdialog.h>
#include <qt/forms/ui_mnemonicdialog.h>

#include <qt/guiutil.h>
#include <qt/walletmodel.h>

#include <crypto/bip39.h>

#include <QApplication>
#include <QClipboard>
#include <QMessageBox>
#include <QPushButton>

MnemonicDialog::MnemonicDialog(Mode mode, WalletModel *model, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MnemonicDialog),
    walletModel(model),
    dialogMode(mode),
    mnemonicConfirmed(false)
{
    ui->setupUi(this);
    setupUI();
    
    // Connect signals
    connect(ui->copyButton, &QPushButton::clicked, this, &MnemonicDialog::on_copyButton_clicked);
    connect(ui->mnemonicText, &QPlainTextEdit::textChanged, this, &MnemonicDialog::on_mnemonicText_textChanged);
    connect(ui->passphraseEdit, &QLineEdit::textChanged, this, &MnemonicDialog::on_passphraseEdit_textChanged);
    connect(ui->confirmCheckBox, &QCheckBox::stateChanged, this, &MnemonicDialog::on_confirmCheckBox_stateChanged);
    
    updateButtonStates();
}

MnemonicDialog::~MnemonicDialog()
{
    delete ui;
}

void MnemonicDialog::setupUI()
{
    switch (dialogMode) {
        case Display:
            setWindowTitle(tr("Wallet Mnemonic Phrase"));
            ui->titleLabel->setText(tr("Your Wallet's Mnemonic Phrase"));
            ui->instructionLabel->setText(
                tr("This is your wallet's BIP39 mnemonic phrase. "
                   "Write it down and store it securely. "
                   "Anyone with this phrase can access your funds.")
            );
            ui->mnemonicText->setReadOnly(true);
            ui->passphraseLabel->setVisible(false);
            ui->passphraseEdit->setVisible(false);
            ui->validateButton->setVisible(false);
            ui->confirmCheckBox->setVisible(false);
            ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Close"));
            break;
            
        case Import:
            setWindowTitle(tr("Import Mnemonic Phrase"));
            ui->titleLabel->setText(tr("Import Wallet from Mnemonic"));
            ui->instructionLabel->setText(
                tr("Enter your BIP39 mnemonic phrase to restore your wallet. "
                   "You can optionally enter a passphrase if one was used.")
            );
            ui->mnemonicText->setReadOnly(false);
            ui->mnemonicText->setPlaceholderText(tr("Enter your 12 or 24 word mnemonic phrase..."));
            ui->passphraseLabel->setVisible(true);
            ui->passphraseEdit->setVisible(true);
            ui->passphraseEdit->setPlaceholderText(tr("Optional passphrase"));
            ui->validateButton->setVisible(true);
            ui->confirmCheckBox->setVisible(false);
            ui->copyButton->setVisible(false);
            ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Import"));
            ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
            break;
            
        case Generate:
            setWindowTitle(tr("New Wallet Mnemonic"));
            ui->titleLabel->setText(tr("Your New Wallet's Mnemonic Phrase"));
            ui->instructionLabel->setText(
                tr("<b>IMPORTANT: Write down these words in order and store them securely!</b><br><br>"
                   "This mnemonic phrase is the ONLY way to recover your wallet if you lose access. "
                   "Anyone with this phrase can access your funds. Never share it with anyone.")
            );
            ui->mnemonicText->setReadOnly(true);
            ui->passphraseLabel->setVisible(false);
            ui->passphraseEdit->setVisible(false);
            ui->validateButton->setVisible(false);
            ui->confirmCheckBox->setVisible(true);
            ui->confirmCheckBox->setText(tr("I have written down my mnemonic phrase"));
            ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Continue"));
            ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
            break;
    }
    
    // Set warning style
    ui->warningFrame->setStyleSheet(
        "QFrame { background-color: #fff3cd; border: 2px solid #ffc107; border-radius: 5px; padding: 10px; }"
    );
    ui->warningLabel->setStyleSheet("QLabel { color: #856404; font-weight: bold; }");
    ui->warningLabel->setText(
        tr("⚠ WARNING: Never share your mnemonic phrase with anyone! "
           "Store it offline in a secure location.")
    );
}

QString MnemonicDialog::getMnemonic() const
{
    return ui->mnemonicText->toPlainText().trimmed();
}

QString MnemonicDialog::getPassphrase() const
{
    return ui->passphraseEdit->text();
}

void MnemonicDialog::setMnemonic(const QString &mnemonic)
{
    ui->mnemonicText->setPlainText(mnemonic);
}

bool MnemonicDialog::validateMnemonic(const QString &mnemonic)
{
    if (mnemonic.isEmpty()) {
        return false;
    }
    
    // Use BIP39 validation
    return BIP39::ValidateMnemonic(mnemonic.toStdString());
}

void MnemonicDialog::on_copyButton_clicked()
{
    QApplication::clipboard()->setText(getMnemonic());
    
    QMessageBox::information(this, tr("Copied"),
        tr("Mnemonic phrase copied to clipboard.\n\n"
           "Remember to clear your clipboard after pasting!"));
}

void MnemonicDialog::on_validateButton_clicked()
{
    QString mnemonic = getMnemonic();
    
    if (validateMnemonic(mnemonic)) {
        QMessageBox::information(this, tr("Valid Mnemonic"),
            tr("The mnemonic phrase is valid!"));
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    } else {
        QMessageBox::warning(this, tr("Invalid Mnemonic"),
            tr("The mnemonic phrase is invalid. Please check your words and try again."));
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
}

void MnemonicDialog::on_mnemonicText_textChanged()
{
    updateButtonStates();
}

void MnemonicDialog::on_passphraseEdit_textChanged(const QString &text)
{
    Q_UNUSED(text);
    updateButtonStates();
}

void MnemonicDialog::on_confirmCheckBox_stateChanged(int state)
{
    mnemonicConfirmed = (state == Qt::Checked);
    updateButtonStates();
}

void MnemonicDialog::updateButtonStates()
{
    switch (dialogMode) {
        case Display:
            // Always enabled for display mode
            ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
            break;
            
        case Import:
            // Enable OK button only if mnemonic is valid
            {
                QString mnemonic = getMnemonic();
                bool isValid = !mnemonic.isEmpty() && validateMnemonic(mnemonic);
                ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isValid);
                ui->validateButton->setEnabled(!mnemonic.isEmpty());
            }
            break;
            
        case Generate:
            // Enable OK button only if user confirmed backup
            ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(mnemonicConfirmed);
            break;
    }
}

void MnemonicDialog::accept()
{
    if (dialogMode == Import) {
        // Validate one more time before accepting
        if (!validateMnemonic(getMnemonic())) {
            QMessageBox::warning(this, tr("Invalid Mnemonic"),
                tr("Please enter a valid mnemonic phrase."));
            return;
        }
        
        // Confirm import action
        QMessageBox::StandardButton reply = QMessageBox::question(this,
            tr("Confirm Import"),
            tr("Are you sure you want to import this mnemonic phrase?\n\n"
               "This will replace your current wallet!"),
            QMessageBox::Yes | QMessageBox::No);
            
        if (reply != QMessageBox::Yes) {
            return;
        }
    }
    
    if (dialogMode == Generate) {
        // Ensure user confirmed backup
        if (!mnemonicConfirmed) {
            QMessageBox::warning(this, tr("Backup Required"),
                tr("You must confirm that you have written down your mnemonic phrase."));
            return;
        }
    }
    
    QDialog::accept();
}

// Made with Bob
