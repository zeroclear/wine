/*
 * Print Ticket Services Module
 *
 * Copyright 2014 Jactry Zeng for CodeWeavers
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include <stdarg.h>

#include "windef.h"
#include "winbase.h"
#include "wingdi.h"
#include "winspool.h"
#include "prntvpt.h"
#include "wine/heap.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(prntvpt);

struct prn_provider
{
    DWORD owner;
    HANDLE hprn;
};

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD reason, LPVOID reserved)
{
    TRACE("(%p, %d, %p)\n", hinst, reason, reserved);

    switch(reason)
    {
        case DLL_WINE_PREATTACH:
            return FALSE; /* prefer native version */
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hinst);
            break;
    }
    return TRUE;
}

HRESULT WINAPI PTQuerySchemaVersionSupport(PCWSTR printer, DWORD *version)
{
    FIXME("stub:%s %p\n", debugstr_w(printer), version);
    return E_NOTIMPL;
}

HRESULT WINAPI PTOpenProvider(PCWSTR printer, DWORD version, HPTPROVIDER *provider)
{
    DWORD used_version;

    TRACE("%s, %d, %p\n", debugstr_w(printer), version, provider);

    if (version != 1) return E_INVALIDARG;

    return PTOpenProviderEx(printer, 1, 1, provider, &used_version);
}

HRESULT WINAPI PTOpenProviderEx(const WCHAR *printer, DWORD max_version, DWORD pref_version, HPTPROVIDER *provider, DWORD *used_version)
{
    struct prn_provider *prov;

    TRACE("%s, %d, %d, %p, %p\n", debugstr_w(printer), max_version, pref_version, provider, used_version);

    if (!max_version || !provider || !used_version)
        return E_INVALIDARG;

    prov = heap_alloc(sizeof(*prov));
    if (!prov) return E_OUTOFMEMORY;

    if (!OpenPrinterW((LPWSTR)printer, &prov->hprn, NULL))
    {
        heap_free(prov);
        return HRESULT_FROM_WIN32(GetLastError());
    }

    prov->owner = GetCurrentThreadId();
    *provider = (HPTPROVIDER)prov;
    *used_version = 1;

    return S_OK;
}
