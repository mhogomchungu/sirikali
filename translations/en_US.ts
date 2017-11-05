<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1">
<context>
    <name>DialogMsg</name>
    <message>
        <location filename="../src/dialogmsg.ui" line="17"/>
        <source>Dialog</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/dialogmsg.ui" line="29"/>
        <source>&amp;Ok</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/dialogmsg.ui" line="42"/>
        <source>&amp;Yes</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/dialogmsg.ui" line="55"/>
        <source>&amp;No</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/dialogmsg.cpp" line="97"/>
        <source>INFORMATION</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/dialogmsg.cpp" line="237"/>
        <source>Do not show this dialog again</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>LXQt::Wallet::changePassWordDialog</name>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.cpp" line="112"/>
        <source>Create a new wallet</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.cpp" line="114"/>
        <source>Create</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.cpp" line="142"/>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.cpp" line="278"/>
        <source>Passwords do not match</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.cpp" line="223"/>
        <source>Wallet password could not be changed</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.cpp" line="232"/>
        <source>Wallet could not be opened with the presented key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.cpp" line="242"/>
        <source>New passwords do not match</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>LXQt::Wallet::password_dialog</name>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/password_dialog.cpp" line="72"/>
        <source>Wallet could not be opened with the presented key</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../src/checkforupdates.cpp" line="83"/>
        <source>%1&quot;%2&quot; Installed Version Is : %3.
Latest Version Is : %4.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1295"/>
        <location filename="../src/sirikali.cpp" line="1311"/>
        <source>ERROR</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1296"/>
        <source>Failed To Find %1 Executable</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1309"/>
        <source>INFORMATION</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1312"/>
        <source>Failed To Get Volume Properties</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="41"/>
        <source>Kde Wallet</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="46"/>
        <source>Internal Wallet</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="51"/>
        <source>Gnome Wallet</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="56"/>
        <source>OSX KeyChain</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1226"/>
        <source>Not Supported KeyFile Encountered Since It Contains AtLeast One Illegal Character(&apos;\n&apos;,&apos;\0&apos;,&apos;\r&apos;).

Please Use a Hash Of The KeyFile Through &quot;HMAC+KeyFile&quot; Option.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1277"/>
        <source>Effective Key Is Generated With Below Formula:

key = hmac_sha256(password,keyfile contents)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/utility.cpp" line="456"/>
        <source>
options:
	-d   Path to where a volume to be auto unlocked/mounted is located.
	-m   Tool to use to open a default file manager(default tool is xdg-open).
	-e   Start the application without showing the GUI.
	-b   A name of a backend to retrieve a password from when a volume is open from CLI.
	     Supported backends are: &quot;internal&quot;,&quot;stdin&quot;,&quot;keyfile&quot;,&quot;osxkeychain&quot;,&quot;kwallet&quot; and &quot;libsecret&quot;.
	     The first three are always present but the rest are compile time dependencies.
	     &quot;internal&quot; option causes SiriKali to read password from lxqt-wallet internal backend.
	     &quot;stdin&quot; option causes SiriKali to read the password from standard input.
	     &quot;keyfile&quot; option causes SiriKali to read the password from a file.
	     &quot;libsecret&quot; option causes SiriKali to read password from lxqt-wallet libsecret backend.
	     &quot;kwallet&quot; option causes SiriKali to read password from lxqt-wallet kwallet backend.
	     &quot;osxkeychain&quot; option causes SiriKali to read password from lxqt-wallet OSX key chain backend.
	-k   When opening a volume from CLI,a value of &quot;rw&quot; will open the volume in read\write
	     mode and a value of &quot;ro&quot; will open the volume in read only mode.
	-z   Full path of the mount point to be used when the volume is opened from CLI.
	     This option is optional.
	-c   Set Volume Configuration File Path when a volume is opened from CLI.
	-i   Set inactivity timeout(in minutes) to dismount the volume when mounted from CLI.
	-o   Set mount options when mounting a volume from CLI.
	-f   Path to keyfile.
	-u   Unmount volume.
	-p   Print a list of unlocked volumes.
	-s   Option to trigger generation of password hash.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/utility.cpp" line="826"/>
        <source>about SiriKali</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1281"/>
        <source>This plugin delegates key generation to an external application</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>changePassWordDialog</name>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.ui" line="17"/>
        <source>Change Wallet&apos;s Password</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.ui" line="45"/>
        <source>C&amp;hange</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.ui" line="58"/>
        <source>&amp;Ok</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.ui" line="129"/>
        <source>Enter Current Password Below.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.ui" line="148"/>
        <source>Enter New Password Below.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.ui" line="167"/>
        <source>Re Enter New Password Below.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.ui" line="186"/>
        <source>&amp;Cancel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.ui" line="199"/>
        <source>&lt;!DOCTYPE HTML PUBLIC &quot;-//W3C//DTD HTML 4.0//EN&quot; &quot;http://www.w3.org/TR/REC-html40/strict.dtd&quot;&gt;
&lt;html&gt;&lt;head&gt;&lt;meta name=&quot;qrichtext&quot; content=&quot;1&quot; /&gt;&lt;style type=&quot;text/css&quot;&gt;
p, li { white-space: pre-wrap; }
&lt;/style&gt;&lt;/head&gt;&lt;body style=&quot; font-family:&apos;Monospace&apos;; font-size:10pt; font-weight:400; font-style:normal;&quot;&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt;&quot;&gt;An application &apos;&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt; font-weight:600;&quot;&gt;%1&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt;&quot;&gt;&apos; has made a request for a password of its wallet &apos;&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt; font-weight:600;&quot;&gt;%2&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt;&quot;&gt;&apos; to be changed&lt;/span&gt;&lt;/p&gt;
&lt;p style=&quot;-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:&apos;Sans Serif&apos;; font-size:9pt;&quot;&gt;&lt;br /&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.ui" line="217"/>
        <source>&lt;!DOCTYPE HTML PUBLIC &quot;-//W3C//DTD HTML 4.0//EN&quot; &quot;http://www.w3.org/TR/REC-html40/strict.dtd&quot;&gt;
&lt;html&gt;&lt;head&gt;&lt;meta name=&quot;qrichtext&quot; content=&quot;1&quot; /&gt;&lt;style type=&quot;text/css&quot;&gt;
p, li { white-space: pre-wrap; }
&lt;/style&gt;&lt;/head&gt;&lt;body style=&quot; font-family:&apos;Monospace&apos;; font-size:10pt; font-weight:400; font-style:normal;&quot;&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt;&quot;&gt;Enter password information below to create a new wallet &apos;&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt; font-weight:600;&quot;&gt;%1&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt;&quot;&gt;&apos; for application &apos;&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt; font-weight:600;&quot;&gt;%2&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt;&quot;&gt;&apos;&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>checkUpdates</name>
    <message>
        <location filename="../src/checkforupdates.cpp" line="66"/>
        <source>Network Request Failed To Respond Within %1 Seconds.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/checkforupdates.cpp" line="68"/>
        <source>ERROR</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/checkforupdates.cpp" line="108"/>
        <location filename="../src/checkforupdates.cpp" line="111"/>
        <source>Version Info</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>configFileOption</name>
    <message>
        <location filename="../src/configfileoption.ui" line="14"/>
        <source>Set Config File Path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/configfileoption.ui" line="26"/>
        <source>&amp;Set</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/configfileoption.ui" line="39"/>
        <source>&amp;Cancel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/configfileoption.ui" line="78"/>
        <source>Select A Folder To Create Config File In.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>cryfscreateoptions</name>
    <message>
        <location filename="../src/cryfscreateoptions.ui" line="14"/>
        <source>Select Volume Creation Options</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/cryfscreateoptions.ui" line="36"/>
        <source>Cipher To Use For Encryption.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/cryfscreateoptions.ui" line="55"/>
        <source>Cipher Text Block Size In Bytes.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/cryfscreateoptions.ui" line="81"/>
        <source>&amp;OK</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/cryfscreateoptions.ui" line="94"/>
        <source>&amp;Cancel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/cryfscreateoptions.ui" line="107"/>
        <source>Select A Folder To Create Config File In.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>dialogok</name>
    <message>
        <location filename="../src/dialogok.ui" line="17"/>
        <source>Dialog</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/dialogok.ui" line="29"/>
        <source>&amp;Ok</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/dialogok.ui" line="67"/>
        <source>&amp;Yes</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/dialogok.ui" line="80"/>
        <source>&amp;No</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>favorites</name>
    <message>
        <location filename="../src/favorites.ui" line="17"/>
        <source>Manage Favorites</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/favorites.ui" line="91"/>
        <source>Config File Path (Optional)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/favorites.ui" line="107"/>
        <source>Idle TimeOut (Optional)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/favorites.ui" line="196"/>
        <source>Mount Options</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/favorites.ui" line="207"/>
        <source>Auto Mount Volume</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/favorites.ui" line="120"/>
        <source>&amp;Add</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/favorites.ui" line="156"/>
        <source>Volume ID</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/favorites.ui" line="164"/>
        <source>Mount Point</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/favorites.ui" line="172"/>
        <source>Auto Mount</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/favorites.ui" line="180"/>
        <source>Config File Path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/favorites.ui" line="188"/>
        <source>Idle Time Out</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/favorites.ui" line="214"/>
        <source>Mount Options (Optional And Comma Separated)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/favorites.ui" line="26"/>
        <source>Encrypted Folder Path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/favorites.ui" line="58"/>
        <source>Mount Point Path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/favorites.ui" line="48"/>
        <source>open file dialog</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/favorites.ui" line="130"/>
        <source>&amp;Done</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/favorites.cpp" line="206"/>
        <source>Toggle AutoMount</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/favorites.cpp" line="211"/>
        <source>Remove Selected Entry</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/favorites.cpp" line="216"/>
        <source>Cancel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/favorites.cpp" line="304"/>
        <location filename="../src/favorites.cpp" line="308"/>
        <location filename="../src/favorites.cpp" line="314"/>
        <source>ERROR!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/favorites.cpp" line="304"/>
        <source>Encrypted Folder Address Field Is Empty</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/favorites.cpp" line="308"/>
        <source>Mount Point Path Field Is Empty</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/favorites.cpp" line="314"/>
        <source>Mount Options Can Not Contain &quot;-&quot;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/favorites.cpp" line="359"/>
        <source>Path To A Config File</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/favorites.cpp" line="400"/>
        <source>Path To An Encrypted Folder</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/favorites.cpp" line="410"/>
        <source>Path To Mount Folder</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>fileManager</name>
    <message>
        <location filename="../src/filemanager.ui" line="14"/>
        <location filename="../src/filemanager.cpp" line="49"/>
        <source>Set File Manager</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/filemanager.ui" line="26"/>
        <source>TextLabel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/filemanager.ui" line="55"/>
        <source>&amp;Set</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/filemanager.cpp" line="40"/>
        <source>Enter Below The Name Of The Application You Want To Be Used To Open Mount Points.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/filemanager.cpp" line="51"/>
        <source>Set Executable For &quot;ExternalExecutable&quot; Plugin</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/filemanager.cpp" line="53"/>
        <source>Enter Below The Name Of The External Executable And Its Arguments To Use When Invoking &quot;ExternalExecutable&quot; Plugin.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>keyDialog</name>
    <message>
        <location filename="../src/keydialog.ui" line="29"/>
        <source>&amp;Open</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="45"/>
        <source>&amp;Cancel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="58"/>
        <location filename="../src/keydialog.cpp" line="209"/>
        <location filename="../src/keydialog.cpp" line="1257"/>
        <location filename="../src/keydialog.cpp" line="1397"/>
        <source>Key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="100"/>
        <location filename="../src/keydialog.cpp" line="179"/>
        <source>Volume Name</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="142"/>
        <source>Mount In &amp;Read Only Mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="220"/>
        <source>&amp;OK</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="290"/>
        <source>Set Key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="303"/>
        <source>Cancel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="358"/>
        <source>Password</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="387"/>
        <source>TextLabel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="155"/>
        <source>O&amp;ptions</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="188"/>
        <source>Volume Path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="177"/>
        <source>&amp;Create</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="189"/>
        <location filename="../src/keydialog.cpp" line="373"/>
        <source>Unlocking &quot;%1&quot;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="191"/>
        <source>Mount Path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="339"/>
        <location filename="../src/keydialog.cpp" line="210"/>
        <location filename="../src/keydialog.cpp" line="1008"/>
        <location filename="../src/keydialog.cpp" line="1263"/>
        <location filename="../src/keydialog.cpp" line="1362"/>
        <source>KeyFile</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="212"/>
        <location filename="../src/keydialog.cpp" line="1298"/>
        <source>HMAC+KeyFile</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="211"/>
        <location filename="../src/keydialog.cpp" line="1271"/>
        <location filename="../src/keydialog.cpp" line="1275"/>
        <location filename="../src/keydialog.cpp" line="1376"/>
        <source>Key+KeyFile</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="213"/>
        <location filename="../src/keydialog.cpp" line="1271"/>
        <source>ExternalExecutable</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="253"/>
        <source>Check This Box To Make Password Visible</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="271"/>
        <source>(%1/%2) Unlocking &quot;%3&quot;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="397"/>
        <source>Create A New &quot;%1&quot; Volume</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="484"/>
        <source>Passphrase Quality: 0%</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="486"/>
        <source>Passphrase Quality: %1%</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="491"/>
        <location filename="../src/keydialog.cpp" line="1325"/>
        <source>Passphrase Quality: 100%</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="542"/>
        <location filename="../src/keydialog.cpp" line="565"/>
        <source>Select A Folder To Create A Mount Point In.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="672"/>
        <location filename="../src/keydialog.cpp" line="686"/>
        <source>Select A File To Be Used As A Keyfile.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="712"/>
        <source>Volume Name Field Is Empty.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="721"/>
        <source>Key Field Is Empty.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="774"/>
        <source>Internal Wallet Is Not Configured.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="819"/>
        <source>Failed To Unlock A Cryfs Volume.
Wrong Password Entered.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="824"/>
        <source>Failed To Unlock An Encfs Volume.
Wrong Password Entered.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="829"/>
        <source>Failed To Unlock A Gocryptfs Volume.
Wrong Password Entered.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="834"/>
        <source>Failed To Unlock An Ecryptfs Volume.
Wrong Password Entered.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="839"/>
        <source>A Space Character Is Not Allowed In Paths When Using Ecryptfs Backend And Polkit.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="844"/>
        <source>This Backend Requires Root&apos;s Privileges And An attempt To Acquire Them Has Failed.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="849"/>
        <source>Failed To Unlock A Securefs Volume.
Wrong Password Entered.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="854"/>
        <source>Failed To Complete The Request.
Cryfs Executable Could Not Be Found.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="859"/>
        <source>SiriKali Can Not Unlock This Volume Because Its FileSystem Has To Manually Be Converted To The Version Of Cryfs That Is Currently In Use.

Run Cryfs With This Volume To Manually Update This Volume&apos;s FileSystem.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="864"/>
        <source>Failed To Complete The Request.
Encfs Executable Could Not Be Found.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="869"/>
        <source>Failed To Complete The Request.
Ecryptfs-simple Executable Could Not Be Found.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="874"/>
        <source>Failed To Complete The Request.
Gocryptfs Executable Could Not Be Found.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="879"/>
        <source>Failed To Complete The Request.
Securefs Executable Could Not Be Found.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="884"/>
        <source>Failed To Create Mount Point.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="889"/>
        <source>Failed To Unlock The Volume.
Not Supported Volume Encountered.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="896"/>
        <source>Failed To Complete The Task And Below Log was Generated By The Backend.
</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1048"/>
        <source>Failed To Generate Key.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1302"/>
        <source>Select A KeyFile</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="957"/>
        <source>Encrypted Folder Path Is Already Taken.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="966"/>
        <location filename="../src/keydialog.cpp" line="1118"/>
        <source>Mount Point Path Already Taken.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="973"/>
        <location filename="../src/keydialog.cpp" line="1107"/>
        <location filename="../src/keydialog.cpp" line="1132"/>
        <source>Atleast One Required Field Is Empty.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1125"/>
        <source>Encrypted Folder Appear To Not Be Present.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1374"/>
        <location filename="../src/keydialog.cpp" line="1386"/>
        <source>Plugin name</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1411"/>
        <source>Keyfile path</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>oneinstance</name>
    <message>
        <location filename="../src/oneinstance.cpp" line="69"/>
        <source>Previous instance seem to have crashed,trying to clean up before starting</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/oneinstance.cpp" line="76"/>
        <source>There seem to be another instance running,exiting this one</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>options</name>
    <message>
        <location filename="../src/options.ui" line="17"/>
        <source>Set Mount Options</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/options.ui" line="80"/>
        <source>Unlock A Cryfs/Gocryptfs/Securefs Volume With Specified Configuration File.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/options.ui" line="109"/>
        <source>Automatically Unmount After Specified Minutes of Inactivity Is Reached.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/options.ui" line="138"/>
        <source>Mount The Volume With Below Mount Options.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/options.ui" line="157"/>
        <source>&amp;Set</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/options.ui" line="170"/>
        <source>&amp;Cancel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/options.cpp" line="139"/>
        <source>Select Cryfs/Gocryptfs Configuration File</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/options.cpp" line="171"/>
        <source>ERROR</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/options.cpp" line="171"/>
        <source>Idle Time Field Requires Digits Only If Not Empty.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>password_dialog</name>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/password_dialog.ui" line="17"/>
        <source>lxqt wallet service</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/password_dialog.ui" line="45"/>
        <source>The application &apos;%1&apos; has requested to open the wallet &apos;%2&apos;.Enter the password below for this wallet to unlock it</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/password_dialog.ui" line="77"/>
        <source>Wallet &apos;%1&apos; does not exist,do you want to create it?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/password_dialog.ui" line="93"/>
        <source>&lt;!DOCTYPE HTML PUBLIC &quot;-//W3C//DTD HTML 4.0//EN&quot; &quot;http://www.w3.org/TR/REC-html40/strict.dtd&quot;&gt;
&lt;html&gt;&lt;head&gt;&lt;meta name=&quot;qrichtext&quot; content=&quot;1&quot; /&gt;&lt;style type=&quot;text/css&quot;&gt;
p, li { white-space: pre-wrap; }
&lt;/style&gt;&lt;/head&gt;&lt;body style=&quot; font-family:&apos;Monospace&apos;; font-size:10pt; font-weight:400; font-style:normal;&quot;&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt;&quot;&gt;Wallet &lt;/span&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt; font-weight:600;&quot;&gt;&apos;%1&apos;&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt;&quot;&gt; does not exist, do you want to create it?&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/password_dialog.ui" line="110"/>
        <source>&lt;!DOCTYPE HTML PUBLIC &quot;-//W3C//DTD HTML 4.0//EN&quot; &quot;http://www.w3.org/TR/REC-html40/strict.dtd&quot;&gt;
&lt;html&gt;&lt;head&gt;&lt;meta name=&quot;qrichtext&quot; content=&quot;1&quot; /&gt;&lt;style type=&quot;text/css&quot;&gt;
p, li { white-space: pre-wrap; }
&lt;/style&gt;&lt;/head&gt;&lt;body style=&quot; font-family:&apos;Monospace&apos;; font-size:10pt; font-weight:400; font-style:normal;&quot;&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt;&quot;&gt;An application &lt;/span&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt; font-weight:600;&quot;&gt;&apos;%1&apos;&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt;&quot;&gt; has requested to open a wallet&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt; font-weight:600;&quot;&gt; &apos;%2&apos;&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt;&quot;&gt;. Enter the password below for this wallet to unlock it.&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/password_dialog.ui" line="127"/>
        <source>&amp;Unlock</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/password_dialog.ui" line="144"/>
        <source>&amp;Cancel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/password_dialog.ui" line="157"/>
        <source>ok</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/password_dialog.ui" line="170"/>
        <source>&amp;Ok</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>plugin</name>
    <message>
        <location filename="../src/plugin.ui" line="17"/>
        <source>Key Generator Using A Passphrase And A KeyFile</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/plugin.ui" line="29"/>
        <source>&amp;Set Key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/plugin.ui" line="42"/>
        <source>&amp;Cancel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/plugin.ui" line="113"/>
        <source>Passphrase</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/plugin.ui" line="142"/>
        <location filename="../src/plugin.cpp" line="163"/>
        <source>KeyFile</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/plugin.cpp" line="113"/>
        <location filename="../src/plugin.cpp" line="152"/>
        <source>ERROR</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/plugin.cpp" line="113"/>
        <source>KeyFile Not Set</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/plugin.cpp" line="152"/>
        <source>Failed To Generate Key</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>readOnlyWarning</name>
    <message>
        <location filename="../src/readonlywarning.ui" line="17"/>
        <source>WARNING</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/readonlywarning.ui" line="29"/>
        <source>Do Not Show This Message Again.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/readonlywarning.ui" line="42"/>
        <source>Setting This Option Will Cause The Volume To Open In Read Only Mode.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/readonlywarning.ui" line="61"/>
        <source>&amp;Ok</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>securefscreateoptions</name>
    <message>
        <location filename="../src/securefscreateoptions.ui" line="14"/>
        <source>Choose Format Type</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/securefscreateoptions.ui" line="29"/>
        <source>The &quot;lite format&quot; simply encrypts filenames and file contents separately, similar to how encfs operates, although with more security.

The &quot;full format&quot; maps files, directories and symlinks in the virtual filesystem all to regular files in the underlying filesystem. The directory structure is flattened and recorded as B-trees in files.

The lite format has become the default on Unix-like operating systems as it is much faster and features easier conflict resolution, especially when used with DropBox, Google Drive, etc. The full format, however, leaks fewer information about the filesystem hierarchy, runs relatively independent of the features of the underlying filesystem, and is in general more secure.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/securefscreateoptions.ui" line="50"/>
        <source>Lite Format</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/securefscreateoptions.ui" line="55"/>
        <source>Full Format</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/securefscreateoptions.ui" line="69"/>
        <source>&amp;OK</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/securefscreateoptions.ui" line="82"/>
        <source>&amp;Cancel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/securefscreateoptions.ui" line="95"/>
        <source>Select A Folder To Create Config File In.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>sirikali</name>
    <message>
        <location filename="../src/sirikali.ui" line="17"/>
        <source>SiriKali</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.ui" line="55"/>
        <source>Volume Path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.ui" line="63"/>
        <source>Mount Point Path</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.ui" line="71"/>
        <source>File System</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.ui" line="79"/>
        <source>Mode</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.ui" line="95"/>
        <source>&amp;Create Volume</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.ui" line="108"/>
        <source>Mount &amp;Volume</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.ui" line="115"/>
        <source>&amp;Refresh</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.ui" line="129"/>
        <source>&amp;Menu</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="335"/>
        <source>Auto Open Mount Point</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="338"/>
        <source>Reuse Mount Point</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="341"/>
        <source>Autocheck For Updates</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="347"/>
        <source>Set Mount Point Prefix</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="356"/>
        <source>Unmount All</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="326"/>
        <location filename="../src/sirikali.cpp" line="394"/>
        <location filename="../src/sirikali.cpp" line="455"/>
        <location filename="../src/sirikali.cpp" line="496"/>
        <source>Internal Wallet</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="396"/>
        <source>Change Password</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="404"/>
        <source>Key Storage</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="327"/>
        <location filename="../src/sirikali.cpp" line="456"/>
        <location filename="../src/sirikali.cpp" line="498"/>
        <location filename="../src/sirikali.cpp" line="579"/>
        <source>KDE Wallet</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="328"/>
        <location filename="../src/sirikali.cpp" line="457"/>
        <location filename="../src/sirikali.cpp" line="500"/>
        <location filename="../src/sirikali.cpp" line="583"/>
        <source>Gnome Wallet</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.ui" line="122"/>
        <location filename="../src/sirikali.cpp" line="409"/>
        <source>Favorites</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="413"/>
        <source>Select Language</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="416"/>
        <source>Check For Updates</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="419"/>
        <source>About</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="424"/>
        <source>Quit</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="421"/>
        <source>Show/Hide</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="621"/>
        <location filename="../src/sirikali.cpp" line="646"/>
        <source>Manage Favorites</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="625"/>
        <location filename="../src/sirikali.cpp" line="647"/>
        <source>Mount All</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="864"/>
        <source>ERROR: Volume Path Not Given.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="965"/>
        <source>ERROR: Key Not Found In The Backend.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="970"/>
        <source>ERROR: Failed To Unlock Requested Backend.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="735"/>
        <location filename="../src/sirikali.cpp" line="783"/>
        <location filename="../src/sirikali.cpp" line="1166"/>
        <location filename="../src/sirikali.cpp" line="1593"/>
        <location filename="../src/sirikali.cpp" line="1709"/>
        <source>ERROR</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1166"/>
        <source>Failed To Read Volume Properties</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1125"/>
        <location filename="../src/sirikali.cpp" line="1171"/>
        <source>INFORMATION</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="161"/>
        <location filename="../src/sirikali.cpp" line="163"/>
        <source>%1 Is Not Installed</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="329"/>
        <location filename="../src/sirikali.cpp" line="459"/>
        <location filename="../src/sirikali.cpp" line="502"/>
        <location filename="../src/sirikali.cpp" line="587"/>
        <source>OSX KeyChain</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="330"/>
        <location filename="../src/sirikali.cpp" line="458"/>
        <location filename="../src/sirikali.cpp" line="575"/>
        <source>None</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="344"/>
        <source>Start Minimized</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="350"/>
        <source>Set File Manager</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="353"/>
        <source>Set External Plugin Executable</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="359"/>
        <source>Unmount All And Quit</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="364"/>
        <source>AutoMount Favorites</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="366"/>
        <source>AutoMount Key Source</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="376"/>
        <source>AutoMount Favorite Volumes At Start Up</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="381"/>
        <source>AutoMount Favorite Volumes When Available</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="386"/>
        <source>Show Mount Dialog When AutoMounting</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="735"/>
        <source>SiriKali Failed To Connect To siriPolkit.
Please Report This Serious Bug.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="783"/>
        <source>&quot;%1&quot; Folder Must Be Writable</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1210"/>
        <source>Block Size: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1211"/>
        <source>Used Blocks: %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1212"/>
        <source>Free Blocks: %3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1213"/>
        <source>Total Blocks %4</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1214"/>
        <source>Used Space: %5</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1215"/>
        <source>Free Space: %6</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1216"/>
        <source>Total Space: %7</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1217"/>
        <source>Used %: %8</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1368"/>
        <source>Unmount</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1366"/>
        <source>Open Folder</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1370"/>
        <source>Properties</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1407"/>
        <source>Close Menu</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1459"/>
        <source>WARNING</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1460"/>
        <source>Could Not Open Mount Point Because &quot;%1&quot; Tool Does Not Appear To Be Working Correctly.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1507"/>
        <source>&amp;Quit</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1594"/>
        <source>Permission To Access The Volume Was Denied
Or
The Volume Is Not Supported</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1614"/>
        <source>Select An Encrypted Volume Directory</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1709"/>
        <source>Failed To Unmount %1 Volume</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>walletconfig</name>
    <message>
        <location filename="../src/walletconfig.ui" line="17"/>
        <source>Manage Volumes In A Wallet</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/walletconfig.ui" line="32"/>
        <source>&amp;Add</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/walletconfig.ui" line="51"/>
        <source>&amp;Delete</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/walletconfig.ui" line="64"/>
        <source>Do&amp;ne</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/walletconfig.ui" line="111"/>
        <source>Volume ID</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/walletconfig.ui" line="116"/>
        <source>Comment</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/walletconfig.cpp" line="104"/>
        <source>Delete Entry</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/walletconfig.cpp" line="117"/>
        <source>WARNING!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/walletconfig.cpp" line="117"/>
        <source>Are you sure you want to delete a volume with an id of &quot;%1&quot;?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/walletconfig.cpp" line="201"/>
        <source>ERROR!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/walletconfig.cpp" line="201"/>
        <source>Failed To Add the Key In The Wallet.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>walletconfiginput</name>
    <message>
        <location filename="../src/walletconfiginput.ui" line="17"/>
        <source>Add An Entry To Wallet</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/walletconfiginput.ui" line="29"/>
        <source>&amp;Add</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/walletconfiginput.ui" line="42"/>
        <source>&amp;Cancel</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/walletconfiginput.ui" line="64"/>
        <source>Volume ID</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/walletconfiginput.ui" line="83"/>
        <source>Comment</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/walletconfiginput.ui" line="102"/>
        <source>Key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/walletconfiginput.ui" line="121"/>
        <source>Repeat Key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/walletconfiginput.cpp" line="102"/>
        <location filename="../src/walletconfiginput.cpp" line="109"/>
        <source>ERROR!</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/walletconfiginput.cpp" line="102"/>
        <source>Atleast one required field is empty</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/walletconfiginput.cpp" line="109"/>
        <source>Passphrases do not match</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/walletconfiginput.cpp" line="132"/>
        <source>Select A Volume</source>
        <translation type="unfinished"></translation>
    </message>
</context>
</TS>
