<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="fr_FR">
<context>
    <name>DialogMsg</name>
    <message>
        <location filename="../src/dialogmsg.ui" line="17"/>
        <source>Dialog</source>
        <translation>Boîte de dialogue</translation>
    </message>
    <message>
        <location filename="../src/dialogmsg.ui" line="29"/>
        <source>&amp;Ok</source>
        <translation>&amp;Ok</translation>
    </message>
    <message>
        <location filename="../src/dialogmsg.ui" line="42"/>
        <source>&amp;Yes</source>
        <translation>&amp;Oui</translation>
    </message>
    <message>
        <location filename="../src/dialogmsg.ui" line="55"/>
        <source>&amp;No</source>
        <translation>&amp;No</translation>
    </message>
    <message>
        <location filename="../src/dialogmsg.cpp" line="97"/>
        <source>INFORMATION</source>
        <translation>INFORMATION</translation>
    </message>
    <message>
        <location filename="../src/dialogmsg.cpp" line="237"/>
        <source>Do not show this dialog again</source>
        <translation>Ne plus montrer cette boîte de dialogue à nouveau</translation>
    </message>
</context>
<context>
    <name>LXQt::Wallet::changePassWordDialog</name>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.cpp" line="112"/>
        <source>Create a new wallet</source>
        <translation>Créer un nouveau trousseau</translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.cpp" line="114"/>
        <source>Create</source>
        <translation>Créer</translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.cpp" line="142"/>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.cpp" line="278"/>
        <source>Passwords do not match</source>
        <translation>Mots de passe non correspondants</translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.cpp" line="223"/>
        <source>Wallet password could not be changed</source>
        <translation>Le mot de passe du trousseau ne peut pas être changé</translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.cpp" line="232"/>
        <source>Wallet could not be opened with the presented key</source>
        <translation>Le trousseau ne peut pas être ouvert avec cette clé</translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.cpp" line="242"/>
        <source>New passwords do not match</source>
        <translation>Nouveaux mots de passe non correspondants</translation>
    </message>
</context>
<context>
    <name>LXQt::Wallet::password_dialog</name>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/password_dialog.cpp" line="72"/>
        <source>Wallet could not be opened with the presented key</source>
        <translation>Le trousseau ne peut pas être ouvert avec cette clé</translation>
    </message>
</context>
<context>
    <name>QObject</name>
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
        <translation>A propos de SiriKali</translation>
    </message>
    <message>
        <source>hmac plugin.

This plugin generates a key using below formular:

key = hmac(sha256,passphrase,keyfile contents)</source>
        <translation type="vanished">hmac plugin.

Ce plugin génère une clé en utilisant la formule ci-dessous:

key = hmac(sha256,passphrase,keyfile contents)</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1282"/>
        <source>This plugin delegates key generation to an external application</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/checkforupdates.cpp" line="83"/>
        <source>%1&quot;%2&quot; Installed Version Is : %3.
Latest Version Is : %4.</source>
        <translation>%1&quot;%2&quot; version installée : %3.
Dernière version disponible : %4.</translation>
    </message>
    <message>
        <source>Version Info</source>
        <translation type="vanished">Infos de Version</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="41"/>
        <source>Kde Wallet</source>
        <translation>Trousseau Kde</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="46"/>
        <source>Internal Wallet</source>
        <translation>Trousseau interne</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="51"/>
        <source>Gnome Wallet</source>
        <translation>Trousseau Gnome</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="56"/>
        <source>OSX KeyChain</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1227"/>
        <source>Not Supported KeyFile Encountered Since It Contains AtLeast One Illegal Character(&apos;\n&apos;,&apos;\0&apos;,&apos;\r&apos;).

Please Use a Hash Of The KeyFile Through &quot;HMAC+KeyFile&quot; Option.</source>
        <translation>Pas de fichier de clé supporté s&apos;il contient ne serait ce qu&apos;un caractère illégal (&apos;\n&apos;,&apos;\0&apos;,&apos;\r&apos;).

Merci d&apos;utiliser un hashage de la clé de fichier au travers de l&apos;option &quot;HMAC+Fichier de clé&quot;.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1278"/>
        <source>Effective Key Is Generated With Below Formula:

key = hmac_sha256(password,keyfile contents)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1018"/>
        <location filename="../src/sirikali.cpp" line="1034"/>
        <source>ERROR</source>
        <translation>ERREUR</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1019"/>
        <source>Failed To Find %1 Executable</source>
        <translation>Échec de la recherche de l&apos;exécutable %1</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1032"/>
        <source>INFORMATION</source>
        <translation>INFORMATION</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1035"/>
        <source>Failed To Get Volume Properties</source>
        <translation>Echec de l&apos;obtention des propriétés du volume</translation>
    </message>
</context>
<context>
    <name>changePassWordDialog</name>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.ui" line="17"/>
        <source>Change Wallet&apos;s Password</source>
        <translation>Changer le mot de passe du trousseau</translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.ui" line="45"/>
        <source>C&amp;hange</source>
        <translation>C&amp;hanger</translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.ui" line="58"/>
        <source>&amp;Ok</source>
        <translation>&amp;Ok</translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.ui" line="129"/>
        <source>Enter Current Password Below.</source>
        <translation>Entrer le mot de passe actuel ci dessous.</translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.ui" line="148"/>
        <source>Enter New Password Below.</source>
        <translation>Entrer le nouveau mot de passe ci dessous.</translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.ui" line="167"/>
        <source>Re Enter New Password Below.</source>
        <translation>Ré-Entrer le nouveau mot de passe ci dessous.</translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.ui" line="186"/>
        <source>&amp;Cancel</source>
        <translation>&amp;Annuler</translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.ui" line="199"/>
        <source>&lt;!DOCTYPE HTML PUBLIC &quot;-//W3C//DTD HTML 4.0//EN&quot; &quot;http://www.w3.org/TR/REC-html40/strict.dtd&quot;&gt;
&lt;html&gt;&lt;head&gt;&lt;meta name=&quot;qrichtext&quot; content=&quot;1&quot; /&gt;&lt;style type=&quot;text/css&quot;&gt;
p, li { white-space: pre-wrap; }
&lt;/style&gt;&lt;/head&gt;&lt;body style=&quot; font-family:&apos;Monospace&apos;; font-size:10pt; font-weight:400; font-style:normal;&quot;&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt;&quot;&gt;An application &apos;&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt; font-weight:600;&quot;&gt;%1&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt;&quot;&gt;&apos; has made a request for a password of its wallet &apos;&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt; font-weight:600;&quot;&gt;%2&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt;&quot;&gt;&apos; to be changed&lt;/span&gt;&lt;/p&gt;
&lt;p style=&quot;-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:&apos;Sans Serif&apos;; font-size:9pt;&quot;&gt;&lt;br /&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;!DOCTYPE HTML PUBLIC &quot;-//W3C//DTD HTML 4.0//EN&quot; &quot;http://www.w3.org/TR/REC-html40/strict.dtd&quot;&gt;
&lt;html&gt;&lt;head&gt;&lt;meta name=&quot;qrichtext&quot; content=&quot;1&quot; /&gt;&lt;style type=&quot;text/css&quot;&gt;
p, li { white-space: pre-wrap; }
&lt;/style&gt;&lt;/head&gt;&lt;body style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt; font-weight:400; font-style:normal;&quot;&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;L&apos;application &apos;&lt;span style=&quot; font-weight:600;&quot;&gt;%1&lt;/span&gt;&apos; a demandé de modifier le mot de passe de son trousseau   &apos;&lt;span style=&quot; font-weight:600;&quot;&gt;%2&lt;/span&gt;&apos;&lt;/p&gt;
&lt;p style=&quot;-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;br /&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.ui" line="217"/>
        <source>&lt;!DOCTYPE HTML PUBLIC &quot;-//W3C//DTD HTML 4.0//EN&quot; &quot;http://www.w3.org/TR/REC-html40/strict.dtd&quot;&gt;
&lt;html&gt;&lt;head&gt;&lt;meta name=&quot;qrichtext&quot; content=&quot;1&quot; /&gt;&lt;style type=&quot;text/css&quot;&gt;
p, li { white-space: pre-wrap; }
&lt;/style&gt;&lt;/head&gt;&lt;body style=&quot; font-family:&apos;Monospace&apos;; font-size:10pt; font-weight:400; font-style:normal;&quot;&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt;&quot;&gt;Enter password information below to create a new wallet &apos;&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt; font-weight:600;&quot;&gt;%1&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt;&quot;&gt;&apos; for application &apos;&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt; font-weight:600;&quot;&gt;%2&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt;&quot;&gt;&apos;&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;!DOCTYPE HTML PUBLIC &quot;-//W3C//DTD HTML 4.0//EN&quot; &quot;http://www.w3.org/TR/REC-html40/strict.dtd&quot;&gt;
&lt;html&gt;&lt;head&gt;&lt;meta name=&quot;qrichtext&quot; content=&quot;1&quot; /&gt;&lt;style type=&quot;text/css&quot;&gt;
p, li { white-space: pre-wrap; }
&lt;/style&gt;&lt;/head&gt;&lt;body style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt; font-weight:400; font-style:normal;&quot;&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;Saisissez le mot de passe du trousseau ci-après pour créer un nouveau trousseau &apos;&lt;span style=&quot; font-weight:600;&quot;&gt;%1&lt;/span&gt;&apos; pour l&apos;application &apos;&lt;span style=&quot; font-weight:600;&quot;&gt;%2&lt;/span&gt;&apos;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
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
        <translation>ERREUR</translation>
    </message>
    <message>
        <location filename="../src/checkforupdates.cpp" line="108"/>
        <location filename="../src/checkforupdates.cpp" line="111"/>
        <source>Version Info</source>
        <translation>Infos de Version</translation>
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
        <translation>&amp;Annuler</translation>
    </message>
    <message>
        <location filename="../src/configfileoption.ui" line="78"/>
        <source>Select A Folder To Create Config File In(Optional).</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>configOptions</name>
    <message>
        <location filename="../src/configoptions.ui" line="14"/>
        <source>Settings</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="30"/>
        <source>General Options</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="42"/>
        <source>Auto Open Mount Point</source>
        <translation>Ouverture automatique du point de montage</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="55"/>
        <source>Re Use Mount Point</source>
        <translation>Réutiliser le Point de Montage</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="68"/>
        <source>Auto Check For Updates</source>
        <translation>Mise à jour automatique du logiciel</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="81"/>
        <source>Start Minimized</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="94"/>
        <source>Select Language</source>
        <translation>Sélectionner la langue</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="100"/>
        <source>AutoMount</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="194"/>
        <source>&amp;KWallet</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="220"/>
        <source>&amp;None</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="253"/>
        <source>Editable Options</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="346"/>
        <source>Set External Plugin Executable</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="362"/>
        <source>Set Command To Run After Mount</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="398"/>
        <source>Set Command To Run Before UnMount</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="112"/>
        <source>Auto Mount Favorites Volumes At Startup</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="125"/>
        <source>Auto Mount Favorites Volumes When Available</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="138"/>
        <source>Show Mount Dialog When Auto Mounting</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="144"/>
        <source>Manage Keys In Wallets</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="156"/>
        <source>Set Default Wallet To Retrieve Volume Keys</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="168"/>
        <location filename="../src/configoptions.cpp" line="174"/>
        <source>Internal Wallet</source>
        <translation>Trousseau interne</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="181"/>
        <source>&amp;Libsecret</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/configoptions.cpp" line="192"/>
        <source>KWallet</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="207"/>
        <source>&amp;MACOS Keychain</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>None</source>
        <translation type="vanished">Aucun</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="234"/>
        <source>Change Internal Wallet Password</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="247"/>
        <source>&amp;Key Storage</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="265"/>
        <source>Set File Manager</source>
        <translation>Définir le gestionnaire de fichiers</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="314"/>
        <source>Set Mount Point Prefix</source>
        <translation>Paramétrer le préfixe du point de montage</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="416"/>
        <source>&amp;Close</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/configoptions.cpp" line="185"/>
        <source>Libsecret</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/configoptions.cpp" line="199"/>
        <source>MACOS Keychain</source>
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
        <translation>&amp;OK</translation>
    </message>
    <message>
        <location filename="../src/cryfscreateoptions.ui" line="94"/>
        <source>&amp;Cancel</source>
        <translation>&amp;Annuler</translation>
    </message>
    <message>
        <location filename="../src/cryfscreateoptions.ui" line="107"/>
        <source>Select A Folder To Create Config File In(Optional).</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>dialogok</name>
    <message>
        <location filename="../src/dialogok.ui" line="17"/>
        <source>Dialog</source>
        <translation>Boîte de dialogue</translation>
    </message>
    <message>
        <location filename="../src/dialogok.ui" line="29"/>
        <source>&amp;Ok</source>
        <translation>&amp;Ok</translation>
    </message>
    <message>
        <location filename="../src/dialogok.ui" line="67"/>
        <source>&amp;Yes</source>
        <translation>&amp;Oui</translation>
    </message>
    <message>
        <location filename="../src/dialogok.ui" line="80"/>
        <source>&amp;No</source>
        <translation>&amp;No</translation>
    </message>
</context>
<context>
    <name>favorites</name>
    <message>
        <location filename="../src/favorites.ui" line="17"/>
        <source>Manage Favorites</source>
        <translation>Gérer les Favoris</translation>
    </message>
    <message>
        <location filename="../src/favorites.ui" line="91"/>
        <source>Config File Path (Optional)</source>
        <translation>Chemin du fichier de configuration (optionnel)</translation>
    </message>
    <message>
        <location filename="../src/favorites.ui" line="107"/>
        <source>Idle TimeOut (Optional)</source>
        <translation>Délai d&apos;inactivité (optionnel)</translation>
    </message>
    <message>
        <location filename="../src/favorites.ui" line="196"/>
        <source>Mount Options</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/favorites.ui" line="207"/>
        <source>Auto Mount Volume</source>
        <translation>Montage Auto du Volume</translation>
    </message>
    <message>
        <location filename="../src/favorites.ui" line="120"/>
        <source>&amp;Add</source>
        <translation>&amp;Ajouter</translation>
    </message>
    <message>
        <location filename="../src/favorites.ui" line="156"/>
        <source>Volume ID</source>
        <translation>Volume ID</translation>
    </message>
    <message>
        <location filename="../src/favorites.ui" line="164"/>
        <source>Mount Point</source>
        <translation>Point de montage</translation>
    </message>
    <message>
        <location filename="../src/favorites.ui" line="172"/>
        <source>Auto Mount</source>
        <translation>Montage Auto</translation>
    </message>
    <message>
        <location filename="../src/favorites.ui" line="180"/>
        <source>Config File Path</source>
        <translation>Chemin du fichier de configuration</translation>
    </message>
    <message>
        <location filename="../src/favorites.ui" line="188"/>
        <source>Idle Time Out</source>
        <translation>Délai d&apos;inactivité</translation>
    </message>
    <message>
        <location filename="../src/favorites.ui" line="214"/>
        <source>Mount Options (Optional And Comma Separated)</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/favorites.ui" line="26"/>
        <source>Encrypted Folder Path</source>
        <translation>Chemin d&apos;accès du répertoire chiffré</translation>
    </message>
    <message>
        <location filename="../src/favorites.ui" line="58"/>
        <source>Mount Point Path</source>
        <translation>Ouvrir Boite de Dialogue de fichier</translation>
    </message>
    <message>
        <location filename="../src/favorites.ui" line="48"/>
        <source>open file dialog</source>
        <translation>Ouvrir Boite de Dialogue de fichier</translation>
    </message>
    <message>
        <location filename="../src/favorites.ui" line="130"/>
        <source>&amp;Done</source>
        <translation>&amp;Fini</translation>
    </message>
    <message>
        <location filename="../src/favorites.cpp" line="206"/>
        <source>Toggle AutoMount</source>
        <translation>Basculer Montage Auto</translation>
    </message>
    <message>
        <location filename="../src/favorites.cpp" line="211"/>
        <source>Remove Selected Entry</source>
        <translation>Enlever l&apos;entrée sélectionnée</translation>
    </message>
    <message>
        <location filename="../src/favorites.cpp" line="216"/>
        <source>Cancel</source>
        <translation>Annuler</translation>
    </message>
    <message>
        <location filename="../src/favorites.cpp" line="304"/>
        <location filename="../src/favorites.cpp" line="308"/>
        <location filename="../src/favorites.cpp" line="314"/>
        <source>ERROR!</source>
        <translation>ERREUR!</translation>
    </message>
    <message>
        <location filename="../src/favorites.cpp" line="304"/>
        <source>Encrypted Folder Address Field Is Empty</source>
        <translation>Le champ d&apos;adresse du répertoire chiffré est vide</translation>
    </message>
    <message>
        <location filename="../src/favorites.cpp" line="308"/>
        <source>Mount Point Path Field Is Empty</source>
        <translation>Le champ du chemin du point de montage est vide</translation>
    </message>
    <message>
        <location filename="../src/favorites.cpp" line="314"/>
        <source>Mount Options Can Not Contain &quot;-&quot;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/favorites.cpp" line="359"/>
        <source>Path To A Config File</source>
        <translation>Chemin vers un fichier de configuration</translation>
    </message>
    <message>
        <location filename="../src/favorites.cpp" line="400"/>
        <source>Path To An Encrypted Folder</source>
        <translation>Chemin d&apos;accès au répertoire chiffré</translation>
    </message>
    <message>
        <location filename="../src/favorites.cpp" line="410"/>
        <source>Path To Mount Folder</source>
        <translation>Chemin d&apos;accès au répertoire de montage</translation>
    </message>
</context>
<context>
    <name>fileManager</name>
    <message>
        <location filename="../src/filemanager.ui" line="14"/>
        <location filename="../src/filemanager.cpp" line="49"/>
        <source>Set File Manager</source>
        <translation type="unfinished">Définir le gestionnaire de fichiers</translation>
    </message>
    <message>
        <location filename="../src/filemanager.ui" line="26"/>
        <source>TextLabel</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/filemanager.ui" line="55"/>
        <source>&amp;Set</source>
        <translation></translation>
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
    <name>help</name>
    <message>
        <location filename="../src/help.ui" line="14"/>
        <source>FAQ</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/help.ui" line="26"/>
        <source>&amp;Close</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/help.ui" line="54"/>
        <source>&lt;!DOCTYPE HTML PUBLIC &quot;-//W3C//DTD HTML 4.0//EN&quot; &quot;http://www.w3.org/TR/REC-html40/strict.dtd&quot;&gt;
&lt;html&gt;&lt;head&gt;&lt;meta name=&quot;qrichtext&quot; content=&quot;1&quot; /&gt;&lt;style type=&quot;text/css&quot;&gt;
p, li { white-space: pre-wrap; }
&lt;/style&gt;&lt;/head&gt;&lt;body style=&quot; font-family:&apos;Noto Sans&apos;; font-size:10pt; font-weight:400; font-style:normal;&quot;&gt;
&lt;p style=&quot; margin-top:16px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;1.0 How do i change the default mount point prefix?&lt;/span&gt;&lt;/p&gt;
&lt;p style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;By default,SiriKali uses a mount point prefix of &amp;quot;~/.SiriKali.tmp/&amp;quot;.&lt;/p&gt;
&lt;p style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;You can change it to something else by clicking &amp;quot;Menu-&amp;gt;Set Mount Point Prefix&amp;quot; option and then set a folder you want to be used as a default mount point prefix.&lt;/p&gt;
&lt;p style=&quot; margin-top:16px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;a name=&quot;user-content-20-how-do-i-set-custom-mount-point-path-for-my-volume&quot;&gt;&lt;/a&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;2&lt;/span&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;.0 How do i set custom mount point path for my volume?&lt;/span&gt;&lt;/p&gt;
&lt;p style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;Unique custom mount point path can be used through adding a volume to the favorites list and then setting custom mount point path in the &amp;quot;mount point path&amp;quot; field.The volume should then be accessed through the favorites list.&lt;/p&gt;
&lt;p style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;Favorites list is access through Menu-&amp;gt;Favorites.&lt;/p&gt;
&lt;p style=&quot; margin-top:16px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;a name=&quot;user-content-30-how-do-i-start-the-application-minimized&quot;&gt;&lt;/a&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;3&lt;/span&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;.0 How do i start the application minimized?&lt;/span&gt;&lt;/p&gt;
&lt;p style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;Start the application with &amp;quot;-e&amp;quot; option. Run &amp;quot;sirikali --help&amp;quot; command from the terminal to get a list of all supported options.&lt;/p&gt;
&lt;p style=&quot; margin-top:16px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;a name=&quot;user-content-40-what-does-different-key-options-means-in-the-dialog-for-creatingunlocking-volume&quot;&gt;&lt;/a&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;4&lt;/span&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;.0 What does different key options means in the dialog for creating/unlocking volume?&lt;/span&gt;&lt;/p&gt;
&lt;ol style=&quot;margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;&quot;&gt;&lt;li style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&amp;quot;Key&amp;quot; entry means the password you enter will be used to create/unlock the volume.&lt;/li&gt;
&lt;li style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&amp;quot;KeyFile&amp;quot; entry means the contents of a specified file will be used as a password to create/unlock the volume. This option is not very safe as the file contents will most likely be truncated on the first encountered new line character making effective password not as strong as expected. It is advised to use one of the next two options if a key file is to be used.&lt;/li&gt;
&lt;li style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&amp;quot;HMAC+KeyFile&amp;quot; entry means the effective password will be a combination of key file contents and an empty password. The effective password will be produced with the following formula: password = hmac_sha256( key file contents,empty password ).&lt;/li&gt;
&lt;li style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&amp;quot;Key+KeyFile&amp;quot; entry means the effective password will be a combination of key file contents and a password. The effective password will be produced with the following formula: password = hmac_sha256( key file contents,password ).&lt;/li&gt;
&lt;li style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&amp;quot;Internal Wallet&amp;quot; entry means the effective password will be retrieved from LXQt internal secure storage system. Section 8.0 has information on how to add passwords to this backend.&lt;/li&gt;
&lt;li style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&amp;quot;KDE Wallet&amp;quot; entry means the effective password will be retrieved from LXQt secure storage system through KDE&apos;s KWallet backend. Section 8.0 has information on how to add passwords to this backend.&lt;/li&gt;
&lt;li style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&amp;quot;GNOME Wallet&amp;quot; entry means the effective password will be retrieved from LXQt secure storage system through libsecret backend. Section 8.0 has information on how to add passwords to this backend.&lt;/li&gt;
&lt;li style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&amp;quot;OSX keychain&amp;quot; entry means the effective password will be retrieved from LXQt secure storage system through OSX keychain backend. Section 8.0 has information on how to add passwords to this backend.&lt;/li&gt;&lt;/ol&gt;
&lt;p style=&quot; margin-top:16px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;a name=&quot;user-content-50-what-directories-does-sirikali-check-for-its-backend-executables&quot;&gt;&lt;/a&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;5&lt;/span&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;.0 what directories does SiriKali check for its backend executables?&lt;/span&gt;&lt;/p&gt;
&lt;p style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;Backend executables are searched in the following directories with the first entry searched first and the last entry searched last: &amp;quot;/usr/local/bin/&amp;quot;,&amp;quot;/usr/local/sbin/&amp;quot;,&amp;quot;/usr/bin/&amp;quot;,&amp;quot;/usr/sbin/&amp;quot;,&amp;quot;/bin/&amp;quot;,&amp;quot;/sbin/&amp;quot;, &amp;quot;/opt/local/bin&amp;quot;,&amp;quot;/opt/local/sbin&amp;quot;,&amp;quot;/opt/bin&amp;quot;,&amp;quot;/opt/sbin&amp;quot;,&amp;quot;/bin&amp;quot;,&amp;quot;/.bin&amp;quot;.&lt;/p&gt;
&lt;p style=&quot; margin-top:16px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;a name=&quot;user-content-60-how-do-i-unlock-a-volume-through-a-favorite-entry-without-invoking-a-wallet&quot;&gt;&lt;/a&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;6&lt;/span&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;.0 How do i unlock a volume through a favorite entry without invoking a wallet?&lt;/span&gt;&lt;/p&gt;
&lt;p style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;Disable the feature by setting an option through &amp;quot;menu-&amp;gt;automount favorites-&amp;gt;automount key source-&amp;gt;none&amp;quot;&lt;/p&gt;
&lt;p style=&quot; margin-top:16px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;a name=&quot;user-content-70-how-do-i-unlock-a-volume-through-a-favorite-entry-using-a-key-in-a-wallet-and-without-showing-mount-dialog-prompt&quot;&gt;&lt;/a&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;7&lt;/span&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;.0 How do i unlock a volume through a favorite entry using a key in a wallet and without showing mount dialog prompt?&lt;/span&gt;&lt;/p&gt;
&lt;ol style=&quot;margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;&quot;&gt;&lt;li style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;Set your preferred password storage system through &amp;quot;menu-&amp;gt;automount favorites-&amp;gt;automount key source&amp;quot;.&lt;/li&gt;
&lt;li style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;Unset &amp;quot;menu-&amp;gt;automount favorites-&amp;gt;show mount dialog when auto mounting&amp;quot;.&lt;/li&gt;&lt;/ol&gt;
&lt;p style=&quot; margin-top:16px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;a name=&quot;user-content-80-how-do-i-add-a-key-to-a-secured-storage-system&quot;&gt;&lt;/a&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;8&lt;/span&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;.0 How do i add a key to a secured storage system?&lt;/span&gt;&lt;/p&gt;
&lt;ol style=&quot;margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;&quot;&gt;&lt;li style=&quot; margin-top:12px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;Click &amp;quot;menu-&amp;gt;key storage&amp;quot; then select your preferred key storage system.&lt;/li&gt;
&lt;li style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;On the dialog window that shows up,click &amp;quot;Add&amp;quot;.&lt;/li&gt;
&lt;li style=&quot; margin-top:0px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;On the dialog window that shows up,add your password in the appropriate fields. &amp;quot;Volume ID&amp;quot; is the full path to the encrypted volume folder. Click the button to the right of the field to invoke file dialog window where you will get the opportunity to browse to where the folder is to select it.&lt;/li&gt;&lt;/ol&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/help.ui" line="99"/>
        <source>Open &amp;PDF</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>keyDialog</name>
    <message>
        <location filename="../src/keydialog.ui" line="29"/>
        <source>&amp;Open</source>
        <translation>&amp;Ouvrir</translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="45"/>
        <source>&amp;Cancel</source>
        <translation>&amp;Annuler</translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="58"/>
        <location filename="../src/keydialog.cpp" line="209"/>
        <location filename="../src/keydialog.cpp" line="1258"/>
        <location filename="../src/keydialog.cpp" line="1398"/>
        <source>Key</source>
        <translation>Clé</translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="100"/>
        <location filename="../src/keydialog.cpp" line="179"/>
        <source>Volume Name</source>
        <translation>Nom du montage</translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="142"/>
        <source>Mount In &amp;Read Only Mode</source>
        <translation>Monter en &amp;Lecture Seule</translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="220"/>
        <source>&amp;OK</source>
        <translation>&amp;OK</translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="290"/>
        <source>Set Key</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="303"/>
        <source>Cancel</source>
        <translation>Annuler</translation>
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
        <translation>O&amp;ptions</translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="188"/>
        <source>Volume Path</source>
        <translation>Chemin du volume</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="177"/>
        <source>&amp;Create</source>
        <translation>&amp;Créer</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="189"/>
        <location filename="../src/keydialog.cpp" line="373"/>
        <source>Unlocking &quot;%1&quot;</source>
        <translation>Déchiffrer &quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="191"/>
        <source>Mount Path</source>
        <translation>Chemin de montage</translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="339"/>
        <location filename="../src/keydialog.cpp" line="210"/>
        <location filename="../src/keydialog.cpp" line="1009"/>
        <location filename="../src/keydialog.cpp" line="1264"/>
        <location filename="../src/keydialog.cpp" line="1363"/>
        <source>KeyFile</source>
        <translation>Fichier de clé</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="212"/>
        <location filename="../src/keydialog.cpp" line="1299"/>
        <source>HMAC+KeyFile</source>
        <translation>HMAC+fichier de clé</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="211"/>
        <location filename="../src/keydialog.cpp" line="1272"/>
        <location filename="../src/keydialog.cpp" line="1276"/>
        <location filename="../src/keydialog.cpp" line="1377"/>
        <source>Key+KeyFile</source>
        <translation>Clé+fichier de clé</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="213"/>
        <location filename="../src/keydialog.cpp" line="1272"/>
        <source>ExternalExecutable</source>
        <translation>ExternalExecutable</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="253"/>
        <source>Check This Box To Make Password Visible</source>
        <translation>Cocher la case pour afficher le mot de passe en clair</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="271"/>
        <source>(%1/%2) Unlocking &quot;%3&quot;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="397"/>
        <source>Create A New &quot;%1&quot; Volume</source>
        <translation>Créer un nouveau volume &quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="485"/>
        <source>Passphrase Quality: 0%</source>
        <translation>Force de la Passphrase : 0%</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="487"/>
        <source>Passphrase Quality: %1%</source>
        <translation>Force de la Passphrase : %1%</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="492"/>
        <location filename="../src/keydialog.cpp" line="1326"/>
        <source>Passphrase Quality: 100%</source>
        <translation>Force de la Passphrase : 100%</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="543"/>
        <location filename="../src/keydialog.cpp" line="566"/>
        <source>Select A Folder To Create A Mount Point In.</source>
        <translation>Sélectionner un répertoire pour y créer un point de montage.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="673"/>
        <location filename="../src/keydialog.cpp" line="687"/>
        <source>Select A File To Be Used As A Keyfile.</source>
        <translation>Sélectionner un fichier afin de l&apos;utiliser comme un fichier de clé.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="713"/>
        <source>Volume Name Field Is Empty.</source>
        <translation>Le champ du Nom du Volume est vide.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="722"/>
        <source>Key Field Is Empty.</source>
        <translation>Champ de la clé est vide.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="775"/>
        <source>Internal Wallet Is Not Configured.</source>
        <translation>Trousseau Interne non configuré.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="820"/>
        <source>Failed To Unlock A Cryfs Volume.
Wrong Password Entered.</source>
        <translation>Echec de l&apos;ouverture du volume Cryfs.
Mot de passe entré incorrect.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="825"/>
        <source>Failed To Unlock An Encfs Volume.
Wrong Password Entered.</source>
        <translation>Echec de l&apos;ouverture du volume Encfs.
Mot de passe entré incorrect.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="830"/>
        <source>Failed To Unlock A Gocryptfs Volume.
Wrong Password Entered.</source>
        <translation>Echec de l&apos;ouverture du volume Gocryptfs.
Mot de passe entré incorrect.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="835"/>
        <source>Failed To Unlock An Ecryptfs Volume.
Wrong Password Entered.</source>
        <translation>Echec de l&apos;ouverture du volume Ecryptfs.
Mot de passe entré incorrect.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="840"/>
        <source>A Space Character Is Not Allowed In Paths When Using Ecryptfs Backend And Polkit.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="845"/>
        <source>This Backend Requires Root&apos;s Privileges And An attempt To Acquire Them Has Failed.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="850"/>
        <source>Failed To Unlock A Securefs Volume.
Wrong Password Entered.</source>
        <translation>Echec de l&apos;ouverture du volume Securefs.
Mot de passe entré incorrect.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="855"/>
        <source>Failed To Complete The Request.
Cryfs Executable Could Not Be Found.</source>
        <translation>Impossible de finir la requête.
L&apos;exécutable Cryfs est introuvable.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="860"/>
        <source>SiriKali Can Not Unlock This Volume Because Its FileSystem Has To Manually Be Converted To The Version Of Cryfs That Is Currently In Use.

Run Cryfs With This Volume To Manually Update This Volume&apos;s FileSystem.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="865"/>
        <source>Failed To Complete The Request.
Encfs Executable Could Not Be Found.</source>
        <translation>Impossible de finir la requête.
L&apos;exécutable Encfs est introuvable.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="870"/>
        <source>Failed To Complete The Request.
Ecryptfs-simple Executable Could Not Be Found.</source>
        <translation>Impossible de finir la requête.
L&apos;exécutable Ecryptfs est introuvable.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="875"/>
        <source>Failed To Complete The Request.
Gocryptfs Executable Could Not Be Found.</source>
        <translation>Impossible de finir la requête.
L&apos;exécutable Gocryptfs est introuvable.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="880"/>
        <source>Failed To Complete The Request.
Securefs Executable Could Not Be Found.</source>
        <translation>Impossible de finir la requête.
L&apos;exécutable Securefs est introuvable.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="885"/>
        <source>Failed To Create Mount Point.</source>
        <translation>Echec de la création du point de montage.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="890"/>
        <source>Failed To Unlock The Volume.
Not Supported Volume Encountered.</source>
        <translation>Echec de l&apos;ouverture du volume.
Volume non supporté rencontré.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="897"/>
        <source>Failed To Complete The Task And Below Log was Generated By The Backend.
</source>
        <translation>Échec pour compléter la tache, le log ci-dessous a été généré par l&apos;unité.
</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1049"/>
        <source>Failed To Generate Key.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1303"/>
        <source>Select A KeyFile</source>
        <translation>Sélectionner un fichier de clé</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="958"/>
        <source>Encrypted Folder Path Is Already Taken.</source>
        <translation>Chemin d&apos;accès du répertoire chiffré déjà occupé.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="967"/>
        <location filename="../src/keydialog.cpp" line="1119"/>
        <source>Mount Point Path Already Taken.</source>
        <translation>Point de montage déjà occupé.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="974"/>
        <location filename="../src/keydialog.cpp" line="1108"/>
        <location filename="../src/keydialog.cpp" line="1133"/>
        <source>Atleast One Required Field Is Empty.</source>
        <translation>Au moins un des champs qui est requis est vide.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1126"/>
        <source>Encrypted Folder Appear To Not Be Present.</source>
        <translation>Le chemin du répertoire chiffré ne semble pas présent.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1375"/>
        <location filename="../src/keydialog.cpp" line="1387"/>
        <source>Plugin name</source>
        <translation>Plugin name</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1412"/>
        <source>Keyfile path</source>
        <translation>Chemin du fichier de clé</translation>
    </message>
</context>
<context>
    <name>oneinstance</name>
    <message>
        <location filename="../src/oneinstance.cpp" line="69"/>
        <source>Previous instance seem to have crashed,trying to clean up before starting</source>
        <translation>L&apos;instance précédente semble avoir crashé, tentative de réinitialisation avant démarrage</translation>
    </message>
    <message>
        <location filename="../src/oneinstance.cpp" line="76"/>
        <source>There seem to be another instance running,exiting this one</source>
        <translation>Il semble y avoir une autre instance en cours d&apos;exécution, sortie de celle-ci</translation>
    </message>
</context>
<context>
    <name>options</name>
    <message>
        <location filename="../src/options.ui" line="17"/>
        <source>Set Mount Options</source>
        <translation>Paramétrer les options de montage</translation>
    </message>
    <message>
        <location filename="../src/options.ui" line="80"/>
        <source>Unlock A Cryfs/Gocryptfs/Securefs Volume With Specified Configuration File.</source>
        <translation>Déverrouiller un volume Cryfs/Gocryptfs/Securefs avec le fichier de configuration proposé.</translation>
    </message>
    <message>
        <location filename="../src/options.ui" line="109"/>
        <source>Automatically Unmount After Specified Minutes of Inactivity Is Reached.</source>
        <translation>Démonter automatiquement lorsque que le délai d&apos;inactivité spécifié -en minutes- est atteint.</translation>
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
        <translation>&amp;Annuler</translation>
    </message>
    <message>
        <location filename="../src/options.cpp" line="139"/>
        <source>Select Cryfs/Gocryptfs Configuration File</source>
        <translation>Sélectionner un fichier de configuration Cryfs/Gocryptfs</translation>
    </message>
    <message>
        <location filename="../src/options.cpp" line="171"/>
        <source>ERROR</source>
        <translation>ERREUR</translation>
    </message>
    <message>
        <location filename="../src/options.cpp" line="171"/>
        <source>Idle Time Field Requires Digits Only If Not Empty.</source>
        <translation>Le champ de délai d&apos;attente (idle) nécessite des nombres uniquement s&apos;il n&apos;est pas vide.</translation>
    </message>
</context>
<context>
    <name>password_dialog</name>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/password_dialog.ui" line="17"/>
        <source>lxqt wallet service</source>
        <translation>lxqt service de trousseau</translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/password_dialog.ui" line="45"/>
        <source>The application &apos;%1&apos; has requested to open the wallet &apos;%2&apos;.Enter the password below for this wallet to unlock it</source>
        <translation>L&apos;application &apos;%1&apos; a demandé d&apos;ouvrir le trousseau &apos;%2&apos;. Enter le mot de passe de ce trousseau ci-dessous pour le déverrouiller</translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/password_dialog.ui" line="77"/>
        <source>Wallet &apos;%1&apos; does not exist,do you want to create it?</source>
        <translation>Le trousseau &apos;%1&apos; n&apos;existe pas, voulez vous le créer ?</translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/password_dialog.ui" line="93"/>
        <source>&lt;!DOCTYPE HTML PUBLIC &quot;-//W3C//DTD HTML 4.0//EN&quot; &quot;http://www.w3.org/TR/REC-html40/strict.dtd&quot;&gt;
&lt;html&gt;&lt;head&gt;&lt;meta name=&quot;qrichtext&quot; content=&quot;1&quot; /&gt;&lt;style type=&quot;text/css&quot;&gt;
p, li { white-space: pre-wrap; }
&lt;/style&gt;&lt;/head&gt;&lt;body style=&quot; font-family:&apos;Monospace&apos;; font-size:10pt; font-weight:400; font-style:normal;&quot;&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt;&quot;&gt;Wallet &lt;/span&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt; font-weight:600;&quot;&gt;&apos;%1&apos;&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt;&quot;&gt; does not exist, do you want to create it?&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;!DOCTYPE HTML PUBLIC &quot;-//W3C//DTD HTML 4.0//EN&quot; &quot;http://www.w3.org/TR/REC-html40/strict.dtd&quot;&gt;
&lt;html&gt;&lt;head&gt;&lt;meta name=&quot;qrichtext&quot; content=&quot;1&quot; /&gt;&lt;style type=&quot;text/css&quot;&gt;
p, li { white-space: pre-wrap; }
&lt;/style&gt;&lt;/head&gt;&lt;body style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt; font-weight:400; font-style:normal;&quot;&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;Le trousseau &lt;span style=&quot; font-weight:600;&quot;&gt;&apos;%1&apos;&lt;/span&gt; n&apos;existe pas, voulez-vous en créer un ?&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/password_dialog.ui" line="110"/>
        <source>&lt;!DOCTYPE HTML PUBLIC &quot;-//W3C//DTD HTML 4.0//EN&quot; &quot;http://www.w3.org/TR/REC-html40/strict.dtd&quot;&gt;
&lt;html&gt;&lt;head&gt;&lt;meta name=&quot;qrichtext&quot; content=&quot;1&quot; /&gt;&lt;style type=&quot;text/css&quot;&gt;
p, li { white-space: pre-wrap; }
&lt;/style&gt;&lt;/head&gt;&lt;body style=&quot; font-family:&apos;Monospace&apos;; font-size:10pt; font-weight:400; font-style:normal;&quot;&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt;&quot;&gt;An application &lt;/span&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt; font-weight:600;&quot;&gt;&apos;%1&apos;&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt;&quot;&gt; has requested to open a wallet&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt; font-weight:600;&quot;&gt; &apos;%2&apos;&lt;/span&gt;&lt;span style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt;&quot;&gt;. Enter the password below for this wallet to unlock it.&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;!DOCTYPE HTML PUBLIC &quot;-//W3C//DTD HTML 4.0//EN&quot; &quot;http://www.w3.org/TR/REC-html40/strict.dtd&quot;&gt;
&lt;html&gt;&lt;head&gt;&lt;meta name=&quot;qrichtext&quot; content=&quot;1&quot; /&gt;&lt;style type=&quot;text/css&quot;&gt;
p, li { white-space: pre-wrap; }
&lt;/style&gt;&lt;/head&gt;&lt;body style=&quot; font-family:&apos;Sans Serif&apos;; font-size:9pt; font-weight:400; font-style:normal;&quot;&gt;
&lt;p style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;L&apos;application &lt;span style=&quot; font-weight:600;&quot;&gt;&apos;%1&apos;&lt;/span&gt; a demandé d&apos;ouvrir le trousseau&lt;span style=&quot; font-weight:600;&quot;&gt; &apos;%2&apos;&lt;/span&gt;.Saisissez le mot de passe du trousseau ci-après pour le déverrouiller.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/password_dialog.ui" line="127"/>
        <source>&amp;Unlock</source>
        <translation>&amp;Déverrouiller</translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/password_dialog.ui" line="144"/>
        <source>&amp;Cancel</source>
        <translation>&amp;Annuler</translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/password_dialog.ui" line="157"/>
        <source>ok</source>
        <translation>ok</translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/password_dialog.ui" line="170"/>
        <source>&amp;Ok</source>
        <translation>&amp;Ok</translation>
    </message>
</context>
<context>
    <name>plugin</name>
    <message>
        <location filename="../src/plugin.ui" line="17"/>
        <source>Key Generator Using A Passphrase And A KeyFile</source>
        <translation>Générateur de clé utilisant une passphrase et un fichier de clé</translation>
    </message>
    <message>
        <location filename="../src/plugin.ui" line="29"/>
        <source>&amp;Set Key</source>
        <translation>&amp;Paramétrer une clé</translation>
    </message>
    <message>
        <location filename="../src/plugin.ui" line="42"/>
        <source>&amp;Cancel</source>
        <translation>&amp;Annuler</translation>
    </message>
    <message>
        <location filename="../src/plugin.ui" line="113"/>
        <source>Passphrase</source>
        <translation>Passphrase</translation>
    </message>
    <message>
        <location filename="../src/plugin.ui" line="142"/>
        <location filename="../src/plugin.cpp" line="163"/>
        <source>KeyFile</source>
        <translation>Fichier de clé</translation>
    </message>
    <message>
        <location filename="../src/plugin.cpp" line="113"/>
        <location filename="../src/plugin.cpp" line="152"/>
        <source>ERROR</source>
        <translation>ERREUR</translation>
    </message>
    <message>
        <location filename="../src/plugin.cpp" line="113"/>
        <source>KeyFile Not Set</source>
        <translation>Fichier de clé non actif</translation>
    </message>
    <message>
        <location filename="../src/plugin.cpp" line="152"/>
        <source>Failed To Generate Key</source>
        <translation>Echec de la génération de clé</translation>
    </message>
</context>
<context>
    <name>readOnlyWarning</name>
    <message>
        <location filename="../src/readonlywarning.ui" line="17"/>
        <source>WARNING</source>
        <translation>ATTENTION</translation>
    </message>
    <message>
        <location filename="../src/readonlywarning.ui" line="29"/>
        <source>Do Not Show This Message Again.</source>
        <translation>Ne plus montrer ce message désormais.</translation>
    </message>
    <message>
        <location filename="../src/readonlywarning.ui" line="42"/>
        <source>Setting This Option Will Cause The Volume To Open In Read Only Mode.</source>
        <translation>Définir cette option ouvrira le volume en mode lecture seule.</translation>
    </message>
    <message>
        <location filename="../src/readonlywarning.ui" line="61"/>
        <source>&amp;Ok</source>
        <translation>&amp;Ok</translation>
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
        <translation>&amp;OK</translation>
    </message>
    <message>
        <location filename="../src/securefscreateoptions.ui" line="82"/>
        <source>&amp;Cancel</source>
        <translation>&amp;Annuler</translation>
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
        <translation>SiriKali</translation>
    </message>
    <message>
        <location filename="../src/sirikali.ui" line="55"/>
        <source>Volume Path</source>
        <translation>Chemin du volume</translation>
    </message>
    <message>
        <location filename="../src/sirikali.ui" line="63"/>
        <source>Mount Point Path</source>
        <translation>Chemin du point de montage</translation>
    </message>
    <message>
        <location filename="../src/sirikali.ui" line="71"/>
        <source>File System</source>
        <translation>Système de Fichiers</translation>
    </message>
    <message>
        <location filename="../src/sirikali.ui" line="79"/>
        <source>Mode</source>
        <translation>Mode</translation>
    </message>
    <message>
        <location filename="../src/sirikali.ui" line="95"/>
        <source>&amp;Create Volume</source>
        <translation>&amp;Créer un Volume</translation>
    </message>
    <message>
        <location filename="../src/sirikali.ui" line="108"/>
        <source>Mount &amp;Volume</source>
        <translation>Monter un &amp;Volume</translation>
    </message>
    <message>
        <location filename="../src/sirikali.ui" line="115"/>
        <source>&amp;Refresh</source>
        <translation>&amp;Rafraîchir</translation>
    </message>
    <message>
        <location filename="../src/sirikali.ui" line="129"/>
        <source>&amp;Menu</source>
        <translation>&amp;Menu</translation>
    </message>
    <message>
        <source>Auto Open Mount Point</source>
        <translation type="vanished">Ouverture automatique du point de montage</translation>
    </message>
    <message>
        <source>Reuse Mount Point</source>
        <translation type="vanished">Réutiliser le Point de Montage</translation>
    </message>
    <message>
        <source>Autocheck For Updates</source>
        <translation type="vanished">Mise à jour automatique du logiciel</translation>
    </message>
    <message>
        <source>Set Mount Point Prefix</source>
        <translation type="vanished">Paramétrer le préfixe du point de montage</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="301"/>
        <source>Unmount All</source>
        <translation>Démonter tout</translation>
    </message>
    <message>
        <source>AutoMount Favorites</source>
        <translation type="vanished">Montage automatique des Favoris</translation>
    </message>
    <message>
        <source>AutoMount Favorite Volumes At Start Up</source>
        <translation type="vanished">Montage automatique des Volumes Favoris au démarrage</translation>
    </message>
    <message>
        <source>AutoMount Favorite Volumes When Available</source>
        <translation type="vanished">Montage automatique des Volumes Favoris lorsqu&apos;ils sont disponibles</translation>
    </message>
    <message>
        <source>Internal Wallet</source>
        <translation type="vanished">Trousseau interne</translation>
    </message>
    <message>
        <source>Change Password</source>
        <translation type="vanished">Changer le mot de passe</translation>
    </message>
    <message>
        <source>Key Storage</source>
        <translation type="vanished">Clé de stockage</translation>
    </message>
    <message>
        <source>KDE Wallet</source>
        <translation type="vanished">Trousseau KDE</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="163"/>
        <location filename="../src/sirikali.cpp" line="165"/>
        <source>%1 Is Not Installed</source>
        <translation>%1 n&apos;est pas installé</translation>
    </message>
    <message>
        <source>AutoMount Key Source</source>
        <translation type="vanished">Source de la clé du Montage automatique</translation>
    </message>
    <message>
        <source>Gnome Wallet</source>
        <translation type="vanished">Trousseau Gnome</translation>
    </message>
    <message>
        <source>Show Mount Dialog When AutoMounting</source>
        <translation type="vanished">Montrer la fenêtre lors du Montage automatique</translation>
    </message>
    <message>
        <location filename="../src/sirikali.ui" line="122"/>
        <location filename="../src/sirikali.cpp" line="307"/>
        <source>Favorites</source>
        <translation>Favoris</translation>
    </message>
    <message>
        <source>Select Language</source>
        <translation type="vanished">Sélectionner la langue</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="298"/>
        <source>Check For Updates</source>
        <translation>Tester si la version du logiciel est à jour</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="314"/>
        <source>About</source>
        <translation>À propos</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="321"/>
        <source>Quit</source>
        <translation>Quitter</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="318"/>
        <source>Show/Hide</source>
        <translation>Montrer/Cacher</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="382"/>
        <location filename="../src/sirikali.cpp" line="407"/>
        <source>Manage Favorites</source>
        <translation>Gérer les favoris</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="386"/>
        <location filename="../src/sirikali.cpp" line="408"/>
        <source>Mount All</source>
        <translation>Monter tout</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="587"/>
        <source>ERROR: Volume Path Not Given.</source>
        <translation>ERREUR: Chemin du volume non fourni.</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="688"/>
        <source>ERROR: Key Not Found In The Backend.</source>
        <translation>ERREUR: Clé non trouvée dans le Gestionnaire.</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="693"/>
        <source>ERROR: Failed To Unlock Requested Backend.</source>
        <translation>ERREUR: Echec du déverrouillage du Gestionnaire sélectionné.</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="458"/>
        <location filename="../src/sirikali.cpp" line="506"/>
        <location filename="../src/sirikali.cpp" line="889"/>
        <location filename="../src/sirikali.cpp" line="1316"/>
        <location filename="../src/sirikali.cpp" line="1432"/>
        <source>ERROR</source>
        <translation>ERREUR</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="889"/>
        <source>Failed To Read Volume Properties</source>
        <translation>Echec de la lecture des propriétés du volume</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="848"/>
        <location filename="../src/sirikali.cpp" line="894"/>
        <source>INFORMATION</source>
        <translation>INFORMATION</translation>
    </message>
    <message>
        <source>None</source>
        <translation type="vanished">Aucun</translation>
    </message>
    <message>
        <source>Set File Manager</source>
        <translation type="vanished">Définir le gestionnaire de fichiers</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="304"/>
        <source>Unmount All And Quit</source>
        <translation>Démonter tout et quitter</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="311"/>
        <source>Settings</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="316"/>
        <source>FAQ</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="458"/>
        <source>SiriKali Failed To Connect To siriPolkit.
Please Report This Serious Bug.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="506"/>
        <source>&quot;%1&quot; Folder Must Be Writable</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="933"/>
        <source>Block Size: %1</source>
        <translation>Taille des blocs: %1</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="934"/>
        <source>Used Blocks: %2</source>
        <translation>Blocs utilisés: %2</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="935"/>
        <source>Free Blocks: %3</source>
        <translation>Blocs Libres: %3</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="936"/>
        <source>Total Blocks %4</source>
        <translation>Total des Blocs %4</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="937"/>
        <source>Used Space: %5</source>
        <translation>Espace Utilisé: %5</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="938"/>
        <source>Free Space: %6</source>
        <translation>Espace Libre: %6</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="939"/>
        <source>Total Space: %7</source>
        <translation>Espace Total: %7</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="940"/>
        <source>Used %: %8</source>
        <translation>Utilisé %: %8</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1091"/>
        <source>Unmount</source>
        <translation>Démonter</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1089"/>
        <source>Open Folder</source>
        <translation>Ouvrir Répertoire</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1093"/>
        <source>Properties</source>
        <translation>Propriétés</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1130"/>
        <source>Close Menu</source>
        <translation>Fermer le Menu</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1182"/>
        <source>WARNING</source>
        <translation>Attention</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1183"/>
        <source>Could Not Open Mount Point Because &quot;%1&quot; Tool Does Not Appear To Be Working Correctly.</source>
        <translation>Impossible d&apos;ouvrir le point de montage parce que l&apos;outil &quot;%1&quot; ne semble pas fonctionner correctement.</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1230"/>
        <source>&amp;Quit</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1317"/>
        <source>Permission To Access The Volume Was Denied
Or
The Volume Is Not Supported</source>
        <translation>La permission d&apos;accéder au volume a été refusée
ou
le volume n&apos;est pas supporté</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1337"/>
        <source>Select An Encrypted Volume Directory</source>
        <translation>Sélectionner un répertoire de volume</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1432"/>
        <source>Failed To Unmount %1 Volume</source>
        <translation>Impossible de démonter le Volume %1</translation>
    </message>
</context>
<context>
    <name>walletconfig</name>
    <message>
        <location filename="../src/walletconfig.ui" line="17"/>
        <source>Manage Volumes In A Wallet</source>
        <translation>Gérer les volumes dans un trousseau</translation>
    </message>
    <message>
        <location filename="../src/walletconfig.ui" line="32"/>
        <source>&amp;Add</source>
        <translation>&amp;Ajouter</translation>
    </message>
    <message>
        <location filename="../src/walletconfig.ui" line="51"/>
        <source>&amp;Delete</source>
        <translation>&amp;Effacer</translation>
    </message>
    <message>
        <location filename="../src/walletconfig.ui" line="64"/>
        <source>Do&amp;ne</source>
        <translation>&amp;Fini</translation>
    </message>
    <message>
        <location filename="../src/walletconfig.ui" line="111"/>
        <source>Volume ID</source>
        <translation>Volume ID</translation>
    </message>
    <message>
        <location filename="../src/walletconfig.ui" line="116"/>
        <source>Comment</source>
        <translation>Commentaires</translation>
    </message>
    <message>
        <location filename="../src/walletconfig.cpp" line="106"/>
        <source>Delete Entry</source>
        <translation>Supprimer l&apos;entrée</translation>
    </message>
    <message>
        <location filename="../src/walletconfig.cpp" line="119"/>
        <source>WARNING!</source>
        <translation>ATTENTION!</translation>
    </message>
    <message>
        <location filename="../src/walletconfig.cpp" line="119"/>
        <source>Are you sure you want to delete a volume with an id of &quot;%1&quot;?</source>
        <translation>Êtes vous sûr d&apos;effacer un volume avec un ID de &quot;%1&quot;?</translation>
    </message>
    <message>
        <location filename="../src/walletconfig.cpp" line="203"/>
        <source>ERROR!</source>
        <translation>ERREUR!</translation>
    </message>
    <message>
        <location filename="../src/walletconfig.cpp" line="203"/>
        <source>Failed To Add the Key In The Wallet.</source>
        <translation>Echec de l&apos;ajout de la clé dans le trousseau.</translation>
    </message>
</context>
<context>
    <name>walletconfiginput</name>
    <message>
        <location filename="../src/walletconfiginput.ui" line="17"/>
        <source>Add An Entry To Wallet</source>
        <translation>Ajouter une entrée au Trousseau</translation>
    </message>
    <message>
        <location filename="../src/walletconfiginput.ui" line="29"/>
        <source>&amp;Add</source>
        <translation>&amp;Ajouter</translation>
    </message>
    <message>
        <location filename="../src/walletconfiginput.ui" line="42"/>
        <source>&amp;Cancel</source>
        <translation>&amp;Annuler</translation>
    </message>
    <message>
        <location filename="../src/walletconfiginput.ui" line="64"/>
        <source>Volume ID</source>
        <translation>Volume ID</translation>
    </message>
    <message>
        <location filename="../src/walletconfiginput.ui" line="83"/>
        <source>Comment</source>
        <translation>Commentaires</translation>
    </message>
    <message>
        <location filename="../src/walletconfiginput.ui" line="102"/>
        <source>Key</source>
        <translation>Clé</translation>
    </message>
    <message>
        <location filename="../src/walletconfiginput.ui" line="121"/>
        <source>Repeat Key</source>
        <translation>Réentrer la clé</translation>
    </message>
    <message>
        <location filename="../src/walletconfiginput.cpp" line="102"/>
        <location filename="../src/walletconfiginput.cpp" line="109"/>
        <source>ERROR!</source>
        <translation>ERREUR!</translation>
    </message>
    <message>
        <location filename="../src/walletconfiginput.cpp" line="102"/>
        <source>Atleast one required field is empty</source>
        <translation>Au moins un des champs qui est requis est vide</translation>
    </message>
    <message>
        <location filename="../src/walletconfiginput.cpp" line="109"/>
        <source>Passphrases do not match</source>
        <translation>Passphrases non correspondantes</translation>
    </message>
    <message>
        <location filename="../src/walletconfiginput.cpp" line="132"/>
        <source>Select A Volume</source>
        <translation>Sélectionner un volume</translation>
    </message>
</context>
</TS>
