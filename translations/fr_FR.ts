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
        <location filename="../src/dialogmsg.cpp" line="239"/>
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
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.cpp" line="144"/>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.cpp" line="246"/>
        <source>Passwords do not match</source>
        <translation>Mots de passe non correspondants</translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.cpp" line="196"/>
        <source>Wallet password could not be changed</source>
        <translation>Le mot de passe du trousseau ne peut pas être changé</translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.cpp" line="185"/>
        <source>Wallet could not be opened with the presented key</source>
        <translation>Le trousseau ne peut pas être ouvert avec cette clé</translation>
    </message>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/changepassworddialog.cpp" line="209"/>
        <source>New passwords do not match</source>
        <translation>Nouveaux mots de passe non correspondants</translation>
    </message>
</context>
<context>
    <name>LXQt::Wallet::password_dialog</name>
    <message>
        <location filename="../src/3rdParty/lxqt_wallet/frontend/password_dialog.cpp" line="74"/>
        <source>Wallet could not be opened with the presented key</source>
        <translation>Le trousseau ne peut pas être ouvert avec cette clé</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../src/utility.cpp" line="529"/>
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
        <location filename="../src/utility.cpp" line="690"/>
        <source>about SiriKali</source>
        <translation>A propos de SiriKali</translation>
    </message>
    <message>
        <location filename="../src/utility.cpp" line="1262"/>
        <source>Policy:</source>
        <translation>Règle:</translation>
    </message>
    <message>
        <location filename="../src/utility.cpp" line="1267"/>
        <source>Comment:</source>
        <translation>Commentaire:</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1579"/>
        <source>This plugin delegates key generation to an external application</source>
        <translation>Ce plugin transmet la génération de clé à une application extérieure</translation>
    </message>
    <message>
        <location filename="../src/checkforupdates.cpp" line="86"/>
        <source>&quot;%1&quot; Installed Version Is : %2.&lt;br&gt;Latest Version Is : %3.&lt;br&gt;</source>
        <translation>&quot;%1&quot; version installée : %2.&lt;br&gt;Dernière version disponible : %3.&lt;br&gt;</translation>
    </message>
    <message>
        <location filename="../src/checkforupdates.cpp" line="226"/>
        <source>Network Request Failed To Respond Within %1 Seconds.</source>
        <translation>Le réseau n&apos;a pas répondu dans l&apos;espace de temps de %1 secondes.</translation>
    </message>
    <message>
        <source>Kde Wallet</source>
        <translation type="vanished">Trousseau Kde</translation>
    </message>
    <message>
        <source>Internal Wallet</source>
        <translation type="vanished">Trousseau interne</translation>
    </message>
    <message>
        <source>Gnome Wallet</source>
        <translation type="vanished">Trousseau Gnome</translation>
    </message>
    <message>
        <source>OSX KeyChain</source>
        <translation type="vanished">Porte clés OSX</translation>
    </message>
    <message>
        <source>Windows DPAPI</source>
        <translation type="vanished">Windows DPAPI</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="407"/>
        <source>Not Used</source>
        <translation>Non Utilisé</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1735"/>
        <source>Not Supported KeyFile Encountered Since It Contains AtLeast One Illegal Character(&apos;\n&apos;,&apos;\0&apos;,&apos;\r&apos;).

Please Use a Hash Of The KeyFile Through &quot;HMAC+KeyFile&quot; Option.</source>
        <translation>Pas de fichier de clé supporté s&apos;il contient ne serait ce qu&apos;un caractère illégal (&apos;\n&apos;,&apos;\0&apos;,&apos;\r&apos;).

Merci d&apos;utiliser un hashage de la clé de fichier au travers de l&apos;option &quot;HMAC+Fichier de clé&quot;.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1575"/>
        <source>Effective Key Is Generated With Below Formula:

key = hmac_sha256(password,keyfile contents)</source>
        <translation>La clé actuelle est générée avec la formule ci-dessous:

clé = hmac_sha256(mot de passe,contenu du fichier de clé)</translation>
    </message>
    <message>
        <location filename="../src/checkforupdates.cpp" line="228"/>
        <source>ERROR</source>
        <translation>ERREUR</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="1726"/>
        <source>Volume Created Successfully.</source>
        <translation>Volume créé avec succès.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="1730"/>
        <source>Backend Requires A Password.</source>
        <translation>Le processus nécessite un mot de passe.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="1742"/>
        <source>Backend Requires Polkit Support and SiriKali Failed To Start It.</source>
        <translation>Le processus nécessite le support de Polkit et SiriKali n&apos;a pas réussi à le démarrer.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="1746"/>
        <source>A Space Character Is Not Allowed In Paths When Using Ecryptfs Backend And Polkit.</source>
        <translation>Le caractére espace n&apos;est pas autorisé quand on utilise un processus Ecryptfs et le Polkit.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="1722"/>
        <source>Failed To Unmount %1 Volume</source>
        <translation>Impossible de démonter le Volume %1</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="1738"/>
        <source>This Fscrypt Volume Requires A KeyFile.</source>
        <translation>Ce volume Fscrypt nécessite un fichier de clé.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="1750"/>
        <source>Backend Does Not Support Custom Configuration File Path.</source>
        <translation>Le processus ne supporte pas un fichier de configuration personnalisé du chemin.d&apos;accès.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="1734"/>
        <source>Failed To Unlock &quot;%1&quot; Volume.
Wrong Password Entered.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="1754"/>
        <source>Failed To Complete The Request.
%1 Executable Could Not Be Found.</source>
        <translation>Impossible de finir la requête.
L&apos;exécutable %1 est introuvable.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="1758"/>
        <source>Something Is Wrong With The Backend And It Took Too Long To Respond.</source>
        <translation>Quelque chose s&apos;est mal passé dans le processus et il prend trop de temps à répondre.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="1762"/>
        <source>This Volume Of Cryfs Needs To Be Upgraded To Work With The Version Of Cryfs You Are Using.

The Upgrade is IRREVERSIBLE And The Volume Will No Longer Work With Older Versions of Cryfs.

To Do The Upgrade, Check The &quot;Upgrade File System&quot; Option And Unlock The Volume Again.</source>
        <translation>Ce volume Cryfs nécessite un système de fichiers mis à jour pour fonctionner avec la version de Cryfs que vous utilisez.

La mise à jour est IRREVERSIBLE et le volume ne fonctionnera alors plus avec l&apos;ancienne version de Cryfs.

Pour réaliser la mise à jour, cochez l&apos;option &quot;Mettre à jour le système de fichiers&quot; et déchiffrez à nouveau le volume.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="1766"/>
        <source>This Volume Of Cryfs Is Different From The Known One.

Check The &quot;Replace File System&quot; Option And Unlock The Volume Again To Replace The Previous File System.</source>
        <translation>Ce volume Cryfs est différent de celui qui est connu

Testez l&apos;option &quot;Remplacer le système de fichiers&quot; et déchiffrez à nouveau le volume pour remplacer l&apos;ancien système de fichiers.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="1770"/>
        <source>Atleast Version 0.9.9 Of Cryfs Is Required To Be Able To Upgrade A Volume and Installed Version Is &quot;%1&quot;.</source>
        <translation>Il faut au moins la version 0.9.9 de Cryfs pour pouvoir mettre à jour le volume, et la version actuellement installée est &quot;%1&quot;.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="1774"/>
        <source>Mount Point Folder Path Must Reside In An NTFS FileSystem.</source>
        <translation>Le chemin de répertoire du point de montage doit se situer dans un système de fichiers NTFS.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="1778"/>
        <source>Mount Point Folder Path Is Not Empty.</source>
        <translation>Le chemin de répertoire du point de montage n&apos;est pas vide.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="1786"/>
        <source>Failed To Complete The Request.
Java Executable Could Not Be Found.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="1782"/>
        <source>Failed To Complete The Request.
Ecryptfs-simple Executable Could Not Be Found.</source>
        <translation>Impossible de finir la requête.
L&apos;exécutable Ecryptfs est introuvable.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="1790"/>
        <source>Failed To Create Mount Point.</source>
        <translation>Echec de la création du point de montage.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="1794"/>
        <source>Backend Could Not Load WinFsp. Please Make Sure You Have WinFsp Properly Installed.</source>
        <translation>Le processus n&apos;a pas pu charger WinFsp. Veuillez vérifier que vous ayez installé WinFsp correctement.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="1798"/>
        <source>Failed To Unlock The Volume.
Not Supported Volume Encountered.</source>
        <translation>Echec de l&apos;ouverture du volume.
Volume non supporté rencontré.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="1802"/>
        <source>Backend Requires Atleast Version &quot;%1&quot; Of SiriKali.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="1812"/>
        <source>Installed &quot;%1&quot; Version Is Too Old.
 Please Update To Atleast Version %2.</source>
        <translation>La version &quot;%1&quot; installée est trop ancienne.
 Veuillez mettre à jour vers une version %2.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="1817"/>
        <source>Folder Not Fully Locked Because Some Files Are Still In Use.</source>
        <translation>Répertoire non complètement fermé car certains fichiers sont encore en cours d&apos;utilisation.</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="1827"/>
        <source>Invalid Config File Name.
Its Name Must End With &quot;%1&quot;</source>
        <translation>Nom de fichier de configuration invalide.
Son nom doit se terminer avec &quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="1830"/>
        <source>Invalid Config File Name.
It Must End With One Of The Following:
&quot;%1&quot;</source>
        <translation>Nom de fichier de configuration invalide.
Il doit se terminer avec un des termes suivants : &quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../src/engines.cpp" line="1838"/>
        <source>Failed To Complete The Task And Below Log was Generated By The Backend.
</source>
        <translation>Échec pour terminer la tâche, le log ci-dessous a été généré par le processus.
</translation>
    </message>
    <message>
        <location filename="../src/settings.cpp" line="547"/>
        <source>Manage Favorites</source>
        <translation>Gérer les Favoris</translation>
    </message>
    <message>
        <location filename="../src/settings.cpp" line="548"/>
        <source>Mount All</source>
        <translation>Monter Tout</translation>
    </message>
    <message>
        <location filename="../src/settings.cpp" line="1245"/>
        <source>Russian (RU)</source>
        <translation>Russe (RU)</translation>
    </message>
    <message>
        <location filename="../src/settings.cpp" line="1246"/>
        <source>French (FR)</source>
        <translation>Français (FR)</translation>
    </message>
    <message>
        <location filename="../src/settings.cpp" line="1247"/>
        <source>German (DE)</source>
        <translation>Allemand (DE)</translation>
    </message>
    <message>
        <location filename="../src/settings.cpp" line="1248"/>
        <source>English (US)</source>
        <translation>Anglais (US)</translation>
    </message>
    <message>
        <location filename="../src/settings.cpp" line="1249"/>
        <source>Swedish (SE)</source>
        <translation>Suedois (SE)</translation>
    </message>
    <message>
        <location filename="../src/settings.cpp" line="1250"/>
        <source>Arabic (SA)</source>
        <translation>Arabe (SA)</translation>
    </message>
    <message>
        <location filename="../src/settings.cpp" line="1251"/>
        <source>Spanish (MX)</source>
        <translation>Espagnol (MX)</translation>
    </message>
    <message>
        <location filename="../src/engines/options.cpp" line="84"/>
        <source>Select %1 Configuration File.</source>
        <translation>Sélectionner le fichier de configuration %1.</translation>
    </message>
    <message>
        <location filename="../src/engines/options.cpp" line="99"/>
        <source>Select %1 KeyFile.</source>
        <translation>Sélectionner le fichier de clé %1.</translation>
    </message>
    <message>
        <location filename="../src/engines/cryfs.cpp" line="225"/>
        <source>Allow Replaced File System</source>
        <translation>Autoriser le remplacement du système de fichiers</translation>
    </message>
    <message>
        <location filename="../src/engines/fscrypt.cpp" line="532"/>
        <source>Unlock Fscrypt Volume With A Specified 32 Byte(256-Bit) KeyFile.</source>
        <translation>Déchiffrer le volume Fscrypt avec un fichier de clé 32 octets (256-Bit) spécifique.</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="760"/>
        <source>The Password Is &quot;%1&quot;</source>
        <translation>Le mot de passe est &quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="101"/>
        <source>Unknown Wallet Option</source>
        <translation>Option de trousseau inconnue</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="172"/>
        <source>Unknown Option</source>
        <translation>Option inconnue</translation>
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
        <location filename="../src/configoptions.ui" line="245"/>
        <source>SiriKali Will Have To Be Restarted For These Changes To Take Effect.</source>
        <translation>SiriKali doit être redémarré pour que les changements prennent effet.</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="377"/>
        <location filename="../src/configoptions.cpp" line="156"/>
        <source>Set External Plugin Executable</source>
        <translation>Choisir le plugin exécutable externe</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="396"/>
        <source>Set Command To Run After Mount</source>
        <translation>Choisir les commandes à exécuter après montage</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="435"/>
        <source>Set Command To Run Before UnMount</source>
        <translation>Choisir les commandes à exécuter avant montage</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="171"/>
        <source>HiDPI Settings</source>
        <translation>Réglages HiDPI</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="225"/>
        <source>Scale Factor
(Enter A Decimal Number)</source>
        <translation>Facteur d&apos;échelle
(Entrer un nombre décimal)</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="206"/>
        <source>Enable HiDPI Scalling</source>
        <translation>Activer la mise à l&apos;échelle HiDPI</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="278"/>
        <source>Show Debug Window On Startup</source>
        <translation>Afficher la fenêtre de débogage au démarrage</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="284"/>
        <source>External Commands</source>
        <translation>Commandes externes</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="296"/>
        <source>Set File Manager</source>
        <translation>Définir le gestionnaire de fichiers</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="345"/>
        <source>Set Mount Point Prefix</source>
        <translation>Paramétrer le préfixe du point de montage</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="480"/>
        <source>Set Command To Run Periodically</source>
        <translation>Paramétrer la commande pour s&apos;exécuter périodiquement</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="532"/>
        <source>Interval In Minutes Periodic Command Will Run</source>
        <translation>Intervalle périodique en minutes entre laquelle la commande s&apos;exécutera</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="577"/>
        <location filename="../src/configoptions.ui" line="593"/>
        <location filename="../src/configoptions.ui" line="609"/>
        <location filename="../src/configoptions.ui" line="625"/>
        <location filename="../src/configoptions.ui" line="641"/>
        <location filename="../src/configoptions.ui" line="657"/>
        <source>Set To Default</source>
        <translation>Paramétrer par défaut</translation>
    </message>
    <message>
        <location filename="../src/configoptions.ui" line="675"/>
        <source>&amp;Close</source>
        <translation>Fermer &amp;C</translation>
    </message>
    <message>
        <location filename="../src/configoptions.cpp" line="79"/>
        <location filename="../src/configoptions.cpp" line="225"/>
        <source>Set Executables Search Path</source>
        <translation>Choisir le chemin de recherche des exécutables</translation>
    </message>
    <message>
        <location filename="../src/configoptions.cpp" line="116"/>
        <source>Set Post Mount Command</source>
        <translation>Paramétrer la commande de prémontage</translation>
    </message>
    <message>
        <location filename="../src/configoptions.cpp" line="126"/>
        <location filename="../src/configoptions.cpp" line="136"/>
        <source>Set Pre UnMount Command</source>
        <translation>Paramétrer la commande de prédémontage</translation>
    </message>
    <message>
        <location filename="../src/configoptions.cpp" line="146"/>
        <source>Set Command To Open Mount Points</source>
        <translation>Paramétrer la commande pour ouvrir les points de montage</translation>
    </message>
</context>
<context>
    <name>cryfsWarning</name>
    <message>
        <location filename="../src/keydialog.h" line="69"/>
        <source>Please be patient because creating a &quot;%1&quot; volume may take a very long time.

</source>
        <translation>Merci de bien vouloir patienter, la création d&apos;un volume &quot;%1&quot; prend un temps assez long.

</translation>
    </message>
    <message>
        <location filename="../src/keydialog.h" line="74"/>
        <source>Please be patient because unlocking a &quot;%1&quot; volume may take a very long time.

</source>
        <translation>Merci de bien vouloir patienter, le déchiffrement d&apos;un volume &quot;%1&quot; prend un temps assez long.

</translation>
    </message>
    <message>
        <location filename="../src/keydialog.h" line="82"/>
        <source>Elapsed time: 0 seconds</source>
        <translation>Temps écoulé: 0 secondes</translation>
    </message>
    <message>
        <location filename="../src/keydialog.h" line="96"/>
        <source>Elapsed time: %0 minutes</source>
        <translation>Temps écoulé: %0 minutes</translation>
    </message>
    <message>
        <location filename="../src/keydialog.h" line="98"/>
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
    <message>
        <location filename="../src/engines/cryfscreateoptions.ui" line="171"/>
        <source>Allow Replaced File System</source>
        <translation>Autoriser le remplacement du système de fichiers</translation>
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
        <location filename="../src/engines/encfscreateoptions.cpp" line="40"/>
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
        <location filename="../src/favorites2.ui" line="49"/>
        <source>Volume ID</source>
        <translation>Volume ID</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="81"/>
        <source>Mount Point</source>
        <translation>Point de montage</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="113"/>
        <source>Auto Mount</source>
        <translation>Montage Auto</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="145"/>
        <location filename="../src/favorites2.cpp" line="1478"/>
        <source>Config File Path</source>
        <translation>Chemin du fichier de configuration</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="177"/>
        <location filename="../src/favorites2.cpp" line="1479"/>
        <source>Idle Time Out</source>
        <translation>Délai d&apos;inactivité</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="196"/>
        <source>Mount Options</source>
        <translation>Options de montage</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="241"/>
        <source>Volume Properties</source>
        <translation>Propriétés du volume</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="309"/>
        <location filename="../src/favorites2.ui" line="897"/>
        <source>PreMount Command</source>
        <translation>Commande de prémontage</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="328"/>
        <location filename="../src/favorites2.ui" line="916"/>
        <source>PostMount Command</source>
        <translation>Commande de postmontage</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="347"/>
        <location filename="../src/favorites2.ui" line="935"/>
        <source>PreUnMount Command</source>
        <translation>Commande de prédémontage</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="366"/>
        <location filename="../src/favorites2.ui" line="954"/>
        <source>PostUnMount Command</source>
        <translation>Commande de postdémontage</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="386"/>
        <source>Options</source>
        <translation>Options</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="392"/>
        <source>Add/Edit Favorite Entry</source>
        <translation>Ajout/Édition d&apos;une entrée de favori</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="449"/>
        <source>General Options</source>
        <translation>Options Générales</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="796"/>
        <location filename="../src/favorites2.cpp" line="1574"/>
        <source>Reverse Mode (Gocryptfs and Encfs Only)</source>
        <translation>Mode Inversé (Gocryptfs and Encfs seulement)</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="764"/>
        <source>Mount In Read Only Mode</source>
        <translation>Monter en Lecture Seule</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="732"/>
        <source>Volume Does Not Use A Password</source>
        <translation>Le volume n&apos;utilise pas de mot de passe</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="700"/>
        <source>Auto Mount Volume</source>
        <translation>Montage Auto du Volume</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="671"/>
        <source>Mount Options (Optional And Comma Separated)</source>
        <translation>Options de montage (facultatif / séparation par virgules)</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="419"/>
        <location filename="../src/favorites2.cpp" line="608"/>
        <source>Edit</source>
        <translation>Éditer</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="432"/>
        <source>Add</source>
        <translation>Ajouter</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="629"/>
        <location filename="../src/favorites2.cpp" line="1576"/>
        <source>Idle TimeOut (Optional)</source>
        <translation>Délai d&apos;inactivité (optionnel)</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="587"/>
        <location filename="../src/favorites2.cpp" line="1575"/>
        <source>Config File Path (Optional)</source>
        <translation>Chemin du fichier de configuration (facultatif)</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="545"/>
        <location filename="../src/favorites2.cpp" line="470"/>
        <location filename="../src/favorites2.cpp" line="883"/>
        <source>Mount Point Prefix</source>
        <translation>Préfixe du point de montage</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="461"/>
        <location filename="../src/favorites2.cpp" line="1619"/>
        <source>Encrypted Folder Path</source>
        <translation>Chemin d&apos;accès du répertoire chiffré</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="503"/>
        <source>Volume Type (Required For Volumes That Do Not Use A Configuration File Like sshfs)</source>
        <translation>Type de volume (Nécessaire pour les volumes n&apos;utilisant pas un fichier de configuration comme sshfs)</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="828"/>
        <source>Add Volume To Secure Wallets</source>
        <translation>Ajouter le volume aux trousseaux sécurisés</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="854"/>
        <location filename="../src/favorites2.cpp" line="1548"/>
        <source>Ssh Port Number</source>
        <translation>Numéro du Port SSH</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="873"/>
        <location filename="../src/favorites2.cpp" line="1263"/>
        <source>Entry Already Exist</source>
        <translation>Entrée déjà existante</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="885"/>
        <source>Custom Commands</source>
        <translation>Commandes personnalisées</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="1067"/>
        <source>OK</source>
        <translation>OK</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="1074"/>
        <source>Manage Keys In Wallets</source>
        <translation>Gérer les clés dans les trousseaux</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="1101"/>
        <source>Set Default Wallet</source>
        <translation>Choisir le trousseau par défaut</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="1113"/>
        <source>Internal Wallet</source>
        <translation>Trousseau interne</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="1126"/>
        <source>&amp;Libsecret</source>
        <translation>&amp;Libsecret</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="1139"/>
        <source>&amp;KWallet</source>
        <translation>&amp;KWallet</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="1152"/>
        <source>&amp;MACOS Keychain</source>
        <translation>Porte clé &amp;MACOS</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="1165"/>
        <source>&amp;None</source>
        <translation>&amp;Aucun</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="1178"/>
        <source>Windows&apos; DPAPI</source>
        <translation>Windows&apos; DPAPI</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="1199"/>
        <source>Volume Path</source>
        <translation>Chemin du volume</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="1216"/>
        <source>Enter Volume Path Below</source>
        <translation>Saisir le chemin de volume ci dessous</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="1245"/>
        <source>Enter Password Below</source>
        <translation>Saisir le mot de passe ci dessous</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="1274"/>
        <source>Add Volume To Selected Wallet</source>
        <translation>Ajouter le volume au trousseau sélectionné</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="1313"/>
        <location filename="../src/favorites2.cpp" line="345"/>
        <location filename="../src/favorites2.cpp" line="388"/>
        <source>Change Internal Wallet Password</source>
        <translation>Modifier le mot de passe interne du trousseau</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="1361"/>
        <source>TextLabel</source>
        <translation>ÉtiquetteTexte</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="1383"/>
        <source>Ok</source>
        <translation>Ok</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="1390"/>
        <source>Settings</source>
        <translation>Paramétrages</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="1428"/>
        <source>Auto Mount Favorites Volumes When Available</source>
        <translation>Montage Automatique des volumes lorsqu&apos;ils sont disponibles</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="1457"/>
        <source>Show Mount Dialog When Auto Mounting</source>
        <translation>Montrer la boîte de dialogue lors du Montage Automatique</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="1473"/>
        <source>Allow External Tools To Get Passwords From Wallets</source>
        <translation>Autoriser des outils externes pour obtenir les mots de passe des trousseaux</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="1508"/>
        <source>Auto Mount Favorites Volumes At Startup</source>
        <translation>Montage Automatique des volumes au démarrage</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="1537"/>
        <source>Show Favorite List On Context Menu(Restart Required)</source>
        <translation>Montrer la liste des favoris dans le menu contextuel (Redémarrage requis)</translation>
    </message>
    <message>
        <location filename="../src/favorites2.ui" line="1555"/>
        <source>&amp;Close</source>
        <translation>Fermer &amp;C</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="659"/>
        <source>Select A Command Executable</source>
        <translation>Sélectionner une commande à exécuter</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="251"/>
        <location filename="../src/favorites2.cpp" line="1110"/>
        <source>Path To A Config File</source>
        <translation>Chemin vers un fichier de configuration</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="330"/>
        <location filename="../src/favorites2.cpp" line="408"/>
        <source>Change Window&apos;s Wallet Password</source>
        <translation>Modifier le mot de passe de trousseau de Windows</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="354"/>
        <location filename="../src/favorites2.cpp" line="363"/>
        <location filename="../src/favorites2.cpp" line="372"/>
        <location filename="../src/favorites2.cpp" line="379"/>
        <location filename="../src/favorites2.cpp" line="421"/>
        <location filename="../src/favorites2.cpp" line="434"/>
        <location filename="../src/favorites2.cpp" line="446"/>
        <location filename="../src/favorites2.cpp" line="456"/>
        <source>Not Applicable</source>
        <translation>Non Applicable</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="466"/>
        <location filename="../src/favorites2.cpp" line="881"/>
        <location filename="../src/favorites2.cpp" line="1598"/>
        <source>Mount Point Path</source>
        <translation>Chemin du point de montage</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="601"/>
        <source>Toggle AutoMount</source>
        <translation>Basculer Montage Auto</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="615"/>
        <source>Remove Selected Entry</source>
        <translation>Enlever l&apos;entrée sélectionnée</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="622"/>
        <source>Add Entry To Default Wallet</source>
        <translation>Ajouter l&apos;entrée de trousseau par défaut</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="645"/>
        <source>Cancel</source>
        <translation>Annuler</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="731"/>
        <source>Delete Entry</source>
        <translation>Supprimer l&apos;entrée</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="745"/>
        <source>Show Password</source>
        <translation>Montrer le mot de passe</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="769"/>
        <source>Close Menu</source>
        <translation>Fermer le Menu</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="1156"/>
        <location filename="../src/favorites2.cpp" line="1161"/>
        <source>ERROR!</source>
        <translation>ERREUR!</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="1156"/>
        <source>Encrypted Folder Address Field Is Empty</source>
        <translation>Le champ d&apos;adresse du répertoire chiffré est vide</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="1161"/>
        <source>Mount Point Path Field Is Empty</source>
        <translation>Le champ du chemin du point de montage est vide</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="1610"/>
        <source>File Path</source>
        <translation>Chemin du fichier</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="290"/>
        <location filename="../src/favorites2.cpp" line="1304"/>
        <source>Path To An Encrypted Folder</source>
        <translation>Chemin d&apos;accès au répertoire chiffré</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="1265"/>
        <source>Failed To Create Entry</source>
        <translation>Echec de création de l&apos;entrée</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="1314"/>
        <source>Select A File</source>
        <translation>Sélectionner un fichier</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="1324"/>
        <source>Path To Mount Folder</source>
        <translation>Chemin d&apos;accès au répertoire de montage</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="1467"/>
        <source>SSH_AUTH_SOCK Socket Path</source>
        <translation>SSH_AUTH_SOCK Chemin du Socket</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="1468"/>
        <source>IdentityFile Path</source>
        <translation>Chemin d&apos;accès de l&apos;IdentityFile</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="1539"/>
        <source>Create Network Drive</source>
        <translation>Créer un disque réseau</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="1542"/>
        <source>Remote Ssh Server Address
(Example: woof@example.com:/remote/path)</source>
        <translation>Adresse distante du serveur SSH
(Exemple: woof@example.com:/remote/path)</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="1543"/>
        <source>SSH_AUTH_SOCK Socket Path (Optional)</source>
        <translation>SSH_AUTH_SOCK Chemin du Socket (Optionnel)</translation>
    </message>
    <message>
        <location filename="../src/favorites2.cpp" line="1544"/>
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
    <name>fscryptcreateoptions</name>
    <message>
        <location filename="../src/engines/fscryptcreateoptions.ui" line="14"/>
        <source>Select Volume Creation Options </source>
        <translation>Sélectionner les options de création de volume </translation>
    </message>
    <message>
        <location filename="../src/engines/fscryptcreateoptions.ui" line="26"/>
        <source>&amp;Cancel</source>
        <translation>&amp;Annuler</translation>
    </message>
    <message>
        <location filename="../src/engines/fscryptcreateoptions.ui" line="39"/>
        <source>&amp;Ok</source>
        <translation>&amp;Ok</translation>
    </message>
    <message>
        <location filename="../src/engines/fscryptcreateoptions.ui" line="62"/>
        <source>Enter Volume&apos;s Protector Name Below(Optional)</source>
        <translation>Saisir le nom de la protection de volume (optionnel)</translation>
    </message>
    <message>
        <location filename="../src/engines/fscryptcreateoptions.ui" line="81"/>
        <source>Key Type</source>
        <translation>Type de clé</translation>
    </message>
    <message>
        <location filename="../src/engines/fscryptcreateoptions.ui" line="93"/>
        <source>A Custom Passphrase</source>
        <translation>Une passphrase personnalisée</translation>
    </message>
    <message>
        <location filename="../src/engines/fscryptcreateoptions.ui" line="106"/>
        <source>Your Login Passphrase</source>
        <translation>Votre passphrase de Login</translation>
    </message>
    <message>
        <location filename="../src/engines/fscryptcreateoptions.ui" line="119"/>
        <source>A Raw 256-Bit Key</source>
        <translation>Une clé Raw 256 bits</translation>
    </message>
    <message>
        <location filename="../src/engines/fscryptcreateoptions.ui" line="143"/>
        <source>Enter A Path To A Raw 256 Bit Key</source>
        <translation>Entrer le chemin vers la clé Raw 256 bits</translation>
    </message>
    <message>
        <location filename="../src/engines/fscryptcreateoptions.cpp" line="46"/>
        <source>Select A 32 Byte KeyFile</source>
        <translation>Sélectionner un fichier de clé 32 octets (256 bits)</translation>
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
        <location filename="../src/engines/gocryptfscreateoptions.cpp" line="48"/>
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
        <source>Key</source>
        <translation>Clé</translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="100"/>
        <location filename="../src/keydialog.cpp" line="284"/>
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
        <location filename="../src/keydialog.cpp" line="1940"/>
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
        <location filename="../src/keydialog.cpp" line="282"/>
        <source>&amp;Create</source>
        <translation>&amp;Créer</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="302"/>
        <location filename="../src/keydialog.cpp" line="391"/>
        <source>Unlocking &quot;%1&quot;</source>
        <translation>Déchiffrer &quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="306"/>
        <source>Mount Path</source>
        <translation>Chemin de montage</translation>
    </message>
    <message>
        <location filename="../src/keydialog.ui" line="352"/>
        <location filename="../src/keydialog.cpp" line="1307"/>
        <location filename="../src/keydialog.cpp" line="1941"/>
        <source>KeyFile</source>
        <translation>Fichier de clé</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1943"/>
        <source>HMAC+KeyFile</source>
        <translation>HMAC+fichier de clé</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1942"/>
        <source>Key+KeyFile</source>
        <translation>Clé+fichier de clé</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1945"/>
        <source>ExternalExecutable</source>
        <translation>ExternalExecutable</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1944"/>
        <source>YubiKey Challenge/Response</source>
        <translation>YubiKey Challenge/Réponse</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="348"/>
        <source>Check This Box To Make Password Visible</source>
        <translation>Cocher la case pour afficher le mot de passe en clair</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="372"/>
        <source>(%1/%2) Connecting To &quot;%3&quot;</source>
        <translation>(%1/%2) Connecté à &quot;%3&quot;</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="374"/>
        <location filename="../src/keydialog.cpp" line="387"/>
        <source>Connect</source>
        <translation>Connecter</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="376"/>
        <location filename="../src/keydialog.cpp" line="389"/>
        <source>Open</source>
        <translation>Ouvrir</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="378"/>
        <source>(%1/%2) Unlocking &quot;%3&quot;</source>
        <translation>(%1/%2) dévérouille &quot;%3&quot;</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="385"/>
        <source>Connecting To &quot;%1&quot;</source>
        <translation>Connecté à &quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="647"/>
        <source>Create A New &quot;%1&quot; Volume</source>
        <translation>Créer un nouveau volume &quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="715"/>
        <source>Passphrase Quality: 0%</source>
        <translation>Force de la Passphrase : 0%</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="717"/>
        <source>Passphrase Quality: %1%</source>
        <translation>Force de la Passphrase : %1%</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="722"/>
        <location filename="../src/keydialog.cpp" line="1624"/>
        <source>Passphrase Quality: 100%</source>
        <translation>Force de la Passphrase : 100%</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="261"/>
        <location filename="../src/keydialog.cpp" line="789"/>
        <location filename="../src/keydialog.cpp" line="802"/>
        <source>Select A Folder To Create A Mount Point In.</source>
        <translation>Sélectionner un répertoire pour y créer un point de montage.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="531"/>
        <source>Select Volume Type</source>
        <translation>Sélectionnez le type de volume</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="569"/>
        <source>Options</source>
        <translation>Options</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="921"/>
        <location filename="../src/keydialog.cpp" line="1796"/>
        <source>Select A File To Be Used As A Keyfile.</source>
        <translation>Sélectionner un fichier afin de l&apos;utiliser comme un fichier de clé.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="947"/>
        <location filename="../src/keydialog.cpp" line="1637"/>
        <source>Volume Name Field Is Empty.</source>
        <translation>Le champ du Nom du Volume est vide.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1009"/>
        <source>Internal Wallet Is Not Configured.</source>
        <translation>Trousseau Interne non configuré.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1027"/>
        <source>Volume Not Found in &quot;%1&quot;.

Set The Volume Key To Add It To The Wallet Before Mounting.</source>
        <translation>Volume introuvable dans &quot;%1&quot;.

Mettre la clé de volume à ajouter dans le trousseau avant le montage.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1056"/>
        <source>Upgrade File System</source>
        <translation>Mettre à jour le système de fichiers</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1060"/>
        <source>Replace File System</source>
        <translation>Remplacer le système de fichiers</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1329"/>
        <source>Volume Key Can Not Be Empty.</source>
        <translation>La clé de volume ne peut pas être vide.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1402"/>
        <source>Failed To Add A Volume To The A Wallet.</source>
        <translation>Echec de l&apos;ajout du volume au trousseau.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1405"/>
        <source>Volume Already Exists In The Wallet.</source>
        <translation>Volume déjà existant dans le trousseau.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1410"/>
        <source>Failed To Open Wallet.</source>
        <translation>Echec de l&apos;ouverture du trousseau.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1475"/>
        <source>Failed To Generate Key.</source>
        <translation>Echec de la génération de clé.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1949"/>
        <source>Internal Wallet</source>
        <translation type="unfinished">Trousseau interne</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1954"/>
        <source>Gnome Wallet</source>
        <translation type="unfinished">Trousseau Gnome</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1959"/>
        <source>Kde Wallet</source>
        <translation type="unfinished">Trousseau Kde</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1964"/>
        <source>OSX KeyChain</source>
        <translation type="unfinished">Porte clés OSX</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1969"/>
        <source>Windows DPAPI</source>
        <translation type="unfinished">Windows DPAPI</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1244"/>
        <source>Mount Point Path Is Not Empty.</source>
        <translation>Le champ du chemin du point de montage n&apos;est pas vide.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1689"/>
        <source>Failed To Locate Or Run Yubikey&apos;s &quot;ykchalresp&quot; Program.</source>
        <translation>Echec de la localisation ou de l&apos;exécution de Yubikey&apos;s &quot;ykchalresp&quot; Program.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1600"/>
        <source>Select A KeyFile</source>
        <translation>Sélectionner un fichier de clé</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1133"/>
        <source>Encrypted Folder Path Is Already Taken.</source>
        <translation>Chemin d&apos;accès du répertoire chiffré déjà occupé.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1144"/>
        <location filename="../src/keydialog.cpp" line="1149"/>
        <location filename="../src/keydialog.cpp" line="1156"/>
        <location filename="../src/keydialog.cpp" line="1164"/>
        <location filename="../src/keydialog.cpp" line="1231"/>
        <location filename="../src/keydialog.cpp" line="1239"/>
        <source>Mount Point Path Already Taken.</source>
        <translation>Point de montage déjà occupé.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1219"/>
        <source>Atleast One Required Field Is Empty.</source>
        <translation>Au moins un des champs qui est requis est vide.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1644"/>
        <source>Create A Volume With Specified Key And Then Add The Key In 
&quot;%1&quot;.</source>
        <translation>Crée un volume avec la clé spécifiée et ajouter la clé dans 
&quot;%1&quot;.</translation>
    </message>
    <message>
        <location filename="../src/keydialog.cpp" line="1758"/>
        <source>Plugin name</source>
        <translation>Plugin name</translation>
    </message>
    <message>
        <source>Keyfile path</source>
        <translation type="vanished">Chemin du fichier de clé</translation>
    </message>
</context>
<context>
    <name>oneinstance</name>
    <message>
        <location filename="../src/oneinstance.cpp" line="59"/>
        <location filename="../src/oneinstance.cpp" line="67"/>
        <source>Previous instance seem to have crashed,trying to clean up before starting</source>
        <translation>L&apos;instance précédente semble avoir crashé, tentative de réinitialisation avant démarrage</translation>
    </message>
    <message>
        <location filename="../src/oneinstance.cpp" line="40"/>
        <source>There seem to be another instance running,exiting this one</source>
        <translation>Il semble y avoir une autre instance en cours d&apos;exécution, sortie de celle-ci</translation>
    </message>
</context>
<context>
    <name>options</name>
    <message>
        <location filename="../src/engines/options.ui" line="17"/>
        <source>Set Mount Options</source>
        <translation>Paramétrer les options de montage</translation>
    </message>
    <message>
        <location filename="../src/engines/options.ui" line="151"/>
        <source>Automatically Unmount After Specified Minutes of Inactivity Is Reached.</source>
        <translation>Démonter automatiquement lorsque que le délai d&apos;inactivité spécifié -en minutes- est atteint.</translation>
    </message>
    <message>
        <location filename="../src/engines/options.ui" line="119"/>
        <source>Mount The Volume With Below Mount Options.</source>
        <translation>Monter le volume avec les options de montage ci-dessous.</translation>
    </message>
    <message>
        <location filename="../src/engines/options.ui" line="61"/>
        <source>Unlock A Volume With Below Configuration File.</source>
        <translation>Déchiffrer le volume avec le fichier de configuration ci-dessous.</translation>
    </message>
    <message>
        <location filename="../src/engines/options.ui" line="90"/>
        <source>Unlock A Volume With Below KeyFile.</source>
        <translation>Déchiffrer le volume avec le fichier de clé ci-dessous.</translation>
    </message>
    <message>
        <location filename="../src/engines/options.ui" line="181"/>
        <source>&amp;Set</source>
        <translation>&amp;Paramétrer</translation>
    </message>
    <message>
        <location filename="../src/engines/options.ui" line="194"/>
        <source>&amp;Cancel</source>
        <translation>&amp;Annuler</translation>
    </message>
    <message>
        <location filename="../src/engines/options.ui" line="207"/>
        <location filename="../src/engines/options.cpp" line="69"/>
        <source>Reverse Mode.</source>
        <translation>Mode Inversé.</translation>
    </message>
    <message>
        <location filename="../src/engines/options.cpp" line="71"/>
        <source>Unlock %1 Volume With A KeyFile.</source>
        <translation>Déchiffrer le volume %1 avec le fichier de clé.</translation>
    </message>
    <message>
        <location filename="../src/engines/options.cpp" line="73"/>
        <source>Unlock %1 Volume With A Configuration File.</source>
        <translation>Déchiffrer le volume %1 avec le fichier de configuration.</translation>
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
        <location filename="../src/plugin.cpp" line="158"/>
        <source>KeyFile</source>
        <translation>Fichier de clé</translation>
    </message>
    <message>
        <location filename="../src/plugin.cpp" line="111"/>
        <location filename="../src/plugin.cpp" line="146"/>
        <source>ERROR</source>
        <translation>ERREUR</translation>
    </message>
    <message>
        <location filename="../src/plugin.cpp" line="111"/>
        <source>KeyFile Not Set</source>
        <translation>Fichier de clé non actif</translation>
    </message>
    <message>
        <location filename="../src/plugin.cpp" line="146"/>
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
        <location filename="../src/engines/securefscreateoptions.cpp" line="54"/>
        <source>The &quot;lite format&quot; simply encrypts filenames and file contents separately, similar to how encfs operates, although with more security.

The &quot;full format&quot; maps files, directories and symlinks in the virtual filesystem all to regular files in the underlying filesystem. The directory structure is flattened and recorded as B-trees in files.

The lite format has become the default on Unix-like operating systems as it is much faster and features easier conflict resolution, especially when used with DropBox, Google Drive, etc. The full format, however, leaks fewer information about the filesystem hierarchy, runs relatively independent of the features of the underlying filesystem, and is in general more secure.</source>
        <translation>Le &quot;format allégé&quot; chiffre simplement les noms de fichiers et le contenu des fichiers séparément, de la même façon que ce qu&apos;opère encfs, avec toutefois plus de sécurité.

Le &quot;format complet&quot; mappe les fichiers, répertoires, liens symboliques du système de fichiers virtuel dans des fichiers réguliers sous-jacents du système de fichiers. La structure de l&apos;arborescence est réduite et sauvegardée des fichiers B-arbres.

Le &quot;format allégé&quot; est devenu le mode par défaut des systèmes basés Unix car il est plus rapide et plus permet une résolution plus facile de conflits notamment lors de l&apos;utilisation de DropBox, Google Drive, etc. Le &quot;format complet&quot;, toutefois, perd moins d&apos;infos sur la hiérarchie du système de fichiers, s&apos;exécute plus indépendamment du système de fichiers sous-jacent et il est en général plus sécurisé.</translation>
    </message>
    <message>
        <location filename="../src/engines/securefscreateoptions.cpp" line="90"/>
        <source>Select A KeyFile</source>
        <translation>Sélectionner un fichier de clé</translation>
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
    <message>
        <location filename="../src/engines/securefscreateoptions.ui" line="140"/>
        <source>Select A KeyFile(Optional).</source>
        <translation>Sélectionner un fichier de clé (optionnel).</translation>
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
        <location filename="../src/sirikali.cpp" line="619"/>
        <source>Unmount All</source>
        <translation>Démonter tout</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="450"/>
        <location filename="../src/sirikali.cpp" line="452"/>
        <source>%1 Is Not Installed</source>
        <translation>%1 n&apos;est pas installé</translation>
    </message>
    <message>
        <location filename="../src/sirikali.ui" line="122"/>
        <location filename="../src/sirikali.cpp" line="625"/>
        <source>Favorites</source>
        <translation>Favoris</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="612"/>
        <source>Check For Updates</source>
        <translation>Tester si la version du logiciel est à jour</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="632"/>
        <source>About</source>
        <translation>À propos</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="639"/>
        <source>Quit</source>
        <translation>Quitter</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="636"/>
        <source>Show/Hide</source>
        <translation>Montrer/Cacher</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="976"/>
        <source>ERROR: Volume Path Not Given.</source>
        <translation>ERREUR: Chemin du volume non fourni.</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1011"/>
        <source>ERROR: Key Not Found In The Backend.</source>
        <translation>ERREUR: Clé non trouvée dans le Gestionnaire.</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1016"/>
        <source>ERROR: Failed To Unlock Requested Backend.</source>
        <translation>ERREUR: Echec du déverrouillage du Gestionnaire sélectionné.</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="849"/>
        <location filename="../src/sirikali.cpp" line="1382"/>
        <location filename="../src/sirikali.cpp" line="2161"/>
        <source>ERROR</source>
        <translation>ERREUR</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1382"/>
        <source>Failed To Read Volume Properties</source>
        <translation>Echec de la lecture des propriétés du volume</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1362"/>
        <location filename="../src/sirikali.cpp" line="1384"/>
        <location filename="../src/sirikali.cpp" line="1408"/>
        <source>INFORMATION</source>
        <translation>INFORMATION</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="300"/>
        <source>Close All Volumes Before Quitting The Application</source>
        <translation>Fermer tous les volumes avant de quitter l&apos;application</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="313"/>
        <source>Close The Following File System(s) Before Quitting The Application
&quot;%1&quot;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="415"/>
        <source>Mount Folder</source>
        <translation>Répertoire monté</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="417"/>
        <source>Mount File</source>
        <translation>Fichier monté</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="622"/>
        <source>Unmount All And Quit</source>
        <translation>Démonter tout et quitter</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="629"/>
        <source>Settings</source>
        <translation>Paramétrages</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="634"/>
        <source>FAQ</source>
        <translation>Foire Aux Questions</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="849"/>
        <source>&quot;%1&quot; Folder Must Be Writable</source>
        <translation>&quot;%1&quot; Répertoire devant être autorisé en écriture</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1444"/>
        <source>Unmount</source>
        <translation>Démonter</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1448"/>
        <source>Add To Favorites</source>
        <translation>Ajouter aux favoris</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1871"/>
        <source>Select A File To Be Mounted</source>
        <translation>Sélectiionner un fichier à monter</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="2216"/>
        <source>Failed To Unmount 1 Volume.</source>
        <translation>Echec de démontage d&apos;un volume.</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="2218"/>
        <source>Failed To Unmount %1 Volumes.</source>
        <translation>Echec de démontage de %1 volumes.</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1442"/>
        <source>Open Folder</source>
        <translation>Ouvrir Répertoire</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1446"/>
        <source>Properties</source>
        <translation>Propriétés</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1452"/>
        <source>Close Menu</source>
        <translation>Fermer le Menu</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="301"/>
        <location filename="../src/sirikali.cpp" line="315"/>
        <location filename="../src/sirikali.cpp" line="1554"/>
        <location filename="../src/sirikali.cpp" line="2216"/>
        <location filename="../src/sirikali.cpp" line="2218"/>
        <source>WARNING</source>
        <translation>Attention</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1555"/>
        <source>Could Not Open Mount Point Because &quot;%1&quot; Tool Does Not Appear To Be Working Correctly.</source>
        <translation>Impossible d&apos;ouvrir le point de montage parce que l&apos;outil &quot;%1&quot; ne semble pas fonctionner correctement.</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1604"/>
        <source>&amp;Quit</source>
        <translation>&amp;Quitter</translation>
    </message>
    <message>
        <location filename="../src/sirikali.cpp" line="1868"/>
        <source>Select An Encrypted Volume Directory</source>
        <translation>Sélectionner un répertoire de volume</translation>
    </message>
</context>
</TS>
