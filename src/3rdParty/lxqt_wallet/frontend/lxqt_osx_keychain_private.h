
/*
 * copyright: 2017
 * name : Francis Banyikwa
 * email: mhogomchungu@gmail.com
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef LXQT_OSX_KEYCHAIN_PRIVATE_H
#define LXQT_OSX_KEYCHAIN_PRIVATE_H

using OSStatus = int;

class foo;
using SecKeychainItemRef = foo *;

#define noErr 0
#define errSecSuccess 0

void SecKeychainItemFreeContent(void *e, void *f)
{
    Q_UNUSED(e);
    Q_UNUSED(f);
}

void CFRelease(void *e)
{
    Q_UNUSED(e);
}

OSStatus SecKeychainItemDelete(void *e)
{
    Q_UNUSED(e);
    return 0;
}

OSStatus SecKeychainFindGenericPassword(void *foo,
                                        quint32 serviceNameLength,
                                        const char *serviceName,
                                        quint32 accountNameLength,
                                        const char *accountName,
                                        quint32 *passwordLength,
                                        void **passwordData,
                                        SecKeychainItemRef *itemRef)
{
    Q_UNUSED(foo);
    Q_UNUSED(serviceNameLength);
    Q_UNUSED(serviceName);
    Q_UNUSED(passwordLength);
    Q_UNUSED(passwordData);
    Q_UNUSED(accountName);
    Q_UNUSED(accountNameLength);
    Q_UNUSED(itemRef);

    return 0;
}

OSStatus SecKeychainAddGenericPassword(void *foo,
                                       quint32 serviceNameLength,
                                       const char *serviceName,
                                       quint32 accountNameLength,
                                       const char *accountName,
                                       quint32 passwordLength,
                                       const void *passwordData,
                                       SecKeychainItemRef *itemRef)
{
    Q_UNUSED(foo);
    Q_UNUSED(serviceNameLength);
    Q_UNUSED(serviceName);
    Q_UNUSED(passwordLength);
    Q_UNUSED(passwordData);
    Q_UNUSED(accountName);
    Q_UNUSED(accountNameLength);
    Q_UNUSED(itemRef);

    return 0;
}

void SecKeychainItemModifyContent(void *a, void *b, quint32 c, const void *d)
{
    Q_UNUSED(a);
    Q_UNUSED(b);
    Q_UNUSED(c);
    Q_UNUSED(d);
}

#endif
