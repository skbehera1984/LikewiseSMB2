/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 * -*- mode: c, c-basic-offset: 4 -*- */

/*
 * Copyright Likewise Software
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.  You should have received a copy of the GNU General
 * Public License along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * LIKEWISE SOFTWARE MAKES THIS SOFTWARE AVAILABLE UNDER OTHER LICENSING
 * TERMS AS WELL.  IF YOU HAVE ENTERED INTO A SEPARATE LICENSE AGREEMENT
 * WITH LIKEWISE SOFTWARE, THEN YOU MAY ELECT TO USE THE SOFTWARE UNDER THE
 * TERMS OF THAT SOFTWARE LICENSE AGREEMENT INSTEAD OF THE TERMS OF THE GNU
 * GENERAL PUBLIC LICENSE, NOTWITHSTANDING THE ABOVE NOTICE.  IF YOU
 * HAVE QUESTIONS, OR WISH TO REQUEST A COPY OF THE ALTERNATE LICENSING
 * TERMS OFFERED BY LIKEWISE SOFTWARE, PLEASE CONTACT LIKEWISE SOFTWARE AT
 * license@likewisesoftware.com
 */

#include "npfs.h"

static
NTSTATUS
NpfsQueryFilePipeLocalInfo(
    PNPFS_IRP_CONTEXT pIrpContext
    );

NTSTATUS
NpfsFilePipeLocalInfo(
    NPFS_INFO_TYPE Type,
    PNPFS_IRP_CONTEXT pIrpContext
    )
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    switch(Type)
    {
        case NPFS_SET:

            ntStatus = STATUS_NOT_SUPPORTED;

            break;

        case NPFS_QUERY:

            ntStatus = NpfsQueryFilePipeLocalInfo(pIrpContext);

            break;

        default:

            ntStatus = STATUS_INVALID_PARAMETER;

            break;
    }
    BAIL_ON_NT_STATUS(ntStatus);

cleanup:

    return ntStatus;

error:

    goto cleanup;
}

static
NTSTATUS
NpfsQueryFilePipeLocalInfo(
    PNPFS_IRP_CONTEXT pIrpContext
    )
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PIRP pIrp = pIrpContext->pIrp;
    PNPFS_CCB pCcb = NULL;
    PNPFS_PIPE pPipe = NULL;
    PNPFS_FCB pFCB = NULL;
    PFILE_PIPE_LOCAL_INFORMATION pPipeInfo = NULL;
    IRP_ARGS_QUERY_SET_INFORMATION Args = pIrpContext->pIrp->Args.QuerySetInformation;

    /* Sanity checks */

    ntStatus = NpfsGetCCB(pIrp->FileHandle, &pCcb);
    BAIL_ON_NT_STATUS(ntStatus);

    pPipe = pCcb->pPipe;
    pFCB = pPipe->pFCB;

    /* No access checked needed for this call */

    BAIL_ON_INVALID_PTR(Args.FileInformation, ntStatus);

    if (Args.Length < sizeof(*pPipeInfo))
    {
        ntStatus = STATUS_BUFFER_TOO_SMALL;
        BAIL_ON_NT_STATUS(ntStatus);
    }

    pPipeInfo = (PFILE_PIPE_LOCAL_INFORMATION)Args.FileInformation;

    pPipeInfo->CurrentInstances = pFCB->CurrentNumberOfInstances;
    pPipeInfo->InboundQuota = 0;
    pPipeInfo->MaximumInstances = pFCB->MaxNumberOfInstances;
    pPipeInfo->NamedPipeConfiguration = pFCB->NamedPipeConfiguration;
    pPipeInfo->NamedPipeEnd = (pCcb->CcbType == NPFS_CCB_SERVER)?
                                FILE_PIPE_SERVER_END: FILE_PIPE_CLIENT_END;
    pPipeInfo->NamedPipeState = 0;
    pPipeInfo->NamedPipeType = pFCB->NamedPipeType;
    pPipeInfo->OutboundQuota = 0;
    pPipeInfo->ReadDataAvailable = 0;
    pPipeInfo->WriteQuotaAvailable = 0;

    pIrp->IoStatusBlock.BytesTransferred = sizeof(*pPipeInfo);

cleanup:

    return ntStatus;

error:

    goto cleanup;
}
