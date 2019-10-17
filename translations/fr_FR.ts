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
        <location filename="../src/dialogmsg.cpp" line="98"/>
        <source>INFORMATION</source>
        <translation>INFORMATION</translation>
    </message>
    <message>
        <location filename="../src/dialogmsg.cpp" line="238"/>
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
        <location filename="../src/utility.cpp" line="584"/>
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
        <translation>
Options:
	-d   localisation du chemin d&apos;un volume pour être auto déverrouillé / monté
	-m   à utiliser pour ouvrir un gestionnaire de fichiers par défaut (l&apos;outil par défaut est xdg-open)
	-e   démarre l&apos;application sans afficher l&apos;interface graphique
	-b   Nom du processus pour  to récupérer le mot de passe du volume quand il est ouvert depuis la ligne de commande CLI.
	     Les processus supportés sont : &quot;internal&quot;,&quot;stdin&quot;,&quot;keyfile&quot;,&quot;osxkeychain&quot;,&quot;kwallet&quot; and &quot;libsecret&quot;.
	     Les trois premiers sont toujours dispo mais les autres ont des dépendances à compiler.
	     &quot;internal&quot; option amène SiriKali à lire le mot de passe depuis lxqt-wallet internal backend.
	     &quot;stdin&quot; option amène SiriKali à lire le mot de passe depuis standard input.
	     &quot;keyfile&quot; option amène SiriKali à lire le mot de passe depuis un simple fichier.
	     &quot;libsecret&quot; option amène SiriKali à lire le mot de passe depuis lxqt-wallet libsecret backend.
	     &quot;kwallet&quot; option amène SiriKali à lire le mot de passe depuis lxqt-wallet kwallet backend.
	     &quot;osxkeychain&quot; option amène SiriKali à lire le mot de passe depuis lxqt-wallet OSX key chain backend.
	-k   quand il est ouvert depuis la ligne de commande CLI, la valeur &quot;rw&quot; ouvrira le volume en mode read\write (lecture/écriture)
	     la valeur &quot;ro&quot; ouvrira le volume en mode read only (lecture seule).
	-z   Chemin complet du point de montage à utiliser quand il est ouvert depuis la ligne de commande CLI.
	     Cette option est toutefois facultative.
	-c   Paramètre le fichier de configuration du volume quand il est ouvert depuis la ligne de commande CLI.
	-i   Paramètre le temps d&apos;inactivité (en minutes) pour démonter le volume quand le montage a été fait en ligne de commande CLI.
	-o   Paramètre les options de montage quand le montage va être fait en ligne de commande CLI.
	-f   Chemin vers le fichier de clé.
	-u   Démonte le volume.
	-p   Affiche la liste des volumes déchiffrés.
	-s   Option pour déclencher la génération du hash du mot de passe.</translation>
    </message>
    <message>
        <location filename="../src/utility.cpp" line="832"/>
        <source>about SiriKali</source>
        <translation>A propos de SiriKali</translation>
    </message>
    <message>
        <location filename="../src/utility.cpp" line="1481"/>
        <source>Policy:</source>
        <translation>Règle:</translation>
    </message>
    <message>
        <location filename="../src/utility.cpp" line="1486"/>
        <source>Comment:</source>
        <translation>Commentaire:</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1628"/>
        <source>This plugin delegates key generation to an external application</source>
        <translation>Ce plugin transmet la génération de clé à une application extérieure</translation>
    </message>
    <message>
        <location filename="../src/checkforupdates.cpp" line="67"/>
        <source>%1&quot;%2&quot; Installed Version Is : %3.
Latest Version Is : %4.</source>
        <translation>%1&quot;%2&quot; version installée : %3.
Dernière version disponible : %4.</translation>
    </message>
    <message>
        <location filename="../src/checkforupdates.cpp" line="197"/>
        <source>Network Request Failed To Respond Within %1 Seconds.</source>
        <translation>Le réseau n&apos;a pas répondu dans l&apos;espace de temps de %1 secondes.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="45"/>
        <source>Kde Wallet</source>
        <translation>Trousseau Kde</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="50"/>
        <source>Internal Wallet</source>
        <translation>Trousseau interne</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="55"/>
        <source>Gnome Wallet</source>
        <translation>Trousseau Gnome</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="60"/>
        <source>OSX KeyChain</source>
        <translation>Porte clés OSX</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1564"/>
        <source>Not Supported KeyFile Encountered Since It Contains AtLeast One Illegal Character(&apos;\n&apos;,&apos;\0&apos;,&apos;\r&apos;).

Please Use a Hash Of The KeyFile Through &quot;HMAC+KeyFile&quot; Option.</source>
        <translation>Pas de fichier de clé supporté s&apos;il contient ne serait ce qu&apos;un caractère illégal (&apos;\n&apos;,&apos;\0&apos;,&apos;\r&apos;).

Merci d&apos;utiliser un hashage de la clé de fichier au travers de l&apos;option &quot;HMAC+Fichier de clé&quot;.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1624"/>
        <source>Effective Key Is Generated With Below Formula:

key = hmac_sha256(password,keyfile contents)</source>
        <translation>La clé actuelle est générée avec la formule ci-dessous:

clé = hmac_sha256(mot de passe,contenu du fichier de clé)</translation>
    </message>
    <message>
        <location filename="../src/checkforupdates.cpp" line="199"/>
        <location filename="../src/sirikali.cpp" line="508"/>
        <location filename="../src/sirikali.cpp" line="516"/>
        <source>ERROR</source>
        <translation>ERREUR</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="684"/>
        <source>Volume Created Successfully.</source>
        <translation>Volume créé avec succès.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="688"/>
        <source>Backend Requires A Password.</source>
        <translation>Le processus nécessite un mot de passe.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="692"/>
        <source>Failed To Unlock A Cryfs Volume.
Wrong Password Entered.</source>
        <translation>Echec de l&apos;ouverture du volume Cryfs.
Mot de passe entré incorrect.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="696"/>
        <source>Failed To Connect To The Remote Computer.
Wrong Password Entered.</source>
        <translation>Echec de la connexion à l&apos;ordinateur distant
Mot de passe erroné.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="700"/>
        <source>Failed To Unlock An Encfs Volume.
Wrong Password Entered.</source>
        <translation>Echec de l&apos;ouverture du volume Encfs.
Mot de passe entré incorrect.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="704"/>
        <source>Failed To Unlock A Gocryptfs Volume.
Wrong Password Entered.</source>
        <translation>Echec de l&apos;ouverture du volume Gocryptfs.
Mot de passe entré incorrect.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="708"/>
        <source>Failed To Unlock An Ecryptfs Volume.
Wrong Password Entered.</source>
        <translation>Echec de l&apos;ouverture du volume Ecryptfs.
Mot de passe entré incorrect.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="712"/>
        <source>Failed To Unlock An fscrypt Volume.
Wrong Password Entered.</source>
        <translation>Echec de l&apos;ouverture du volume fscrypt.
Mot de passe entré incorrect.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="716"/>
        <source>Backend Requires Polkit Support and SiriKali Failed To Start It.</source>
        <translation>Le processus nécessite le support de Polkit et SiriKali n&apos;a pas réussi à le démarrer.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="720"/>
        <source>A Space Character Is Not Allowed In Paths When Using Ecryptfs Backend And Polkit.</source>
        <translation>Le caractére espace n&apos;est pas autorisé quand on utilise un processus Ecryptfs et le Polkit.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="724"/>
        <source>This Backend Requires Root&apos;s Privileges And An attempt To Acquire Them Has Failed.</source>
        <translation>Ce processus nécessite des privilèges Root et la tentative de les obtenir a échoué.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="728"/>
        <source>Failed To Unlock A Securefs Volume.
Wrong Password Entered.</source>
        <translation>Echec de l&apos;ouverture du volume Securefs.
Mot de passe entré incorrect.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="732"/>
        <source>Failed To Unlock A Custom Volume.
Wrong Password Entered.</source>
        <translation>Echec de l&apos;ouverture du volume personnalisé.
Mot de passe entré incorrect.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="736"/>
        <location filename="../src/sirikali.cpp" line="506"/>
        <source>Failed To Complete The Request.
Sshfs Executable Could Not Be Found.</source>
        <translation>Echec pour compléter la requête.
L&apos;exécutable Sshfs n&apos;a pu être trouvé.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="740"/>
        <source>Failed To Complete The Request.
Fscrypt Executable Could Not Be Found.</source>
        <translation>Impossible de finir la requête.
L&apos;exécutable Fscrypt est introuvable.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="744"/>
        <source>Backend Does Not Support Custom Configuration File Path.</source>
        <translation>Le processus ne supporte pas un fichier de configuration personnalisé du chemin.d&apos;accès.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="748"/>
        <source>Failed To Complete The Request.
Cryfs Executable Could Not Be Found.</source>
        <translation>Impossible de finir la requête.
L&apos;exécutable Cryfs est introuvable.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="752"/>
        <source>Something Is Wrong With The Backend And It Took Too Long To Respond.</source>
        <translation>Quelque chose s&apos;est mal passé dans le processus et il prend trop de temps à répondre.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="756"/>
        <source>This Volume Of Cryfs Needs To Be Upgraded To Work With The Version Of Cryfs You Are Using.

The Upgrade is IRREVERSIBLE And The Volume Will No Longer Work With Older Versions of Cryfs.

To Do The Upgrade, Check The &quot;Upgrade File System&quot; Option And Unlock The Volume Again.</source>
        <translation>Ce volume Cryfs nécessite un système de fichiers mis à jour pour fonctionner avec la version de Cryfs que vous utilisez.

La mise à jour est IRREVERSIBLE et le volume ne fonctionnera alors plus avec l&apos;ancienne version de Cryfs.

Pour réaliser la mise à jour, cochez l&apos;option &quot;Mettre à jour le système de fichiers&quot; et déchiffrez à nouveau le volume.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="760"/>
        <source>Failed To Complete The Request.
Encfs Executable Could Not Be Found.</source>
        <translation>Impossible de finir la requête.
L&apos;exécutable Encfs est introuvable.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="764"/>
        <source>Failed To Complete The Request.
Ecryptfs-simple Executable Could Not Be Found.</source>
        <translation>Impossible de finir la requête.
L&apos;exécutable Ecryptfs est introuvable.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="768"/>
        <source>Failed To Complete The Request.
Gocryptfs Executable Could Not Be Found.</source>
        <translation>Impossible de finir la requête.
L&apos;exécutable Gocryptfs est introuvable.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="772"/>
        <source>Failed To Complete The Request.
Securefs Executable Could Not Be Found.</source>
        <translation>Impossible de finir la requête.
L&apos;exécutable Securefs est introuvable.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="776"/>
        <source>Failed To Create Mount Point.</source>
        <translation>Echec de la création du point de montage.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="780"/>
        <source>Backend Could Not Load WinFsp. Please Make Sure You Have WinFsp Properly Installed.</source>
        <translation>Le processus n&apos;a pas pu charger WinFsp. Veuillez vérifier que vous ayez installé WinFsp correctement.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="784"/>
        <source>Failed To Unlock The Volume.
Not Supported Volume Encountered.</source>
        <translation>Echec de l&apos;ouverture du volume.
Volume non supporté rencontré.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="788"/>
        <source>Installed &quot;%1&quot; Version Is Too Old.
 Please Update To Atleast Version %2.</source>
        <translation>La version &quot;%1&quot; installée est trop ancienne.
 Veuillez mettre à jour vers une version %2.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="792"/>
        <source>Failed To Complete The Request.
The Executable For This Backend Could Not Be Found.</source>
        <translation>Impossible de finir la requête.
L&apos;exécutable de ce processus est introuvable.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="800"/>
        <source>Invalid Config File Name.
Its Name Must End With &quot;%1&quot;</source>
        <translation>Nom de fichier de configuration invalide.
Son nom doit se terminer avec &quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="803"/>
        <source>Invalid Config File Name.
It Must End With One Of The Following:
&quot;%1&quot;</source>
        <translation>Nom de fichier de configuration invalide.
Il doit se terminer avec un des termes suivants : &quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="812"/>
        <source>Failed To Complete The Task And Below Log was Generated By The Backend.
</source>
        <translation>Échec pour terminer la tâche, le log ci-dessous a été généré par le processus.
</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="512"/>
        <source>Failed To Connect To &quot;%1&quot; And Below Log was Generated By The Backend.
</source>
        <translation>Échec pour se connecter à &quot;%1&quot; le log ci-dessous a été généré par le processus.
</translation>
    </message>
    <message>
        <location filename="../src/settings.cpp" line="434"/>
        <source>Manage Favorites</source>
        <translation>Gérer les Favoris</translation>
    </message>
    <message>
        <location filename="../src/settings.cpp" line="435"/>
        <source>Mount All</source>
        <translation>Monter Tout</translation>
    </message>
    <message>
        <location filename="../src/settings.cpp" line="1084"/>
        <source>Russian (RU)</source>
        <translation>Russe (RU)</translation>
    </message>
    <message>
        <location filename="../src/settings.cpp" line="1085"/>
        <source>French (FR)</source>
        <translation>Français (FR)</translation>
    </message>
    <message>
        <location filename="../src/settings.cpp" line="1086"/>
        <source>German (DE)</source>
        <translation>Allemand (DE)</translation>
    </message>
    <message>
        <location filename="../src/settings.cpp" line="1087"/>
        <source>English (US)</source>
        <translation>Anglais (US)</translation>
    </message>
    <message>
        <location filename="../src/settings.cpp" line="1088"/>
        <source>Swedish (SE)</source>
        <translation>Suedois (SE)</translation>
    </message>
    <message>
        <location filename="../src/settings.cpp" line="1089"/>
        <source>Arabic (SA)</source>
        <translation>Arabe (SA)</translation>
    </message>
    <message>
        <location filename="../src/settings.cpp" line="1090"/>
        <source>Spanish (MX)</source>
        <translation>Espagnol (MX)</translation>
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
        <location filename="../src/checkforupdates.cpp" line="92"/>
        <location filename="../src/checkforupdates.cpp" line="95"/>
        <source>Version Info</source>
        <translation>Infos de Version</translation>
    </message>
</context>
<context>
    <name>configFileOption</name>
    <message>
        <location filename="../src/configfileoption.ui" line="14"/>
        <source>Set Config File Path</source>
        <translation>Choisir le chemin du fichier de configuration</translation>
    </message>
    <message>
        <location filename="../src/configfileoption.ui" line="26"/>
        <source>&amp;Set</source>
        <translation>&amp;Paramétrer</translation>
    </message>
    <message>
        <location filename="../src/configfileoption.ui" line="39"/>
        <source>&amp;Cancel</source>
        <translation>&amp;Annuler</translation>
    </message>
    <message>
        <location filename="../src/configfileoption.ui" line="78"/>
        <source>Select A Folder To Create Config File In(Optional).</source>
        <translation>Sélectionner un répertoire pour y créer un fichier de configuration (facultatif).</translation>
    </message>
</context>
<context>
    <name>configOptions</name>
    <message>
        <location filename="../src/configoptions.ui" line="14"/>
        <source>Settings</source>
        <translation>Paramétrages</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="30"/>
        <source>General Options</source>
        <translation>Options Générales</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="107"/>
        <source>Auto Open Mount Point</source>
        <translatorcomment>Ouverture automatique du point de montage</translatorcomment>
        <translation>Ouverture automatique du point de montage</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="123"/>
        <source>Re Use Mount Point</source>
        <translation>Réutiliser le Point de Montage</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="139"/>
        <source>Auto Check For Updates</source>
        <translation>Mise à jour automatique du logiciel</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="155"/>
        <source>Start Minimized</source>
        <translation>Lancer Réduit (barre des tâches)</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="94"/>
        <source>Select Language</source>
        <translation>Sélectionner la langue</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="404"/>
        <source>SiriKali Will Have To Be Restarted For These Changes To Take Effect.</source>
        <translation>SiriKali doit être redémarré pour que les changements prennent effet.</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="467"/>
        <source>&amp;KWallet</source>
        <translation>&amp;KWallet</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="493"/>
        <source>&amp;None</source>
        <translation>&amp;Aucun</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="619"/>
        <location filename="../src/configoptions.cpp" line="306"/>
        <source>Set External Plugin Executable</source>
        <translation>Choisir le plugin exécutable externe</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="638"/>
        <source>Set Command To Run After Mount</source>
        <translation>Choisir les commandes à exécuter après montage</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="677"/>
        <source>Set Command To Run Before UnMount</source>
        <translation>Choisir les commandes à exécuter avant montage</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="184"/>
        <source>Auto Mount Favorites Volumes At Startup</source>
        <translation>Montage Automatique des volumes au démarrage</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="219"/>
        <source>Auto Mount Favorites Volumes When Available</source>
        <translation>Montage Automatique des volumes lorsqu&apos;ils sont disponibles</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="248"/>
        <source>Show Mount Dialog When Auto Mounting</source>
        <translation>Montrer la boîte de dialogue lors du Montage Automatique</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="277"/>
        <source>Allow External Tools To Get Passwords From Wallets</source>
        <translation>Autoriser des outils externes pour obtenir les mots de passe des trousseaux</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="306"/>
        <source>Show Favorite List On Context Menu(Restart Required)</source>
        <translation>Montrer la liste des favoris dans le menu contextuel (Redémarrage requis)</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="315"/>
        <source>HiDPI Settings</source>
        <translation>Réglages HiDPI</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="350"/>
        <source>Scale Factor
(Enter A Decimal Number)</source>
        <translation>Facteur d&apos;échelle
(Entrer un nombre décimal)</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="370"/>
        <source>Enable HiDPI Scalling</source>
        <translation>Activer la mise à l&apos;échelle HiDPI</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="417"/>
        <source>Manage Keys In Wallets</source>
        <translation>Gérer les clés dans les trousseaux</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="429"/>
        <source>Set Default Wallet To Retrieve Volume Keys</source>
        <translation>Choisir le trousseau par défaut pour récupérer les clés de volume</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="441"/>
        <location filename="../src/configoptions.cpp" line="190"/>
        <source>Internal Wallet</source>
        <translation>Trousseau interne</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="454"/>
        <source>&amp;Libsecret</source>
        <translation>&amp;Libsecret</translation>
    </message>
    <message>
        <location filename="../src/configoptions.cpp" line="192"/>
        <source>KWallet</source>
        <translation>KWallet</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="480"/>
        <source>&amp;MACOS Keychain</source>
        <translation>Porte clé &amp;MACOS</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="507"/>
        <source>Change Internal Wallet Password</source>
        <translation>Modifier le mot de passe interne du trousseau</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="520"/>
        <source>&amp;Key Storage</source>
        <translation>&amp;Clé de stockage</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="526"/>
        <source>External Commands</source>
        <translation>Commandes externes</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="538"/>
        <source>Set File Manager</source>
        <translation>Définir le gestionnaire de fichiers</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="587"/>
        <source>Set Mount Point Prefix</source>
        <translation>Paramétrer le préfixe du point de montage</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="722"/>
        <source>Set Command To Run Periodically</source>
        <translation>Paramétrer la commande pour s&apos;exécuter périodiquement</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="774"/>
        <source>Interval In Minutes Periodic Command Will Run</source>
        <translation>Intervalle périodique en minutes entre laquelle la commande s&apos;exécutera</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="819"/>
        <location filename="../src/configoptions.ui" line="835"/>
        <location filename="../src/configoptions.ui" line="851"/>
        <location filename="../src/configoptions.ui" line="867"/>
        <location filename="../src/configoptions.ui" line="883"/>
        <location filename="../src/configoptions.ui" line="899"/>
        <source>Set To Default</source>
        <translation>Paramétrer par défaut</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="917"/>
        <source>&amp;Close</source>
        <translation>Fermer &amp;C</translation>
    </message>
    <message>
        <location filename="../src/configoptions.cpp" line="100"/>
        <location filename="../src/configoptions.cpp" line="370"/>
        <source>Set Executables Search Path</source>
        <translation>Choisir le chemin de recherche des exécutables</translation>
    </message>
    <message>
        <location filename="../src/configoptions.cpp" line="191"/>
        <source>Libsecret</source>
        <translation>Libsecret</translation>
    </message>
    <message>
        <location filename="../src/configoptions.cpp" line="193"/>
        <source>MACOS Keychain</source>
        <translation>Porte clés MACOS</translation>
    </message>
    <message>
        <location filename="../src/configoptions.cpp" line="266"/>
        <source>Set Post Mount Command</source>
        <translation>Paramétrer la commande de prémontage</translation>
    </message>
    <message>
        <location filename="../src/configoptions.cpp" line="276"/>
        <location filename="../src/configoptions.cpp" line="286"/>
        <source>Set Pre UnMount Command</source>
        <translation>Paramétrer la commande de prédémontage</translation>
    </message>
    <message>
        <location filename="../src/configoptions.cpp" line="296"/>
        <source>Set Command To Open Mount Points</source>
        <translation>Paramétrer la commande pour ouvrir les points de montage</translation>
    </message>
</context>
<context>
    <name>cryfsWarning</name>
    <message>
        <location filename="../src/keydialog.h" line="68"/>
        <source>Please be patient because creating a CryFS volume may take a very long time.

</source>
        <translation>Merci de bien vouloir patienter, la création d&apos;un volume CryFS prend un temps assez long.

</translation>
    </message>
    <message>
        <location filename="../src/keydialog.h" line="73"/>
        <source>Please be patient because unlocking a CryFS volume may take a very long time.

</source>
        <translation>Merci de bien vouloir patienter, le déchiffrement d&apos;un volume CryFS prend un temps assez long.

</translation>
    </message>
    <message>
        <location filename="../src/keydialog.h" line="81"/>
        <source>Elapsed time: 0 seconds</source>
        <translation>Temps écoulé: 0 secondes</translation>
    </message>
    <message>
        <location filename="../src/keydialog.h" line="103"/>
        <source>Elapsed time: %0 minutes</source>
        <translation>Temps écoulé: %0 minutes</translation>
    </message>
    <message>
        <location filename="../src/keydialog.h" line="105"/>
        <source>Elapsed time: %0 seconds</source>
        <translation>Temps écoulé: %0 secondes</translation>
    </message>
</context>
<context>
    <name>cryfscreateoptions</name>
    <message>
        <location filename="../src/engines/cryfscreateoptions.ui" line="14"/>
        <source>Select Volume Creation Options</source>
        <translation>Sélectionner les options de création du volume</translation>
    </message>
    <message>
        <location filename="../src/engines/cryfscreateoptions.ui" line="36"/>
        <source>Cipher To Use For Encryption.</source>
        <translation>Chiffrement à utiliser pour l&apos;encryptage.</translation>
    </message>
    <message>
        <location filename="../src/engines/cryfscreateoptions.ui" line="55"/>
        <source>Cipher Text Block Size In Bytes.</source>
        <translation>Taille du bloc de texte de chiffrement en bytes.</translation>
    </message>
    <message>
        <location filename="../src/engines/cryfscreateoptions.ui" line="84"/>
        <source>&amp;OK</source>
        <translation>&amp;OK</translation>
    </message>
    <message>
        <location filename="../src/engines/cryfscreateoptions.ui" line="97"/>
        <source>&amp;Cancel</source>
        <translation>&amp;Annuler</translation>
    </message>
    <message>
        <location filename="../src/engines/cryfscreateoptions.ui" line="110"/>
        <source>Select A Folder To Create Config File In(Optional).</source>
        <translation>Sélectionner un répertoire pour y créer un fichier de configuration (facultatif).</translation>
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
    <name>ecryptfscreateoptions</name>
    <message>
        <location filename="../src/engines/ecryptfscreateoptions.ui" line="14"/>
        <source>Select Volume Creation Options</source>
        <translation>Sélectionner les options de création du volume</translation>
    </message>
    <message>
        <location filename="../src/engines/ecryptfscreateoptions.ui" line="26"/>
        <source>&amp;OK</source>
        <translation>&amp;OK</translation>
    </message>
    <message>
        <location filename="../src/engines/ecryptfscreateoptions.ui" line="39"/>
        <source>&amp;Cancel</source>
        <translation>&amp;Annuler</translation>
    </message>
    <message>
        <location filename="../src/engines/ecryptfscreateoptions.ui" line="52"/>
        <source>Select A Folder To Create Config File In(Optional).</source>
        <translation>Sélectionner un répertoire pour y créer un fichier de configuration (facultatif).</translation>
    </message>
    <message>
        <location filename="../src/engines/ecryptfscreateoptions.ui" line="138"/>
        <source>Do Not Encrypt File Names</source>
        <translation>Noms des fichiers à Ne pas chiffrer</translation>
    </message>
    <message>
        <location filename="../src/engines/ecryptfscreateoptions.ui" line="154"/>
        <source>Encrypt File Names</source>
        <translation>Noms des fichiers à chiffrer</translation>
    </message>
    <message>
        <location filename="../src/engines/ecryptfscreateoptions.ui" line="225"/>
        <source>Enable Plain Text Passthrough</source>
        <translatorcomment>Détails de la description de l&apos;option &apos;Plaintext passthrough&apos; de ecryptfs</translatorcomment>
        <translation>Activer une arborescence de répertoires avec des fichiers en clair</translation>
    </message>
    <message>
        <location filename="../src/engines/ecryptfscreateoptions.ui" line="209"/>
        <source>Do Not Enable Plain Text Passthrough</source>
        <translatorcomment>Détails de la description de l&apos;option &apos;Plaintext passthrough&apos; de ecryptfs</translatorcomment>
        <translation>Désactiver une arborescence de répertoires avec des fichiers en clair</translation>
    </message>
</context>
<context>
    <name>encfscreateoptions</name>
    <message>
        <location filename="../src/engines/encfscreateoptions.ui" line="14"/>
        <source>Select Volume Creation Options</source>
        <translation>Sélectionner les options de création du volume</translation>
    </message>
    <message>
        <location filename="../src/engines/encfscreateoptions.ui" line="26"/>
        <source>Reverse Mode</source>
        <translation>Mode Inversé</translation>
    </message>
    <message>
        <location filename="../src/engines/encfscreateoptions.ui" line="39"/>
        <source>&amp;Cancel</source>
        <translation>&amp;Annuler</translation>
    </message>
    <message>
        <location filename="../src/engines/encfscreateoptions.ui" line="52"/>
        <source>&amp;OK</source>
        <translation>&amp;OK</translation>
    </message>
    <message>
        <location filename="../src/engines/encfscreateoptions.cpp" line="39"/>
        <source>Normally EncFS provides a plaintext view of data on demand: it stores enciphered data and displays plaintext data.  With this option set, it takes as source plaintext data and produces enciphered data on-demand. This can be useful for creating remote encrypted backups, where you do not wish to keep the local files unencrypted.</source>
        <translation>En général EncFS fournit à la demande une vision en clair des données : il stocke les données chiffrées et affiche les textes clair. Avec cette option activée, il prend comme source les données en texte brut et produit des données chiffrées à la demande. Cela peut être utile pour créer des sauvegardes chiffrées à distance, où vous ne souhaitez pas conserver les fichiers locaux non chiffrés.</translation>
    </message>
</context>
<context>
    <name>favorites2</name>
    <message>
        <location filename="../src/favorites2.ui" line="14"/>
        <source>Favorites</source>
        <translation>Favoris</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="30"/>
        <source>Favorite List</source>
        <translation>Liste des favoris</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="46"/>
        <source>Volume ID</source>
        <translation>Volume ID</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="78"/>
        <source>Mount Point</source>
        <translation>Point de montage</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="110"/>
        <source>Auto Mount</source>
        <translation>Montage Auto</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="142"/>
        <source>Config File Path</source>
        <translation>Chemin du fichier de configuration</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="174"/>
        <source>Idle Time Out</source>
        <translation>Délai d&apos;inactivité</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="193"/>
        <source>Mount Options</source>
        <translation>Options de montage</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="238"/>
        <source>Volume Properties</source>
        <translation>Propriétés du volume</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="306"/>
        <location filename="../src/favorites2.ui" line="830"/>
        <source>PreMount Command</source>
        <translation>Commande de prémontage</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="325"/>
        <location filename="../src/favorites2.ui" line="849"/>
        <source>PostMount Command</source>
        <translation>Commande de postmontage</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="344"/>
        <location filename="../src/favorites2.ui" line="868"/>
        <source>PreUnMount Command</source>
        <translation>Commande de prédémontage</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="363"/>
        <location filename="../src/favorites2.ui" line="887"/>
        <source>PostUnMount Command</source>
        <translation>Commande de postdémontage</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="383"/>
        <source>Options</source>
        <translation>Options</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="389"/>
        <source>Add/Edit Favorite Entry</source>
        <translation>Ajout/Édition d&apos;une entrée de favori</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="446"/>
        <source>General Options</source>
        <translation>Options Générales</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="793"/>
        <source>Reverse Mode (Gocryptfs and Encfs Only)</source>
        <translation>Mode Inversé (Gocryptfs and Encfs seulement)</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="761"/>
        <source>Mount In Read Only Mode</source>
        <translation>Monter en Lecture Seule</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="729"/>
        <source>Volume Does Not Use A Password</source>
        <translation>Le volume n&apos;utilise pas de mot de passe</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="697"/>
        <source>Auto Mount Volume</source>
        <translation>Montage Auto du Volume</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="668"/>
        <source>Mount Options (Optional And Comma Separated)</source>
        <translation>Options de montage (facultatif / séparation par virgules)</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="416"/>
        <location filename="../src/favorites2.cpp" line="334"/>
        <source>Edit</source>
        <translation>Éditer</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="429"/>
        <source>Add</source>
        <translation>Ajouter</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="626"/>
        <source>Idle TimeOut (Optional)</source>
        <translation>Délai d&apos;inactivité (optionnel)</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="584"/>
        <source>Config File Path (Optional)</source>
        <translation>Chemin du fichier de configuration (facultatif)</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="542"/>
        <location filename="../src/favorites2.cpp" line="163"/>
        <source>Mount Point Prefix</source>
        <translation>Préfixe du point de montage</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="458"/>
        <source>Encrypted Folder Path</source>
        <translation>Chemin d&apos;accès du répertoire chiffré</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="500"/>
        <source>Volume Type (Required For Volumes That Do Not Use A Configuration File Like sshfs)</source>
        <translation>Type de volume (Nécessaire pour les volumes n&apos;utilisant pas un fichier de configuration comme sshfs)</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="818"/>
        <source>Custom Commands</source>
        <translation>Commandes personnalisées</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="1003"/>
        <source>&amp;Close</source>
        <translation>Fermer &amp;C</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="91"/>
        <source>Select A Command Executable</source>
        <translation>Sélectionner une commande à exécuter</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="143"/>
        <location filename="../src/favorites2.cpp" line="435"/>
        <source>Path To A Config File</source>
        <translation>Chemin vers un fichier de configuration</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="159"/>
        <source>Mount Point Path</source>
        <translation>Chemin du point de montage</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="327"/>
        <source>Toggle AutoMount</source>
        <translation>Basculer Montage Auto</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="341"/>
        <source>Remove Selected Entry</source>
        <translation>Enlever l&apos;entrée sélectionnée</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="352"/>
        <source>Cancel</source>
        <translation>Annuler</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="472"/>
        <location filename="../src/favorites2.cpp" line="477"/>
        <source>ERROR!</source>
        <translation>ERREUR!</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="472"/>
        <source>Encrypted Folder Address Field Is Empty</source>
        <translation>Le champ d&apos;adresse du répertoire chiffré est vide</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="477"/>
        <source>Mount Point Path Field Is Empty</source>
        <translation>Le champ du chemin du point de montage est vide</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="590"/>
        <source>Path To An Encrypted Folder</source>
        <translation>Chemin d&apos;accès au répertoire chiffré</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="600"/>
        <source>Path To Mount Folder</source>
        <translation>Chemin d&apos;accès au répertoire de montage</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="758"/>
        <source>Remote Ssh Server Address
(Example: woof@example.com:/remote/path)</source>
        <translation>Adresse distante du serveur SSH
(Exemple: woof@example.com:/remote/path)</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="759"/>
        <source>SSH_AUTH_SOCK Socket Path (Optional)</source>
        <translation>SSH_AUTH_SOCK Chemin du Socket (Optionnel)</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="760"/>
        <source>IdentityFile Path (Optional)</source>
        <translation>Chemin d&apos;accès de l&apos;IdentityFile (Optionnel)</translation>
    </message>
</context>
<context>
    <name>fileManager</name>
    <message>
        <location filename="../src/filemanager.ui" line="14"/>
        <location filename="../src/filemanager.cpp" line="50"/>
        <source>Set File Manager</source>
        <translation>Définir le gestionnaire de fichiers</translation>
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
        <location filename="../src/filemanager.cpp" line="41"/>
        <source>Enter Below The Name Of The Application You Want To Be Used To Open Mount Points.</source>
        <translation>Tapez ci dessous le nom de l&apos;application que vous voulez utiliser pour ouvrir les points de montage.</translation>
    </message>
    <message>
        <location filename="../src/filemanager.cpp" line="52"/>
        <source>Set Executable For &quot;ExternalExecutable&quot; Plugin</source>
        <translation>Choisir l&apos;exécutable pour le plugin &quot;ExecutableExterne&quot;</translation>
    </message>
    <message>
        <location filename="../src/filemanager.cpp" line="54"/>
        <source>Enter Below The Name Of The External Executable And Its Arguments To Use When Invoking &quot;ExternalExecutable&quot; Plugin.</source>
        <translation>Tapez ci dessous le nom de l&apos;exécutable externe et les arguments que vous voulez utiliser quand vous utilisez le plugin &quot;ExécutableExterne&quot;.</translation>
    </message>
</context>
<context>
    <name>gocryptfscreateoptions</name>
    <message>
        <location filename="../src/engines/gocryptfscreateoptions.ui" line="14"/>
        <source>Select Volume Creation Options</source>
        <translation>Sélectionner les options de création du volume</translation>
    </message>
    <message>
        <location filename="../src/engines/gocryptfscreateoptions.ui" line="36"/>
        <source>Cipher To Use For File Contents Encryption.</source>
        <translation>Chiffrement à utiliser pour l&apos;encryptage du contenu du fichier.</translation>
    </message>
    <message>
        <location filename="../src/engines/gocryptfscreateoptions.ui" line="55"/>
        <source>&amp;OK</source>
        <translation>&amp;OK</translation>
    </message>
    <message>
        <location filename="../src/engines/gocryptfscreateoptions.ui" line="68"/>
        <source>&amp;Cancel</source>
        <translation>&amp;Annuler</translation>
    </message>
    <message>
        <location filename="../src/engines/gocryptfscreateoptions.ui" line="81"/>
        <source>Select A Folder To Create Config File In(Optional).</source>
        <translation>Sélectionner un répertoire pour y créer un fichier de configuration (facultatif).</translation>
    </message>
    <message>
        <location filename="../src/engines/gocryptfscreateoptions.ui" line="167"/>
        <source>Do Not Encrypt File Names</source>
        <translation>Noms des fichiers à Ne pas chiffrer</translation>
    </message>
    <message>
        <location filename="../src/engines/gocryptfscreateoptions.ui" line="183"/>
        <source>Encrypt File Names</source>
        <translation>Noms des fichiers à chiffrer</translation>
    </message>
    <message>
        <location filename="../src/engines/gocryptfscreateoptions.ui" line="200"/>
        <source>Reverse Mode</source>
        <translation>Mode Inversé</translation>
    </message>
    <message>
        <location filename="../src/engines/gocryptfscreateoptions.cpp" line="46"/>
        <source>Normally Gocryptfs provides a plaintext view of data on demand.
It stores enciphered data and displays plaintext data.
With this option set, it takes as source plaintext data and produces enciphered data on-demand.
This can be useful for creating remote encrypted backups,
where you do not wish to keep the local files unencrypted.</source>
        <translation>En général Gocryptfs fournit à la demande une vision en clair des données : il stocke les données chiffrées et affiche les textes clair. Avec cette option activée, il prend comme source les données en texte brut et produit des données chiffrées à la demande. Cela peut être utile pour créer des sauvegardes chiffrées à distance, où vous ne souhaitez pas conserver les fichiers locaux non chiffrés.</translation>
    </message>
</context>
<context>
    <name>help</name>
    <message>
        <location filename="../src/help.ui" line="14"/>
        <source>FAQ</source>
        <translation>Foire Aux Questions</translation>
    </message>
    <message>
        <location filename="../src/help.ui" line="26"/>
        <source>&amp;Close</source>
        <translation>Fermer &amp;C</translation>
    </message>
    <message>
        <location filename="../src/help.ui" line="54"/>
        <source>&lt;!DOCTYPE HTML PUBLIC &quot;-//W3C//DTD HTML 4.0//EN&quot; &quot;http://www.w3.org/TR/REC-html40/strict.dtd&quot;&gt;
&lt;html&gt;&lt;head&gt;&lt;meta name=&quot;qrichtext&quot; content=&quot;1&quot; /&gt;&lt;style type=&quot;text/css&quot;&gt;
p, li { white-space: pre-wrap; }
&lt;/style&gt;&lt;/head&gt;&lt;body style=&quot; font-family:&apos;Noto Sans&apos;; font-size:10pt; font-weight:400; font-style:normal;&quot;&gt;
&lt;p style=&quot; margin-top:16px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;1.0 How do i change the default mount point prefix?&lt;/span&gt;&lt;/p&gt;
&lt;p style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;By default,SiriKali uses a mount point prefix of &amp;quot;~/.SiriKali.tmp/&amp;quot;.&lt;/p&gt;
&lt;p style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;You can change it to something else by clicking &amp;quot;Menu-&amp;gt;Settings-&amp;gt;Editable Options-&amp;gt;Set Mount Point Prefix&amp;quot; option and then set a folder you want to be used as a default mount point prefix.&lt;/p&gt;
&lt;p style=&quot; margin-top:16px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;a name=&quot;user-content-20-how-do-i-set-custom-mount-point-path-for-my-volume&quot;&gt;&lt;/a&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;2&lt;/span&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;.0 How do i set custom mount point path for my volume?&lt;/span&gt;&lt;/p&gt;
&lt;p style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;Unique custom mount point path can be used through adding a volume to the favorites list and then setting custom mount point path in the &amp;quot;mount point path&amp;quot; field.The volume should then be accessed through the favorites list.&lt;/p&gt;
&lt;p style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;Favorites list is access through Menu-&amp;gt;Favorites-&amp;gt;Manage Favorites.&lt;/p&gt;
&lt;p style=&quot; margin-top:16px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;a name=&quot;user-content-30-how-do-i-start-the-application-minimized&quot;&gt;&lt;/a&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;3&lt;/span&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;.0 How do i start the application minimized?&lt;/span&gt;&lt;/p&gt;
&lt;p style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;Start the application with &amp;quot;-e&amp;quot; option or set it through GUI by setting &amp;quot;Menu-&amp;gt;Settings-&amp;gt;General Options-&amp;gt;Start Minimized&amp;quot;.&lt;/p&gt;
&lt;p style=&quot; margin-top:16px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;a name=&quot;user-content-40-what-does-different-key-options-means-in-the-dialog-for-creatingunlocking-volume&quot;&gt;&lt;/a&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;4&lt;/span&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;.0 What does different key options means in the dialog for creating/unlocking volume?&lt;/span&gt;&lt;/p&gt;
&lt;ol style=&quot;margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;&quot;&gt;&lt;li style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&amp;quot;Key&amp;quot; entry means the password you enter will be used to create/unlock the volume.&lt;/li&gt;
&lt;li style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&amp;quot;KeyFile&amp;quot; entry means the contents of a specified file will be used as a password to create/unlock the volume. This option is not very safe as the file contents will most likely be truncated on the first encountered new line character making effective password not as strong as expected. It is advised to use one of the next two options if a key file is to be used.&lt;/li&gt;
&lt;li style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&amp;quot;HMAC+KeyFile&amp;quot; entry means the effective password will be a combination of key file contents and an empty password. The effective password will be produced with the following formula: password = hmac_sha256( key file contents,empty password ).&lt;/li&gt;
&lt;li style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&amp;quot;Key+KeyFile&amp;quot; entry means the effective password will be a combination of key file contents and a password. The effective password will be produced with the following formula: password = hmac_sha256( key file contents,password ).&lt;/li&gt;
&lt;li style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&amp;quot;Internal Wallet&amp;quot; entry means the effective password will be retrieved from LXQt internal secure storage system. Section 8.0 has information on how to add passwords to this backend.&lt;/li&gt;
&lt;li style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&amp;quot;KDE Wallet&amp;quot; entry means the effective password will be retrieved from LXQt secure storage system through KDE&apos;s KWallet backend. Section 8.0 has information on how to add passwords to this backend.&lt;/li&gt;
&lt;li style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&amp;quot;GNOME Wallet&amp;quot; entry means the effective password will be retrieved from LXQt secure storage system through libsecret backend. Section 8.0 has information on how to add passwords to this backend.&lt;/li&gt;
&lt;li style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&amp;quot;OSX keychain&amp;quot; entry means the effective password will be retrieved from LXQt secure storage system through OSX keychain backend. Section 8.0 has information on how to add passwords to this backend.&lt;/li&gt;&lt;/ol&gt;
&lt;p style=&quot; margin-top:16px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;a name=&quot;user-content-50-what-directories-does-sirikali-check-for-its-backend-executables&quot;&gt;&lt;/a&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;5&lt;/span&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;.0 What directories does SiriKali check for its backend executables?&lt;/span&gt;&lt;/p&gt;
&lt;p style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;Backend executables are searched in the following directories with the first entry searched first and the last entry searched last: &amp;quot;/usr/local/bin/&amp;quot;,&amp;quot;/usr/local/sbin/&amp;quot;,&amp;quot;/usr/bin/&amp;quot;,&amp;quot;/usr/sbin/&amp;quot;,&amp;quot;/bin/&amp;quot;,&amp;quot;/sbin/&amp;quot;, &amp;quot;/opt/local/bin&amp;quot;,&amp;quot;/opt/local/sbin&amp;quot;,&amp;quot;/opt/bin&amp;quot;,&amp;quot;/opt/sbin&amp;quot;,&amp;quot;~/bin&amp;quot;,&amp;quot;~/.bin&amp;quot;.&lt;/p&gt;
&lt;p style=&quot; margin-top:16px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;a name=&quot;user-content-60-how-do-i-unlock-a-volume-through-a-favorite-entry-without-invoking-a-wallet&quot;&gt;&lt;/a&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;6&lt;/span&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;.0 How do i unlock a volume through a favorite feature without invoking a wallet?&lt;/span&gt;&lt;/p&gt;
&lt;p style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;Disable the feature by setting an option through &amp;quot;Menu-&amp;gt;Settings-&amp;gt;Manage Keys In Wallets-&amp;gt;none&amp;quot;&lt;/p&gt;
&lt;p style=&quot; margin-top:16px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;a name=&quot;user-content-70-how-do-i-unlock-a-volume-through-a-favorite-entry-using-a-key-in-a-wallet-and-without-showing-mount-dialog-prompt&quot;&gt;&lt;/a&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;7&lt;/span&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;.0 How do i unlock a volume through a favorite feature using a key in a wallet and without showing mount dialog prompt?&lt;/span&gt;&lt;/p&gt;
&lt;ol style=&quot;margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;&quot;&gt;&lt;li style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;Set your preferred password storage system at &amp;quot;Menu-&amp;gt;Settings-&amp;gt;Manage Keys In Wallets&amp;quot;.&lt;/li&gt;
&lt;li style=&quot; margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;Unset &amp;quot;Menu-&amp;gt;Settings-&amp;gt;Auto Mount-&amp;gt;Show Mount Dialog When Auto Mounting&amp;quot;.&lt;/li&gt;&lt;/ol&gt;
&lt;p style=&quot; margin-top:16px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;&lt;a name=&quot;user-content-80-how-do-i-add-a-key-to-a-secured-storage-system&quot;&gt;&lt;/a&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;8&lt;/span&gt;&lt;span style=&quot; font-size:x-large; font-weight:600;&quot;&gt;.0 How do i add a key to a secured storage system?&lt;/span&gt;&lt;/p&gt;
&lt;ol style=&quot;margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;&quot;&gt;&lt;li style=&quot; margin-top:12px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;Click &amp;quot;Menu-&amp;gt;Settings-&amp;gt;Manage Keys In Wallets-&amp;gt;Key Storage&amp;quot; and then select your preferred key storage system.&lt;/li&gt;
&lt;li style=&quot; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;On the dialog window that shows up,click &amp;quot;Add&amp;quot;.&lt;/li&gt;
&lt;li style=&quot; margin-top:0px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;&quot;&gt;On the dialog window that shows up,add your password in the appropriate fields. &amp;quot;Volume ID&amp;quot; is the full path to the encrypted volume folder. Click the button to the right of the field to invoke file dialog window where you will get the opportunity to browse to where the folder is to select it.&lt;/li&gt;&lt;/ol&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/help.ui" line="99"/>
        <source>Open &amp;PDF</source>
        <translation>Ouvrir le &amp;PDF</translation>
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
        <location filename="../src/keydialog.cpp" line="264"/>
        <location filename="../src/keydialog.cpp" line="1604"/>
        <location filename="../src/keydialog.cpp" line="1763"/>
        <source>Key</source>
        <translation>Clé</translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="100"/>
        <location filename="../src/keydialog.cpp" line="221"/>
        <source>Volume Name</source>
        <translation>Nom du montage</translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="142"/>
        <source>Mount In &amp;Read Only Mode</source>
        <translation>Monter en &amp;Lecture Seule</translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="233"/>
        <source>&amp;OK</source>
        <translation>&amp;OK</translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="303"/>
        <source>Set Key</source>
        <translation>Paramétrer une clé</translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="316"/>
        <source>Cancel</source>
        <translation>Annuler</translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="371"/>
        <source>Password</source>
        <translation>Mot de passe</translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="400"/>
        <location filename="../src/keydialog.ui" line="425"/>
        <source>TextLabel</source>
        <translation>ÉtiquetteTexte</translation>
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
        <location filename="../src/keydialog.cpp" line="219"/>
        <source>&amp;Create</source>
        <translation>&amp;Créer</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="239"/>
        <location filename="../src/keydialog.cpp" line="534"/>
        <source>Unlocking &quot;%1&quot;</source>
        <translation>Déchiffrer &quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="243"/>
        <source>Mount Path</source>
        <translation>Chemin de montage</translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="352"/>
        <location filename="../src/keydialog.cpp" line="265"/>
        <location filename="../src/keydialog.cpp" line="1133"/>
        <location filename="../src/keydialog.cpp" line="1610"/>
        <location filename="../src/keydialog.cpp" line="1728"/>
        <source>KeyFile</source>
        <translation>Fichier de clé</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="267"/>
        <location filename="../src/keydialog.cpp" line="1645"/>
        <source>HMAC+KeyFile</source>
        <translation>HMAC+fichier de clé</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="266"/>
        <location filename="../src/keydialog.cpp" line="1618"/>
        <location filename="../src/keydialog.cpp" line="1622"/>
        <location filename="../src/keydialog.cpp" line="1742"/>
        <source>Key+KeyFile</source>
        <translation>Clé+fichier de clé</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="268"/>
        <location filename="../src/keydialog.cpp" line="1618"/>
        <source>ExternalExecutable</source>
        <translation>ExternalExecutable</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="269"/>
        <location filename="../src/keydialog.cpp" line="1604"/>
        <source>YubiKey Challenge/Response</source>
        <translation>YubiKey Challenge/Réponse</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="312"/>
        <source>Check This Box To Make Password Visible</source>
        <translation>Cocher la case pour afficher le mot de passe en clair</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="330"/>
        <source>(%1/%2) Unlocking &quot;%3&quot;</source>
        <translation>(%1/%2) dévérouille &quot;%3&quot;</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="558"/>
        <source>Create A New &quot;%1&quot; Volume</source>
        <translation>Créer un nouveau volume &quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="638"/>
        <source>Passphrase Quality: 0%</source>
        <translation>Force de la Passphrase : 0%</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="640"/>
        <source>Passphrase Quality: %1%</source>
        <translation>Force de la Passphrase : %1%</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="647"/>
        <location filename="../src/keydialog.cpp" line="1672"/>
        <source>Passphrase Quality: 100%</source>
        <translation>Force de la Passphrase : 100%</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="198"/>
        <location filename="../src/keydialog.cpp" line="712"/>
        <location filename="../src/keydialog.cpp" line="725"/>
        <source>Select A Folder To Create A Mount Point In.</source>
        <translation>Sélectionner un répertoire pour y créer un point de montage.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="389"/>
        <source>Select Volume Type</source>
        <translation>Sélectionnez le type de volume</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="414"/>
        <source>Options</source>
        <translation>Options</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="442"/>
        <source>Not Used</source>
        <translation>Non Utilisé</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="839"/>
        <location filename="../src/keydialog.cpp" line="853"/>
        <source>Select A File To Be Used As A Keyfile.</source>
        <translation>Sélectionner un fichier afin de l&apos;utiliser comme un fichier de clé.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="879"/>
        <location filename="../src/keydialog.cpp" line="1687"/>
        <source>Volume Name Field Is Empty.</source>
        <translation>Le champ du Nom du Volume est vide.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="925"/>
        <source>Internal Wallet Is Not Configured.</source>
        <translation>Trousseau Interne non configuré.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="938"/>
        <source>Volume Not Found in &quot;%1&quot;.

Set The Volume Key To Add It To The Wallet Before Mounting.</source>
        <translation>Volume introuvable dans &quot;%1&quot;.

Mettre la clé de volume à ajouter dans le trousseau avant le montage.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="965"/>
        <location filename="../src/keydialog.cpp" line="1476"/>
        <source>Upgrade File System</source>
        <translation>Mettre à jour le système de fichiers</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1155"/>
        <source>Volume Key Can Not Be Empty.</source>
        <translation>La clé de volume ne peut pas être vide.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1209"/>
        <source>Failed To Add A Volume To The A Wallet.</source>
        <translation>Echec de l&apos;ajout du volume au trousseau.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1212"/>
        <source>Volume Already Exists In The Wallet.</source>
        <translation>Volume déjà existant dans le trousseau.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1248"/>
        <source>Failed To Open Wallet.</source>
        <translation>Echec de l&apos;ouverture du trousseau.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1310"/>
        <source>Failed To Generate Key.</source>
        <translation>Echec de la génération de clé.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1410"/>
        <source>Mount Point Path Is Not Empty.</source>
        <translation>Le champ du chemin du point de montage n&apos;est pas vide.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1510"/>
        <source>Failed To Locate Or Run Yubikey&apos;s &quot;ykchalresp&quot; Program.</source>
        <translation>Echec de la localisation ou de l&apos;exécution de Yubikey&apos;s &quot;ykchalresp&quot; Program.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1649"/>
        <source>Select A KeyFile</source>
        <translation>Sélectionner un fichier de clé</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1031"/>
        <source>Encrypted Folder Path Is Already Taken.</source>
        <translation>Chemin d&apos;accès du répertoire chiffré déjà occupé.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1044"/>
        <location filename="../src/keydialog.cpp" line="1051"/>
        <location filename="../src/keydialog.cpp" line="1060"/>
        <location filename="../src/keydialog.cpp" line="1071"/>
        <location filename="../src/keydialog.cpp" line="1393"/>
        <location filename="../src/keydialog.cpp" line="1403"/>
        <source>Mount Point Path Already Taken.</source>
        <translation>Point de montage déjà occupé.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1377"/>
        <source>Atleast One Required Field Is Empty.</source>
        <translation>Au moins un des champs qui est requis est vide.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1694"/>
        <source>Create A Volume With Specified Key And Then Add The Key In 
&quot;%1&quot;.</source>
        <translation>Crée un volume avec la clé spécifiée et ajouter la clé dans 
&quot;%1&quot;.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1740"/>
        <location filename="../src/keydialog.cpp" line="1752"/>
        <source>Plugin name</source>
        <translation>Plugin name</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1777"/>
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
        <translation>Monter le volume avec les options de montage ci-dessous.</translation>
    </message>
    <message>
        <location filename="../src/options.ui" line="157"/>
        <source>Reverse Mode (Gocryptfs and Encfs Only).</source>
        <translation>Mode Inversé (Gocryptfs and Encfs seulement).</translation>
    </message>
    <message>
        <location filename="../src/options.ui" line="171"/>
        <source>&amp;Set</source>
        <translation>&amp;Paramétrer</translation>
    </message>
    <message>
        <location filename="../src/options.ui" line="184"/>
        <source>&amp;Cancel</source>
        <translation>&amp;Annuler</translation>
    </message>
    <message>
        <location filename="../src/options.cpp" line="142"/>
        <source>Select Cryfs/Gocryptfs Configuration File</source>
        <translation>Sélectionner un fichier de configuration Cryfs/Gocryptfs</translation>
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
        <location filename="../src/plugin.cpp" line="162"/>
        <source>KeyFile</source>
        <translation>Fichier de clé</translation>
    </message>
    <message>
        <location filename="../src/plugin.cpp" line="111"/>
        <location filename="../src/plugin.cpp" line="150"/>
        <source>ERROR</source>
        <translation>ERREUR</translation>
    </message>
    <message>
        <location filename="../src/plugin.cpp" line="111"/>
        <source>KeyFile Not Set</source>
        <translation>Fichier de clé non actif</translation>
    </message>
    <message>
        <location filename="../src/plugin.cpp" line="150"/>
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
        <location filename="../src/engines/securefscreateoptions.ui" line="14"/>
        <source>Choose Format Type</source>
        <translation>Choisir le type de format</translation>
    </message>
    <message>
        <location filename="../src/engines/securefscreateoptions.cpp" line="43"/>
        <source>The &quot;lite format&quot; simply encrypts filenames and file contents separately, similar to how encfs operates, although with more security.

The &quot;full format&quot; maps files, directories and symlinks in the virtual filesystem all to regular files in the underlying filesystem. The directory structure is flattened and recorded as B-trees in files.

The lite format has become the default on Unix-like operating systems as it is much faster and features easier conflict resolution, especially when used with DropBox, Google Drive, etc. The full format, however, leaks fewer information about the filesystem hierarchy, runs relatively independent of the features of the underlying filesystem, and is in general more secure.</source>
        <translation>Le &quot;format allégé&quot; chiffre simplement les noms de fichiers et le contenu des fichiers séparément, de la même façon que ce qu&apos;opère encfs, avec toutefois plus de sécurité.

Le &quot;format complet&quot; mappe les fichiers, répertoires, liens symboliques du système de fichiers virtuel dans des fichiers réguliers sous-jacents du système de fichiers. La structure de l&apos;arborescence est réduite et sauvegardée des fichiers B-arbres.

Le &quot;format allégé&quot; est devenu le mode par défaut des systèmes basés Unix car il est plus rapide et plus permet une résolution plus facile de conflits notamment lors de l&apos;utilisation de DropBox, Google Drive, etc. Le &quot;format complet&quot;, toutefois, perd moins d&apos;infos sur la hiérarchie du système de fichiers, s&apos;exécute plus indépendamment du système de fichiers sous-jacent et il est en général plus sécurisé.</translation>
    </message>
    <message>
        <location filename="../src/engines/securefscreateoptions.ui" line="27"/>
        <source>Lite Format</source>
        <translation>Format allégé</translation>
    </message>
    <message>
        <location filename="../src/engines/securefscreateoptions.ui" line="32"/>
        <source>Full Format</source>
        <translation>Format complet</translation>
    </message>
    <message>
        <location filename="../src/engines/securefscreateoptions.ui" line="46"/>
        <source>&amp;OK</source>
        <translation>&amp;OK</translation>
    </message>
    <message>
        <location filename="../src/engines/securefscreateoptions.ui" line="59"/>
        <source>&amp;Cancel</source>
        <translation>&amp;Annuler</translation>
    </message>
    <message>
        <location filename="../src/engines/securefscreateoptions.ui" line="72"/>
        <source>Select A Folder To Create Config File In(Optional).</source>
        <translation>Sélectionner un répertoire pour y créer un fichier de configuration (facultatif).</translation>
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
        <location filename="../src/sirikali.cpp" line="436"/>
        <source>Unmount All</source>
        <translation>Démonter tout</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="276"/>
        <location filename="../src/sirikali.cpp" line="278"/>
        <source>%1 Is Not Installed</source>
        <translation>%1 n&apos;est pas installé</translation>
    </message>
    <message>
        <location filename="../src/sirikali.ui" line="122"/>
        <location filename="../src/sirikali.cpp" line="442"/>
        <source>Favorites</source>
        <translation>Favoris</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="433"/>
        <source>Check For Updates</source>
        <translation>Tester si la version du logiciel est à jour</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="449"/>
        <source>About</source>
        <translation>À propos</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="456"/>
        <source>Quit</source>
        <translation>Quitter</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="453"/>
        <source>Show/Hide</source>
        <translation>Montrer/Cacher</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="783"/>
        <source>ERROR: Volume Path Not Given.</source>
        <translation>ERREUR: Chemin du volume non fourni.</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="884"/>
        <source>ERROR: Key Not Found In The Backend.</source>
        <translation>ERREUR: Clé non trouvée dans le Gestionnaire.</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="889"/>
        <source>ERROR: Failed To Unlock Requested Backend.</source>
        <translation>ERREUR: Echec du déverrouillage du Gestionnaire sélectionné.</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="647"/>
        <location filename="../src/sirikali.cpp" line="689"/>
        <location filename="../src/sirikali.cpp" line="1086"/>
        <location filename="../src/sirikali.cpp" line="1399"/>
        <location filename="../src/sirikali.cpp" line="1728"/>
        <source>ERROR</source>
        <translation>ERREUR</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1086"/>
        <source>Failed To Read Volume Properties</source>
        <translation>Echec de la lecture des propriétés du volume</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1068"/>
        <location filename="../src/sirikali.cpp" line="1088"/>
        <location filename="../src/sirikali.cpp" line="1112"/>
        <source>INFORMATION</source>
        <translation>INFORMATION</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="143"/>
        <source>Close All Volumes Before Quitting The Application</source>
        <translation>Fermer tous les volumes avant de quitter l&apos;application</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="243"/>
        <source>Mount Folder</source>
        <translation>Répertoire monté</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="245"/>
        <source>Mount File</source>
        <translation>Fichier monté</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="439"/>
        <source>Unmount All And Quit</source>
        <translation>Démonter tout et quitter</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="446"/>
        <source>Settings</source>
        <translation>Paramétrages</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="451"/>
        <source>FAQ</source>
        <translation>Foire Aux Questions</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="647"/>
        <source>SiriKali Failed To Connect To siriPolkit.
Please Report This Serious Bug.</source>
        <translation>SiriKali a échoué à la connexion de siriPolkit.
Merci de reporter ce bug important.</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="689"/>
        <source>&quot;%1&quot; Folder Must Be Writable</source>
        <translation>&quot;%1&quot; Répertoire devant être autorisé en écriture</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1146"/>
        <source>Unmount</source>
        <translation>Démonter</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1150"/>
        <source>Add To Favorites</source>
        <translation>Ajouter aux favoris</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1427"/>
        <source>Select A File To Be Mounted</source>
        <translation>Sélectiionner un fichier à monter</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1793"/>
        <source>Failed To Unmount 1 Volume.</source>
        <translation>Echec de démontage d&apos;un volume.</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1795"/>
        <source>Failed To Unmount %1 Volumes.</source>
        <translation>Echec de démontage de %1 volumes.</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1144"/>
        <source>Open Folder</source>
        <translation>Ouvrir Répertoire</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1148"/>
        <source>Properties</source>
        <translation>Propriétés</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1154"/>
        <source>Close Menu</source>
        <translation>Fermer le Menu</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="144"/>
        <location filename="../src/sirikali.cpp" line="1227"/>
        <location filename="../src/sirikali.cpp" line="1793"/>
        <location filename="../src/sirikali.cpp" line="1795"/>
        <source>WARNING</source>
        <translation>Attention</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1228"/>
        <source>Could Not Open Mount Point Because &quot;%1&quot; Tool Does Not Appear To Be Working Correctly.</source>
        <translation>Impossible d&apos;ouvrir le point de montage parce que l&apos;outil &quot;%1&quot; ne semble pas fonctionner correctement.</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1277"/>
        <source>&amp;Quit</source>
        <translation>&amp;Quitter</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1400"/>
        <source>Permission To Access The Volume Was Denied
Or
The Volume Is Not Supported</source>
        <translation>La permission d&apos;accéder au volume a été refusée
ou
le volume n&apos;est pas supporté</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1424"/>
        <source>Select An Encrypted Volume Directory</source>
        <translation>Sélectionner un répertoire de volume</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1728"/>
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
        <location filename="../src/walletconfig.cpp" line="108"/>
        <source>Delete Entry</source>
        <translation>Supprimer l&apos;entrée</translation>
    </message>
    <message>
        <location filename="../src/walletconfig.cpp" line="121"/>
        <source>WARNING!</source>
        <translation>ATTENTION!</translation>
    </message>
    <message>
        <location filename="../src/walletconfig.cpp" line="121"/>
        <source>Are you sure you want to delete a volume with an id of &quot;%1&quot;?</source>
        <translation>Êtes vous sûr d&apos;effacer un volume avec un ID de &quot;%1&quot;?</translation>
    </message>
    <message>
        <location filename="../src/walletconfig.cpp" line="211"/>
        <source>ERROR!</source>
        <translation>ERREUR!</translation>
    </message>
    <message>
        <location filename="../src/walletconfig.cpp" line="211"/>
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
        <location filename="../src/walletconfiginput.cpp" line="111"/>
        <location filename="../src/walletconfiginput.cpp" line="118"/>
        <source>ERROR!</source>
        <translation>ERREUR!</translation>
    </message>
    <message>
        <location filename="../src/walletconfiginput.cpp" line="111"/>
        <source>Atleast one required field is empty</source>
        <translation>Au moins un des champs qui est requis est vide</translation>
    </message>
    <message>
        <location filename="../src/walletconfiginput.cpp" line="118"/>
        <source>Passphrases do not match</source>
        <translation>Passphrases non correspondantes</translation>
    </message>
    <message>
        <location filename="../src/walletconfiginput.cpp" line="141"/>
        <source>Select A Volume</source>
        <translation>Sélectionner un volume</translation>
    </message>
</context>
</TS>
