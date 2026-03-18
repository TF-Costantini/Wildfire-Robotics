# Project Setup and IDE Config

The setup is intended for CLion/Rider or any JetBrains suite products.
It will likely be similar with any other IDE.

### IMPORTANT:
Initial board setup required Arduino App Lab.

**DO NOT CHANGE BOARD NAME** `phoenix` as mDNS will fail. We are using `phoenix.local` as the IP address.

Use the app only to specify which WI-FI network it should connect to.

**NOTE**:
It's not that you can't change the board name. But if you do, SSH host must be changed for all users.
A different board will have a different name for DNS resolution in the same LAN and its own connection parameters

### Create SSH Connection profile to Arduino

- **Host:** <tab> `phoenix.local`
- **Username:** `arduino`
- **Private key file:** *ASK ENGINEERING TEAM*

<img src="READMEs/IMGs/SETUP/SSH_Setup.png" width="80%" alt="SSH Connection Profile Image">

