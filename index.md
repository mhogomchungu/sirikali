## SiriKali works on Linux, macOS and Microsoft Windows Operating Systems

SiriKali is a Qt/C++ GUI application that manages ecryptfs, cryfs, encfs, gocryptfs, fscrypt, securefs and Cryptomator encrypted folders. These projects are compared <a href="https://nuetzlich.net/gocryptfs/comparison/">here.</a> SiriKali can also connect to ssh servers using sshfs.

#### Supported Backends on Linux:

<a href="https://github.com/google/fscrypt">Fscrypt</a>, <a href="https://github.com/netheril96/securefs">Securefs</a>, <a href="https://github.com/mhogomchungu/ecryptfs-simple">Ecryptfs</a>, <a href="https://www.cryfs.org/">Cryfs</a>, <a href="https://github.com/vgough/encfs">Encfs, </a><a href="https://nuetzlich.net/gocryptfs">Gocryptfs</a>, <a href="https://github.com/libfuse/sshfs">Sshfs</a> and <a href="https://github.com/cryptomator/cli">Cryptomator</a>.

#### Supported Backends on Flatpak:

<a href="https://github.com/netheril96/securefs">Securefs</a>, <a href="https://www.cryfs.org/">Cryfs</a>, <a href="https://nuetzlich.net/gocryptfs">Gocryptfs</a> and <a href="https://github.com/cryptomator/cli">Cryptomator</a>.

#### Supported Backends on Microsoft Windows:

 <a href="https://www.cryfs.org/">Cryfs</a>, <a href="https://github.com/netheril96/securefs">Securefs</a>, <a href="https://github.com/mhogomchungu/encfs">Encfs</a>, <a href="https://github.com/billziss-gh/sshfs-win">Sshfs</a>, <a href="https://github.com/bailey27/cppcryptfs">Cppcryptfs</a> and <a href="https://github.com/cryptomator/cli">Cryptomator</a>.


#### Supported Backends on macOS:

<a href="https://github.com/netheril96/securefs">Securefs, </a><a href="https://www.cryfs.org/">Cryfs, </a><a href="https://github.com/vgough/encfs">Encfs</a> and <a href="https://nuetzlich.net/gocryptfs">Gocryptfs</a>.


#### Using SiriKali to connect to ssh server using sshfs:

Steps to setup SiriKali to connect to an ssh server are documented <a href="https://github.com/mhogomchungu/sirikali/wiki/Frequently-Asked-Questions#90-how-do-i-add-options-to-connect-to-an-ssh-server">here.</a>

## FAQ

Question: How do i install these backends?
Steps to install a backend are below:-
1. Click "Menu" button.
2. Click "Check For Updates".
3. Right click an entry you want to install.
4. Click "Update" to install it if the backend can be installed by SiriKali to its private location. If SiriKali can not install it then go to "Menu"->"Settings"->"External Commands" and then put the executable where "Executable Search Path" points to.

Rest of FAQs are <a href="https://github.com/mhogomchungu/SiriKali/wiki/Frequently-Asked-Questions">here.</a>

## Source download link.

Latest released version is 1.8.3 and its source code can be downloaded by clicking below link.

<a href="https://github.com/mhogomchungu/sirikali/releases/download/1.8.3/SiriKali-1.8.3.tar.xz">SiriKali-1.8.3.tar.xz</a>

## Download links for macOS.

MacOS bundle can be downloaded from <a href="https://github.com/mhogomchungu/sirikali/releases/download/1.8.3/SiriKaliQt6-1.8.3.dmg">here</a>. The bundle is for x86_64 only and is not notarized so you may need to jump a few hoops to get it to run on your system. Supported versions of MacOS is 13+.

## Download links for Microsoft Windows Operating System.

The installer for Microsoft Windows operating system is <a href="https://github.com/mhogomchungu/sirikali/releases/download/1.8.3/SiriKaliQt5-1.8.3.setup.exe">here</a> for those with windows7 going up and <a href="https://github.com/mhogomchungu/sirikali/releases/download/1.8.3/SiriKaliQt6-1.8.3.setup.exe">here</a> for those with Windows10 going up.

The installer is not signed and expect to get a warning from windows saying the installer is from an unknown publisher.
We recommend using the installer version.

There is also a portable version and it is found <a href="https://github.com/mhogomchungu/sirikali/releases/download/1.8.3/SiriKaliQt5-1.8.3.zip">here</a> for those with windows7 going up and <a href="https://github.com/mhogomchungu/sirikali/releases/download/1.8.3/SiriKaliQt6-1.8.3.zip">here</a> for those with windows10 going up.

Packages are also available for <a href="https://github.com/microsoft/winget-pkgs/tree/master/manifests/f/FrancisBanyikwa/SiriKali">Winget</a> and <a href="https://github.com/ScoopInstaller/Extras/blob/master/bucket/sirikali.json">Scoop</a>.

## Flatpak package.

Flatpak users should install SiriKali from <a href="https://flathub.org/apps/io.github.mhogomchungu.sirikali">Flathub</a>.

## Binary packages for Linux.

Check first if SiriKali is present in your distribution's repositories and use that version if it is.
```Fedora,opensuse,ubuntu and debian``` users can install binary packages by following instructions on <a href="http://software.opensuse.org//download.html?project=home%3Aobs_mhogomchungu&package=sirikali">this</a> link.

Gentoo users can get SiriKali from <a href="https://github.com/holgersson32644/holgersson-overlay">holgersson overlay.</a>

Arch Linux users can get SiriKali from <a href="https://aur.archlinux.org/packages/sirikali/">AUR.</a>

<a href="http://macdownload.informer.com/sirikali/" target="_blank"><img border="0" src="http://img.informer.com/awards/mi-award-epick4.png" alt="Mac Informer Editor's pick award" height="129" width="130" /></a>

## Where did the name come from?
"SiriKali" name is a combination of two Swahili words, "siri" and "kali".In Swahili, "Siri" means <a href="https://translate.google.com/#view=home&op=translate&sl=sw&tl=en&text=siri">"secret"</a> and "kali" means <a href="https://translate.google.com/#view=home&op=translate&sl=sw&tl=en&text=kali">"severe".</a> "SiriKali" can therefore loosely be translated to "top secret".

## Screenshots

<a href="https://github.com/mhogomchungu/sirikali/raw/master/images/Screenshot_20190912_113741.png" target="_blank"><img src="https://github.com/mhogomchungu/sirikali/raw/master/images/Screenshot_20190912_113741.png"></a>

<a href="https://github.com/mhogomchungu/sirikali/raw/master/images/Screenshot_20190912_113930.png" target="_blank"><img src="https://github.com/mhogomchungu/sirikali/raw/master/images/Screenshot_20190912_113930.png"></a>

<a href="https://github.com/mhogomchungu/sirikali/raw/master/images/Screenshot_20190912_114512.png" target="_blank"><img src="https://github.com/mhogomchungu/sirikali/raw/master/images/Screenshot_20190912_114512.png"></a>

<a href="https://github.com/mhogomchungu/sirikali/raw/master/images/Screenshot_20190912_114700.png" target="_blank"><img src="https://github.com/mhogomchungu/sirikali/raw/master/images/Screenshot_20190912_114700.png"></a>

## Contact info:
```
Name: Francis Banyikwa
email: banyikwafb@gmail.com

gpg key finderpring: E3AF84691424AD00E099003502FC64E8DEBF43A8

The key can be retrieved with command: gpg --recv-keys 0x02FC64E8DEBF43A8

The key is below:

-----BEGIN PGP PUBLIC KEY BLOCK-----

mQINBFmVdDYBEAC/pVNcWOEw5PMLtGiCR9WhY5LOMnR7RlXX4l7JPTiYWGAq+WOV
3n6ueZ8TMsFxRl2TZoV7u1SyExP1RbBrtS2d8aQM/GmWFh1HSoUfiOwXqCFVTExC
ORh5lgOTOWH/zkYLJksrcFhDyeip6Bjy368eFHkVRnSmYR5xj0UQpM0bsd3VGmsY
5YmGMudqAblutdME7DYX8E7tQ91JH8jTmtoLI99/1NVa28W75C0ixC+nu767rMFb
eIEzer9eZSi3Eud2u9Cj3vVyTI9f7uWltpJ3V9RbpApdc4NFPCrqaJuXyooGUdfJ
TPc2ofBkT/Oxyl2aNd5bl7Xexf1TprXRv5MN1FQ1pqYN8gjYWc/ZfVNQ/kR5kIx5
ZX1xMaDscyZZX96lP+xYVhfVSehFVDC2DtXPwbiQR8/EnyuRXUMnNWas0FUoZCKJ
VCeYFpvHJUKdrvc6jc3jKM+8Mu9rrB7KvgqRb+MrjT0LKFK4N40xCJT0+evlDljA
5HmcEvwOTnBIy/JnqsjjCNCO80tlS4fuibBSv0z0MIiaggQHzsU+UcxrEDCSMaSM
rVxfINXuOCEhS4Lw7Lwge28bYV1Sdvr6JJI3BC1mrX06l+FVm9ccOKAPMmcbyjMc
32cuJxWdaJdJsYKnR5+S11JrhjF24OsqCTQhAOF3EPPKEl7eo5/S//kTDQARAQAB
tCdGcmFuY2lzIEJhbnlpa3dhIDxiYW55aWt3YWZiQGdtYWlsLmNvbT6JAjgEEwEC
ACIFAlmVdDYCGwMGCwkIBwMCBhUIAgkKCwQWAgMBAh4BAheAAAoJEAL8ZOjev0Oo
W/YP+wXokzRTzVMQOFENGuSAvC3ZvsVa6o2Pu9R/CoVFE5I7KIN6i8ZdECO3E0Bw
8VXXPmo/LB+/fCzeKBPqdEAHl1+q0OJU2peYfqHz/2MwZ9lQ29Oh503SmXg914oQ
ChZQDEdLnaNIzs+2EOGO6XVvzB2Mnt161FdzvZt2xMrpxO4rFtjztwTuCEwwHjVS
w/zAsWJ3X4NKhPbjwNM7PAF81hmVAh/ysr1yOCE8O9qr+QDMJRCe1IQayFFB9X6R
K67q9k3brW6XvinzYn2fg36zwkuMcKsFZDy/OGpdkVYqRon74cliNmSdIz50S5Ph
LQWfeblVSUfv+p0J1uXXSg3sTYh24cotWzn4/FlChBIaGVon0SQRIFVqGDxoK+nQ
vlq5c8dLWn7Hg4oyOaECn0IsTIOHNzrA56yHuqtTOUe3aMhXMyInZ85LM3dYB58d
Edbw58AWE0fEGsESbiaUKilZQ2hh19u8JsBwpxBTLYQTO0qem0LJxlDRP0PwApKN
xXbdjFR4EgT6JED5bR7u2mGOA4IY0CPDGmecCNf4hFtHpwnfHt15n//3SjdaAbGV
9eu3i6xTlq41nKyWALPpRLgFXqLcJUiMVa+9OQ8vGSZlYxBOC9rRopL2qVyxzNhQ
m9l+w2aDyauzVW2Kgeie6snxsFSD06B3PYdgYzcHI8Oc2RFzuQINBFmVdDYBEACW
0TiKpNsGnG9A2trMAiIW0K+jKd6pOs8JOphC4QPWnoDdD4Pdwka99LjOAPS6m1ok
81Wf5ogc8/czP6sYkzloFIpEEKI7LlsmBRXnlW9Gn6LKC6WAeDTLVXP0ZUIh5MsW
DqvgpWQKjsV/p5RNubQuFS82JWutAyughz0CYwSSfHJBWxeDdDp3y9pVJKilucEp
vB8axiDDiEPZkCaNGp3zUIiFJo0ndyCKyVUCO+HK22NCP5pkAOEZXbwCIDnL8GrG
p/gwFreotBshaDbX+UCQfbkzZ5zPOFXOlpCQc7Omh1EWTX1ws9nUiBzutJOggdAZ
UHr2drmRujIRhZlXwDGORqLYDNR8kNYeA6UrmZ5JhthI8vfdBEmC/WTPDIHvonpA
39V9n6LqSTc/+0fR9qe/qaliy9Nbr3q5hJXa75+bcP567dgNKhFvhUreWEwzPqaB
NXO4xyG8HT2shH9DfFaE4rwYb+K0COo/9KyZD5QEvMnV7FEk+I8cliCU/NPYLSNp
9nRhopyAhqwvr/7fEJ/W7UhUcl4+aht4B6lWUHW+72Hvb5F/ehbKSthW0azzRjBJ
nXTi9XrJFsNzzHHlNI/QJxAiAJ3Z2mKBS8iJ2RR2R3prGIocKu3e9hVM6HSfF4ZS
curwmRejKHLrUs6kg22ZqBzvmPhg5W8rwIvsi2/2pwARAQABiQIfBBgBAgAJBQJZ
lXQ2AhsMAAoJEAL8ZOjev0OoGWEP/3kvNrOz/+JW1SGQ4SrrL6dihIUhmdgdDBH0
9d+UX+70ao2hk0tv6lDARy8uf9tt4fJ75otRvYS3PR9Sn0PUohqen4+6OsfoydUX
e9qz934jWHUv+QV6KhifC3qh3ECi2/ouRS0Mnxzd1CaD0np9d/fSt4porfUJ5ck1
IfSPu7TFnnBhk6xt0V/SGEspcTnDAiiDcY6agMbCPo7rXMNyGVgk/a6OsO2qQGX5
42Hp4dPTXSjy1JiZ1htRvoau8OSrfEsHjW+x0dz6fyByW5EfewZXmFCjM3w+G5bT
2yAL//AkBZlL7T7IIdqkA4pVFEwAn2V1Xs5hfDuoRTFT4WkIkhaDIhiQZk/lHAz8
aZPWktdHzAROXiQvLEX+L5mjXgO8hPud7yXZTRIpDPRdKMZA0XVTwee4LxX/VnfB
cE2guccUEFD+kZiR3Dng91V9xZ/DIu98ZcAV46lYhK83Csr6XGOPdIoIdT/v/3gc
wvtwg4b/D8c1TmXPISCIX/cBzT3al1xjOPGQ/v+XnqMafJI5MHnFUzoRleE6slVB
MLTWZGcEfBcgMIzoSF+i4dqg1v3dv1pDx1X3IR0n7440uDV1XDt48j5Rv51O0GdP
cv9jY9bZAbVaMUjF7dw/bsqgJ2HCJ33cf16GybRtozOE36YDB2omg58ngIrlmlea
9azeqex9
=L6yQ
-----END PGP PUBLIC KEY BLOCK-----
```

last update: Mon Apr 21 10:53:34 AM EAT 2025
