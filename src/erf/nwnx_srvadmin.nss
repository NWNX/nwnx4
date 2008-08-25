// Name     : NWNX Srvadmin include
// Purpose  : Various functions for performing server admin functions from nwscript
// Authors  : Skywing
// Modified : August 10, 2008

// This file is placed into the public domain.

/************************************/
/* Function prototypes              */
/************************************/

//
// Shutdown the server gracefully.  The function returns to the scripting
// environment but the server will terminate gracefully (saving all characters) a
// short time after.  Note that if you are using the NWNX watchdog functions, the
// server process may be automagically restarted by NWNX.
//
void ShutdownNwn2server();

//
// Send a broadcast server message, as per the administrative console input box.
//
void BroadcastServerMessage(string sServerMessage);

//
// Boot a player.
//
void BootPlayer(string sAccountName);

//
// Ban a player's account name.  The player must be logged in.
//
void BanPlayerName(string sAccountName);

//
// Ban a player's IP address.  The player must be logged in.  You must supply the
// account name and not the IP address.
//
void BanPlayerIP(string sAccountName);

//
// Ban a player's CD-Key.  The player must be logged in.  You must supply the
// account name and not the CD-Key.
//
void BanPlayerCDKey(string sAccountName);

//
// Set the ELC state on the fly, as per the administrative console check box.
//
void SetELC(int nELCEnabled);

const string srvadminSpacer = "                                               ";

/************************************/
/* Implementation                   */
/************************************/

void ShutdownNwn2server()
{
    NWNXSetString( "SRVADMIN", "SHUTDOWNNWN2SERVER", "", 0, "" );
}

void BroadcastServerMessage(string sServerMessage)
{
    NWNXSetString( "SRVADMIN", "BROADCASTSERVERMESSAGE", sServerMessage, 0, "" );
}

void BootPlayer(string sAccountName)
{
    NWNXSetString( "SRVADMIN", "BOOTPLAYER", sAccountName, 0, "" );
}

void BanPlayerName(string sAccountName)
{
    NWNXSetString( "SRVADMIN", "BANPLAYERNAME", sAccountName, 0, "" );
}

void BanPlayerIP(string sAccountName)
{
    NWNXSetString( "SRVADMIN", "BANPLAYERIP", sAccountName, 0, "" );
}

void BanPlayerCDKey(string sAccountName)
{
    NWNXSetString( "SRVADMIN", "BANPLAYERCDKEY", sAccountName, 0, "" );
}

void SetELC(int nELCEnabled)
{
    string sTfString;

    if (nELCEnabled == FALSE)
        sTfString = "false";
    else
        sTfString = "true";

    NWNXSetString( "SRVADMIN", "SETELC", sTfString, 0, "" );
}

