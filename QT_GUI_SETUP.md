# Qt GUI Setup for NoteBlockchain

## Overview

The NoteBlockchain Qt GUI (notecoin-qt) provides a graphical user interface for the wallet. By default, it's not built because Qt5 dependencies are missing.

---

## Installing Qt5 Dependencies

### For Ubuntu 24.04 / Debian-based Systems

```bash
# Install Qt5 development libraries
sudo apt-get update
sudo apt-get install -y \
    qtbase5-dev \
    qttools5-dev \
    qttools5-dev-tools \
    libqt5gui5 \
    libqt5core5a \
    libqt5dbus5 \
    qtbase5-dev-tools \
    libprotobuf-dev \
    protobuf-compiler \
    libqrencode-dev
```

### For Ubuntu 22.04 / Older Systems

```bash
sudo apt-get update
sudo apt-get install -y \
    qt5-default \
    qtbase5-dev \
    qttools5-dev \
    qttools5-dev-tools \
    libprotobuf-dev \
    protobuf-compiler \
    libqrencode-dev
```

### For Fedora / RHEL-based Systems

```bash
sudo dnf install -y \
    qt5-qtbase-devel \
    qt5-qttools-devel \
    protobuf-devel \
    qrencode-devel
```

### For Arch Linux

```bash
sudo pacman -S \
    qt5-base \
    qt5-tools \
    protobuf \
    qrencode
```

---

## Building with Qt GUI

### Step 1: Install Qt Dependencies

```bash
# For Ubuntu 24.04
sudo apt-get install -y qtbase5-dev qttools5-dev qttools5-dev-tools \
    libprotobuf-dev protobuf-compiler libqrencode-dev
```

### Step 2: Clone the code

```bash
git clone https://github.com/note-llc/NoteBlockchain.git
cd NoteBlockchain
chmod +x ./contrib/install_db4.sh
./contrib/install_db4.sh `pwd`
```

This will result in building Berkley DB locally from source. At the end of the build process, it will display two commands that you need to save. For example, it might look something like this

```bash
  export BDB_PREFIX='your/library/here'
  ./configure BDB_LIBS="-L${BDB_PREFIX}/lib -ldb_cxx-4.8" BDB_CFLAGS="-I${BDB_PREFIX}/include"
```

Execute the first export command and copy the ./configure command. You will need this later.

### Step 3: Build the code

```bash
./autogen.sh
```

This builds the configure script which can now be executed.

```bash
./configure BDB_LIBS="-L${BDB_PREFIX}/lib -ldb_cxx-4.8" BDB_CFLAGS="-I${BDB_PREFIX}/include"
```

### Step 4: Compile

```bash
make -j$(nproc)
```

### Step 6: Verify Binaries generated

```bash
ls -lh src/qt/notecoin-qt src/notecoin-cli src/notecoind
# Should show the binaries
```

### Step 7: (Optional) Install the binaries

```bash
sudo make install
```

This will copy the binaries to a folder so that you can invoke the commands notecoin* directly from anywhere.

---

## Running the Qt GUI

### Launch the GUI

```bash
# From the build directory
./src/qt/notecoin-qt

# Or if installed
notecoin-qt
```

### Command Line Options

```bash
# Run on testnet
./src/qt/notecoin-qt -testnet

# Specify data directory
./src/qt/notecoin-qt -datadir=/path/to/data

# Run with debug output
./src/qt/notecoin-qt -debug

# Show help
./src/qt/notecoin-qt -help
```

---

## Troubleshooting

### Issue: "Qt dependencies not found"

**Solution**: Install all Qt5 packages listed above, then reconfigure:

```bash
sudo apt-get install -y qtbase5-dev qttools5-dev qttools5-dev-tools
```

### Issue: "protobuf not found"

**Solution**: Install protobuf development libraries:

```bash
sudo apt-get install -y libprotobuf-dev protobuf-compiler
```

### Issue: "qrencode not found"

**Solution**: Install qrencode library (for QR code support):

```bash
sudo apt-get install -y libqrencode-dev
```

### Issue: Qt GUI has circular dependency errors

**Solution**: The same Makefile fixes apply. If you encounter linker errors, check that `src/Makefile.qt.include` exists and review its LDADD configuration.

---

## Qt GUI Features

The notecoin-qt GUI provides:

- ✅ **Wallet Management**: Send/receive transactions
- ✅ **Address Book**: Manage contacts
- ✅ **Transaction History**: View all transactions
- ✅ **Mnemonic Support**: Generate and restore from BIP39 mnemonic
- ✅ **QR Codes**: Generate QR codes for addresses
- ✅ **Coin Control**: Advanced UTXO management
- ✅ **Network Info**: View peer connections and sync status
- ✅ **Console**: Debug console for RPC commands

---

## Building Without Qt (CLI Only)

If you don't need the GUI, you can explicitly disable it:

```bash
./configure BDB_LIBS="-L${BDB_PREFIX}/lib -ldb_cxx-4.8" BDB_CFLAGS="-I${BDB_PREFIX}/include" \
    --without-gui
```

This builds only:
- `notecoind` - Daemon
- `notecoin-cli` - Command-line interface
- `notecoin-tx` - Transaction utility

---

## Complete Build Script with Qt

```bash
#!/bin/bash
# Build NoteBlockchain with Qt GUI

git clone https://github.com/note-llc/NoteBlockchain.git
cd NoteBlockchain
chmod +x ./contrib/install_db4.sh
./contrib/install_db4.sh `pwd`

# Install dependencies
echo "Installing Qt dependencies..."
sudo apt-get update
sudo apt-get install -y \
    qtbase5-dev qttools5-dev qttools5-dev-tools \
    libprotobuf-dev protobuf-compiler libqrencode-dev

# Generate build files
echo "Running autogen.sh..."
./autogen.sh

# Configure with Qt
echo "Configuring with Qt GUI..."
./configure BDB_LIBS="-L${BDB_PREFIX}/lib -ldb_cxx-4.8" BDB_CFLAGS="-I${BDB_PREFIX}/include" \
    --with-gui=qt5

# Check if Qt was enabled
if grep -q "with gui / qt = yes" config.log; then
    echo "✅ Qt GUI enabled"
else
    echo "❌ Qt GUI not enabled - check dependencies"
    exit 1
fi

# Compile
echo "Compiling..."
make -j$(nproc)

# Verify binaries
echo ""
echo "Build complete! Binaries:"
ls -lh src/notecoind src/notecoin-cli src/notecoin-tx src/qt/notecoin-qt 2>/dev/null || echo "Some binaries missing"

echo ""
echo "To run the GUI:"
echo "  ./src/qt/notecoin-qt"
```

Save this as `build_with_qt.sh`, make it executable, and run:

```bash
chmod +x build_with_qt.sh
./build_with_qt.sh
```

---

## Qt Version Requirements

- **Minimum**: Qt 5.5.1
- **Recommended**: Qt 5.9 or later
- **Tested**: Qt 5.15.x (Ubuntu 24.04 default)

Check your Qt version:

```bash
qmake --version
# or
pkg-config --modversion Qt5Core
```

---

## Additional Qt Modules (Optional)

For full functionality, you may also want:

```bash
# DBus support (for desktop notifications)
sudo apt-get install -y libqt5dbus5

# SVG support (for icons)
sudo apt-get install -y libqt5svg5-dev

# Multimedia support
sudo apt-get install -y qtmultimedia5-dev
```

---

## Summary

**Quick Install for Ubuntu 24.04:**

```bash
# 1. Install Qt dependencies
sudo apt-get install -y qtbase5-dev qttools5-dev qttools5-dev-tools \
    libprotobuf-dev protobuf-compiler libqrencode-dev

# 2. Reconfigure
cd /home/rvish/NoteBlockchain
./configure BDB_LIBS='-L/home/rvish/NoteBlockchain/db4/lib -ldb_cxx-4.8' \
    BDB_CFLAGS=-I/home/rvish/NoteBlockchain/db4/include --with-gui=qt5

# 3. Build
make clean
make -j$(nproc)

# 4. Run
./src/qt/notecoin-qt
```

The Qt GUI will include all the BIP39 mnemonic features with a user-friendly interface!
