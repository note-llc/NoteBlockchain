// Copyright (c) 2011-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_MNEMONICDIALOG_H
#define BITCOIN_QT_MNEMONICDIALOG_H

#include <QDialog>

class WalletModel;

namespace Ui {
    class MnemonicDialog;
}

/** Dialog for displaying or importing BIP39 mnemonic phrases */
class MnemonicDialog : public QDialog
{
    Q_OBJECT

public:
    enum Mode {
        Display,  // Show existing mnemonic
        Import,   // Import new mnemonic
        Generate  // Generate and display new mnemonic
    };

    explicit MnemonicDialog(Mode mode, WalletModel *model, QWidget *parent = nullptr);
    ~MnemonicDialog();

    /** Get the entered mnemonic phrase (for Import mode) */
    QString getMnemonic() const;
    
    /** Get the entered passphrase (for Import mode) */
    QString getPassphrase() const;
    
    /** Set the mnemonic phrase to display (for Display/Generate mode) */
    void setMnemonic(const QString &mnemonic);

private Q_SLOTS:
    /** Copy mnemonic to clipboard */
    void on_copyButton_clicked();
    
    /** Validate the entered mnemonic */
    void on_validateButton_clicked();
    
    /** Handle mnemonic text changes */
    void on_mnemonicText_textChanged();
    
    /** Handle passphrase text changes */
    void on_passphraseEdit_textChanged(const QString &text);
    
    /** Confirm backup checkbox state changed */
    void on_confirmCheckBox_stateChanged(int state);
    
    /** Accept button clicked */
    void accept() override;

private:
    Ui::MnemonicDialog *ui;
    WalletModel *walletModel;
    Mode dialogMode;
    bool mnemonicConfirmed;

    /** Validate a BIP39 mnemonic phrase */
    bool validateMnemonic(const QString &mnemonic);
    
    /** Setup UI based on dialog mode */
    void setupUI();
    
    /** Update button states based on input */
    void updateButtonStates();
};

#endif // BITCOIN_QT_MNEMONICDIALOG_H

// Made with Bob
