## SiriKali works on Linux, MACOS and Microsoft Windows

SiriKali is a Qt/C++ GUI application that manages ecryptfs,cryfs,encfs,gocryptfs and securefs based encrypted folders. These projects are compared <a href="https://nuetzlich.net/gocryptfs/comparison/">here.</a>SiriKali can also connect to ssh servers using ssh.

#### Supported Backends on Linux:

```
<a href="https://github.com/netheril96/securefs">1. securefs.</a>

<a href="https://github.com/mhogomchungu/ecryptfs-simple">ecryptfs</a>

<a href="https://www.cryfs.org/">3. cryfs.</a>

<a href="https://github.com/vgough/encfs">4. encfs.</a>

<a href="https://nuetzlich.net/gocryptfs">5. gocryptfs.</a>

<a href="https://github.com/libfuse/sshfs">6. sshfs.</a>

```
#### Supported Backends on MACOS:

```
<a href="https://github.com/netheril96/securefs">1. securefs.</a>

<a href="https://www.cryfs.org/">2. cryfs.</a>

<a href="https://github.com/vgough/encfs">3. encfs.</a>

<a href="https://nuetzlich.net/gocryptfs">4. gocryptfs.</a>

```
#### Supported Backends on Microsoft Windows:

```
<a href="https://github.com/netheril96/securefs">1. securefs.</a>

<a href="https://github.com/mhogomchungu/encfs/latest">2. encfs.</a>

<a href="https://github.com/billziss-gh/sshfs-win">3. sshfs.</a> Steps to setup SiriKali to
connect to an ssh server are documented <a href="https://github.com/mhogomchungu/sirikali/wiki/Frequently-Asked-Questions#90-how-do-i-add-options-to-connect-to-an-ssh-server">here.</a>
```

Encrypted container ```folders``` have an advantage over encrypted container ```files``` like the ones that are created by <a href="http://mhogomchungu.github.io/zuluCrypt/">zuluCrypt</a>,TrueCrypt,VeraCrypt among other projects that use file based encrypted containers.

#### Advantages are:

The encrypted container folder can freely grow and shrink as files are added,removed,grow or shrink. File based encrypted containers are limited to the size of the container and the size is set when the container is created and does not change to reflect the amount of data the container is hosting.

#### Disadvantages are:

The encrypted container folder does not hide the space usage of its contents and an adversary can derive meaning from space usage of the encrypted container folder. File based container hides the space utilization of the volume and the only thing an adversary can see is the fixed size of the container.

More advantages/disadvantages are discussed <a href="https://www.cryfs.org/comparison">here.</a>

## FAQ

Most frequently asked questions are answered <a href="https://github.com/mhogomchungu/SiriKali/wiki/Frequently-Asked-Questions">here.</a>

## DONATIONS

```
Paypal does not work where i live(United Republic of Tanzania)
and i accept donations only through Western Union and bank transfers.

1. Information for donating through western union
Name:    Francis Nkulu Banyikwa
Email:   banyikwafb@gmail.com
Country: United Republic Of Tanzania

The email address should be used to send tracking number(MTCN)

2. Bank information for donating through bank tranfer:
Name:		Francis Nkulu Banyikwa
Bank Name:	CRDB BANK PLC
Bank Location:	Tegeta Kibo complex,
		Dar Es Salaam,
		United Republic of Tanzania
Swift Code:	CORUTZTZ
Account Number:	0152454497500
```

## Download links

Latest released version is 1.3.5 and its source code can be downloaded by clicking below link.

<a href="https://github.com/mhogomchungu/sirikali/releases/download/1.3.5/SiriKali-1.3.5.tar.xz">SiriKali-1.3.5.tar.xz</a>

Users of Microsoft Windows Operating System should download SiriKali installer by clicking below link. The installer is not
signed and expect to get a warning from windows saying the installer is from unknown publisher.

<a href="https://github.com/mhogomchungu/sirikali/releases/download/1.3.5/SiriKali-setup.exe">SiriKali-setup.exe</a>

```
Currently, SiriKali on windows supports only securefs,encfs and sshfs backends.
SiriKali will expect to find securefs at "~/bin" and in window's language,
SiriKali will expect to find "securefs.exe" at "C:\Users\<username>\bin"
```

## Binary packages

Check first if SiriKali is present in your distribution's repositories and use that version if it is.
```Fedora,opensuse,ubuntu and debian``` users can install binary packages by following instructions on <a href="http://software.opensuse.org//download.html?project=home%3Aobs_mhogomchungu&package=sirikali">this</a> link.

<a href="http://macdownload.informer.com/sirikali/" target="_blank"><img border="0" src="http://img.informer.com/awards/mi-award-epick4.png" alt="Mac Informer Editor's pick award" height="129" width="130" /></a>


## Screenshots

<a href="http://tinypic.com?ref=1572g3n" target="_blank"><img src="http://i67.tinypic.com/1572g3n.png" border="0" alt="Image and video hosting by TinyPic"></a>

<a href="http://tinypic.com?ref=v81umb" target="_blank"><img src="http://i64.tinypic.com/v81umb.png" border="0" alt="Image and video hosting by TinyPic"></a>

<a href="http://tinypic.com?ref=2h2hax3" target="_blank"><img src="http://i67.tinypic.com/2h2hax3.png" border="0" alt="Image and video hosting by TinyPic"></a>

<a href="http://tinypic.com?ref=2rojij6" target="_blank"><img src="http://i65.tinypic.com/2rojij6.png" border="0" alt="Image and video hosting by TinyPic"></a>

## Contact info:
```
Name: Francis Banyikwa
email: banyikwafb@gmail.com

gpg key finderpring: E3AF84691424AD00E099003502FC64E8DEBF43A8

The key can be retrieved with command: gpg --recv-keys 0x02FC64E8DEBF43A8

The key is below:

-----BEGIN PGP PUBLIC KEY BLOCK-----

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
9azeqex9
=L6yQ
-----END PGP PUBLIC KEY BLOCK-----
```

last update: Wed Aug  1 21:59:11 EAT 2018
